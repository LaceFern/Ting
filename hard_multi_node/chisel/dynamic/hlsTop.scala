package static.u280

import chisel3._
import chisel3.util._
import cmac._
import qdma._
import common._
import common.storage._
import common.axi._
import hbm._
import ddr._
import static._
import common.partialReconfig.AlveoStaticIO
import common.partialReconfig.DDRPort

class HLS_COMMON_AXIS(DATA_WIDTH:Int)extends Bundle{
    val data        = UInt(DATA_WIDTH.W)
}

// From ddrInstHLS_aw_w_wrapper.sv
class ddrInstHLS_aw_w_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val dir_data_f_DC_t_DI_in = Flipped(Decoupled(new HLS_COMMON_AXIS(176)))
		val ddr_aw = Decoupled(new HLS_COMMON_AXIS(72))
		val ddr_w = Decoupled(new HLS_COMMON_AXIS(584))
	}) 
} 


// From h2cDataDeliverHLS_wrapper.sv
class h2cDataDeliverHLS_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val h2c_data_f_QS_t_DD_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val h2c_data_f_DD_t_DMF_out = Decoupled(new HLS_COMMON_AXIS(528))
	}) 
} 


// From ddrInstHLS_ar_wrapper.sv
class ddrInstHLS_ar_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val dir_cmd_f_DC_t_DI_in = Flipped(Decoupled(new HLS_COMMON_AXIS(32)))
		val ddr_ar = Decoupled(new HLS_COMMON_AXIS(72))
	}) 
} 


// From c2hCmdDataDeliverHLS_wrapper.sv
class c2hCmdDataDeliverHLS_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val c2h_cmd_f_D_t_CDD_in = Flipped(Decoupled(new HLS_COMMON_AXIS(112)))
		val c2h_data_f_D_t_CDD_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val c2h_cmd_f_SBM_t_CDD_in = Flipped(Decoupled(new HLS_COMMON_AXIS(112)))
		val c2h_data_f_SBM_t_CDD_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val c2h_cmd_f_ABM_t_CDD_in = Flipped(Decoupled(new HLS_COMMON_AXIS(112)))
		val c2h_data_f_ABM_t_CDD_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val c2h_cmd_f_CDD_t_QS_out = Decoupled(new HLS_COMMON_AXIS(112))
		val c2h_data_f_CDD_t_QS_out = Decoupled(new HLS_COMMON_AXIS(528))
		val debug_ready_flag_idx = Output(UInt(32.W))
		val debug_end_flag = Output(UInt(1.W))
		val CDD_debug_count = Output(UInt(32.W))
		val CDD_debug_cmd = Output(UInt(107.W))
		val CDD_debug_data = Output(UInt(524.W))
	}) 
} 


// From c2hSysMultiBufferMgrHLS_wrapper.sv
class c2hSysMultiBufferMgrHLS_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val SBM_q_addr_arr = Input(UInt(512.W))
		val SBM_q_tidx_arr = Output(UInt(256.W))
		val SBM_q_hidx_arr = Input(UInt(256.W))
		val c2h_cmd_f_SBM_t_CDD_out = Decoupled(new HLS_COMMON_AXIS(112))
		val c2h_data_f_SBM_t_CDD_out = Decoupled(new HLS_COMMON_AXIS(528))
		val c2h_data_f_CMD_t_SBM_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val SBM_debug_count = Output(UInt(32.W))
	}) 
} 


// From ddrInstHLS_r_wrapper.sv
class ddrInstHLS_r_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val dir_data_f_DI_t_DC_out = Decoupled(new HLS_COMMON_AXIS(176))
		val ddr_r = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
	}) 
} 


// From c2hAppMultiBufferMgrHLS_wrapper.sv
class c2hAppMultiBufferMgrHLS_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val ABM_q_addr_arr = Input(UInt(1536.W))
		val ABM_q_tidx_arr = Output(UInt(768.W))
		val ABM_q_hidx_arr = Input(UInt(768.W))
		val c2h_cmd_f_ABM_t_CDD_out = Decoupled(new HLS_COMMON_AXIS(112))
		val c2h_data_f_ABM_t_CDD_out = Decoupled(new HLS_COMMON_AXIS(528))
		val c2h_data_f_CMD_t_ABM_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val ABM_debug_count = Output(UInt(32.W))
	}) 
} 


