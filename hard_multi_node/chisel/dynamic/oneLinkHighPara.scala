// U280DynamicGrayBox.scala

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


class h2cCmdDeliverHLS_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val h2c_cmd_from_DMF = Flipped(Decoupled(new HLS_COMMON_AXIS(112)))
		val h2c_cmd_to_qdma = Decoupled(new HLS_COMMON_AXIS(112))
	}) 
} 
class h2cDataDeliverHLS_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val h2c_data_from_qdma = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val h2c_data_to_DMF = Decoupled(new HLS_COMMON_AXIS(528))
	}) 
} 
class h2cBufferMgrHLS_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val q_id = Input(UInt(32.W))
		val q_addr = Input(UInt(64.W))
		val q_free_start_idx = Input(UInt(32.W))
		val q_occu_start_idx = Output(UInt(32.W))
		val h2c_cmd_to_CD = Decoupled(new HLS_COMMON_AXIS(112))
		val h2c_data_from_DD = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val h2c_data_to_IMD_or_CMD = Decoupled(new HLS_COMMON_AXIS(528))
	}) 
} 
class h2cInitMsgDeliverHLS_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val h2c_data_from_BMI = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val net_init_msg = Decoupled(new HLS_COMMON_AXIS(64))
		val dir_init_msg = Decoupled(new HLS_COMMON_AXIS(112))
	}) 
} 
class h2cCtrlMsgDeliverHLS_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val h2c_w_from_qdma = Flipped(Decoupled(new HLS_COMMON_AXIS(584)))
		val ctrl_msg_to_CC = Decoupled(new HLS_COMMON_AXIS(512))
		val net_init_msg = Decoupled(new HLS_COMMON_AXIS(64))
		val dir_init_msg = Decoupled(new HLS_COMMON_AXIS(112))
	}) 
} 
class cacheCtrlHLS_from_net_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val ctrl_msg_from_D = Flipped(Decoupled(new HLS_COMMON_AXIS(504)))
		val ctrl_msg_to_CMD = Decoupled(new HLS_COMMON_AXIS(504))
		val debug_request_from_home_count = Output(UInt(32.W))
		val debug_cache_from_home_count = Output(UInt(32.W))
	}) 
} 
class cacheCtrlHLS_to_net_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val ctrl_msg_from_CMD = Flipped(Decoupled(new HLS_COMMON_AXIS(504)))
		val ctrl_msg_to_CMC = Decoupled(new HLS_COMMON_AXIS(504))
		val debug_request_to_home_count = Output(UInt(32.W))
		val debug_cache_to_home_count = Output(UInt(32.W))
	}) 
} 
class ctrlMsgCopy_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val ctrl_msg_from_CC = Flipped(Decoupled(new HLS_COMMON_AXIS(504)))
		val ctrl_msg_from_DC = Flipped(Decoupled(new HLS_COMMON_AXIS(504)))
		val ctrl_msg_to_fifoC = Decoupled(new HLS_COMMON_AXIS(504))
		val ctrl_msg_from_fifoC = Flipped(Decoupled(new HLS_COMMON_AXIS(504)))
		val ctrl_msg_to_DMF = Decoupled(new HLS_COMMON_AXIS(504))
	}) 
} 
class dataMsgFetch_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val h2c_cmd_to_CD = Decoupled(new HLS_COMMON_AXIS(112))
		val h2c_data_from_DD = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val ctrl_msg_from_CMC = Flipped(Decoupled(new HLS_COMMON_AXIS(504)))
		val ctrl_msg_to_P = Decoupled(new HLS_COMMON_AXIS(504))
		val data_msg_to_P = Decoupled(new HLS_COMMON_AXIS(584))
		val ctrl_msg_to_fifoM = Decoupled(new HLS_COMMON_AXIS(504))
		val ctrl_msg_from_fifoM = Flipped(Decoupled(new HLS_COMMON_AXIS(504)))
	}) 
} 
class dirCtrlHLS_from_net_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val ctrl_msg_from_D = Flipped(Decoupled(new HLS_COMMON_AXIS(504)))
		val dir_cmd_to_DI = Decoupled(new HLS_COMMON_AXIS(40))
		val ctrl_msg_to_fifoD = Decoupled(new HLS_COMMON_AXIS(504))
		val debug_home_from_request_count = Output(UInt(32.W))
		val debug_home_from_cache_count = Output(UInt(32.W))
	}) 
} 
class dirCtrlHLS_to_net_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val ctrl_msg_to_CMC = Decoupled(new HLS_COMMON_AXIS(504))
		val dir_data_from_DI = Flipped(Decoupled(new HLS_COMMON_AXIS(112)))
		val ctrl_msg_from_fifoD = Flipped(Decoupled(new HLS_COMMON_AXIS(504)))
		val debug_home_to_request_count = Output(UInt(32.W))
		val debug_home_to_cache_count = Output(UInt(32.W))
	}) 
} 
class ddrInstHLS_aw_w_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val dir_init_msg = Flipped(Decoupled(new HLS_COMMON_AXIS(112)))
		val ddr_aw = Decoupled(new HLS_COMMON_AXIS(72))
		val ddr_w = Decoupled(new HLS_COMMON_AXIS(584))
	}) 
} 
class ddrInstHLS_b_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val ddr_b = Flipped(Decoupled(new HLS_COMMON_AXIS(8)))
	}) 
} 
class ddrInstHLS_ar_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val dir_cmd_from_DC = Flipped(Decoupled(new HLS_COMMON_AXIS(40)))
		val ddr_ar = Decoupled(new HLS_COMMON_AXIS(72))
	}) 
} 
class ddrInstHLS_r_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val dir_data_to_DC = Decoupled(new HLS_COMMON_AXIS(112))
		val ddr_r = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
	}) 
} 
class c2hCtrlMsgDeliverHLS_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val ctrl_msg_from_CC = Flipped(Decoupled(new HLS_COMMON_AXIS(504)))
		val c2h_data_to_BM = Decoupled(new HLS_COMMON_AXIS(528))
		val c2h_cmd_to_CDD = Decoupled(new HLS_COMMON_AXIS(112))
		val c2h_data_to_CDD = Decoupled(new HLS_COMMON_AXIS(528))
	}) 
} 
class c2hMultiBufferMgrHLS_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val q_num = Input(UInt(32.W))
		val q_addr_arr = Input(UInt(512.W))
		val q_tidx_arr = Output(UInt(256.W))
		val q_hidx_arr = Input(UInt(256.W))
		val c2h_cmd_to_CDD = Decoupled(new HLS_COMMON_AXIS(112))
		val c2h_data_to_CDD = Decoupled(new HLS_COMMON_AXIS(528))
		val c2h_data_from_CMD = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
	}) 
} 
class c2hCmdDataDeliverHLS_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val c2h_cmd_from_D = Flipped(Decoupled(new HLS_COMMON_AXIS(112)))
		val c2h_data_from_D = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val c2h_cmd_from_BM = Flipped(Decoupled(new HLS_COMMON_AXIS(112)))
		val c2h_data_from_BM = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val c2h_cmd_from_CMD = Flipped(Decoupled(new HLS_COMMON_AXIS(112)))
		val c2h_data_from_CMD = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val c2h_cmd_to_qdma = Decoupled(new HLS_COMMON_AXIS(112))
		val c2h_data_to_qdma = Decoupled(new HLS_COMMON_AXIS(528))
	}) 
} 
class ddrSim_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val ddr_aw = Flipped(Decoupled(new HLS_COMMON_AXIS(72)))
		val ddr_w = Flipped(Decoupled(new HLS_COMMON_AXIS(584)))
		val ddr_ar = Flipped(Decoupled(new HLS_COMMON_AXIS(72)))
		val ddr_r = Decoupled(new HLS_COMMON_AXIS(528))
		val ddr_b = Decoupled(new HLS_COMMON_AXIS(8))
	}) 
} 
class qdmaSim_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val h2c_w_to_CMD = Decoupled(new HLS_COMMON_AXIS(584))
		val h2c_cmd_from_CD = Flipped(Decoupled(new HLS_COMMON_AXIS(112)))
		val h2c_data_to_DD = Decoupled(new HLS_COMMON_AXIS(528))
		val c2h_cmd_from_CDD = Flipped(Decoupled(new HLS_COMMON_AXIS(112)))
		val c2h_data_from_CDD = Flipped(Decoupled(new HLS_COMMON_AXIS(528)))
		val c2h_q_hidx = Output(UInt(32.W))
		val net_init_flag = Input(UInt(1.W))
		val dir_init_flag = Input(UInt(1.W))
		val request_flag = Input(UInt(1.W))
		val iter = Input(UInt(32.W))
	}) 
} 
class parser_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val m_net_tx = Decoupled(new HLS_COMMON_AXIS(584))
		val net_init_msg = Flipped(Decoupled(new HLS_COMMON_AXIS(64)))
		val ctrl_msg_from_DMF = Flipped(Decoupled(new HLS_COMMON_AXIS(504)))
		val data_msg_from_DMF = Flipped(Decoupled(new HLS_COMMON_AXIS(584)))
	}) 
} 
class deparser_wrapper extends BlackBox {
	val io = IO(new Bundle {
		val ap_clk = Input(Clock())
		val ap_rst_n = Input(Bool())
		val s_net_rx = Flipped(Decoupled(new HLS_COMMON_AXIS(584)))
		val ctrl_msg_to_CC = Decoupled(new HLS_COMMON_AXIS(504))
		val ctrl_msg_to_DC = Decoupled(new HLS_COMMON_AXIS(504))
		val c2h_cmd_to_CDD = Decoupled(new HLS_COMMON_AXIS(112))
		val c2h_data_to_CDD = Decoupled(new HLS_COMMON_AXIS(528))
	}) 
} 

