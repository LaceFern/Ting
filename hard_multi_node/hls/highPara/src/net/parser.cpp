#include "../utils.hpp"
void parser(
		hls::stream<struct_net_rxtx >& 		net_tx,
		hls::stream<struct_net_init_msg >& 	net_init_msg,
        hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_DMF_t_P_in,
        hls::stream<struct_data_msg >& 	data_msg_f_DMF_t_P_in
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=net_tx depth=16
#pragma HLS INTERFACE axis register both port=net_init_msg depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_DMF_t_P_in depth=16
#pragma HLS INTERFACE axis register both port=data_msg_f_DMF_t_P_in depth=16
#pragma HLS aggregate variable=net_tx compact=bit
#pragma HLS aggregate variable=net_init_msg compact=bit
#pragma HLS aggregate variable=ctrl_msg_f_DMF_t_P_in compact=bit
#pragma HLS aggregate variable=data_msg_f_DMF_t_P_in compact=bit
	static ap_uint<48> 	local_mac;
	static ap_uint<48> 	mac_table[max_node_num];
#pragma HLS reset variable=mac_table
#pragma HLS ARRAY_PARTITION variable=mac_table dim=0 complete
	if(!net_init_msg.empty()){
		struct_net_init_msg tmp_msg = net_init_msg.read();
		mac_table[tmp_msg.node_id] = tmp_msg.node_mac;
	}
	static struct_ctrl_msg reg_send_ctrl_msg;
	static ap_uint<8> reg_send_stage = SEND_STAGE_MAC;
#pragma HLS reset variable=reg_send_ctrl_msg
#pragma HLS reset variable=reg_send_stage
#pragma HLS ARRAY_PARTITION variable=reg_send_ctrl_msg.dir_gblock_bitmap dim=0 complete
	struct_ctrl_msg wire_send_ctrl_msg = reg_send_ctrl_msg;
	ap_uint<8> wire_send_stage = reg_send_stage;
	switch(wire_send_stage){
		case SEND_STAGE_MAC:{
			if(!ctrl_msg_f_DMF_t_P_in.empty()){
				struct_ctrl_msg tmp_ctrl_msg = ctrl_msg_f_DMF_t_P_in.read();
				struct_net_rxtx 		tmp_net_tx;
				set_mac(tmp_net_tx, local_mac, mac_table[tmp_ctrl_msg.dst_node_id]);
				reg_send_ctrl_msg = tmp_ctrl_msg;
				reg_send_stage = SEND_STAGE_CTRL;
				net_tx 			<< tmp_net_tx;
			}
			break;
		}
		case SEND_STAGE_CTRL:{
			struct_net_rxtx 		tmp_net_tx;
			ctrl_msg_to_net_tx(wire_send_ctrl_msg, tmp_net_tx);
			net_tx 			<< tmp_net_tx;
			if(wire_send_ctrl_msg.data_msg_length == 0){
				reg_send_stage = SEND_STAGE_MAC;
			}
			else{
				reg_send_stage = SEND_STAGE_DATA;
			}
			break;
		}
		case SEND_STAGE_DATA:{
			if(!data_msg_f_DMF_t_P_in.empty()){
				struct_data_msg tmp_data_msg = data_msg_f_DMF_t_P_in.read();
				struct_net_rxtx 	tmp_net_tx;
				data_msg_to_net_tx(tmp_data_msg, tmp_net_tx);
				net_tx 			<< tmp_net_tx;
				if(tmp_data_msg.last == 1){
					reg_send_stage = SEND_STAGE_MAC;
				}
			}
			break;
		}
	}
}