#ifndef _GALLOC_H_
#define _GALLOC_H_
#include "DSM.h"
#include "log.h"
#include <string>
using GAlloc = DSM;
using GAddr = GlobalAddress;
struct Conf {
    bool is_master = true; 
    int master_port = 12345;
    std::string master_ip = "localhost";
    std::string master_bindaddr;
    int worker_port = 12346;
    std::string worker_bindaddr;
    std::string worker_ip = "localhost";
    size_t size = 1024 * 1024L * 512; 
    size_t ghost_th = 1024 * 1024;
    double cache_th = 0.15; 
    int unsynced_th = 1;
    double factor = 1.25;
    int maxclients = 1024;
    int maxthreads = 10;
    int loglevel = LOG_DEBUG;
    std::string *logfile = nullptr;
    int timeout = 10;          
    int eviction_period = 100; 
    int cluster_size = 0;
};
class GAllocFactory {
private:
    static Conf *conf;
public:
    static const int appt_num_per_node = 24;
    static const int obj_size = 4096;
    static const uint64_t obj_aligned_size = 4096;
    static const uint64_t workload_size_per_appt = uint64_t(define::MB * 4);
    static constexpr float cache_ratio = 2.0f;  
    static const int read_ratio = 60;
    static const int sharing_ratio = 50;
    static std::string *LogFile() {
        return conf->logfile;
    }
    static int LogLevel() { 
        return LOG_INFO; 
    }
    static int64_t GetGmemSizePerNode() {
        return workload_size_per_appt * appt_num_per_node + 
               workload_size_per_appt * appt_num_per_node;
    }
    static uint64_t GetCacheSizePerNode() {
        return GetGmemSizePerNode() * cache_ratio;
    }
    static DSM *CreateAllocator(Conf *gam_conf) {
        conf = gam_conf;
        int node_id = 8;
        int node_num = 8;
        int appt_num_per_node_local = 24;
        int syst_num_per_node = max_syst_num_per_node;
        uint8_t fpga_pci_bus = 0x43;
        mac_t mac_vec[8];
        DSMConfig c(node_id,
            node_num,
            appt_num_per_node_local,
            syst_num_per_node,
            GetGmemSizePerNode(),     
            GetCacheSizePerNode(),    
            fpga_pci_bus,
            mac_vec);
        auto dsm = DSM::getInstance(c);
        return dsm;
    }
};
#endif