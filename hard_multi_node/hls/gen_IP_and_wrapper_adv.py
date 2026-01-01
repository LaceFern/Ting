import os
import subprocess
import threading
import socket
import re
from tabulate import tabulate



project_name = 'highPara'
outPath = '/home/zxy/nfs/DSM_prj/highPara_ips_and_wrappers_1/'





def find_cpp_files(root_dir):
    file_names = []
    file_paths = []
    exclude_files = ['utils.cpp', 'ddrSim.cpp', 'qdmaSim.cpp', 'shiftSim.cpp']

    for dirpath, _, filenames in os.walk(root_dir):
        for filename in filenames:
            if filename.endswith('.cpp') and filename not in exclude_files:
                file_name_without_extension = os.path.splitext(filename)[0]
                file_names.append(file_name_without_extension)
                file_paths.append(os.path.join(dirpath, filename))

    return file_names, file_paths



root_directory = './' + project_name
ip_name_arr, ip_srcPath_arr = find_cpp_files(root_directory)

print("ip_name_arr:")
print(ip_name_arr)
print("\nip_srcPath_arr:")
print(ip_srcPath_arr)


cmd = 'mkdir -p ' + outPath
print(cmd)
p=subprocess.Popen(cmd,shell=True)
return_code=p.wait()  # waiting for end
print(os.path.exists(outPath))

fifo_name_arr = []
fifo_bits_arr = []
fifo_depth = 1024

def remove_suffix_if_exists(s):
    suffix = "_V"
    if s.endswith(suffix):
        return s[:-len(suffix)]
    return s

