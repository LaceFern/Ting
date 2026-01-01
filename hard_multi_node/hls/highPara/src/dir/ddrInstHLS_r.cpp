#include "../utils.hpp"
void ddrInstHLS_r(
		hls::stream<struct_dir_data_e >& 		dir_data_f_DI_t_DC_out,     
        hls::stream<struct_ddr_r >& 	    ddr_r
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=dir_data_f_DI_t_DC_out depth=16
#pragma HLS INTERFACE axis register both port=ddr_r depth=16
#pragma HLS aggregate variable=dir_data_f_DI_t_DC_out compact=bit
#pragma HLS aggregate variable=ddr_r compact=bit
	static ap_uint<1> reg_end_flag = 1;
	static struct_dir_data_e reg_dir_data;
#pragma HLS reset variable=reg_end_flag
#pragma HLS reset variable=reg_dir_data
	ap_uint<1> wire_end_flag = reg_end_flag;
	struct_dir_data_e wire_dir_data = reg_dir_data;
	if(wire_end_flag == 1){
		if(!ddr_r.empty()){
			struct_ddr_r tmp_r = ddr_r.read();
			ddr_r_to_dir_data_e_part1(tmp_r, reg_dir_data);
			reg_end_flag = 0;
		}
	}
	else{
		if(!ddr_r.empty()){
			struct_ddr_r tmp_r = ddr_r.read();
			ddr_r_to_dir_data_e_part2(tmp_r, wire_dir_data);
			dir_data_f_DI_t_DC_out << wire_dir_data;
			reg_end_flag = 1;
		}
	}
}
