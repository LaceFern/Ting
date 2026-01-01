#include "../utils.hpp"
#include "../dsmtype.hpp"
void h2cCtrlMsgDeliverHLS(
	hls::stream<struct_h2c_w >&  h2c_w_f_QS_t_CMD_in,
	hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_CMD_t_CC_out,
	hls::stream<struct_net_init_msg >& 	net_init_msg,
	hls::stream<struct_dir_data >& 	dir_data_init,
	ap_uint<32>&				CMD_debug_count,
	struct_ctrl_msg&			CMD_debug_msg
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=h2c_w_f_QS_t_CMD_in depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_CMD_t_CC_out depth=16
#pragma HLS INTERFACE axis register both port=net_init_msg depth=16
#pragma HLS INTERFACE axis register both port=dir_data_init depth=16
#pragma HLS aggregate variable=h2c_w_f_QS_t_CMD_in compact=bit
#pragma HLS aggregate variable=ctrl_msg_f_CMD_t_CC_out compact=bit
#pragma HLS aggregate variable=net_init_msg compact=bit
#pragma HLS aggregate variable=dir_data_init compact=bit
#pragma HLS INTERFACE ap_none port=CMD_debug_count
#pragma HLS INTERFACE ap_none port=CMD_debug_msg
#pragma HLS aggregate variable=CMD_debug_msg compact=bit
	static ap_uint<32> reg_count = 0;
	ap_uint<32> wire_count = reg_count;
	CMD_debug_count = wire_count;
#pragma HLS reset variable=reg_count
	static struct_ctrl_msg reg_msg;
	struct_ctrl_msg wire_msg = reg_msg;
	CMD_debug_msg = wire_msg;
#pragma HLS reset variable=reg_msg
#pragma HLS array_partition variable=reg_msg.dir_gblock_bitmap dim=0 complete
    if(!h2c_w_f_QS_t_CMD_in.empty()){
        struct_h2c_w tmp_h2c_w = h2c_w_f_QS_t_CMD_in.read();
		ap_uint<8> mtype = extract_mtype_from_h2c_w(tmp_h2c_w);
		if(mtype == RawMessageType::INIT_DIR){
		    struct_dir_data tmp_dir_data;
		    h2c_w_to_dir_data(tmp_h2c_w, tmp_dir_data);
		    dir_data_init << tmp_dir_data;
		}
		else if(mtype == RawMessageType::INIT_NET){
		    struct_net_init_msg tmp_net_init_msg;
		    h2c_w_to_net_init_msg(tmp_h2c_w, tmp_net_init_msg);
		    net_init_msg << tmp_net_init_msg;
		}
		else{
            struct_ctrl_msg tmp_ctrl_msg;
            h2c_w_to_ctrl_msg(tmp_h2c_w, tmp_ctrl_msg);
            ctrl_msg_f_CMD_t_CC_out << tmp_ctrl_msg;
			reg_count = wire_count + 1;
			reg_msg = tmp_ctrl_msg;
		}
	}
}