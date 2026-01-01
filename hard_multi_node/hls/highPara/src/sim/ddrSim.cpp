#include "../utils.hpp"
void ddrSim(
        hls::stream<struct_ddr_aw >& 	    ddr_aw,
        hls::stream<struct_ddr_w >& 	    ddr_w,
        hls::stream<struct_ddr_ar >& 		ddr_ar,
        hls::stream<struct_ddr_r >& 	    ddr_r,
        hls::stream<struct_ddr_b >& 	    ddr_b
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis register both port=ddr_aw depth=16
#pragma HLS INTERFACE axis register both port=ddr_w depth=16
#pragma HLS INTERFACE axis register both port=ddr_ar depth=16
#pragma HLS INTERFACE axis register both port=ddr_r depth=16
#pragma HLS INTERFACE axis register both port=ddr_b depth=16
#pragma HLS aggregate variable=ddr_aw compact=bit
#pragma HLS aggregate variable=ddr_w compact=bit
#pragma HLS aggregate variable=ddr_ar compact=bit
#pragma HLS aggregate variable=ddr_r compact=bit
#pragma HLS aggregate variable=ddr_b compact=bit
	static ap_uint<512> ddr_data[16384];
	if(!ddr_aw.empty() && !ddr_w.empty()){
		struct_ddr_aw tmp_aw = ddr_aw.read();
		struct_ddr_w tmp_w = ddr_w.read();
		ddr_data[tmp_aw.awaddr >> 6] = tmp_w.wdata;
		struct_ddr_b tmp_b;
		tmp_b.bresp = 0;
		tmp_b.bid = 0;
		ddr_b << tmp_b;
	}
	if(!ddr_ar.empty()){
		struct_ddr_ar tmp_ar = ddr_ar.read();
		struct_ddr_r tmp_r;
		tmp_r.rdata = ddr_data[tmp_ar.araddr >> 6];
		tmp_r.rlast = 1;
		tmp_r.rresp = 0;
		tmp_r.rid = 0;
		ddr_r << tmp_r;
	}
}
