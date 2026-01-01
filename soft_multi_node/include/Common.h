#ifndef __COMMON_H__
#define __COMMON_H__
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <bitset>
#include <vector>
#include "Debug.h"
#define NUMA_CORE_NUM 12
#define MAX_MACHINE 8
#define ADD_ROUND(x, n) ((x) = ((x) + 1) % (n))
#define MESSAGE_SIZE 96 
#define NIC_NUMA_NODE 0
namespace define {
constexpr uint64_t KB = 1024ull * 1;
constexpr uint64_t MB = 1024ull * 1024;
constexpr uint64_t GB = 1024ull * MB;
constexpr uint64_t kChunkSize = MB * 32;
} 
struct mac_t {
    uint8_t mac_addr[6];
};
#define DSM_CACHE_LINE_WIDTH (12) 
#define DSM_CACHE_LINE_SIZE (1u << DSM_CACHE_LINE_WIDTH)
#define DSM_CACHE_INDEX_WIDTH (16)
#define DSM_CACHE_INDEX_SIZE (1u << DSM_CACHE_INDEX_WIDTH)
#define CACHE_WAYS (8)
const int max_node_num = 8;
const int max_appt_num_per_node = 24;
const int max_syst_num_per_node = 8;
const int max_cline_num_per_node = 5000000;
const int max_gblock_num_per_node = 5000000;
const int max_cline_set_depth_per_node = DSM_CACHE_INDEX_SIZE;
const int max_cline_set_width_per_node = CACHE_WAYS;
enum RawMessageType : uint8_t {
  R_DEBUG = 0,
  R_READ_MISS = 1,
  R_WRITE_MISS = 2,
  R_WRITE_SHARED = 3,
  R_EVICT_SHARED,
  R_EVICT_DIRTY,
  DIR_2_APP_READ_MISS_ACKDATA = 11, 
  DIR_2_APP_WRITE_MISS_ACKDATA,
  DIR_2_APP_WRITE_MISS_SHAREDDATA,
  DIR_2_APP_WRITE_SHARED_ONLYACK,
  DIR_2_APP_EVICT_SHARED_ACK,
  DIR_2_APP_EVICT_DIRTY_ACK,
  DIR_2_APP_M_CHECK_FAIL,
  DIR_2_APP_BITMAP_CHECK_FAIL,
  DIR_2_AGENT_READ_MISS_DIRTY = 21,
  DIR_2_AGENT_WRITE_MISS_DIRTY,
  DIR_2_AGENT_WRITE_MISS_SHARED,
  DIR_2_AGENT_WRITE_SHARED_NOTONLY,
  AGENT_2_DIR_READ_MISS_DIRTY_ACKDATA=31,
  AGENT_2_DIR_WRITE_MISS_DIRTY_ACK,
  AGENT_2_DIR_WRITE_MISS_SHARED_ACK,
  AGENT_2_DIR_WRITE_SHARED_NOTONLY_ACK,
  AGENT_2_DIR_M_CHECK_FAIL,
  AGENT_2_APP_READ_MISS_DIRTY_ACKDATA=41,
  AGENT_2_APP_WRITE_MISS_DIRTY_ACKDATA,
  DIR_2_APP_WRITE_MISS_SHARED_AGGACK_ACK=51,
  DIR_2_APP_WRITE_SHARED_NOTONLY_AGGACK_ACK,
  INIT_DIR,
  INIT_NET,
};
enum RawState : uint8_t {
  S_UNSHARED = 0,
  S_SHARED = 1,
  S_DIRTY = 2,
  S_TO_UNSHARED,
  S_TO_SHARED,
  S_TO_DIRTY,
};
enum CacheStatus : uint8_t {
  INVALID = 0,
  SHARED = 1,
  MODIFIED = 2,
  TO_INVALID = 3,
  TO_SHARED,
  TO_MODIFIED,
  PRESERVED,
  TO_PRESERVED
};
enum CacheOrganizaion : uint8_t {
  FULLY_ASSOCIATIVE = 0,
  SET_ASSOCIATIVE = 1,
};
enum EvictType : uint8_t {
  EVICT_SHARED = 0,
  EVICT_DIRTY = 1,
};
#endif 
