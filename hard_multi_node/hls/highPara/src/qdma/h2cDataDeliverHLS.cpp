#include "../utils.hpp"
void h2cDataDeliverHLS(
    hls::stream<struct_h2c_data >& 	h2c_data_f_QS_t_DD_in,
    hls::stream<struct_h2c_data >& 	h2c_data_f_DD_t_DMF_out
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=h2c_data_f_QS_t_DD_in depth=16
#pragma HLS INTERFACE axis register both port=h2c_data_f_DD_t_DMF_out depth=16
#pragma HLS aggregate variable=h2c_data_f_QS_t_DD_in compact=bit
#pragma HLS aggregate variable=h2c_data_f_DD_t_DMF_out compact=bit
    if(!h2c_data_f_QS_t_DD_in.empty()){
        struct_h2c_data tmp_h2c_data = h2c_data_f_QS_t_DD_in.read();
        h2c_data_f_DD_t_DMF_out << tmp_h2c_data;
    }
}