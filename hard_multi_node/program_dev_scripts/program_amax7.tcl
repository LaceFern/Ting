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
    connect_hw_server -url TCP:192.168.189.16:3127 -cs_url TCP:localhost:3057
    # Important: FPGA to program, specify Server and FPGA number, can list using script or query from Notion
    current_hw_target 192.168.189.16:3127/xilinx_tcf/Xilinx/21770205K01JA

    open_hw_target

    current_hw_device [lindex [get_hw_devices] 0]

    refresh_hw_device -update_hw_probes false [lindex [get_hw_devices] 0]

    set_property PROGRAM.FILE $bitfile [lindex [get_hw_devices] 0]
    set_property PROBES.FILE $ltxfile [lindex [get_hw_devices] 0]

    program_hw_devices [lindex [get_hw_devices] 0]
    refresh_hw_device [lindex [get_hw_devices] 0]

    quit