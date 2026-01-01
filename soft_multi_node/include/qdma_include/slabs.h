#pragma once
#include "Common.h"
#include "common.h"
#include "spinlock.h"
#include "phmap.h"
#define ITEM_SIZE_MAX (DSM_CACHE_LINE_SIZE)
#define POWER_SMALLEST 1
#define POWER_LARGEST  200
#define CHUNK_ALIGN_BYTES 8
#define MAX_NUMBER_OF_SLAB_CLASSES (POWER_LARGEST + 1)
#define ITEM_SLABBED 4
#define ITEM_LINKED 1
#define ITEM_CAS 2
#define ITEM_key(item) ((item)->data)
typedef struct {
    unsigned int size; 
    unsigned int perslab; 
    void *slots; 
    unsigned int sl_curr; 
    unsigned int slabs; 
    void **slab_list; 
    unsigned int list_size; 
    unsigned int killing; 
#ifdef FINE_SLAB_LOCK
    std::atomic<size_t> requested; 
    spinlock lock_;
    inline void lock() {
        lock_.lock();
    }
    inline void unlock() {
        lock_.unlock();
    }
#else
    size_t requested; 
#endif
} slabclass_t;
typedef struct _stritem {
    struct _stritem *next;
    struct _stritem *prev;
    int size;
    uint8_t it_flags; 
    uint8_t slabs_clsid;
    void *data;
} item;
class SlabAllocator {
    slabclass_t slabclass[MAX_NUMBER_OF_SLAB_CLASSES];
    size_t mem_limit = 0;
    size_t mem_malloced = 0;
    int power_largest;
    char *mem_base = NULL;
#ifdef FINE_SLAB_LOCK
    std::atomic<char *> mem_current;  
    std::atomic<size_t> mem_avail;
    std::atomic<size_t> mem_free;
#else
    char *mem_current = NULL;
    size_t mem_avail = 0;
    size_t mem_free;
#endif
    int SB_PREFIX_SIZE = 0;  
#ifdef FINE_SLAB_LOCK
    phmap::flat_hash_map<void *, item *> stats_map;
#else
    std::unordered_map<void *, item *> stats_map;
#endif
    int chunk_size = 48;
    int item_size_max = ITEM_SIZE_MAX;
    size_t maxbytes = 64 * 1024 * 1024;
    bool slab_reassign = true;
    bool use_prealloc_addr = false;
#ifndef FINE_SLAB_LOCK
    spinlock lock_;
#endif
    int nz_strcmp(int nzlength, const char *nz, const char *z);
    void do_slabs_free(void *ptr, const size_t size, unsigned int id);
    void *do_slabs_alloc(const size_t size, unsigned int id);
    int do_slabs_newslab(const unsigned int id);
    void split_slab_page_into_freelist(char *ptr, const unsigned int id);
    int grow_slab_list(const unsigned int id);
    void *memory_allocate(size_t size);
    void slabs_preallocate(const unsigned int maxslabs);
    void *mmap_malloc(size_t size);
    void mmap_free(void *ptr);
    void *slabs_alloc(const size_t size, unsigned int id);
    void slabs_free(void *ptr, size_t size, unsigned int id);
    void slabs_adjust_mem_requested(unsigned int id, size_t old, size_t ntotal);
    inline void lock() {
#ifndef FINE_SLAB_LOCK
        lock_.lock();
#endif
    }
    inline void unlock() {
#ifndef FINE_SLAB_LOCK
        lock_.unlock();
#endif
    }
    unsigned int slabs_clsid(const size_t size);
public:
    void *slabs_init(const size_t limit, const double factor,
        void *prealloc_addr = nullptr);
    size_t get_avail();
    void *sb_calloc(size_t count, size_t size);
    void *sb_malloc(size_t size);
    void *sb_aligned_malloc(size_t size, size_t block = DSM_CACHE_LINE_SIZE);
    void *sb_aligned_calloc(size_t count, size_t size, size_t block = DSM_CACHE_LINE_SIZE);
    void *sb_realloc(void *ptr, size_t size);
    size_t sb_free(void *ptr);
    bool is_free(void *ptr);
    size_t get_size(void *ptr);
    ~SlabAllocator();
};