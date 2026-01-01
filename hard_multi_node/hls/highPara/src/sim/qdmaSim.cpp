#include "../utils.hpp"
#include "../dsmtype.hpp"
void qdmaSim(
    hls::stream<struct_h2c_w >& 	h2c_w_f_QS_t_CMD_out,
    hls::stream<struct_h2c_cmd >& 	h2c_cmd_f_CD_t_QS_in,
    hls::stream<struct_h2c_data >& 	h2c_data_f_QS_t_DD_out,
    hls::stream<struct_c2h_cmd >& 	c2h_cmd_f_CDD_t_QS_in,
    hls::stream<struct_c2h_data >& 	c2h_data_f_CDD_t_QS_in,
	ap_uint<32>&	ABM_c2h_q_hidx,
    ap_uint<32>&	SBM_c2h_q_hidx,
    ap_uint<1>		net_init_flag,
    ap_uint<1>		dir_init_flag,
    ap_uint<1>		warmup_flag,
    ap_uint<1>		test_flag,
    ap_uint<1>		unlock_flag,
    ap_uint<32>     iter_num,
    ap_uint<8>      node_num,
    ap_uint<8>      node_id,
    ap_uint<8>      warmup_type,
    ap_uint<8>      test_type
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=h2c_w_f_QS_t_CMD_out depth=16
#pragma HLS INTERFACE axis register both port=h2c_cmd_f_CD_t_QS_in depth=16
#pragma HLS INTERFACE axis register both port=h2c_data_f_QS_t_DD_out depth=16
#pragma HLS INTERFACE axis register both port=c2h_cmd_f_CDD_t_QS_in depth=16
#pragma HLS INTERFACE axis register both port=c2h_data_f_CDD_t_QS_in depth=16
#pragma HLS aggregate variable=h2c_w_f_QS_t_CMD_out compact=bit
#pragma HLS aggregate variable=h2c_cmd_f_CD_t_QS_in compact=bit
#pragma HLS aggregate variable=h2c_data_f_QS_t_DD_out compact=bit
#pragma HLS aggregate variable=c2h_cmd_f_CDD_t_QS_in compact=bit
#pragma HLS aggregate variable=c2h_data_f_CDD_t_QS_in compact=bit
#pragma HLS INTERFACE ap_none port=ABM_c2h_q_hidx
#pragma HLS INTERFACE ap_none port=SBM_c2h_q_hidx
#pragma HLS INTERFACE ap_none port=net_init_flag
#pragma HLS INTERFACE ap_none port=dir_init_flag
#pragma HLS INTERFACE ap_none port=warmup_flag
#pragma HLS INTERFACE ap_none port=test_flag
#pragma HLS INTERFACE ap_none port=iter_num
#pragma HLS INTERFACE ap_none port=node_num
#pragma HLS INTERFACE ap_none port=node_id
#pragma HLS INTERFACE ap_none port=warmup_type
#pragma HLS INTERFACE ap_none port=test_type
    static ap_uint<32> reg_ABM_c2h_q_hidx = 0;
    ap_uint<32> wire_ABM_c2h_q_hidx;
    wire_ABM_c2h_q_hidx = reg_ABM_c2h_q_hidx;
    ABM_c2h_q_hidx = reg_ABM_c2h_q_hidx;
    static ap_uint<32> reg_SBM_c2h_q_hidx = 0;
    ap_uint<32> wire_SBM_c2h_q_hidx;
    wire_SBM_c2h_q_hidx = reg_SBM_c2h_q_hidx;
    SBM_c2h_q_hidx = reg_SBM_c2h_q_hidx;
    static ap_uint<1> c2h_data_end_flag = 1;
    static ap_uint<32> reg_case_ready_idx = 0;
    static ap_uint<32>          reg_net_init_count = 0;
    ap_uint<32>                 wire_net_init_count;     
    wire_net_init_count     = reg_net_init_count;
    static ap_uint<32>          reg_dir_init_count = 0;
    ap_uint<32>                 wire_dir_init_count;        
    wire_dir_init_count     = reg_dir_init_count;
    static ap_uint<1>           reg_first_flag = 1;
    static ap_uint<32>          reg_count = 0;
    ap_uint<32>                 wire_count;    
    wire_count       = reg_count;
    static ap_uint<32> reg_warmup_count = 0;
    ap_uint<32> wire_warmup_count = reg_warmup_count;
    if(net_init_flag == 1 && wire_net_init_count < node_num){
        struct_ctrl_msg tmp_ctrl_msg;
        tmp_ctrl_msg.mtype = RawMessageType::INIT_NET;
        tmp_ctrl_msg.dir_gblock_nodeID = wire_net_init_count;
        tmp_ctrl_msg.init_mac = wire_net_init_count; 
        struct_h2c_w tmp_h2c_w;
        ctrl_msg_to_h2c_w(tmp_ctrl_msg, tmp_h2c_w);
        h2c_w_f_QS_t_CMD_out << tmp_h2c_w;
        reg_net_init_count = wire_net_init_count + 1;
    }
    else if(dir_init_flag == 1 && wire_dir_init_count < iter_num){
        struct_ctrl_msg tmp_ctrl_msg;
        tmp_ctrl_msg.mtype = RawMessageType::INIT_DIR;
        tmp_ctrl_msg.dir_gblock_addr = wire_dir_init_count;
        tmp_ctrl_msg.dir_gblock_index = wire_dir_init_count;
        for(int i = 0; i < max_node_num; i++){
            tmp_ctrl_msg.dir_gblock_bitmap[i] = 0;
        }
        tmp_ctrl_msg.init_state = RawState::S_UNSHARED; 
        struct_h2c_w tmp_h2c_w;
        ctrl_msg_to_h2c_w(tmp_ctrl_msg, tmp_h2c_w);
        h2c_w_f_QS_t_CMD_out << tmp_h2c_w;
        reg_dir_init_count = wire_dir_init_count + 1;
    }
    else if(warmup_flag == 0 && test_flag == 0){
        reg_count = 0;
        reg_first_flag = 0;
    }
    else if(warmup_flag == 1 && warmup_type != TYPE_NO && wire_count < iter_num){
            struct_ctrl_msg tmp_ctrl_msg;
            if(unlock_flag == 0){
                if(warmup_type == TYPE_READ) tmp_ctrl_msg.mtype = RawMessageType::R_READ_MISS;
                else if(warmup_type == TYPE_WRITE) tmp_ctrl_msg.mtype = RawMessageType::R_WRITE_MISS;
            }
            else{
                if(warmup_type == TYPE_READ) tmp_ctrl_msg.mtype = RawMessageType::R_READ_MISS_UNLOCK;
                else if(warmup_type == TYPE_WRITE) tmp_ctrl_msg.mtype = RawMessageType::R_WRITE_MISS_UNLOCK;
            }
            tmp_ctrl_msg.req_cline_nodeID = node_id;
            tmp_ctrl_msg.req_cline_appThreadID = node_id;
            tmp_ctrl_msg.req_cline_addr = 0xf1;
            tmp_ctrl_msg.req_cline_index = 0xf2;
            tmp_ctrl_msg.cline_tag = 0xf3;
            tmp_ctrl_msg.dir_gblock_index = wire_count;
            if(wire_warmup_count == 0){
                tmp_ctrl_msg.dir_gblock_nodeID = node_id;
            }
            else if(wire_warmup_count == 1){
                if(node_id == 0) tmp_ctrl_msg.dir_gblock_nodeID = 2;
                else if(node_id == 1) tmp_ctrl_msg.dir_gblock_nodeID = 0;
                else if(node_id == 2) tmp_ctrl_msg.dir_gblock_nodeID = 1;
            }
            struct_h2c_w tmp_h2c_w;
            ctrl_msg_to_h2c_w(tmp_ctrl_msg, tmp_h2c_w);
            h2c_w_f_QS_t_CMD_out << tmp_h2c_w;
            reg_count = wire_count + 1;
            if(wire_count == iter_num - 1 && unlock_flag == 1){
                reg_warmup_count = wire_warmup_count + 1;
            }
    }
    else if(test_flag == 1 && test_type != TYPE_NO && wire_count < iter_num){
            struct_ctrl_msg tmp_ctrl_msg;
            if(unlock_flag == 0){
                if(test_type == TYPE_READ) tmp_ctrl_msg.mtype = RawMessageType::R_READ_MISS;
                else if(test_type == TYPE_WRITE) tmp_ctrl_msg.mtype = RawMessageType::R_WRITE_MISS;
                else if(test_type == TYPE_WRITE_SHARED) tmp_ctrl_msg.mtype = RawMessageType::R_WRITE_SHARED;
            }
            else{
                if(test_type == TYPE_READ) tmp_ctrl_msg.mtype = RawMessageType::R_READ_MISS_UNLOCK;
                else if(test_type == TYPE_WRITE) tmp_ctrl_msg.mtype = RawMessageType::R_WRITE_MISS_UNLOCK;
                else if(test_type == TYPE_WRITE_SHARED) tmp_ctrl_msg.mtype = RawMessageType::R_WRITE_SHARED_UNLOCK;
            }
            tmp_ctrl_msg.req_cline_nodeID = node_id;
            tmp_ctrl_msg.req_cline_appThreadID = 0xf0;
            tmp_ctrl_msg.req_cline_addr = 0xf1;
            tmp_ctrl_msg.req_cline_index = 0xf2;
            tmp_ctrl_msg.cline_tag = 0xf3;
            tmp_ctrl_msg.dir_gblock_index = wire_count;
            if(test_type == TYPE_WRITE_SHARED){
                tmp_ctrl_msg.dir_gblock_nodeID = node_id;
            }
            else{
                if(node_id == node_num) tmp_ctrl_msg.dir_gblock_nodeID = 0;
                else tmp_ctrl_msg.dir_gblock_nodeID = node_id + 1;
            }
            struct_h2c_w tmp_h2c_w;
            ctrl_msg_to_h2c_w(tmp_ctrl_msg, tmp_h2c_w);
            h2c_w_f_QS_t_CMD_out << tmp_h2c_w;
            reg_count = wire_count + 1;
    }
    if(c2h_data_end_flag == 1){
        if(!c2h_cmd_f_CDD_t_QS_in.empty() && !c2h_data_f_CDD_t_QS_in.empty()){
            struct_c2h_cmd tmp_c2h_cmd = c2h_cmd_f_CDD_t_QS_in.read();
            struct_c2h_data tmp_c2h_data = c2h_data_f_CDD_t_QS_in.read();
            c2h_data_end_flag = tmp_c2h_data.last;
            if(tmp_c2h_data.data(7, 0) < 91){
                ap_uint<32> tmp_idx = 0;
                if(wire_ABM_c2h_q_hidx == max_msg_num_per_q - 1){
		        	tmp_idx = 0;
		        }
		        else{
		        	tmp_idx = wire_ABM_c2h_q_hidx + 1;
		        }
		        reg_ABM_c2h_q_hidx = tmp_idx;
            }
            else{
                ap_uint<32> tmp_idx = 0;
                if(wire_SBM_c2h_q_hidx == max_msg_num_per_q - 1){
		        	tmp_idx = 0;
		        }
		        else{
		        	tmp_idx = wire_SBM_c2h_q_hidx + 1;
		        }
		        reg_SBM_c2h_q_hidx = tmp_idx;
            }
        }
    }
    else{
        if(!c2h_data_f_CDD_t_QS_in.empty()){
            struct_c2h_data tmp_c2h_data = c2h_data_f_CDD_t_QS_in.read();
        }
    }
    static ap_uint<1> h2c_data_msg_end_flag = 1;
    static ap_uint<32> h2c_data_msg_count = 0;
    if(h2c_data_msg_end_flag == 0){
        struct_h2c_data tmp_h2c_data;
        tmp_h2c_data.data = 0;
        tmp_h2c_data.tuser_qid = 0;
        h2c_data_msg_count++;
        if(h2c_data_msg_count == 64){
            h2c_data_msg_end_flag = 1;
            tmp_h2c_data.last = 1;
            h2c_data_msg_count = 0;
        }
        else{
            tmp_h2c_data.last = 0;
        }
        h2c_data_f_QS_t_DD_out << tmp_h2c_data;
    }
    else if(!h2c_cmd_f_CD_t_QS_in.empty()){
        struct_h2c_cmd tmp_h2c_cmd = h2c_cmd_f_CD_t_QS_in.read();
        if(tmp_h2c_cmd.len == DSM_CACHE_LINE_SIZE){ 
            struct_h2c_data tmp_h2c_data;
            tmp_h2c_data.data = 0xaaaa;
            tmp_h2c_data.tuser_qid = 0;
            tmp_h2c_data.last = 0;
            h2c_data_msg_end_flag = 0;
            h2c_data_msg_count ++;
            h2c_data_f_QS_t_DD_out << tmp_h2c_data;
        }
    }
}