## Parameter Configuration
1. The value of `--fpga_pci_bus` can be obtained using the following shell command:
```shell
lspci | grep Xilinx
```
2. `--macs` specifies the MAC addresses stored in the witch for each participating node

## Environment
1. The switch needs to support FPGA MAC-based forwarding
2. FPGA needs to be programmed with bit file. Programming method see hard_multi_node/program_dev_scripts/README.md

