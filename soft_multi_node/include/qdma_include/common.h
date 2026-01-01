#pragma once
#include <bits/stdc++.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <execinfo.h>
#include "log.h"
#define _unused(x) ((void)(x)) 
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define _unreach() __builtin_unreachable()
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))
#define ALIGN(x,a)              __ALIGN_MASK(x,(a)-1)
typedef uint64_t g_addr;
#define OFF_MASK 0xFFFFFFFFFFFFL
#define WID(gaddr) ((gaddr) >> 48)
#define OFF(gaddr) ((gaddr) & OFF_MASK)
#define TO_GLOB(addr, base, wid) ((uint64_t)(addr) - (uint64_t)(base) + ((uint64_t)(wid) << 48))
#define TO_BLOCK(x) ((x) & BLOCK_MASK)
#define TO_BLOCK_ALIGN(x) (ALIGN(x, BLOCK_SIZE))
#define TO_LOCAL(gaddr, base) (void*)((uint64_t)(base) + OFF(gaddr))
#define TO_NEXT_BLOCK(x) TO_BLOCK(x+BLOCK_SIZE)
#define EMPTY_GLOB(wid) ((uint64_t)(wid) << 48)
#define Ki(x) (static_cast<size_t>(x) * 1000)
#define Mi(x) (static_cast<size_t>(x) * 1000 * 1000)
#define Gi(x) (static_cast<size_t>(x) * 1000 * 1000 * 1000)
#define KB(x) (static_cast<size_t>(x) << 10)
#define MB(x) (static_cast<size_t>(x) << 20)
#define GB(x) (static_cast<size_t>(x) << 30)
#define min_(x, y) x < y ? x : y
#define max_(x, y) x > y ? x : y
#define min_t(type, x, y) ({            \
    type __min1 = (x);            \
    type __min2 = (y);            \
    __min1 < __min2 ? __min1: __min2; })
#define max_t(type, x, y) ({            \
    type __max1 = (x);            \
    type __max2 = (y);            \
    __max1 > __max2 ? __max1: __max2; })
#define SWAP(x, y)             \
    do                         \
    {                          \
        typeof(x) ____val = x; \
        x = y;                 \
        y = ____val;           \
    } while (0)
#define is_log2(v) (((v) & ((v)-1)) == 0)
#pragma GCC diagnostic ignored "-Wunused-function"
    static size_t round_up(size_t num, size_t factor) {
        return num + factor - 1 - (num + factor - 1) % factor;
    }
    static inline void rt_assert(bool condition, const std::string &throw_str, char *s) {
        if (unlikely(!condition)) {
            print_stack_trace();
            throw std::runtime_error(throw_str + std::string(s));
        }
    }
    static inline void rt_assert(bool condition, const char *throw_str) {
        if (unlikely(!condition)) {
            print_stack_trace();
            throw std::runtime_error(std::string(throw_str));
        }
    }
    static inline void rt_assert(bool condition, const std::string &throw_str) {
        if (unlikely(!condition)) {
            print_stack_trace();
            throw std::runtime_error(throw_str);
        }
    }
    static inline void rt_assert(bool condition) {
        if (unlikely(!condition)) {
            print_stack_trace();
            throw std::runtime_error("Error");
        }
    }
    static inline void rt_assert(bool condition, const char *file_name, int line) {
        if (unlikely(!condition)) {
            print_stack_trace();
            throw std::runtime_error(std::string(file_name) + ":" + std::to_string(line));
        }
    }
    template <typename... Args>
    std::string string_format(const std::string &format, Args... args) {
        int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; 
        if (size_s <= 0) {
            throw std::runtime_error("Error during formatting.");
        }
        auto size = static_cast<size_t>(size_s);
        std::unique_ptr<char[]> buf(new char[size]);
        std::snprintf(buf.get(), size, format.c_str(), args...);
        return std::string(buf.get(), buf.get() + size - 1); 
    }
