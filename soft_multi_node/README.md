## Parameter Configuration
1. The value of `--fpga_pci_bus` can be obtained using the following shell command:
2. `--macs` specifies the MAC addresses stored in the P4 switch for each participating node (e.g., amax5 corresponds to MAC 05:05:05:05:05:05, not 01:01:01:01:01:01)
```shell
lspci | grep Xilinx
```
## Environment
1. FPGA needs to be connected to P4 switch (because ARP and other functions are not implemented), and the P4 switch needs to support FPGA MAC-based forwarding
2. FPGA needs to be programmed with bit file. Programming method see hard_multi_node/program_dev_scripts/README.md

## Testing Notes
1. Check if the P4 switch is running P4-Test code (supports FPGA forwarding)
2. Check if OPR and RDY are enabled on related machines
3. If the system hangs due to P4 switch not supporting FPGA forwarding, it needs to be reprogrammed (because the system may have received some strange packets and these packets could not be processed due to system error interrupts)
