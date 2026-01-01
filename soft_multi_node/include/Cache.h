#ifndef __CACHE_H__
#define __CACHE_H__
#include "CacheStat.h"
#include "CacheStatus.h"
#include "Common.h"
#include "GlobalAddress.h"
#include <unordered_map>
#include "agent_stat.h"
#include "concurrentqueue.h"
#include "WRLock.h"
#include "channel_context.h"
#include "LocalAllocator.h"
class DSM;
class DSMConfig;
union AtomicTag {
  struct {
    bool flag;
    CacheStatus status;
  };
  uint64_t v;
  AtomicTag() noexcept : v(0) {}
  AtomicTag(bool f, CacheStatus c) noexcept : v(0) {
    this->flag = f;
    this->status = c;
  }
  bool isUnstable() {
    return status == CacheStatus::TO_MODIFIED || status == CacheStatus::TO_SHARED || status == CacheStatus::TO_INVALID;
  }
} __attribute__((packed));
struct LineInfo {
  WRLock lock;
  void *data;
  std::atomic<AtomicTag> status;
  int line_index;
  bool shadow_flag = false;
  uint64_t tag;
  GlobalAddress addr;
  uint32_t set_index;
  uint32_t slot_index;
  void setFlagAndStatus(bool flag, CacheStatus s, std::memory_order order = std::memory_order_relaxed) {
    status.store(AtomicTag{flag, s}, order);
  }
  bool casFlagAndStatus(bool flag_from, CacheStatus s_from,
                        bool flag_to, CacheStatus s_to) {
    AtomicTag a{flag_from, s_from};
    AtomicTag b{flag_to, s_to};
    return status.compare_exchange_strong(a, b);
  }
  bool casStatus(CacheStatus s_from, CacheStatus s_to) {
    auto tmp_flag = status.load(std::memory_order_relaxed).flag;
    AtomicTag a{tmp_flag, s_from};
    AtomicTag b{tmp_flag, s_to};
    return status.compare_exchange_strong(a, b);
  }
  bool getFlag(std::memory_order order = std::memory_order_relaxed) {
    return status.load(order).flag;
  }
  CacheStatus getStatus(std::memory_order order = std::memory_order_relaxed) {
    return status.load(order).status;
  }
  bool isStable() {
    auto tmp_status = getStatus();
    return tmp_status == CacheStatus::INVALID ||
           tmp_status == CacheStatus::SHARED ||
           tmp_status == CacheStatus::MODIFIED;
  }
};
class Cache {
public:
  friend class DSM;
  friend class CacheAgent;
private:
  Cache(const DSMConfig &conf, DSM *dsm);
  void initLine(SlabAllocator& cline_data_sb);
  void readLine(const GlobalAddress &addr, uint16_t start, uint16_t size,
                void *to);
  void writeLine(const GlobalAddress &addr, uint16_t start, uint16_t size,
                 const void *from);
  bool findLine(const GlobalAddress &addr, LineInfo *&line);
  bool findLineForAgent(uint64_t tag, LineInfo *&line, int agent_id);
  bool readMiss(const GlobalAddress &addr, LineInfo *info);
  bool writeMiss(const GlobalAddress &addr, LineInfo *info);
  bool writeShared(const GlobalAddress &addr, LineInfo *info);
  RawMessage generate_message(RawMessageType mtype, const GlobalAddress &addr, LineInfo *info);
  void start_evict();
  void wait_evict();
  uint32_t calLineSet(const uint64_t tag);
  bool findLine(uint32_t set_idx, const GlobalAddress &addr, LineInfo *&cline);
  bool findShadowLine(uint32_t set_idx, const GlobalAddress &addr, LineInfo *&cline);
  bool newLine(uint32_t set_idx, const GlobalAddress &addr, LineInfo *&cline);
  void print_cline(LineInfo* line) {
      if(line == nullptr){
          printf("  null pointer\n");
      }
      else{
          printf("  tag = 0x%x\n", line->tag);
          printf("  status = %d\n", line->getStatus());
          printf("  flag = %d\n", line->getFlag());
      }
  }
  DSMConfig conf;
  DSM *dsm;
  ~Cache();
  std::vector<WRLock> cline_set_rwlock_vector{max_cline_set_depth_per_node};
  std::vector<std::array<LineInfo, max_cline_set_width_per_node>> cline_set_vector{max_cline_set_depth_per_node};
  std::vector<std::array<LineInfo, max_cline_set_width_per_node>> shadow_cline_set_vector{max_cline_set_depth_per_node};
  std::vector<void *> available_data_stack[max_appt_num_per_node];
  moodycamel::ConcurrentQueue<LineInfo*> available_queue;
  static thread_local bool start_next_evict_cline_flag;
  static thread_local bool wait_cur_evict_cline_flag;
  static thread_local LineInfo * cur_evict_cline;
  static thread_local LineInfo * next_evict_cline;
  static thread_local bool recv_cur_evict_msg_flag;
  static thread_local RawMessage cur_evict_msg;
  static thread_local EvictType cur_evict_cline_type;
  static thread_local int req_appt_id;
  static thread_local channel_context *channel;
  static thread_local int txn_count;
  static thread_local int ws_only_count;
  static thread_local int ws_multi_count;
  static thread_local RawMessage last_sendmsg;
  static thread_local bool last_cnodebitmap[max_node_num];
  static thread_local bool last_recvbitmap[max_node_num];
  static thread_local RawMessage last_recvmsg[max_node_num];
  static thread_local RawMessage last_one_recvmsg;
  static thread_local CacheStatus debug_status[max_cline_set_width_per_node];
  static thread_local CacheStatus debug_shadow_status[max_cline_set_width_per_node];
  static thread_local uint64_t debug_tag[max_cline_set_width_per_node];
  static thread_local uint64_t debug_shadow_tag[max_cline_set_width_per_node];
  static thread_local GlobalAddress trigger_evict_addr;
  static thread_local uint32_t seed;
  static thread_local uint32_t next_allocator_node;
  static thread_local LocalAllocator* alloc;
  GlobalAddress malloc(size_t size, bool align = false);
  void free(const GlobalAddress &addr);
};
#endif 
