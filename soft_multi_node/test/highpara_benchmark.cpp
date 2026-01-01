#include <algorithm>
#include <atomic>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <random>
#include <thread>
#include "DSM.h"
#include "agent_stat.h"
#include <cstdint>
#include <chrono>
#include <vector>
using namespace std;
using namespace chrono;
int is_home = 0;
int is_cache = 0;
int is_request = 0;
int cache_rw = 0;
int request_rw = 0;
int breakdown_times = 1024;
const char *result_directory = "gam_result";
uint64_t breakdown_size = DSM_CACHE_LINE_SIZE * breakdown_times;
#define MAX_THREAD 24
#define OP_NUMS 2000000
#define OBJ_SIZE 8
#define MB (1024ull * 1024)
const uint64_t BLOCK_SIZE = uint64_t(MB * 31); 
#define SUB_PAGE_SIZE (512) 
#define STEPS (BLOCK_SIZE * nodeNR / SUB_PAGE_SIZE)
extern thread_local uint64_t evict_time;
GlobalAddress *access_[MAX_THREAD];
double latency[OP_NUMS];
int nodeNR = 0;
int threadNR = 0;
int readNR = 0;
int locality = 0;
int sharing = 0;
int round_cnt = 0;
thread_local uint32_t seed;
std::thread *th[MAX_THREAD];
DSM *dsm;
int node_id = 0xff;
uint8_t fpga_pci_bus = 0xff;
mac_t mac_vec[8];
int mac_count = 0;
std::atomic<int> ready{0};
std::atomic<int> finish{0};
std::atomic<int> warmup_no_breakdown{0};
std::atomic<int> warmup_breakdown{0};
std::atomic<int> warmup_start{0};
char *output_file = nullptr;
void init_trace(int nodeID, int threadID) {
  auto traces = new GlobalAddress[STEPS];
  int id = nodeID * threadNR + threadID;
  int all = nodeNR * threadNR;
  uint64_t addrStart = id * BLOCK_SIZE;
  uint64_t addrSize = BLOCK_SIZE;
  uint64_t sharingStart = (all + 1) * BLOCK_SIZE;
  uint64_t sharingSize = BLOCK_SIZE;
  seed = time(NULL) + nodeID * 10 + threadID;
  int node = 0;
  uint64_t offset = 0;
  for (int op = 0; op < STEPS; ++op) {
    bool isSharing = (rand_r(&seed) % 100) < sharing;
    GlobalAddress addr;
    if (!isSharing) {
      addr.nodeID = node;
      addr.addr = addrStart + offset;
      node = (node + 1) % nodeNR;
      if (node == 0) {
        offset += SUB_PAGE_SIZE;
      }
    } else {
      addr.nodeID = rand_r(&seed) % nodeNR;
      addr.addr = sharingStart + rand_r(&seed) % sharingSize;
    }
    traces[op] = addr;
  }
  auto traces_breakdown = new GlobalAddress[breakdown_times];
  for (int op = 0; op < breakdown_times; ++op) {
    GlobalAddress addr;
    addr.nodeID = agent_stats_inst.home_node_id;
    addr.addr = sharingStart + sharingSize + op * DSM_CACHE_LINE_SIZE;
    traces_breakdown[op] = addr;
  }
  access_[threadID] = new GlobalAddress[OP_NUMS + breakdown_times];
  auto thread_access_ = access_[threadID];
  thread_access_[0] = traces[rand_r(&seed) % STEPS];
  GlobalAddress next = thread_access_[0];
  for (int i = 1; i < OP_NUMS; ++i) {
    bool isLocality = (rand_r(&seed) % 100) < locality;
    if (isLocality) {
      uint64_t offset = next.addr % 4096;
      if (offset + OBJ_SIZE + OBJ_SIZE < 4096) {
        next.addr += OBJ_SIZE;
      } else {
        next.addr -= offset;
      }
    } else {
      next = traces[rand_r(&seed) % STEPS];
    }
    thread_access_[i] = next;
  }
  for (int i = 0; i < breakdown_times; ++i) {
    next = traces_breakdown[i];
    thread_access_[OP_NUMS + i] = next;
    if (threadID == 0) {
      agent_stats_inst.push_valid_gaddr(next.addr);
    }
  }
  delete[] traces;
  delete[] traces_breakdown;
}
void Run_cache(int nodeID, int threadID, const std::string &prefix) {
  GlobalAddress *thread_access_ = access_[threadID];
  uint8_t from[OBJ_SIZE];
  uint8_t to[OBJ_SIZE];
  for (int op = 0; op < breakdown_times; ++op) {
    if (is_cache == 1 && threadID == 0) {
        GlobalAddress to_access_breakdown = thread_access_[OP_NUMS + op];
        switch (cache_rw) {
        case 0: {
          dsm->Try_RLock(to_access_breakdown, OBJ_SIZE);
          dsm->read(to_access_breakdown, OBJ_SIZE, to);
          dsm->UnLock(to_access_breakdown, OBJ_SIZE);
          break;
        }
        case 1: {
          dsm->Try_WLock(to_access_breakdown, OBJ_SIZE);
          dsm->write(to_access_breakdown, OBJ_SIZE, from);
          dsm->UnLock(to_access_breakdown, OBJ_SIZE);
          break;
        }
        default: {
          break;
        }
      }
    }
  }
}
void Run_request(int nodeID, int threadID, const std::string &prefix) {
  int count_4_nobreakdown = 0;
  int count_4_breakdown = 0;
  int thres_4_nobreakdown = 0.75 * (OP_NUMS / (breakdown_times + 1)) + 1;
  GlobalAddress *thread_access_ = access_[threadID];
  uint8_t from[OBJ_SIZE];
  uint8_t to[OBJ_SIZE];
  timespec s, e;
  for (int op = 0; op < OP_NUMS && count_4_breakdown < breakdown_times; ++op) {
    if (is_request == 1 && prefix != "warmup" && threadID == 0) {
      count_4_nobreakdown++;
      if (count_4_nobreakdown == thres_4_nobreakdown) {
        count_4_nobreakdown = 0;
        GlobalAddress to_access_breakdown = thread_access_[OP_NUMS + count_4_breakdown];
        count_4_breakdown++;
        switch (request_rw) {
        case 0: {
          agent_stats_inst.start_record_app_thread(to_access_breakdown.addr);
          dsm->read(to_access_breakdown, OBJ_SIZE, to);
          agent_stats_inst.stop_record_app_thread_with_op(to_access_breakdown.addr, APP_THREAD_OP::WAKEUP_2_READ_RETURN);
          break;
        }
        case 1: {
          agent_stats_inst.start_record_app_thread(to_access_breakdown.addr);
          dsm->write(to_access_breakdown, OBJ_SIZE, from);
          agent_stats_inst.stop_record_app_thread_with_op(to_access_breakdown.addr, APP_THREAD_OP::WAKEUP_2_WRITE_RETURN);
          break;
        }
        default: {
          break;
        }
        }
      }
    }
    memset(from, 0, OBJ_SIZE);
    bool isRead = (rand_r(&seed) % 100) < readNR;
#ifdef SHOW_LATENCY
    clock_gettime(CLOCK_REALTIME, &s);
#endif
    if (isRead) {
      dsm->read(thread_access_[op], OBJ_SIZE, to);
    } else {
      dsm->write(thread_access_[op], OBJ_SIZE, from);
    }
#ifdef SHOW_LATENCY
    clock_gettime(CLOCK_REALTIME, &e);
    if (threadID == 0) {
      latency[op] = (e.tv_sec - s.tv_sec) * 1000000 +
                    (double)(e.tv_nsec - s.tv_nsec) / 1000;
    }
#endif
  }
}
void start_thread(int nodeID, int threadID) {
  printf("checkpoint 0 on thread %d\n", threadID);
  fflush(stdout);
  printf("checkpoint 1 on thread %d\n", threadID);
  fflush(stdout);
  dsm->registerThread();
  printf("wait no-breakdown warmup start on thread %d\n", threadID);
  if (threadID == 0) {
    agent_stats_inst.stall();
  }
  warmup_start.fetch_add(1);
  while (warmup_start.load() != threadNR) ;
  printf("start warmup the cache for no-breakdown on thread %d\n", threadID);
  fflush(stdout);
  Run_request(nodeID, threadID, "warmup"); 
  printf("checkpoint 2 on thread %d\n", threadID);
  fflush(stdout);
  printf("wait breakdown warmup start on thread %d\n", threadID);
  if (threadID == 0) {
    agent_stats_inst.stall();
  }  
  warmup_no_breakdown.fetch_add(1);
  while (warmup_no_breakdown.load() != threadNR) ;
  printf("start warmup the cache for breakdown on thread %d\n", threadID);
  fflush(stdout);
  Run_cache(nodeID, threadID, "warmup"); 
  printf("checkpoint 3 on thread %d\n", threadID);
  fflush(stdout);
  printf("wait test start on thread %d\n", threadID);
  warmup_breakdown.fetch_add(1);
  while (warmup_breakdown.load() != threadNR) ;
  if (threadID == 0) {
    agent_stats_inst.stall();
  }
  printf("checkpoint 4 on thread %d\n", threadID);
  fflush(stdout);
  ready.fetch_add(1);
  while (ready.load() != threadNR) ;
  if (threadID == 0) agent_stats_inst.start_collection();
  printf("start run the benchmark on thread %d\n", threadID);
  fflush(stdout);
  Run_request(nodeID, threadID, "benchmark");
  finish.fetch_add(1);
  if (threadID == 0) {
    agent_stats_inst.save_stat_to_file(std::string(result_directory));
  }
}
void parserArgs(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--no_node") == 0) {
      nodeNR = atoi(argv[++i]);  
    }
    else if (strcmp(argv[i], "--no_thread") == 0) {
      threadNR = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--locality") == 0) {
      locality = atoi(argv[++i]);  
    } else if (strcmp(argv[i], "--shared_ratio") == 0) {
      sharing = atoi(argv[++i]);  
    } else if (strcmp(argv[i], "--read_ratio") == 0) {
      readNR = atoi(argv[++i]);  
    }
    else if (strcmp(argv[i], "--is_cache") == 0) {
      is_cache = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--cache_rw") == 0) {
      cache_rw = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--is_request") == 0) {
      is_request = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--request_rw") == 0) {
      request_rw = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--is_home") == 0) {
      is_home = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--home_node_id") == 0) {
      agent_stats_inst.home_node_id = atoi(argv[++i]);
    }else if (strcmp(argv[i], "--breakdown_times") == 0) {
      breakdown_times = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--result_dir") == 0) {
      result_directory = argv[++i];  
    }
    else if (strcmp(argv[i], "--node_id") == 0) {
      node_id = atoi(argv[++i]);
    } 
    else if (strcmp(argv[i], "--fpga_pci_bus") == 0) {
      char* endptr;
      fpga_pci_bus = strtol(argv[++i], &endptr, 16);
      if (*endptr != '\0') {
          std::cerr << "Invalid hexadecimal number: " << argv[i] << std::endl;
          exit(1);
      }
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
        mac_count = idx;
    }
    else {
      fprintf(stderr, "Unrecognized option %s for benchmark\n", argv[i]);
    }
  }
  fprintf(stdout,
          "Benchmark Config: nodeNR %d, threadNR %d, readNR %d, locality %d, "
          "sharing %d\n",
          nodeNR, threadNR, readNR, locality, sharing);
}
int main(int argc, char **argv) {
  parserArgs(argc, argv);
  printf("My CC configuration is: ");
  printf("is_home = %d, is_cache = %d, cache_rw = %d, is_request = %d, request_rw = %d, breakdown_times = %d\n",
    is_home, is_cache, cache_rw, is_request, request_rw, breakdown_times);
  agent_stats_inst.is_cache = is_cache;
  agent_stats_inst.is_request = is_request;
  agent_stats_inst.is_home = is_home;
  if(node_id == 0xff){
    printf("EXIT 1: node_id == 0xff");
    exit(1);
  }
  float cache_ratio = 1;
  int node_num = nodeNR;
  int appt_num_per_node = threadNR;
  int syst_num_per_node = max_syst_num_per_node;
  uint64_t gmem_size_per_node = BLOCK_SIZE * (nodeNR * threadNR + 1 + 1) + breakdown_size;
  uint64_t cache_size_per_node = gmem_size_per_node * cache_ratio;
  if(fpga_pci_bus == 0xff){
    printf("EXIT 2: fpga_pci_bus == 0xff");
    exit(2);
  }
  if(mac_count == 0){
    printf("EXIT 3: mac_count == 0");
    exit(3);
  }
  DSMConfig conf(node_id,
                  node_num,
                  appt_num_per_node,
                  syst_num_per_node,
                  gmem_size_per_node,
                  cache_size_per_node,
                  fpga_pci_bus,
                  mac_vec);
  cout << "----------" << endl;
  cout << "user access space per node = " << ((long)STEPS) * SUB_PAGE_SIZE * threadNR * 1.0 / (1024 * 1024 * 1024) << "GB" << endl;
  cout << "user cache ratio per node = " << cache_size_per_node / ((long)STEPS * SUB_PAGE_SIZE * threadNR * 1.0) << endl;
  agent_stats_inst.end_collection();
  dsm = DSM::getInstance(conf);
  printf("benchmark init\n");
  agent_stats_inst.set1_thread_init_flag(0);
  for (int i = 0; i < threadNR; ++i) {
    th[i] = new std::thread(init_trace, dsm->getMyNodeID(), i);
  }
  for (int i = 0; i < threadNR; ++i) {
    th[i]->join();
  }
  warmup_no_breakdown.store(0);
  warmup_breakdown.store(0);
  ready.store(0);
  finish.store(0);
  agent_stats_inst.set1_thread_init_flag(0);
  for (int i = 0; i < threadNR; i++) {
    th[i] = new std::thread(start_thread, dsm->getMyNodeID(), i);
    if(i < 24) bind_to_core(*(th[i]), 1, i);
    else{
      printf("App threads num larger than 24! exit!");
    }
  }
  timespec s, e;
  while (ready.load() != threadNR)
    ;
  clock_gettime(CLOCK_REALTIME, &s);
  while (finish.load() != threadNR)
    ;
  clock_gettime(CLOCK_REALTIME, &e);
  double microseconds = (e.tv_sec - s.tv_sec) * 1000000 + (double)(e.tv_nsec - s.tv_nsec) / 1000;
  uint64_t ops = threadNR * OP_NUMS;
  uint64_t tp = ops * 1000000 / microseconds;
  for (int i = 0; i < threadNR; ++i) {
    th[i]->join();
    delete[] access_[i];
  }
  printf("benchmark ends\n");
  std::string common_suffix = ".txt";
  if (!fs::exists(result_directory)) {
      if (!fs::create_directory(result_directory)) {
          std::cerr << "Error creating folder " << result_directory << std::endl;
          exit(1);
      }
  }
  FILE *file;
  fs::path dir(result_directory);
  fs::path filePath = dir / fs::path("end_to_end" + common_suffix);
  file = fopen(filePath.c_str(), "a");
  assert(file != nullptr);
  if (dsm->myNodeID == 0) {
    fprintf(file, "sharing ratio = %d\t tp: %llu op/s\n", sharing, tp);
    fprintf(file, agent_stats_inst.return_cc_stat().c_str());
  }
  fclose(file);
  printf("wait end\n");
  agent_stats_inst.stall();
  dsm->stop();
  sleep(1);
  return 0;
}
