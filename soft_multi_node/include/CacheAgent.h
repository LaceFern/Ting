#ifndef __CACHEAGENT_H__
#define __CACHEAGENT_H__
#include <thread>
#include "RawMessageConnection.h"
#include <atomic>
#include "Config.h"
#include "channel_context.h"
enum AgentPendingReason: uint8_t {
    NOP,
    WAIT_WRITE_BACK_2_SHARED,
    WAIT_WRITE_BACK_2_INVALID,
};
union AgentWrID {
    uint64_t wrId;
    struct {
        uint32_t padding1;
        uint16_t padding2;
        uint8_t  fingerprint;
        AgentPendingReason type;
    };
} __attribute__((packed));
static_assert(sizeof(AgentWrID) == sizeof(uint64_t), "XXX");
class CacheAgentConnection;
class RemoteConnection;
class Cache;
struct LineInfo;
class CacheAgent {
   public:
    int agent_id;
    CacheAgent(int agent_id, Cache *cache, channel_context *channel);
    ~CacheAgent();
    std::atomic<bool> stopFlag{false};
   private:
    Cache *cache;
    std::thread *agent;
    channel_context *channel;
    void agentThread();
};
#endif 
