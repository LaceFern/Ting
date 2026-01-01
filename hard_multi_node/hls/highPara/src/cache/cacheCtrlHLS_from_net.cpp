#include "../utils.hpp"
#include "../dsmtype.hpp"
void cacheCtrlHLS_from_net(
                hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_D_t_CC_in,
                hls::stream<struct_ctrl_msg >&  ctrl_msg_f_CC_t_CMD_out,
                hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_CC_t_CC_out
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_D_t_CC_in depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_CC_t_CMD_out depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_CC_t_CC_out depth=16
#pragma HLS aggregate variable=ctrl_msg_f_D_t_CC_in compact=bit
#pragma HLS aggregate variable=ctrl_msg_f_CC_t_CMD_out compact=bit
#pragma HLS aggregate variable=ctrl_msg_f_CC_t_CC_out compact=bit
        static ap_uint<8> reg_copy_flag = 0;
        static ap_uint<8> reg_copy_type = 0;
        static struct_ctrl_msg reg_ctrl_msg;
#pragma HLS reset variable=reg_copy_flag 
        ap_uint<8> wire_copy_flag = reg_copy_flag;
        ap_uint<8> wire_copy_type = reg_copy_type;
        struct_ctrl_msg wire_ctrl_msg = reg_ctrl_msg;
        if(wire_copy_flag == 0){
                if(!ctrl_msg_f_D_t_CC_in.empty()){
                        struct_ctrl_msg tmp_ctrl_msg = ctrl_msg_f_D_t_CC_in.read();
                        switch(tmp_ctrl_msg.mtype){
                                case RawMessageType::DIR_2_AGENT_WRITE_SHARED_NOTONLY:{
                                        tmp_ctrl_msg.dst_thread_type = THREAD_TYPE_SYS;
                                        ctrl_msg_f_CC_t_CMD_out << tmp_ctrl_msg;
                                        tmp_ctrl_msg.mtype = RawMessageType::AGENT_2_DIR_WRITE_SHARED_NOTONLY_ACK;
                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.dir_gblock_nodeID;
                                        ctrl_msg_f_CC_t_CC_out << tmp_ctrl_msg;
                                        break;
                                }
                                case RawMessageType::DIR_2_AGENT_WRITE_MISS_SHARED:{
                                        tmp_ctrl_msg.dst_thread_type = THREAD_TYPE_SYS;
                                        ctrl_msg_f_CC_t_CMD_out << tmp_ctrl_msg;
                                        tmp_ctrl_msg.mtype = RawMessageType::AGENT_2_DIR_WRITE_MISS_SHARED_ACK;
                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.dir_gblock_nodeID;
                                        ctrl_msg_f_CC_t_CC_out << tmp_ctrl_msg;
                                        break;
                                }
                                case RawMessageType::DIR_2_AGENT_READ_MISS_DIRTY:{
                                        tmp_ctrl_msg.dst_thread_type = THREAD_TYPE_SYS;
                                        ctrl_msg_f_CC_t_CMD_out << tmp_ctrl_msg;
                                        reg_ctrl_msg = tmp_ctrl_msg;
                                        reg_copy_flag = 1;
                                        tmp_ctrl_msg.mtype = RawMessageType::AGENT_2_APP_READ_MISS_DIRTY_ACKDATA;
                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                        ctrl_msg_f_CC_t_CC_out << tmp_ctrl_msg;
                                        break;
                                }
                                case RawMessageType::DIR_2_AGENT_WRITE_MISS_DIRTY:{
                                        tmp_ctrl_msg.dst_thread_type = THREAD_TYPE_SYS;
                                        ctrl_msg_f_CC_t_CMD_out << tmp_ctrl_msg;
                                        reg_ctrl_msg = tmp_ctrl_msg;
                                        reg_copy_flag = 1;
                                        tmp_ctrl_msg.mtype = RawMessageType::AGENT_2_APP_WRITE_MISS_DIRTY_ACKDATA;
                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                        ctrl_msg_f_CC_t_CC_out << tmp_ctrl_msg;
                                        break;
                                }
                                default:{
                                        tmp_ctrl_msg.dst_thread_type = THREAD_TYPE_APP;
                                        ctrl_msg_f_CC_t_CMD_out << tmp_ctrl_msg;
                                        break;
                                }
                        }
                }
        }
        else{
                switch(wire_ctrl_msg.mtype){
                        case RawMessageType::DIR_2_AGENT_READ_MISS_DIRTY:{
                                struct_ctrl_msg tmp_ctrl_msg = wire_ctrl_msg;
                                tmp_ctrl_msg.mtype = RawMessageType::AGENT_2_DIR_READ_MISS_DIRTY_ACKDATA;
                                tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.dir_gblock_nodeID;
                                ctrl_msg_f_CC_t_CC_out << tmp_ctrl_msg;
                                reg_copy_flag = 0;
                                break;
                        }
                        case RawMessageType::DIR_2_AGENT_WRITE_MISS_DIRTY:{
                                struct_ctrl_msg tmp_ctrl_msg = wire_ctrl_msg;
                                tmp_ctrl_msg.mtype = RawMessageType::AGENT_2_DIR_WRITE_MISS_DIRTY_ACK;
                                tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.dir_gblock_nodeID;
                                ctrl_msg_f_CC_t_CC_out << tmp_ctrl_msg;
                                reg_copy_flag = 0;
                                break;
                        }
                        default:{
                                break;
                        }
                }
        }
}