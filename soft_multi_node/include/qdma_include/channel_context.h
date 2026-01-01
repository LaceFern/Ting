#pragma once
#include "common.h"
#include "slabs.h"
#include "ring_buffer.h"
#include "RawMessageConnection.h"
#include "QDMAController.hpp"
#include "Config.h"
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <fmt/os.h>
#include <fmt/args.h>
#include <fmt/ostream.h>
#include <fmt/std.h>	
#include <fmt/color.h>
class node_resource;
class channel_context;
class node_resource_factory;
class node_resource {
    friend channel_context;
private:
    FPGACtl *fpga_ctl;
    CPUMemCtl *cpu_mem_ctl;
    uint8_t fpga_pci_bus;
    size_t fpga_bridge_size;
    size_t pool_size;
    bool system_channel_inited = false;
public:
    channel_context *system_channel;
    SlabAllocator sb;
    explicit node_resource(uint8_t pci_bus, size_t fpga_bridge_bar_size, size_t cpu_pool_size, double factor);
    ~node_resource();
    uint8_t get_fpga_pci();
    void *get_data_base_addr();
    void init_system_channel();
    void init_channel_parameter(const DSMConfig &config) {
        printf("init channel parameter\n");
        fpga_ctl->writeReg(126, config.appt_num_per_node);
        fpga_ctl->writeReg(127, config.syst_num_per_node);
    }
    void debug_print(){
        fmt::println("tlb.io.tlb_miss_count                                       : {}", fpga_ctl->readReg(512+400));
        fmt::println("io.c2h_cmd.[fire]                                           : {}", fpga_ctl->readReg(512+401));
        fmt::println("io.h2c_cmd.[fire]                                           : {}", fpga_ctl->readReg(512+402));
        fmt::println("io.c2h_data.[fire]                                          : {}", fpga_ctl->readReg(512+403));
        fmt::println("io.h2c_data.[fire]                                          : {}", fpga_ctl->readReg(512+404));
        fmt::println("fifo_c2h_cmd.io.out.[fire]                                  : {}", fpga_ctl->readReg(512+405));
        fmt::println("fifo_h2c_cmd.io.out.[fire]                                  : {}", fpga_ctl->readReg(512+406));
        fmt::println("fifo_c2h_data.io.out.[fire]                                 : {}", fpga_ctl->readReg(512+407));
        fmt::println("fifo_h2c_data.io.in.[fire]                                  : {}", fpga_ctl->readReg(512+408));
        fmt::println("fifo_ddrPort_aw.io.out.[fire]                               : {}", fpga_ctl->readReg(512+409));
        fmt::println("fifo_ddrPort_w.io.out.[fire]                                : {}", fpga_ctl->readReg(512+410));
        fmt::println("fifo_ddrPort_b.io.in.[fire]                                 : {}", fpga_ctl->readReg(512+411));
        fmt::println("fifo_ddrPort_ar.io.out.[fire]                               : {}", fpga_ctl->readReg(512+412));
        fmt::println("fifo_ddrPort_r.io.in.[fire]                                 : {}", fpga_ctl->readReg(512+413));
        fmt::println("io.chisel_net_rx.[fire]                                     : {}", fpga_ctl->readReg(512+414));
        fmt::println("io.chisel_net_tx.[fire]                                     : {}", fpga_ctl->readReg(512+415));
        fmt::println("io.chisel_c2h_cmd.[fire]                                    : {}", fpga_ctl->readReg(512+416));
        fmt::println("io.chisel_c2h_data.[fire]                                   : {}", fpga_ctl->readReg(512+417));
        fmt::println("io.chisel_axib.w.[fire]                                     : {}", fpga_ctl->readReg(512+418));
        fmt::println("fifo_c2h_cmd.io.out.valid                                   : {}", (fpga_ctl->readReg(512+419) >> 0) & 1);
        fmt::println("fifo_c2h_cmd.io.out.ready                                   : {}", (fpga_ctl->readReg(512+419) >> 1) & 1);
        fmt::println("fifo_h2c_cmd.io.out.valid                                   : {}", (fpga_ctl->readReg(512+419) >> 2) & 1);
        fmt::println("fifo_h2c_cmd.io.out.ready                                   : {}", (fpga_ctl->readReg(512+419) >> 3) & 1);
        fmt::println("fifo_c2h_data.io.out.valid                                  : {}", (fpga_ctl->readReg(512+419) >> 4) & 1);
        fmt::println("fifo_c2h_data.io.out.ready                                  : {}", (fpga_ctl->readReg(512+419) >> 5) & 1);
        fmt::println("fifo_h2c_data.io.in.valid                                   : {}", (fpga_ctl->readReg(512+419) >> 6) & 1);
        fmt::println("fifo_h2c_data.io.in.ready                                   : {}", (fpga_ctl->readReg(512+419) >> 7) & 1);
    }
};
class node_resource_factory {
private:
    static std::map<uint8_t, node_resource *> node_resources;
    static spinlock lock;
public:
    static node_resource *get_node_resource(const DSMConfig &config);
    static void destory_node_resource(uint8_t pci_bus);
};
    class channel_context {
        friend node_resource;
        static constexpr uint32_t FPGA_REG_OFFSET = 128;
    private:
        uint32_t get_tx_cpu_send_reg_index() {
            return FPGA_REG_OFFSET + channel_index * 8 + 2;
        }
        uint32_t get_tx_fpga_recv_reg_index() {
            return FPGA_REG_OFFSET + channel_index * 8 + 3;
        }
        uint32_t get_rx_head_reg_index() {
            return FPGA_REG_OFFSET + channel_index * 8 + 6;
        }
        uint32_t get_rx_tail_reg_index() {
            return FPGA_REG_OFFSET + channel_index * 8 + 7;
        }
        node_resource *resource;
        uint32_t channel_index;
        uint32_t tx_depth = 64;
        uint32_t rx_depth = 64;
        tx_ring_buffer<RawMessage> *tx_ring;
        rx_ring_buffer<RawMessage> *rx_ring;
        uint32_t tx_writeBridge_idx;
    public:
        uint64_t rdtsc() {
          unsigned int lo, hi;
          __asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
          return ((uint64_t)hi << 32) | lo;
        }
        uint64_t rdtscp() {
          unsigned int lo, hi;
          __asm__ __volatile__("rdtscp" : "=a" (lo), "=d" (hi));
          return ((uint64_t)hi << 32) | lo;
        }
        channel_context(node_resource *node_resource, uint32_t ch_index) {
            resource = node_resource;
            channel_index = ch_index;
            void *tx_buffer = reinterpret_cast<void *>(reinterpret_cast<uint8_t *>(resource->fpga_ctl->getBridgeAddr()) + ch_index * tx_depth * sizeof(RawMessage));
            void *rx_buffer = resource->sb.sb_calloc(sizeof(RawMessage), rx_depth);
            assert(tx_buffer);
            assert(rx_buffer);
            tx_writeBridge_idx = ch_index * tx_depth;
            tx_ring = new tx_ring_buffer<RawMessage>(resource->fpga_ctl, get_tx_cpu_send_reg_index(), get_tx_fpga_recv_reg_index(), static_cast<RawMessage *>(tx_buffer), tx_depth);
            rx_ring = new rx_ring_buffer<RawMessage>(resource->fpga_ctl, get_rx_head_reg_index(), get_rx_tail_reg_index(), static_cast<RawMessage *>(rx_buffer), rx_depth);
            std::memset(rx_buffer, 0, rx_depth * sizeof(RawMessage)); 
            resource->fpga_ctl->writeReg(FPGA_REG_OFFSET + channel_index * 8, ch_index * tx_depth);
            resource->fpga_ctl->writeReg(FPGA_REG_OFFSET + channel_index * 8 + 1, (ch_index + 1) * tx_depth - 1);
            resource->fpga_ctl->writeReg(FPGA_REG_OFFSET + channel_index * 8 + 4, static_cast<uint32_t>((reinterpret_cast<uint64_t>(rx_buffer))));
            resource->fpga_ctl->writeReg(FPGA_REG_OFFSET + channel_index * 8 + 5, static_cast<uint32_t>(reinterpret_cast<uint64_t>(rx_buffer) >> 32));
        }
        void send(RawMessage &message) {
            RawMessageUnion msgUnion;
            msgUnion.msg = message;
            resource->fpga_ctl->writeBridge(tx_writeBridge_idx, {msgUnion.values[0],
                                                                msgUnion.values[1], 
                                                                msgUnion.values[2], 
                                                                msgUnion.values[3],
                                                                msgUnion.values[4],
                                                                msgUnion.values[5],
                                                                msgUnion.values[6],
                                                                msgUnion.values[7]});
        }
        RawMessage *recv(std::atomic<bool>&stop) {
            volatile uint8_t flag = 0;
            while (flag == 0) {
                flag = rx_ring->get_head_element()->general_c2h_owner;
                if (unlikely(stop.load())) {
                    return nullptr;
                }
            }
            RawMessage *message = rx_ring->get_head_element();
            rx_ring->get_head_element()->general_c2h_owner = 0; 
            rx_ring->step_head();
            return message;
        }
        RawMessage *recv() {
            volatile uint8_t flag = 0;
            volatile uint64_t start_tp = rdtsc();
            uint64_t s_thres = 1ull * 10 * 1000 * 1000 * 2200;
            uint8_t s_flag = 0;
            while (flag == 0) {
                flag = rx_ring->get_head_element()->general_c2h_owner;
                uint64_t end_tp = rdtscp();
                if(end_tp - start_tp > s_thres && s_flag == 0){
                    printf("channel index = %d, stall position = %d\n", channel_index, rx_ring->get_head());
                    for(int i = 0; i < rx_depth; i++){
                        RawMessage *message = (RawMessage *)((uint64_t)(rx_ring->get_ring_buffer_addr()) + i * sizeof(RawMessage));
                        printf("[%d] = (%lx, %d)\n", i, (uint64_t)message, message->general_c2h_owner);
                        if(message->general_c2h_owner == 1){
                            printf("mtype = %u\n", message->mtype);
                            printf("req_cline_nodeID = %u\n", message->req_cline_nodeID);
                            printf("req_cline_appThreadID = %u\n", message->req_cline_appThreadID);
                            printf("req_cline_addr = %lx\n", message->req_cline_addr);
                            printf("req_cline_index = %u\n", message->req_cline_index);
                            printf("cline_tag = %lx\n", message->cline_tag);
                            printf("dir_gblock_addr = %lx\n", message->dir_gblock_addr);
                            printf("dir_gblock_index = %u\n", message->dir_gblock_index);
                            printf("dir_gblock_nodeID = %u\n", message->dir_gblock_nodeID);
                        }
                    }
                    s_flag = 1;
                    exit(1);
                }
            }
            RawMessage *message = rx_ring->get_head_element();
            rx_ring->get_head_element()->general_c2h_owner = 0; 
            rx_ring->step_head();
            return message;
        }
        RawMessage recv_msg() {
            volatile uint8_t flag = 0;
            volatile uint64_t start_tp = rdtsc();
            uint64_t s_thres = 1ull * 10 * 1000 * 1000 * 2200;
            uint8_t s_flag = 0;
            while (flag == 0) {
                flag = rx_ring->get_head_element()->general_c2h_owner;
                uint64_t end_tp = rdtscp();
                if(end_tp - start_tp > s_thres && s_flag == 0){
                    RawMessage message;
                    message.mtype = 0xff;
                    return message;
                }
            }
            RawMessage message = *(rx_ring->get_head_element());
            rx_ring->get_head_element()->general_c2h_owner = 0; 
            rx_ring->step_head();
            return message;
        }
        ~channel_context() {
            resource->sb.sb_free(rx_ring->get_ring_buffer_addr());
            delete tx_ring;
            delete rx_ring;
            printf("destroy channel_context %u\n", channel_index);
        }
    };
