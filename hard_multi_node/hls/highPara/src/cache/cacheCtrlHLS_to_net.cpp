#include "../utils.hpp"
void cacheCtrlHLS_to_net(
		hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_CMD_t_CC_in,
                hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_CC_t_CC_in,
                hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_CC_t_CMC_out
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_CMD_t_CC_in depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_CC_t_CC_in depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_CC_t_CMC_out depth=16
#pragma HLS aggregate variable=ctrl_msg_f_CMD_t_CC_in compact=bit
#pragma HLS aggregate variable=ctrl_msg_f_CC_t_CC_in compact=bit
#pragma HLS aggregate variable=ctrl_msg_f_CC_t_CMC_out compact=bit
        if(!ctrl_msg_f_CC_t_CC_in.empty()){
                struct_ctrl_msg tmp_ctrl_msg = ctrl_msg_f_CC_t_CC_in.read();
                ctrl_msg_f_CC_t_CMC_out << tmp_ctrl_msg;
        }
        else if(!ctrl_msg_f_CMD_t_CC_in.empty()){
                struct_ctrl_msg tmp_ctrl_msg = ctrl_msg_f_CMD_t_CC_in.read();
                tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.dir_gblock_nodeID;
                ctrl_msg_f_CC_t_CMC_out << tmp_ctrl_msg;
        }
}