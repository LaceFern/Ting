#ifndef __DSM_H__
#define __DSM_H__
#include <atomic>
#include "Cache.h"
#include "Config.h"
#include "GlobalAddress.h"
#include "channel_context.h"
#include "GlobalAllocator.h"
class CacheAgent;
class Directory;
class DSM {
  friend class Cache;
public:
  static DSM *getInstance(const DSMConfig &conf);
  void registerThread();
  void deleteThread();
  void read(const GlobalAddress &addr, uint32_t size, uint8_t *to);
  void write(const GlobalAddress &addr, uint32_t size, const uint8_t *from);
  void UnLock(const GlobalAddress addr, const size_t count) {;}
  int Try_RLock(const GlobalAddress addr, const size_t count) {return 0;}
  int Try_WLock(const GlobalAddress addr, const size_t count) {return 0;}
  bool r_lock(const GlobalAddress &addr, uint32_t size = 1) {;}
  void r_unlock(const GlobalAddress &addr, uint32_t size = 1) {;}
  bool w_lock(const GlobalAddress &addr, uint32_t size = 1) {;}
  void w_unlock(const GlobalAddress &addr, uint32_t size = 1) {;}
  GlobalAddress malloc(size_t size, bool align = false);
  void free(const GlobalAddress &addr);
  void drain_eviction();
  void stop();
  uint16_t getMyNodeID() { return conf.node_id; }
  uint16_t getMyThreadID() { return Cache::req_appt_id; }
  void print_config();
  GlobalAddress AlignedMalloc(size_t size) { return this->malloc(size, true); };
  GlobalAddress Malloc(size_t size) { return this->malloc(size); };
  void Free(const GlobalAddress addr) { this->free(addr); }
  int Read(const GlobalAddress addr, void *buf, const size_t count) {
    this->read(addr, count, (uint8_t *)buf);
    return count;
  }
  int Read(const GlobalAddress addr, const size_t offset, void *buf,
           const size_t count) {
    this->read(GADD(addr, offset), count, (uint8_t *)buf);
    return count;
  }
  int Write(const GlobalAddress addr, void *buf, const size_t count) {
    this->write(addr, count, (uint8_t *)buf);
    return count;
  }
  int Write(const GlobalAddress addr, const size_t offset, void *buf,
           const size_t count) {
    this->write(GADD(addr, offset), count, (uint8_t *)buf);
    return count;
  }
  void MFence() {
  }
  void SFence() {
  }
  void RLock(const GlobalAddress addr, const size_t count) {
    throw std::runtime_error("not implemented yet!");
  }
  void WLock(const GlobalAddress addr, const size_t count) {
    throw std::runtime_error("not implemented yet!");
  }
  size_t Put(uint64_t key, const void *value, size_t count) {throw std::runtime_error("not implemented yet!");}
  size_t Get(uint64_t key, void *value) { throw std::runtime_error("not implemented yet!"); }
private:
  DSM(const DSMConfig &conf);
  ~DSM();
  DSMConfig conf;
  Cache cache;
public:
  uint32_t myNodeID;
  std::atomic_int appID;
  CacheAgent *cacheAgent[max_syst_num_per_node];
  node_resource *resource;
  channel_context *app_channel[max_appt_num_per_node];
  channel_context *sys_channel[max_syst_num_per_node];
  GlobalAllocator *chunckAlloc[max_syst_num_per_node];
  void * gblock_data_base_addr;
  void * cline_data_base_addr;
  SlabAllocator gblock_data_sb;
  SlabAllocator cline_data_sb;
};
inline GlobalAddress DSM::malloc(size_t size, bool align) {
  return cache.malloc(size, align);
}
inline void DSM::free(const GlobalAddress &addr) { cache.free(addr); }
#endif 