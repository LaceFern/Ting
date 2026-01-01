#include "../utils.hpp"
void c2hAppMultiBufferMgrHLS(
		struct_q_addr_arr<max_appt_num_per_node>	ABM_q_addr_arr,
		struct_q_tidx_arr<max_appt_num_per_node>&	ABM_q_tidx_arr,
		struct_q_hidx_arr<max_appt_num_per_node>	ABM_q_hidx_arr,
		hls::stream<struct_c2h_cmd >& 	c2h_cmd_f_ABM_t_CDD_out,
		hls::stream<struct_c2h_data >& 	c2h_data_f_ABM_t_CDD_out,
		hls::stream<struct_c2h_data >& 	c2h_data_f_CMD_t_ABM_in,
		ap_uint<32>&			ABM_debug_count
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE ap_none port=ABM_q_addr_arr
#pragma HLS INTERFACE ap_none port=ABM_q_tidx_arr
#pragma HLS INTERFACE ap_none port=ABM_q_hidx_arr
#pragma HLS INTERFACE axis register both port=c2h_cmd_f_ABM_t_CDD_out depth=16
#pragma HLS INTERFACE axis register both port=c2h_data_f_ABM_t_CDD_out depth=16
#pragma HLS INTERFACE axis register both port=c2h_data_f_CMD_t_ABM_in depth=16
#pragma HLS aggregate variable=ABM_q_addr_arr compact=bit
#pragma HLS aggregate variable=ABM_q_tidx_arr compact=bit
#pragma HLS aggregate variable=ABM_q_hidx_arr compact=bit
#pragma HLS aggregate variable=c2h_cmd_f_ABM_t_CDD_out compact=bit
#pragma HLS aggregate variable=c2h_data_f_ABM_t_CDD_out compact=bit
#pragma HLS aggregate variable=c2h_data_f_CMD_t_ABM_in compact=bit
#pragma HLS INTERFACE ap_none port=ABM_debug_count
	static ap_uint<32> reg_count = 0;
	ap_uint<32> wire_count = reg_count;
	ABM_debug_count = wire_count;
#pragma HLS reset variable=reg_count
	static struct_q_tidx_arr<max_appt_num_per_node> reg_q_tidx_arr;
#pragma HLS reset variable=reg_q_tidx_arr
#pragma HLS ARRAY_PARTITION variable=reg_q_tidx_arr.idx complete
	struct_q_tidx_arr<max_appt_num_per_node> wire_q_tidx_arr = reg_q_tidx_arr;
	ABM_q_tidx_arr = wire_q_tidx_arr;
	static ap_uint<1> reg_retry_flag;
	static struct_c2h_data reg_retry_c2h_data;
#pragma HLS reset variable=reg_retry_flag
	ap_uint<1> wire_retry_flag = reg_retry_flag;
	struct_c2h_data wire_retry_c2h_data = reg_retry_c2h_data;
	ap_uint<1> tmp_c2h_data_ready_flag = 0;
	struct_c2h_data tmp_c2h_data;
	if(wire_retry_flag){
		tmp_c2h_data = wire_retry_c2h_data;
		tmp_c2h_data_ready_flag = 1;
	}
	else if(!c2h_data_f_CMD_t_ABM_in.empty()){
		tmp_c2h_data = c2h_data_f_CMD_t_ABM_in.read();
		tmp_c2h_data_ready_flag = 1;
	}
	if(tmp_c2h_data_ready_flag){
		reg_retry_c2h_data = tmp_c2h_data;
		ap_uint<8> tmp_q_id = tmp_c2h_data.ctrl_qid;
		ap_uint<64> tmp_q_addr = ABM_q_addr_arr.addr[tmp_q_id];
		ap_uint<8> tmp_q_tidx = wire_q_tidx_arr.idx[tmp_q_id];
		ap_uint<8> tmp_q_hidx = ABM_q_hidx_arr.idx[tmp_q_id];
		ap_uint<8> tmp_q_tidx_plus1;
		if(tmp_q_tidx == max_msg_num_per_q - 1){
			tmp_q_tidx_plus1 = 0;
		}
		else{
			tmp_q_tidx_plus1 = tmp_q_tidx + 1;
		}
		struct_c2h_cmd 		tmp_c2h_cmd;
		tmp_c2h_cmd.addr	= tmp_q_addr + max_msg_length * tmp_q_tidx;
		tmp_c2h_cmd.qid		= PSEUDO_QID_BMAPP;
		tmp_c2h_cmd.len		= max_msg_length;
		tmp_c2h_data.ctrl_qid 	= PSEUDO_QID_BMAPP;
		if(tmp_q_tidx_plus1 != tmp_q_hidx){
			c2h_cmd_f_ABM_t_CDD_out 		<< tmp_c2h_cmd;
			c2h_data_f_ABM_t_CDD_out 	<< tmp_c2h_data;
			reg_q_tidx_arr.idx[tmp_q_id] = tmp_q_tidx_plus1;
			reg_retry_flag = 0;
			reg_count = wire_count + 1;
		}
		else{
			reg_retry_flag = 1;
		}
	}
}
