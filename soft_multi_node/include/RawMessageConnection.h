#ifndef __RAWMESSAGECONNECTION_H__
#define __RAWMESSAGECONNECTION_H__
#include "Common.h"
struct RawMessage {
  uint8_t mtype;
  uint8_t req_cline_nodeID;
  uint8_t req_cline_appThreadID; 
  uint64_t req_cline_addr; 
  uint32_t req_cline_index; 
  uint64_t cline_tag; 
  uint8_t  agent_cline_nodeID;
  uint64_t agent_cline_addr;
  uint64_t dir_gblock_addr; 
  uint32_t dir_gblock_index; 
  bool dir_gblock_bitmap[max_node_num];
  uint8_t  dir_gblock_nodeID;
  uint8_t general_c2h_owner; 
  uint8_t init_state; 
  uint8_t init_mac[6]; 
  uint8_t padding_0;
  uint16_t padding_1;
} __attribute__((packed));
union RawMessageUnion {
    RawMessage msg;               
    uint64_t values[8];           
};
inline int bits_in(bool bitmap[]) {
  int count = 0;
  for(int i = 0; i < max_node_num; i++){
    if(bitmap[i]) count++;
  }
  return count;
}
#endif 
