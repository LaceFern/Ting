import os
import re

wrapper_path = '/home/zxy/2024_vivado_pro/DSM/test_pro_multi_node_2/rc4ml_static_u280/rc4ml_static_u280.srcs/dynamic_sources/add_sources/highPara_wrapper/'
output_path = '/home/zxy/nfs/DSM_prj/FPGA-DSM/hard_multi_node/chisel/dynamic/'
output_filename = 'hlsTop.scala'

with open(output_path + output_filename, 'w') as output_file:

    output_file.write(
"""package static.u280

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

""")

    io_info_for_nonaxis = {}
    io_info_for_axis = {}
    module_instances = {}

    io_pattern = re.compile(
        r'\s*val\s+(\w+)\s*=\s*(Input|Output|Decoupled|Flipped)\((.+)\)'
    )
    module_pattern = re.compile(r'class\s+(\w+)\s+extends\s+BlackBox')

    def get_unique_key(io_name):
        match = re.match(r'(.*f_[^_]+_t_[^_]+_)', io_name)
        return match.group(1) if match else io_name

    for filename in os.listdir(wrapper_path):
        if filename.endswith('.sv'):
            file_path = os.path.join(wrapper_path, filename)
            with open(file_path, 'r') as sv_file:
                chisel_definition = []
                in_comment_block = False
                for line in sv_file:
                    if '/**' in line:
                        in_comment_block = True
                    elif '*/' in line and in_comment_block:
                        in_comment_block = False
                        break
                    elif in_comment_block:
                        chisel_definition.append(line)
                
                if chisel_definition:
                    output_file.write(f"// From {filename}\n")
                    output_file.writelines(chisel_definition)
                    output_file.write("\n\n")

            with open(file_path, 'r') as sv_file:

                for line in sv_file:
                    match = module_pattern.search(line)
                    if match:
                        module_name = match.group(1)
                        if module_name not in module_instances:
                            module_instances[module_name] = []

                    match = io_pattern.search(line)
                    if match:
                        io_name = match.group(1)  # Name of the IO port
                        io_direction = match.group(2)  # Direction (Input or Output)
                        io_type = match.group(3)  # Type details, e.g., Clock(), UInt(512.W)

                        io_key = get_unique_key(io_name)

                        if io_direction == "Input" or io_direction == "Output":
                            if io_key not in io_info_for_nonaxis:
                                io_info_for_nonaxis[io_key] = (io_direction, io_type)

                        elif io_key in io_info_for_axis:
                            io_info_for_axis[io_key].append((io_name, io_direction, io_type))
                        else:
                            io_info_for_axis[io_key] = [(io_name, io_direction, io_type)]

                        module_instances[module_name].append((io_name, io_direction, io_type))


    output_file.write("\nclass hlsTop extends RawModule {\n")
    output_file.write("    val io = IO(new Bundle {\n")
    
    for io_key, (io_direction, io_type) in io_info_for_nonaxis.items():
            output_file.write(f"        val {io_key} = {io_direction}({io_type})\n")

    for io_key, io_list in io_info_for_axis.items():
        if len(io_list) == 1:
            io_name, io_direction, io_type = io_list[0]
            output_file.write(f"        val {io_key} = {io_direction}({io_type})\n")

    output_file.write("    })\n\n")


    def remove_decoupled(io_type):
        decoupled_pattern = re.compile(r'Decoupled\((.*)\)')
        match = decoupled_pattern.search(io_type)
        if match:
            return match.group(1)
        else:
            return io_type

    for io_key, io_list in io_info_for_axis.items():
        if len(io_list) != 1:
            io_name, io_direction, io_type = io_list[0]
            tmp_io_type = remove_decoupled(io_type)
            output_file.write(f"    val {io_key} = Wire(Decoupled({tmp_io_type}))\n")
    output_file.write("\n")


    for module_name, io_list in module_instances.items():
        output_file.write(f"    val {module_name} = Module(new {module_name}())\n")
        
        for io_name, io_direction, io_match_flag in io_list:
            if io_direction == "Input":
                output_file.write(f"    {module_name}.io.{io_name} := io.{io_name}\n")
            elif io_direction == "Output":
                output_file.write(f"    io.{io_name} := {module_name}.io.{io_name}\n")
            else:
                io_key = get_unique_key(io_name)
                io_list = io_info_for_axis[io_key]
                if len(io_list) == 1:
                    output_file.write(f"    {module_name}.io.{io_name} <> io.{io_key}\n")
                else:
                    output_file.write(f"    {module_name}.io.{io_name} <> {io_key}\n")
        output_file.write("\n")

    output_file.write("}\n")