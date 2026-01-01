#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "DSM.h"
#include "CacheAgent.h"
#include "LocalAllocator.h"
DSM *DSM::getInstance(const DSMConfig &conf) {
  static DSM *dsm = nullptr;
  if (!dsm) {
    dsm = new DSM(conf);
  }
  return dsm;
}
void DSM::registerThread() {
  cache.req_appt_id = appID.fetch_add(1);
  cache.channel = new channel_context(resource, cache.req_appt_id);
  app_channel[cache.req_appt_id] = cache.channel;
  cache.seed = time(NULL);
  cache.next_allocator_node = rand_r(&cache.seed) % conf.node_num;
  cache.alloc = new LocalAllocator();
}
void DSM::deleteThread() {
  delete cache.channel;
}
DSM::DSM(const DSMConfig &conf)
    : conf(conf), cache(conf, this){
  resource = node_resource_factory::get_node_resource(conf);
  gblock_data_base_addr = resource->get_data_base_addr();
  cline_data_base_addr = reinterpret_cast<void *>(reinterpret_cast<uint64_t>(gblock_data_base_addr) + conf.gmem_size_per_node);
  gblock_data_sb.slabs_init(conf.gmem_size_per_node * 1.5, conf.factor, gblock_data_base_addr);
  cline_data_sb.slabs_init(conf.cache_size_per_node * 1.5, conf.factor, cline_data_base_addr);
  resource->init_system_channel();
  printf("INFO: gblock_data_base_addr = 0x%lx\n", (uint64_t)gblock_data_base_addr);
  printf("INFO: cline_data_base_addr = 0x%lx\n", (uint64_t)cline_data_base_addr);
  printf("INFO: init fpga dir info\n");
  printf("INFO: sizeof(RawMessage) = %ld\n", sizeof(RawMessage));
  for(int i = 0; i < conf.gblock_num_per_node; i++){
    RawMessage m;
    m.mtype = RawMessageType::INIT_DIR;
    m.dir_gblock_addr = (uint64_t)(this->gblock_data_sb.sb_aligned_calloc(1, DSM_CACHE_LINE_SIZE));
    m.dir_gblock_index = i;
    for(int j = 0; j < max_node_num; j++){
      m.dir_gblock_bitmap[j] = 0;
    }
    m.init_state = RawState::S_UNSHARED;
    resource->system_channel->send(m);
    if(i == 0){
      printf("INFO: msg[0] = ");
      agent_stats_inst.printRaw(&m, sizeof(m));
    }
  }
  printf("INFO: init fpga net info\n");
  for (uint32_t i = 0;i < max_node_num; i++) {
      RawMessage m;
      m.mtype = RawMessageType::INIT_NET;
      m.dir_gblock_nodeID = i;
      m.init_mac[0] = conf.mac_vec[i].mac_addr[0];
      m.init_mac[1] = conf.mac_vec[i].mac_addr[1];
      m.init_mac[2] = conf.mac_vec[i].mac_addr[2];
      m.init_mac[3] = conf.mac_vec[i].mac_addr[3];
      m.init_mac[4] = conf.mac_vec[i].mac_addr[4];
      m.init_mac[5] = conf.mac_vec[i].mac_addr[5];
      resource->system_channel->send(m);
      if(i == 0){
        printf("INFO: msg[0] = ");
        agent_stats_inst.printRaw(&m, sizeof(m));
      }
  }
  printf("INFO: init cpu cline info\n");
  cache.initLine(cline_data_sb);
  resource->init_channel_parameter(conf);
  for(int i = 0; i < conf.syst_num_per_node; i++){
    sys_channel[i] = new channel_context(resource, max_appt_num_per_node + i);
    cacheAgent[i] = new CacheAgent(i, &cache, sys_channel[i]);
  }
  for(int i = 0; i < conf.syst_num_per_node; i++){
    GlobalAddress gmem_start;
    uint64_t gmem_size_per_syst = conf.gmem_size_per_node / conf.syst_num_per_node;
    gmem_start.nodeID = conf.node_id;
    gmem_start.addr = gmem_size_per_syst * i; 
    chunckAlloc[i] = new GlobalAllocator(gmem_start, gmem_size_per_syst);
  }
}
DSM::~DSM() {
  for(int i = 0; i < conf.syst_num_per_node; i++){
    delete sys_channel[i];
    delete cacheAgent[i];
    delete chunckAlloc[i];
  }
  node_resource_factory::destory_node_resource(conf.fpga_pci_bus);
}
void DSM::stop(){
  for(int i = 0; i < conf.syst_num_per_node; i++){
    cacheAgent[i]->stopFlag = true;
  }
}
void DSM::read(const GlobalAddress &addr, uint32_t size, uint8_t *to) {
  uint32_t start = addr.addr % DSM_CACHE_LINE_SIZE;
  uint32_t len = std::min(size, DSM_CACHE_LINE_SIZE - start);
  cache.readLine(addr, start, len, to);
  agent_stats_inst.cc_stat.total_access[getMyThreadID()]++;
  if (len == size)
    return;
  uint8_t *end = to + size;
  GlobalAddress iter = addr;
  to += len;
  iter.addr += len;
  while (end - to >= DSM_CACHE_LINE_SIZE) {
    cache.readLine(iter, 0, DSM_CACHE_LINE_SIZE, to);
    to += DSM_CACHE_LINE_SIZE;
    iter.addr += DSM_CACHE_LINE_SIZE;
    agent_stats_inst.cc_stat.total_access[getMyThreadID()]++;
  }
  if (end != to) {
    cache.readLine(iter, 0, end - to, to);
    agent_stats_inst.cc_stat.total_access[getMyThreadID()]++;
  }
}
void DSM::write(const GlobalAddress &addr, uint32_t size, const uint8_t *from) {
  uint32_t start = addr.addr % DSM_CACHE_LINE_SIZE;
  uint32_t len = std::min(size, DSM_CACHE_LINE_SIZE - start);
  cache.writeLine(addr, start, len, from);
  agent_stats_inst.cc_stat.total_access[getMyThreadID()]++;
  if (len == size)
    return;
  uint8_t *end = (uint8_t *)from + size;
  GlobalAddress iter = addr;
  from += len;
  iter.addr += len;
  while (end - from >= DSM_CACHE_LINE_SIZE) {
    cache.writeLine(iter, 0, DSM_CACHE_LINE_SIZE, from);
    from += DSM_CACHE_LINE_SIZE;
    iter.addr += DSM_CACHE_LINE_SIZE;
    agent_stats_inst.cc_stat.total_access[getMyThreadID()]++;
  }
  if (end != from) {
    cache.writeLine(iter, 0, end - from, from);
    agent_stats_inst.cc_stat.total_access[getMyThreadID()]++;
  }
}
void DSM::drain_eviction(){
  cache.wait_evict();
}
void DSM::print_config(){
  std::cout << std::fixed << std::setprecision(2);  
  std::cout << "node_num = " << conf.node_num << "\n";
  std::cout << "appt_num_per_node = " << conf.appt_num_per_node << "\n";
  std::cout << "gmem_size_per_node = " << conf.gmem_size_per_node << "\n";
  std::cout << "cache_size_per_node = " << conf.cache_size_per_node << "\n";
  std::cout << "cline_num_per_node = " << conf.cline_num_per_node << "\n";
  std::cout << "gblock_num_per_node = " << conf.gblock_num_per_node << "\n";
}
