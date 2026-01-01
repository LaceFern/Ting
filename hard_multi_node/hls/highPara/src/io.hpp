#include <hls_stream.h>
#include "ap_int.h"
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <iomanip>
typedef struct net_rxtx{
	ap_uint<512>	data;
	ap_uint<64>		keep;
	ap_uint<1>		last;
} struct_net_rxtx;
typedef struct pkt{
	ap_uint<512> data;
	ap_uint<64>  keep;
	ap_uint<1>   last;
} struct_pkt;
typedef struct ddr_aw{
	ap_uint<34>		awaddr;
	ap_uint<2>		awburst;
	ap_uint<4>		awcache;
	ap_uint<6>		awid;
	ap_uint<8>		awlen;
	ap_uint<2>		awlock;
	ap_uint<3>		awprot;
	ap_uint<4>		awqos;
	ap_uint<3>		awsize;
} struct_ddr_aw;
typedef struct ddr_w{
	ap_uint<512>	wdata;
	ap_uint<1>		wlast;
	ap_uint<64>		wstrb;
} struct_ddr_w;
typedef struct ddr_ar{
	ap_uint<34>		araddr;
	ap_uint<2>		arburst;
	ap_uint<4>		arcache;
	ap_uint<6>		arid;
	ap_uint<8>		arlen;
	ap_uint<2>		arlock;
	ap_uint<3>		arprot;
	ap_uint<4>		arqos;
	ap_uint<3>		arsize;
} struct_ddr_ar;
typedef struct ddr_r{
	ap_uint<512>	rdata;
	ap_uint<1>		rlast;
	ap_uint<2>		rresp;
	ap_uint<6>		rid;
} struct_ddr_r;
typedef struct ddr_b{
	ap_uint<2>		bresp;
	ap_uint<6>		bid;
} struct_ddr_b;
typedef struct h2c_cmd{
	ap_uint<64>		addr;
	ap_uint<32>		len;
	ap_uint<11>		qid;
} struct_h2c_cmd;
typedef struct h2c_data{
	ap_uint<512>	data;
	ap_uint<11>		tuser_qid;
	ap_uint<1>		last;
} struct_h2c_data;
typedef struct c2h_cmd{
	ap_uint<64>		addr;
	ap_uint<32>		len;
	ap_uint<11>		qid;
} struct_c2h_cmd;
typedef struct c2h_data{
	ap_uint<512>	data;
	ap_uint<11>		ctrl_qid;
	ap_uint<1>		last;
} struct_c2h_data;
typedef struct h2c_w{
	ap_uint<512>	wdata;
	ap_uint<1>		wlast;
	ap_uint<64>		wstrb;
} struct_h2c_w;
typedef struct hbm_aw{
	ap_uint<33>		awaddr;
	ap_uint<2>		awburst;
	ap_uint<8>		awlen;
	ap_uint<3>		awsize;
	ap_uint<6>		awid;
} struct_hbm_aw;
typedef struct hbm_w{
	ap_uint<256>	wdata;
	ap_uint<1>		wlast;
	ap_uint<32>		wstrb;
} struct_hbm_w;
typedef struct hbm_ar{
	ap_uint<33>		araddr;
	ap_uint<2>		arburst;
	ap_uint<8>		arlen;
	ap_uint<3>		arsize;
	ap_uint<6>		arid;
} struct_hbm_ar;
typedef struct hbm_r{
	ap_uint<256>	rdata;
	ap_uint<1>		rlast;
	ap_uint<2>		rresp;
	ap_uint<6>		rid;
} struct_hbm_r;
typedef struct hbm_b{
	ap_uint<2>		bresp;
	ap_uint<6>		bid;
} struct_hbm_b;