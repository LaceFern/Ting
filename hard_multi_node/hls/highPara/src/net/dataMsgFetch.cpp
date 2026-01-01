#include "../utils.hpp"
#include "../dsmtype.hpp"
void dataMsgFetch(
		hls::stream<struct_h2c_cmd >& 	h2c_cmd_f_DMF_t_CD_out,
		hls::stream<struct_h2c_data >& 	h2c_data_f_DD_t_DMF_in,
        hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_CMC_t_DMF_in,
        hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_DMF_t_P_out,
        hls::stream<struct_data_msg >& 	data_msg_f_DMF_t_P_out,
		hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_DMF_t_DMF_out,	
		hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_DMF_t_DMF_in,
		ap_uint<8> & 	debug_state
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=h2c_cmd_f_DMF_t_CD_out depth=16
#pragma HLS INTERFACE axis register both port=h2c_data_f_DD_t_DMF_in depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_CMC_t_DMF_in depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_DMF_t_P_out depth=16
#pragma HLS INTERFACE axis register both port=data_msg_f_DMF_t_P_out depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_DMF_t_DMF_out depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_DMF_t_DMF_in depth=16
#pragma HLS aggregate variable=h2c_cmd_f_DMF_t_CD_out compact=bit
#pragma HLS aggregate variable=h2c_data_f_DD_t_DMF_in compact=bit
#pragma HLS aggregate variable=ctrl_msg_f_CMC_t_DMF_in compact=bit
#pragma HLS aggregate variable=ctrl_msg_f_DMF_t_P_out compact=bit
#pragma HLS aggregate variable=data_msg_f_DMF_t_P_out compact=bit
#pragma HLS aggregate variable=ctrl_msg_f_DMF_t_DMF_out compact=bit
#pragma HLS aggregate variable=ctrl_msg_f_DMF_t_DMF_in compact=bit
#pragma HLS INTERFACE ap_none port=debug_state
	static ap_uint<1> reg_h2c_data_end_flag = 1;
#pragma HLS reset variable=reg_h2c_data_end_flag
	ap_uint<1> wire_h2c_data_end_flag = reg_h2c_data_end_flag;
	static ap_uint<8> reg_debug_state;
	debug_state = reg_debug_state;
	if(wire_h2c_data_end_flag == 0){
		if(!h2c_data_f_DD_t_DMF_in.empty()){
			reg_debug_state = 1;
			struct_h2c_data tmp_h2c_data = h2c_data_f_DD_t_DMF_in.read();
			struct_data_msg tmp_data_msg;
			h2c_data_to_data_msg(tmp_h2c_data, tmp_data_msg);
			data_msg_f_DMF_t_P_out << tmp_data_msg;
			if(tmp_data_msg.last == 1){
				reg_h2c_data_end_flag = 1;
			}
		}
	}
	else if(wire_h2c_data_end_flag == 1){
		if(!h2c_data_f_DD_t_DMF_in.empty() && !ctrl_msg_f_DMF_t_DMF_in.empty()){
			reg_debug_state = 3;
			struct_ctrl_msg tmp_ctrl_msg = ctrl_msg_f_DMF_t_DMF_in.read();
			ctrl_msg_f_DMF_t_P_out << tmp_ctrl_msg;
			struct_h2c_data tmp_h2c_data = h2c_data_f_DD_t_DMF_in.read();
			struct_data_msg tmp_data_msg;
			h2c_data_to_data_msg(tmp_h2c_data, tmp_data_msg);
			data_msg_f_DMF_t_P_out << tmp_data_msg;
			if(tmp_data_msg.last == 0){
				reg_h2c_data_end_flag = 0;
			}
		}
		else if(!ctrl_msg_f_CMC_t_DMF_in.empty()){
			reg_debug_state = 5;
			struct_ctrl_msg tmp_ctrl_msg = ctrl_msg_f_CMC_t_DMF_in.read();
			struct_h2c_cmd tmp_h2c_cmd;
			if(tmp_ctrl_msg.mtype == RawMessageType::AGENT_2_DIR_READ_MISS_DIRTY_ACKDATA
    		    || tmp_ctrl_msg.mtype == RawMessageType::AGENT_2_APP_READ_MISS_DIRTY_ACKDATA
    		    || tmp_ctrl_msg.mtype == RawMessageType::AGENT_2_APP_WRITE_MISS_DIRTY_ACKDATA){
    		    tmp_h2c_cmd.addr = tmp_ctrl_msg.agent_cline_addr;
    		    tmp_h2c_cmd.len = DSM_CACHE_LINE_SIZE;
			    tmp_h2c_cmd.qid = PSEUDO_QID_DMF; 
				h2c_cmd_f_DMF_t_CD_out << tmp_h2c_cmd;
				tmp_ctrl_msg.data_msg_length = DSM_CACHE_LINE_SIZE;
				ctrl_msg_f_DMF_t_DMF_out << tmp_ctrl_msg;
    		}
    		else if(tmp_ctrl_msg.mtype == RawMessageType::DIR_2_APP_READ_MISS_ACKDATA
				|| tmp_ctrl_msg.mtype == RawMessageType::DIR_2_APP_WRITE_MISS_ACKDATA
				|| tmp_ctrl_msg.mtype == RawMessageType::DIR_2_APP_WRITE_MISS_SHAREDDATA){
    		    tmp_h2c_cmd.addr = tmp_ctrl_msg.dir_gblock_addr;
    		    tmp_h2c_cmd.len = DSM_CACHE_LINE_SIZE;
			    tmp_h2c_cmd.qid = PSEUDO_QID_DMF; 
				h2c_cmd_f_DMF_t_CD_out << tmp_h2c_cmd;
				tmp_ctrl_msg.data_msg_length = DSM_CACHE_LINE_SIZE;
				ctrl_msg_f_DMF_t_DMF_out << tmp_ctrl_msg;
    		}	
			else if(tmp_ctrl_msg.mtype == RawMessageType::R_EVICT_DIRTY){
    		    tmp_h2c_cmd.addr = tmp_ctrl_msg.req_cline_addr;
    		    tmp_h2c_cmd.len = DSM_CACHE_LINE_SIZE;
			    tmp_h2c_cmd.qid = PSEUDO_QID_DMF; 
				h2c_cmd_f_DMF_t_CD_out << tmp_h2c_cmd;
				tmp_ctrl_msg.data_msg_length = DSM_CACHE_LINE_SIZE;
				ctrl_msg_f_DMF_t_DMF_out << tmp_ctrl_msg;
			}
			else{
				tmp_ctrl_msg.data_msg_length = 0;
				ctrl_msg_f_DMF_t_P_out << tmp_ctrl_msg;
			}
		}
		else{
			reg_debug_state = 6;
		}
	}
	else{
		reg_debug_state = 6;
	}
}
