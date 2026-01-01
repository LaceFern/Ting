## Generate simTop for hls module

```bash
python3 ./gen_chisel_from_hls_wrapper.py
```

## Step-By-Step Tutorial to Build a reconfigurable Project with Static IO part

https://github.com/RC4ML/rc4ml_static/blob/16ee581b48f2ad823337872864376e1f3160da3b/docs/HOWTOUSE.md#step-by-step-tutorial-to-build-a-project

## Complete Reconfigurable Part with Chisel

Clean the source file of the static part & Config complete_vpro_with_chisel.py & Run the following code

```bash
python3 ./complete_vpro_with_chisel.py
```

## Complete Reconfigurable Part with HLS

See hls/README.md

## Generate Bitstream for Reconfigurable Part

https://github.com/RC4ML/qdma/tree/master?tab=readme-ov-file#partial-reconfiguration-workflow-1

Notes:

-2. 【Chisel Tips】Note that the ILA clock needs to match the clock of the captured signal (especially pay attention to DDR module)

-1. 【Chisel Tips】Bus/Signal name conflict! Duplicated buses/signals named io_out_ready in ila_ddr detected!
    Reason: The ILA module names captured signals starting from the "io." field of the signal

0. 【Routing Tips】userclk adjustment is in <rc4ml_static_repo>/static/src/u280/StaticU280Top.scala (usually 200M is sufficient)

0. 【Routing Tips】Enabling performance_extraTimingOpt optimization helps with timing

0. 【Routing Tips】ILA capture of static modules (e.g., DDR, QDMA) may cause routing difficulties, should add ILA signals directly when compiling static base project


1. 【Process Experience】Remember to check if all modules are present in the project and if the correct top file is selected (if AlveoDynamicTop in U280DynamicGreyBox.sv at two different locations conflicts, the wrong AlveoDynamicTop may be selected as top, where sub-modules appear gray, meaning they are not included by this top but are included by another module with the same name)

2. 【Process Experience】When recompiling Reconfigurable Part, need to Reset Runs for Synthesis Out-of-date U280CoreReconfig_synth_1 first!

3. 【Process Experience】stat machine vivado gui interface often crashes, can run via command line (see "Reference TCL Commands")

4. 【Programming Experience】After first programming and reboot, can check if host discovered FPGA via lspci | grep Xilinx

5. 【Process Experience】After stat machine power cycle restart, can sudo reboot first to increase system stability

Reference TCL Commands
```shell
vivado -mode tcl
set_param general.maxThreads 1
open_project /home/zxy/2024_vivado_pro/DSM/test_pro_multi_node_2/rc4ml_static_u280/rc4ml_static_u280.xpr

############ S T A T I C #############

# Re-synthesize and re-implement static part
reset_run synth_1
reset_run U280CoreBase_synth_1
reset_run impl_base

# impl_base from current step to end
launch_runs impl_base -to_step write_bitstream -jobs 2

# Re-implement from previous step
reset_run impl_base -prev_step 
# reset_run impl_base -from_step route_design
launch_runs impl_base -next_step 

############ D Y N A M I C #############

# Re-synthesize and re-implement dynamic part
reset_run U280CoreReconfig_synth_1
reset_run impl_reconfig

# Re-implement from previous step
reset_run impl_reconfig -prev_step 
# reset_run impl_base -from_step route_design
launch_runs impl_reconfig -next_step

# impl_reconfig from current step to end
launch_runs impl_reconfig -to_step write_bitstream -jobs 2

```

Reference Log Locations
[Thu Aug 14 15:54:19 2025] Launched U280CoreReconfig_synth_1...
Run output will be captured here: /home/zxy/2024_vivado_pro/DSM/test_pro_multi_node_2/rc4ml_static_u280/rc4ml_static_u280.runs/U280CoreReconfig_synth_1/runme.log
[Thu Aug 14 15:54:19 2025] Launched impl_reconfig...
Run output will be captured here: /home/zxy/2024_vivado_pro/DSM/test_pro_multi_node_2/rc4ml_static_u280/rc4ml_static_u280.runs/impl_reconfig/runme.log


Reference Background Running Commands (Note: check if function is full rerun or return to previous step)
```shell
# Select any amax server
ssh 192.168.189.99
# Run script (background)
python3 run_vivado.py
# Check log information to see progress
tail -n 100 /home/zxy/2024_vivado_pro/DSM/test_pro_multi_node_2/rc4ml_static_u280/rc4ml_static_u280.runs/impl_reconfig/runme.log
# If problems occur, use to kill process
pkill -f run_vivado.py
killall vivado
```

Reference Moving Project to Another Machine for Download
```shell
zip -r test_pro_multi_node_2.zip 2024_vivado_pro/DSM/test_pro_multi_node_2
ls -lh test_pro_multi_node_2.zip
scp -r test_pro_multi_node_2.zip  zxy@192.168.189.12:~/
```
