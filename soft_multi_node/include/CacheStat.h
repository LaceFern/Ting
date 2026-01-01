#ifndef __CACHESTAT_H__
#define __CACHESTAT_H__ 
#include <cstdint>
#include <cstring>
#include <string>
#include <cmath>
#include "Common.h"
struct CacheStat {
    uint64_t total_access[max_appt_num_per_node];
    uint64_t hit[max_appt_num_per_node];
    uint64_t write_miss_shared_copies[max_appt_num_per_node];
    uint64_t write_shared_copies[max_appt_num_per_node];
    double write_miss_shared_copies_avg;
    double write_shared_copies_avg;
    uint64_t read_miss_unshared_or_shared[max_appt_num_per_node];
    uint64_t read_miss_dirty[max_appt_num_per_node];
    uint64_t read_miss_check_fail[max_appt_num_per_node];
    uint64_t write_miss_unshared[max_appt_num_per_node];
    uint64_t write_miss_shared[max_appt_num_per_node];
    uint64_t write_miss_dirty[max_appt_num_per_node];
    uint64_t write_miss_check_fail[max_appt_num_per_node];
    uint64_t write_shared[max_appt_num_per_node];
    uint64_t write_shared_check_fail[max_appt_num_per_node];
    uint64_t evict_invalid[max_appt_num_per_node];
    uint64_t evict_shared[max_appt_num_per_node];
    uint64_t evict_modified[max_appt_num_per_node];
    CacheStat() {
        memset(this, 0, sizeof(*this));
    }
    void clean(){
        memset(this, 0, sizeof(*this));
    }
    uint64_t sum(uint64_t count_arr[max_appt_num_per_node]){
        uint64_t sum = 0;
        for(int i = 0; i < max_appt_num_per_node; i++){
            sum += count_arr[i];
        }
        return sum;
    }
    std::string toString() {
        if(write_miss_shared != 0){
            write_miss_shared_copies_avg = 1.0 * sum(write_miss_shared_copies) / sum(write_miss_shared);
        }
        else{
            write_miss_shared_copies_avg = 0;
        }
        if(write_shared != 0){
            write_shared_copies_avg = 1.0 * sum(write_shared_copies) / sum(write_shared);
        }
        else{
            write_shared_copies_avg = 0;
        }
        uint64_t hit_cal = (sum(total_access) - sum(read_miss_unshared_or_shared) - sum(read_miss_dirty) - sum(write_miss_unshared) - sum(write_miss_shared) - sum(write_miss_dirty) - sum(write_shared));
        std::string s = "";
        s.append("total_access:\t" + std::to_string(sum(total_access)) + "\n");
        s.append("hit(count):\t" + std::to_string(sum(hit)) + "\n");
        s.append("hit(cal):\t" + std::to_string(hit_cal) + "\n");
        s.append("write_miss_shared_copies(avg):\t" + std::to_string(write_miss_shared_copies_avg) + "\n");
        s.append("write_shared_copies(avg):\t" + std::to_string(write_shared_copies_avg) + "\n");
        s.append("read_miss_unshared_or_shared:\t" + std::to_string(sum(read_miss_unshared_or_shared)) + "\n");
        s.append("read_miss_dirty:\t" + std::to_string(sum(read_miss_dirty)) + "\n");
        s.append("read_miss_check_fail:\t" + std::to_string(sum(read_miss_check_fail)) + "\n");
        s.append("write_miss_unshared:\t" + std::to_string(sum(write_miss_unshared)) + "\n");
        s.append("write_miss_shared:\t" + std::to_string(sum(write_miss_shared)) + "\n");
        s.append("write_miss_dirty:\t" + std::to_string(sum(write_miss_dirty)) + "\n");
        s.append("write_miss_check_fail:\t" + std::to_string(sum(write_miss_check_fail)) + "\n");
        s.append("write_shared:\t" + std::to_string(sum(write_shared)) + "\n");
        s.append("write_shared_check_fail:\t" + std::to_string(sum(write_shared_check_fail)) + "\n");
        s.append("evict_invalid:\t" + std::to_string(sum(evict_invalid)) + "\n");
        s.append("evict_shared:\t" + std::to_string(sum(evict_shared)) + "\n");
        s.append("evict_modified:\t" + std::to_string(sum(evict_modified)) + "\n");
        return s;
    }
};
#endif 
