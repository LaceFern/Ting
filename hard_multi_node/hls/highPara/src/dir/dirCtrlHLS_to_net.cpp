#include "../utils.hpp"
#include "../dsmtype.hpp"
void dirCtrlHLS_to_net(
                hls::stream<struct_dir_data >& 	dir_data_init,
                hls::stream<struct_ctrl_msg >& 	ctrl_msg_f_DC_t_CMC_out,
                hls::stream<struct_dir_data_e >& 	dir_data_f_DI_t_DC_in,               
                hls::stream<struct_ctrl_msg >&  ctrl_msg_r_f_DC_t_DC_in,
                hls::stream<struct_dir_data_e >& 	dir_data_f_DC_t_DI_out,
                hls::stream<struct_ctrl_msg >&  ctrl_msg_w_f_DC_t_DC_out,
                struct_ctrl_msg & debug_ctrl_msg,
                struct_dir_data_e & debug_dir_data,
                ap_uint<8> & debug_copy_flag
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=dir_data_init depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_f_DC_t_CMC_out depth=16
#pragma HLS INTERFACE axis register both port=dir_data_f_DI_t_DC_in depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_r_f_DC_t_DC_in depth=16
#pragma HLS INTERFACE axis register both port=dir_data_f_DC_t_DI_out depth=16
#pragma HLS INTERFACE axis register both port=ctrl_msg_w_f_DC_t_DC_out depth=16
#pragma HLS aggregate variable=dir_data_init compact=bit
#pragma HLS aggregate variable=ctrl_msg_f_DC_t_CMC_out compact=bit
#pragma HLS aggregate variable=dir_data_f_DI_t_DC_in compact=bit
#pragma HLS aggregate variable=ctrl_msg_r_f_DC_t_DC_in compact=bit
#pragma HLS aggregate variable=dir_data_f_DC_t_DI_out compact=bit
#pragma HLS aggregate variable=ctrl_msg_w_f_DC_t_DC_out compact=bit
#pragma HLS INTERFACE ap_none port=debug_ctrl_msg
#pragma HLS INTERFACE ap_none port=debug_dir_data
#pragma HLS INTERFACE ap_none port=debug_copy_flag
        static ap_uint<8> reg_copy_flag = 0;
        static struct_ctrl_msg reg_ctrl_msg;
        static struct_dir_data_e reg_dir_data;
#pragma HLS reset variable=reg_copy_flag 
#pragma HLS array_partition variable=reg_ctrl_msg.dir_gblock_bitmap dim=0 complete
#pragma HLS array_partition variable=reg_dir_data.dir_gblock_bitmap dim=0 complete
        ap_uint<8> wire_copy_flag = reg_copy_flag;
        struct_ctrl_msg wire_ctrl_msg = reg_ctrl_msg;
        struct_dir_data_e wire_dir_data = reg_dir_data;
        debug_ctrl_msg = reg_ctrl_msg;
        debug_dir_data = reg_dir_data;
        debug_copy_flag = reg_copy_flag;
        if(!dir_data_init.empty()){
                struct_dir_data tmp_dir_data_init = dir_data_init.read();
                struct_ctrl_msg tmp_ctrl_msg;
                tmp_ctrl_msg.mtype = RawMessageType::INIT_DIR;
                tmp_ctrl_msg.dir_gblock_index = tmp_dir_data_init.dir_gblock_index;
                struct_dir_data_e tmp_dir_data;
                dir_data_to_dir_data_e(tmp_dir_data_init, tmp_dir_data);
                dir_data_f_DC_t_DI_out << tmp_dir_data; 
                ctrl_msg_w_f_DC_t_DC_out << tmp_ctrl_msg;
        }
        else{
                if(wire_copy_flag == 0){        
                        if(!ctrl_msg_r_f_DC_t_DC_in.empty() && !dir_data_f_DI_t_DC_in.empty()){
                                struct_ctrl_msg tmp_ctrl_msg = ctrl_msg_r_f_DC_t_DC_in.read();
                                struct_dir_data_e tmp_dir_data = dir_data_f_DI_t_DC_in.read();
                                ap_uint<8> tmp_agent_node_id = 0;
                                ap_uint<8> tmp_count = tmp_dir_data.replica_count;
                                tmp_ctrl_msg.dir_gblock_addr = tmp_dir_data.dir_gblock_addr;
                                for(int i = 0; i < max_node_num; i++){
                                        if(tmp_ctrl_msg.mtype != AGENT_2_DIR_WRITE_MISS_SHARED_ACK
                                                && tmp_ctrl_msg.mtype != AGENT_2_DIR_WRITE_SHARED_NOTONLY_ACK){
                                                tmp_ctrl_msg.dir_gblock_bitmap[i] = tmp_dir_data.dir_gblock_bitmap[i];
                                        }
                                        if(tmp_dir_data.dir_gblock_bitmap[i] == 1){
                                                tmp_agent_node_id = i;
                                        }
                                }
                                switch(tmp_ctrl_msg.mtype){
                                        case RawMessageType::R_READ_MISS:{
                                                switch(tmp_dir_data.state){
                                                        case RawState::S_UNSHARED:
                                                        case RawState::S_SHARED: {
                                                                if(tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.req_cline_nodeID] == 0){
                                                                        tmp_dir_data.state = RawState::S_SHARED;
                                                                        tmp_dir_data.replica_count = tmp_count + 1;
                                                                        tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.req_cline_nodeID] = 1;
                                                                        tmp_dir_data.agent_cline_addr[tmp_ctrl_msg.req_cline_nodeID] = tmp_ctrl_msg.req_cline_addr;
                                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_READ_MISS_ACKDATA;
                                                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                        ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                }
                                                                else{
                                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_BITMAP_CHECK_FAIL; 
                                                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                        ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                }
                                                                break;
                                                        }
                                                        case RawState::S_DIRTY: {  
                                                                if(tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.req_cline_nodeID] == 0){
                                                                        tmp_dir_data.state = RawState::S_TO_SHARED;
                                                                        tmp_dir_data.agent_cline_addr[tmp_ctrl_msg.req_cline_nodeID] = tmp_ctrl_msg.req_cline_addr;
                                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_AGENT_READ_MISS_DIRTY;
                                                                        tmp_ctrl_msg.agent_cline_nodeID = tmp_agent_node_id;
                                                                        tmp_ctrl_msg.agent_cline_addr = tmp_dir_data.agent_cline_addr[tmp_agent_node_id];
                                                                        tmp_ctrl_msg.dst_node_id = tmp_agent_node_id;
                                                                        ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                }
                                                                else{
                                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_BITMAP_CHECK_FAIL; 
                                                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                        ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                }
                                                                break;
                                                        }
                                                        default:{
                                                                tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_M_CHECK_FAIL; 
                                                                tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                break;
                                                        } 
                                                }      
                                                break;
                                        }
                                        case RawMessageType::R_WRITE_MISS:{
                                                switch(tmp_dir_data.state){
                                                        case RawState::S_UNSHARED: {
                                                                if(tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.req_cline_nodeID] == 0){
                                                                        tmp_dir_data.state = RawState::S_DIRTY;
                                                                        tmp_dir_data.replica_count = 1;
                                                                        tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.req_cline_nodeID] = 1;
                                                                        tmp_dir_data.agent_cline_addr[tmp_ctrl_msg.req_cline_nodeID] = tmp_ctrl_msg.req_cline_addr;
                                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_WRITE_MISS_ACKDATA;
                                                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                        ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                }
                                                                else{
                                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_BITMAP_CHECK_FAIL; 
                                                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                        ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                }
                                                                break;
                                                        }
                                                        case RawState::S_SHARED: {
                                                                if(tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.req_cline_nodeID] == 0){
                                                                        reg_dir_data = tmp_dir_data;
                                                                        reg_ctrl_msg = tmp_ctrl_msg;
                                                                        reg_copy_flag = 1;
                                                                        tmp_dir_data.state = RawState::S_TO_DIRTY;
                                                                        tmp_dir_data.agent_cline_addr[tmp_ctrl_msg.req_cline_nodeID] = tmp_ctrl_msg.req_cline_addr;
                                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_WRITE_MISS_SHAREDDATA;
                                                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                        ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                }
                                                                else{
                                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_BITMAP_CHECK_FAIL; 
                                                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                        ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                }
                                                                break;
                                                        }
                                                        case RawState::S_DIRTY: {
                                                                if(tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.req_cline_nodeID] == 0){
                                                                        tmp_dir_data.state = RawState::S_TO_DIRTY;
                                                                        tmp_dir_data.agent_cline_addr[tmp_ctrl_msg.req_cline_nodeID] = tmp_ctrl_msg.req_cline_addr;
                                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_AGENT_WRITE_MISS_DIRTY;
                                                                        tmp_ctrl_msg.agent_cline_nodeID = tmp_agent_node_id;
                                                                        tmp_ctrl_msg.agent_cline_addr = tmp_dir_data.agent_cline_addr[tmp_agent_node_id];
                                                                        tmp_ctrl_msg.dst_node_id = tmp_agent_node_id;
                                                                        ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                }
                                                                else{
                                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_BITMAP_CHECK_FAIL; 
                                                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                        ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                }
                                                                break;
                                                        }
                                                        default:{
                                                                tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_M_CHECK_FAIL; 
                                                                tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg; 
                                                                break;
                                                        } 
                                                }
                                                break;
                                        }
                                        case RawMessageType::R_WRITE_SHARED:{ 
                                                switch(tmp_dir_data.state){
                                                        case RawState::S_SHARED: {
                                                                if(tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.req_cline_nodeID] == 0){
                                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_BITMAP_CHECK_FAIL; 
                                                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                        ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                }
                                                                else{
                                                                        if(tmp_count > 1){
                                                                                reg_dir_data = tmp_dir_data;
                                                                                reg_ctrl_msg = tmp_ctrl_msg;
                                                                                reg_copy_flag = 1;
                                                                                tmp_dir_data.state = RawState::S_TO_DIRTY;
                                                                        }
                                                                        else{
                                                                                tmp_dir_data.state = RawState::S_DIRTY;
                                                                                tmp_dir_data.replica_count = 1;
                                                                                tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.req_cline_nodeID] = 1;
                                                                                tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_WRITE_SHARED_ONLYACK;
                                                                                tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                                ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg; 
                                                                        }       
                                                                }
                                                                break;
                                                        }
                                                        default:{
                                                                tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_M_CHECK_FAIL; 
                                                                tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg; 
                                                                break;
                                                        } 
                                                }
                                                break;
                                        }
                                        case RawMessageType::R_EVICT_SHARED:{ 
                                                switch(tmp_dir_data.state){
                                                        case RawState::S_SHARED: {
                                                                if(tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.req_cline_nodeID] == 0){
                                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_BITMAP_CHECK_FAIL; 
                                                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                        ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                }
                                                                else{
                                                                        if(tmp_count == 1){
                                                                                tmp_dir_data.state = RawState::S_UNSHARED;
                                                                        }
                                                                        else{
                                                                                tmp_dir_data.state = RawState::S_SHARED;
                                                                        }
                                                                        tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.req_cline_nodeID] = 0;
                                                                        tmp_dir_data.replica_count = tmp_count - 1;
                                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_EVICT_SHARED_ACK; 
                                                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                        ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                }       
                                                                break;
                                                        }
                                                        default:{
                                                                tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_M_CHECK_FAIL; 
                                                                tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg; 
                                                                break;
                                                        } 
                                                }
                                                break;
                                        }
                                        case RawMessageType::R_EVICT_DIRTY:{ 
                                                switch(tmp_dir_data.state){
                                                        case RawState::S_DIRTY: {
                                                                if(tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.req_cline_nodeID] == 0){
                                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_BITMAP_CHECK_FAIL; 
                                                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                        ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                }
                                                                else{
                                                                        tmp_dir_data.state = RawState::S_UNSHARED;
                                                                        tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.req_cline_nodeID] = 0;
                                                                        tmp_dir_data.replica_count = tmp_count - 1;
                                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_EVICT_DIRTY_ACK; 
                                                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                        ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                }
                                                                break;
                                                        }
                                                        default:{
                                                                tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_M_CHECK_FAIL; 
                                                                tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg; 
                                                                break;
                                                        } 
                                                }
                                                break;
                                        }
                                        case RawMessageType::AGENT_2_DIR_READ_MISS_DIRTY_ACKDATA:{ 
                                                switch(tmp_dir_data.state){
                                                        case RawState::S_TO_SHARED:{
                                                                tmp_dir_data.state = RawState::S_SHARED;
                                                                tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.req_cline_nodeID] = 1;
                                                                tmp_dir_data.replica_count = tmp_count + 1;
                                                                break;
                                                        }
                                                        default:{
                                                                break;
                                                        }
                                                }
                                                break;
                                        }
                                        case RawMessageType::AGENT_2_DIR_WRITE_MISS_DIRTY_ACK:{ 
                                                switch(tmp_dir_data.state){
                                                        case RawState::S_TO_DIRTY:{
                                                                tmp_dir_data.state = RawState::S_DIRTY;
                                                                tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.req_cline_nodeID] = 1;
                                                                tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.agent_cline_nodeID] = 0;
                                                                tmp_dir_data.replica_count = 1;
                                                                break;
                                                        }
                                                        default:{
                                                                break;
                                                        }
                                                }
                                                break;
                                        }
                                        case RawMessageType::AGENT_2_DIR_WRITE_MISS_SHARED_ACK:{ 
                                                switch(tmp_dir_data.state){
                                                        case RawState::S_TO_DIRTY:{
                                                                tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.req_cline_nodeID] = 1;
                                                                tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.agent_cline_nodeID] = 0;
                                                                if(tmp_count == 1){
                                                                        tmp_dir_data.state = RawState::S_DIRTY;
                                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_WRITE_MISS_SHARED_AGGACK_ACK; 
                                                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                        ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                }                                      
                                                                else{
                                                                        tmp_dir_data.replica_count = tmp_count - 1;
                                                                }
                                                                break;
                                                        }
                                                        default:{
                                                                break;
                                                        }
                                                }
                                                break;
                                        }
                                        case RawMessageType::AGENT_2_DIR_WRITE_SHARED_NOTONLY_ACK:{ 
                                                switch(tmp_dir_data.state){
                                                        case RawState::S_TO_DIRTY:{
                                                                tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.req_cline_nodeID] = 1;
                                                                tmp_dir_data.dir_gblock_bitmap[tmp_ctrl_msg.agent_cline_nodeID] = 0;
                                                                if(tmp_count == 2){
                                                                        tmp_dir_data.state = RawState::S_DIRTY;
                                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_APP_WRITE_SHARED_NOTONLY_AGGACK_ACK; 
                                                                        tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.req_cline_nodeID;
                                                                        ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                        tmp_dir_data.replica_count = 1;
                                                                }     
                                                                else{
                                                                        tmp_dir_data.replica_count = tmp_count - 1;
                                                                }                               
                                                                break;
                                                        }
                                                        default:{
                                                                break;
                                                        }
                                                }
                                                break;
                                        }
                                        default:{
                                                break;
                                        }
                                }
                                dir_data_f_DC_t_DI_out << tmp_dir_data; 
                                ctrl_msg_w_f_DC_t_DC_out << tmp_ctrl_msg;
                        }
                }
                else{
                        switch(wire_ctrl_msg.mtype){
                                case RawMessageType::R_WRITE_MISS:{ 
                                        switch(wire_dir_data.state){
                                                case RawState::S_SHARED: {
                                                        struct_ctrl_msg tmp_ctrl_msg = wire_ctrl_msg;
                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_AGENT_WRITE_MISS_SHARED;
                                                        ap_uint<8> tmp_nodeID = 0xff;
                                                        for(int i = 0; i < max_node_num; i++){
                                                                if(wire_dir_data.dir_gblock_bitmap[i] == 1){
                                                                        tmp_nodeID = i;
                                                                }
                                                        }
                                                        if(tmp_nodeID == 0xff){
                                                                reg_copy_flag = 0;
                                                        }
                                                        else{
                                                                tmp_ctrl_msg.agent_cline_nodeID = tmp_nodeID;
                                                                tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.agent_cline_nodeID;
                                                                tmp_ctrl_msg.agent_cline_addr = reg_dir_data.agent_cline_addr[tmp_nodeID];
                                                                ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg;
                                                                reg_dir_data.dir_gblock_bitmap[tmp_nodeID] = 0;
                                                        }
                                                        break;
                                                }
                                                default:{
                                                        break;
                                                }
                                        }
                                        break;
                                }
                                case RawMessageType::R_WRITE_SHARED:{ 
                                        switch(wire_dir_data.state){
                                                case RawState::S_SHARED: {
                                                        struct_ctrl_msg tmp_ctrl_msg = wire_ctrl_msg;
                                                        tmp_ctrl_msg.mtype = RawMessageType::DIR_2_AGENT_WRITE_SHARED_NOTONLY;
                                                        ap_uint<8> tmp_nodeID = 0xff;
                                                        for(int i = 0; i < max_node_num; i++){
                                                                if(wire_dir_data.dir_gblock_bitmap[i] == 1){
                                                                        tmp_nodeID = i;
                                                                }
                                                        }
                                                        if(tmp_nodeID == 0xff){
                                                                reg_copy_flag = 0;
                                                        }
                                                        else if(tmp_nodeID == tmp_ctrl_msg.req_cline_nodeID){
                                                                reg_dir_data.dir_gblock_bitmap[tmp_nodeID] = 0;
                                                        }
                                                        else{
                                                                tmp_ctrl_msg.agent_cline_nodeID = tmp_nodeID;
                                                                tmp_ctrl_msg.dst_node_id = tmp_ctrl_msg.agent_cline_nodeID;
                                                                tmp_ctrl_msg.agent_cline_addr = reg_dir_data.agent_cline_addr[tmp_nodeID];
                                                                ctrl_msg_f_DC_t_CMC_out << tmp_ctrl_msg; 
                                                                reg_dir_data.dir_gblock_bitmap[tmp_nodeID] = 0;
                                                        }
                                                        break;
                                                }
                                                default:{
                                                        break;
                                                }
                                        }
                                        break;
                                }
                                default:{
                                        break;
                                }
                        }   
                }
        } 
}
