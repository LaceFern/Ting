`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 06/05/2024 07:27:37 PM
// Design Name: 
// Module Name: test
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module test(

    );
  
reg           io_ap_clk              ;
reg           io_ap_rst_n            ;
reg           io_pass_flag           ;
reg [31:0]    io_pass_count          ;
wire [31:0]   io_ABM_c2h_q_hidx[3];
wire [31:0]   io_SBM_c2h_q_hidx[3];
reg           io_net_init_flag       ;
reg           io_dir_init_flag       ;
reg           io_warmup_flag         ;
reg           io_test_flag           ;
reg           io_unlock_flag         ;
reg [31:0]    io_iter_num            ;
reg [7:0]     io_node_num            ;
reg [7:0]     io_node_id             ;
reg [7:0]     io_warmup_type         ;
reg [7:0]     io_test_type           ;
reg [31:0]    io_debug_ready_flag_idx;
reg           io_debug_end_flag      ;
reg [511:0]  io_SBM_q_addr_arr[3];
reg [255:0]  io_SBM_q_tidx_arr[3];
wire [255:0]  io_SBM_q_hidx_arr[3];
reg [1535:0] io_ABM_q_addr_arr[3];
reg [767:0]  io_ABM_q_tidx_arr[3];
wire [767:0]  io_ABM_q_hidx_arr[3];
reg [31:0]    io_q_num               ;
reg [7:0]     io_debug_state         ;
wire          io_net_0_ready      [3];
wire          io_net_0_valid      [3];
wire [583:0]  io_net_0_bits_data  [3];
wire          io_net_1_ready      [3];
wire          io_net_1_valid      [3];
wire [583:0]  io_net_1_bits_data  [3];
wire          io_net_2_ready      [3];
wire          io_net_2_valid      [3];
wire [583:0]  io_net_2_bits_data  [3];

  U280DynamicGreyBox U280DynamicGreyBox_inst_0 ( 
    .io_ap_clk                (io_ap_clk              ),
    .io_ap_rst_n              (io_ap_rst_n            ),
    .io_pass_flag             (io_pass_flag           ),
    .io_pass_count            (io_pass_count          ),
    .io_ABM_c2h_q_hidx        (io_ABM_c2h_q_hidx[0]     ),
    .io_SBM_c2h_q_hidx        (io_SBM_c2h_q_hidx[0]      ),
    .io_net_init_flag         (io_net_init_flag       ),
    .io_dir_init_flag         (io_dir_init_flag       ),
    .io_warmup_flag           (io_warmup_flag         ),
    .io_test_flag             (io_test_flag           ),
    .io_unlock_flag           (io_unlock_flag         ),
    .io_iter_num              (io_iter_num            ),
    .io_node_num              (io_node_num            ),
    .io_node_id               (0             ),
    .io_warmup_type           (io_warmup_type         ),
    .io_test_type             (io_test_type           ),
    .io_debug_ready_flag_idx  (io_debug_ready_flag_idx),
    .io_debug_end_flag        (io_debug_end_flag      ),
    .io_ABM_q_addr_arr        ({24{64'hff0}}      ),
    .io_ABM_q_tidx_arr        (io_ABM_q_tidx_arr[0]      ),
    .io_ABM_q_hidx_arr        ({24{io_ABM_c2h_q_hidx[0]}}),
    .io_SBM_q_addr_arr        ({8{64'hff0}}      ),
    .io_SBM_q_tidx_arr        (io_SBM_q_tidx_arr[0]      ),
    .io_SBM_q_hidx_arr        ({8{io_SBM_c2h_q_hidx[0]}} ),
    .io_q_num                 (io_q_num               ),
    .io_debug_state           (io_debug_state         ),
    .io_net_tx_0_ready        (io_net_0_ready      [0]),
    .io_net_tx_0_valid        (io_net_0_valid      [0]),
    .io_net_tx_0_bits_data    (io_net_0_bits_data  [0]),
    .io_net_tx_1_ready        (io_net_1_ready      [0]),
    .io_net_tx_1_valid        (io_net_1_valid      [0]),
    .io_net_tx_1_bits_data    (io_net_1_bits_data  [0]),
    .io_net_tx_2_ready        (io_net_2_ready      [0]),
    .io_net_tx_2_valid        (io_net_2_valid      [0]),
    .io_net_tx_2_bits_data    (io_net_2_bits_data  [0]),
    .io_net_rx_0_ready        (io_net_0_ready      [0]),
    .io_net_rx_0_valid        (io_net_0_valid      [0]),
    .io_net_rx_0_bits_data    (io_net_0_bits_data  [0]),
    .io_net_rx_1_ready        (io_net_0_ready      [1]),
    .io_net_rx_1_valid        (io_net_0_valid      [1]),
    .io_net_rx_1_bits_data    (io_net_0_bits_data  [1]),
    .io_net_rx_2_ready        (io_net_0_ready      [2]),
    .io_net_rx_2_valid        (io_net_0_valid      [2]),
    .io_net_rx_2_bits_data    (io_net_0_bits_data  [2])
  );
    

  U280DynamicGreyBox U280DynamicGreyBox_inst_1 ( 
    .io_ap_clk                (io_ap_clk              ),
    .io_ap_rst_n              (io_ap_rst_n            ),
    .io_pass_flag             (io_pass_flag           ),
    .io_pass_count            (io_pass_count          ),
    .io_ABM_c2h_q_hidx        (io_ABM_c2h_q_hidx[1]      ),
    .io_SBM_c2h_q_hidx        (io_SBM_c2h_q_hidx[1]      ),
    .io_net_init_flag         (io_net_init_flag       ),
    .io_dir_init_flag         (io_dir_init_flag       ),
    .io_warmup_flag           (io_warmup_flag         ),
    .io_test_flag             (io_test_flag           ),
    .io_unlock_flag           (io_unlock_flag         ),
    .io_iter_num              (io_iter_num            ),
    .io_node_num              (io_node_num            ),
    .io_node_id               (1             ),
    .io_warmup_type           (io_warmup_type         ),
    .io_test_type             (io_test_type           ),
    .io_debug_ready_flag_idx  (io_debug_ready_flag_idx),
    .io_debug_end_flag        (io_debug_end_flag      ),
    .io_ABM_q_addr_arr        ({24{64'hff1}}      ),
    .io_ABM_q_tidx_arr        (io_ABM_q_tidx_arr[1]      ),
    .io_ABM_q_hidx_arr        ({24{io_ABM_c2h_q_hidx[1]}}),
    .io_SBM_q_addr_arr        ({8{64'hff1}}      ),
    .io_SBM_q_tidx_arr        (io_SBM_q_tidx_arr[1]      ),
    .io_SBM_q_hidx_arr        ({8{io_SBM_c2h_q_hidx[1]}} ),
    .io_q_num                 (io_q_num               ),
    .io_debug_state           (io_debug_state         ),
    .io_net_tx_0_ready        (io_net_0_ready      [1]),
    .io_net_tx_0_valid        (io_net_0_valid      [1]),
    .io_net_tx_0_bits_data    (io_net_0_bits_data  [1]),
    .io_net_tx_1_ready        (io_net_1_ready      [1]),
    .io_net_tx_1_valid        (io_net_1_valid      [1]),
    .io_net_tx_1_bits_data    (io_net_1_bits_data  [1]),
    .io_net_tx_2_ready        (io_net_2_ready      [1]),
    .io_net_tx_2_valid        (io_net_2_valid      [1]),
    .io_net_tx_2_bits_data    (io_net_2_bits_data  [1]),
    .io_net_rx_0_ready        (io_net_1_ready      [0]),
    .io_net_rx_0_valid        (io_net_1_valid      [0]),
    .io_net_rx_0_bits_data    (io_net_1_bits_data  [0]),
    .io_net_rx_1_ready        (io_net_1_ready      [1]),
    .io_net_rx_1_valid        (io_net_1_valid      [1]),
    .io_net_rx_1_bits_data    (io_net_1_bits_data  [1]),
    .io_net_rx_2_ready        (io_net_1_ready      [2]),
    .io_net_rx_2_valid        (io_net_1_valid      [2]),
    .io_net_rx_2_bits_data    (io_net_1_bits_data  [2])
  );

    U280DynamicGreyBox U280DynamicGreyBox_inst_2 ( 
    .io_ap_clk                (io_ap_clk              ),
    .io_ap_rst_n              (io_ap_rst_n            ),
    .io_pass_flag             (io_pass_flag           ),
    .io_pass_count            (io_pass_count          ),
    .io_ABM_c2h_q_hidx        (io_ABM_c2h_q_hidx[2]      ),
    .io_SBM_c2h_q_hidx        (io_SBM_c2h_q_hidx[2]      ),
    .io_net_init_flag         (io_net_init_flag       ),
    .io_dir_init_flag         (io_dir_init_flag       ),
    .io_warmup_flag           (io_warmup_flag         ),
    .io_test_flag             (io_test_flag           ),
    .io_unlock_flag           (io_unlock_flag         ),
    .io_iter_num              (io_iter_num            ),
    .io_node_num              (io_node_num            ),
    .io_node_id               (2             ),
    .io_warmup_type           (io_warmup_type         ),
    .io_test_type             (io_test_type           ),
    .io_debug_ready_flag_idx  (io_debug_ready_flag_idx),
    .io_debug_end_flag        (io_debug_end_flag      ),
    .io_ABM_q_addr_arr        ({24{64'hff2}}      ),
    .io_ABM_q_tidx_arr        (io_ABM_q_tidx_arr[2]      ),
    .io_ABM_q_hidx_arr        ({24{io_ABM_c2h_q_hidx[2]}}),
    .io_SBM_q_addr_arr        ({8{64'hff2}}      ),
    .io_SBM_q_tidx_arr        (io_SBM_q_tidx_arr[2]      ),
    .io_SBM_q_hidx_arr        ({8{io_SBM_c2h_q_hidx[2]}} ),
    .io_q_num                 (io_q_num               ),
    .io_debug_state           (io_debug_state         ),
    .io_net_tx_0_ready        (io_net_0_ready      [2]),
    .io_net_tx_0_valid        (io_net_0_valid      [2]),
    .io_net_tx_0_bits_data    (io_net_0_bits_data  [2]),
    .io_net_tx_1_ready        (io_net_1_ready      [2]),
    .io_net_tx_1_valid        (io_net_1_valid      [2]),
    .io_net_tx_1_bits_data    (io_net_1_bits_data  [2]),
    .io_net_tx_2_ready        (io_net_2_ready      [2]),
    .io_net_tx_2_valid        (io_net_2_valid      [2]),
    .io_net_tx_2_bits_data    (io_net_2_bits_data  [2]),
    .io_net_rx_0_ready        (io_net_2_ready      [0]),
    .io_net_rx_0_valid        (io_net_2_valid      [0]),
    .io_net_rx_0_bits_data    (io_net_2_bits_data  [0]),
    .io_net_rx_1_ready        (io_net_2_ready      [1]),
    .io_net_rx_1_valid        (io_net_2_valid      [1]),
    .io_net_rx_1_bits_data    (io_net_2_bits_data  [1]),
    .io_net_rx_2_ready        (io_net_2_ready      [2]),
    .io_net_rx_2_valid        (io_net_2_valid      [2]),
    .io_net_rx_2_bits_data    (io_net_2_bits_data  [2])
  );
    
    initial begin
        io_ap_clk               = 0;
        io_ap_rst_n             = 0;
        io_pass_flag = 1;
        io_pass_count = 32'hffffffff;
        // io_c2h_q_hidx           = 0;
        io_net_init_flag        = 0;
        io_dir_init_flag        = 0;
        io_warmup_flag          = 0;
        io_test_flag            = 0;
        io_unlock_flag          = 0;
        io_iter_num             = 1024;
        io_node_num             = 3;
        // io_node_id              = 0;
        io_warmup_type          = 0;
        io_test_type            = 0;
        // io_debug_ready_flag_idx = 0;
        // io_debug_end_flag       = 0;
        // io_q_addr_arr           = 0;
        // io_q_tidx_arr           = 0;
        // io_q_hidx_arr           = 0;
        io_q_num                = 1;
        // io_debug_state          = 0;
        //----------------------------

        # 512
        io_ap_rst_n = 1;
        //----------------------------

        // net init
        # 512
        io_net_init_flag = 1;

        # 512
        io_dir_init_flag = 1;

        // warm up
        # 4096
        io_warmup_flag = 1;
        io_warmup_type = 0;
        io_unlock_flag = 0;

        # 8192
        io_warmup_flag = 0;
        io_warmup_type = 0;
        io_unlock_flag = 0;

        # 512
        io_warmup_flag = 1;
        io_warmup_type = 0;
        io_unlock_flag = 1;

        # 4096
        io_warmup_flag = 0;
        io_warmup_type = 0;
        io_unlock_flag = 0;

        // warm up (second)
        # 512
        io_warmup_flag = 1;
        io_warmup_type = 0;
        io_unlock_flag = 0;

        # 8192
        io_warmup_flag = 0;
        io_warmup_type = 0;
        io_unlock_flag = 0;

        # 512
        io_warmup_flag = 1;
        io_warmup_type = 0;
        io_unlock_flag = 1;

        # 8192
        io_warmup_flag = 0;
        io_warmup_type = 0;
        io_unlock_flag = 0;

        // test
        # 512
        io_test_flag = 1;
        io_test_type = 1;
        io_unlock_flag = 0;

        # 8192
        io_test_flag = 0;
        io_test_type = 0;
        io_unlock_flag = 0;

        # 512
        io_test_flag = 1;
        io_test_type = 1;
        io_unlock_flag = 1;

        
    end

    //----------------------------

    always begin
        # 1
        io_ap_clk <= ~io_ap_clk;
    end

endmodule
