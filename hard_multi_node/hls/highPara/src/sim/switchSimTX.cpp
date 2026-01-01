#include "../utils.hpp"
void switchSimTX(
		hls::stream<struct_net_rxtx >& 		net_tx,
		hls::stream<struct_net_rxtx >& 		net_tx_0,
        hls::stream<struct_net_rxtx >& 		net_tx_1,
        hls::stream<struct_net_rxtx >& 		net_tx_2,
        ap_uint<1>		                    pass_flag,
        ap_uint<32>		                    pass_count
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=net_tx depth=16
#pragma HLS INTERFACE axis register both port=net_tx_0 depth=16
#pragma HLS INTERFACE axis register both port=net_tx_1 depth=16
#pragma HLS INTERFACE axis register both port=net_tx_2 depth=16
#pragma HLS aggregate variable=net_tx compact=bit
#pragma HLS aggregate variable=net_tx_0 compact=bit
#pragma HLS aggregate variable=net_tx_1 compact=bit
#pragma HLS aggregate variable=net_tx_2 compact=bit
#pragma HLS INTERFACE ap_none port=pass_flag
#pragma HLS INTERFACE ap_none port=pass_count
    static ap_uint<32> reg_pass_flag;
    static ap_uint<32> reg_pass_count;
    ap_uint<32> wire_pass_flag = reg_pass_flag;
    ap_uint<32> wire_pass_count = reg_pass_count;
    if(wire_pass_flag == 0 && pass_flag == 1){
        reg_pass_flag = 1;
        reg_pass_count = 0;
    }
    else if(wire_pass_flag == 1 && pass_flag == 0){
        reg_pass_flag = 0;
    }    
	static ap_uint<8> reg_recv_node_id;
	static ap_uint<8> reg_recv_stage = RECV_STAGE_MAC;
#pragma HLS reset variable=reg_recv_node_id
#pragma HLS reset variable=reg_recv_stage
	ap_uint<8> wire_recv_node_id = reg_recv_node_id;
	ap_uint<8> wire_recv_stage = reg_recv_stage;
	switch(wire_recv_stage){
		case RECV_STAGE_MAC:{
			if(wire_pass_flag == 1 && wire_pass_count < pass_count && !net_tx.empty()){
				struct_net_rxtx tmp_net_tx = net_tx.read();
                ap_uint<48> dst_mac = get_dst_mac(tmp_net_tx);
                reg_recv_node_id = dst_mac;
				reg_recv_stage = RECV_STAGE_CTRL;
                reg_pass_count = wire_pass_count + 1;
                switch(dst_mac){
                    case 0:{net_tx_0 << tmp_net_tx; break;}
                    case 1:{net_tx_1 << tmp_net_tx; break;}
                    case 2:{net_tx_2 << tmp_net_tx; break;}
                }
			}
			break;
		}
		case RECV_STAGE_CTRL:{
			if(!net_tx.empty()){
				struct_net_rxtx tmp_net_tx = net_tx.read();
                switch(wire_recv_node_id){
                    case 0:{net_tx_0 << tmp_net_tx; break;}
                    case 1:{net_tx_1 << tmp_net_tx; break;}
                    case 2:{net_tx_2 << tmp_net_tx; break;}
                }
				if(tmp_net_tx.last == 1){
					reg_recv_stage = RECV_STAGE_MAC;
				}
			}
			break;
		}
	}
}