//-------------------------------------------------

class HLS_COMMON_AXIS(DATA_WIDTH:Int)extends Bundle{
	val data		= UInt(DATA_WIDTH.W)
}

class XQueue2[T <: Data](gen: T, entries: Int) extends Module {
  val io = IO(new Bundle {
    val in = Flipped(Decoupled(gen))
    val out = Decoupled(gen)
  })
  val fifo_1 = XQueue(gen, entries)
  val fifo_2 = XQueue(gen, entries)

  fifo_1.io.in 	<> io.in
  fifo_2.io.in 	<> fifo_1.io.out
  io.out 		<> fifo_2.io.out
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

	// with fifo
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

	// class ila_netrxtx(seq:Seq[Data]) extends BaseILA(seq)
	// val ilaInst_netrxtx = Module(new ila_netrxtx(Seq(	
	// 	io.chisel_net_rx.bits.last,
	// 	io.chisel_net_rx.bits.keep,
	// 	io.chisel_net_rx.valid,
	// 	io.chisel_net_rx.ready,
	// 	io.chisel_net_rx.bits.data,

	// 	io.hls_net_rx.valid,
	// 	io.hls_net_rx.ready,
	// 	io.hls_net_rx.bits.data,

	// 	io.chisel_net_tx.bits.last,
	// 	io.chisel_net_tx.bits.keep,
	// 	io.chisel_net_tx.valid,
	// 	io.chisel_net_tx.ready,
	// 	io.chisel_net_tx.bits.data,

