#include <algorithm>
#include <atomic>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <random>
#include <thread>
#include "DSM.h"
#include "agent_stat.h"
DSM *dsm;
int node_id = 8;
int node_num = 8;
int appt_num_per_node = 24;
int syst_num_per_node = max_syst_num_per_node;
int read_ratio = 60;
int sharing_ratio = 50;
uint8_t fpga_pci_bus = 0x43;
mac_t mac_vec[8];
int warmup_type = 0;
int test_type = 0;
int obj_size = 8;
uint64_t obj_aligned_size = 512;
uint64_t workload_size_per_appt = uint64_t(define::MB * 31);
float cache_ratio = 2;
uint64_t gmem_size_per_node = workload_size_per_appt * appt_num_per_node + workload_size_per_appt * appt_num_per_node;
uint64_t cache_size_per_node = gmem_size_per_node * cache_ratio;
uint64_t step_num_per_appt = 1;
uint64_t op_num_per_appt = 1000000; 
std::atomic<int> init_ready{0};
std::atomic<int> warmup_ready{0};
std::atomic<int> warmup_stats{0};
std::atomic<int> test_stats{0};
float test_latency[max_appt_num_per_node];
float test_throughput[max_appt_num_per_node];
uint64_t rdtsc() {
  unsigned int lo, hi;
  __asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
  return ((uint64_t)hi << 32) | lo;
}
uint64_t rdtscp() {
  unsigned int lo, hi;
  __asm__ __volatile__("rdtscp" : "=a" (lo), "=d" (hi));
  return ((uint64_t)hi << 32) | lo;
}
WRLock trace_map_lock;
std::unordered_map<int, GlobalAddress*> all_traces;
GlobalAddress* get_trace(int node_id, int appt_id){
  int glb_id = node_id * appt_num_per_node + appt_id;
  return all_traces[glb_id];
}
void init_trace(int node_id, int appt_id){
  auto traces = new GlobalAddress[step_num_per_appt];
  int glb_id = node_id * appt_num_per_node + appt_id;
  uint32_t seed = 0;
  uint64_t sharingStart = appt_num_per_node * workload_size_per_appt;
  uint64_t sharingSize = ((workload_size_per_appt / node_num) * (sharing_ratio / 100.0)) > (2 * obj_aligned_size) ?
                          ((workload_size_per_appt / node_num) * (sharing_ratio / 100.0)) : (2 * obj_aligned_size); 
  uint64_t access_base = appt_id * workload_size_per_appt;
  uint64_t access_offset = 0;
  for (uint64_t i = 0; i < step_num_per_appt; ++i) {
    bool isSharing = (rand_r(&seed) % 100) < sharing_ratio;
    GlobalAddress addr;
    if (!isSharing) {
      addr.nodeID = node_id;
      addr.addr = access_base + access_offset; 
      addr.org = CacheOrganizaion::FULLY_ASSOCIATIVE;
      if(access_offset > workload_size_per_appt - obj_aligned_size){
        access_offset = 0;
      }
      else{
        access_offset += obj_aligned_size;
      }
    } else {
      addr.nodeID = rand_r(&seed) % node_num;
      addr.addr = sharingStart + (rand_r(&seed) % (sharingSize - obj_aligned_size)) / obj_aligned_size * obj_aligned_size; 
      addr.org = CacheOrganizaion::SET_ASSOCIATIVE;
    }
    traces[i] = addr;
  }
  trace_map_lock.wLock();
  all_traces[glb_id] = traces;
  trace_map_lock.wUnlock();
}
void request(){
  dsm->registerThread();
  int cur_node_id = dsm->getMyNodeID();
  int cur_appt_id = dsm->getMyThreadID();
  uint8_t from[obj_size];
  uint8_t to[obj_size];
  uint64_t tmp[8];
  init_trace(cur_node_id, cur_appt_id);
  GlobalAddress* trace = get_trace(cur_node_id, cur_appt_id);
  if(cur_appt_id == 0){
    agent_stats_inst.clean_cc_stat();
    printf("node[%d]: init ends!\n", cur_node_id);
    agent_stats_inst.stall();
  }
  init_ready.fetch_add(1);
  while (init_ready.load() != appt_num_per_node) ;
  uint64_t warmup_start_tp = rdtsc();
  uint32_t seed = cur_node_id * 100 + cur_appt_id;
  for(uint64_t i = 0; i < op_num_per_appt; i++){
    uint64_t idx = i % step_num_per_appt;
    GlobalAddress access = trace[idx];
    if(warmup_type == 0){
      dsm->read(access, obj_size, to);
    }
    else if(warmup_type == 1){
      dsm->write(access, obj_size, from);
    }
    else if(warmup_type == 3){
      GlobalAddress tmp_trace;
      tmp_trace.nodeID = 0;
      tmp_trace.addr = 0; 
      dsm->read(tmp_trace, 64, (uint8_t *)tmp);
      tmp[node_id] += 1;
      dsm->write(tmp_trace, 64, (uint8_t *)tmp);
    }
    else{
      int rand_num = rand_r(&seed) % 100;
      bool isRead = (rand_num < read_ratio) ? true : false;
      if(isRead){
        dsm->read(access, obj_size, to);
      }
      else{
        dsm->write(access, obj_size, from);
      }
    }
  }
  dsm->drain_eviction();
  uint64_t warmup_end_tp = rdtscp();
  float warmup_us = (warmup_end_tp - warmup_start_tp) / 2200;
  printf("node[%d], appt[%d]: op latency = %.2f us, op throughput = %.2f Mqps\n", cur_node_id, cur_appt_id, warmup_us / op_num_per_appt, op_num_per_appt / warmup_us);
  warmup_stats.fetch_add(1);
  while (warmup_stats.load() != appt_num_per_node) ;
  if(cur_appt_id == 0){
    agent_stats_inst.print_cc_stat();
    agent_stats_inst.clean_cc_stat();
    if(warmup_type == 3){
      GlobalAddress tmp_trace;
      tmp_trace.nodeID = 0;
      tmp_trace.addr = 0; 
      dsm->read(tmp_trace, 64, (uint8_t *)tmp);
      printf("node %d accesses tmp_trace %d times.\n", node_id, tmp[node_id]);
    }
    dsm->drain_eviction();
    dsm->resource->debug_print();
  }
  if(cur_appt_id == 0){
    printf("node[%d]: warm up ends!\n", cur_node_id);
    agent_stats_inst.stall();
  }
  warmup_ready.fetch_add(1);
  while (warmup_ready.load() != appt_num_per_node) ;
  uint64_t test_start_tp = rdtsc();
  uint32_t s_seed = cur_node_id * 100 + cur_appt_id;
  for(uint64_t i = 0; i < op_num_per_appt; i++){
    uint64_t idx = i % step_num_per_appt;
    GlobalAddress access = trace[idx];
    if(i < 10) Debug::notifyInfo("node id = %d, local addr = %x", access.nodeID, access.addr);
    if(test_type == 0) dsm->read(access, obj_size, to);
    else if(test_type == 1) dsm->write(access, obj_size, from);
    else if(test_type == 3){
      GlobalAddress tmp_trace;
      tmp_trace.nodeID = 0;
      tmp_trace.addr = 0; 
      dsm->read(tmp_trace, 64, (uint8_t *)tmp);
      tmp[node_id] += 1;
      dsm->write(tmp_trace, 64, (uint8_t *)tmp);
    }
    else{
      int rand_num = rand_r(&s_seed) % 100;
      bool isRead = (rand_num < read_ratio) ? true : false;
      if(isRead){
        dsm->read(access, obj_size, to);
      }
      else{
        dsm->write(access, obj_size, from);
      }
    }
  }
  dsm->drain_eviction();
  uint64_t test_end_tp = rdtscp();
  float test_us = (test_end_tp - test_start_tp) / 2200;
  printf("node[%d], appt[%d]: op latency = %.2f us, op throughput = %.2f Mqps\n", cur_node_id, cur_appt_id, test_us / op_num_per_appt, op_num_per_appt / test_us);
  test_latency[cur_appt_id] = test_us / op_num_per_appt;
  test_throughput[cur_appt_id] = op_num_per_appt / test_us;
  test_stats.fetch_add(1);
  while (test_stats.load() != appt_num_per_node) ;
  if(cur_appt_id == 0){
    agent_stats_inst.print_cc_stat();
    if(test_type == 3){
      GlobalAddress tmp_trace;
      tmp_trace.nodeID = 0;
      tmp_trace.addr = 0; 
      dsm->read(tmp_trace, 64, (uint8_t *)tmp);
      printf("node %d accesses tmp_trace %d times.\n", node_id, tmp[node_id]);
    }
    dsm->drain_eviction();
    dsm->resource->debug_print();
  }
  dsm->deleteThread();
}
void parserArgs(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--node_num") == 0) {
      node_num = atoi(argv[++i]);  
    }
    else if (strcmp(argv[i], "--node_id") == 0) {
      node_id = atoi(argv[++i]);
    } 
    else if (strcmp(argv[i], "--appt_num") == 0) {
      appt_num_per_node = atoi(argv[++i]);
    } 
    else if (strcmp(argv[i], "--syst_num") == 0) {
      syst_num_per_node = atoi(argv[++i]);
    } 
    else if (strcmp(argv[i], "--read_ratio") == 0) {
      read_ratio = atoi(argv[++i]);  
    }
    else if (strcmp(argv[i], "--shared_ratio") == 0) {
      sharing_ratio = atoi(argv[++i]);  
    } 
    else if (strcmp(argv[i], "--fpga_pci_bus") == 0) {
      char* endptr;
      fpga_pci_bus = strtol(argv[++i], &endptr, 16);
      if (*endptr != '\0') {
          std::cerr << "Invalid hexadecimal number: " << argv[i] << std::endl;
          exit(1);
      }
    }
    else if (strcmp(argv[i], "--warmup_type") == 0) {
      warmup_type = atoi(argv[++i]);  
    }
    else if (strcmp(argv[i], "--test_type") == 0) {
      test_type = atoi(argv[++i]);  
    } 
    else if (strcmp(argv[i], "--macs") == 0) {
        std::string macs_input = argv[++i];
        std::stringstream ss(macs_input);
        std::string mac_str;
        int idx = 0;
        while (std::getline(ss, mac_str, ',')) {  
            mac_t mac;
            if (sscanf(mac_str.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                       &mac.mac_addr[0], &mac.mac_addr[1], &mac.mac_addr[2],
                       &mac.mac_addr[3], &mac.mac_addr[4], &mac.mac_addr[5]) != 6) {
                std::cerr << "Invalid MAC address format: " << mac_str << std::endl;
                exit(1);
            }
            mac_vec[idx] = mac;  
            idx++;
        }
    }
    else {
      fprintf(stderr, "Unrecognized option %s for benchmark\n", argv[i]);
    }
  }
}
int main(int argc, char **argv) {
  Monitor monitor;
  monitor.start();
  parserArgs(argc, argv);
  printf("node[%d]: step_num_per_appt = %d\n", node_id, step_num_per_appt);
  printf("node[%d]: op_num_per_appt = %d\n", node_id, op_num_per_appt);
  DSMConfig conf(node_id,
                  node_num,
                  appt_num_per_node,
                  syst_num_per_node,
                  gmem_size_per_node,
                  cache_size_per_node,
                  fpga_pci_bus,
                  mac_vec);
  dsm = DSM::getInstance(conf);
  printf("node[%d]: start appt thread\n", node_id);
  agent_stats_inst.stall();
  std::thread *reqt[48];
  for(int i = 0; i < appt_num_per_node; i++){
    reqt[i] = new std::thread(request);
  }
  for(int i = 0; i < appt_num_per_node; i++){
    reqt[i]->join();
  }
  for(int i = 0; i < appt_num_per_node; i++){
    delete reqt[i];
  }
  float latency = 0;
  float throughput = 0;
  for(int i = 0; i < appt_num_per_node; i++){
    latency += test_latency[i];
    throughput += test_throughput[i];
  }
  printf("node[%d]: avg op latency = %.2f us, total op throughput = %.2f Mqps\n", node_id, latency / appt_num_per_node, throughput);
  agent_stats_inst.stall();
  dsm->stop();
  printf("Execution Finished\n");
  return 0;
}