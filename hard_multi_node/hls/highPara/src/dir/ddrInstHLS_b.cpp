#include "../utils.hpp"
void ddrInstHLS_b(
        hls::stream<struct_ddr_b >& 	    ddr_b,
        hls::stream<struct_dir_ack >& 	    dir_ack_f_DI_t_DC_out
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=ddr_b depth=16
#pragma HLS INTERFACE axis register both port=dir_ack_f_DI_t_DC_out depth=16
#pragma HLS aggregate variable=ddr_b compact=bit
    if(!ddr_b.empty()){
        struct_ddr_b tmp_b = ddr_b.read();
        struct_dir_ack tmp_dir_ack;
        tmp_dir_ack.ack = tmp_b.bid;
        dir_ack_f_DI_t_DC_out << tmp_dir_ack;
    }
}
