## FPGA Reboot Method Without Power Off

After system startup, first time programming FPGA requires reboot. When FPGA is running normally as PCIe EP, run resetFPGA.py before programming new bitstream, follow the prompts

## Multi-Machine Parallel Bit File Programming Method

Updated 2025/8/21:
    1. Configure and run python3 program_device_and_reboot_server.py on stat server (192.168.189.99), skip the following Steps
    2. Although script supports programming multiple devices simultaneously, it's better to program one by one to reduce Vivado hang probability
    3. .bit and .ltx files programmed into FPGA in Python script can be copied from /home/zxy/2024_vivado_pro/DSM/bitstream/StaticU280Top.bit, /home/zxy/2024_vivado_pro/DSM/bitstream/StaticU280Top.ltx

Step 1. Run the following code on debug machine to start multiple hardware servers (sm has already started them, no need to repeat)

```shell
    ./hw_server -s tcp::3121 -e "set jtag-port-filter 21760297J03YA" & 
    ./hw_server -s tcp::3122 -e "set jtag-port-filter 21770205K02TA" -p 0 & (Note: amax2 missing)
    ./hw_server -s tcp::3123 -e "set jtag-port-filter 21760297J03CA" -p 0 & 
    ./hw_server -s tcp::3124 -e "set jtag-port-filter 21770205K02VA" -p 0 & 
    ./hw_server -s tcp::3125 -e "set jtag-port-filter 21770205K02JA" -p 0 & 
    ./hw_server -s tcp::3126 -e "set jtag-port-filter 21770205K01EA" -p 0 & 
    ./hw_server -s tcp::3127 -e "set jtag-port-filter 21770202700VA" -p 0 & 
    ./hw_server -s tcp::3128 -e "set jtag-port-filter 21770205K022A" -p 0 
```

Step 2. Write single-machine programming TCL script for each machine to program FPGA (can be placed in nfs)

```shell
    set bitfile [lindex $argv 0]
    set ltxfile [lindex $argv 1]

    if { [file exists $bitfile] != 1 } {
        puts "No bitfile $bitfile"
        quit
    } else {
        puts "Using bitfile $bitfile"
    }

    if { [file exists $ltxfile] != 1 } {
        puts "No bitfile $ltxfile"
        quit
    } else {
        puts "Using bitfile $ltxfile"
    }

    open_hw_manager
    # Important: url is IP and port of hw_server (corresponds to Step 1), different for each machine
    # Important: cs_url is IP and port of cs_server (corresponds to Step 3), different for each machine
    connect_hw_server -url TCP:192.168.189.16:3121 -cs_url TCP:localhost:3051
    # Important: FPGA to program, specify Server and FPGA number, can list using script or query from Notion
    current_hw_target 192.168.189.16:3121/xilinx_tcf/Xilinx/21760297J03YA

    open_hw_target

    current_hw_device [lindex [get_hw_devices] 0]

    refresh_hw_device -update_hw_probes false [lindex [get_hw_devices] 0]

    set_property PROGRAM.FILE $bitfile [lindex [get_hw_devices] 0]
    set_property PROBES.FILE $ltxfile [lindex [get_hw_devices] 0]

    program_hw_devices [lindex [get_hw_devices] 0]
    refresh_hw_device [lindex [get_hw_devices] 0]

    quit
```

Step 3. Run the following script on one of the machines to program FPGA to start multiple cs servers

```shell
    #!/bin/bash
    cs_server -s TCP::3051 -I 60&
    cs_server -s TCP::3052 -I 60&
    cs_server -s TCP::3053 -I 60&
    cs_server -s TCP::3054 -I 60&
    cs_server -s TCP::3055 -I 60&
    cs_server -s TCP::3056 -I 60&
    cs_server -s TCP::3057 -I 60&
    cs_server -s TCP::3058 -I 60&
    cs_server -s TCP::3059 -I 60&

    vivado -mode tcl -nolog -nojournal -source ./program_amax1.tcl -tclargs ./StaticU280Top.bit ./StaticU280Top.ltx&
    vivado -mode tcl -nolog -nojournal -source ./program_amax2.tcl -tclargs ./StaticU280Top.bit ./StaticU280Top.ltx&
    vivado -mode tcl -nolog -nojournal -source ./program_amax3.tcl -tclargs ./StaticU280Top.bit ./StaticU280Top.ltx&
    vivado -mode tcl -nolog -nojournal -source ./program_amax4.tcl -tclargs ./StaticU280Top.bit ./StaticU280Top.ltx&
    vivado -mode tcl -nolog -nojournal -source ./program_amax5.tcl -tclargs ./StaticU280Top.bit ./StaticU280Top.ltx&
    vivado -mode tcl -nolog -nojournal -source ./program_amax6.tcl -tclargs ./StaticU280Top.bit ./StaticU280Top.ltx&
    vivado -mode tcl -nolog -nojournal -source ./program_amax7.tcl -tclargs ./StaticU280Top.bit ./StaticU280Top.ltx&
    vivado -mode tcl -nolog -nojournal -source ./program_amax8.tcl -tclargs ./StaticU280Top.bit ./StaticU280Top.ltx&
```

Step 4. How to Login to Corresponding HD Server GUI Interface to View ILA Signals

sm started hd server on debug machine in machine room, amax1~8 correspond to 10.12.86.210:13121~13128, directly connect local vivado to above IP and port to view ILA and VIO


Notes:

First, single-machine programming script needs to specify corresponding IP and port, then execute in parallel.

[Confirmed] Currently this script will have some output issues after execution, such as pipe and EOF problems, can be ignored.

If corresponding cs_server doesn't automatically exit after idle event, and next programming successful count is less than specified total, can use htop command with F9 to quickly send SIGTERM signal to kill stuck cs_server to resolve. (Low probability)
