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
		val dir_data_f_DC_t_DI_in = Flipped(Decoupled(new HLS_COMMON_AXIS(688)))
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
		val dir_data_f_DI_t_DC_out = Decoupled(new HLS_COMMON_AXIS(688))
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
		val dir_data_f_DI_t_DC_in = Flipped(Decoupled(new HLS_COMMON_AXIS(688)))
		val ctrl_msg_r_f_DC_t_DC_in = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val dir_data_f_DC_t_DI_out = Decoupled(new HLS_COMMON_AXIS(688))
		val ctrl_msg_w_f_DC_t_DC_out = Decoupled(new HLS_COMMON_AXIS(528))
		val debug_ctrl_msg = Output(UInt(528.W))
		val debug_dir_data = Output(UInt(688.W))
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
        val debug_dir_data = Output(UInt(688.W))
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

    val dir_data_f_DC_t_DI_ = Wire(Decoupled(new HLS_COMMON_AXIS(688)))
    val h2c_data_f_DD_t_DMF_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val dir_cmd_f_DC_t_DI_ = Wire(Decoupled(new HLS_COMMON_AXIS(32)))
    val c2h_cmd_f_D_t_CDD_ = Wire(Decoupled(new HLS_COMMON_AXIS(112)))
    val c2h_data_f_D_t_CDD_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val c2h_cmd_f_SBM_t_CDD_ = Wire(Decoupled(new HLS_COMMON_AXIS(112)))
    val c2h_data_f_SBM_t_CDD_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val c2h_cmd_f_ABM_t_CDD_ = Wire(Decoupled(new HLS_COMMON_AXIS(112)))
    val c2h_data_f_ABM_t_CDD_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val c2h_data_f_CMD_t_SBM_ = Wire(Decoupled(new HLS_COMMON_AXIS(528)))
    val dir_data_f_DI_t_DC_ = Wire(Decoupled(new HLS_COMMON_AXIS(688)))
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




class ChiselHLSTransformer extends Module{
	val io = IO(new Bundle {
		// val ap_clk	    = Input(Clock())
		// val ap_rst_n	    = Input(Bool())

		val pfch_tag	= Input(UInt(32.W))
		val tag_index	= Input(UInt(32.W))

		val hls_net_rx		= Decoupled(new HLS_COMMON_AXIS(584))
		val chisel_net_rx	= Flipped(Decoupled(new AXIS(512)))

		val hls_net_tx		= Flipped(Decoupled(new HLS_COMMON_AXIS(584)))
		val chisel_net_tx	= Decoupled(new AXIS(512))

		val hls_h2c_cmd	    = Flipped(Decoupled(new HLS_COMMON_AXIS(112)))
		val chisel_h2c_cmd	= Decoupled(new H2C_CMD)

		val hls_h2c_data	= Decoupled(new HLS_COMMON_AXIS(528))
		val chisel_h2c_data	= Flipped(Decoupled(new H2C_DATA))

		val hls_c2h_cmd	    = Flipped(Decoupled(new HLS_COMMON_AXIS(112)))
		val chisel_c2h_cmd	= Decoupled(new C2H_CMD)

		val hls_c2h_data	= Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val chisel_c2h_data	= Decoupled(new C2H_DATA)

		val hls_ddrPort_aw	= Flipped(Decoupled(new HLS_COMMON_AXIS(72)))
		val hls_ddrPort_w	= Flipped(Decoupled(new HLS_COMMON_AXIS(584)))
		val hls_ddrPort_b	= Decoupled(new HLS_COMMON_AXIS(8))
		val hls_ddrPort_ar	= Flipped(Decoupled(new HLS_COMMON_AXIS(72)))
		val hls_ddrPort_r	= Decoupled(new HLS_COMMON_AXIS(528))
		val chisel_ddrPort	= new DDRPort()

		val hls_h2c_w		= Decoupled(new HLS_COMMON_AXIS(584))
		val chisel_axib		= Flipped(new AXIB)
	})

	val MAX_Q = 32
	val tags = RegInit(VecInit(Seq.fill(MAX_Q)(0.U(7.W))))
	when(io.tag_index === (RegNext(io.tag_index)+1.U)){
		tags(RegNext(io.tag_index))	:= io.pfch_tag
	}