for i in range(len(ip_name_arr)):
    ip_name = ip_name_arr[i]
    ip_srcPath = ip_srcPath_arr[i]

    with open(outPath + 'gen_HLSIP_script.tcl','w') as f:
        if i == 0:
            f.write('open_project -reset ' + project_name + ' \n') ## this command will clean all files generated before
        else:
            f.write('open_project ' + project_name + ' \n')
        f.write('set_top ' + ip_name + ' \n')
        f.write('add_files ' + ip_srcPath + '\n')
        f.write('add_files ./' + project_name + '/src/' + 'utils.hpp \n')
        f.write('add_files ./' + project_name + '/src/' + 'utils.cpp \n')
        f.write('open_solution "' + ip_name + '" \n')
        f.write('set_part {xcu280-fsvh2892-2L-e} \n')
        f.write('create_clock -period 4 -name default \n')
        f.write('config_export -format ip_catalog -rtl verilog \n')
        f.write('csynth_design \n')
        f.write('export_design -rtl verilog -format ip_catalog \n')
    f.close()
    cmd = 'vitis_hls ' + outPath + 'gen_HLSIP_script.tcl'
    p=subprocess.Popen(cmd,shell=True)
    return_code=p.wait()  # waiting for end

    interface_attr = ['name', 'Object', 'IOProtocol', 'Dir', 'Bits', 'Derivation']
    interface = []
    port = []
    with open('./' + project_name + '/' + ip_name + '/syn/report/' + ip_name + '_csynth.xml','r') as f:
        while True:
            line = f.readline()
            if not line:
                break
            line_sep = re.split('[<>]',line)
            if len(line_sep) > 1:
                if '/RtlPorts' in line_sep[1]:
                    interface.append(port.copy())
                elif 'RtlPorts' in line_sep[1]:
                    port.clear()
                elif any(line_sep[1] in s for s in interface_attr):
                    port.append(line_sep[2])
    f.close()
    for port in interface:
        if port[interface_attr.index('IOProtocol')] in 'axis':
            if 'DATA' in port[interface_attr.index('name')]:
                port.append(0)
            else:
                port.append(1)
        else:
            port.append(0)

    for port in interface:
        port[interface_attr.index('Object')] = remove_suffix_if_exists(port[interface_attr.index('Object')])

    cmd = 'mkdir -p ' + outPath + 'add_sources/' + project_name + '_wrapper'
    p=subprocess.Popen(cmd,shell=True)
    return_code=p.wait()  # waiting for end
    with open(outPath + 'add_sources/' + project_name + '_wrapper/' + ip_name + '_wrapper.sv','w') as f:

        f.write('/**\n')
        f.write('class ' + ip_name + '_wrapper ' + 'extends BlackBox {\n')
        f.write('\tval io = IO(new Bundle {\n')
        
        for port in interface:
            if port[interface_attr.index('Derivation')] == 0:
                if port[interface_attr.index('Dir')] in 'in':
                    if port[interface_attr.index('name')] in 'ap_clk':
                        f.write('\t\tval ap_clk = Input(Clock())\n')
                    elif port[interface_attr.index('name')] in 'ap_rst_n':
                        f.write('\t\tval ap_rst_n = Input(Bool())\n')
                    elif port[interface_attr.index('IOProtocol')] in 'axis':
                        f.write('\t\tval ' + port[interface_attr.index('Object')] + ' = Flipped(Decoupled(new HLS_COMMON_AXIS(' + port[interface_attr.index('Bits')] + ')))\n')
                    else:
                        f.write('\t\tval ' + port[interface_attr.index('Object')] + ' = Input(UInt(' + port[interface_attr.index('Bits')] + '.W))\n')
                elif port[interface_attr.index('Dir')] in 'out':
                    if port[interface_attr.index('IOProtocol')] in 'axis':
                        f.write('\t\tval ' + port[interface_attr.index('Object')] + ' = Decoupled(new HLS_COMMON_AXIS(' + port[interface_attr.index('Bits')] + '))\n')
                    else:
                        f.write('\t\tval ' + port[interface_attr.index('Object')] + ' = Output(UInt(' + port[interface_attr.index('Bits')] + '.W))\n')
        f.write('\t}) \n')
        f.write('} \n')
        f.write('**/\n')

        f.write('module ' + ip_name + '_wrapper( \n')
        for port in interface:
            if port[interface_attr.index('Derivation')] == 0:
                if port[interface_attr.index('Dir')] in 'in':
                    if port[interface_attr.index('IOProtocol')] in 'axis':
                        f.write('\tinput ' + '[0:0] ' + port[interface_attr.index('Object')] + '_valid,\n')
                        f.write('\toutput ' + '[0:0] ' + port[interface_attr.index('Object')] + '_ready,\n')
                        f.write('\tinput ' + '[' + str(int(port[interface_attr.index('Bits')])-1) + ':0] ' + port[interface_attr.index('Object')] + '_bits_data')
                    else:
                        f.write('\tinput ' + '[' + str(int(port[interface_attr.index('Bits')])-1) + ':0] ' + port[interface_attr.index('name')])
                elif port[interface_attr.index('Dir')] in 'out':
                    if port[interface_attr.index('IOProtocol')] in 'axis':
                        f.write('\toutput ' + '[0:0] ' + port[interface_attr.index('Object')] + '_valid,\n')
                        f.write('\tinput ' + '[0:0] ' + port[interface_attr.index('Object')] + '_ready,\n')
                        f.write('\toutput ' + '[' + str(int(port[interface_attr.index('Bits')])-1) + ':0] ' + port[interface_attr.index('Object')] + '_bits_data')
                    else:
                        f.write('\toutput ' + '[' + str(int(port[interface_attr.index('Bits')])-1) + ':0] ' + port[interface_attr.index('name')])
                if port[interface_attr.index('Object')] != interface[-1][interface_attr.index('Object')]:
                    f.write(',\n')
                else:
                    f.write('\n')
        f.write('); \n')
        for port in interface:
            if port[interface_attr.index('Dir')] in 'in' and port[interface_attr.index('IOProtocol')] in 'axis':
                if port[interface_attr.index('Derivation')] == 0:

                    f.write('wire ' + '[0:0] ' + port[interface_attr.index('Object')] + '_inner_valid;\n')
                    f.write('wire ' + '[0:0] ' + port[interface_attr.index('Object')] + '_inner_ready;\n')
                    f.write('wire ' + '[' + str(int(port[interface_attr.index('Bits')])-1) + ':0] ' + port[interface_attr.index('Object')] + '_inner_bits_data;\n')

                    f.write('axis_data_fifo_w' + port[interface_attr.index('Bits')] + 'd' + str(fifo_depth) + ' ' + 'inner_fifo_4' + port[interface_attr.index('Object')] + '(\n')
                    f.write('\t.s_axis_aresetn(ap_rst_n),\n')
                    f.write('\t.s_axis_aclk(ap_clk),\n')
                    f.write('\t.s_axis_tvalid(' + port[interface_attr.index('Object')] + '_valid),\n')
                    f.write('\t.s_axis_tready(' + port[interface_attr.index('Object')] + '_ready),\n')
                    f.write('\t.s_axis_tdata(' + port[interface_attr.index('Object')] + '_bits_data),\n')
                    f.write('\t.m_axis_tvalid(' + port[interface_attr.index('Object')] + '_inner_valid),\n')
                    f.write('\t.m_axis_tready(' + port[interface_attr.index('Object')] + '_inner_ready),\n')
                    f.write('\t.m_axis_tdata(' + port[interface_attr.index('Object')] + '_inner_bits_data)\n')
                    f.write(');\n')
                    if 'axis_data_fifo_w' + port[interface_attr.index('Bits')] + 'd' + str(fifo_depth) not in fifo_name_arr:
                        fifo_name_arr.append('axis_data_fifo_w' + port[interface_attr.index('Bits')] + 'd' + str(fifo_depth))
                        fifo_bits_arr.append(port[interface_attr.index('Bits')])

        f.write(ip_name + '_0 ' + ip_name + '_inst(\n')
        for port in interface:
            if port[interface_attr.index('IOProtocol')] not in 'axis':
                f.write('\t.' + port[interface_attr.index('name')] + '(' + port[interface_attr.index('name')] + ')')
            else:
                if port[interface_attr.index('Dir')] in 'input':
                    if 'DATA' in port[interface_attr.index('name')]:
                        f.write('\t.' + port[interface_attr.index('name')] + '(' + port[interface_attr.index('Object')] + '_inner_bits_data)')
                    elif 'VALID' in port[interface_attr.index('name')]:
                        f.write('\t.' + port[interface_attr.index('name')] + '(' + port[interface_attr.index('Object')] + '_inner_valid)')
                    elif 'READY' in port[interface_attr.index('name')]:
                        f.write('\t.' + port[interface_attr.index('name')] + '(' + port[interface_attr.index('Object')] + '_ready)')
                else:
                    if 'DATA' in port[interface_attr.index('name')]:
                        f.write('\t.' + port[interface_attr.index('name')] + '(' + port[interface_attr.index('Object')] + '_bits_data)')
                    elif 'VALID' in port[interface_attr.index('name')]:
                        f.write('\t.' + port[interface_attr.index('name')] + '(' + port[interface_attr.index('Object')] + '_valid)')
                    elif 'READY' in port[interface_attr.index('name')]:
                        f.write('\t.' + port[interface_attr.index('name')] + '(' + port[interface_attr.index('Object')] + '_inner_ready)')     
            if port[interface_attr.index('name')] != interface[-1][interface_attr.index('name')]:
                f.write(',\n')
            else:
                f.write('\n')
        f.write(');\n')
        f.write('endmodule \n')
    f.close()   

