#include <hls_stream.h>
#include "ap_int.h"
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "io.hpp"
#define TASK_R 0
#define TASK_W 1
#define LEN_WIN 1024
#define WIDTH_WINIDX 10
#define NUM_PKTIDX 4294967296
#define WIDTH_PKTIDX 32
#define NUM_STALLCYCLE 400
#define WINSTATUS_NOTFULL 0
#define WINSTATUS_FULL    1
typedef ap_uint<64> var_stallCycle;
typedef ap_uint<WIDTH_WINIDX> var_winIdx;
typedef ap_uint<WIDTH_PKTIDX> var_pktIdx;
typedef ap_uint<512> var_pktSeg;
typedef ap_uint<2> var_pktStatus;
typedef ap_uint<2> var_winStatus;
typedef struct cacheInfo{
	var_pktIdx	pktIdx;
	var_winIdx  winIdx;
} struct_cacheInfo;
typedef struct sendInfo{
	var_pktIdx	pktIdx;
	ap_uint<1>  ifStall;
} struct_sendInfo;
#define MAX_NODE_NUM 8
#define MAX_MSG_IN_Q_NUM 64
#define MAX_Q_NUM 32
#define DATA_BLOCK_LENGTH 512
#define OP_WRITE 1
#define OP_INVALIDATION_FORWARD 2
#define STATUS_SUCC 1
#define C2H_BUFFER 0
#define C2H_NOTIFY 1
#define TYPE_DC 0
#define TYPE_CC 1
#define RULE_NOT_PARENT 0
#define RULE_PARENT 1
#define PSEUDO_QID_BMI 0
#define PSEUDO_QID_H2CBM  1
#define PSEUDO_QID_DMF 2
const int max_node_num = 8;
const int max_appt_num_per_node = 24;
const int max_syst_num_per_node = 8; 
const int max_cline_num_per_node = 5000000;
const int max_gblock_num_per_node = 5000000;
const int max_msg_num_per_q = 64;
const int max_msg_length = 64; 
#define PSEUDO_QID_D 2
#define PSEUDO_QID_BMSYS 1
#define PSEUDO_QID_BMAPP 0
#define THREAD_TYPE_APP 0
#define THREAD_TYPE_SYS 1
#define LOCK_BKT_SIZE 1
#define LOCK_BKT_NUM 256
#define LOCK_BKT_WIDTH 8 
#define SEND_STAGE_MAC 0
#define SEND_STAGE_CTRL 1
#define SEND_STAGE_DATA 2
#define RECV_STAGE_MAC 0
#define RECV_STAGE_CTRL 1
#define RECV_STAGE_DATA 2
ap_uint<8> extract_mtype_from_h2c_w(struct_h2c_w &tmp_h2c_w);
typedef struct dir_ack{
	ap_uint<8> ack;
}struct_dir_ack;
typedef struct dir_cmd{
	ap_uint<32> dir_gblock_index;
}struct_dir_cmd;
typedef struct dir_data{
	ap_uint<64> dir_gblock_addr;
	ap_uint<32> dir_gblock_index;
	ap_uint<8> 	dir_gblock_bitmap[max_node_num];
	ap_uint<8> 	state;
	ap_uint<8> 	replica_count;
}struct_dir_data;
typedef struct dir_data_extend{
	ap_uint<64> dir_gblock_addr;
	ap_uint<32> dir_gblock_index;
	ap_uint<8> 	dir_gblock_bitmap[max_node_num];
	ap_uint<8> 	state;
	ap_uint<8> 	replica_count;
	ap_uint<64> agent_cline_addr[max_node_num];
}struct_dir_data_e;
void h2c_w_to_dir_data(struct_h2c_w &tmp_h2c_w, dir_data &tmp_dir_data);
void dir_data_e_to_ddr_w_part1(struct_dir_data_e &tmp_dir_data_e, struct_ddr_w &tmp_ddr_w);
void dir_data_e_to_ddr_w_part2(struct_dir_data_e &tmp_dir_data_e, struct_ddr_w &tmp_ddr_w);
void ddr_r_to_dir_data_e_part1(struct_ddr_r &tmp_ddr_r, struct_dir_data_e &tmp_dir_data_e);
void ddr_r_to_dir_data_e_part2(struct_ddr_r &tmp_ddr_r, struct_dir_data_e &tmp_dir_data_e);
void dir_data_to_dir_data_e(struct_dir_data &tmp_dir_data, struct_dir_data_e &tmp_dir_data_e);
typedef struct net_init_msg{
	ap_uint<8> 	node_id;
	ap_uint<48> node_mac;
}struct_net_init_msg;
void h2c_w_to_net_init_msg(struct_h2c_w &tmp_h2c_w, struct_net_init_msg &tmp_net_init_msg);
typedef struct data_msg{
	ap_uint<512>	data;
	ap_uint<64>		keep;
	ap_uint<1>		last;
} struct_data_msg;
typedef struct ctrl_msg{
	ap_uint<8> 	mtype;
	ap_uint<8> 	req_cline_nodeID;
	ap_uint<8> 	req_cline_appThreadID; 
	ap_uint<64> req_cline_addr; 
	ap_uint<32> req_cline_index; 
	ap_uint<64> cline_tag; 
	ap_uint<8> 	agent_cline_nodeID;
	ap_uint<64> agent_cline_addr;
	ap_uint<64> dir_gblock_addr; 
	ap_uint<32> dir_gblock_index; 
	ap_uint<8> 	dir_gblock_bitmap[max_node_num];
	ap_uint<8>  dir_gblock_nodeID;
  	ap_uint<8> general_c2h_owner;
  	ap_uint<8> init_state; 
  	ap_uint<48> init_mac; 
	ap_uint<16> data_msg_length = 0; 
	ap_uint<8> dst_thread_type;
	ap_uint<8> dst_node_id; 
	ap_uint<8> lock_position;
}struct_ctrl_msg;
void h2c_w_to_ctrl_msg(struct_h2c_w &tmp_h2c_w, struct_ctrl_msg &tmp_ctrl_msg);
void ctrl_msg_to_c2h_data(struct_ctrl_msg &tmp_ctrl_msg, struct_c2h_data &tmp_c2h_data, int tmp_qid);
void h2c_data_to_data_msg(struct_h2c_data &tmp_msg, struct_data_msg &tmp_data_msg);
template <int T>
struct q_id_arr {
    ap_uint<32> id[T];
};
template <int T>
using struct_q_id_arr = q_id_arr<T>;
template <int T>
struct q_addr_arr{
	ap_uint<64>		addr[T];
};
template <int T>
using struct_q_addr_arr = q_addr_arr<T>;
template <int T>
struct q_hidx_arr{
	ap_uint<32>		idx[T];
}; 
template <int T>
using struct_q_hidx_arr = q_hidx_arr<T>;
template <int T>
struct q_tidx_arr{
	ap_uint<32>		idx[T];
};
template <int T>
using struct_q_tidx_arr = q_tidx_arr<T>;
void set_mac(struct_net_rxtx &tmp_net_tx, ap_uint<48> &src_mac, ap_uint<48> &dst_mac);
ap_uint<48> get_dst_mac(struct_net_rxtx &tmp_net_rx);
void ctrl_msg_to_net_tx(struct_ctrl_msg &tmp_ctrl_msg, struct_net_rxtx &tmp_net_tx);
void data_msg_to_net_tx(struct_data_msg &tmp_data_msg, struct_net_rxtx &tmp_net_tx);
void net_rx_to_ctrl_msg(struct_net_rxtx &tmp_net_rx, struct_ctrl_msg &tmp_ctrl_msg);
void net_rx_to_c2h_data(struct_net_rxtx &tmp_net_rx, struct_c2h_data &tmp_c2h_data);
typedef struct header{
	ap_uint<16>	 	udp_padding;
	ap_uint<16>	 	dataLen;
	ap_uint<16>	 	srcPort;
	ap_uint<16>	 	dstPort;
	ap_uint<32>	 	src_ip;
	ap_uint<32>	 	dst_ip;
	ap_uint<96>	 	ip_padding;
	ap_uint<16>	 	eth_type;
	ap_uint<48>	 	src_mac;
	ap_uint<48>	 	dst_mac;
} struct_header;
#define TYPE_READ 0
#define TYPE_WRITE 1
#define TYPE_WRITE_SHARED 2
#define TYPE_NO 3
void ctrl_msg_to_h2c_w(struct_ctrl_msg &tmp_ctrl_msg, struct_h2c_w &tmp_h2c_w);
unsigned int gen_21bit_pseudo_random_num();