#include "CacheAgent.h"
#include "Cache.h"
#include "Common.h"
#include "agent_stat.h"
CacheAgent::CacheAgent(int agent_id, Cache *cache, channel_context *channel)
    : agent_id(agent_id), cache(cache), agent(nullptr), channel(channel) {
  agent = new std::thread(&CacheAgent::agentThread, this);
}
CacheAgent::~CacheAgent(){
  delete agent;
}
void CacheAgent::agentThread() {
  while (!stopFlag) {
    RawMessage m;
    auto m_ptr = channel->recv(stopFlag);
    if(m_ptr){
      m = *m_ptr;
      if(m.mtype == RawMessageType::PRIMITIVE_ALLOC){
        auto [chunck, succ] = cache->dsm->chunckAlloc[agent_id]->alloc_chunck();
        if(succ){
          m.mtype = RawMessageType::PRIMITIVE_ALLOC_ACK;
          m.dir_gblock_addr = chunck.addr; 
          m.dir_gblock_index = chunck.getIndex();
        }
        else{
          m.mtype = RawMessageType::PRIMITIVE_ALLOC_FAIL;
        }
        cache->dsm->resource->system_channel->send_with_lock(m);
        continue;
      }
      auto set_idx = cache->calLineSet(m.cline_tag);
      auto& set_rwlock = cache->cline_set_rwlock_vector[set_idx];
      set_rwlock.rLock();
      LineInfo *line;
      auto res = cache->findLineForAgent(m.cline_tag, line, agent_id);
      if(!res){
        Debug::notifyInfo("to agent: cannot find cache line for m.mtype = %d, m.cline_tag = %lu, m.agent_cline_nodeID=%d, m.req_cline_nodeID=%d", m.mtype, m.cline_tag, m.agent_cline_nodeID, m.req_cline_nodeID);
      }
      else{
        switch(m.mtype){
          case RawMessageType::DIR_2_AGENT_WRITE_SHARED_NOTONLY:{
            Monitor monitor;
            monitor.start();
            bool succ = false;
            while(succ == false){
              CacheStatus tmp_status = line->getStatus();
              monitor.check("DIR_2_AGENT_WRITE_SHARED_NOTONLY timeout. agent " + std::to_string(agent_id) + " tag=" + std::to_string(line->tag) + " status=" + std::to_string(tmp_status) + " shadow_flag=" + std::to_string(line->shadow_flag), 5.0);
              if(tmp_status == CacheStatus::SHARED){
                succ = line->casFlagAndStatus(false, CacheStatus::SHARED, false, CacheStatus::INVALID);
              }
              else if(tmp_status == CacheStatus::TO_MODIFIED || tmp_status == CacheStatus::TO_SHARED || tmp_status == CacheStatus::TO_PRESERVED){
                  succ = line->casFlagAndStatus(false, tmp_status, true, tmp_status);
              }
              else if(tmp_status == CacheStatus::PRESERVED) succ = true;
            }
            break;
          }
          case RawMessageType::DIR_2_AGENT_WRITE_MISS_SHARED:{
            Monitor monitor;
            monitor.start();
            bool succ = false;
            while(succ == false){
              CacheStatus tmp_status = line->getStatus();
              monitor.check("DIR_2_AGENT_WRITE_MISS_SHARED timeout. agent " + std::to_string(agent_id) + " tag=" + std::to_string(line->tag) + " status=" + std::to_string(tmp_status) + " shadow_flag=" + std::to_string(line->shadow_flag), 5.0);
              if(tmp_status == CacheStatus::SHARED){
                succ = line->casFlagAndStatus(false, CacheStatus::SHARED, false, CacheStatus::INVALID);
              }
              else if(tmp_status == CacheStatus::TO_MODIFIED || tmp_status == CacheStatus::TO_SHARED || tmp_status == CacheStatus::TO_PRESERVED){
                  succ = line->casFlagAndStatus(false, tmp_status, true, tmp_status);
              }
              else if(tmp_status == CacheStatus::PRESERVED) succ = true;
            }
            break;
          }
          case RawMessageType::DIR_2_AGENT_READ_MISS_DIRTY:{
            Monitor monitor;
            monitor.start();
            bool succ = false;
            while(succ == false){
              CacheStatus tmp_status = line->getStatus();
              monitor.check("DIR_2_AGENT_READ_MISS_DIRTY timeout. agent " + std::to_string(agent_id) + " tag=" + std::to_string(line->tag) + " status=" + std::to_string(tmp_status) + " shadow_flag=" + std::to_string(line->shadow_flag), 5.0);
              if(tmp_status == CacheStatus::MODIFIED){
                succ = line->casFlagAndStatus(false, CacheStatus::MODIFIED, false, CacheStatus::SHARED); 
                if(succ){
                }
              }
              else if(tmp_status == CacheStatus::TO_PRESERVED){
                succ = line->casFlagAndStatus(false, CacheStatus::TO_PRESERVED, false, CacheStatus::TO_SHARED); 
              }
              else if(tmp_status == CacheStatus::PRESERVED) succ = true;
            }
            break;
          }
          case RawMessageType::DIR_2_AGENT_WRITE_MISS_DIRTY:{
            Monitor monitor;
            monitor.start();
            bool succ = false;
            while(succ == false){
              CacheStatus tmp_status = line->getStatus();
              monitor.check("DIR_2_AGENT_WRITE_MISS_DIRTY timeout. agent " + std::to_string(agent_id) + " tag=" + std::to_string(line->tag) + " status=" + std::to_string(tmp_status) + " shadow_flag=" + std::to_string(line->shadow_flag), 5.0);
              if(tmp_status == CacheStatus::MODIFIED){
                succ = line->casFlagAndStatus(false, CacheStatus::MODIFIED, false, CacheStatus::INVALID);
                if(succ){
                }
              }
              else if(tmp_status == CacheStatus::TO_MODIFIED || tmp_status == CacheStatus::TO_SHARED || tmp_status == CacheStatus::TO_PRESERVED){
                  succ = line->casFlagAndStatus(false, tmp_status, true, tmp_status);
              }
              else if(tmp_status == CacheStatus::PRESERVED) succ = true;
            }
            break;
          }
          default:{
            agent_stats_inst.print_msg(m);
            assert(false);
          }
        }
      }
      set_rwlock.rUnlock();
    }
  }
}