print(fifo_name_arr)
print(fifo_bits_arr)
cmd = 'mkdir -p ' + outPath + 'add_sources/' + project_name + '_fifo'
print(cmd)
p=subprocess.Popen(cmd,shell=True)
cmd = 'mkdir -p ' + outPath + 'add_sources/' + project_name + '_fifo/ip'
print(cmd)
p=subprocess.Popen(cmd,shell=True)
return_code=p.wait()  # waiting for end 
with open(outPath + 'gen_RTLFIFO_script.tcl','w') as f:
    f.write('set_param general.maxThreads 2\n')
    f.write('create_project -force ' + outPath + 'add_sources/' + project_name + '_fifo/' + project_name + '_fifo_prj.xpr\n')
    f.write('set_property board_part xilinx.com:au280:part0:1.1 [current_project]\n')
    for fifo_idx in range(len(fifo_name_arr)):
        f.write('create_ip -name axis_data_fifo ')
        f.write('-vendor xilinx.com -library ip -version 2.0 ')
        f.write('-module_name ' + fifo_name_arr[fifo_idx] + ' ')
        f.write('-dir ' + outPath + 'add_sources/' + project_name + '_fifo/ip\n')
        f.write('set_property -dict [list CONFIG.TDATA_NUM_BYTES {' + str(int(fifo_bits_arr[fifo_idx])/8) + '} CONFIG.FIFO_DEPTH {' + str(fifo_depth) + '} CONFIG.Component_Name {axis_data_fifo_w' + fifo_bits_arr[fifo_idx] + 'd' + str(fifo_depth) + '}] [get_ips axis_data_fifo_w' + fifo_bits_arr[fifo_idx] + 'd' + str(fifo_depth) + ']\n')
            
