#ifndef __GLOBALADDRESS_H__
#define __GLOBALADDRESS_H__
#include "Common.h"
#define HASH_INDEX
class GlobalAddress {
public:
  uint64_t addr;
  uint8_t nodeID;
  uint8_t org;
  static GlobalAddress Null() {
    static GlobalAddress zero{0, 0};
    return zero;
  }
  uint64_t getTag() const {
    assert(DSM_CACHE_LINE_WIDTH > (sizeof(nodeID) * 8));
    return (((addr >> DSM_CACHE_LINE_WIDTH) << (sizeof(nodeID) * 8)) + nodeID);
  }
  uint64_t getIndex() const {
    return (addr >> DSM_CACHE_LINE_WIDTH);
  }
  bool operator==(const GlobalAddress& other) const {
    return addr == other.addr && nodeID == other.nodeID;
  }
  bool operator!=(const GlobalAddress& other) const {
    return !(*this == other);
  }
} __attribute__((packed));
using GAddr = GlobalAddress;
#define DB_Gnullptr (GlobalAddress::Null())
inline GlobalAddress GADD(const GlobalAddress &addr, int off) {
  auto ret = addr;
  ret.addr += off;
  return ret;
}
#endif 