// From deparser_wrapper.sv
class deparser_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val net_rx = Flipped(Decoupled(new HLS_COMMON_AXIS(584)))
		val ctrl_msg_f_D_t_CC_out = Decoupled(new HLS_COMMON_AXIS(528))
		val ctrl_msg_f_D_t_DC_out = Decoupled(new HLS_COMMON_AXIS(528))
		val c2h_cmd_f_D_t_CDD_out = Decoupled(new HLS_COMMON_AXIS(112))
		val c2h_data_f_D_t_CDD_out = Decoupled(new HLS_COMMON_AXIS(528))
	}) 
} 


// From c2hCtrlMsgDeliverHLS_wrapper.sv
class c2hCtrlMsgDeliverHLS_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val q_num = Input(UInt(32.W))
		val ctrl_msg_f_CC_t_CMD_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val c2h_data_f_CMD_t_ABM_out = Decoupled(new HLS_COMMON_AXIS(528))
		val c2h_data_f_CMD_t_SBM_out = Decoupled(new HLS_COMMON_AXIS(528))
	}) 
} 


// From parser_wrapper.sv
class parser_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val net_tx = Decoupled(new HLS_COMMON_AXIS(584))
		val net_init_msg = Flipped(Decoupled(new HLS_COMMON_AXIS(56)))
		val ctrl_msg_f_DMF_t_P_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val data_msg_f_DMF_t_P_in = Flipped(Decoupled(new HLS_COMMON_AXIS(584)))
	}) 
} 


// From dirCtrlHLS_from_net_wrapper.sv
class dirCtrlHLS_from_net_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val ctrl_msg_f_D_t_DC_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val dir_cmd_f_DC_t_DI_out = Decoupled(new HLS_COMMON_AXIS(32))
		val ctrl_msg_r_f_DC_t_DC_out = Decoupled(new HLS_COMMON_AXIS(528))
		val dir_ack_f_DI_t_DC_in = Flipped(Decoupled(new HLS_COMMON_AXIS(8)))
		val ctrl_msg_w_f_DC_t_DC_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val debug_source_flag = Output(UInt(32.W))
		val debug_retry_flag = Output(UInt(32.W))
	}) 
} 


// From dataMsgFetch_wrapper.sv
class dataMsgFetch_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val h2c_cmd_f_DMF_t_CD_out = Decoupled(new HLS_COMMON_AXIS(112))
		val h2c_data_f_DD_t_DMF_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val ctrl_msg_f_CMC_t_DMF_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val ctrl_msg_f_DMF_t_P_out = Decoupled(new HLS_COMMON_AXIS(528))
		val data_msg_f_DMF_t_P_out = Decoupled(new HLS_COMMON_AXIS(584))
		val ctrl_msg_f_DMF_t_DMF_out = Decoupled(new HLS_COMMON_AXIS(528))
		val ctrl_msg_f_DMF_t_DMF_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val debug_state = Output(UInt(8.W))
	}) 
} 


// From dirCtrlHLS_to_net_wrapper.sv
class dirCtrlHLS_to_net_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val dir_data_init = Flipped(Decoupled(new HLS_COMMON_AXIS(176)))
		val ctrl_msg_f_DC_t_CMC_out = Decoupled(new HLS_COMMON_AXIS(528))
		val dir_data_f_DI_t_DC_in = Flipped(Decoupled(new HLS_COMMON_AXIS(176)))
		val ctrl_msg_r_f_DC_t_DC_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val dir_data_f_DC_t_DI_out = Decoupled(new HLS_COMMON_AXIS(176))
		val ctrl_msg_w_f_DC_t_DC_out = Decoupled(new HLS_COMMON_AXIS(528))
		val debug_ctrl_msg = Output(UInt(528.W))
		val debug_dir_data = Output(UInt(176.W))
		val debug_copy_flag = Output(UInt(8.W))
	}) 
} 


// From cacheCtrlHLS_from_net_wrapper.sv
class cacheCtrlHLS_from_net_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val ctrl_msg_f_D_t_CC_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val ctrl_msg_f_CC_t_CMD_out = Decoupled(new HLS_COMMON_AXIS(528))
		val ctrl_msg_f_CC_t_CC_out = Decoupled(new HLS_COMMON_AXIS(528))
	}) 
} 


