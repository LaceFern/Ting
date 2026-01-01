
import paramiko
import threading
import time
import sys
import subprocess
import os

user = "zxy"
ssh_key_file = "/home/zxy/.ssh/id_rsa"
base = "/home/zxy/nfs/DSM_prj/FPGA-DSM/soft_multi_node"


server_machine = ["192.168.189.11", "192.168.189.13", "192.168.189.14"]
server_type = ["RC", "D", "O"]
fpga_pcie_bus = ["0x1e", "0x1a", "0x1a"]
fpga_mac = ["05:05:05:05:05:05", "07:07:07:07:07:07", "08:08:08:08:08:08"]



program_name = "highpara_benchmark"


app_thread_num = [1, 2, 4, 8, 12, 16, 20, 24]
blksize_MB = [1024]
sys_thread_num = [8] # no use
read_ratio = [50]
sharing_ratio = [0] # 0, 20, 40, 60, 80, 100
result_dir = "/home/zxy/evaluation_3_ting_v1"


node_ready = [False] * len(server_machine)
node_running = [True] * len(server_machine)  # Flag to indicate if each node's process is still running
lock = threading.Lock()
channel_stdout = [None] * len(server_machine)
channel_stdin = [None] * len(server_machine)

latencies = [0.0] * len(server_machine)  # Placeholder for latencies
throughputs = [0.0] * len(server_machine)  # Placeholder for throughputs

def reset_run_state():
    global node_ready, node_running, latencies, throughputs, channel_stdout, channel_stdin
    with lock:
        node_ready = [False] * len(server_machine)
        node_running = [True] * len(server_machine)
        latencies = [0.0] * len(server_machine)
        throughputs = [0.0] * len(server_machine)
        channel_stdout = [None] * len(server_machine)
        channel_stdin = [None] * len(server_machine)

def ssh_connect(ip, user):
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    ssh.connect(ip, 22, user, None, key_filename=ssh_key_file)
    return ssh

def node_init_4_page(ssh):
    stdin, stdout, stderr = ssh.exec_command(
        "cd {0} && sudo bash ./hugepage.sh".format(base + "/scripts")
    )
    str1 = stdout.read().decode('utf-8')
    str2 = stderr.read().decode('utf-8')
    print(str1)
    print(str2)


def monitor_all_nodes(ssh_list):
    """
    Monitor all nodes' stdout in a single thread to check for synchronization points, program completion,
    and collect & print non-synchronization output.
    """
    global node_ready, node_running, channel_stdout, latencies, throughputs

    while any(node_running):  # Continue until all nodes finish execution
        for i, stdout in enumerate(channel_stdout):
            if not node_running[i]:  # Skip finished nodes
                continue

            if stdout.channel.recv_ready():
                line = stdout.channel.recv(1024).decode("utf-8")  # Use `channel.recv` on the underlying channel
                print(f"[Node {i}] {line.strip()}", flush=True)  # for debug, may broken the synchronization message and cause stalling            

                if "Press Space to continue" in line:
                    with lock:
                        node_ready[i] = True

                elif "Execution Finished" in line:  # Replace with actual end-of-program output
                    with lock:
                        node_running[i] = False

                if "avg op latency" in line and "total op throughput" in line:
                    try:
                        latency = float(line.split("avg op latency = ")[1].split(" us")[0])
                        throughput = float(line.split("total op throughput = ")[1].split(" Mqps")[0])
                        with lock:
                            latencies[i] = latency
                            throughputs[i] = throughput
                    except Exception as e:
                        print(f"Error parsing latency/throughput for node {i}: {e}", flush=True)


        time.sleep(0.1)  # Avoid busy-looping



def wait_for_all_ready():
    """
    Wait until all nodes are ready for synchronization.
    """
    global node_running
    while all(node_running):
        with lock:
            if all(node_ready):
                print("All nodes are ready for synchronization.")
                break
        time.sleep(0.1)


def send_space_to_all(ssh_list):
    """
    Send a space character to all nodes' stdin for synchronization.
    """
    for i, ssh in enumerate(ssh_list):
        print(f"Sending space to node {i}...")
        stdin = channel_stdin[i]
        stdin.write(' ')
        stdin.flush()

def reset_ready_flags():
    """
    Reset the node_ready flags for the next synchronization round.
    """
    global node_ready
    with lock:
        node_ready = [False] * len(node_ready)

