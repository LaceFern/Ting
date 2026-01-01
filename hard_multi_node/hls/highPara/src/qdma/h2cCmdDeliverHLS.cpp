#include "../utils.hpp"
void h2cCmdDeliverHLS(
    hls::stream<struct_h2c_cmd >& 	h2c_cmd_f_DMF_t_CD_in,
    hls::stream<struct_h2c_cmd >& 	h2c_cmd_f_CD_t_QS_out
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=h2c_cmd_f_DMF_t_CD_in depth=16
#pragma HLS INTERFACE axis register both port=h2c_cmd_f_CD_t_QS_out depth=16
#pragma HLS aggregate variable=h2c_cmd_f_DMF_t_CD_in compact=bit
#pragma HLS aggregate variable=h2c_cmd_f_CD_t_QS_out compact=bit
    if(!h2c_cmd_f_DMF_t_CD_in.empty()){
        struct_h2c_cmd tmp_h2c_cmd = h2c_cmd_f_DMF_t_CD_in.read();
        tmp_h2c_cmd.qid = 0; 
        h2c_cmd_f_CD_t_QS_out << tmp_h2c_cmd;
    }
}