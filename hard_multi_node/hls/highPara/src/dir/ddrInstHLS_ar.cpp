#include "../utils.hpp"
void ddrInstHLS_ar(
        hls::stream<struct_dir_cmd >& 	dir_cmd_f_DC_t_DI_in,
        hls::stream<struct_ddr_ar >& 	ddr_ar
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=dir_cmd_f_DC_t_DI_in depth=16
#pragma HLS INTERFACE axis register both port=ddr_ar depth=16
#pragma HLS aggregate variable=dir_cmd_f_DC_t_DI_in compact=bit
#pragma HLS aggregate variable=ddr_ar compact=bit
	if(!dir_cmd_f_DC_t_DI_in.empty()){
		struct_dir_cmd tmp_dir_cmd = dir_cmd_f_DC_t_DI_in.read();
        struct_ddr_ar tmp_ar;
		tmp_ar.araddr	= tmp_dir_cmd.dir_gblock_index * 128;
		tmp_ar.arburst	= 1;
		tmp_ar.arcache	= 0;
		tmp_ar.arid	= 1;
		tmp_ar.arlen	= 1; 
		tmp_ar.arlock	= 0;
		tmp_ar.arprot	= 0;
		tmp_ar.arqos	= 0;
		tmp_ar.arsize	= 6;
		ddr_ar << tmp_ar;
	}
}