// From ddrInstHLS_b_wrapper.sv
class ddrInstHLS_b_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val ddr_b = Flipped(Decoupled(new HLS_COMMON_AXIS(8)))
		val dir_ack_f_DI_t_DC_out = Decoupled(new HLS_COMMON_AXIS(8))
	}) 
} 


// From ctrlMsgCopy_wrapper.sv
class ctrlMsgCopy_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val ctrl_msg_f_CC_t_CMC_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val ctrl_msg_f_DC_t_CMC_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val ctrl_msg_f_CMC_t_DMF_out = Decoupled(new HLS_COMMON_AXIS(528))
	}) 
} 


// From h2cCtrlMsgDeliverHLS_wrapper.sv
class h2cCtrlMsgDeliverHLS_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val h2c_w_f_QS_t_CMD_in = Flipped(Decoupled(new HLS_COMMON_AXIS(584)))
		val ctrl_msg_f_CMD_t_CC_out = Decoupled(new HLS_COMMON_AXIS(528))
		val net_init_msg = Decoupled(new HLS_COMMON_AXIS(56))
		val dir_data_init = Decoupled(new HLS_COMMON_AXIS(176))
		val CMD_debug_count = Output(UInt(32.W))
		val CMD_debug_msg = Output(UInt(528.W))
	}) 
} 


// From h2cCmdDeliverHLS_wrapper.sv
class h2cCmdDeliverHLS_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val h2c_cmd_f_DMF_t_CD_in = Flipped(Decoupled(new HLS_COMMON_AXIS(112)))
		val h2c_cmd_f_CD_t_QS_out = Decoupled(new HLS_COMMON_AXIS(112))
	}) 
} 


// From cacheCtrlHLS_to_net_wrapper.sv
class cacheCtrlHLS_to_net_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val ctrl_msg_f_CMD_t_CC_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val ctrl_msg_f_CC_t_CC_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val ctrl_msg_f_CC_t_CMC_out = Decoupled(new HLS_COMMON_AXIS(528))
	}) 
} 



class hlsTop extends RawModule {
    val io = IO(new Bundle {
        val ap_clk = Input(Clock())
        val ap_rst_n = Input(Bool())
        val debug_ready_flag_idx = Output(UInt(32.W))
        val debug_end_flag = Output(UInt(1.W))
        val CDD_debug_count = Output(UInt(32.W))
        val CDD_debug_cmd = Output(UInt(107.W))
        val CDD_debug_data = Output(UInt(524.W))
        val SBM_q_addr_arr = Input(UInt(512.W))
        val SBM_q_tidx_arr = Output(UInt(256.W))
        val SBM_q_hidx_arr = Input(UInt(256.W))
        val SBM_debug_count = Output(UInt(32.W))
        val ABM_q_addr_arr = Input(UInt(1536.W))
        val ABM_q_tidx_arr = Output(UInt(768.W))
        val ABM_q_hidx_arr = Input(UInt(768.W))
        val ABM_debug_count = Output(UInt(32.W))
        val q_num = Input(UInt(32.W))
        val debug_source_flag = Output(UInt(32.W))
        val debug_retry_flag = Output(UInt(32.W))
        val debug_state = Output(UInt(8.W))
        val debug_ctrl_msg = Output(UInt(528.W))
        val debug_dir_data = Output(UInt(176.W))
        val debug_copy_flag = Output(UInt(8.W))
        val CMD_debug_count = Output(UInt(32.W))
        val CMD_debug_msg = Output(UInt(528.W))
        val ddr_aw = Decoupled(new HLS_COMMON_AXIS(72))
        val ddr_w = Decoupled(new HLS_COMMON_AXIS(584))
        val h2c_data_f_QS_t_DD_ = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
        val ddr_ar = Decoupled(new HLS_COMMON_AXIS(72))
        val c2h_cmd_f_CDD_t_QS_ = Decoupled(new HLS_COMMON_AXIS(112))
        val c2h_data_f_CDD_t_QS_ = Decoupled(new HLS_COMMON_AXIS(528))
        val ddr_r = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
        val net_rx = Flipped(Decoupled(new HLS_COMMON_AXIS(584)))
        val net_tx = Decoupled(new HLS_COMMON_AXIS(584))
        val ddr_b = Flipped(Decoupled(new HLS_COMMON_AXIS(8)))
        val h2c_w_f_QS_t_CMD_ = Flipped(Decoupled(new HLS_COMMON_AXIS(584)))
        val h2c_cmd_f_CD_t_QS_ = Decoupled(new HLS_COMMON_AXIS(112))
    })

