#pragma once
#include <atomic>
class spinlock final {
private:
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    template<typename WhileIdleFunc = void()>
    void lock(WhileIdleFunc idle_work = [] {}) {
        while (flag.test_and_set(std::memory_order_acquire)) {
            idle_work();
        }
    }
    bool try_lock() {
        return !flag.test_and_set(std::memory_order_acquire);
    }
    void unlock() {
        flag.clear(std::memory_order_release);
    }
    spinlock() = default;
    spinlock(const spinlock &) = delete;
    spinlock &operator=(const spinlock &) = delete;
};
class spinlock_rw {
private:
    std::atomic<int> readers_count{ 0 };
    std::atomic<bool> writer_lock{ false };
public:
    void lock_read() {
        while (true) {
            while (writer_lock) {}
            readers_count.fetch_add(1, std::memory_order_acquire);
            if (writer_lock) {
                readers_count.fetch_sub(1, std::memory_order_release);
            } else {
                break;  
            }
        }
    }
    bool try_lock_read() {
        if (!writer_lock) {
            readers_count.fetch_add(1, std::memory_order_acquire);
            return true;
        }
        return false;
    }
    void unlock_read() {
        readers_count.fetch_sub(1, std::memory_order_release);
    }
    void lock_write() {
        while (true) {
            while (readers_count != 0 || writer_lock) {}
            bool expected = false;
            if (writer_lock.compare_exchange_weak(expected, true, std::memory_order_acquire)) {
                break;  
            }
        }
    }
    bool try_lock_write() {
        bool expected = false;
        return writer_lock.compare_exchange_strong(expected, true, std::memory_order_acquire);
    }
    void unlock_write() {
        writer_lock.store(false, std::memory_order_release);
    }
};