	val xfifo_ddrPort_aw		= XConverter(new HLS_COMMON_AXIS(72), clock, ~reset.asBool(), io.chisel_ddrPort.clk)
	val xfifo_ddrPort_w			= XConverter(new HLS_COMMON_AXIS(584), clock, ~reset.asBool(), io.chisel_ddrPort.clk)
	val xfifo_ddrPort_b			= XConverter(new HLS_COMMON_AXIS(8), io.chisel_ddrPort.clk, ~io.chisel_ddrPort.rst, clock)
	val xfifo_ddrPort_ar		= XConverter(new HLS_COMMON_AXIS(72), clock, ~reset.asBool(), io.chisel_ddrPort.clk)
	val xfifo_ddrPort_r			= XConverter(new HLS_COMMON_AXIS(528), io.chisel_ddrPort.clk, ~io.chisel_ddrPort.rst, clock)

	val fifo_ddrPort_aw		= XQueue(new HLS_COMMON_AXIS(72), 64)
	val fifo_ddrPort_w		= XQueue(new HLS_COMMON_AXIS(584), 64)
	val fifo_ddrPort_b		= XQueue(new HLS_COMMON_AXIS(8), 64)
	val fifo_ddrPort_ar		= XQueue(new HLS_COMMON_AXIS(72), 64)
	val fifo_ddrPort_r		= XQueue(new HLS_COMMON_AXIS(528), 64)

	fifo_ddrPort_aw.io.out	<> xfifo_ddrPort_aw.io.in
	fifo_ddrPort_w.io.out 	<> xfifo_ddrPort_w.io.in
	fifo_ddrPort_b.io.in	<> xfifo_ddrPort_b.io.out
	fifo_ddrPort_ar.io.out	<> xfifo_ddrPort_ar.io.in
	fifo_ddrPort_r.io.in	<> xfifo_ddrPort_r.io.out

	io.hls_ddrPort_aw	<> fifo_ddrPort_aw.io.in
	io.hls_ddrPort_w	<> fifo_ddrPort_w.io.in 
	io.hls_ddrPort_b	<> fifo_ddrPort_b.io.out
	io.hls_ddrPort_ar	<> fifo_ddrPort_ar.io.in
	io.hls_ddrPort_r	<> fifo_ddrPort_r.io.out

	io.chisel_ddrPort.axi.aw.valid	<> xfifo_ddrPort_aw.io.out.valid
	io.chisel_ddrPort.axi.aw.ready	<> xfifo_ddrPort_aw.io.out.ready
	io.chisel_ddrPort.axi.aw.bits.addr		<> xfifo_ddrPort_aw.io.out.bits.data(33,0)
	io.chisel_ddrPort.axi.aw.bits.burst		<> xfifo_ddrPort_aw.io.out.bits.data(35,34)
	io.chisel_ddrPort.axi.aw.bits.cache		<> xfifo_ddrPort_aw.io.out.bits.data(39,36)
	io.chisel_ddrPort.axi.aw.bits.id		<> xfifo_ddrPort_aw.io.out.bits.data(45,40)
	io.chisel_ddrPort.axi.aw.bits.len		<> xfifo_ddrPort_aw.io.out.bits.data(53,46)
	io.chisel_ddrPort.axi.aw.bits.lock		<> xfifo_ddrPort_aw.io.out.bits.data(55,54)
	io.chisel_ddrPort.axi.aw.bits.prot		<> xfifo_ddrPort_aw.io.out.bits.data(58,56)
	io.chisel_ddrPort.axi.aw.bits.qos		<> xfifo_ddrPort_aw.io.out.bits.data(62,59)
	io.chisel_ddrPort.axi.aw.bits.region	:= 0.U
	io.chisel_ddrPort.axi.aw.bits.size		<> xfifo_ddrPort_aw.io.out.bits.data(65,63)
	io.chisel_ddrPort.axi.aw.bits.user		:= 0.U

	io.chisel_ddrPort.axi.w.valid			<> xfifo_ddrPort_w.io.out.valid
	io.chisel_ddrPort.axi.w.ready			<> xfifo_ddrPort_w.io.out.ready
	io.chisel_ddrPort.axi.w.bits.data		<> xfifo_ddrPort_w.io.out.bits.data(511,0)
	io.chisel_ddrPort.axi.w.bits.last		<> xfifo_ddrPort_w.io.out.bits.data(512,512)
	io.chisel_ddrPort.axi.w.bits.strb		<> xfifo_ddrPort_w.io.out.bits.data(576,513)
	io.chisel_ddrPort.axi.w.bits.user		:= 0.U

