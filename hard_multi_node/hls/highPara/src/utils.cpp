#include "./utils.hpp"
void set_mac(struct_net_rxtx &tmp_net_tx, ap_uint<48> &src_mac, ap_uint<48> &dst_mac){
	ap_uint<512> 		tmp_l2payload;
	tmp_l2payload(511, 464) = dst_mac; 
	tmp_l2payload(463, 416) = src_mac; 
	for(int i = 0; i < 64; i++){
		#pragma HLS unroll
		tmp_net_tx.data((i + 1) * 8 - 1, i * 8) = tmp_l2payload(511 - i * 8, 511 - ((i + 1) * 8 - 1));
	}
	tmp_net_tx.keep 	= 0xffffffffffffffff;
	tmp_net_tx.last 	= 0x0;
}
ap_uint<48> get_dst_mac(struct_net_rxtx &tmp_net_rx){
	ap_uint<512> 		tmp_l2payload;
	for(int i = 0; i < 64; i++){
		#pragma HLS unroll
		tmp_l2payload(511 - i * 8, 511 - ((i + 1) * 8 - 1)) = tmp_net_rx.data((i + 1) * 8 - 1, i * 8);
	}
	ap_uint<48> dst_mac = tmp_l2payload(511, 464); 
    return dst_mac;
}
void ctrl_msg_to_net_tx(struct_ctrl_msg &tmp_ctrl_msg, struct_net_rxtx &tmp_net_tx){
	ap_uint<512> 		tmp_l2payload;
	tmp_l2payload(7, 0) = tmp_ctrl_msg.mtype;
	tmp_l2payload(15, 8)    = tmp_ctrl_msg.req_cline_nodeID;
	tmp_l2payload(23, 16)   = tmp_ctrl_msg.req_cline_appThreadID; 
	tmp_l2payload(87, 24)   = tmp_ctrl_msg.req_cline_addr; 
	tmp_l2payload(119, 88)  = tmp_ctrl_msg.req_cline_index; 
	tmp_l2payload(183, 120) = tmp_ctrl_msg.cline_tag; 
	tmp_l2payload(191, 184) = tmp_ctrl_msg.agent_cline_nodeID;
	tmp_l2payload(255, 192) = tmp_ctrl_msg.agent_cline_addr;
	tmp_l2payload(319, 256) = tmp_ctrl_msg.dir_gblock_addr; 
	tmp_l2payload(351, 320) = tmp_ctrl_msg.dir_gblock_index; 
    for(ap_uint<8> i = 0; i < max_node_num; i++){
        tmp_l2payload(351 + 8 * (i+1), 352 + 8 * i) = tmp_ctrl_msg.dir_gblock_bitmap[i];
    }
	tmp_l2payload(423, 416) = tmp_ctrl_msg.dir_gblock_nodeID;
    tmp_l2payload(439, 424) = tmp_ctrl_msg.data_msg_length;
    for(int i = 0; i < 64; i++){
    	#pragma HLS unroll
    	tmp_net_tx.data((i + 1) * 8 - 1, i * 8) = tmp_l2payload(511 - i * 8, 511 - ((i + 1) * 8 - 1));
    }
    tmp_net_tx.keep 	= 0xffffffffffffffff;
    if(tmp_ctrl_msg.data_msg_length == 0){
        tmp_net_tx.last 	= 1;
    }
    else{
        tmp_net_tx.last 	= 0;
    }
}
void data_msg_to_net_tx(struct_data_msg &tmp_data_msg, struct_net_rxtx &tmp_net_tx){
    for(int i = 0; i < 64; i++){
    	#pragma HLS unroll
    	tmp_net_tx.data((i + 1) * 8 - 1, i * 8) = tmp_data_msg.data(511 - i * 8, 511 - ((i + 1) * 8 - 1));
    }
    tmp_net_tx.keep 	= 0xffffffffffffffff;
    tmp_net_tx.last 	= tmp_data_msg.last;
}
void net_rx_to_c2h_data(struct_net_rxtx &tmp_net_rx, struct_c2h_data &tmp_c2h_data){
	ap_uint<512> 		tmp_l2payload;
	for(int i = 0; i < 64; i++){
		#pragma HLS unroll
		tmp_l2payload(511 - i * 8, 511 - ((i + 1) * 8 - 1)) = tmp_net_rx.data((i + 1) * 8 - 1, i * 8);
	}
	tmp_c2h_data.data = tmp_l2payload;
    tmp_c2h_data.ctrl_qid = PSEUDO_QID_D;
	tmp_c2h_data.last = tmp_net_rx.last;
}
void net_rx_to_ctrl_msg(struct_net_rxtx &tmp_net_rx, struct_ctrl_msg &tmp_ctrl_msg){
	ap_uint<512> 		tmp_l2payload;
	for(int i = 0; i < 64; i++){
		#pragma HLS unroll
		tmp_l2payload(511 - i * 8, 511 - ((i + 1) * 8 - 1)) = tmp_net_rx.data((i + 1) * 8 - 1, i * 8);
	}
    tmp_ctrl_msg.mtype = tmp_l2payload(7, 0);
    tmp_ctrl_msg.req_cline_nodeID = tmp_l2payload(15, 8);
    tmp_ctrl_msg.req_cline_appThreadID = tmp_l2payload(23, 16); 
    tmp_ctrl_msg.req_cline_addr = tmp_l2payload(87, 24); 
    tmp_ctrl_msg.req_cline_index = tmp_l2payload(119, 88); 
    tmp_ctrl_msg.cline_tag = tmp_l2payload(183, 120); 
    tmp_ctrl_msg.agent_cline_nodeID = tmp_l2payload(191, 184);
    tmp_ctrl_msg.agent_cline_addr = tmp_l2payload(255, 192);
    tmp_ctrl_msg.dir_gblock_addr = tmp_l2payload(319, 256); 
    tmp_ctrl_msg.dir_gblock_index = tmp_l2payload(351, 320); 
    for (ap_uint<8> i = 0; i < max_node_num; i++) {
        tmp_ctrl_msg.dir_gblock_bitmap[i] = tmp_l2payload(351 + 8 * (i + 1), 352 + 8 * i);
    }
    tmp_ctrl_msg.dir_gblock_nodeID = tmp_l2payload(423, 416);
    tmp_ctrl_msg.data_msg_length = tmp_l2payload(439, 424);
}
void h2c_data_to_data_msg(struct_h2c_data &tmp_h2c_data, struct_data_msg &tmp_data_msg){
    tmp_data_msg.data = tmp_h2c_data.data;
	tmp_data_msg.keep = 0xffffffffffffffff;
	tmp_data_msg.last = tmp_h2c_data.last;
}
void ctrl_msg_to_c2h_data(struct_ctrl_msg &tmp_ctrl_msg, struct_c2h_data &tmp_c2h_data, int tmp_qid){
    tmp_ctrl_msg.general_c2h_owner = 1;
	tmp_c2h_data.data(7, 0) = tmp_ctrl_msg.mtype;
	tmp_c2h_data.data(15, 8)    = tmp_ctrl_msg.req_cline_nodeID;
	tmp_c2h_data.data(23, 16)   = tmp_ctrl_msg.req_cline_appThreadID; 
	tmp_c2h_data.data(87, 24)   = tmp_ctrl_msg.req_cline_addr; 
	tmp_c2h_data.data(119, 88)  = tmp_ctrl_msg.req_cline_index; 
	tmp_c2h_data.data(183, 120) = tmp_ctrl_msg.cline_tag; 
	tmp_c2h_data.data(191, 184) = tmp_ctrl_msg.agent_cline_nodeID;
	tmp_c2h_data.data(255, 192) = tmp_ctrl_msg.agent_cline_addr;
	tmp_c2h_data.data(319, 256) = tmp_ctrl_msg.dir_gblock_addr; 
	tmp_c2h_data.data(351, 320) = tmp_ctrl_msg.dir_gblock_index; 
    for(ap_uint<8> i = 0; i < max_node_num; i++){
        tmp_c2h_data.data(351 + 8 * (i+1), 352 + 8 * i) = tmp_ctrl_msg.dir_gblock_bitmap[i];
    }
	tmp_c2h_data.data(423, 416) = tmp_ctrl_msg.dir_gblock_nodeID;
  	tmp_c2h_data.data(431, 424) = tmp_ctrl_msg.general_c2h_owner;
    tmp_c2h_data.last = 1;
    tmp_c2h_data.ctrl_qid = tmp_qid;
}
void h2c_w_to_ctrl_msg(struct_h2c_w &tmp_h2c_w, struct_ctrl_msg &tmp_ctrl_msg){
	tmp_ctrl_msg.mtype = tmp_h2c_w.wdata(7, 0);
	tmp_ctrl_msg.req_cline_nodeID       = tmp_h2c_w.wdata(15, 8);
	tmp_ctrl_msg.req_cline_appThreadID  = tmp_h2c_w.wdata(23, 16); 
	tmp_ctrl_msg.req_cline_addr         = tmp_h2c_w.wdata(87, 24); 
	tmp_ctrl_msg.req_cline_index        = tmp_h2c_w.wdata(119, 88); 
	tmp_ctrl_msg.cline_tag              = tmp_h2c_w.wdata(183, 120); 
	tmp_ctrl_msg.agent_cline_nodeID = tmp_h2c_w.wdata(191, 184);
	tmp_ctrl_msg.agent_cline_addr   = tmp_h2c_w.wdata(255, 192);
	tmp_ctrl_msg.dir_gblock_addr  = tmp_h2c_w.wdata(319, 256); 
	tmp_ctrl_msg.dir_gblock_index = tmp_h2c_w.wdata(351, 320); 
    for(ap_uint<8> i = 0; i < max_node_num; i++){
        tmp_ctrl_msg.dir_gblock_bitmap[i] = tmp_h2c_w.wdata(351 + 8 * (i+1), 352 + 8 * i);
    }
	tmp_ctrl_msg.dir_gblock_nodeID = tmp_h2c_w.wdata(423, 416);
	tmp_ctrl_msg.init_state = tmp_h2c_w.wdata(439, 432);
	tmp_ctrl_msg.init_mac   = tmp_h2c_w.wdata(487, 440);
}
void h2c_w_to_net_init_msg(struct_h2c_w &tmp_h2c_w, struct_net_init_msg &tmp_net_init_msg){
	tmp_net_init_msg.node_id 	= tmp_h2c_w.wdata(423, 416); 
	tmp_net_init_msg.node_mac 	= tmp_h2c_w.wdata(487, 440); 
}
void h2c_w_to_dir_data(struct_h2c_w &tmp_h2c_w, struct_dir_data &tmp_dir_data){
	tmp_dir_data.dir_gblock_addr    = tmp_h2c_w.wdata(319, 256);
	tmp_dir_data.dir_gblock_index   = tmp_h2c_w.wdata(351, 320);
    for(ap_uint<8> i = 0; i < max_node_num; i++){
        tmp_dir_data.dir_gblock_bitmap[i] = tmp_h2c_w.wdata(351 + 8 * (i+1), 352 + 8 * i);
    }
    tmp_dir_data.state          = tmp_h2c_w.wdata(439, 432);
	tmp_dir_data.replica_count      = 0;
}
void ctrl_msg_to_h2c_w(struct_ctrl_msg &tmp_ctrl_msg, struct_h2c_w &tmp_h2c_w){
	tmp_h2c_w.wdata(7, 0) = tmp_ctrl_msg.mtype;
	tmp_h2c_w.wdata(15, 8)    = tmp_ctrl_msg.req_cline_nodeID;
	tmp_h2c_w.wdata(23, 16)   = tmp_ctrl_msg.req_cline_appThreadID; 
	tmp_h2c_w.wdata(87, 24)   = tmp_ctrl_msg.req_cline_addr; 
	tmp_h2c_w.wdata(119, 88)  = tmp_ctrl_msg.req_cline_index; 
	tmp_h2c_w.wdata(183, 120) = tmp_ctrl_msg.cline_tag; 
	tmp_h2c_w.wdata(191, 184) = tmp_ctrl_msg.agent_cline_nodeID;
	tmp_h2c_w.wdata(255, 192) = tmp_ctrl_msg.agent_cline_addr;
	tmp_h2c_w.wdata(319, 256) = tmp_ctrl_msg.dir_gblock_addr; 
	tmp_h2c_w.wdata(351, 320) = tmp_ctrl_msg.dir_gblock_index; 
    for(ap_uint<8> i = 0; i < max_node_num; i++){
        tmp_h2c_w.wdata(351 + 8 * (i+1), 352 + 8 * i) = tmp_ctrl_msg.dir_gblock_bitmap[i];
    }
	tmp_h2c_w.wdata(423, 416) = tmp_ctrl_msg.dir_gblock_nodeID;
	tmp_h2c_w.wdata(439, 432) = tmp_ctrl_msg.init_state;
	tmp_h2c_w.wdata(487, 440) = tmp_ctrl_msg.init_mac;
	tmp_h2c_w.wstrb = 0xffffffffffffffff;
	tmp_h2c_w.wlast = 1;
}
void dir_data_e_to_ddr_w_part1(struct_dir_data_e &tmp_dir_data_e, struct_ddr_w &tmp_ddr_w){
	tmp_ddr_w.wdata(63, 0) = tmp_dir_data_e.dir_gblock_addr;
	tmp_ddr_w.wdata(95, 64) = tmp_dir_data_e.dir_gblock_index;
    for(ap_uint<8> i = 0; i < max_node_num; i++){
        tmp_ddr_w.wdata(95 + 8 * (i+1), 96 + 8 * i) = tmp_dir_data_e.dir_gblock_bitmap[i];
    }
    tmp_ddr_w.wdata(167, 160) = tmp_dir_data_e.state;
	tmp_ddr_w.wdata(175, 168) = tmp_dir_data_e.replica_count;
    tmp_ddr_w.wstrb = 0xffffffffffffffff;
    tmp_ddr_w.wlast = 0;
}
void dir_data_e_to_ddr_w_part2(struct_dir_data_e &tmp_dir_data_e, struct_ddr_w &tmp_ddr_w){
    for(ap_uint<8> i = 0; i < max_node_num; i++){
        tmp_ddr_w.wdata(64 * (i+1), 64 * i) = tmp_dir_data_e.agent_cline_addr[i];
    }
    tmp_ddr_w.wstrb = 0xffffffffffffffff;
    tmp_ddr_w.wlast = 1;
}
void ddr_r_to_dir_data_e_part1(struct_ddr_r &tmp_ddr_r, struct_dir_data_e &tmp_dir_data_e){
	tmp_dir_data_e.dir_gblock_addr = tmp_ddr_r.rdata(63, 0);
	tmp_dir_data_e.dir_gblock_index = tmp_ddr_r.rdata(95, 64);
    for(ap_uint<8> i = 0; i < max_node_num; i++){
        tmp_dir_data_e.dir_gblock_bitmap[i] = tmp_ddr_r.rdata(95 + 8 * (i+1), 96 + 8 * i);
    }
    tmp_dir_data_e.state = tmp_ddr_r.rdata(167, 160);
	tmp_dir_data_e.replica_count = tmp_ddr_r.rdata(175, 168);
}
void ddr_r_to_dir_data_e_part2(struct_ddr_r &tmp_ddr_r, struct_dir_data_e &tmp_dir_data_e){
    for(ap_uint<8> i = 0; i < max_node_num; i++){
        tmp_dir_data_e.agent_cline_addr[i] = tmp_ddr_r.rdata(64 * (i+1), 64 * i);
    }
}
void dir_data_to_dir_data_e(struct_dir_data &tmp_dir_data, struct_dir_data_e &tmp_dir_data_e){
	tmp_dir_data_e.dir_gblock_addr = tmp_dir_data.dir_gblock_addr;
	tmp_dir_data_e.dir_gblock_index = tmp_dir_data.dir_gblock_index;
    for(ap_uint<8> i = 0; i < max_node_num; i++){
        tmp_dir_data_e.dir_gblock_bitmap[i] = tmp_dir_data.dir_gblock_bitmap[i];
    }
    tmp_dir_data_e.state = tmp_dir_data.state;
	tmp_dir_data_e.replica_count = tmp_dir_data.replica_count;
}
ap_uint<8> extract_mtype_from_h2c_w(struct_h2c_w &tmp_h2c_w){
    return tmp_h2c_w.wdata(7, 0);
}
unsigned int gen_21bit_pseudo_random_num(){
        static unsigned int lfsr = 1;
        unsigned lsb = lfsr & 1;
        lfsr >>= 1;
        if (lsb == 1){
                lfsr ^= (1<<20)|(1<<18);
        }
        return lfsr;
}