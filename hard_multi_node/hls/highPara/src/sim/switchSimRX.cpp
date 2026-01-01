#include "../utils.hpp"
void switchSimRX(
		hls::stream<struct_net_rxtx >& 		net_rx,
		hls::stream<struct_net_rxtx >& 		net_rx_0,
        hls::stream<struct_net_rxtx >& 		net_rx_1,
        hls::stream<struct_net_rxtx >& 		net_rx_2
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=net_rx depth=16
#pragma HLS INTERFACE axis register both port=net_rx_0 depth=16
#pragma HLS INTERFACE axis register both port=net_rx_1 depth=16
#pragma HLS INTERFACE axis register both port=net_rx_2 depth=16
#pragma HLS aggregate variable=net_rx compact=bit
#pragma HLS aggregate variable=net_rx_0 compact=bit
#pragma HLS aggregate variable=net_rx_1 compact=bit
#pragma HLS aggregate variable=net_rx_2 compact=bit
	static ap_uint<8> reg_recv_node_id;
	static ap_uint<8> reg_recv_stage = RECV_STAGE_MAC;
#pragma HLS reset variable=reg_recv_node_id
#pragma HLS reset variable=reg_recv_stage
	ap_uint<8> wire_recv_node_id = reg_recv_node_id;
	ap_uint<8> wire_recv_stage = reg_recv_stage;
	switch(wire_recv_stage){
		case RECV_STAGE_MAC:{
			if(!net_rx_0.empty() && net_rx_1.empty() && net_rx_2.empty()){
				struct_net_rxtx tmp_net_rx = net_rx_0.read();
                reg_recv_node_id = 0;
				reg_recv_stage = RECV_STAGE_CTRL;
                net_rx << tmp_net_rx;
			}
            else if(net_rx_0.empty() && !net_rx_1.empty() && net_rx_2.empty()){
				struct_net_rxtx tmp_net_rx = net_rx_1.read();
                reg_recv_node_id = 1;
				reg_recv_stage = RECV_STAGE_CTRL;
                net_rx << tmp_net_rx;
            }
            else if(net_rx_0.empty() && net_rx_1.empty() && !net_rx_2.empty()){
				struct_net_rxtx tmp_net_rx = net_rx_2.read();
                reg_recv_node_id = 2;
				reg_recv_stage = RECV_STAGE_CTRL;
                net_rx << tmp_net_rx;
            }
            else{
                switch(wire_recv_node_id){
                    case 0:{
                        if(!net_rx_0.empty()){
                            struct_net_rxtx tmp_net_rx = net_rx_0.read();
                            reg_recv_node_id = 0;
                            reg_recv_stage = RECV_STAGE_CTRL;
                            net_rx << tmp_net_rx;
                        }
                        else{
                            reg_recv_node_id = 1;
                        }
                        break;  
                    }
                    case 1:{
                        if(!net_rx_1.empty()){
                            struct_net_rxtx tmp_net_rx = net_rx_1.read();
                            reg_recv_node_id = 1;
                            reg_recv_stage = RECV_STAGE_CTRL;
                            net_rx << tmp_net_rx;
                        }
                        else{
                            reg_recv_node_id = 2;
                        }
                        break;  
                    }
                    case 2:{
                        if(!net_rx_2.empty()){
                            struct_net_rxtx tmp_net_rx = net_rx_2.read();
                            reg_recv_node_id = 2;
                            reg_recv_stage = RECV_STAGE_CTRL;
                            net_rx << tmp_net_rx;
                        }
                        else{
                            reg_recv_node_id = 0;
                        }
                        break;  
                    }
                }
            }
			break;
		}
		case RECV_STAGE_CTRL:{
            switch(wire_recv_node_id){
                case 0:{
                    if(!net_rx_0.empty()){
                        struct_net_rxtx tmp_net_rx = net_rx_0.read();
                        net_rx << tmp_net_rx;
                        if(tmp_net_rx.last == 1){
				        	reg_recv_stage = RECV_STAGE_MAC;
                            reg_recv_node_id = 1;
				        }
                    }
                    break;
                }
                case 1:{
                    if(!net_rx_1.empty()){
                        struct_net_rxtx tmp_net_rx = net_rx_1.read();
                        net_rx << tmp_net_rx;
                        if(tmp_net_rx.last == 1){
				        	reg_recv_stage = RECV_STAGE_MAC;
                            reg_recv_node_id = 2;
				        }
                    }
                    break;
                }
                case 2:{
                    if(!net_rx_2.empty()){
                        struct_net_rxtx tmp_net_rx = net_rx_2.read();
                        net_rx << tmp_net_rx;
                        if(tmp_net_rx.last == 1){
				        	reg_recv_stage = RECV_STAGE_MAC;
                            reg_recv_node_id = 0;
				        }
                    }
                    break;
                }
            }
			break;
		}
	}
}