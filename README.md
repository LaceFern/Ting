# FPGA-DSM: Distributed Shared Memory on FPGA

## Prerequisites

### Software Requirements

- **Operating System**: Linux (tested on Ubuntu 18.04+)
- **Compiler**: GCC 7+ with C++14 support
- **Build Tools**: 
  - CMake 3.10+
  - Make
- **Dependencies**:
  ```bash
  # Ubuntu/Debian
  sudo apt-get install cmake build-essential libnuma-dev
  sudo apt-get install libibverbs-dev libmemcached-dev
  sudo apt-get install libboost-filesystem-dev libboost-system-dev
  sudo apt-get install libboost-date-time-dev libboost-thread-dev
  ```

### Hardware Requirements

- **FPGA**: Xilinx Alveo U280 (or compatible)
- **Network**: P4 switch with FPGA MAC-based forwarding support
- **Memory**: Hugepages enabled
### FPGA Setup

1. FPGA must be connected to P4 switch (ARP not implemented)
2. P4 switch must support FPGA MAC-based forwarding
3. FPGA must be programmed with bitstream