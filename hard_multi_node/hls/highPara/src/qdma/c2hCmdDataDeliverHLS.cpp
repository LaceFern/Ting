#include "../utils.hpp"
void c2hCmdDataDeliverHLS(
    hls::stream<struct_c2h_cmd >& 	c2h_cmd_f_D_t_CDD_in,
    hls::stream<struct_c2h_data >& 	c2h_data_f_D_t_CDD_in,
    hls::stream<struct_c2h_cmd >& 	c2h_cmd_f_SBM_t_CDD_in,
    hls::stream<struct_c2h_data >& 	c2h_data_f_SBM_t_CDD_in,
    hls::stream<struct_c2h_cmd >& 	c2h_cmd_f_ABM_t_CDD_in,
    hls::stream<struct_c2h_data >& 	c2h_data_f_ABM_t_CDD_in,
    hls::stream<struct_c2h_cmd >& 	c2h_cmd_f_CDD_t_QS_out,
    hls::stream<struct_c2h_data >& 	c2h_data_f_CDD_t_QS_out,
    ap_uint<32>&            debug_ready_flag_idx,
    ap_uint<1>&             debug_end_flag,
    ap_uint<32>&			CDD_debug_count,
	struct_c2h_cmd&		    CDD_debug_cmd,
    struct_c2h_data&		CDD_debug_data
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=c2h_cmd_f_D_t_CDD_in depth=16
#pragma HLS INTERFACE axis register both port=c2h_data_f_D_t_CDD_in depth=16
#pragma HLS INTERFACE axis register both port=c2h_cmd_f_SBM_t_CDD_in depth=16
#pragma HLS INTERFACE axis register both port=c2h_data_f_SBM_t_CDD_in depth=16
#pragma HLS INTERFACE axis register both port=c2h_cmd_f_ABM_t_CDD_in depth=16
#pragma HLS INTERFACE axis register both port=c2h_data_f_ABM_t_CDD_in depth=16
#pragma HLS INTERFACE axis register both port=c2h_cmd_f_CDD_t_QS_out depth=16
#pragma HLS INTERFACE axis register both port=c2h_data_f_CDD_t_QS_out depth=16
#pragma HLS aggregate variable=c2h_cmd_f_D_t_CDD_in compact=bit
#pragma HLS aggregate variable=c2h_data_f_D_t_CDD_in compact=bit
#pragma HLS aggregate variable=c2h_cmd_f_SBM_t_CDD_in compact=bit
#pragma HLS aggregate variable=c2h_data_f_SBM_t_CDD_in compact=bit
#pragma HLS aggregate variable=c2h_cmd_f_ABM_t_CDD_in compact=bit
#pragma HLS aggregate variable=c2h_data_f_ABM_t_CDD_in compact=bit
#pragma HLS aggregate variable=c2h_cmd_f_CDD_t_QS_out compact=bit
#pragma HLS aggregate variable=c2h_data_f_CDD_t_QS_out compact=bit
#pragma HLS INTERFACE ap_none port=debug_ready_flag_idx
#pragma HLS INTERFACE ap_none port=debug_end_flag
#pragma HLS INTERFACE ap_none port=CDD_debug_count
#pragma HLS INTERFACE ap_none port=CDD_debug_cmd
#pragma HLS INTERFACE ap_none port=CDD_debug_data
#pragma HLS aggregate variable=CDD_debug_cmd compact=bit
#pragma HLS aggregate variable=CDD_debug_data compact=bit
	static ap_uint<32> reg_count = 0;
	ap_uint<32> wire_count = reg_count;
	CDD_debug_count = wire_count;
#pragma HLS reset variable=reg_count
	static struct_c2h_cmd reg_cmd;
	struct_c2h_cmd wire_cmd = reg_cmd;
	CDD_debug_cmd = wire_cmd;
#pragma HLS reset variable=reg_cmd
	static struct_c2h_data reg_data;
	struct_c2h_data wire_data = reg_data;
	CDD_debug_data = wire_data;
#pragma HLS reset variable=reg_data
    static struct_c2h_cmd reg_c2h_cmd;
    static ap_uint<1> reg_end_flag = 1;
#pragma HLS reset variable=reg_c2h_cmd
#pragma HLS reset variable=reg_end_flag
    struct_c2h_cmd wire_c2h_cmd = reg_c2h_cmd;
    ap_uint<1> wire_end_flag = reg_end_flag;
    debug_end_flag = reg_end_flag;
    ap_uint<1> tmp_BMAPP_valid_flag = (!c2h_cmd_f_ABM_t_CDD_in.empty() && !c2h_data_f_ABM_t_CDD_in.empty());
    ap_uint<1> tmp_BMSYS_valid_flag = (!c2h_cmd_f_SBM_t_CDD_in.empty() && !c2h_data_f_SBM_t_CDD_in.empty());
    ap_uint<1> tmp_D_valid_flag = (!c2h_cmd_f_D_t_CDD_in.empty() && !c2h_data_f_D_t_CDD_in.empty());
    ap_uint<3> tmp_valid_flag = 0;
    tmp_valid_flag[0] = tmp_BMAPP_valid_flag;
    tmp_valid_flag[1] = tmp_BMSYS_valid_flag;
    tmp_valid_flag[2] = tmp_D_valid_flag;
    ap_uint<32> tmp_ready_flag_idx = 0xffff;
    static ap_uint<32> reg_ready_flag_idx = 0;
    ap_uint<32> wire_ready_flag_idx = reg_ready_flag_idx;
    debug_ready_flag_idx = reg_ready_flag_idx;
    if(wire_end_flag == 1){
        switch(tmp_valid_flag){
            case 0b000:{
                tmp_ready_flag_idx = 0xffff;
                break;
            }
            case 0b001:{
                tmp_ready_flag_idx = 0;
                break;
            }
            case 0b010:{
                tmp_ready_flag_idx = 1;
                break;
            }
            case 0b100:{
                tmp_ready_flag_idx = 2;
                break;
            }
            case 0b111:{
                if(wire_ready_flag_idx == 2){
                    tmp_ready_flag_idx = 0;
                }
                else{
                    tmp_ready_flag_idx = wire_ready_flag_idx + 1;
                }
                break;
            }
            case 0b011:{
                if(wire_ready_flag_idx == 0) tmp_ready_flag_idx = 1;
                else tmp_ready_flag_idx = 0;
                break;
            }
            case 0b101:{
                if(wire_ready_flag_idx == 2) tmp_ready_flag_idx = 0;
                else tmp_ready_flag_idx = 2;
                break;
            }
            case 0b110:{
                if(wire_ready_flag_idx == 1) tmp_ready_flag_idx = 2;
                else tmp_ready_flag_idx = 1;
                break;
            }
        }
        reg_ready_flag_idx = tmp_ready_flag_idx;
        if(tmp_ready_flag_idx == 0){
            struct_c2h_cmd tmp_c2h_cmd = c2h_cmd_f_ABM_t_CDD_in.read();
            struct_c2h_data tmp_c2h_data = c2h_data_f_ABM_t_CDD_in.read();
            if(tmp_c2h_data.last == 0){
                reg_end_flag = 0;
                reg_c2h_cmd = tmp_c2h_cmd;
            }
            tmp_c2h_cmd.qid = 0;
            tmp_c2h_data.ctrl_qid = 0;
            c2h_cmd_f_CDD_t_QS_out << tmp_c2h_cmd;
            c2h_data_f_CDD_t_QS_out << tmp_c2h_data;
            reg_count = wire_count + 1;
            reg_cmd = tmp_c2h_cmd;
            reg_data = tmp_c2h_data;
        }
        else if(tmp_ready_flag_idx == 1){
            struct_c2h_cmd tmp_c2h_cmd = c2h_cmd_f_SBM_t_CDD_in.read();
            struct_c2h_data tmp_c2h_data = c2h_data_f_SBM_t_CDD_in.read();
            if(tmp_c2h_data.last == 0){
                reg_end_flag = 0;
                reg_c2h_cmd = tmp_c2h_cmd;
            }
            tmp_c2h_cmd.qid = 0;
            tmp_c2h_data.ctrl_qid = 0;
            c2h_cmd_f_CDD_t_QS_out << tmp_c2h_cmd;
            c2h_data_f_CDD_t_QS_out << tmp_c2h_data;
            reg_count = wire_count + 1;
            reg_cmd = tmp_c2h_cmd;
            reg_data = tmp_c2h_data;
        }
        else if(tmp_ready_flag_idx == 2){
            struct_c2h_cmd tmp_c2h_cmd = c2h_cmd_f_D_t_CDD_in.read();
            struct_c2h_data tmp_c2h_data = c2h_data_f_D_t_CDD_in.read();
            if(tmp_c2h_data.last == 0){
                reg_end_flag = 0;
                reg_c2h_cmd = tmp_c2h_cmd;
            }
            tmp_c2h_cmd.qid = 0;
            tmp_c2h_data.ctrl_qid = 0;
            c2h_cmd_f_CDD_t_QS_out << tmp_c2h_cmd;
            c2h_data_f_CDD_t_QS_out << tmp_c2h_data;
            reg_count = wire_count + 1;
            reg_cmd = tmp_c2h_cmd;
            reg_data = tmp_c2h_data;
        }
    }
    else if(wire_end_flag == 0){
        if(wire_c2h_cmd.qid == PSEUDO_QID_BMAPP && !c2h_data_f_ABM_t_CDD_in.empty()){
            struct_c2h_data tmp_c2h_data = c2h_data_f_ABM_t_CDD_in.read();
            tmp_c2h_data.ctrl_qid = 0;
            c2h_data_f_CDD_t_QS_out << tmp_c2h_data;
            if(tmp_c2h_data.last == 1){
                reg_end_flag = 1;
            }
        }
        else if(wire_c2h_cmd.qid == PSEUDO_QID_BMSYS && !c2h_data_f_SBM_t_CDD_in.empty()){
            struct_c2h_data tmp_c2h_data = c2h_data_f_SBM_t_CDD_in.read();
            tmp_c2h_data.ctrl_qid = 0;
            c2h_data_f_CDD_t_QS_out << tmp_c2h_data;
            if(tmp_c2h_data.last == 1){
                reg_end_flag = 1;
            }
        }
        else if(wire_c2h_cmd.qid == PSEUDO_QID_D && !c2h_data_f_D_t_CDD_in.empty()){
            struct_c2h_data tmp_c2h_data = c2h_data_f_D_t_CDD_in.read();
            tmp_c2h_data.ctrl_qid = 0;
            c2h_data_f_CDD_t_QS_out << tmp_c2h_data;
            if(tmp_c2h_data.last == 1){
                reg_end_flag = 1;
            }
        }
    }
}