
## Overview

FPGA-DSM provides:

- **Distributed Shared Memory**: Global memory abstraction across multiple nodes
- **Cache Coherence**: MSI protocol-based cache coherence maintained by FPGA
- **Memory Allocators**: 
  - GlobalAllocator: Coarse-grained chunk-level allocation
  - LocalAllocator: Fine-grained local memory management
- **TPCC Database**: Transactional database engine built on top of DSM
- **Multi-node Support**: Tested with up to 8 nodes

## Project Structure

```
FPGA-DSM/
├── soft_multi_node/          # Software implementation
│   ├── include/              # DSM headers
│   ├── src/                  # DSM implementation
│   ├── database/             # TPCC database implementation
│   ├── scripts/              # Testing and deployment scripts
│   ├── test/                 # Test programs
│   └── CMakeLists.txt        # Build configuration
│
├── hard_multi_node/          # Hardware implementation
│   ├── hls/                  # High-Level Synthesis code
│   ├── chisel/               # Chisel hardware description
│   └── program_dev_scripts/  # FPGA programming scripts
│
└── README.md                 # This file
```

## Prerequisites

### Software Requirements

- **Operating System**: Linux (tested on Ubuntu 18.04+)
- **Compiler**: GCC 7+ with C++14 support
- **Build Tools**: 
  - CMake 3.10+
  - Make
- **Python**: Python 3.6+ (for scripts)
- **Dependencies**:
  ```bash
  # Ubuntu/Debian
  sudo apt-get update
  sudo apt-get install cmake build-essential libnuma-dev
  sudo apt-get install libibverbs-dev libmemcached-dev
  sudo apt-get install libboost-filesystem-dev libboost-system-dev
  sudo apt-get install libboost-date-time-dev libboost-thread-dev
  sudo apt-get install python3-pip
  pip3 install paramiko tabulate
  ```
  
### Hardware Requirements

- **FPGA**: Xilinx Alveo U280 (or compatible)
- **Network**: A switch with static MAC forwarding support (ARP is not implemented on FPGAs)
- **Memory**: Hugepages enabled (see Setup section)


## Hardware Development

### Building Hardware

1. **HLS IP Generation**: See `hard_multi_node/hls/README.md`
   ```bash
   cd hard_multi_node/hls
   source /tools/Xilinx/Vitis_HLS/2021.1/settings64.sh
   python3 gen_IP_and_wrapper_adv.py
   ```

2. **Complete Vivado Project with HLS**:
   ```bash
   python3 complete_vpro_with_hls.py
   ```

3. **Chisel Wrapper Generation**: See `hard_multi_node/chisel/README.md`
   ```bash
   cd hard_multi_node/chisel
   python3 gen_chisel_from_hls_wrapper.py
   ```

4. **Complete Vivado Project with Chisel**:
   ```bash
   python3 complete_vpro_with_chisel.py
   ```

5. **Generate Bitstream**: See `hard_multi_node/chisel/README.md`

See `hard_multi_node/README.md` for complete hardware development workflow.


### Programming FPGA

**First Time Programming (Requires Reboot):**

```bash
cd hard_multi_node/program_dev_scripts
python3 resetFPGA.py
# Follow prompts
```

**Multi-Device Programming:**

See `hard_multi_node/program_dev_scripts/README.md` for detailed instructions.

For quick programming:
```bash
cd hard_multi_node/program_dev_scripts
python3 program_device_and_reboot_server.py
```

**Important Notes:**
- After system startup, first-time FPGA programming requires reboot
- When FPGA is running normally, run `resetFPGA.py` before programming new bitstream
- Bitstream files: `/home/zxy/2024_vivado_pro/DSM/bitstream/StaticU280Top.bit`
- Probe files: `/home/zxy/2024_vivado_pro/DSM/bitstream/StaticU280Top.ltx`

### Verifying FPGA Status

```bash
# Check if FPGA is detected
lspci | grep Xilinx

# Check QDMA driver
lsmod | grep qdma
# If not loaded, load driver:
sudo insmod /path/to/qdma-pf.ko
```


## Software Development

### Software Build

```bash
cd soft_multi_node
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```


## Running the System

### Getting FPGA PCI Bus ID

```bash
lspci | grep Xilinx
# Example output: 01:00.0 Processing accelerators: Xilinx Corporation Device 5000
# Use 0x01 as the fpga_pci_bus value
```

### Command-Line Arguments

Common arguments for test programs:

- `--node_num N`: Total number of nodes in cluster
- `--node_id ID`: This node's ID (0 to node_num-1)
- `--appt_num N`: Number of application threads per node
- `--syst_num N`: Number of system threads per node (typically 8)
- `--read_ratio R`: Read ratio percentage (0-100)
- `--shared_ratio R`: Sharing ratio percentage (0-100)
- `--fpga_pci_bus 0xXX`: FPGA PCI bus ID in hex
- `--macs MAC1,MAC2,...`: MAC addresses for all nodes (comma-separated)
- `--warmup_type T`: Warmup type (0=read, 1=write, 2=mixed)
- `--test_type T`: Test type (0=read, 1=write, 2=mixed)




**Project Status**: Active Development
