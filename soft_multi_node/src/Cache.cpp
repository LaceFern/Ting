#include "Cache.h"
#include "DSM.h"
#include "RawMessageConnection.h"
#include "Hash.h"
Cache::Cache(const DSMConfig &conf, DSM *dsm) : conf(conf), dsm(dsm) {  
  printf("dsm->getMyNodeID() = %d\n", dsm->getMyNodeID());
}
Cache::~Cache() {
}
void Cache::initLine(SlabAllocator& cline_data_sb){
  int appt_idx = 0;
  for(int i = 0; i < conf.cline_num_per_node; i++){
    auto dataptr = cline_data_sb.sb_aligned_calloc(1, DSM_CACHE_LINE_SIZE);
    available_data_stack[appt_idx].push_back(dataptr);
    appt_idx = (appt_idx + 1) % conf.appt_num_per_node;
  }
  auto dataptr = available_data_stack[0].back();
  printf("appt[%d] dataptr = 0x%lx\n", dsm->getMyThreadID(), reinterpret_cast<uint64_t>(dataptr));
  uint32_t* p = reinterpret_cast<uint32_t*>(dataptr);
  printf("appt[%d] p[0] = %d\n", dsm->getMyThreadID(), p[0]);
  for(int i = 0; i < max_cline_set_depth_per_node; i++){
    for(int j = 0; j < max_cline_set_width_per_node; j++){
      auto& cline_info = cline_set_vector[i][j];
      cline_info.set_index = i;
      cline_info.slot_index = j;
      cline_info.shadow_flag = false;
      cline_info.setFlagAndStatus(false, CacheStatus::PRESERVED);
      cline_info.data = nullptr;
      auto& shadow_cline_info = shadow_cline_set_vector[i][j];
      shadow_cline_info.set_index = i;
      shadow_cline_info.slot_index = j;
      shadow_cline_info.shadow_flag = true;
      shadow_cline_info.setFlagAndStatus(false, CacheStatus::PRESERVED);
      shadow_cline_info.data = nullptr;
    }
  }
}
void Cache::readLine(const GlobalAddress &addr, uint16_t start, uint16_t size, void *to) {
  LineInfo *info = nullptr;
  LineInfo *shadow_info = nullptr;
  bool succ = false;
  int count = 0;
  int w_count = 0;
  Monitor monitor;
  monitor.start();
  while(!succ){
    start_evict();
    auto m_flag = monitor.check("", 5.0);
    auto set_idx = calLineSet(addr.getTag());
    auto& set_rwlock = cline_set_rwlock_vector[set_idx];
    set_rwlock.rLock();
    auto findLine_flag = findLine(set_idx, addr, info);
    auto findShadowLine_flag = findShadowLine(set_idx, addr, shadow_info);
    if(m_flag){
      printf("appt[%d] findLine_flag = %d, findShadowLine_flag = %d\n", dsm->getMyThreadID(), findLine_flag, findShadowLine_flag);
      if(info != nullptr) printf("appt[%d] readLine try timeout info->shadow_flag = %d, info->getStatus = %d, info->getFlag = %d\n", dsm->getMyThreadID(), info->shadow_flag, info->getStatus(), info->getFlag());
      else printf("appt[%d] readLine try timeout info = null\n", dsm->getMyThreadID());
      if(wait_cur_evict_cline_flag == true) printf("appt[%d] cur_evict_cline->tag = %d\n", dsm->getMyThreadID());
      else printf("appt[%d] cur_evict_cline = null\n", dsm->getMyThreadID());
      if(dsm->getMyThreadID() == 0){
        Debug::notifyInfo("appt[%d] --- set ---", dsm->getMyThreadID());
        print_cline(&cline_set_vector[0][0]);
        Debug::notifyInfo("appt[%d] --- shadow set ---", dsm->getMyThreadID());
        print_cline(&shadow_cline_set_vector[0][0]);
        agent_stats_inst.print_cc_stat();
      }
    } 
    bool dontWaitEvict_flag = false;
    if(!findLine_flag && !findShadowLine_flag){
      set_rwlock.rUnlock();
      set_rwlock.wLock();
      auto newLine_flag = newLine(set_idx, addr, info);
      set_rwlock.wUnlock();
      dontWaitEvict_flag = newLine_flag; 
    }
    else if(!findLine_flag && findShadowLine_flag){
      if(wait_cur_evict_cline_flag == true && cur_evict_cline->tag == addr.getTag()){
        set_rwlock.rUnlock();
      }
      else{
        set_rwlock.rUnlock();
        shadow_info->lock.rLock();
        if(shadow_info->getStatus() == CacheStatus::SHARED || shadow_info->getStatus() == CacheStatus::MODIFIED){
          agent_stats_inst.cc_stat.hit[dsm->getMyThreadID()]++;
          succ = true;
          memcpy(to, (char *)shadow_info->data + start, size);
          dontWaitEvict_flag = true;
        }
        shadow_info->lock.rUnlock();
      }
    }
    else if(findLine_flag && findShadowLine_flag){
        set_rwlock.rUnlock();
        Debug::notifyInfo("appt[%d] WARNING: readLine findLine_flag=1, findShadowLine_flag=1", dsm->getMyThreadID());
        Debug::notifyInfo("appt[%d] access tag = %d", dsm->getMyThreadID(), addr.getTag());
        Debug::notifyInfo("appt[%d] ----TEST----", dsm->getMyThreadID(), addr.getTag());
        for(int i = 0; i < max_cline_set_depth_per_node; i++){
          for(int j = 0; j < max_cline_set_width_per_node; j++){
            Debug::notifyInfo("appt[%d] [%d][%d] --- set ---", dsm->getMyThreadID(), i, j);
            print_cline(&cline_set_vector[i][j]);
          }
        }
        for(int i = 0; i < max_cline_set_depth_per_node; i++){
          for(int j = 0; j < max_cline_set_width_per_node; j++){
            Debug::notifyInfo("appt[%d] [%d][%d] --- shadow set ---", dsm->getMyThreadID(), i, j);
            print_cline(&shadow_cline_set_vector[i][j]);
          }
        }
        Debug::notifyInfo("\n\nappt[%d] ----RECORD----", dsm->getMyThreadID(), addr.getTag());
        for(int j = 0; j < max_cline_set_width_per_node; j++){
          Debug::notifyInfo("appt[%d] [%d][%d] --- set ---", dsm->getMyThreadID(), set_idx, j);
          printf("tag = %d, status = %d\n", debug_tag[j], debug_status[j]);
        }
        for(int j = 0; j < max_cline_set_width_per_node; j++){
          Debug::notifyInfo("appt[%d] [%d][%d] --- shadow set ---", dsm->getMyThreadID(), set_idx, j);
          printf("tag = %d, status = %d\n", debug_shadow_tag[j], debug_shadow_status[j]);
        }
        assert(false);
    }
    else{
      if(info->casFlagAndStatus(false, CacheStatus::INVALID, false, CacheStatus::TO_SHARED)){
        set_rwlock.rUnlock();
        succ = readMiss(addr, info);
        if(!succ){
          bool cas_flag = false;
          Monitor monitor_inner;
          monitor_inner.start();
          while(cas_flag == false){
            auto m_flag_inner = monitor_inner.check("", 5.0);
            if(m_flag_inner){
              printf("appt[%d] readLine fail timeout info->shadow_flag = %d, info->getStatus = %d, info->getFlag = %d\n", dsm->getMyThreadID(), info->shadow_flag, info->getStatus(), info->getFlag());
            }
            if(info->casFlagAndStatus(true, CacheStatus::TO_SHARED, false, CacheStatus::INVALID)){
              cas_flag = true;
            }
            else if(info->casFlagAndStatus(false, CacheStatus::TO_SHARED, false, CacheStatus::INVALID)){
              cas_flag = true;
            }
            else{
              cas_flag = false;
            }
          }
        }
        else{
          bool cas_flag = false;
          Monitor monitor_inner;
          monitor_inner.start();
          while(cas_flag == false){
            auto m_flag_inner = monitor_inner.check("", 5.0);
            if(m_flag_inner){
              printf("appt[%d] readLine succ timeout info->shadow_flag = %d, info->getStatus = %d, info->getFlag = %d\n", dsm->getMyThreadID(), info->shadow_flag, info->getStatus(), info->getFlag());
            }
            if(info->casFlagAndStatus(true, CacheStatus::TO_SHARED, false, CacheStatus::INVALID)){
              cas_flag = true;
              succ = false; 
            }
            else if(info->casFlagAndStatus(false, CacheStatus::TO_SHARED, false, CacheStatus::SHARED)){
              cas_flag = true;
              info->lock.rLock(); 
              if(info->getStatus() == CacheStatus::SHARED || info->getStatus() == CacheStatus::MODIFIED){
                memcpy(to, (char *)info->data + start, size);
              }
              else{
                succ = false;  
              }
              info->lock.rUnlock();
            }
            else{
              cas_flag = false;
            }
          }
        }
      }
      else{
        set_rwlock.rUnlock();
        info->lock.rLock(); 
        if(info->getStatus() == CacheStatus::SHARED || info->getStatus() == CacheStatus::MODIFIED){
          agent_stats_inst.cc_stat.hit[dsm->getMyThreadID()]++;
          succ = true;
          memcpy(to, (char *)info->data + start, size);
          dontWaitEvict_flag = true;
        }
        else{
          succ = false;  
        }
        info->lock.rUnlock();
      }
    }
    if(dontWaitEvict_flag == false) wait_evict();
  }
}
void Cache::writeLine(const GlobalAddress &addr, uint16_t start, uint16_t size, const void *from) {
  LineInfo *info = nullptr;
  LineInfo *shadow_info = nullptr;
  bool succ = false;
  int count = 0;  
  int w_count = 0; 
  Monitor monitor;
  monitor.start();  
  while(!succ){
    start_evict();
    auto m_flag = monitor.check("", 5.0);
    auto set_idx = calLineSet(addr.getTag());
    auto& set_rwlock = cline_set_rwlock_vector[set_idx];
    set_rwlock.rLock();
    auto findLine_flag = findLine(set_idx, addr, info);
    auto findShadowLine_flag = findShadowLine(set_idx, addr, shadow_info);
    if(m_flag){
      printf("appt[%d] findLine_flag = %d, findShadowLine_flag = %d\n", dsm->getMyThreadID(), findLine_flag, findShadowLine_flag);
      if(info != nullptr) printf("appt[%d] writeLine try timeout info->shadow_flag = %d, info->getStatus = %d, info->getFlag = %d\n", dsm->getMyThreadID(), info->shadow_flag, info->getStatus(), info->getFlag());
      else printf("appt[%d] writeLine try timeout info = null\n", dsm->getMyThreadID());
      if(wait_cur_evict_cline_flag == true) printf("appt[%d] cur_evict_cline->tag = %d\n", dsm->getMyThreadID());
      else printf("appt[%d] cur_evict_cline = null\n", dsm->getMyThreadID());
      if(dsm->getMyThreadID() == 0){
        Debug::notifyInfo("appt[%d] --- set ---", dsm->getMyThreadID());
        print_cline(&cline_set_vector[0][0]);
        Debug::notifyInfo("appt[%d] --- shadow set ---", dsm->getMyThreadID());
        print_cline(&shadow_cline_set_vector[0][0]);
        agent_stats_inst.print_cc_stat();
      }
    } 
    bool dontWaitEvict_flag = false;
    if(!findLine_flag && !findShadowLine_flag){
      set_rwlock.rUnlock();
      set_rwlock.wLock();
      auto newLine_flag = newLine(set_idx, addr, info);
      set_rwlock.wUnlock();
      dontWaitEvict_flag = newLine_flag; 
    }
    else if(!findLine_flag && findShadowLine_flag){
      if(wait_cur_evict_cline_flag == true && cur_evict_cline->tag == addr.getTag()){
        set_rwlock.rUnlock();
      }
      else{
        set_rwlock.rUnlock();
        shadow_info->lock.rLock();
        if(shadow_info->getStatus() == CacheStatus::MODIFIED){
          agent_stats_inst.cc_stat.hit[dsm->getMyThreadID()]++;
          succ = true;
          memcpy((char *)shadow_info->data + start, from, size);
          dontWaitEvict_flag = true;
        }
        shadow_info->lock.rUnlock();
      }
    }
    else if(findLine_flag && findShadowLine_flag){
        set_rwlock.rUnlock();
        Debug::notifyInfo("appt[%d] WARNING: writeLine findLine_flag=1, findShadowLine_flag=1", dsm->getMyThreadID());
        Debug::notifyInfo("appt[%d] access tag = %d", dsm->getMyThreadID(), addr.getTag());
        Debug::notifyInfo("appt[%d] ----TEST----", dsm->getMyThreadID(), addr.getTag());
        for(int i = 0; i < max_cline_set_depth_per_node; i++){
          for(int j = 0; j < max_cline_set_width_per_node; j++){
            Debug::notifyInfo("appt[%d] [%d][%d] --- set ---", dsm->getMyThreadID(), i, j);
            print_cline(&cline_set_vector[i][j]);
          }
        }
        for(int i = 0; i < max_cline_set_depth_per_node; i++){
          for(int j = 0; j < max_cline_set_width_per_node; j++){
            Debug::notifyInfo("appt[%d] [%d][%d] --- shadow set ---", dsm->getMyThreadID(), i, j);
            print_cline(&shadow_cline_set_vector[i][j]);
          }
        }
        Debug::notifyInfo("\n\nappt[%d] ----RECORD----", dsm->getMyThreadID(), addr.getTag());
        for(int j = 0; j < max_cline_set_width_per_node; j++){
          Debug::notifyInfo("appt[%d] [%d][%d] --- set ---", dsm->getMyThreadID(), set_idx, j);
          printf("tag = %d, status = %d\n", debug_tag[j], debug_status[j]);
        }
        for(int j = 0; j < max_cline_set_width_per_node; j++){
          Debug::notifyInfo("appt[%d] [%d][%d] --- shadow set ---", dsm->getMyThreadID(), set_idx, j);
          printf("tag = %d, status = %d\n", debug_shadow_tag[j], debug_shadow_status[j]);
        }
        assert(false);
    }
    else{
      if(info->casFlagAndStatus(false, CacheStatus::SHARED, false, CacheStatus::TO_MODIFIED)){
        set_rwlock.rUnlock();
        succ = writeShared(addr, info);
        if(!succ){
          bool cas_flag = false;
          Monitor monitor_inner;
          monitor_inner.start();
          while(cas_flag == false){
            auto m_flag_inner = monitor_inner.check("", 5.0);
            if(m_flag_inner){
              printf("appt[%d] writeLine(S2M) fail timeout info->shadow_flag = %d, info->getStatus = %d, info->getFlag = %d\n", dsm->getMyThreadID(), info->shadow_flag, info->getStatus(), info->getFlag());
            }
            if(info->casFlagAndStatus(true, CacheStatus::TO_MODIFIED, false, CacheStatus::INVALID)){
              cas_flag = true;
            }
            else if(info->casFlagAndStatus(false, CacheStatus::TO_MODIFIED, false, CacheStatus::SHARED)){
              cas_flag = true;
            }
            else{
              cas_flag = false;
            }
          }
        }
        else{
          bool cas_flag = false;
          Monitor monitor_inner;
          monitor_inner.start();
          while(cas_flag == false){
            auto m_flag_inner = monitor_inner.check("", 5.0);
            if(m_flag_inner){
              printf("appt[%d] writeLine(S2M) succ timeout info->shadow_flag = %d, info->getStatus = %d, info->getFlag = %d\n", dsm->getMyThreadID(), info->shadow_flag, info->getStatus(), info->getFlag());
            }
            if(info->casFlagAndStatus(true, CacheStatus::TO_MODIFIED, false, CacheStatus::INVALID)){
              cas_flag = true;
              succ = false; 
            }
            else if(info->casFlagAndStatus(false, CacheStatus::TO_MODIFIED, false, CacheStatus::MODIFIED)){
              cas_flag = true;
              info->lock.wLock();
              if(info->getStatus() == CacheStatus::MODIFIED){
                memcpy((char *)info->data + start, from, size);
              }
              else{
                succ = false;
              }
              info->lock.wUnlock();
            }
            else{
              cas_flag = false;
            }
          }
        }
      }
      else if(info->casFlagAndStatus(false, CacheStatus::INVALID, false, CacheStatus::TO_MODIFIED)){
        set_rwlock.rUnlock();
        succ = writeMiss(addr, info);
        if(!succ){
          bool cas_flag = false;
          Monitor monitor_inner;
          monitor_inner.start();
          while(cas_flag == false){
            auto m_flag_inner = monitor_inner.check("", 5.0);
            if(m_flag_inner){
              printf("appt[%d] writeLine(I2M) fail timeout info->shadow_flag = %d, info->getStatus = %d, info->getFlag = %d\n", dsm->getMyThreadID(), info->shadow_flag, info->getStatus(), info->getFlag());
            }
            if(info->casFlagAndStatus(true, CacheStatus::TO_MODIFIED, false, CacheStatus::INVALID)){
              cas_flag = true;
            }
            else if(info->casFlagAndStatus(false, CacheStatus::TO_MODIFIED, false, CacheStatus::INVALID)){
              cas_flag = true;
            }
            else{
              cas_flag = false;
            }
          }
        }
        else{
          bool cas_flag = false;
          Monitor monitor_inner;
          monitor_inner.start();
          while(cas_flag == false){
            auto m_flag_inner = monitor_inner.check("", 5.0);
            if(m_flag_inner){
              printf("appt[%d] writeLine(I2M) succ timeout info->shadow_flag = %d, info->getStatus = %d, info->getFlag = %d\n", dsm->getMyThreadID(), info->shadow_flag, info->getStatus(), info->getFlag());
            }
            if(info->casFlagAndStatus(true, CacheStatus::TO_MODIFIED, false, CacheStatus::INVALID)){
              cas_flag = true;
              succ = false; 
            }
            else if(info->casFlagAndStatus(false, CacheStatus::TO_MODIFIED, false, CacheStatus::MODIFIED)){
              cas_flag = true;
              info->lock.wLock();
              if(info->getStatus() == CacheStatus::MODIFIED){
                memcpy((char *)info->data + start, from, size);
              }
              else{
                succ = false;
              }
              info->lock.wUnlock();
            }
            else{
              cas_flag = false;
            }
          }
        }
      }
      else{
        set_rwlock.rUnlock();
        info->lock.wLock();
        if(info->getStatus() == CacheStatus::MODIFIED){
          agent_stats_inst.cc_stat.hit[dsm->getMyThreadID()]++;
          succ = true;
          memcpy((char *)info->data + start, from, size);
          dontWaitEvict_flag = true;
        }
        else{
          succ = false;
        }
        info->lock.wUnlock();
      }
    }
    if(dontWaitEvict_flag == false) wait_evict();
  }
}
uint32_t Cache::calLineSet(const uint64_t tag){
  uint32_t v = tag;
  return hash::jenkins(&v, sizeof(v)) % max_cline_set_depth_per_node;
}
bool Cache::findLine(uint32_t set_idx, const GlobalAddress &addr, LineInfo *&cline) {
  uint64_t tag = addr.getTag();
  auto& set = cline_set_vector[set_idx];
  bool exist = false;
  for(int i = 0; i < max_cline_set_width_per_node; i++){
    debug_status[i] = set[i].getStatus();
    debug_tag[i] = set[i].tag;
    if(debug_status[i] != CacheStatus::PRESERVED && debug_tag[i] == tag){
      cline = &set[i];
      exist = true;
      break;
    }
  }
  return exist;
}
bool Cache::findShadowLine(uint32_t set_idx, const GlobalAddress &addr, LineInfo *&cline) {
  uint64_t tag = addr.getTag();
  auto& shadow_set = shadow_cline_set_vector[set_idx];
  bool shadow_exist = false;
  for(int i = 0; i < max_cline_set_width_per_node; i++){
    debug_shadow_status[i] = shadow_set[i].getStatus();
    debug_shadow_tag[i] = shadow_set[i].tag;
    if(debug_shadow_status[i] != CacheStatus::PRESERVED && debug_shadow_tag[i] == tag){ 
      cline = &shadow_set[i];
      shadow_exist = true;
      break;
    }
  }
  return shadow_exist;
}
bool Cache::newLine(uint32_t set_idx, const GlobalAddress &addr, LineInfo *&cline) {
  uint64_t tag = addr.getTag();
  auto& set = cline_set_vector[set_idx];
  auto& shadow_set = shadow_cline_set_vector[set_idx];
  auto& data_stack = available_data_stack[req_appt_id];
  bool new_flag = false;
  int exist_slot_idx = -1;
  int available_slot_idx = -1;
  int eviction_slot_idx = -1;
  for(int i = 0; i < max_cline_set_width_per_node; i++){
    if((set[i].getStatus() != CacheStatus::PRESERVED && set[i].tag == tag)
      || (shadow_set[i].getStatus() != CacheStatus::PRESERVED && shadow_set[i].tag == tag)){
      exist_slot_idx = i;
    }
    else if(set[i].getStatus() == CacheStatus::PRESERVED){
      available_slot_idx = i;
    }
    else if((set[i].getStatus() == CacheStatus::INVALID 
      || set[i].getStatus() == CacheStatus::SHARED
      || set[i].getStatus() == CacheStatus::MODIFIED)
      && shadow_set[i].getStatus() == CacheStatus::PRESERVED){
      eviction_slot_idx = i;
    }
  }
  if(exist_slot_idx != -1){
    new_flag = false; 
  }
  else if(available_slot_idx == -1 && eviction_slot_idx == -1){
    new_flag = false; 
  }
  else if(available_slot_idx != -1 && data_stack.size() >= 1){ 
    set[available_slot_idx].tag = addr.getTag();
    set[available_slot_idx].addr = addr;
    set[available_slot_idx].data = data_stack.back();
    set[available_slot_idx].setFlagAndStatus(false, CacheStatus::INVALID);
    data_stack.pop_back();
    cline = &set[available_slot_idx];
    new_flag = true;
  }
  else if(eviction_slot_idx != -1){
    if(set[eviction_slot_idx].getStatus() == CacheStatus::INVALID){ 
      set[eviction_slot_idx].tag = addr.getTag();
      set[eviction_slot_idx].addr = addr;
      cline = &set[eviction_slot_idx];
      agent_stats_inst.cc_stat.evict_invalid[dsm->getMyThreadID()]++;
    }
    else{
      shadow_set[eviction_slot_idx].tag = set[eviction_slot_idx].tag;
      shadow_set[eviction_slot_idx].addr = set[eviction_slot_idx].addr;
      shadow_set[eviction_slot_idx].data = set[eviction_slot_idx].data;
      shadow_set[eviction_slot_idx].setFlagAndStatus(false, set[eviction_slot_idx].getStatus());
      start_next_evict_cline_flag = true;
      next_evict_cline = &shadow_set[eviction_slot_idx]; 
      set[eviction_slot_idx].tag = addr.getTag();
      set[eviction_slot_idx].addr = addr;
      set[eviction_slot_idx].data = data_stack.back();
      set[eviction_slot_idx].setFlagAndStatus(false, CacheStatus::INVALID);
      data_stack.pop_back();
      cline = &set[eviction_slot_idx];
      if(next_evict_cline->getStatus() == CacheStatus::SHARED) agent_stats_inst.cc_stat.evict_shared[dsm->getMyThreadID()]++;
      else agent_stats_inst.cc_stat.evict_modified[dsm->getMyThreadID()]++;
    }
    new_flag = true;
  }
  return new_flag;
};
bool Cache::findLineForAgent(uint64_t tag, LineInfo *&line, int agent_id) {
  uint64_t set_idx = calLineSet(tag); 
  auto& set = cline_set_vector[set_idx];
  auto& shadow_set = shadow_cline_set_vector[set_idx];
  Monitor monitor;
  monitor.start();
  while(1){
    monitor.check("agent findline timeout. agent " + std::to_string(agent_id), 5.0);
    bool exist = false;
    for(int i = 0; i < max_cline_set_width_per_node; i++){
      if(set[i].getStatus() != CacheStatus::PRESERVED && set[i].tag == tag){
        line = &set[i];
        exist = true;
        break;
      }
    }
    bool shadow_exist = false;
    for(int i = 0; i < max_cline_set_width_per_node; i++){
      if(shadow_set[i].getStatus() != CacheStatus::PRESERVED && shadow_set[i].tag == tag){
        line = &shadow_set[i];
        shadow_exist = true;
        break;
      }
    }
    if(exist && shadow_exist){
      Debug::notifyInfo("agent[%d] WARNING: findLine_flag=1, findShadowLine_flag=1", agent_id);
      assert(false);
    }
    else if (exist || shadow_exist) return true;
    else return false;
  }
}
RawMessage Cache::generate_message(
  RawMessageType mtype,
  const GlobalAddress &addr, 
  LineInfo *info) {
  RawMessage m_send;
  m_send.req_cline_nodeID = dsm->getMyNodeID();
  m_send.req_cline_appThreadID = dsm->getMyThreadID();
  m_send.req_cline_addr = (uint64_t)info->data;
  m_send.req_cline_index = info->line_index;
  m_send.cline_tag = addr.getTag();
  m_send.dir_gblock_index = addr.getIndex();
  m_send.dir_gblock_nodeID = addr.nodeID;
  m_send.mtype = mtype;
  return m_send;
}
void Cache::start_evict(){
  if(start_next_evict_cline_flag == true){
    start_next_evict_cline_flag = false;
    wait_cur_evict_cline_flag = true;
    cur_evict_cline = next_evict_cline;
    next_evict_cline = nullptr;
    bool cas_flag = false;
    Monitor monitor;
    monitor.start();
    while(cas_flag == false){
      monitor.check("start_evict timeout. appt " + std::to_string(req_appt_id), 5.0);
      CacheStatus tmp_status = cur_evict_cline->getStatus();
      if(tmp_status == CacheStatus::INVALID){
        cas_flag = true;
        wait_cur_evict_cline_flag = false;
        auto& set_index = cur_evict_cline->set_index;
        auto& slot_index = cur_evict_cline->slot_index;
        auto& set_rwlock = cline_set_rwlock_vector[set_index];
        auto& data_stack = available_data_stack[req_appt_id];
        auto& cline = shadow_cline_set_vector[set_index][slot_index];
        data_stack.push_back(cline.data);
        cline.data = nullptr;
        cline.setFlagAndStatus(false, CacheStatus::PRESERVED);
      }
      else{
        if(cur_evict_cline->casStatus(tmp_status, CacheStatus::TO_PRESERVED)){
          cur_evict_cline->lock.wLock();
          cas_flag = true;
          RawMessageType mtype;
          if(tmp_status == CacheStatus::SHARED){
            cur_evict_cline_type = EvictType::EVICT_SHARED;
            mtype = RawMessageType::R_EVICT_SHARED;
          }
          else{
            cur_evict_cline_type = EvictType::EVICT_DIRTY;
            mtype = RawMessageType::R_EVICT_DIRTY;
          }
          RawMessage m_send = generate_message(mtype, cur_evict_cline->addr, cur_evict_cline);
          channel->send(m_send);
          last_sendmsg = m_send;
        }
      }
    }
  }
}
void Cache::wait_evict(){
  if(wait_cur_evict_cline_flag == true){
    auto& set_index = cur_evict_cline->set_index;
    auto& slot_index = cur_evict_cline->slot_index;
    auto& set_rwlock = cline_set_rwlock_vector[set_index];
    auto& data_stack = available_data_stack[req_appt_id];
    auto& cline = shadow_cline_set_vector[set_index][slot_index];
    bool end_flag = false;
    while(end_flag == false){
      RawMessage m_recv; 
      if(recv_cur_evict_msg_flag == true){
        recv_cur_evict_msg_flag = false;
        m_recv = cur_evict_msg;
      }
      else m_recv = channel->recv_msg();
      if(m_recv.cline_tag == cur_evict_cline->tag){
        switch(m_recv.mtype){
          case RawMessageType::DIR_2_APP_EVICT_DIRTY_ACK:
          case RawMessageType::DIR_2_APP_EVICT_SHARED_ACK:{
            data_stack.push_back(cline.data);
            cline.data = nullptr;
            cline.setFlagAndStatus(false, CacheStatus::PRESERVED);
            end_flag = true;
            break;
          }
          case RawMessageType::DIR_2_APP_M_CHECK_FAIL:{
            if(cur_evict_cline_type == EvictType::EVICT_SHARED){
              bool succ = false;
              while(succ == false){
                succ = cur_evict_cline->casFlagAndStatus(false, CacheStatus::TO_PRESERVED, false, CacheStatus::SHARED);
                if(succ == false){
                  succ = cur_evict_cline->casFlagAndStatus(true, CacheStatus::TO_PRESERVED, false, CacheStatus::INVALID);
                }
              }
            }
            else{
              bool succ = false;
              while(succ == false){
                succ = cur_evict_cline->casFlagAndStatus(false, CacheStatus::TO_PRESERVED, false, CacheStatus::MODIFIED);
                if(succ == false){
                  succ = cur_evict_cline->casFlagAndStatus(true, CacheStatus::TO_PRESERVED, false, CacheStatus::INVALID);
                }
                if(succ == false){
                  succ = cur_evict_cline->casFlagAndStatus(false, CacheStatus::TO_SHARED, false, CacheStatus::SHARED);
                }
                if(succ == false){
                  succ = cur_evict_cline->casFlagAndStatus(true, CacheStatus::TO_SHARED, false, CacheStatus::INVALID);
                }
              }
            }
            start_next_evict_cline_flag = true;
            next_evict_cline = cur_evict_cline;
            start_evict(); 
            if(wait_cur_evict_cline_flag == false) end_flag = true;
            break;
          }
          case RawMessageType::DIR_2_APP_BITMAP_CHECK_FAIL:{
            Monitor monitor_inner;
            monitor_inner.start();
            auto flag = false;
            while(!flag){
              flag = cur_evict_cline->getFlag();
              auto m_flag_inner = monitor_inner.check("", 5.0);
              if(m_flag_inner){
                printf("appt[%d] wait_evict DIR_2_APP_BITMAP_CHECK_FAIL\n", dsm->getMyThreadID());
                printf("appt[%d] req to dir:\n", dsm->getMyThreadID());
                agent_stats_inst.print_msg(last_sendmsg); 
                printf("appt[%d] to req:\n", dsm->getMyThreadID());
                agent_stats_inst.print_msg(m_recv);
                printf("appt[%d] info->getStatus = %d, info->getFlag = %d\n", dsm->getMyThreadID(), cur_evict_cline->getStatus(), cur_evict_cline->getFlag());
                agent_stats_inst.print_cc_stat();
              }
            } 
            data_stack.push_back(cline.data);
            cline.data = nullptr;
            cline.setFlagAndStatus(false, CacheStatus::PRESERVED);
            end_flag = true;
            break;
          }
          default:{
            printf("appt[%d] to req:\n", dsm->getMyThreadID());
            agent_stats_inst.print_msg(m_recv);  
            printf("last_cnodebitmap:\n");
            for(int i = 0; i < max_node_num; i++){
              printf("%d\t", last_cnodebitmap[i]);
            }    
            printf("\n");
            printf("last_recvmsg:\n");
            agent_stats_inst.print_msg(last_one_recvmsg);
            assert(false);
            break;
          }
        }
      }
      else{
        printf("appt[%d] to req:\n", dsm->getMyThreadID());
        agent_stats_inst.print_msg(m_recv);   
        assert(false); 
      }
      last_one_recvmsg = m_recv;
    }
    cur_evict_cline->lock.wUnlock();
    wait_cur_evict_cline_flag = false;
  }
}
bool Cache::readMiss(const GlobalAddress &addr, LineInfo *info) {
  RawMessage m_send = generate_message(RawMessageType::R_READ_MISS, addr, info);
  channel->send(m_send);
  agent_stats_inst.stop_record_app_thread_with_op(addr.addr, APP_THREAD_OP::AFTER_PROCESS_LOCAL_REQUEST_READ);
  agent_stats_inst.start_record_app_thread(addr.addr);
  Monitor monitor;
  monitor.start();
  while(true){
    auto m_flag = monitor.check("", 5.0);
    if(m_flag) printf("appt[%d] readMiss recv timeout\n", dsm->getMyThreadID());
    RawMessage m_recv = channel->recv_msg();
    agent_stats_inst.stop_record_app_thread_with_op(addr.addr, APP_THREAD_OP::WAIT_ASYNC_FINISH);
    agent_stats_inst.start_record_app_thread(addr.addr);
    if(m_recv.cline_tag == m_send.cline_tag){
      switch(m_recv.mtype){
        case RawMessageType::AGENT_2_APP_READ_MISS_DIRTY_ACKDATA:
        case RawMessageType::DIR_2_APP_READ_MISS_ACKDATA:{
          if(m_recv.mtype == RawMessageType::AGENT_2_APP_READ_MISS_DIRTY_ACKDATA) agent_stats_inst.cc_stat.read_miss_dirty[dsm->getMyThreadID()]++;
          else if(m_recv.mtype == RawMessageType::DIR_2_APP_READ_MISS_ACKDATA) agent_stats_inst.cc_stat.read_miss_unshared_or_shared[dsm->getMyThreadID()]++;
          Monitor::wait(2);
          return true;
          break;
        }
        case RawMessageType::DIR_2_APP_M_CHECK_FAIL:{ 
          agent_stats_inst.cc_stat.read_miss_check_fail[dsm->getMyThreadID()]++;
          return false; 
          break;
        }
        case RawMessageType::DIR_2_APP_BITMAP_CHECK_FAIL:{
          Monitor monitor_inner;
          monitor_inner.start();
          while(info->getFlag() == false){
            auto m_flag_inner = monitor_inner.check("", 5.0);
            if(m_flag_inner){
              printf("appt[%d] readMiss DIR_2_APP_BITMAP_CHECK_FAIL\n", dsm->getMyThreadID());
            }
          } 
          return false;
          break;
        }
        default:{
          printf("readMiss, txncount = %d\n", txn_count);
          printf("appt[%d] to req:\n", dsm->getMyThreadID());
          agent_stats_inst.print_msg(m_recv);  
          printf("last_cnodebitmap:\n");
          for(int i = 0; i < max_node_num; i++){
            printf("%d\t", last_cnodebitmap[i]);
          }    
          printf("\n");
          printf("last_recvmsg:\n");
          agent_stats_inst.print_msg(last_one_recvmsg);
          dsm->resource->debug_print();
          int false_count = 1;
          while(true){
            RawMessage m_recv = channel->recv_msg();
            if(m_recv.mtype == 0) false_count ++;
            printf("appt[%d] readMiss default count = %d\n", dsm->getMyThreadID(), false_count);
          }
          assert(false);
          return false;
          break;
        }
      }
    }
    else if(wait_cur_evict_cline_flag && m_recv.cline_tag == cur_evict_cline->tag){
      recv_cur_evict_msg_flag = true;
      cur_evict_msg = m_recv;
    }
    else{
      printf("appt[%d] readMiss false req to dir:\n", dsm->getMyThreadID());
      agent_stats_inst.print_msg(m_send);
      printf("appt[%d] readMiss false to req:\n", dsm->getMyThreadID());
      agent_stats_inst.print_msg(m_recv);  
      dsm->resource->debug_print();
      int false_count = 1;
      while(true){
        RawMessage m_recv = channel->recv_msg();
        if(m_recv.mtype == 0) false_count ++;
        printf("appt[%d] readMiss false count = %d\n", dsm->getMyThreadID(), false_count);
      }
      assert(false);  
    }
    last_one_recvmsg = m_recv;
  }
}
bool Cache::writeMiss(const GlobalAddress &addr, LineInfo *info) {
  RawMessage m_send = generate_message(RawMessageType::R_WRITE_MISS, addr, info);
  channel->send(m_send); 
  agent_stats_inst.stop_record_app_thread_with_op(addr.addr, APP_THREAD_OP::AFTER_PROCESS_LOCAL_REQUEST_WRITE);
  agent_stats_inst.start_record_app_thread(addr.addr);
  bool bitmap[max_node_num] = {0};
  bool first = true;
  int copies = -1; 
  bool hasData = false;
  Monitor monitor;
  monitor.start();
  while(true){
    auto m_flag = monitor.check("", 5.0);
    if(m_flag) printf("appt[%d] writeMiss recv timeout\n", dsm->getMyThreadID());
    RawMessage m_recv = channel->recv_msg();
    agent_stats_inst.stop_record_app_thread_with_op(addr.addr, APP_THREAD_OP::WAIT_ASYNC_FINISH);
    agent_stats_inst.start_record_app_thread(addr.addr);
    if(m_recv.cline_tag == m_send.cline_tag){
      switch(m_recv.mtype){
        case RawMessageType::AGENT_2_APP_WRITE_MISS_DIRTY_ACKDATA:
        case RawMessageType::DIR_2_APP_WRITE_MISS_ACKDATA:{
          if(m_recv.mtype == RawMessageType::AGENT_2_APP_WRITE_MISS_DIRTY_ACKDATA) agent_stats_inst.cc_stat.write_miss_dirty[dsm->getMyThreadID()]++;
          else if(m_recv.mtype == RawMessageType::DIR_2_APP_WRITE_MISS_ACKDATA) agent_stats_inst.cc_stat.write_miss_unshared[dsm->getMyThreadID()]++;
          Monitor::wait(2);
          return true;
          break;
        }
        case RawMessageType::DIR_2_APP_WRITE_MISS_SHAREDDATA:{
          Monitor::wait(2);
          assert(!hasData);
          hasData = true;
          if(hasData && copies == 0){
            return true;
          }
          break;
        }
        case RawMessageType::DIR_2_APP_WRITE_MISS_SHARED_AGGACK_ACK:{
          std::copy(m_recv.dir_gblock_bitmap, m_recv.dir_gblock_bitmap + max_node_num, bitmap);
          copies = bits_in(bitmap);
          agent_stats_inst.cc_stat.write_miss_shared_copies[dsm->getMyThreadID()] += copies;
          agent_stats_inst.cc_stat.write_miss_shared[dsm->getMyThreadID()]++;
          copies = 0;
          if(hasData && copies == 0){
            return true;
          }
          break;
        }
        case RawMessageType::DIR_2_APP_M_CHECK_FAIL:{
          agent_stats_inst.cc_stat.write_miss_check_fail[dsm->getMyThreadID()]++;
          return false; 
          break;
        }
        case RawMessageType::DIR_2_APP_BITMAP_CHECK_FAIL:{
          Monitor monitor_inner;
          monitor_inner.start();
          while(info->getFlag() == false){
            auto m_flag_inner = monitor_inner.check("", 5.0);
            if(m_flag_inner) printf("appt[%d] writeMiss DIR_2_APP_BITMAP_CHECK_FAIL\n", dsm->getMyThreadID());
          } 
          return false;
          break;
        }
        default:{
          printf("writeMiss, txncount = %d\n", txn_count);
          printf("appt[%d] to req:\n", dsm->getMyThreadID());
          agent_stats_inst.print_msg(m_recv);    
          printf("last_cnodebitmap:\n");
          for(int i = 0; i < max_node_num; i++){
            printf("%d\t", last_cnodebitmap[i]);
          }    
          printf("\n");
          printf("last_recvmsg:\n");
          agent_stats_inst.print_msg(last_one_recvmsg);
          dsm->resource->debug_print();
          int false_count = 1;
          while(true){
            RawMessage m_recv = channel->recv_msg();
            if(m_recv.mtype == 0) false_count ++;
            printf("appt[%d] writeMiss default count = %d\n", dsm->getMyThreadID(), false_count);
          }
          assert(false);
          return false;
          break;
        }
      }
    }
    else if(wait_cur_evict_cline_flag && m_recv.cline_tag == cur_evict_cline->tag){
      recv_cur_evict_msg_flag = true;
      cur_evict_msg = m_recv;
    }
    else{
      printf("appt[%d] writeMiss false req to dir:\n", dsm->getMyThreadID());
      agent_stats_inst.print_msg(m_send);
      printf("appt[%d] writeMiss false to req:\n", dsm->getMyThreadID());
      agent_stats_inst.print_msg(m_recv);   
      dsm->resource->debug_print();
      int false_count = 1;
      while(true){
        RawMessage m_recv = channel->recv_msg();
        if(m_recv.mtype == 0) false_count ++;
        printf("appt[%d] writeMiss false count = %d\n", dsm->getMyThreadID(), false_count);
      }
      assert(false); 
    }
    last_one_recvmsg = m_recv;
  }
}
bool Cache::writeShared(const GlobalAddress &addr, LineInfo *info) {
  RawMessage m_send = generate_message(RawMessageType::R_WRITE_SHARED, addr, info);
  channel->send(m_send);
  last_sendmsg = m_send;
  agent_stats_inst.stop_record_app_thread_with_op(addr.addr, APP_THREAD_OP::AFTER_PROCESS_LOCAL_REQUEST_WRITE);
  agent_stats_inst.start_record_app_thread(addr.addr);
  bool bitmap[max_node_num] = {0};
  bool first = true;
  int copies = -1;
  Monitor monitor;
  monitor.start();
  while(true){
    auto m_flag = monitor.check("", 5.0);
    if(m_flag) printf("appt[%d] writeShared recv timeout\n", dsm->getMyThreadID());
    RawMessage m_recv = channel->recv_msg();
    agent_stats_inst.stop_record_app_thread_with_op(addr.addr, APP_THREAD_OP::WAIT_ASYNC_FINISH);
    agent_stats_inst.start_record_app_thread(addr.addr);
    if(m_recv.cline_tag == m_send.cline_tag){
      switch(m_recv.mtype){
        case RawMessageType::DIR_2_APP_WRITE_SHARED_NOTONLY_AGGACK_ACK:{          
          ws_multi_count++;
          std::copy(m_recv.dir_gblock_bitmap, m_recv.dir_gblock_bitmap + max_node_num, bitmap);
          copies = bits_in(bitmap);
          std::copy(m_recv.dir_gblock_bitmap, m_recv.dir_gblock_bitmap + max_node_num, last_cnodebitmap); 
          agent_stats_inst.cc_stat.write_shared[dsm->getMyThreadID()]++;
          agent_stats_inst.cc_stat.write_shared_copies[dsm->getMyThreadID()] += copies - 1;
          copies = 1;
          if(copies == 1){
            return true;
          }
          break;
        }
        case RawMessageType::DIR_2_APP_WRITE_SHARED_ONLYACK:{
          agent_stats_inst.cc_stat.write_shared[dsm->getMyThreadID()]++;
          ws_only_count++;
          return true;
          break;
        } 
        case RawMessageType::DIR_2_APP_M_CHECK_FAIL:{
          agent_stats_inst.cc_stat.write_shared_check_fail[dsm->getMyThreadID()]++;
          return false; 
          break;
        }
        case RawMessageType::DIR_2_APP_BITMAP_CHECK_FAIL:{
            Monitor monitor_inner;
            monitor_inner.start();
            while(info->getFlag() == false){
              auto m_flag_inner = monitor_inner.check("", 5.0);
              if(m_flag_inner){
                printf("appt[%d] writeShared DIR_2_APP_BITMAP_CHECK_FAIL\n", dsm->getMyThreadID());
              }
            } 
          return false;
          break;
        }
        default:{
          printf("writeShared, txncount = %d\n", txn_count);
          printf("appt[%d] to req:\n", dsm->getMyThreadID());
          agent_stats_inst.print_msg(m_recv);  
          printf("last_cnodebitmap:\n");
          for(int i = 0; i < max_node_num; i++){
            printf("%d\t", last_cnodebitmap[i]);
          }    
          printf("\n");
          printf("last_recvmsg:\n");
          agent_stats_inst.print_msg(last_one_recvmsg);
          dsm->resource->debug_print();
          int false_count = 1;
          while(true){
            RawMessage m_recv = channel->recv_msg();
            if(m_recv.mtype == 0) false_count ++;
            printf("appt[%d] writeShared default count = %d\n", dsm->getMyThreadID(), false_count);
          }
          assert(false);
          return false;
          break;
        }
      }
    }
    else if(wait_cur_evict_cline_flag && m_recv.cline_tag == cur_evict_cline->tag){
      recv_cur_evict_msg_flag = true;
      cur_evict_msg = m_recv;
    }
    else{
      printf("writeShared: m_recv.cline_tag != m_send.cline_tag, txn_count = %d\n", txn_count);
      printf("appt[%d] req to dir:\n", dsm->getMyThreadID());
      agent_stats_inst.print_msg(m_send);
      printf("appt[%d] to req:\n", dsm->getMyThreadID());
      agent_stats_inst.print_msg(m_recv); 
      printf("appt[%d] last recv msg:\n", dsm->getMyThreadID());
      agent_stats_inst.print_msg(last_one_recvmsg); 
      printf("\n");
      dsm->resource->debug_print();
      int false_count = 0;
      while(true){
        RawMessage m_recv = channel->recv_msg();
        if(m_recv.mtype == 0) false_count ++;
        printf("appt[%d] writeShared false count = %d\n", dsm->getMyThreadID(), false_count);
      }
      assert(false);
    }
    last_one_recvmsg = m_recv;
  }
}
thread_local bool Cache::start_next_evict_cline_flag = false;
thread_local bool Cache::wait_cur_evict_cline_flag = false;
thread_local LineInfo * Cache::cur_evict_cline = nullptr;
thread_local LineInfo * Cache::next_evict_cline = nullptr;
thread_local bool Cache::recv_cur_evict_msg_flag = false;
thread_local RawMessage Cache::cur_evict_msg;
thread_local EvictType Cache::cur_evict_cline_type;
thread_local int Cache::req_appt_id = -1;
thread_local channel_context * Cache::channel = nullptr;
thread_local int Cache::txn_count = 0;
thread_local int Cache::ws_only_count = 0;
thread_local int Cache::ws_multi_count = 0;
thread_local RawMessage Cache::last_sendmsg;
thread_local bool Cache::last_cnodebitmap[max_node_num] = {0};
thread_local bool Cache::last_recvbitmap[max_node_num] = {0};
thread_local RawMessage Cache::last_recvmsg[max_node_num];
thread_local RawMessage Cache::last_one_recvmsg;
thread_local CacheStatus Cache::debug_status[max_cline_set_width_per_node];
thread_local CacheStatus Cache::debug_shadow_status[max_cline_set_width_per_node];
thread_local uint64_t Cache::debug_tag[max_cline_set_width_per_node];
thread_local uint64_t Cache::debug_shadow_tag[max_cline_set_width_per_node];
thread_local GlobalAddress Cache::trigger_evict_addr;
thread_local uint32_t Cache::seed;
thread_local uint32_t Cache::next_allocator_node = 0;
thread_local LocalAllocator *Cache::alloc = nullptr;
GlobalAddress Cache::malloc(size_t size, bool align) {
  bool need_new_chunck = false;
  auto res = alloc->malloc(size, need_new_chunck, align);
  assert(size <= define::kChunkSize);
  if (need_new_chunck) {
    RawMessage m_send;
    m_send.req_cline_nodeID = dsm->getMyNodeID();
    m_send.req_cline_appThreadID = dsm->getMyThreadID();
    m_send.req_cline_addr = 0xff;
    m_send.req_cline_index = 0xff;
    m_send.cline_tag = 0xff;
    m_send.dir_gblock_index = 0xff;
    m_send.dir_gblock_nodeID = next_allocator_node;
    m_send.dir_gblock_addr = 0xff; 
    m_send.mtype = RawMessageType::PRIMITIVE_ALLOC;
    channel->send(m_send);
    RawMessage m_recv = channel->recv_msg();
    if(m_recv.mtype == RawMessageType::PRIMITIVE_ALLOC_FAIL){
      printf("appt[%d] Malloc failure! Should increase gmem_size_per_node.\n", dsm->getMyThreadID());
      printf("appt[%d] to req:\n", dsm->getMyThreadID());
      agent_stats_inst.print_msg(m_recv);
      agent_stats_inst.print_cc_stat();
      while(1){;}
    }
    assert(m_recv.mtype == RawMessageType::PRIMITIVE_ALLOC_ACK);
    GlobalAddress new_chunck;
    new_chunck.nodeID = next_allocator_node;
    new_chunck.addr = m_recv.dir_gblock_addr;
    alloc->set_chunck(new_chunck);
    res = alloc->malloc(size, need_new_chunck, align);
    assert(need_new_chunck == false);
    next_allocator_node = (next_allocator_node + 1) % dsm->conf.node_num;
  }
  return res;
}
void Cache::free(const GlobalAddress &addr) { alloc->free(addr); }