f.close()
cmd = 'vivado -mode batch -source ' + outPath + 'gen_RTLFIFO_script.tcl'
p=subprocess.Popen(cmd,shell=True)
return_code=p.wait()  # waiting for end 

cmd = 'mkdir -p ' + outPath + 'add_sources/' + project_name + '_hlsIP'
print(cmd)
p=subprocess.Popen(cmd,shell=True)
cmd = 'mkdir -p ' + outPath + 'add_sources/' + project_name + '_hlsIP/ip'
print(cmd)
p=subprocess.Popen(cmd,shell=True)
return_code=p.wait()  # waiting for end 
with open(outPath + 'gen_RTLHLSIP_script.tcl','w') as f:
    f.write('set_param general.maxThreads 2\n')
    f.write('create_project -force ' + outPath + 'add_sources/' + project_name + '_hlsIP/' + project_name + '_hlsIP_prj.xpr\n')
    f.write('set_property board_part xilinx.com:au280:part0:1.1 [current_project]\n')
    f.write('set_property ip_repo_paths  ./' + project_name + ' [current_project]\n')
    f.write('update_ip_catalog\n')
    for i in range(len(ip_name_arr)):
        ip_name = ip_name_arr[i]

        f.write('create_ip -name ' + ip_name + ' ')
        f.write('-vendor xilinx.com -library hls -version 1.0 ')
        f.write('-module_name ' + ip_name + '_0 ')
        f.write('-dir ' + outPath + 'add_sources/' + project_name + '_hlsIP/ip\n')
f.close()
cmd = 'vivado -mode batch -source ' + outPath + 'gen_RTLHLSIP_script.tcl'
p=subprocess.Popen(cmd,shell=True)
return_code=p.wait()  # waiting for end 

cmd = 'rm -rf *.log *.jou'
p=subprocess.Popen(cmd,shell=True)
return_code=p.wait()  # waiting for end 
cmd = 'rm -rf .Xil'
p=subprocess.Popen(cmd,shell=True)
return_code=p.wait()  # waiting for end 

cmd = 'rm -rf ./' + project_name + '/hls.app'
p=subprocess.Popen(cmd,shell=True)
return_code=p.wait()  # waiting for end 

cmd = 'mkdir -p ' + outPath + 'ip_catalog/'
p=subprocess.Popen(cmd,shell=True)
return_code=p.wait()  # waiting for end 
for i in range(len(ip_name_arr)):
    ip_name = ip_name_arr[i]
    
    cmd = 'rm -rf ' + outPath + 'ip_catalog/' + ip_name
    p=subprocess.Popen(cmd,shell=True)
    return_code=p.wait()  # waiting for end 

    cmd = 'mv ./' + project_name + '/' + ip_name + ' ' + outPath + 'ip_catalog/'
    p=subprocess.Popen(cmd,shell=True)
    return_code=p.wait()  # waiting for end 