	// 	io.hls_net_tx.valid,
	// 	io.hls_net_tx.ready,
	// 	io.hls_net_tx.bits.data
	// )))
	// ilaInst_netrxtx.connect(clock)

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

	// class ila_h2ccmd(seq:Seq[Data]) extends BaseILA(seq)
	// val ilaInst_h2ccmd = Module(new ila_h2ccmd(Seq(	
	// 	io.chisel_h2c_cmd.bits.sop,
	// 	io.chisel_h2c_cmd.bits.eop,
	// 	io.chisel_h2c_cmd.valid,
	// 	io.chisel_h2c_cmd.ready,
	// 	io.chisel_h2c_cmd.bits.addr,
	// 	io.chisel_h2c_cmd.bits.len,
	// 	io.chisel_h2c_cmd.bits.qid,

	// 	io.hls_h2c_cmd.valid,
	// 	io.hls_h2c_cmd.ready,
	// 	io.hls_h2c_cmd.bits.data
	// )))
	// ilaInst_h2ccmd.connect(clock)

	val fifo_h2c_data				= XQueue(new HLS_COMMON_AXIS(528), 64)
	fifo_h2c_data.io.in.bits.data	<> Cat(io.chisel_h2c_data.bits.last,
										io.chisel_h2c_data.bits.tuser_qid,
										io.chisel_h2c_data.bits.data)
	fifo_h2c_data.io.in.valid		<> io.chisel_h2c_data.valid
	fifo_h2c_data.io.in.ready		<> io.chisel_h2c_data.ready
	io.hls_h2c_data					<> fifo_h2c_data.io.out

