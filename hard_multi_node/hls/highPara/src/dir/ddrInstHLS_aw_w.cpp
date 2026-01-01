#include "../utils.hpp"
void ddrInstHLS_aw_w(
		hls::stream<struct_dir_data_e >& 		dir_data_f_DC_t_DI_in,
        hls::stream<struct_ddr_aw >& 	    ddr_aw,
        hls::stream<struct_ddr_w >& 	    ddr_w
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=dir_data_f_DC_t_DI_in depth=16
#pragma HLS INTERFACE axis register both port=ddr_aw depth=16
#pragma HLS INTERFACE axis register both port=ddr_w depth=16
#pragma HLS aggregate variable=dir_data_f_DC_t_DI_in compact=bit
#pragma HLS aggregate variable=ddr_aw compact=bit
#pragma HLS aggregate variable=ddr_w compact=bit
	static ap_uint<1> reg_end_flag = 1;
	static struct_dir_data_e reg_dir_data;
#pragma HLS reset variable=reg_end_flag
#pragma HLS reset variable=reg_dir_data
	ap_uint<1> wire_end_flag = reg_end_flag;
	struct_dir_data_e wire_dir_data = reg_dir_data;
	if(wire_end_flag == 1){
		if(!dir_data_f_DC_t_DI_in.empty()){
			struct_dir_data_e tmp_dir_data = dir_data_f_DC_t_DI_in.read();
    	    struct_ddr_aw tmp_aw;
			tmp_aw.awaddr	= tmp_dir_data.dir_gblock_index * 128;
			tmp_aw.awburst	= 1;
			tmp_aw.awcache	= 0;
			tmp_aw.awid	= 1;
			tmp_aw.awlen	= 1; 
			tmp_aw.awlock	= 0;
			tmp_aw.awprot	= 0;
			tmp_aw.awqos	= 0;
			tmp_aw.awsize	= 6;
			ddr_aw << tmp_aw;
    	    struct_ddr_w tmp_w;
			dir_data_e_to_ddr_w_part1(tmp_dir_data, tmp_w);
			ddr_w << tmp_w;
			reg_dir_data = tmp_dir_data;
			reg_end_flag = 0;
		}
	}
	else{
    	struct_ddr_w tmp_w_addition;
		dir_data_e_to_ddr_w_part2(wire_dir_data, tmp_w_addition);
		ddr_w << tmp_w_addition;
		reg_end_flag = 1;
	}
}
