import os
import subprocess
import threading
import socket
import re
from tabulate import tabulate
import json
import re

vpro_path = '/home/zxy/2024_vivado_pro/DSM/test_pro_multi_node_2/'
vpro_name = 'rc4ml_static_u280'
vpro_reconfig_flag = True #False #True
vpro_reconfig_core_name = 'U280CoreReconfig' # 'U280CoreBase' 'U280CoreReconfig'
vpro_runTCL_flag = True #True
hpro_name = 'highPara'

rc4ml_static_repo = '/home/zxy/2024_vivado_pro/chisel_dynamic/rc4ml_static'
chisel_top = '/home/zxy/nfs/DSM_prj/FPGA-DSM/hard_multi_node/chisel/dynamic/testTop.scala'

vpro = vpro_path + vpro_name + '/' + vpro_name + '.xpr'
outPath = vpro_path + vpro_name + '/' + vpro_name + '.srcs' + '/' + 'dynamic_sources/'
cmd = 'mkdir -p ' + outPath + 'add_sources/' + hpro_name + '_chiselTop'
print(cmd)
p=subprocess.Popen(cmd,shell=True)
cmd = 'mkdir -p ' + outPath + 'add_sources/' + hpro_name + '_chiselTop/ip'
print(cmd)
p=subprocess.Popen(cmd,shell=True)
cmd = 'mkdir -p ' + outPath + 'add_sources/' + hpro_name + '_chiselTop/sv'
print(cmd)
p=subprocess.Popen(cmd,shell=True)

def replace_dest_ip_repo_path(data):
    for key, value in data.items():
        if isinstance(value, dict):
            replace_dest_ip_repo_path(value)
        elif key == "destIPRepoPath":
            data[key] = outPath + 'add_sources/' + hpro_name + '_chiselTop/ip'
        elif key == "destSrcPath":
            data[key] = outPath + 'add_sources/' + hpro_name + '_chiselTop/sv'

config = rc4ml_static_repo + '/' + 'config.json'

with open(config, 'r') as file:
    data = json.load(file)

replace_dest_ip_repo_path(data)

with open(config, 'w') as file:
    json.dump(data, file, indent=4)

def execute_command_in_directory(command, directory):
    original_directory = os.getcwd()
    try:
        os.chdir(directory)
        result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
        print("Command output:", result.stdout)
        print("Command error:", result.stderr)   
    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        os.chdir(original_directory)
    
    ip_names = []
    create_ip_tcls = []
    set_property_tcls = []
    for line in result.stdout.splitlines():
        if line.startswith("create_ip"):
            create_ip_tcls.append(line)
            match = re.search(r'module_name\s+(\w+)', line)
            if match:
                ip_name = match.group(1)
                ip_names.append(ip_name)
        if line.startswith("set_property"):
            set_property_tcls.append(line)
    return ip_names, create_ip_tcls, set_property_tcls

cmd = 'ln -sf ' + chisel_top + ' ' + rc4ml_static_repo + '/static/src/u280/U280DynamicGrayBox.scala'
print(cmd)
p=subprocess.Popen(cmd,shell=True)

cmd = 'python3 postElaborating.py static U280DynamicGreyBox -t -p'
print(cmd)
ip_names, create_ip_tcls, set_property_tcls = execute_command_in_directory(cmd, rc4ml_static_repo)

def find_files_with_prefix_and_suffix(directory, prefix, suffix):
    matching_files = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.startswith(prefix) and file.endswith(suffix):
                matching_files.append(os.path.join(root, file))
    return matching_files

with open(outPath + 'complete_vpro_with_chisel.tcl','w') as f:
    f.write('set_param general.maxThreads 2\n')
    f.write('open_project ' + vpro + '\n')

    for ip_idx in range(len(ip_names)):
        ip_name = ip_names[ip_idx]
        create_ip_tcl = create_ip_tcls[ip_idx]
        set_property_tcl = set_property_tcls[ip_idx]

        if ip_name != "DebugBridge" and ip_name != "DebugBridgeBase" :
            
            directory_path = outPath + 'add_sources/' + hpro_name + '_chiselTop/ip'
            matching_xci = find_files_with_prefix_and_suffix(directory_path, ip_name, ".xci")
            matching_xcix = find_files_with_prefix_and_suffix(directory_path, ip_name, ".xcix")
            for file in matching_xci + matching_xcix:
                f.write('remove_files ' + file + '\n')
                if vpro_reconfig_flag:
                    f.write('remove_files [get_files -of_objects [get_reconfig_modules ' + vpro_reconfig_core_name+ '] ' + file + ']\n')
                f.write('file delete -force ' + file + '\n')

            f.write(create_ip_tcl + ' -dir ' + outPath + 'add_sources/' + hpro_name + '_chiselTop/ip\n')

            f.write(set_property_tcl + '\n')

    sv = outPath + 'add_sources/' + hpro_name + '_chiselTop/sv/' + 'U280DynamicGreyBox.sv'
    if vpro_reconfig_flag:
        f.write('add_files ' + sv + '  -of_objects [get_reconfig_modules ' + vpro_reconfig_core_name+ ']' + '\n')
    else:
        f.write('add_files ' + sv + '\n')

f.close()

if vpro_runTCL_flag:
    cmd = 'vivado -mode batch -source ' + outPath + 'complete_vpro_with_chisel.tcl'
    p=subprocess.Popen(cmd,shell=True)
    return_code=p.wait()  # waiting for end 


with open(outPath + 'config_vpro_with_chisel.tcl','w') as f:
    f.write('set_param general.maxThreads 2\n')
    f.write('open_project ' + vpro + '\n')

    for ip_idx in range(len(ip_names)):
        ip_name = ip_names[ip_idx]
        if ip_name != "DebugBridge" and ip_name != "DebugBridgeBase" :
            directory_path = outPath + 'add_sources/' + hpro_name + '_chiselTop/ip'
            matching_xci = find_files_with_prefix_and_suffix(directory_path, ip_name, ".xci")
            matching_xcix = find_files_with_prefix_and_suffix(directory_path, ip_name, ".xcix")
            if matching_xci != []:
                xci = matching_xci[0]
                if vpro_reconfig_flag:
                    f.write('set_property GENERATE_SYNTH_CHECKPOINT 0 [get_files ' + xci + ']' + '\n')
                    f.write('move_files -of_objects [get_reconfig_modules ' + vpro_reconfig_core_name+ '] [get_files ' + xci + ']\n')
            elif matching_xcix != []:
                xcix = matching_xcix[0]
                pattern = r'(.+)/([^/]+)\.xcix'
                replacement = r'\1/\2/\2.xci'
                xci = re.sub(pattern, replacement, xcix)
                if vpro_reconfig_flag:
                    f.write('set_property GENERATE_SYNTH_CHECKPOINT 0 [get_files ' + xci + ']' + '\n')
                    f.write('move_files -of_objects [get_reconfig_modules ' + vpro_reconfig_core_name+ '] [get_files ' + xci + ']\n')
f.close()

if vpro_runTCL_flag:
    cmd = 'vivado -mode batch -source ' + outPath + 'config_vpro_with_chisel.tcl'
    p=subprocess.Popen(cmd,shell=True)
    return_code=p.wait()  # waiting for end 


cmd = 'rm -rf *.log *.jou'
p=subprocess.Popen(cmd,shell=True)
return_code=p.wait()  # waiting for end 
cmd = 'rm -rf .Xil'
p=subprocess.Popen(cmd,shell=True)
return_code=p.wait()  # waiting for end 