def server_run(ssh, atn_i, stn_i, rr_i, sr_i, bus_i, i, blk_i):
    """
    Execute the command on a server.
    """
    global node_running, channel_stdout, channel_stdin

    if server_type[i] == "R":
        cmd = (
            "cd {0}/build && sudo ./{1} "
            "--no_node {2} --no_thread {3} --node_id {4} --result_dir {5} "
            "--locality {6} --read_ratio {7} --shared_ratio {8} "
            "--fpga_pci_bus {9} --macs {10} --blksize_MB {11} "
            "--is_request 1 --request_rw 1 --is_cache 0 --cache_rw 0 --is_home 0 --home_node_id 1".format(
                base, program_name, 
                len(server_machine), atn_i, i, result_dir, 
                0, rr_i, sr_i,
                bus_i, ",".join(fpga_mac), blk_i
            )
        )
    elif server_type[i] == "D":
        cmd = (
            "cd {0}/build && sudo ./{1} "
            "--no_node {2} --no_thread {3} --node_id {4} --result_dir {5} "
            "--locality {6} --read_ratio {7} --shared_ratio {8} "
            "--fpga_pci_bus {9} --macs {10}  --blksize_MB {11} "
            "--is_request 0 --request_rw 0 --is_cache 0 --cache_rw 0 --is_home 1 --home_node_id 1".format(
                base, program_name, 
                len(server_machine), atn_i, i, result_dir, 
                0, rr_i, sr_i, 
                bus_i, ",".join(fpga_mac), blk_i
            )
        )
    elif server_type[i] == "C":
        cmd = (
            "cd {0}/build && sudo ./{1} "
            "--no_node {2} --no_thread {3} --node_id {4} --result_dir {5} "
            "--locality {6} --read_ratio {7} --shared_ratio {8} "
            "--fpga_pci_bus {9} --macs {10}  --blksize_MB {11} "
            "--is_request 0 --request_rw 0 --is_cache 1 --cache_rw 0 --is_home 0 --home_node_id 1".format(
                base, program_name, 
                len(server_machine), atn_i, i, result_dir, 
                0, rr_i, sr_i, 
                bus_i, ",".join(fpga_mac), blk_i
            )
        )
    elif server_type[i] == "RC":
        cmd = (
            "cd {0}/build && sudo ./{1} "
            "--no_node {2} --no_thread {3} --node_id {4} --result_dir {5} "
            "--locality {6} --read_ratio {7} --shared_ratio {8} "
            "--fpga_pci_bus {9} --macs {10}  --blksize_MB {11} "
            "--is_request 1 --request_rw 0 --is_cache 1 --cache_rw 0 --is_home 0 --home_node_id 1".format(
                base, program_name, 
                len(server_machine), atn_i, i, result_dir, 
                0, rr_i, sr_i, 
                bus_i, ",".join(fpga_mac), blk_i
            )
        )
    else:
        cmd = (
            "cd {0}/build && sudo ./{1} "
            "--no_node {2} --no_thread {3} --node_id {4} --result_dir {5} "
            "--locality {6} --read_ratio {7} --shared_ratio {8} "
            "--fpga_pci_bus {9} --macs {10}  --blksize_MB {11} "
            "--is_request 0 --request_rw 0 --is_cache 0 --cache_rw 0 --is_home 0 --home_node_id 1".format(
                base, program_name, 
                len(server_machine), atn_i, i, result_dir, 
                0, rr_i, sr_i, 
                bus_i, ",".join(fpga_mac), blk_i
            )
        ) 

    print(f"Running command on node {i}: {cmd}")
    stdin, stdout, stderr = ssh.exec_command(cmd)
    channel_stdin[i] = stdin  # Save stdin for later use
    channel_stdout[i] = stdout


if __name__ == '__main__':
    ssh_server = [ssh_connect(server_machine[i], user) for i in range(len(server_machine))]

    if(True):
        print("init node (page) starts")
        server_list = [threading.Thread(target=node_init_4_page,
                                args=(ssh_server[i],))  for i in range(len(ssh_server))]            
        for i in range(len(ssh_server)):
            server_list[i].start()
        for i in range(len(ssh_server)):
            server_list[i].join()
        print("init node (page) ends")

    for atn_i in app_thread_num:
        for stn_i in sys_thread_num:
            for rr_i in read_ratio:
                for sr_i in sharing_ratio:
                    for blk_i in blksize_MB:

                        reset_run_state()

                        server_cmd_list = [threading.Thread(target=server_run, args=(ssh_server[i], atn_i, stn_i, rr_i, sr_i, fpga_pcie_bus[i], i, blk_i))  for i in range(len(ssh_server))]            
                        for i in range(len(ssh_server)):
                            server_cmd_list[i].start()
                        time.sleep(2)

                        monitor_thread = threading.Thread(target=monitor_all_nodes, args=(ssh_server,))
                        monitor_thread.start()

                        while all(node_running):  # Repeat until all nodes complete execution
                            wait_for_all_ready()
                            if all(node_running):
                                send_space_to_all(ssh_server)
                                reset_ready_flags()  # Reset for the next synchronization round

                        monitor_thread.join()  # Wait for the monitor thread to finish
                        for i in range(len(ssh_server)):
                            server_cmd_list[i].join()


                        avg_latency = sum(latencies) / len(latencies)
                        total_throughput = sum(throughputs)

                        finish_msg = (
                            "Finish: app_thread_num = {0}, sys_thread_num = {1}, read_ratio = {2}, sharing_ratio = {3}, "
                            "avg op Latency (us) = {4:.2f}, total op throughput (Mqps) = {5:.2f}"
                        ).format(atn_i, stn_i, rr_i, sr_i, avg_latency, total_throughput)

                        # Console print (preserve original behavior)
                        print(finish_msg)

                        os.makedirs(result_dir, exist_ok=True)
                        outfile = os.path.join(result_dir, "end_to_end_pyresult.txt")
                        write_header = (not os.path.exists(outfile)) or (os.path.getsize(outfile) == 0)
                        with open(outfile, "a", encoding="utf-8") as f:
                            if write_header:
                                f.write("app_thread_num\tsys_thread_num\tread_ratio\tsharing_ratio\tavg_latency_us\ttotal_throughput_Mqps\n")
                            f.write("{0}\t{1}\t{2}\t{3}\t{4:.2f}\t{5:.2f}".format(atn_i, stn_i, rr_i, sr_i, avg_latency, total_throughput) + "\n")
                        
                        time.sleep(10)