	// class ila_h2cdata(seq:Seq[Data]) extends BaseILA(seq)
	// val ilaInst_h2cdata = Module(new ila_h2cdata(Seq(	
	// 	io.chisel_h2c_data.bits.last,
	// 	io.chisel_h2c_data.bits.tuser_qid,
	// 	io.chisel_h2c_data.bits.data,
	// 	io.chisel_h2c_data.valid,
	// 	io.chisel_h2c_data.ready,

	// 	io.hls_h2c_data.valid,
	// 	io.hls_h2c_data.ready,
	// 	io.hls_h2c_data.bits.data
	// )))
	// ilaInst_h2cdata.connect(clock)

	val fifo_c2h_cmd				= XQueue(new HLS_COMMON_AXIS(112), 64)
	ToZero(io.chisel_c2h_cmd.bits)
	io.chisel_c2h_cmd.bits.pfch_tag	:= tags(io.chisel_c2h_cmd.bits.qid)
	io.chisel_c2h_cmd.bits.addr		:= fifo_c2h_cmd.io.out.bits.data(63, 0)
	io.chisel_c2h_cmd.bits.len		:= fifo_c2h_cmd.io.out.bits.data(95, 64)
	io.chisel_c2h_cmd.bits.qid		:= fifo_c2h_cmd.io.out.bits.data(106, 96)
	io.chisel_c2h_cmd.valid			<> fifo_c2h_cmd.io.out.valid
	io.chisel_c2h_cmd.ready			<> fifo_c2h_cmd.io.out.ready
	fifo_c2h_cmd.io.in				<> io.hls_c2h_cmd
	
	// class ila_c2hcmd(seq:Seq[Data]) extends BaseILA(seq)
	// val ilaInst_c2hcmd = Module(new ila_c2hcmd(Seq(	
	// 	io.chisel_c2h_cmd.bits.pfch_tag,
	// 	io.chisel_c2h_cmd.valid,
	// 	io.chisel_c2h_cmd.ready,
	// 	io.chisel_c2h_cmd.bits.addr,
	// 	io.chisel_c2h_cmd.bits.len,
	// 	io.chisel_c2h_cmd.bits.qid,

	// 	io.hls_c2h_cmd.valid,
	// 	io.hls_c2h_cmd.ready,
	// 	io.hls_c2h_cmd.bits.data
	// )))
	// ilaInst_c2hcmd.connect(clock)

	val fifo_c2h_data					= XQueue(new HLS_COMMON_AXIS(528), 64)
	ToZero(io.chisel_c2h_data.bits)
	io.chisel_c2h_data.bits.data		:= fifo_c2h_data.io.out.bits.data(511, 0)
	io.chisel_c2h_data.bits.ctrl_qid	:= fifo_c2h_data.io.out.bits.data(522, 512)
	io.chisel_c2h_data.bits.last		:= fifo_c2h_data.io.out.bits.data(523, 523)
	io.chisel_c2h_data.valid			<> fifo_c2h_data.io.out.valid
	io.chisel_c2h_data.ready			<> fifo_c2h_data.io.out.ready
	fifo_c2h_data.io.in					<> io.hls_c2h_data

