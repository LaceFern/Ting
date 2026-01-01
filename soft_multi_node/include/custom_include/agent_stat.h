#pragma once
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <atomic>
#include <infiniband/verbs.h>
#include <boost/filesystem.hpp> 
#include <fstream>
#include <cstdio>
#include <regex>
#include <mutex>
#include <iostream>
#include <string>
#include "histogram.h"
#include "atomic_queue/atomic_queue.h"
#include "numautil.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include "Common.h"
#include "GlobalAddress.h"
#include "concurrentqueue.h"
#include "CacheStat.h"
#include "RawMessageConnection.h"
#include <iomanip>  
#include "Config.h"
class Monitor {
public:
    static uint64_t rdtsc() {
      unsigned int lo, hi;
      __asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
      return ((uint64_t)hi << 32) | lo;
    }
    static uint64_t rdtscp() {
      unsigned int lo, hi;
      __asm__ __volatile__("rdtscp" : "=a" (lo), "=d" (hi));
      return ((uint64_t)hi << 32) | lo;
    }
    void start() {
        start_time_ = rdtsc();
        has_logged_ = false; 
    }
    bool check(const std::string& message, double threshold_seconds) {
        auto now = rdtscp();
        auto elapsed = (now - start_time_) / CPU_CLOCK_HZ;
        bool res = false;
        if (elapsed > threshold_seconds && !has_logged_) {
            std::cout << "Timeout: " << message << " (Elapsed: " << elapsed << " seconds)\n" << std::endl;
            has_logged_ = true; 
            res = true;
        }
        return res;
    }
    static void wait(uint64_t microseconds) {
        uint64_t start_time = rdtsc();
        uint64_t end_time = start_time + static_cast<uint64_t>(microseconds * CPU_CLOCK_HZ / 1e6);
        while (rdtsc() < end_time) {
        }
    }
private:
    uint64_t start_time_; 
    bool has_logged_ = false; 
    static constexpr double CPU_CLOCK_HZ = 2.2e9;
};
enum class MEMACCESS_TYPE {
    WITH_CC,
    WITHOUT_CC,
    DONT_DISTINGUISH,
    _count,
};
enum class APP_THREAD_OP {
    NONE,
    AFTER_PROCESS_LOCAL_REQUEST_LOCK,
    AFTER_PROCESS_LOCAL_REQUEST_UNLOCK,
    AFTER_PROCESS_LOCAL_REQUEST_READ, 
    AFTER_PROCESS_LOCAL_REQUEST_WRITE, 
    WAIT_ASYNC_FINISH_LOCK,
    WAIT_ASYNC_FINISH_UNLOCK,
    WAIT_ASYNC_FINISH,
    WAKEUP_2_LOCK_RETURN,
    WAKEUP_2_UNLOCK_RETURN,
    WAKEUP_2_READ_RETURN, 
    WAKEUP_2_WRITE_RETURN, 
    MEMSET,
    _count,
};
namespace fs = boost::filesystem;
class agent_stats {
    using GAddr = uint64_t;
private:
    uint64_t memaccess_counter;
    MEMACCESS_TYPE memaccess_type;
    void save_clean_stat(std::string result_dir, std::string Tag){
        std::string common_suffix = ".txt";
        double result_99 = -1;
        double result_avg = -1;
        double result_count = -1;
        std::ifstream file;
        std::string file_name = result_dir + "/" + Tag + common_suffix;
        file.open(file_name);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file" << file_name << std::endl;
        }
        std::string line;
        std::getline(file, line); 
        std::getline(file, line); 
        while (std::getline(file, line)) {
            if (line[0] != '#') {
                std::istringstream iss(line);
                std::string word;
                std::vector<std::string> wordlist;
                while (iss >> word) {
                    wordlist.push_back(word);
                }
                double value = std::stod(wordlist[0]);
                double percentile = std::stod(wordlist[1]);
                if (percentile >= 0.99 && result_99 == -1) {
                    result_99 = value;
                }
            } else {
                std::smatch match;
                if (std::regex_search(line, match, std::regex(R"(\bMean\s*=\s*([-+]?\d*\.\d+|\d+))"))) {
                    result_avg = std::stod(match[1]);
                }
                if (std::regex_search(line, match, std::regex(R"(\bTotal count\s*=\s*([-+]?\d*\.\d+|\d+))"))) {
                    result_count = std::stod(match[1]);
                }
            }
        }
        file.close();
        int ret = remove(file_name.c_str());
        if (ret != 0) {
            std::cerr << "Error: Unable to delete file " << file_name << std::endl;
        }
        std::ofstream result;
        std::string result_name = result_dir + "/" + "clean_" + Tag + common_suffix;
        result.open(result_name, std::ios::app);
        if (!result.is_open()) {
            std::cerr << "Error: Unable to open file " << result_name << std::endl;
        }
        result << result_count << "\t" << result_avg << "\t" << result_99 << "\n";
        result.close();
    }    std::unordered_map<MEMACCESS_TYPE, Histogram *> memaccess_type_stats;
    volatile uint64_t app_thread_counter;
    std::unordered_map<APP_THREAD_OP, Histogram *> app_thread_op_stats;
    std::unordered_set <GAddr> valid_gaddrs;
    std::atomic<int> start;
    volatile int thread_init[48] = {0};
    std::mutex init_mtx;
    std::mutex app_mtx;
