#pragma once
#include "common.h"
#include "QDMAController.hpp"
    static constexpr uint32_t MAX_LOOP_SIZE = 1000;
    template<typename T>
    class ring_buffer {
    public:
        virtual void step_head() = 0;
        virtual void step_tail() = 0;
        virtual void update_head_with_fpga() = 0;
        virtual void update_tail_with_fpga() = 0;
        explicit ring_buffer(T *buffer, uint32_t size) {
            element_buffer = buffer;
            ring_size = size;
        }
        virtual ~ring_buffer() = default;
        uint32_t get_head() {
            return head;
        }
        uint32_t get_tail() {
            return tail;
        }
        bool is_empty() {
            return head == tail;
        }
        bool is_full() {
            return (tail + 1) % ring_size == head;
        }
        uint32_t ring_length() {
            return (tail + ring_size - head) % ring_size;
        }
        T *get_ring_buffer_addr() {
            return element_buffer;
        }
    protected:
        uint32_t head{};
        uint32_t tail{};
        T *element_buffer{};
        uint32_t ring_size{};
    };
    template<typename T>
    class tx_ring_buffer:public ring_buffer<T> {
    public:
        explicit tx_ring_buffer(FPGACtl *fpga_inst, uint32_t cpu_send_index, uint32_t fpga_recv_index, T *buffer, uint32_t size):ring_buffer<T>(buffer, size) {
            fpga_ctl = fpga_inst;
            cpu_send_reg_index = cpu_send_index;
            fpga_recv_reg_index = fpga_recv_index;
            cpu_send_cnt = 0;
            fpga_recv_cnt = 0;
        }
        ~tx_ring_buffer() override = default;
        void update_head_with_fpga() override {
            fpga_recv_cnt = fpga_ctl->readReg(fpga_recv_reg_index + 512);
        }
        void update_tail_with_fpga() override {
            fpga_ctl->writeReg(cpu_send_reg_index, cpu_send_cnt);
        }
        T *get_tail_element() {
            size_t loop_index = 0;
            while (unlikely(ring_buffer<T>::is_full()) && loop_index < MAX_LOOP_SIZE) {
                update_head_with_fpga();
                loop_index++;
            }
            rt_assert(loop_index != MAX_LOOP_SIZE, "tx_ring_buffer is always full after 1000 poll");
            return ring_buffer<T>::element_buffer + ring_buffer<T>::tail;
        }
        void step_tail() override {
            ring_buffer<T>::tail = (ring_buffer<T>::tail + 1) % ring_buffer<T>::ring_size;
            ring_buffer<T>::head = ring_buffer<T>::tail;
            cpu_send_cnt++;
            if (cpu_send_cnt % 1024 == 0) {
                update_tail_with_fpga();
                update_head_with_fpga();
            }
        }
        void step_head() override {
            rt_assert(false, "never use step_head in tx_ring_buffer");
        }
    private:
        FPGACtl *fpga_ctl;
        uint32_t cpu_send_reg_index;
        uint32_t fpga_recv_reg_index;
        uint32_t cpu_send_cnt;
        uint32_t fpga_recv_cnt;
    };
    template<typename T>
    class rx_ring_buffer:public ring_buffer<T> {
    public:
        explicit rx_ring_buffer(FPGACtl *fpga_inst, uint32_t head_index, uint32_t tail_index, T *buffer, uint32_t size):ring_buffer<T>(buffer, size) {
            fpga_ctl = fpga_inst;
            head_reg_index = head_index;
            tail_reg_index = tail_index;
            ring_buffer<T>::head = show_tail_with_fpga();
        }
        ~rx_ring_buffer() override = default;
        void update_head_with_fpga() override {
            fpga_ctl->writeReg(head_reg_index, ring_buffer<T>::head);
            last_updated_head = ring_buffer<T>::head;
        }
        void update_tail_with_fpga() override {
            rt_assert(false, "never use update_tail_with_fpga in rx_ring_buffer");
        }
        T *get_head_element() {
            return ring_buffer<T>::element_buffer + ring_buffer<T>::head;
        }
        void step_head() override {
            uint32_t count = ((ring_buffer<T>::head + ring_buffer<T>::ring_size - last_updated_head) % ring_buffer<T>::ring_size);
            if (count >= 16) {
                update_head_with_fpga();
            }
            update_head_with_fpga();
            ring_buffer<T>::head = (ring_buffer<T>::head + 1) % ring_buffer<T>::ring_size;
        }
        void step_tail() override {
            rt_assert(false, "never use step_tail in rx_ring_buffer");
        }
        uint32_t show_tail_with_fpga(){
            return fpga_ctl->readReg(tail_reg_index + 512);
        }
        uint32_t show_head_with_cpu(){
            return ring_buffer<T>::head;
        }
        uint32_t show_head_with_fpga(){
            return last_updated_head;
        }
    private:
        FPGACtl *fpga_ctl;
        uint32_t head_reg_index;
        uint32_t tail_reg_index;
        uint32_t last_updated_head{};
    };