	// class ila_c2hdata(seq:Seq[Data]) extends BaseILA(seq)
	// val ilaInst_c2hdata = Module(new ila_c2hdata(Seq(	
	// 	io.chisel_c2h_data.bits.data,
	// 	io.chisel_c2h_data.bits.ctrl_qid,
	// 	io.chisel_c2h_data.bits.last,
	// 	io.chisel_c2h_data.valid,
	// 	io.chisel_c2h_data.ready,

	// 	io.hls_c2h_data.valid,
	// 	io.hls_c2h_data.ready,
	// 	io.hls_c2h_data.bits.data
	// )))
	// ilaInst_c2hdata.connect(clock)


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
	val dbgBridgeInst = DebugBridge(IP_CORE_NAME="DebugBridgeBase", clk=clock)
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

    Collector.connect_to_status_reg(statusReg, 400)

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
	val h2c_cmd_from_DMF_to_CD		= Wire(Decoupled(new HLS_COMMON_AXIS(112)))
	val h2c_data_from_DD_to_DMF		= Wire(Decoupled(new HLS_COMMON_AXIS(528)))
	val net_init_msg_from_CMD_to_P	= Wire(Decoupled(new HLS_COMMON_AXIS(64)))
	val dir_init_msg_from_CMD_to_DI	= Wire(Decoupled(new HLS_COMMON_AXIS(112)))
	val ctrl_msg_from_D_to_CC		= Wire(Decoupled(new HLS_COMMON_AXIS(504)))
	val ctrl_msg_from_D_to_DC		= Wire(Decoupled(new HLS_COMMON_AXIS(504)))
	val ctrl_msg_from_CMD_to_CC		= Wire(Decoupled(new HLS_COMMON_AXIS(504)))
	val ctrl_msg_from_CC_to_CMC		= Wire(Decoupled(new HLS_COMMON_AXIS(504)))
	val ctrl_msg_from_CC_to_CMD		= Wire(Decoupled(new HLS_COMMON_AXIS(504)))
	val ctrl_msg_from_DC_to_CMC		= Wire(Decoupled(new HLS_COMMON_AXIS(504)))
	val ctrl_msg_from_CMC_to_DMF	= Wire(Decoupled(new HLS_COMMON_AXIS(504)))
	val ctrl_msg_from_DMF_to_P		= Wire(Decoupled(new HLS_COMMON_AXIS(504)))
	val data_msg_from_DMF_to_P		= Wire(Decoupled(new HLS_COMMON_AXIS(584)))
	val c2h_cmd_from_D_to_CDD		= Wire(Decoupled(new HLS_COMMON_AXIS(112)))
	val c2h_data_from_D_to_CDD		= Wire(Decoupled(new HLS_COMMON_AXIS(528)))
	val dir_cmd_from_DC_to_DI		= Wire(Decoupled(new HLS_COMMON_AXIS(40)))
	val dir_data_from_DI_to_DC		= Wire(Decoupled(new HLS_COMMON_AXIS(112)))
	val c2h_data_from_CMD_to_BM		= Wire(Decoupled(new HLS_COMMON_AXIS(528)))
	val c2h_cmd_from_CMD_to_CDD		= Wire(Decoupled(new HLS_COMMON_AXIS(112)))
	val c2h_data_from_CMD_to_CDD	= Wire(Decoupled(new HLS_COMMON_AXIS(528)))
	val c2h_cmd_from_BM_to_CDD		= Wire(Decoupled(new HLS_COMMON_AXIS(112)))
	val c2h_data_from_BM_to_CDD		= Wire(Decoupled(new HLS_COMMON_AXIS(528)))

