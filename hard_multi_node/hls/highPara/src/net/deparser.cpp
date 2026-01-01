#include "../utils.hpp"
#include "../dsmtype.hpp"
void deparser(
		hls::stream<struct_net_rxtx >&		net_rx,
        hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_D_t_CC_out,
		hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_D_t_DC_out,
        hls::stream<struct_c2h_cmd >&  c2h_cmd_f_D_t_CDD_out,
        hls::stream<struct_c2h_data >&  c2h_data_f_D_t_CDD_out
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=net_rx depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_D_t_CC_out depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_D_t_DC_out depth=16
#pragma HLS INTERFACE axis register both port=c2h_cmd_f_D_t_CDD_out depth=16
#pragma HLS INTERFACE axis register both port=c2h_data_f_D_t_CDD_out depth=16
#pragma HLS aggregate variable=net_rx compact=bit
#pragma HLS aggregate variable=ctrl_msg_f_D_t_CC_out compact=bit
#pragma HLS aggregate variable=ctrl_msg_f_D_t_DC_out compact=bit
#pragma HLS aggregate variable=c2h_cmd_f_D_t_CDD_out compact=bit
#pragma HLS aggregate variable=c2h_data_f_D_t_CDD_out compact=bit
	static struct_ctrl_msg reg_recv_ctrl_msg;
	static ap_uint<8> reg_recv_stage = RECV_STAGE_MAC;
#pragma HLS reset variable=reg_recv_ctrl_msg
#pragma HLS reset variable=reg_recv_stage
#pragma HLS ARRAY_PARTITION variable=reg_recv_ctrl_msg.dir_gblock_bitmap dim=0 complete
	struct_ctrl_msg wire_recv_ctrl_msg = reg_recv_ctrl_msg;
	ap_uint<8> wire_recv_stage = reg_recv_stage;
	switch(wire_recv_stage){
		case RECV_STAGE_MAC:{
			if(!net_rx.empty()){
				struct_net_rxtx tmp_net_rx = net_rx.read();
				reg_recv_stage = RECV_STAGE_CTRL;
			}
			break;
		}
		case RECV_STAGE_CTRL:{
			if(!net_rx.empty()){
				struct_net_rxtx tmp_net_rx = net_rx.read();
				struct_ctrl_msg		tmp_ctrl_msg;
				net_rx_to_ctrl_msg(tmp_net_rx, tmp_ctrl_msg);
				reg_recv_ctrl_msg = tmp_ctrl_msg;
				if(tmp_ctrl_msg.mtype >= RawMessageType::R_READ_MISS && tmp_ctrl_msg.mtype <= RawMessageType::R_EVICT_DIRTY){
					ctrl_msg_f_D_t_DC_out << tmp_ctrl_msg;
				}
				else if(tmp_ctrl_msg.mtype >= RawMessageType::AGENT_2_DIR_READ_MISS_DIRTY_ACKDATA && tmp_ctrl_msg.mtype <= RawMessageType::AGENT_2_DIR_M_CHECK_FAIL){
					ctrl_msg_f_D_t_DC_out << tmp_ctrl_msg;
				}
				else{
					ctrl_msg_f_D_t_CC_out << tmp_ctrl_msg;
				}
				if(tmp_ctrl_msg.data_msg_length == 0){
					reg_recv_stage = RECV_STAGE_MAC;
				}
				else{
					reg_recv_stage = RECV_STAGE_DATA;
					struct_c2h_cmd tmp_c2h_cmd;
					if(tmp_ctrl_msg.mtype == RawMessageType::AGENT_2_DIR_READ_MISS_DIRTY_ACKDATA
					|| tmp_ctrl_msg.mtype == RawMessageType::R_EVICT_DIRTY){
						tmp_c2h_cmd.addr = tmp_ctrl_msg.dir_gblock_addr;
						tmp_c2h_cmd.len = DSM_CACHE_LINE_SIZE;
						tmp_c2h_cmd.qid = PSEUDO_QID_D;
					}
					else{
						tmp_c2h_cmd.addr = tmp_ctrl_msg.req_cline_addr;
						tmp_c2h_cmd.len = DSM_CACHE_LINE_SIZE;
						tmp_c2h_cmd.qid = PSEUDO_QID_D;
					}
					c2h_cmd_f_D_t_CDD_out << tmp_c2h_cmd;
				}
			}
			break;
		}
		case RECV_STAGE_DATA:{
			if(!net_rx.empty()){
				struct_net_rxtx tmp_net_rx = net_rx.read();
				struct_c2h_data tmp_c2h_data;
				net_rx_to_c2h_data(tmp_net_rx, tmp_c2h_data);
				c2h_data_f_D_t_CDD_out << tmp_c2h_data;
				if(tmp_net_rx.last == 1){
					reg_recv_stage = RECV_STAGE_MAC;
				}
				else{
					reg_recv_stage = RECV_STAGE_DATA;
				}
			}
			break;
		}
	}
}