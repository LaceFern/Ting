
## Complete Vivado Project with HLS

Config complete_vpro_with_hls.py & Run the following code

```bash
source ~/.bashrc
source /tools/Xilinx/Vitis_HLS/2021.1/settings64.sh
python3 ./complete_vpro_with_hls.py
```

Attention: do not run ./gen_IP_and_wrapper_adv.py while opening vivado, otherwise the process may stall!

Attention: 

if you want to replace one certain ip with ./complete_vpro_with_hls.py in the vpro, mention the two case:

if the module is not yet synthesis, just directly using .py
if the module is synthesis, delete the module and the related fifo under the wrapper and then run .py 