	val h2cCmdDeliverHLS					= Module(new h2cCmdDeliverHLS_wrapper())
	h2cCmdDeliverHLS.io.ap_clk 				:= userClk
	h2cCmdDeliverHLS.io.ap_rst_n 			:= userRstn
	h2cCmdDeliverHLS.io.h2c_cmd_from_DMF	<> h2c_cmd_from_DMF_to_CD
	h2cCmdDeliverHLS.io.h2c_cmd_to_qdma		<> transInst.io.hls_h2c_cmd

	val h2cDataDeliverHLS						= Module(new h2cDataDeliverHLS_wrapper())
	h2cDataDeliverHLS.io.ap_clk 				:= userClk
	h2cDataDeliverHLS.io.ap_rst_n 				:= userRstn
	h2cDataDeliverHLS.io.h2c_data_from_qdma		<> transInst.io.hls_h2c_data
	h2cDataDeliverHLS.io.h2c_data_to_DMF		<> h2c_data_from_DD_to_DMF

	val h2cCtrlMsgDeliverHLS					= Module(new h2cCtrlMsgDeliverHLS_wrapper())
	h2cCtrlMsgDeliverHLS.io.ap_clk 				:= userClk
	h2cCtrlMsgDeliverHLS.io.ap_rst_n 			:= userRstn
	h2cCtrlMsgDeliverHLS.io.h2c_w_from_qdma		<> transInst.io.hls_h2c_w
	h2cCtrlMsgDeliverHLS.io.ctrl_msg_to_CC		<> ctrl_msg_from_CMD_to_CC
	h2cCtrlMsgDeliverHLS.io.net_init_msg		<> net_init_msg_from_CMD_to_P
	h2cCtrlMsgDeliverHLS.io.dir_init_msg		<> dir_init_msg_from_CMD_to_DI

	val cacheCtrlHLS_from_net					= Module(new cacheCtrlHLS_from_net_wrapper())
	cacheCtrlHLS_from_net.io.ap_clk 			:= userClk
	cacheCtrlHLS_from_net.io.ap_rst_n 			:= userRstn
	cacheCtrlHLS_from_net.io.ctrl_msg_from_D	<> ctrl_msg_from_D_to_CC
	cacheCtrlHLS_from_net.io.ctrl_msg_to_CMD	<> ctrl_msg_from_CC_to_CMD

	val cacheCtrlHLS_to_net						= Module(new cacheCtrlHLS_to_net_wrapper())
	cacheCtrlHLS_to_net.io.ap_clk 				:= userClk
	cacheCtrlHLS_to_net.io.ap_rst_n 			:= userRstn
	cacheCtrlHLS_to_net.io.ctrl_msg_from_CMD	<> ctrl_msg_from_CMD_to_CC
	cacheCtrlHLS_to_net.io.ctrl_msg_to_CMC		<> ctrl_msg_from_CC_to_CMC

	val ctrlMsgCopy						= Module(new ctrlMsgCopy_wrapper())
	ctrlMsgCopy.io.ap_clk 				:= userClk
	ctrlMsgCopy.io.ap_rst_n 			:= userRstn
	ctrlMsgCopy.io.ctrl_msg_from_CC		<> ctrl_msg_from_CC_to_CMC
	ctrlMsgCopy.io.ctrl_msg_from_DC		<> ctrl_msg_from_DC_to_CMC
	ctrlMsgCopy.io.ctrl_msg_to_fifoC	<> ctrlMsgCopy.io.ctrl_msg_from_fifoC
	ctrlMsgCopy.io.ctrl_msg_to_DMF		<> ctrl_msg_from_CMC_to_DMF

