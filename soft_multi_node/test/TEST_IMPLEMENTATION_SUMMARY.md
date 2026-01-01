# Test File Implementation Summary

## Overview

Successfully adapted test files from the `tmp-test` directory to the `FPGA-DSM-merged` project. All test files are placed in the `soft_multi_node/test` directory.

## Implemented Test Files

### 1. alloc_test.cpp
**Functionality**: Test memory allocation functionality
**Main test content**:
- Allocate multiple chunk-sized memory blocks
- Allocate memory blocks of different sizes (10247 bytes, 11 bytes, etc.)
- Verify allocated address correctness

**Usage**:
```bash
./alloc_test --node_num 3 --node_id 1 --appt_num 1 --syst_num 8 --fpga_pci_bus 0x43 --macs 01:01:01:01:01:01,02:02:02:02:02:02,03:03:03:03:03:03
```

### 2. lock_test.cpp
**Functionality**: Test read-write lock functionality
**Main test content**:
- Multiple nodes acquiring read locks
- Write lock blocking test
- Lock release and acquisition order

**Usage**:
```bash
./lock_test --node_num 3 --node_id 0 --appt_num 1 --syst_num 8 --fpga_pci_bus 0x43
```

### 3. evict_test.cpp
**Functionality**: Test cache eviction functionality
**Main test content**:
- Trigger cache eviction by reading multiple different addresses
- Verify cache replacement policy

**Usage**:
```bash
./evict_test --node_num 3 --node_id 0 --appt_num 1 --syst_num 8 --fpga_pci_bus 0x43
```

### 4. write_miss_dirty.cpp
**Functionality**: Test write miss dirty page scenario
**Main test content**:
- Node 0 initializes data
- Node 1 and Node 2 write to same address
- Verify cache coherence protocol during write miss

**Usage**:
```bash
./write_miss_dirty --node_num 3 --node_id 0 --appt_num 1 --syst_num 8 --fpga_pci_bus 0x43
```

### 5. write_miss_shared.cpp
**Functionality**: Test write miss shared page scenario
**Main test content**:
- Node 2 initializes shared data
- Multiple nodes read shared data
- Node 1 writes shared data, triggering write miss shared state transition

**Usage**:
```bash
./write_miss_shared --node_num 3 --node_id 0 --appt_num 1 --syst_num 8 --fpga_pci_bus 0x43
```

### 6. write_shared.cpp
**Functionality**: Test write shared page scenario
**Main test content**:
- Node 0 initializes shared data
- Multiple nodes read shared data
- Node 1 writes shared data

**Usage**:
```bash
./write_shared --node_num 3 --node_id 0 --appt_num 1 --syst_num 8 --fpga_pci_bus 0x43
```

### 7. read_miss_dirty.cpp
**Functionality**: Test read miss dirty page scenario
**Main test content**:
- Node 0 initializes and writes data
- Node 1 writes to same address (makes data dirty)
- Node 2 reads dirty data (triggers read miss dirty page handling)

**Usage**:
```bash
./read_miss_dirty --node_num 3 --node_id 0 --appt_num 1 --syst_num 8 --fpga_pci_bus 0x43
```

### 8. benchmark.cpp
**Functionality**: Performance benchmark test
**Main test content**:
- Multi-threaded concurrent read-write test
- Configurable read-write ratio, locality, sharing
- Throughput and latency statistics

**Usage**:
```bash
./benchmark --node_num 4 --node_id 0 --appt_num 4 --read_ratio 60 --locality 50 --sharing 30 --fpga_pci_bus 0x43
```

## Adaptation Points

### 1. DSM Initialization Adaptation
Original test used simple constructor:
```cpp
DSMConfig conf(CacheConfig(), 3);
```

New project uses complete parameter list:
```cpp
DSMConfig conf(node_id, node_num, appt_num, syst_num,
               gmem_size_per_node, cache_size_per_node,
               fpga_pci_bus, mac_vec);
```

### 2. Parameter Parsing
All test files added command-line parameter parsing functionality, supporting:
- `--node_num`: Number of nodes
- `--node_id`: Current node ID
- `--appt_num`: Number of application threads
- `--syst_num`: Number of system threads
- `--fpga_pci_bus`: FPGA PCI bus address (hexadecimal)
- `--macs`: MAC address list (comma-separated)

### 3. Memory Allocation
- Added `define::kChunkSize` constant definition (32MB) to `Common.h`
- Use `dsm->malloc()` for memory allocation
- Use `GADD()` macro for address offset calculation

### 4. Global Address Usage
- All tests use `GlobalAddress` structure
- Perform read-write operations via `dsm->read()` and `dsm->write()`

## Compilation Instructions

All test files are configured in `CMakeLists.txt` and will be compiled automatically. Compilation command:

```bash
cd soft_multi_node
mkdir build && cd build
cmake ..
make
```

After compilation, executables are located in the `build/` directory.

## Running Instructions

1. **Single-node testing**: Can directly run individual test files
2. **Multi-node testing**: Need to run simultaneously on multiple nodes with different `--node_id` parameters

Example (3-node test):
```bash
# Node 0
./alloc_test --node_num 3 --node_id 0 --fpga_pci_bus 0x43

# Node 1  
./alloc_test --node_num 3 --node_id 1 --fpga_pci_bus 0x44

# Node 2
./alloc_test --node_num 3 --node_id 2 --fpga_pci_bus 0x45
```

## Notes

1. **DSM Initialization**: Each test file correctly initializes DSM instance at the beginning of `main()` function
2. **Thread Registration**: Use `dsm->registerThread()` to register current thread
3. **Resource Cleanup**: Call `dsm->stop()` to cleanup resources after test completion
4. **MAC Address**: If `--macs` is not specified, default MAC addresses will be automatically generated
5. **Memory Size**: Default uses 200MB global memory and 400MB cache memory, can be adjusted by modifying code

## Differences from Original Tests

1. **Simplified functionality**: Some complex functions (e.g., Statistics, keeper synchronization) may not exist in new project, accordingly simplified
2. **Parameter passing**: Use command-line parameters instead of hard-coded configuration
3. **Error handling**: Added more error checking and log output
4. **Resource management**: Improved resource allocation and cleanup logic

## Future Extensions

Can consider adding the following tests:
- `bitmap_test.cpp`: Bitmap test
- `dht_test.cpp`: Distributed hash table test
- `pagerank_test.cpp`: PageRank algorithm test
- `controller_test.cpp`: Controller test

All test files have correctly initialized DSM and can run and test independently.
