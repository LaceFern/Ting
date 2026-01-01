#include "common.h"
#include "slabs.h"
#include "ring_buffer.h"
#include "RawMessageConnection.h"
#include "QDMAController.hpp"
#include "channel_context.h"
node_resource::node_resource(uint8_t pci_bus, size_t fpga_bridge_bar_size, size_t cpu_pool_size, double factor) {
    FPGACtl::explictInit(pci_bus, fpga_bridge_bar_size);
    fpga_ctl = FPGACtl::getInstance(pci_bus);
    cpu_mem_ctl = CPUMemCtl::getInstance(cpu_pool_size); 
    cpu_mem_ctl->writeTLB([=](uint32_t page_index, uint32_t page_size, uint64_t vaddr, uint64_t paddr) {
        (void)page_size;
        fpga_ctl->writeReg(8, (uint32_t)(vaddr));
        fpga_ctl->writeReg(9, (uint32_t)((vaddr) >> 32));
        fpga_ctl->writeReg(10, (uint32_t)(paddr));
        fpga_ctl->writeReg(11, (uint32_t)((paddr) >> 32));
        fpga_ctl->writeReg(12, (page_index == 0));
        fpga_ctl->writeReg(13, 1);
        fpga_ctl->writeReg(13, 0);
        });
    uint32_t total_qs = 4;
    int pfch_tag_reg_idx = 98;
    int tag_index_reg_idx = 99;
    for (uint32_t i = 0; i < total_qs; i++) {
        fpga_ctl->writeConfig(0x1408 / 4, i);
        uint32_t tag = fpga_ctl->readConfig(0x140c / 4);
        fpga_ctl->writeReg(pfch_tag_reg_idx, tag);
        fpga_ctl->writeReg(tag_index_reg_idx, i + 1);
        printf("%u\n", tag & 0x7f);
    }    
    fpga_pci_bus = pci_bus;
    fpga_bridge_size = fpga_bridge_bar_size;
    pool_size = cpu_pool_size;
    assert(cpu_mem_ctl->base_addr);
    sb.slabs_init(GB(1), factor, cpu_mem_ctl->base_addr);
}
node_resource::~node_resource() {
    delete fpga_ctl;
    delete cpu_mem_ctl;
    delete system_channel;
    printf("destory node_resource");
}
uint8_t node_resource::get_fpga_pci() {
    return fpga_pci_bus;
}
void *node_resource::get_data_base_addr() {
    return  reinterpret_cast<void *>(reinterpret_cast<uint64_t>(cpu_mem_ctl->base_addr) + GB(1));
}
void node_resource::init_system_channel() {
    if (system_channel_inited) {
        printf("system channel already inited");
        assert(false);
    }
    system_channel_inited = true;
    system_channel = new channel_context(this, 32);
    printf("init system channel\n");
}
std::map<uint8_t, node_resource *> node_resource_factory::node_resources;
spinlock node_resource_factory::lock;
node_resource * node_resource_factory::get_node_resource(const DSMConfig &config) {
    lock.lock();
    if (node_resources.find(config.fpga_pci_bus) == node_resources.end()) {
        node_resources[config.fpga_pci_bus] = new node_resource(config.fpga_pci_bus, config.fpga_bridge_bar_size, config.cpu_pool_size, config.factor);
        printf("create node_resource %u\n", config.fpga_pci_bus);
    }
    lock.unlock();
    return node_resources[config.fpga_pci_bus];
}
void node_resource_factory::destory_node_resource(uint8_t pci_bus) {
    lock.lock();
    if (node_resources.find(pci_bus) != node_resources.end()) {
        delete node_resources[pci_bus];
        node_resources.erase(pci_bus);
        printf("destory node_resource %u", pci_bus);
    }
    lock.unlock();
}