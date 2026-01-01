#include "slabs.h"
#include "numautils.h"
unsigned int SlabAllocator::slabs_clsid(const size_t size) {
    int res = POWER_SMALLEST;
    if (size == 0)
        return 0;
    while (size > slabclass[res].size) {
        if (res++ == power_largest) 
            return 0;
    }
    return res;
}
size_t SlabAllocator::get_avail() {
    return mem_free;
}
void *SlabAllocator::slabs_init(const size_t limit, const double factor,
    void *prealloc_addr) {
    printf("limit = %ld, factor = %lf, prealloc_addr = %p\n", limit,
        factor, prealloc_addr);
    int i = POWER_SMALLEST - 1;
    unsigned int size = SB_PREFIX_SIZE + chunk_size;
    unsigned int pre_size = size;
    mem_limit = limit;
    mem_free = mem_limit;
    if (!prealloc_addr) {
        mem_base = (char *) get_huge_mem(NIC_NUMA_NODE, mem_limit);
        if (mem_base != NULL) {
            printf("allocate succeed\n");
        } else {
            fprintf(stderr, "Warning: Failed to allocate requested memory in"
                " one large chunk.\nWill allocate in smaller chunks\n");
        }
        use_prealloc_addr = false;
    } else {
        mem_base = (char *)prealloc_addr;
        use_prealloc_addr = true;
    }
    mem_current = mem_base;
    mem_avail = mem_limit;
    memset(slabclass, 0, sizeof(slabclass));
    while (++i < POWER_LARGEST && size <= item_size_max / factor) {
        if (size % CHUNK_ALIGN_BYTES)
            size += CHUNK_ALIGN_BYTES - (size % CHUNK_ALIGN_BYTES);
        if ((int)(pre_size / DSM_CACHE_LINE_SIZE) < (int)(size / DSM_CACHE_LINE_SIZE)
            && (size % DSM_CACHE_LINE_SIZE)) {
            slabclass[i].size = size / DSM_CACHE_LINE_SIZE * DSM_CACHE_LINE_SIZE;
            slabclass[i].perslab = item_size_max / slabclass[i].size;
            i++;
        }
        slabclass[i].size = size;
        slabclass[i].perslab = item_size_max / slabclass[i].size;
        pre_size = size;
        size *= factor;
    }
    power_largest = i;
    slabclass[power_largest].size = item_size_max;
    slabclass[power_largest].perslab = 1;
    {
        char *t_initial_malloc = getenv("T_MEMD_INITIAL_MALLOC");
        if (t_initial_malloc) {
            mem_malloced = (size_t)atol(t_initial_malloc);
        }
    }
    slabs_preallocate(power_largest);
    return mem_base;
}
void SlabAllocator::slabs_preallocate(const unsigned int maxslabs) {
    int i;
    unsigned int prealloc = 0;
    for (i = POWER_SMALLEST; i <= POWER_LARGEST; i++) {
        if (++prealloc > maxslabs)
            return;
        if (do_slabs_newslab(i) == 0) {
            fprintf(stderr, "Error while preallocating slab memory!\n"
                "If using -L or other prealloc options, max memory must be "
                "at least %d megabytes.\n",
                power_largest);
            exit(1);
        }
    }
}
void *SlabAllocator::memory_allocate(size_t size) {
    void *ret = NULL;
    if (mem_base == NULL) {
        printf("allocator is not initialized!");
    } else {
        ret = mem_current;
        if (size > mem_avail) {
            return NULL;
        }
        if (size % CHUNK_ALIGN_BYTES) {
            size += CHUNK_ALIGN_BYTES - (size % CHUNK_ALIGN_BYTES);
        }
        mem_current += size;
        if (size < mem_avail) {
            mem_avail -= size;
        } else {
            mem_avail = 0;
        }
    }
    return ret;
}
int SlabAllocator::grow_slab_list(const unsigned int id) {
    slabclass_t *p = &slabclass[id];
    if (p->slabs == p->list_size) {
        size_t new_size = (p->list_size != 0) ? p->list_size * 2 : 16;
        void *new_list = realloc(p->slab_list, new_size * sizeof(void *));
        if (new_list == 0)
            return 0;
        p->list_size = new_size;
        p->slab_list = (void **)new_list;
    }
    return 1;
}
void SlabAllocator::split_slab_page_into_freelist(char *ptr,
    const unsigned int id) {
    slabclass_t *p = &slabclass[id];
    int x;
    for (x = 0; x < p->perslab; x++) {
        do_slabs_free(ptr, 0, id);
        ptr += p->size;
    }
}
int SlabAllocator::do_slabs_newslab(const unsigned int id) {
    slabclass_t *p = &slabclass[id];
    int len = slab_reassign ? item_size_max : p->size * p->perslab;
    char *ptr;
    if ((mem_limit && mem_malloced + len > mem_limit && p->slabs > 0)
        || (grow_slab_list(id) == 0)
        || ((ptr = (char *)memory_allocate((size_t)len)) == 0)) {
        printf("new slab class %d failed, mem limit: %ld, mem malloced: %ld, len: %d, slabs: %d\n", id, mem_limit, mem_malloced, len, p->slabs);
        return 0;
    }
    split_slab_page_into_freelist(ptr, id);
    p->slab_list[p->slabs++] = ptr;
    mem_malloced += len;
    return 1;
}
void *SlabAllocator::do_slabs_alloc(const size_t size, unsigned int id) {
    slabclass_t *p;
    void *ret = NULL;
    item *it = NULL;
    if (id < POWER_SMALLEST || id > power_largest) {
        printf("id < POWER_SMALLEST || id > power_largest\n");
        return NULL;
    }
    p = &slabclass[id];
#ifdef FINE_SLAB_LOCK
    p->lock();
#endif
    assert(p->sl_curr == 0 || ((item *)p->slots)->slabs_clsid == 0);
    if (!(p->sl_curr != 0 || do_slabs_newslab(id) != 0)) {
        printf("We don't have more memory available\n");
        ret = NULL;
    } else if (p->sl_curr != 0) {
        it = (item *)p->slots;
        p->slots = it->next;
        if (it->next)
            it->next->prev = 0;
        it->size = size;
        it->slabs_clsid = id;
        p->sl_curr--;
        ret = it->data;  
    }
#ifdef FINE_SLAB_LOCK
    p->unlock();
#endif
    if (ret) {
        p->requested += size;
        mem_free -= p->size;
    } else {
    }
    return ret;
}
void SlabAllocator::do_slabs_free(void *ptr, const size_t size,
    unsigned int id) {
    slabclass_t *p;
    item *it;
    assert(id >= POWER_SMALLEST && id <= power_largest);
    if (id < POWER_SMALLEST || id > power_largest)
        return;
    p = &slabclass[id];
    if (stats_map.count(ptr)) {
        it = stats_map.at(ptr);  
    } else {
        it = new item();
        stats_map[ptr] = it;
    }
    it->data = ptr;  
    it->it_flags |= ITEM_SLABBED;
    it->prev = 0;
    it->next = (struct _stritem *)p->slots;
    if (it->next)
        it->next->prev = it;
    p->slots = it;
    p->sl_curr++;
    p->requested -= size;
    if (size)
        mem_free += p->size;
    return;
}
void *SlabAllocator::sb_malloc(size_t size) {
    lock();
    if (mem_limit == 0) {
        printf("sb_mallocator is not initiated. Use default malloc\n");
        return NULL;
    }
    size_t newsize = size + SB_PREFIX_SIZE;
    unsigned int id = slabs_clsid(newsize);
    void *ret = slabs_alloc(newsize, id);  
    assert(ret);
    unlock();
    return ret;
}
void *SlabAllocator::sb_aligned_malloc(size_t size, size_t block) {
    lock();
    if (mem_limit == 0) {
        printf("sb_mallocator is not initiated. Use default malloc\n");
        return NULL;
    }
    size_t newsize = size + SB_PREFIX_SIZE;
    newsize = ALIGN(newsize, block);
    unsigned int id = slabs_clsid(newsize);
    void *ret = slabs_alloc(newsize, id);  
    assert(ret);
    assert((uint64_t)ret % block == 0);
    unlock();
    return ret;
}
void *SlabAllocator::sb_calloc(size_t count, size_t size) {
    if (unlikely(mem_limit == 0)) {
        printf("using default calloc\n");
        return NULL;
    }
    void *ptr = sb_malloc(count * size);
    if (ptr != NULL) {
        memset(ptr, 0, size);
    } else {
        assert(false);
    }
    return ptr;
}
void *SlabAllocator::sb_aligned_calloc(size_t count, size_t size,
    size_t block) {
    if (unlikely(mem_limit == 0)) {
        printf("using default calloc\n");
        return NULL;
    }
    void *ptr = sb_aligned_malloc(count * size, block);
    if (ptr != NULL) {
        memset(ptr, 0, size);
    } else {
        printf("no free memory");
        assert(false);
    }
    return ptr;
}
void *SlabAllocator::sb_realloc(void *ptr, size_t size) {
    printf("sb_realloc size = %ld\n", size);
    if (unlikely(mem_limit == 0)) {
        printf("using default realloc\n");
        return NULL;
    }
    if (ptr == NULL)
        return sb_malloc(size);
    lock();
    assert(stats_map.count(ptr));  
    item *it1 = stats_map.at(ptr);  
    unsigned int id1 = it1->slabs_clsid;
    int size1 = it1->size;
    assert(id1 == slabs_clsid(size1));
    size_t size2 = size + SB_PREFIX_SIZE;
    unsigned int id2 = slabs_clsid(size2);
    void *ret = nullptr;
    if (id1 == id2) {
        it1->size = size2;
        slabs_adjust_mem_requested(id1, size1, size2);
        ret = ptr;
    } else {
        assert(size1 != size2);
        void *ptr = slabs_alloc(size2, id2);  
        assert(stats_map.count(ptr));  
        item *it2 = stats_map.at(ptr);  
        if (size2 < size1)
            memcpy(ITEM_key(it2), ptr, size);
        else
            memcpy(ITEM_key(it2), ptr, size1 - SB_PREFIX_SIZE);
        it1->slabs_clsid = 0;
        slabs_free(it1->data, size1, id1);  
        ret = ITEM_key(it2);
    }
    unlock();
    assert(ret);
    return ret;
}
bool SlabAllocator::is_free(void *ptr) {
    lock();
    assert(stats_map.count(ptr));  
    item *it = stats_map.at(ptr);  
    bool ret = it->slabs_clsid == 0 ? true : false;
    unlock();
    return ret;
}
size_t SlabAllocator::get_size(void *ptr) {
    assert(stats_map.count(ptr));
    item *it = stats_map[ptr];
    return it->size;
}
size_t SlabAllocator::sb_free(void *ptr) {
    lock();
    if (mem_limit == 0) {
        printf("allocator is not initialized");
        return 0;
    }
    assert(stats_map.count(ptr));  
    item *it = stats_map.at(ptr);  
    unsigned int id = it->slabs_clsid;
    size_t size = it->size;
    assert(id == slabs_clsid(it->size));
    it->slabs_clsid = 0;
    it->size = 0;
    slabs_free(it->data, size, id);
    unlock();
    return size;
}
void *SlabAllocator::slabs_alloc(size_t size, unsigned int id) {
    void *ret;
    ret = do_slabs_alloc(size, id);
    return ret;
}
void SlabAllocator::slabs_free(void *ptr, size_t size, unsigned int id) {
    slabclass_t *p = &slabclass[id];
#ifdef FINE_SLAB_LOCK
    p->lock();
#endif
    do_slabs_free(ptr, size, id);
#ifdef FINE_SLAB_LOCK
    p->unlock();
#endif
}
int SlabAllocator::nz_strcmp(int nzlength, const char *nz, const char *z) {
    int zlength = strlen(z);
    return (zlength == nzlength) && (strncmp(nz, z, zlength) == 0) ? 0 : -1;
}
void SlabAllocator::slabs_adjust_mem_requested(unsigned int id, size_t old,
    size_t ntotal) {
    slabclass_t *p;
    if (id < POWER_SMALLEST || id > power_largest) {
        fprintf(stderr, "Internal error! Invalid slab class\n");
        abort();
    }
    p = &slabclass[id];
    p->requested = p->requested - old + ntotal;
}
SlabAllocator::~SlabAllocator() {
    if (!use_prealloc_addr && mem_base != nullptr) {
        free_huge_mem(mem_base);
    }
}