	xfifo_ddrPort_b.io.in.valid				<> io.chisel_ddrPort.axi.b.valid
	xfifo_ddrPort_b.io.in.ready				<> io.chisel_ddrPort.axi.b.ready
	xfifo_ddrPort_b.io.in.bits.data			<> Cat(io.chisel_ddrPort.axi.b.bits.id,
													io.chisel_ddrPort.axi.b.bits.resp)

	io.chisel_ddrPort.axi.ar.valid  		<> xfifo_ddrPort_ar.io.out.valid
	io.chisel_ddrPort.axi.ar.ready 			<> xfifo_ddrPort_ar.io.out.ready
	io.chisel_ddrPort.axi.ar.bits.addr		<> xfifo_ddrPort_ar.io.out.bits.data(33,0)
	io.chisel_ddrPort.axi.ar.bits.burst		<> xfifo_ddrPort_ar.io.out.bits.data(35,34)
	io.chisel_ddrPort.axi.ar.bits.cache		<> xfifo_ddrPort_ar.io.out.bits.data(39,36)
	io.chisel_ddrPort.axi.ar.bits.id		<> xfifo_ddrPort_ar.io.out.bits.data(45,40)
	io.chisel_ddrPort.axi.ar.bits.len		<> xfifo_ddrPort_ar.io.out.bits.data(53,46)
	io.chisel_ddrPort.axi.ar.bits.lock		<> xfifo_ddrPort_ar.io.out.bits.data(55,54)
	io.chisel_ddrPort.axi.ar.bits.prot		<> xfifo_ddrPort_ar.io.out.bits.data(58,56)
	io.chisel_ddrPort.axi.ar.bits.qos		<> xfifo_ddrPort_ar.io.out.bits.data(62,59)
	io.chisel_ddrPort.axi.ar.bits.region	:= 0.U
	io.chisel_ddrPort.axi.ar.bits.size		<> xfifo_ddrPort_ar.io.out.bits.data(65,63)
	io.chisel_ddrPort.axi.ar.bits.user		:= 0.U

	xfifo_ddrPort_r.io.in.valid				<> io.chisel_ddrPort.axi.r.valid
	xfifo_ddrPort_r.io.in.ready				<> io.chisel_ddrPort.axi.r.ready
	xfifo_ddrPort_r.io.in.bits.data			<> Cat(io.chisel_ddrPort.axi.r.bits.id,
													io.chisel_ddrPort.axi.r.bits.resp,
													io.chisel_ddrPort.axi.r.bits.last,
													io.chisel_ddrPort.axi.r.bits.data)

	Collector.fire(fifo_ddrPort_aw.io.out)
	Collector.fire(fifo_ddrPort_w.io.out)
	Collector.fire(fifo_ddrPort_b.io.in)
	Collector.fire(fifo_ddrPort_ar.io.out)
	Collector.fire(fifo_ddrPort_r.io.in)	
	// class ila_ddr(seq:Seq[Data]) extends BaseILA(seq)
	// val ilaInst_ddr = Module(new ila_ddr(Seq(	
	// 	io.hls_ddrPort_aw.valid,
	// 	io.hls_ddrPort_aw.ready,
	// 	io.hls_ddrPort_aw.bits.data(33,0),
	// 	io.hls_ddrPort_aw.bits.data(53,46),

	// 	io.hls_ddrPort_ar.valid,
	// 	io.hls_ddrPort_ar.ready,
	// 	io.hls_ddrPort_ar.bits.data(33,0),
	// 	io.hls_ddrPort_ar.bits.data(53,46),

	// 	io.hls_ddrPort_w.valid,
	// 	io.hls_ddrPort_w.ready,
	// 	io.hls_ddrPort_w.bits.data(511,0),
	// 	io.hls_ddrPort_w.bits.data(512,512),

	// 	io.hls_ddrPort_r.valid,
	// 	io.hls_ddrPort_r.ready,
	// 	io.hls_ddrPort_r.bits.data(511,0),
	// 	io.hls_ddrPort_r.bits.data(512,512),

	// 	io.hls_ddrPort_b.valid,
	// 	io.hls_ddrPort_b.ready
	// )))
	// ilaInst_ddr.connect(clock)

	/************ N E T **************/
	val fifo_net_rx				= XQueue(new HLS_COMMON_AXIS(584), 64)
	fifo_net_rx.io.in.bits.data	<> Cat(io.chisel_net_rx.bits.last,
										io.chisel_net_rx.bits.keep,
										io.chisel_net_rx.bits.data)
	fifo_net_rx.io.in.valid		<> io.chisel_net_rx.valid
	fifo_net_rx.io.in.ready		<> io.chisel_net_rx.ready
	io.hls_net_rx				<> fifo_net_rx.io.out