public:
    int is_request = 0;
    int is_cache = 0;
    int is_home = 0;
    int home_node_id = 0;
    CacheStat cc_stat;
    void clean_cc_stat(){
      cc_stat.clean();
    }
    void print_cc_stat(){
      std::cout << cc_stat.toString();
    }
    std::string return_cc_stat(){
      return cc_stat.toString();
    }
    void stall() {
        std::cout << "Press Space to continue..." << std::endl;
        char input;
        do {
            input = std::cin.get(); 
        } while (input != ' ');     
        std::cout << "Continuing execution..." << std::endl;
    }
    void printRaw(const void* ptr, size_t size) {
        const uint8_t* data = reinterpret_cast<const uint8_t*>(ptr);
        std::cout << "Raw Data (Hex Dump):" << std::endl;
        for (size_t i = 0; i < size; ++i) {
            if (i > 0 && i % 4 == 0) {
                std::cout << " "; 
            }
            std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') 
                      << static_cast<int>(data[size - 1 - i]);
        }
        std::cout << std::endl;
    }
    void print_msg(RawMessage msg) {
        printf("  mtype: %u\n", msg.mtype);
        printf("  req_cline_nodeID: %u\n", msg.req_cline_nodeID);
        printf("  req_cline_appThreadID: %u\n", msg.req_cline_appThreadID);
        printf("  req_cline_addr: 0x%lx\n", msg.req_cline_addr);
        printf("  req_cline_index: %u\n", msg.req_cline_index);
        printf("  cline_tag: 0x%lx\n", msg.cline_tag);
        printf("  agent_cline_nodeID: %u\n", msg.agent_cline_nodeID);
        printf("  agent_cline_addr: 0x%lx\n", msg.agent_cline_addr);
        printf("  dir_gblock_addr: 0x%lx\n", msg.dir_gblock_addr);
        printf("  dir_gblock_index: %u\n", msg.dir_gblock_index);
        printf("  dir_gblock_bitmap: ");
        for (int i = 0; i < max_node_num; ++i) {
            printf("%d", msg.dir_gblock_bitmap[i]);
        }
        printf("\n");
        printf("  dir_gblock_nodeID: %u\n", msg.dir_gblock_nodeID);
        printf("  general_c2h_owner: %u\n", msg.general_c2h_owner);
    }
    void set1_thread_init_flag(int id){
        std::lock_guard<std::mutex> lock(init_mtx);
        thread_init[id] = 1;
    }
    int read_thread_init_flag(int id){
        std::lock_guard<std::mutex> lock(init_mtx);
        return thread_init[id];
    }
    explicit agent_stats() {
        for(int j = 0; j < static_cast<int>(MEMACCESS_TYPE::_count); j++){
            memaccess_type_stats[(MEMACCESS_TYPE) j] = new Histogram(1, 10000000, 3, 10);
        }
        for(int j = 0; j < static_cast<int>(APP_THREAD_OP::_count); j++){
            app_thread_op_stats[(APP_THREAD_OP) j] = new Histogram(1, 10000000, 3, 10);
        }
    }
    ~agent_stats() {
    }
    void print_app_thread_stat() {
        std::cout << "\nAFTER_PROCESS_LOCAL_REQUEST_LOCK: " << std::endl;
        app_thread_op_stats[APP_THREAD_OP::AFTER_PROCESS_LOCAL_REQUEST_LOCK]->print(stdout, 5);
        std::cout << "\nAFTER_PROCESS_LOCAL_REQUEST_UNLOCK: " << std::endl;
        app_thread_op_stats[APP_THREAD_OP::AFTER_PROCESS_LOCAL_REQUEST_UNLOCK]->print(stdout, 5);
        std::cout << "\nAFTER_PROCESS_LOCAL_REQUEST_READ: " << std::endl;
        app_thread_op_stats[APP_THREAD_OP::AFTER_PROCESS_LOCAL_REQUEST_READ]->print(stdout, 5);
        std::cout << "\nAFTER_PROCESS_LOCAL_REQUEST_WRITE: " << std::endl;
        app_thread_op_stats[APP_THREAD_OP::AFTER_PROCESS_LOCAL_REQUEST_WRITE]->print(stdout, 5);
        std::cout << "\nWAIT_ASYNC_FINISH: " << std::endl;
        app_thread_op_stats[APP_THREAD_OP::WAIT_ASYNC_FINISH]->print(stdout, 5);
        std::cout << "\nWAIT_ASYNC_FINISH_LOCK: " << std::endl;
        app_thread_op_stats[APP_THREAD_OP::WAIT_ASYNC_FINISH_LOCK]->print(stdout, 5);
        std::cout << "\nWAIWAIT_ASYNC_FINISH_UNLOCK: " << std::endl;
        app_thread_op_stats[APP_THREAD_OP::WAIT_ASYNC_FINISH_UNLOCK]->print(stdout, 5);
        std::cout << "\nWAKEUP_2_READ_RETURN: " << std::endl;
        app_thread_op_stats[APP_THREAD_OP::WAKEUP_2_READ_RETURN]->print(stdout, 5);
        std::cout << "\nWAKEUP_2_WRITE_RETURN: " << std::endl;
        app_thread_op_stats[APP_THREAD_OP::WAKEUP_2_WRITE_RETURN]->print(stdout, 5);
        std::cout << "\nWAKEUP_2_LOCK_RETURN: " << std::endl;
        app_thread_op_stats[APP_THREAD_OP::WAKEUP_2_LOCK_RETURN]->print(stdout, 5);
        std::cout << "\nWAKEUP_2_UNLOCK_RETURN: " << std::endl;
        app_thread_op_stats[APP_THREAD_OP::WAKEUP_2_UNLOCK_RETURN]->print(stdout, 5);
    }
    void save_stat_to_file(std::string result_dir) {
        std::string common_suffix = ".txt";
        if (!fs::exists(result_dir)) {
            if (!fs::create_directory(result_dir)) {
                std::cerr << "Error creating folder " << result_dir << std::endl;
                exit(1);
            }
        }
        FILE *file;
        fs::path result_directory(result_dir);
        fs::path filePath;
        filePath = result_directory / fs::path("WITH_CC" + common_suffix);
        file = fopen(filePath.c_str(), "w");
        assert(file != nullptr);
        memaccess_type_stats[MEMACCESS_TYPE::WITH_CC]->print(file, 5);
        fclose(file);
        filePath = result_directory / fs::path("WITHOUT_CC" + common_suffix);
        file = fopen(filePath.c_str(), "w");
        assert(file != nullptr);
        memaccess_type_stats[MEMACCESS_TYPE::WITHOUT_CC]->print(file, 5);
        fclose(file);
        filePath = result_directory / fs::path("DONT_DISTINGUISH" + common_suffix);
        file = fopen(filePath.c_str(), "w");
        assert(file != nullptr);
        memaccess_type_stats[MEMACCESS_TYPE::DONT_DISTINGUISH]->print(file, 5);
        fclose(file);
        save_clean_stat(result_dir, "WITH_CC");
        save_clean_stat(result_dir, "WITHOUT_CC");
        save_clean_stat(result_dir, "DONT_DISTINGUISH");
        if(is_request){
            filePath = result_directory / fs::path("AFTER_PROCESS_LOCAL_REQUEST_LOCK" + common_suffix);
            file = fopen(filePath.c_str(), "w");
            assert(file != nullptr);
            app_thread_op_stats[APP_THREAD_OP::AFTER_PROCESS_LOCAL_REQUEST_LOCK]->print(file, 5);
            fclose(file);
            save_clean_stat(result_dir, "AFTER_PROCESS_LOCAL_REQUEST_LOCK");
            filePath = result_directory / fs::path("AFTER_PROCESS_LOCAL_REQUEST_UNLOCK" + common_suffix);
            file = fopen(filePath.c_str(), "w");
            assert(file != nullptr);
            app_thread_op_stats[APP_THREAD_OP::AFTER_PROCESS_LOCAL_REQUEST_UNLOCK]->print(file, 5);
            fclose(file);
            save_clean_stat(result_dir, "AFTER_PROCESS_LOCAL_REQUEST_UNLOCK");
            filePath = result_directory / fs::path("AFTER_PROCESS_LOCAL_REQUEST_READ" + common_suffix);
            file = fopen(filePath.c_str(), "w");
            assert(file != nullptr);
            app_thread_op_stats[APP_THREAD_OP::AFTER_PROCESS_LOCAL_REQUEST_READ]->print(file, 5);
            fclose(file);
            save_clean_stat(result_dir, "AFTER_PROCESS_LOCAL_REQUEST_READ");
            filePath = result_directory / fs::path("AFTER_PROCESS_LOCAL_REQUEST_WRITE" + common_suffix);
            file = fopen(filePath.c_str(), "w");
            assert(file != nullptr);
            app_thread_op_stats[APP_THREAD_OP::AFTER_PROCESS_LOCAL_REQUEST_WRITE]->print(file, 5);
            fclose(file);
            save_clean_stat(result_dir, "AFTER_PROCESS_LOCAL_REQUEST_WRITE");
            filePath = result_directory / fs::path("WAIT_ASYNC_FINISH" + common_suffix);
            file = fopen(filePath.c_str(), "w");
            assert(file != nullptr);
            app_thread_op_stats[APP_THREAD_OP::WAIT_ASYNC_FINISH]->print(file, 5);
            fclose(file);
            save_clean_stat(result_dir, "WAIT_ASYNC_FINISH");
            filePath = result_directory / fs::path("WAKEUP_2_READ_RETURN" + common_suffix);
            file = fopen(filePath.c_str(), "w");
            assert(file != nullptr);
            app_thread_op_stats[APP_THREAD_OP::WAKEUP_2_READ_RETURN]->print(file, 5);
            fclose(file);
            save_clean_stat(result_dir, "WAKEUP_2_READ_RETURN");
            filePath = result_directory / fs::path("WAKEUP_2_WRITE_RETURN" + common_suffix);
            file = fopen(filePath.c_str(), "w");
            assert(file != nullptr);
            app_thread_op_stats[APP_THREAD_OP::WAKEUP_2_WRITE_RETURN]->print(file, 5);
            fclose(file);
            save_clean_stat(result_dir, "WAKEUP_2_WRITE_RETURN");
            filePath = result_directory / fs::path("WAKEUP_2_LOCK_RETURN" + common_suffix);
            file = fopen(filePath.c_str(), "w");
            assert(file != nullptr);
            app_thread_op_stats[APP_THREAD_OP::WAKEUP_2_LOCK_RETURN]->print(file, 5);
            fclose(file);
            save_clean_stat(result_dir, "WAKEUP_2_LOCK_RETURN");
            filePath = result_directory / fs::path("WAKEUP_2_UNLOCK_RETURN" + common_suffix);
            file = fopen(filePath.c_str(), "w");
            assert(file != nullptr);
            app_thread_op_stats[APP_THREAD_OP::WAKEUP_2_UNLOCK_RETURN]->print(file, 5);
            fclose(file);
            save_clean_stat(result_dir, "WAKEUP_2_UNLOCK_RETURN");
            filePath = result_directory / fs::path("MEMSET" + common_suffix);
            file = fopen(filePath.c_str(), "w");
            assert(file != nullptr);
            app_thread_op_stats[APP_THREAD_OP::MEMSET]->print(file, 5);
            fclose(file);
            save_clean_stat(result_dir, "MEMSET");
        }
    }
    bool is_valid_gaddr_without_start(GAddr gaddr) {
        return valid_gaddrs.count(gaddr);
    }
    bool is_valid_gaddr(GAddr gaddr) {
        if(!start){
            return false;
        }
        else{
            return valid_gaddrs.count(gaddr);
        }
    }
    void push_valid_gaddr(GAddr gaddr) {
        valid_gaddrs.insert(gaddr);
    }
    void print_valid_gaddr() {
        printf("valid Gaddr:");
        for (const auto& gaddr : valid_gaddrs) {
            std::cout << gaddr << std::endl;
        }
    }
    void pop_valid_gaddr(GAddr gaddr) {
        valid_gaddrs.erase(gaddr);
    }
    void start_collection() {
        start = 1;
    }
    void end_collection() {
        start = 0;
    }
    bool is_start() {
        return start;
    }
    inline void start_record_with_memaccess_type() {
        if (start) {
            app_thread_counter = Monitor::rdtsc();
        }
    }
    inline void set_memaccess_type(MEMACCESS_TYPE type = MEMACCESS_TYPE::DONT_DISTINGUISH) {
        if (start) {
            memaccess_type = type;
        }
    }
    inline void stop_record_with_memaccess_type() {
        if (start) {
            uint64_t ns = Monitor::rdtscp() - app_thread_counter;
            memaccess_type_stats[memaccess_type]->record(ns);
            memaccess_type_stats[MEMACCESS_TYPE::DONT_DISTINGUISH]->record(ns);
        }
    }
    inline void start_record_app_thread(GAddr gaddr) {
        if (is_valid_gaddr(gaddr) && start) {
            std::lock_guard<std::mutex> lock(app_mtx);
            app_thread_counter = Monitor::rdtsc();
        }
    }
    inline void stop_record_app_thread_with_op(GAddr gaddr, APP_THREAD_OP op = APP_THREAD_OP::NONE) {
        if (is_valid_gaddr(gaddr) && start) {
            std::lock_guard<std::mutex> lock(app_mtx);
            uint64_t ns = Monitor::rdtscp() - app_thread_counter;
            app_thread_op_stats[op]->record(ns);
        }
    }
};      
extern agent_stats agent_stats_inst;