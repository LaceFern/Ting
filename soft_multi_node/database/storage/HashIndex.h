#ifndef __DATABASE_STORAGE_HASH_INDEX_H__
#define __DATABASE_STORAGE_HASH_INDEX_H__
#include "HashIndexHelper.h"
#include "GlobalAddress.h"
#include "GAddrArray.h"
#include "Profiler.h"
namespace Database {
class HashIndex : public GAMObject{
public:
  HashIndex() {
  }
  ~HashIndex() {
  }
  void Init(const uint64_t& buckets_count, GAlloc* gallocator) {
    buckets_count_ = buckets_count;
    record_count_ = 0;
    buckets_.Init(buckets_count_, BucketHeader::GetSerializeSize(), gallocator);
    for (uint64_t i = 0; i < buckets_count_; ++i) {
      BucketHeader bk_hd;
      bk_hd.Init();
      GAddr bk_hd_addr = buckets_.GetElementGAddr(i, gallocator);
      bk_hd.Serialize(bk_hd_addr, gallocator);
    }
  }
  bool InsertRecord(const IndexKey& key, const GAddr& record_ptr,
                            GAlloc* gallocator, size_t thread_id) {
    uint64_t offset = Hash(key);
    GAddr hd_addr = buckets_.GetElementGAddr(offset, gallocator);
    this->TryWLockBucketHeader(hd_addr, gallocator);
    BucketHeader bk_hd;
    bk_hd.Deserialize(hd_addr, gallocator);
    bool ret = bk_hd.InsertNode(key, record_ptr, gallocator);
    bk_hd.Serialize(hd_addr, gallocator);
    this->UnLock(hd_addr, gallocator);
    record_count_++;
    return ret;
  }
  GAddr SearchRecord(const IndexKey& key, 
      GAlloc *gallocator, size_t thread_id) {
    uint64_t offset = Hash(key);
    GAddr hd_addr = buckets_.GetElementGAddr(offset, gallocator);
    BucketHeader bk_hd;
    bk_hd.Deserialize(hd_addr, gallocator);
    GAddr items_list = DB_Gnullptr;
    if (bk_hd.SearchNode(key, items_list, gallocator)) {
      Items items;
      gallocator->Read(items_list, &items, sizeof(Items));
      return items.record_ptrs_[0];
    } else {
      return DB_Gnullptr;
    }
  }
  virtual void Serialize(const GAddr& addr, GAlloc *gallocator) {
    size_t off = 0;
    buckets_.Serialize(addr, gallocator);
    off += buckets_.GetSerializeSize();
    gallocator->Write(addr, off, &buckets_count_, sizeof(uint64_t));
    off += sizeof(uint64_t);
    gallocator->Write(addr, off, &record_count_, sizeof(uint64_t));
  }
  virtual void Deserialize(const GAddr& addr, GAlloc *gallocator) {
    size_t off = 0;
    buckets_.Deserialize(addr, gallocator);
    off += buckets_.GetSerializeSize();
    gallocator->Read(addr, off, &buckets_count_, sizeof(uint64_t));
    off += sizeof(uint64_t);
    gallocator->Read(addr, off, &record_count_, sizeof(uint64_t));
  }
  static size_t GetSerializeSize() {
    return GAddrArray<BucketHeader>::GetSerializeSize() 
      + sizeof(uint64_t)*2;
  }
  uint64_t GetRecordCount() const {
    return record_count_;
  }
  uint64_t GetBucketsCount() const {
    return buckets_count_;
  }
 private:
  HashIndex(const HashIndex&);
  HashIndex& operator=(const HashIndex&);
  uint64_t Hash(const IndexKey& key) {
    uint64_t ret = key % buckets_count_;
    return ret;
  }
  void TryWLockBucketHeader(const GAddr& header_addr, GAlloc* gallocator) {
    size_t sz = BucketHeader::GetSerializeSize();
    gallocator->WLock(header_addr, sz);
  }
  void TryRLockBucketHeader(const GAddr& header_addr, GAlloc* gallocator) {
    size_t sz = BucketHeader::GetSerializeSize();
    gallocator->RLock(header_addr, sz);
  }
  void UnLock(const GAddr& header_addr, GAlloc* gallocator) {
    size_t sz = BucketHeader::GetSerializeSize();
    gallocator->UnLock(header_addr, sz);
  }
 private:
  GAddrArray<BucketHeader> buckets_;
  uint64_t buckets_count_;
  uint64_t record_count_;
};
}
#endif