	val fifo_net_tx				= XQueue(new HLS_COMMON_AXIS(584), 64)
	io.chisel_net_tx.bits.data	:= fifo_net_tx.io.out.bits.data(511, 0)
	io.chisel_net_tx.bits.keep	:= fifo_net_tx.io.out.bits.data(575, 512)
	io.chisel_net_tx.bits.last	:= fifo_net_tx.io.out.bits.data(576)
	io.chisel_net_tx.valid		<> fifo_net_tx.io.out.valid
	io.chisel_net_tx.ready		<> fifo_net_tx.io.out.ready
	fifo_net_tx.io.in			<> io.hls_net_tx

	Collector.fire(io.chisel_net_rx)
	Collector.fire(io.chisel_net_tx)
	// class ila_netrxtx(seq:Seq[Data]) extends BaseILA(seq)
	// val ilaInst_netrxtx = Module(new ila_netrxtx(Seq(	
	// 	io.chisel_net_rx.valid,
	// 	io.chisel_net_rx.ready,
	// 	io.chisel_net_rx.bits.data,
	// 	io.chisel_net_rx.bits.last,
	// 	io.chisel_net_tx.valid,
	// 	io.chisel_net_tx.ready,
	// 	io.chisel_net_tx.bits.data,
	// 	io.chisel_net_tx.bits.last
	// )))
	// ilaInst_netrxtx.connect(clock)

	/************ H 2 C - C M D **************/
	val fifo_h2c_cmd			= XQueue(new HLS_COMMON_AXIS(112), 64)
	ToZero(io.chisel_h2c_cmd.bits)
	io.chisel_h2c_cmd.bits.sop	:= (1.U===1.U)
	io.chisel_h2c_cmd.bits.eop	:= (1.U===1.U)
	io.chisel_h2c_cmd.bits.addr	:= fifo_h2c_cmd.io.out.bits.data(63, 0)
	io.chisel_h2c_cmd.bits.len	:= fifo_h2c_cmd.io.out.bits.data(95, 64)
	io.chisel_h2c_cmd.bits.qid	:= fifo_h2c_cmd.io.out.bits.data(106, 96)
	io.chisel_h2c_cmd.valid		<> fifo_h2c_cmd.io.out.valid
	io.chisel_h2c_cmd.ready		<> fifo_h2c_cmd.io.out.ready
	fifo_h2c_cmd.io.in			<> io.hls_h2c_cmd

	// Collector.fire(io.chisel_h2c_cmd)
	// class ila_h2ccmd(seq:Seq[Data]) extends BaseILA(seq)
	// val ilaInst_h2ccmd = Module(new ila_h2ccmd(Seq(	
	// 	io.chisel_h2c_cmd.valid,
	// 	io.chisel_h2c_cmd.ready,
	// 	io.chisel_h2c_cmd.bits.addr,
	// 	io.chisel_h2c_cmd.bits.len,
	// 	io.chisel_h2c_cmd.bits.qid
	// )))
	// ilaInst_h2ccmd.connect(clock)

	/************ H 2 C - D A T A **************/
	val fifo_h2c_data				= XQueue(new HLS_COMMON_AXIS(528), 64)
	fifo_h2c_data.io.in.bits.data	<> Cat(io.chisel_h2c_data.bits.last,
										io.chisel_h2c_data.bits.tuser_qid,
										io.chisel_h2c_data.bits.data)
	fifo_h2c_data.io.in.valid		<> io.chisel_h2c_data.valid
	fifo_h2c_data.io.in.ready		<> io.chisel_h2c_data.ready
	io.hls_h2c_data					<> fifo_h2c_data.io.out

	// Collector.fire(io.chisel_h2c_data)
	// class ila_h2cdata(seq:Seq[Data]) extends BaseILA(seq)
	// val ilaInst_h2cdata = Module(new ila_h2cdata(Seq(	
	// 	io.chisel_h2c_data.bits.last,
	// 	io.chisel_h2c_data.bits.tuser_qid,
	// 	io.chisel_h2c_data.bits.data,
	// 	io.chisel_h2c_data.valid,
	// 	io.chisel_h2c_data.ready
	// )))
	// ilaInst_h2cdata.connect(clock)