	val dataMsgFetch					= Module(new dataMsgFetch_wrapper())
	dataMsgFetch.io.ap_clk 				:= userClk
	dataMsgFetch.io.ap_rst_n 			:= userRstn
	dataMsgFetch.io.h2c_cmd_to_CD		<> h2c_cmd_from_DMF_to_CD
	dataMsgFetch.io.h2c_data_from_DD	<> h2c_data_from_DD_to_DMF
	dataMsgFetch.io.ctrl_msg_from_CMC	<> ctrl_msg_from_CMC_to_DMF
	dataMsgFetch.io.ctrl_msg_to_P		<> ctrl_msg_from_DMF_to_P	
	dataMsgFetch.io.data_msg_to_P		<> data_msg_from_DMF_to_P	
	dataMsgFetch.io.ctrl_msg_to_fifoM	<> dataMsgFetch.io.ctrl_msg_from_fifoM

	val parser						= Module(new parser_wrapper())
	parser.io.ap_clk 				:= userClk
	parser.io.ap_rst_n 				:= userRstn
	parser.io.m_net_tx				<> transInst.io.hls_net_tx
	parser.io.net_init_msg			<> net_init_msg_from_CMD_to_P
	parser.io.ctrl_msg_from_DMF		<> ctrl_msg_from_DMF_to_P
	parser.io.data_msg_from_DMF		<> data_msg_from_DMF_to_P

	val deparser					= Module(new deparser_wrapper())
	deparser.io.ap_clk 				:= userClk
	deparser.io.ap_rst_n 			:= userRstn
	deparser.io.s_net_rx			<> transInst.io.hls_net_rx
	deparser.io.ctrl_msg_to_CC		<> ctrl_msg_from_D_to_CC	
	deparser.io.ctrl_msg_to_DC		<> ctrl_msg_from_D_to_DC	
	deparser.io.c2h_cmd_to_CDD		<> c2h_cmd_from_D_to_CDD
	deparser.io.c2h_data_to_CDD		<> c2h_data_from_D_to_CDD

	val dirCtrlHLS_from_net						= Module(new dirCtrlHLS_from_net_wrapper())
	dirCtrlHLS_from_net.io.ap_clk 				:= userClk
	dirCtrlHLS_from_net.io.ap_rst_n 			:= userRstn
	dirCtrlHLS_from_net.io.ctrl_msg_from_D		<> ctrl_msg_from_D_to_DC
	dirCtrlHLS_from_net.io.dir_cmd_to_DI		<> dir_cmd_from_DC_to_DI	

	val dirCtrlHLS_to_net						= Module(new dirCtrlHLS_to_net_wrapper())
	dirCtrlHLS_to_net.io.ap_clk 				:= userClk
	dirCtrlHLS_to_net.io.ap_rst_n 				:= userRstn
	dirCtrlHLS_to_net.io.ctrl_msg_to_CMC		<> ctrl_msg_from_DC_to_CMC
	dirCtrlHLS_to_net.io.dir_data_from_DI		<> dir_data_from_DI_to_DC	
	dirCtrlHLS_from_net.io.ctrl_msg_to_fifoD	<> dirCtrlHLS_to_net.io.ctrl_msg_from_fifoD


	val ddrInstHLS_aw_w						= Module(new ddrInstHLS_aw_w_wrapper())
	ddrInstHLS_aw_w.io.ap_clk 				:= userClk
	ddrInstHLS_aw_w.io.ap_rst_n 			:= userRstn
	ddrInstHLS_aw_w.io.dir_init_msg			<> dir_init_msg_from_CMD_to_DI
	ddrInstHLS_aw_w.io.ddr_aw				<> transInst.io.hls_ddrPort_aw	
	ddrInstHLS_aw_w.io.ddr_w				<> transInst.io.hls_ddrPort_w		

	val ddrInstHLS_b						= Module(new ddrInstHLS_b_wrapper())
	ddrInstHLS_b.io.ap_clk 					:= userClk
	ddrInstHLS_b.io.ap_rst_n 				:= userRstn
	ddrInstHLS_b.io.ddr_b					<> transInst.io.hls_ddrPort_b	

