#include <iostream>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include "DSM.h"

DSM *dsm;

void parseArgs(int argc, char **argv, int &node_id, int &node_num, int &appt_num, 
               int &syst_num, uint8_t &fpga_pci_bus, mac_t *mac_vec) {
  node_id = 0;
  node_num = 3;
  appt_num = 1;
  syst_num = 8;
  fpga_pci_bus = 0x43;
  
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--node_num") == 0) {
      node_num = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--node_id") == 0) {
      node_id = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--appt_num") == 0) {
      appt_num = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--syst_num") == 0) {
      syst_num = atoi(argv[++i]);
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

int main(int argc, char **argv) {
  int node_id = 0;
  int node_num = 3;
  int appt_num = 1;
  int syst_num = 8;
  uint8_t fpga_pci_bus = 0x43;
  mac_t mac_vec[8];
  memset(mac_vec, 0, sizeof(mac_vec));
  
  parseArgs(argc, argv, node_id, node_num, appt_num, syst_num, fpga_pci_bus, mac_vec);
  
  uint64_t gmem_size_per_node = define::MB * 200;
  uint64_t cache_size_per_node = define::MB * 400;
  
  DSMConfig conf(node_id, node_num, appt_num, syst_num,
                 gmem_size_per_node, cache_size_per_node,
                 fpga_pci_bus, mac_vec);
  
  dsm = DSM::getInstance(conf);
  dsm->registerThread();
  
  if (dsm->getMyNodeID() == 1) {
    auto res = dsm->malloc(kChunkSize);
    printf("Allocated chunk 1: nodeID=%d, addr=0x%lx\n", res.nodeID, res.addr);
    
    res = dsm->malloc(kChunkSize);
    printf("Allocated chunk 2: nodeID=%d, addr=0x%lx\n", res.nodeID, res.addr);
    
    res = dsm->malloc(kChunkSize);
    printf("Allocated chunk 3: nodeID=%d, addr=0x%lx\n", res.nodeID, res.addr);
    
    res = dsm->malloc(10247);
    printf("Allocated 10247 bytes: nodeID=%d, addr=0x%lx\n", res.nodeID, res.addr);
    
    res = dsm->malloc(11);
    printf("Allocated 11 bytes: nodeID=%d, addr=0x%lx\n", res.nodeID, res.addr);
  }
  
  std::cout << "Allocation test completed\n";
  
  sleep(10);
  dsm->stop();
  return 0;
}