	/************ C 2 H - C M D **************/
	val fifo_c2h_cmd				= XQueue(new HLS_COMMON_AXIS(112), 64)
	ToZero(io.chisel_c2h_cmd.bits)
	io.chisel_c2h_cmd.bits.pfch_tag	:= tags(io.chisel_c2h_cmd.bits.qid)
	io.chisel_c2h_cmd.bits.addr		:= fifo_c2h_cmd.io.out.bits.data(63, 0)
	io.chisel_c2h_cmd.bits.len		:= fifo_c2h_cmd.io.out.bits.data(95, 64)
	io.chisel_c2h_cmd.bits.qid		:= fifo_c2h_cmd.io.out.bits.data(106, 96)
	io.chisel_c2h_cmd.valid			<> fifo_c2h_cmd.io.out.valid
	io.chisel_c2h_cmd.ready			<> fifo_c2h_cmd.io.out.ready
	fifo_c2h_cmd.io.in				<> io.hls_c2h_cmd
	
	Collector.fire(io.chisel_c2h_cmd)
	// class ila_c2hcmd(seq:Seq[Data]) extends BaseILA(seq)
	// val ilaInst_c2hcmd = Module(new ila_c2hcmd(Seq(	
	// 	// io.chisel_c2h_cmd.bits.pfch_tag,
	// 	io.chisel_c2h_cmd.valid,
	// 	io.chisel_c2h_cmd.ready,
	// 	io.chisel_c2h_cmd.bits.addr,
	// 	io.chisel_c2h_cmd.bits.len
	// 	// io.chisel_c2h_cmd.bits.qid
	// )))
	// ilaInst_c2hcmd.connect(clock)

	/************ C 2 H - D A T A **************/
	val fifo_c2h_data					= XQueue(new HLS_COMMON_AXIS(528), 64)
	ToZero(io.chisel_c2h_data.bits)
	io.chisel_c2h_data.bits.data		:= fifo_c2h_data.io.out.bits.data(511, 0)
	io.chisel_c2h_data.bits.ctrl_qid	:= fifo_c2h_data.io.out.bits.data(522, 512)
	io.chisel_c2h_data.bits.last		:= fifo_c2h_data.io.out.bits.data(523, 523)
	io.chisel_c2h_data.valid			<> fifo_c2h_data.io.out.valid
	io.chisel_c2h_data.ready			<> fifo_c2h_data.io.out.ready
	fifo_c2h_data.io.in					<> io.hls_c2h_data

	Collector.fire(io.chisel_c2h_data)
	// class ila_c2hdata(seq:Seq[Data]) extends BaseILA(seq)
	// val ilaInst_c2hdata = Module(new ila_c2hdata(Seq(	
	// 	io.chisel_c2h_data.bits.data,
	// 	// io.chisel_c2h_data.bits.ctrl_qid,
	// 	io.chisel_c2h_data.bits.last,
	// 	io.chisel_c2h_data.valid,
	// 	io.chisel_c2h_data.ready
	// )))
	// ilaInst_c2hdata.connect(clock)

	class ila_c2h(seq:Seq[Data]) extends BaseILA(seq)
	val ilaInst_c2h = Module(new ila_c2h(Seq(	
		// io.chisel_c2h_cmd.bits.pfch_tag,
		io.chisel_c2h_cmd.valid,
		io.chisel_c2h_cmd.ready,
		io.chisel_c2h_cmd.bits.addr,
		io.chisel_c2h_cmd.bits.len,
		// io.chisel_c2h_cmd.bits.qid
		io.chisel_c2h_data.bits.data,
		// io.chisel_c2h_data.bits.ctrl_qid,
		io.chisel_c2h_data.bits.last,
		io.chisel_c2h_data.valid,
		io.chisel_c2h_data.ready
	)))
	ilaInst_c2h.connect(clock)

	/************ A X I B **************/
	io.chisel_axib.ar.ready	:= 1.U
    io.chisel_axib.aw.ready	:= 1.U
    // io.chisel_axib.w.ready	:= 1.U 
    io.chisel_axib.r.valid	:= 1.U
    ToZero(io.chisel_axib.r.bits)
    io.chisel_axib.b.valid	:= 1.U
    ToZero(io.chisel_axib.b.bits)

	val fifo_h2c_w				= XQueue(new HLS_COMMON_AXIS(584), 64)
	io.hls_h2c_w				<> fifo_h2c_w.io.out
	fifo_h2c_w.io.in.bits.data	<> Cat(io.chisel_axib.w.bits.strb,
										io.chisel_axib.w.bits.last,
										io.chisel_axib.w.bits.data)
	fifo_h2c_w.io.in.valid		<> io.chisel_axib.w.valid
	fifo_h2c_w.io.in.ready		<> io.chisel_axib.w.ready