	val ddrInstHLS_ar						= Module(new ddrInstHLS_ar_wrapper())
	ddrInstHLS_ar.io.ap_clk 				:= userClk
	ddrInstHLS_ar.io.ap_rst_n 				:= userRstn
	ddrInstHLS_ar.io.dir_cmd_from_DC		<> dir_cmd_from_DC_to_DI
	ddrInstHLS_ar.io.ddr_ar					<> transInst.io.hls_ddrPort_ar

	val ddrInstHLS_r						= Module(new ddrInstHLS_r_wrapper())
	ddrInstHLS_r.io.ap_clk 					:= userClk
	ddrInstHLS_r.io.ap_rst_n 				:= userRstn
	ddrInstHLS_r.io.dir_data_to_DC			<> dir_data_from_DI_to_DC
	ddrInstHLS_r.io.ddr_r					<> transInst.io.hls_ddrPort_r

	val c2hCtrlMsgDeliverHLS					= Module(new c2hCtrlMsgDeliverHLS_wrapper())
	c2hCtrlMsgDeliverHLS.io.ap_clk 				:= userClk
	c2hCtrlMsgDeliverHLS.io.ap_rst_n 			:= userRstn
	c2hCtrlMsgDeliverHLS.io.ctrl_msg_from_CC	<> ctrl_msg_from_CC_to_CMD
	c2hCtrlMsgDeliverHLS.io.c2h_data_to_BM		<> c2h_data_from_CMD_to_BM
	c2hCtrlMsgDeliverHLS.io.c2h_cmd_to_CDD		<> c2h_cmd_from_CMD_to_CDD	
	c2hCtrlMsgDeliverHLS.io.c2h_data_to_CDD		<> c2h_data_from_CMD_to_CDD

	val c2hBufferMgrHLS							= Module(new c2hMultiBufferMgrHLS_wrapper())
	c2hBufferMgrHLS.io.ap_clk 				:= userClk
	c2hBufferMgrHLS.io.ap_rst_n 			:= userRstn
	c2hBufferMgrHLS.io.q_num				:= 1.U
	c2hBufferMgrHLS.io.q_addr_arr			<> Cat(controlReg(132), controlReg(133))
	c2hBufferMgrHLS.io.q_hidx_arr			<> controlReg(134)
	c2hBufferMgrHLS.io.q_tidx_arr			<> statusReg(135)
	c2hBufferMgrHLS.io.c2h_cmd_to_CDD		<> c2h_cmd_from_BM_to_CDD
	c2hBufferMgrHLS.io.c2h_data_to_CDD		<> c2h_data_from_BM_to_CDD
	c2hBufferMgrHLS.io.c2h_data_from_CMD	<> c2h_data_from_CMD_to_BM

	val c2hCmdDataDeliverHLS					= Module(new c2hCmdDataDeliverHLS_wrapper())
	c2hCmdDataDeliverHLS.io.ap_clk 				:= userClk
	c2hCmdDataDeliverHLS.io.ap_rst_n 			:= userRstn
	c2hCmdDataDeliverHLS.io.c2h_cmd_from_D		<> c2h_cmd_from_D_to_CDD
	c2hCmdDataDeliverHLS.io.c2h_data_from_D		<> c2h_data_from_D_to_CDD
	c2hCmdDataDeliverHLS.io.c2h_cmd_from_BM		<> c2h_cmd_from_BM_to_CDD
	c2hCmdDataDeliverHLS.io.c2h_data_from_BM	<> c2h_data_from_BM_to_CDD
	c2hCmdDataDeliverHLS.io.c2h_cmd_from_CMD	<> c2h_cmd_from_CMD_to_CDD
	c2hCmdDataDeliverHLS.io.c2h_data_from_CMD	<> c2h_data_from_CMD_to_CDD
	c2hCmdDataDeliverHLS.io.c2h_cmd_to_qdma		<> transInst.io.hls_c2h_cmd
	c2hCmdDataDeliverHLS.io.c2h_data_to_qdma	<> transInst.io.hls_c2h_data
    /******************************/
    /******************************/
}	