    val dir_data_f_DC_t_DI_ = Wire(Decoupled(new HLS_COMMON_AXIS(176)))
    val h2c_data_f_DD_t_DMF_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val dir_cmd_f_DC_t_DI_ = Wire(Decoupled(new HLS_COMMON_AXIS(32)))
    val c2h_cmd_f_D_t_CDD_ = Wire(Decoupled(new HLS_COMMON_AXIS(112)))
    val c2h_data_f_D_t_CDD_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val c2h_cmd_f_SBM_t_CDD_ = Wire(Decoupled(new HLS_COMMON_AXIS(112)))
    val c2h_data_f_SBM_t_CDD_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val c2h_cmd_f_ABM_t_CDD_ = Wire(Decoupled(new HLS_COMMON_AXIS(112)))
    val c2h_data_f_ABM_t_CDD_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val c2h_data_f_CMD_t_SBM_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val dir_data_f_DI_t_DC_ = Wire(Decoupled(new HLS_COMMON_AXIS(176)))
    val c2h_data_f_CMD_t_ABM_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val ctrl_msg_f_D_t_CC_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val ctrl_msg_f_D_t_DC_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val ctrl_msg_f_CC_t_CMD_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val net_init_msg = Wire(Decoupled(new HLS_COMMON_AXIS(56)))
    val ctrl_msg_f_DMF_t_P_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val data_msg_f_DMF_t_P_ = Wire(Decoupled(new HLS_COMMON_AXIS(584)))
    val ctrl_msg_r_f_DC_t_DC_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val dir_ack_f_DI_t_DC_ = Wire(Decoupled(new HLS_COMMON_AXIS(8)))
    val ctrl_msg_w_f_DC_t_DC_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val h2c_cmd_f_DMF_t_CD_ = Wire(Decoupled(new HLS_COMMON_AXIS(112)))
    val ctrl_msg_f_CMC_t_DMF_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val ctrl_msg_f_DMF_t_DMF_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val dir_data_init = Wire(Decoupled(new HLS_COMMON_AXIS(176)))
    val ctrl_msg_f_DC_t_CMC_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val ctrl_msg_f_CC_t_CC_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val ctrl_msg_f_CC_t_CMC_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val ctrl_msg_f_CMD_t_CC_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))

    val ddrInstHLS_aw_w_wrapper = Module(new ddrInstHLS_aw_w_wrapper())
    ddrInstHLS_aw_w_wrapper.io.ap_clk := io.ap_clk
    ddrInstHLS_aw_w_wrapper.io.ap_rst_n := io.ap_rst_n
    ddrInstHLS_aw_w_wrapper.io.dir_data_f_DC_t_DI_in <> dir_data_f_DC_t_DI_
    ddrInstHLS_aw_w_wrapper.io.ddr_aw <> io.ddr_aw
    ddrInstHLS_aw_w_wrapper.io.ddr_w <> io.ddr_w

    val h2cDataDeliverHLS_wrapper = Module(new h2cDataDeliverHLS_wrapper())
    h2cDataDeliverHLS_wrapper.io.ap_clk := io.ap_clk
    h2cDataDeliverHLS_wrapper.io.ap_rst_n := io.ap_rst_n
    h2cDataDeliverHLS_wrapper.io.h2c_data_f_QS_t_DD_in <> io.h2c_data_f_QS_t_DD_
    h2cDataDeliverHLS_wrapper.io.h2c_data_f_DD_t_DMF_out <> h2c_data_f_DD_t_DMF_

    val ddrInstHLS_ar_wrapper = Module(new ddrInstHLS_ar_wrapper())
    ddrInstHLS_ar_wrapper.io.ap_clk := io.ap_clk
    ddrInstHLS_ar_wrapper.io.ap_rst_n := io.ap_rst_n
    ddrInstHLS_ar_wrapper.io.dir_cmd_f_DC_t_DI_in <> dir_cmd_f_DC_t_DI_
    ddrInstHLS_ar_wrapper.io.ddr_ar <> io.ddr_ar

    val c2hCmdDataDeliverHLS_wrapper = Module(new c2hCmdDataDeliverHLS_wrapper())
    c2hCmdDataDeliverHLS_wrapper.io.ap_clk := io.ap_clk
    c2hCmdDataDeliverHLS_wrapper.io.ap_rst_n := io.ap_rst_n
    c2hCmdDataDeliverHLS_wrapper.io.c2h_cmd_f_D_t_CDD_in <> c2h_cmd_f_D_t_CDD_
    c2hCmdDataDeliverHLS_wrapper.io.c2h_data_f_D_t_CDD_in <> c2h_data_f_D_t_CDD_
    c2hCmdDataDeliverHLS_wrapper.io.c2h_cmd_f_SBM_t_CDD_in <> c2h_cmd_f_SBM_t_CDD_
    c2hCmdDataDeliverHLS_wrapper.io.c2h_data_f_SBM_t_CDD_in <> c2h_data_f_SBM_t_CDD_
    c2hCmdDataDeliverHLS_wrapper.io.c2h_cmd_f_ABM_t_CDD_in <> c2h_cmd_f_ABM_t_CDD_
    c2hCmdDataDeliverHLS_wrapper.io.c2h_data_f_ABM_t_CDD_in <> c2h_data_f_ABM_t_CDD_
    c2hCmdDataDeliverHLS_wrapper.io.c2h_cmd_f_CDD_t_QS_out <> io.c2h_cmd_f_CDD_t_QS_
    c2hCmdDataDeliverHLS_wrapper.io.c2h_data_f_CDD_t_QS_out <> io.c2h_data_f_CDD_t_QS_
    io.debug_ready_flag_idx := c2hCmdDataDeliverHLS_wrapper.io.debug_ready_flag_idx
    io.debug_end_flag := c2hCmdDataDeliverHLS_wrapper.io.debug_end_flag
    io.CDD_debug_count := c2hCmdDataDeliverHLS_wrapper.io.CDD_debug_count
    io.CDD_debug_cmd := c2hCmdDataDeliverHLS_wrapper.io.CDD_debug_cmd
    io.CDD_debug_data := c2hCmdDataDeliverHLS_wrapper.io.CDD_debug_data

    val c2hSysMultiBufferMgrHLS_wrapper = Module(new c2hSysMultiBufferMgrHLS_wrapper())
    c2hSysMultiBufferMgrHLS_wrapper.io.ap_clk := io.ap_clk
    c2hSysMultiBufferMgrHLS_wrapper.io.ap_rst_n := io.ap_rst_n
    c2hSysMultiBufferMgrHLS_wrapper.io.SBM_q_addr_arr := io.SBM_q_addr_arr
    io.SBM_q_tidx_arr := c2hSysMultiBufferMgrHLS_wrapper.io.SBM_q_tidx_arr
    c2hSysMultiBufferMgrHLS_wrapper.io.SBM_q_hidx_arr := io.SBM_q_hidx_arr
    c2hSysMultiBufferMgrHLS_wrapper.io.c2h_cmd_f_SBM_t_CDD_out <> c2h_cmd_f_SBM_t_CDD_
    c2hSysMultiBufferMgrHLS_wrapper.io.c2h_data_f_SBM_t_CDD_out <> c2h_data_f_SBM_t_CDD_
    c2hSysMultiBufferMgrHLS_wrapper.io.c2h_data_f_CMD_t_SBM_in <> c2h_data_f_CMD_t_SBM_
    io.SBM_debug_count := c2hSysMultiBufferMgrHLS_wrapper.io.SBM_debug_count

    val ddrInstHLS_r_wrapper = Module(new ddrInstHLS_r_wrapper())
    ddrInstHLS_r_wrapper.io.ap_clk := io.ap_clk
    ddrInstHLS_r_wrapper.io.ap_rst_n := io.ap_rst_n
    ddrInstHLS_r_wrapper.io.dir_data_f_DI_t_DC_out <> dir_data_f_DI_t_DC_
    ddrInstHLS_r_wrapper.io.ddr_r <> io.ddr_r

    val c2hAppMultiBufferMgrHLS_wrapper = Module(new c2hAppMultiBufferMgrHLS_wrapper())
    c2hAppMultiBufferMgrHLS_wrapper.io.ap_clk := io.ap_clk
    c2hAppMultiBufferMgrHLS_wrapper.io.ap_rst_n := io.ap_rst_n
    c2hAppMultiBufferMgrHLS_wrapper.io.ABM_q_addr_arr := io.ABM_q_addr_arr
    io.ABM_q_tidx_arr := c2hAppMultiBufferMgrHLS_wrapper.io.ABM_q_tidx_arr
    c2hAppMultiBufferMgrHLS_wrapper.io.ABM_q_hidx_arr := io.ABM_q_hidx_arr
    c2hAppMultiBufferMgrHLS_wrapper.io.c2h_cmd_f_ABM_t_CDD_out <> c2h_cmd_f_ABM_t_CDD_
    c2hAppMultiBufferMgrHLS_wrapper.io.c2h_data_f_ABM_t_CDD_out <> c2h_data_f_ABM_t_CDD_
    c2hAppMultiBufferMgrHLS_wrapper.io.c2h_data_f_CMD_t_ABM_in <> c2h_data_f_CMD_t_ABM_
    io.ABM_debug_count := c2hAppMultiBufferMgrHLS_wrapper.io.ABM_debug_count

    val deparser_wrapper = Module(new deparser_wrapper())
    deparser_wrapper.io.ap_clk := io.ap_clk
    deparser_wrapper.io.ap_rst_n := io.ap_rst_n
    deparser_wrapper.io.net_rx <> io.net_rx
    deparser_wrapper.io.ctrl_msg_f_D_t_CC_out <> ctrl_msg_f_D_t_CC_
    deparser_wrapper.io.ctrl_msg_f_D_t_DC_out <> ctrl_msg_f_D_t_DC_
    deparser_wrapper.io.c2h_cmd_f_D_t_CDD_out <> c2h_cmd_f_D_t_CDD_
    deparser_wrapper.io.c2h_data_f_D_t_CDD_out <> c2h_data_f_D_t_CDD_

    val c2hCtrlMsgDeliverHLS_wrapper = Module(new c2hCtrlMsgDeliverHLS_wrapper())
    c2hCtrlMsgDeliverHLS_wrapper.io.ap_clk := io.ap_clk
    c2hCtrlMsgDeliverHLS_wrapper.io.ap_rst_n := io.ap_rst_n
    c2hCtrlMsgDeliverHLS_wrapper.io.q_num := io.q_num
    c2hCtrlMsgDeliverHLS_wrapper.io.ctrl_msg_f_CC_t_CMD_in <> ctrl_msg_f_CC_t_CMD_
    c2hCtrlMsgDeliverHLS_wrapper.io.c2h_data_f_CMD_t_ABM_out <> c2h_data_f_CMD_t_ABM_
    c2hCtrlMsgDeliverHLS_wrapper.io.c2h_data_f_CMD_t_SBM_out <> c2h_data_f_CMD_t_SBM_

    val parser_wrapper = Module(new parser_wrapper())
    parser_wrapper.io.ap_clk := io.ap_clk
    parser_wrapper.io.ap_rst_n := io.ap_rst_n
    parser_wrapper.io.net_tx <> io.net_tx
    parser_wrapper.io.net_init_msg <> net_init_msg
    parser_wrapper.io.ctrl_msg_f_DMF_t_P_in <> ctrl_msg_f_DMF_t_P_
    parser_wrapper.io.data_msg_f_DMF_t_P_in <> data_msg_f_DMF_t_P_

    val dirCtrlHLS_from_net_wrapper = Module(new dirCtrlHLS_from_net_wrapper())
    dirCtrlHLS_from_net_wrapper.io.ap_clk := io.ap_clk
    dirCtrlHLS_from_net_wrapper.io.ap_rst_n := io.ap_rst_n
    dirCtrlHLS_from_net_wrapper.io.ctrl_msg_f_D_t_DC_in <> ctrl_msg_f_D_t_DC_
    dirCtrlHLS_from_net_wrapper.io.dir_cmd_f_DC_t_DI_out <> dir_cmd_f_DC_t_DI_
    dirCtrlHLS_from_net_wrapper.io.ctrl_msg_r_f_DC_t_DC_out <> ctrl_msg_r_f_DC_t_DC_
    dirCtrlHLS_from_net_wrapper.io.dir_ack_f_DI_t_DC_in <> dir_ack_f_DI_t_DC_
    dirCtrlHLS_from_net_wrapper.io.ctrl_msg_w_f_DC_t_DC_in <> ctrl_msg_w_f_DC_t_DC_
    io.debug_source_flag := dirCtrlHLS_from_net_wrapper.io.debug_source_flag
    io.debug_retry_flag := dirCtrlHLS_from_net_wrapper.io.debug_retry_flag

    val dataMsgFetch_wrapper = Module(new dataMsgFetch_wrapper())
    dataMsgFetch_wrapper.io.ap_clk := io.ap_clk
    dataMsgFetch_wrapper.io.ap_rst_n := io.ap_rst_n
    dataMsgFetch_wrapper.io.h2c_cmd_f_DMF_t_CD_out <> h2c_cmd_f_DMF_t_CD_
    dataMsgFetch_wrapper.io.h2c_data_f_DD_t_DMF_in <> h2c_data_f_DD_t_DMF_
    dataMsgFetch_wrapper.io.ctrl_msg_f_CMC_t_DMF_in <> ctrl_msg_f_CMC_t_DMF_
    dataMsgFetch_wrapper.io.ctrl_msg_f_DMF_t_P_out <> ctrl_msg_f_DMF_t_P_
    dataMsgFetch_wrapper.io.data_msg_f_DMF_t_P_out <> data_msg_f_DMF_t_P_
    dataMsgFetch_wrapper.io.ctrl_msg_f_DMF_t_DMF_out <> ctrl_msg_f_DMF_t_DMF_
    dataMsgFetch_wrapper.io.ctrl_msg_f_DMF_t_DMF_in <> ctrl_msg_f_DMF_t_DMF_
    io.debug_state := dataMsgFetch_wrapper.io.debug_state

    val dirCtrlHLS_to_net_wrapper = Module(new dirCtrlHLS_to_net_wrapper())
    dirCtrlHLS_to_net_wrapper.io.ap_clk := io.ap_clk
    dirCtrlHLS_to_net_wrapper.io.ap_rst_n := io.ap_rst_n
    dirCtrlHLS_to_net_wrapper.io.dir_data_init <> dir_data_init
    dirCtrlHLS_to_net_wrapper.io.ctrl_msg_f_DC_t_CMC_out <> ctrl_msg_f_DC_t_CMC_
    dirCtrlHLS_to_net_wrapper.io.dir_data_f_DI_t_DC_in <> dir_data_f_DI_t_DC_
    dirCtrlHLS_to_net_wrapper.io.ctrl_msg_r_f_DC_t_DC_in <> ctrl_msg_r_f_DC_t_DC_
    dirCtrlHLS_to_net_wrapper.io.dir_data_f_DC_t_DI_out <> dir_data_f_DC_t_DI_
    dirCtrlHLS_to_net_wrapper.io.ctrl_msg_w_f_DC_t_DC_out <> ctrl_msg_w_f_DC_t_DC_
    io.debug_ctrl_msg := dirCtrlHLS_to_net_wrapper.io.debug_ctrl_msg
    io.debug_dir_data := dirCtrlHLS_to_net_wrapper.io.debug_dir_data
    io.debug_copy_flag := dirCtrlHLS_to_net_wrapper.io.debug_copy_flag

    val cacheCtrlHLS_from_net_wrapper = Module(new cacheCtrlHLS_from_net_wrapper())
    cacheCtrlHLS_from_net_wrapper.io.ap_clk := io.ap_clk
    cacheCtrlHLS_from_net_wrapper.io.ap_rst_n := io.ap_rst_n
    cacheCtrlHLS_from_net_wrapper.io.ctrl_msg_f_D_t_CC_in <> ctrl_msg_f_D_t_CC_
    cacheCtrlHLS_from_net_wrapper.io.ctrl_msg_f_CC_t_CMD_out <> ctrl_msg_f_CC_t_CMD_
    cacheCtrlHLS_from_net_wrapper.io.ctrl_msg_f_CC_t_CC_out <> ctrl_msg_f_CC_t_CC_

    val ddrInstHLS_b_wrapper = Module(new ddrInstHLS_b_wrapper())
    ddrInstHLS_b_wrapper.io.ap_clk := io.ap_clk
    ddrInstHLS_b_wrapper.io.ap_rst_n := io.ap_rst_n
    ddrInstHLS_b_wrapper.io.ddr_b <> io.ddr_b
    ddrInstHLS_b_wrapper.io.dir_ack_f_DI_t_DC_out <> dir_ack_f_DI_t_DC_

    val ctrlMsgCopy_wrapper = Module(new ctrlMsgCopy_wrapper())
    ctrlMsgCopy_wrapper.io.ap_clk := io.ap_clk
    ctrlMsgCopy_wrapper.io.ap_rst_n := io.ap_rst_n
    ctrlMsgCopy_wrapper.io.ctrl_msg_f_CC_t_CMC_in <> ctrl_msg_f_CC_t_CMC_
    ctrlMsgCopy_wrapper.io.ctrl_msg_f_DC_t_CMC_in <> ctrl_msg_f_DC_t_CMC_
    ctrlMsgCopy_wrapper.io.ctrl_msg_f_CMC_t_DMF_out <> ctrl_msg_f_CMC_t_DMF_

    val h2cCtrlMsgDeliverHLS_wrapper = Module(new h2cCtrlMsgDeliverHLS_wrapper())
    h2cCtrlMsgDeliverHLS_wrapper.io.ap_clk := io.ap_clk
    h2cCtrlMsgDeliverHLS_wrapper.io.ap_rst_n := io.ap_rst_n
    h2cCtrlMsgDeliverHLS_wrapper.io.h2c_w_f_QS_t_CMD_in <> io.h2c_w_f_QS_t_CMD_
    h2cCtrlMsgDeliverHLS_wrapper.io.ctrl_msg_f_CMD_t_CC_out <> ctrl_msg_f_CMD_t_CC_
    h2cCtrlMsgDeliverHLS_wrapper.io.net_init_msg <> net_init_msg
    h2cCtrlMsgDeliverHLS_wrapper.io.dir_data_init <> dir_data_init
    io.CMD_debug_count := h2cCtrlMsgDeliverHLS_wrapper.io.CMD_debug_count
    io.CMD_debug_msg := h2cCtrlMsgDeliverHLS_wrapper.io.CMD_debug_msg

    val h2cCmdDeliverHLS_wrapper = Module(new h2cCmdDeliverHLS_wrapper())
    h2cCmdDeliverHLS_wrapper.io.ap_clk := io.ap_clk
    h2cCmdDeliverHLS_wrapper.io.ap_rst_n := io.ap_rst_n
    h2cCmdDeliverHLS_wrapper.io.h2c_cmd_f_DMF_t_CD_in <> h2c_cmd_f_DMF_t_CD_
    h2cCmdDeliverHLS_wrapper.io.h2c_cmd_f_CD_t_QS_out <> io.h2c_cmd_f_CD_t_QS_

    val cacheCtrlHLS_to_net_wrapper = Module(new cacheCtrlHLS_to_net_wrapper())
    cacheCtrlHLS_to_net_wrapper.io.ap_clk := io.ap_clk
    cacheCtrlHLS_to_net_wrapper.io.ap_rst_n := io.ap_rst_n
    cacheCtrlHLS_to_net_wrapper.io.ctrl_msg_f_CMD_t_CC_in <> ctrl_msg_f_CMD_t_CC_
    cacheCtrlHLS_to_net_wrapper.io.ctrl_msg_f_CC_t_CC_in <> ctrl_msg_f_CC_t_CC_
    cacheCtrlHLS_to_net_wrapper.io.ctrl_msg_f_CC_t_CMC_out <> ctrl_msg_f_CC_t_CMC_

}
