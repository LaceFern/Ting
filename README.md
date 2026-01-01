
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

- **FPGA**: Xilinx Alveo U280
- **Network**:  Switch that supports statically configured MAC-based forwarding
