
set_param general.maxThreads 2
open_project /home/zxy/2024_vivado_pro/DSM/test_pro_multi_node_2/rc4ml_static_u280/rc4ml_static_u280.xpr
reset_run U280CoreReconfig_synth_1
reset_run impl_reconfig
launch_runs impl_reconfig -to_step write_bitstream -jobs 10
