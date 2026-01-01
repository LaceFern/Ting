#include <algorithm>
#include <atomic>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <random>
#include <thread>
#include <ctime>
#include <cstring>
#include <unistd.h>
#include <sstream>
#include "DSM.h"

#define MAX_THREAD 5
#define OP_NUMS 2000000
#define OBJ_SIZE 8
#define MB (1024ull * 1024)
const uint64_t BLOCK_SIZE = uint64_t(MB * 62);
#define SUB_PAGE_SIZE (512)
#define STEPS (BLOCK_SIZE * nodeNR / SUB_PAGE_SIZE)

GlobalAddress *access_[MAX_THREAD];
double latency[OP_NUMS];

int nodeNR = 4;
int threadNR = 4;
int readNR = 0;
int locality = 0;
int sharing = 0;

thread_local uint32_t seed;

std::thread *th[MAX_THREAD];
DSM *dsm;

std::atomic<int> init{0};
std::atomic<int> ready{0};
std::atomic<int> finish{0};

char *output_file = nullptr;

void parseArgs(int argc, char **argv, int &node_id, int &node_num, int &appt_num, 
               int &syst_num, uint8_t &fpga_pci_bus, mac_t *mac_vec) {
  node_id = 0;
  node_num = 4;
  appt_num = 4;
  syst_num = 8;
  fpga_pci_bus = 0x43;
  
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--node_num") == 0) {
      node_num = atoi(argv[++i]);
      nodeNR = node_num;
    } else if (strcmp(argv[i], "--node_id") == 0) {
      node_id = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--appt_num") == 0) {
      appt_num = atoi(argv[++i]);
      threadNR = appt_num;
    } else if (strcmp(argv[i], "--syst_num") == 0) {
      syst_num = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--read_ratio") == 0) {
      readNR = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--locality") == 0) {
      locality = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--sharing") == 0) {
      sharing = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--fpga_pci_bus") == 0) {
      char* endptr;
      fpga_pci_bus = strtol(argv[++i], &endptr, 16);
    } else if (strcmp(argv[i], "--macs") == 0) {
      std::string macs_input = argv[++i];
      std::stringstream ss(macs_input);
      std::string mac_str;
      int idx = 0;
      while (std::getline(ss, mac_str, ',')) {
        mac_t mac;
        if (sscanf(mac_str.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                   &mac.mac_addr[0], &mac.mac_addr[1], &mac.mac_addr[2],
                   &mac.mac_addr[3], &mac.mac_addr[4], &mac.mac_addr[5]) == 6) {
          mac_vec[idx++] = mac;
        }
      }
    } else if (strcmp(argv[i], "--output") == 0) {
      output_file = argv[++i];
    }
  }
  
  if (mac_vec[0].mac_addr[0] == 0) {
    for (int i = 0; i < node_num; i++) {
      mac_vec[i].mac_addr[0] = 0x01;
      mac_vec[i].mac_addr[1] = 0x01;
      mac_vec[i].mac_addr[2] = 0x01;
      mac_vec[i].mac_addr[3] = 0x01;
      mac_vec[i].mac_addr[4] = 0x01;
      mac_vec[i].mac_addr[5] = 0x01 + i;
    }
  }
}

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
  
  access_[threadID] = new GlobalAddress[OP_NUMS];
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
  
  delete[] traces;
}

void benchmark(int nodeID, int threadID, const std::string &prefix);
void start_thread(int nodeID, int threadID) {
  dsm->registerThread();
  
  benchmark(nodeID, threadID, "warmup");
  
  init.fetch_add(1);
  while (init.load() != threadNR);
  
  ready.fetch_add(1);
  while (ready.load() != threadNR);
  
  benchmark(nodeID, threadID, "benchmark");
  finish.fetch_add(1);
}

void benchmark(int nodeID, int threadID, const std::string &prefix) {
  GlobalAddress *thread_access_ = access_[threadID];
  
  uint8_t from[OBJ_SIZE];
  uint8_t to[OBJ_SIZE];
  
  for (int op = 0; op < OP_NUMS; ++op) {
    memset(from, 0, OBJ_SIZE);
    
    bool isRead = (rand_r(&seed) % 100) < readNR;
    
    if (isRead) {
      dsm->read(thread_access_[op], OBJ_SIZE, to);
    } else {
      dsm->write(thread_access_[op], OBJ_SIZE, from);
    }
  }
}

void parserArgs(int argc, char **argv) {
  if (argc < 6) {
    fprintf(stderr,
        "Usage: ./benchmark --node_num N --node_id ID --appt_num T --read_ratio R --locality L --sharing S [--output FILE]\n");
    exit(-1);
  }
}

int main(int argc, char **argv) {
  int node_id = 0;
  int node_num = 4;
  int appt_num = 4;
  int syst_num = 8;
  uint8_t fpga_pci_bus = 0x43;
  mac_t mac_vec[8];
  memset(mac_vec, 0, sizeof(mac_vec));
  
  parseArgs(argc, argv, node_id, node_num, appt_num, syst_num, fpga_pci_bus, mac_vec);
  
  fprintf(stdout,
          "Benchmark Config: nodeNR %d, threadNR %d, readNR %d, locality %d, "
          "sharing %d\n",
          nodeNR, threadNR, readNR, locality, sharing);
  
  uint64_t gmem_size_per_node = define::MB * 200;
  uint64_t cache_size_per_node = define::MB * 400;
  
  DSMConfig conf(node_id, node_num, appt_num, syst_num,
                 gmem_size_per_node, cache_size_per_node,
                 fpga_pci_bus, mac_vec);
  
  dsm = DSM::getInstance(conf);
  
  for (int i = 0; i < threadNR; ++i) {
    th[i] = new std::thread(init_trace, dsm->getMyNodeID(), i);
  }
  for (int i = 0; i < threadNR; ++i) {
    th[i]->join();
  }
  
  init.store(0);
  ready.store(0);
  finish.store(0);
  
  for (int i = 0; i < threadNR; ++i) {
    th[i] = new std::thread(start_thread, dsm->getMyNodeID(), i);
  }
  
  timespec s, e;
  
  while (ready.load() != threadNR);
  
  clock_gettime(CLOCK_REALTIME, &s);
  
  while (finish.load() != threadNR);
  
  clock_gettime(CLOCK_REALTIME, &e);
  
  double microseconds =
      (e.tv_sec - s.tv_sec) * 1000000 + (double)(e.tv_nsec - s.tv_nsec) / 1000;
  
  uint64_t ops = threadNR * OP_NUMS;
  uint64_t tp = ops * 1000000 / microseconds;
  
  fprintf(stderr, "Node %d: throughput %llu op/s\n", dsm->getMyNodeID(), tp);
  
  for (int i = 0; i < threadNR; ++i) {
    th[i]->join();
    delete[] access_[i];
  }
  
  if (output_file != nullptr && dsm->getMyNodeID() == 0) {
    std::ofstream f(std::string(output_file), std::ios::app);
    if (f) {
      f << "[" << nodeNR << "," << threadNR << "," << readNR << "," << locality
        << "," << sharing << "]" << std::endl;
      f << tp << " op/s\n" << std::endl;
    }
    f.close();
  }
  
  sleep(1);
  dsm->stop();
  
  return 0;
}
