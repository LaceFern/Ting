#include "../utils.hpp"
void ctrlMsgCopy(
        hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_CC_t_CMC_in,
        hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_DC_t_CMC_in,
		hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_CMC_t_DMF_out	
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_CC_t_CMC_in depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_DC_t_CMC_in depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_CMC_t_DMF_out depth=16
#pragma HLS aggregate variable=ctrl_msg_f_CC_t_CMC_in compact=bit
#pragma HLS aggregate variable=ctrl_msg_f_DC_t_CMC_in compact=bit
#pragma HLS aggregate variable=ctrl_msg_f_CMC_t_DMF_out compact=bit
    static ap_uint<1> reg_flag = 0;
    ap_uint<1> wire_flag = reg_flag;
	ap_uint<1> tmp_ctrl_msg_flag = 0;
	struct_ctrl_msg tmp_ctrl_msg;
	if(!ctrl_msg_f_CC_t_CMC_in.empty() && ctrl_msg_f_DC_t_CMC_in.empty()){
        tmp_ctrl_msg = ctrl_msg_f_CC_t_CMC_in.read();
		tmp_ctrl_msg_flag = 1;
        reg_flag = 0;
    }
    else if(ctrl_msg_f_CC_t_CMC_in.empty() && !ctrl_msg_f_DC_t_CMC_in.empty()){
        tmp_ctrl_msg = ctrl_msg_f_DC_t_CMC_in.read();
		tmp_ctrl_msg_flag = 1;
        reg_flag = 1;
    }
    else if(!ctrl_msg_f_CC_t_CMC_in.empty() && !ctrl_msg_f_DC_t_CMC_in.empty()){
        if(wire_flag == 1){
            tmp_ctrl_msg = ctrl_msg_f_CC_t_CMC_in.read();
			tmp_ctrl_msg_flag = 1;
            reg_flag = 0;
        }
        else{
            tmp_ctrl_msg = ctrl_msg_f_DC_t_CMC_in.read();
			tmp_ctrl_msg_flag = 1;
            reg_flag = 1;
        }
    }
	if(tmp_ctrl_msg_flag == 1){
		ctrl_msg_f_CMC_t_DMF_out << tmp_ctrl_msg;
	}
}