	Collector.fire(io.chisel_axib.w)
	class ila_h2c_w(seq:Seq[Data]) extends BaseILA(seq)
	val ilaInst_h2c_w = Module(new ila_h2c_w(Seq(	
		io.chisel_axib.w.bits.data,
		io.chisel_axib.w.bits.last,
		io.chisel_axib.w.valid,
		io.chisel_axib.w.ready
	)))
	ilaInst_h2c_w.connect(clock)
}

// Module name must be AlveoDynamicTop
class U280DynamicGreyBox extends MultiIOModule {

	// Configure the parameters.

	val USE_AXI_SLAVE_BRIDGE	= true		// Turn on this if you want to enable QDMA's Slave bridge.
	val ENABLE_CMAC_1			= true		// Turn on this if you want to enable CMAC at QSFP0
	val ENABLE_CMAC_2			= true		// Turn on this if you want to enable CMAC at QSFP1
	val ENABLE_DDR_1			= false		// Turn on this if you want to enable DDR at Channel 0
	val ENABLE_DDR_2			= true		// Turn on this if you want to enable DDR at Channel 1
	val ENABLE_HBM				= true		// Turn on this if you want to enable HBM

	assert((ENABLE_DDR_1 & ENABLE_HBM) != true, "DDR1 and HBM cannot be enabled at the same time.")

	override val desiredName = "AlveoDynamicTop"
    val io = IO(Flipped(new AlveoStaticIO(
        VIVADO_VERSION		 = "202101", 
		QDMA_PCIE_WIDTH 	= 16, 
		QDMA_SLAVE_BRIDGE 	= USE_AXI_SLAVE_BRIDGE, 
		QDMA_AXI_BRIDGE 	= true,
		ENABLE_CMAC_1 		= ENABLE_CMAC_1,
		ENABLE_CMAC_2 		= ENABLE_CMAC_2,
		ENABLE_DDR_1		= ENABLE_DDR_1,
		ENABLE_DDR_2		= ENABLE_DDR_2,
    )))

	// val dbgBridgeInst = DebugBridge(clk=clock)
	val dbgBridgeInst = DebugBridge(IP_CORE_NAME="DebugBridge", clk=clock)
	dbgBridgeInst.getTCL()

	// Since this is related to partial reonfiguration,
	// All ports, no matter used or not, should be precisely defined.
	dontTouch(io)

	val userClk  	= Wire(Clock())
	val userRstn 	= Wire(Bool())

	val qdmaInst = Module(new QDMADynamic(
		VIVADO_VERSION		= "202101",
		PCIE_WIDTH			= 16,
		TLB_TYPE			= new TLB,
		SLAVE_BRIDGE		= USE_AXI_SLAVE_BRIDGE,
		BRIDGE_BAR_SCALE	= "Megabytes",
		BRIDGE_BAR_SIZE 	= 4
	))

	qdmaInst.io.qdma_port	<> io.qdma
	qdmaInst.io.user_clk	:= userClk
	qdmaInst.io.user_arstn	:= userRstn

	// Connect QDMA's pins
	val controlReg  = qdmaInst.io.reg_control
	val statusReg   = qdmaInst.io.reg_status
	ToZero(statusReg)

	userClk		:= clock
	userRstn	:= ((~reset.asBool & ~controlReg(0)(0)).asClock).asBool

    // ToZero(qdmaInst.io.c2h_cmd.bits) // used by hls module
    // qdmaInst.io.c2h_cmd.valid   := 0.U 
    // ToZero(qdmaInst.io.c2h_data.bits)
    // qdmaInst.io.c2h_data.valid   := 0.U
    // ToZero(qdmaInst.io.h2c_cmd.bits)
    // qdmaInst.io.h2c_cmd.valid   := 0.U
    // qdmaInst.io.h2c_data.ready  := 1.U

