#ifndef __CONFIG_H__
#define __CONFIG_H__
#include "Common.h"
class CacheConfig {
public:
    uint32_t cacheSize;
};
class DSMConfig {
   public:
    int node_id;
    int node_num;
    int appt_num_per_node;
    int syst_num_per_node;
    mac_t mac_vec[max_node_num] = {0};
    uint64_t gmem_size_per_node;
    uint64_t cache_size_per_node;
    int cline_num_per_node;
    int gblock_num_per_node;
    uint16_t    local_ip = 12345; 
    std::string local_port = "10.0.0.1"; 
    uint8_t     fpga_pci_bus = 0x43;
    size_t      fpga_bridge_bar_size = uint64_t(define::MB * 4);
    size_t      cpu_pool_size;
    double      factor = 1;
    DSMConfig(int u_node_id,
            int u_node_num, 
            int u_appt_num_per_node, 
            int u_syst_num_per_node, 
            uint64_t u_gmem_size_per_node, 
            uint64_t u_cache_size_per_node,
            uint8_t  u_fpga_pci_bus,
            mac_t u_mac_vec[max_node_num]
            ){
        node_id = u_node_id;        
        node_num = u_node_num;
        appt_num_per_node = u_appt_num_per_node;
        syst_num_per_node = u_syst_num_per_node;
        cline_num_per_node = u_cache_size_per_node / DSM_CACHE_LINE_SIZE;
        gblock_num_per_node = u_gmem_size_per_node / DSM_CACHE_LINE_SIZE;
        gmem_size_per_node = (u_gmem_size_per_node > define::MB * 200) ? u_gmem_size_per_node : define::MB * 200;
        cache_size_per_node = (u_cache_size_per_node > define::MB * 200) ? u_cache_size_per_node : define::MB * 200;
        gmem_size_per_node = (gmem_size_per_node / (define::MB * 1) + 1) * (define::MB * 1); 
        cache_size_per_node = (cache_size_per_node / (define::MB * 1) + 1) * (define::MB * 1);
        fpga_pci_bus = u_fpga_pci_bus;
        for(int i = 0; i < u_node_num; i++){
            mac_vec[i].mac_addr[0] = u_mac_vec[i].mac_addr[0];
            mac_vec[i].mac_addr[1] = u_mac_vec[i].mac_addr[1];
            mac_vec[i].mac_addr[2] = u_mac_vec[i].mac_addr[2];
            mac_vec[i].mac_addr[3] = u_mac_vec[i].mac_addr[3];
            mac_vec[i].mac_addr[4] = u_mac_vec[i].mac_addr[4];
            mac_vec[i].mac_addr[5] = u_mac_vec[i].mac_addr[5];
        }
        cpu_pool_size = 0;
        cpu_pool_size += 1 * define::GB; 
        cpu_pool_size += gmem_size_per_node;
        cpu_pool_size += cache_size_per_node;
        cpu_pool_size += 1 * define::GB; 
        printf("node_id = %d\n", node_id);
        printf("node_num = %d\n", node_num);
        printf("appt_num_per_node = %d\n", appt_num_per_node);
        printf("syst_num_per_node = %d\n", syst_num_per_node);
        printf("gmem_size_per_node = %lu\n", gmem_size_per_node);
        printf("cache_size_per_node = %lu\n", cache_size_per_node);
        printf("cline_num_per_node = %d\n", cline_num_per_node);
        printf("gblock_num_per_node = %d\n", gblock_num_per_node);
        printf("cpu_pool_size = %lu\n", cpu_pool_size);
        printf("sizeof(bool) = %ld\n",sizeof(bool));
        assert(node_num <= max_node_num);
        assert(appt_num_per_node <= max_appt_num_per_node);
        assert(syst_num_per_node <= max_syst_num_per_node);
        assert(cline_num_per_node <= max_cline_num_per_node);
        assert(gblock_num_per_node <= max_gblock_num_per_node);
        assert((gmem_size_per_node + cache_size_per_node + 1 * define::GB) <= cpu_pool_size);
        assert(fpga_bridge_bar_size >= 1ull * (appt_num_per_node + syst_num_per_node) * 64);
    }
};
#endif 
