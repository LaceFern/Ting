#ifndef BARRIER_H_INCLUDED
#define BARRIER_H_INCLUDED
#include "defs.h"
namespace atomic_queue {
class Barrier {
    std::atomic<unsigned> counter_ = {};
public:
    void wait() noexcept {
        counter_.fetch_add(1, std::memory_order_acquire);
        while(counter_.load(std::memory_order_relaxed))
            spin_loop_pause();
    }
    void release(unsigned expected_counter) noexcept {
        while(expected_counter != counter_.load(std::memory_order_relaxed))
            spin_loop_pause();
        counter_.store(0, std::memory_order_release);
    }
};
} 
#endif 