    // In this case AXIB is not used. Just simply init it. // used by hls module
    // qdmaInst.io.axib.ar.ready	:= 1.U
    // qdmaInst.io.axib.aw.ready	:= 1.U
    // qdmaInst.io.axib.w.ready	:= 1.U 
    // qdmaInst.io.axib.r.valid	:= 1.U
    // ToZero(qdmaInst.io.axib.r.bits)
    // qdmaInst.io.axib.b.valid	:= 1.U
    // ToZero(qdmaInst.io.axib.b.bits)
	if (USE_AXI_SLAVE_BRIDGE) {
		ToZero(qdmaInst.io.s_axib.get.aw.bits)
		qdmaInst.io.s_axib.get.aw.valid   := 0.U
		ToZero(qdmaInst.io.s_axib.get.w.bits)
		qdmaInst.io.s_axib.get.w.valid   := 0.U
		ToZero(qdmaInst.io.s_axib.get.ar.bits)
		qdmaInst.io.s_axib.get.ar.valid   := 0.U
		qdmaInst.io.s_axib.get.b.ready	:= 1.U
		qdmaInst.io.s_axib.get.r.ready	:= 1.U
	}

	if (ENABLE_CMAC_1) {
		val cmacInst = Module(new XCMAC(BOARD="u280", PORT=0, IP_CORE_NAME="CMACBlackBoxBase"))
		cmacInst.getTCL()

		// Connect CMAC's pins
		cmacInst.io.pin			<> io.cmacPin.get
		cmacInst.io.drp_clk		:= io.cmacClk.get
		cmacInst.io.user_clk	:= userClk
		cmacInst.io.user_arstn	:= userRstn
		cmacInst.io.sys_reset	:= reset

		cmacInst.io.m_net_rx.ready  := 1.U
		ToZero(cmacInst.io.s_net_tx.bits)
		cmacInst.io.s_net_tx.valid  := 0.U
	}

	// if (ENABLE_CMAC_2) {
		val cmacInst2 = Module(new XCMAC(BOARD="u280", PORT=1, IP_CORE_NAME="CMACBlackBoxBase2"))
		cmacInst2.getTCL()

		// Connect CMAC's pins
		cmacInst2.io.pin			<> io.cmacPin2.get
		cmacInst2.io.drp_clk		:= io.cmacClk.get
		cmacInst2.io.user_clk		:= userClk
		cmacInst2.io.user_arstn		:= userRstn
		cmacInst2.io.sys_reset		:= reset

	// 	cmacInst2.io.m_net_rx.ready  := 1.U  // used by hls module
	// 	ToZero(cmacInst2.io.s_net_tx.bits)
	// 	cmacInst2.io.s_net_tx.valid  := 0.U
	// }

	if (ENABLE_DDR_1) {
		// Connect DDR's ports
		io.ddrPort.get.axi.qdma_init()
	}

	// if (ENABLE_DDR_2) {
		// Connect DDR's ports
		// io.ddrPort2.get.axi.qdma_init() // used by hls module
	// }

	if (ENABLE_HBM) {
		val hbmDriver = withClockAndReset(io.sysClk, false.B) {Module(new HBM_DRIVER(WITH_RAMA=false, IP_CORE_NAME="HBMBlackBoxBase"))}
		hbmDriver.getTCL()

		for (i <- 0 until 32) {
			hbmDriver.io.axi_hbm(i).hbm_init()	// Read hbm_init function if you're not familiar with AXI.
		}
	}

    /******************************/
    /*** Chisel-HLS Transformer ***/
    val transInst 	= Module(new ChiselHLSTransformer())
	transInst.io.pfch_tag	:= controlReg(98)
	transInst.io.tag_index	:= controlReg(99)
    transInst.io.chisel_net_rx		<> cmacInst2.io.m_net_rx
    transInst.io.chisel_net_tx		<> cmacInst2.io.s_net_tx
    transInst.io.chisel_h2c_cmd		<> qdmaInst.io.h2c_cmd
    transInst.io.chisel_h2c_data	<> qdmaInst.io.h2c_data
    transInst.io.chisel_c2h_cmd		<> qdmaInst.io.c2h_cmd
    transInst.io.chisel_c2h_data	<> qdmaInst.io.c2h_data

	transInst.io.chisel_ddrPort		<> io.ddrPort2.get
	transInst.io.chisel_axib		<> qdmaInst.io.axib

    /******************************/
    /******************************/

    /******************************/
    /************* HLS ************/
	// TODO: link to chisel interface
    val hlsInst 	= Module(new hlsTop())
    hlsInst.io.ddr_aw   <>  transInst.io.hls_ddrPort_aw
    hlsInst.io.ddr_w    <>  transInst.io.hls_ddrPort_w
    hlsInst.io.ddr_ar   <>  transInst.io.hls_ddrPort_ar
    hlsInst.io.ddr_r    <>  transInst.io.hls_ddrPort_r
    hlsInst.io.ddr_b    <>  transInst.io.hls_ddrPort_b

