#include "../utils.hpp"
#include "../dsmtype.hpp"
void dirCtrlHLS_from_net(
		hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_D_t_DC_in,
                hls::stream<struct_dir_cmd >& 	dir_cmd_f_DC_t_DI_out,           
                hls::stream<struct_ctrl_msg >&  ctrl_msg_r_f_DC_t_DC_out,
                hls::stream<struct_dir_ack >& 	dir_ack_f_DI_t_DC_in,
                hls::stream<struct_ctrl_msg >& 	ctrl_msg_w_f_DC_t_DC_in,
                ap_uint<32>&			debug_source_flag,
                ap_uint<32>&			debug_retry_flag
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_D_t_DC_in depth=16
#pragma HLS INTERFACE axis register both port=dir_cmd_f_DC_t_DI_out depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_r_f_DC_t_DC_out depth=16
#pragma HLS INTERFACE axis register both port=dir_ack_f_DI_t_DC_in depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_w_f_DC_t_DC_in depth=16
#pragma HLS aggregate variable=ctrl_msg_f_D_t_DC_in compact=bit
#pragma HLS aggregate variable=dir_cmd_f_DC_t_DI_out compact=bit
#pragma HLS aggregate variable=ctrl_msg_r_f_DC_t_DC_out compact=bit
#pragma HLS aggregate variable=dir_ack_f_DI_t_DC_in compact=bit
#pragma HLS aggregate variable=ctrl_msg_w_f_DC_t_DC_in compact=bit
#pragma HLS INTERFACE ap_none port=debug_source_flag
#pragma HLS INTERFACE ap_none port=debug_retry_flag
        static ap_uint<8> reg_init_flag = 1;
#pragma HLS reset variable=reg_init_flag 
        ap_uint<8> wire_init_flag = reg_init_flag;
        static ap_uint<8> reg_retry_flag = 0;
        static struct_ctrl_msg reg_retry_ctrl_msg;
#pragma HLS reset variable=reg_retry_flag 
        ap_uint<8> wire_retry_flag = reg_retry_flag;
        struct_ctrl_msg wire_retry_ctrl_msg = reg_retry_ctrl_msg;
        debug_retry_flag = wire_retry_flag;
	static ap_uint<32> reg_gb_index_arr[LOCK_BKT_NUM][LOCK_BKT_SIZE] = {0xffffffff};
#pragma HLS reset variable=reg_gb_index_arr
#pragma HLS array_partition variable=reg_gb_index_arr dim=0 complete
        static ap_uint<1> reg_source_flag = 0;
#pragma HLS reset variable=reg_source_flag
        ap_uint<1> wire_source_flag = reg_source_flag;
        debug_source_flag = wire_source_flag;
        if(wire_init_flag){
                for(int i = 0; i < LOCK_BKT_NUM; i++){
                        for(int j = 0; j < LOCK_BKT_SIZE; j++){
                                reg_gb_index_arr[i][j] = 0xffffffff;
                        }
                }
                reg_init_flag = 0;
        }
        else{
                ap_uint<8> tmp_lock_flag = 0;
                struct_dir_ack tmp_dir_ack;
                struct_ctrl_msg tmp_ctrl_msg;
                struct_ctrl_msg tmp_orig_ctrl_msg;
                struct_dir_cmd tmp_dir_cmd;
                if(wire_source_flag){
                        reg_source_flag = 0;
                        if(!dir_ack_f_DI_t_DC_in.empty() && !ctrl_msg_w_f_DC_t_DC_in.empty()){
                                tmp_dir_ack = dir_ack_f_DI_t_DC_in.read();
                                tmp_ctrl_msg = ctrl_msg_w_f_DC_t_DC_in.read();
                                ap_uint<8> tmp_bucket_id = tmp_ctrl_msg.dir_gblock_index(LOCK_BKT_WIDTH - 1, 0);
                                reg_gb_index_arr[tmp_bucket_id][tmp_ctrl_msg.lock_position] = 0xffffffff;
                        }
                }
                else{
                        reg_source_flag = 1;
                        if(wire_retry_flag == 1){
                                tmp_orig_ctrl_msg = wire_retry_ctrl_msg;
                                tmp_lock_flag = 1;
                        }
                        else if(!ctrl_msg_f_D_t_DC_in.empty()){
                                tmp_orig_ctrl_msg = ctrl_msg_f_D_t_DC_in.read();
                                tmp_lock_flag = 1;
                        }
                        if(tmp_lock_flag){
                                ap_uint<8> tmp_free_slot_pos = 0xff;
                                ap_uint<8> tmp_conflict_flag = 0;
                                ap_uint<8> tmp_bucket_id = tmp_orig_ctrl_msg.dir_gblock_index(LOCK_BKT_WIDTH - 1, 0);
                                for(int i = 0; i < LOCK_BKT_SIZE; i++){
                                        if(tmp_orig_ctrl_msg.dir_gblock_index == reg_gb_index_arr[tmp_bucket_id][i]){
                                                tmp_conflict_flag = 1;
                                        }
                                        if(reg_gb_index_arr[tmp_bucket_id][i] == 0xffffffff){
                                                tmp_free_slot_pos = i;
                                        }
                                }
                                if(tmp_conflict_flag == 1 || tmp_free_slot_pos == 0xff){
                                        reg_retry_flag = 1;
                                        reg_retry_ctrl_msg = tmp_orig_ctrl_msg;
                                }
                                else{
                                        reg_retry_flag = 0;
                                        reg_gb_index_arr[tmp_bucket_id][tmp_free_slot_pos] = tmp_orig_ctrl_msg.dir_gblock_index;
                                        tmp_orig_ctrl_msg.lock_position = tmp_free_slot_pos;
                                        ctrl_msg_r_f_DC_t_DC_out << tmp_orig_ctrl_msg;
                                        tmp_dir_cmd.dir_gblock_index = tmp_orig_ctrl_msg.dir_gblock_index;
                                        dir_cmd_f_DC_t_DI_out << tmp_dir_cmd;
                                } 
                        }
                }
        }
}
