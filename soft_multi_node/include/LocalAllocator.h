#if !defined(_LOCAL_ALLOC_H_)
#define _LOCAL_ALLOC_H_
#include "Common.h"
#include "GlobalAddress.h"
#include <vector>
class LocalAllocator {
public:
  LocalAllocator() {
    head = GlobalAddress::Null();
    cur = GlobalAddress::Null();
  }
  GlobalAddress malloc(size_t size, bool &need_chunck, bool align) {
    if (align) {
      if (cur.addr % DSM_CACHE_LINE_SIZE != 0) {
        cur.addr = (cur.addr / DSM_CACHE_LINE_SIZE + 1) * DSM_CACHE_LINE_SIZE;
      }
    }
    GlobalAddress res = cur;
    if (log_heads.empty() || (cur.addr + size > head.addr + define::kChunkSize)) {
        need_chunck = true;
    } else {
        need_chunck = false;
        cur.addr += size;
    }
    assert(res.addr + size <= 40 * define::GB);
    return res;
  }
  void set_chunck(GlobalAddress &addr) {
    log_heads.push_back(addr);
    head = cur = addr;
  }
  void free(const GlobalAddress &addr) {
  }
private:
  GlobalAddress head;
  GlobalAddress cur;
  std::vector<GlobalAddress> log_heads;
};
#endif 