    hlsInst.io.net_rx   <>  transInst.io.hls_net_rx
    hlsInst.io.net_tx   <>  transInst.io.hls_net_tx

    hlsInst.io.c2h_cmd_f_CDD_t_QS_      <>  transInst.io.hls_c2h_cmd
    hlsInst.io.c2h_data_f_CDD_t_QS_     <>  transInst.io.hls_c2h_data
    hlsInst.io.h2c_data_f_QS_t_DD_      <>  transInst.io.hls_h2c_data
    hlsInst.io.h2c_cmd_f_CD_t_QS_       <>  transInst.io.hls_h2c_cmd
    hlsInst.io.h2c_w_f_QS_t_CMD_        <>  transInst.io.hls_h2c_w

    hlsInst.io.ap_clk   :=  userClk
    hlsInst.io.ap_rst_n :=  userRstn

    hlsInst.io.q_num                    :=  controlReg(127) //Input(UInt(32.W))  

    // Define Vector type signals to store concatenated 32-bit parts
    val ABM_q_addr_arr_all = Wire(Vec(24, UInt(64.W)))  // 24 64-bit signals
    val ABM_q_hidx_arr_all = Wire(Vec(24, UInt(32.W)))  // 24 32-bit signals
    val SBM_q_addr_arr_all = Wire(Vec(8, UInt(64.W)))   // 8 64-bit signals
    val SBM_q_hidx_arr_all = Wire(Vec(8, UInt(32.W)))   // 8 32-bit signals
    
    // Assign values to each item in Vector within for loop
    for (i <- 0 until 24) { 
        // Assign 32-bit signals from controlReg to corresponding positions in Vector
        ABM_q_addr_arr_all(i) := Cat(controlReg(128 + i * 8 + 5), controlReg(128 + i * 8 + 4))  // Concatenate 64-bit address
        ABM_q_hidx_arr_all(i) := controlReg(128 + i * 8 + 6)  // 32-bit index
        statusReg(128 + i * 8 + 7) := hlsInst.io.ABM_q_tidx_arr(32 * (i + 1) - 1, 32 * i) //Output(UInt(768.W))
    }
    
    for (i <- 0 until 8) { 
        // Assign 32-bit signals from controlReg to corresponding positions in Vector
        SBM_q_addr_arr_all(i) := Cat(controlReg(128 + (24 + i) * 8 + 5), controlReg(128 + (24 + i) * 8 + 4))  // Concatenate 64-bit address
        SBM_q_hidx_arr_all(i) := controlReg(128 + (24 + i) * 8 + 6)  // 32-bit index
        statusReg(128 + (24 + i) * 8 + 7) := hlsInst.io.SBM_q_tidx_arr(32 * (i + 1) - 1, 32 * i) //Output(UInt(768.W))
    }
    
    // Finally convert the entire Vector to UInt type and assign to target signals
    hlsInst.io.ABM_q_addr_arr := ABM_q_addr_arr_all.asUInt  // Concatenated result
    hlsInst.io.ABM_q_hidx_arr := ABM_q_hidx_arr_all.asUInt  // Concatenated result
    hlsInst.io.SBM_q_addr_arr := SBM_q_addr_arr_all.asUInt  // Concatenated result
    hlsInst.io.SBM_q_hidx_arr := SBM_q_hidx_arr_all.asUInt  // Concatenated result

	class ila_c2h_reginfo(seq:Seq[Data]) extends BaseILA(seq)
	val ilaInst_c2h_reginfo = Module(new ila_c2h_reginfo(Seq(	
		hlsInst.io.ABM_q_addr_arr(63, 0),
		hlsInst.io.ABM_q_hidx_arr(31, 0),
		hlsInst.io.SBM_q_addr_arr(63, 0),
		hlsInst.io.SBM_q_hidx_arr(31, 0),
		hlsInst.io.SBM_debug_count,
		hlsInst.io.ABM_debug_count,
		hlsInst.io.CDD_debug_count,
		hlsInst.io.CDD_debug_cmd,
		hlsInst.io.CDD_debug_data
	)))
	ilaInst_c2h_reginfo.connect(userClk)

	class ila_h2c_reginfo(seq:Seq[Data]) extends BaseILA(seq)
	val ilaInst_h2c_reginfo = Module(new ila_h2c_reginfo(Seq(	
		hlsInst.io.CMD_debug_count,
		hlsInst.io.CMD_debug_msg
	)))
	ilaInst_h2c_reginfo.connect(userClk)

	Collector.connect_to_status_reg(statusReg, 400) // Must be after all fire and report
}	