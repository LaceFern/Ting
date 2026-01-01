#include "../utils.hpp"
#include "../dsmtype.hpp"
void c2hCtrlMsgDeliverHLS(
                ap_uint<32>			q_num,
                hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_CC_t_CMD_in,
                hls::stream<struct_c2h_data >&  c2h_data_f_CMD_t_ABM_out,
                hls::stream<struct_c2h_data >&  c2h_data_f_CMD_t_SBM_out
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE ap_none port=q_num
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_CC_t_CMD_in depth=16
#pragma HLS INTERFACE axis register both port=c2h_data_f_CMD_t_ABM_out depth=16
#pragma HLS INTERFACE axis register both port=c2h_data_f_CMD_t_SBM_out depth=16
#pragma HLS aggregate variable=ctrl_msg_f_CC_t_CMD_in compact=bit
#pragma HLS aggregate variable=c2h_data_f_CMD_t_ABM_out compact=bit
#pragma HLS aggregate variable=c2h_data_f_CMD_t_SBM_out compact=bit
	static ap_uint<32> reg_q_id = 0;
#pragma HLS reset variable=reg_q_id
	ap_uint<32> wire_q_id = reg_q_id;
        if(!ctrl_msg_f_CC_t_CMD_in.empty()){
                struct_ctrl_msg tmp_ctrl_msg = ctrl_msg_f_CC_t_CMD_in.read();
                struct_c2h_data tmp_c2h_data;
                if(tmp_ctrl_msg.dst_thread_type == THREAD_TYPE_APP){
                        ctrl_msg_to_c2h_data(tmp_ctrl_msg, tmp_c2h_data, PSEUDO_QID_BMAPP); 
                        tmp_c2h_data.ctrl_qid = tmp_ctrl_msg.req_cline_appThreadID;
                        c2h_data_f_CMD_t_ABM_out << tmp_c2h_data;   
                }
                else{
                        ap_uint<32> tmp_q_id = 0;
		        if(wire_q_id >= q_num - 1){
                                tmp_q_id = 0;
		        	reg_q_id = 0;
		        }
		        else{
                                tmp_q_id = wire_q_id;
		        	reg_q_id = wire_q_id + 1;
		        }
                        ctrl_msg_to_c2h_data(tmp_ctrl_msg, tmp_c2h_data, PSEUDO_QID_BMSYS); 
                        tmp_c2h_data.ctrl_qid = tmp_q_id;
                        c2h_data_f_CMD_t_SBM_out << tmp_c2h_data;
                }
        }       
}