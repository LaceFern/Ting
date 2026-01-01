
import paramiko
import threading
import time
import sys

user = "zxy"
ssh_key_file = "/home/zxy/.ssh/id_rsa"
base = "/home/zxy/nfs/DSM_prj/FPGA-DSM/soft_multi_node"



server_machine = ["192.168.189.7", "192.168.189.9", "192.168.189.12", "192.168.189.13", "192.168.189.14"]
fpga_pcie_bus = ["0x43", "0x1a", "0x1a", "0x1a", "0x1a"]
fpga_mac = ["01:01:01:01:01:01", "03:03:03:03:03:03", "06:06:06:06:06:06", "07:07:07:07:07:07", "08:08:08:08:08:08"]

program_name = "simulate"

app_thread_num = [24]
sys_thread_num = [8]
read_ratio = [90]
sharing_ratio = [100]



node_ready = [False] * len(server_machine)
node_running = [True] * len(server_machine)  # Flag to indicate if each node's process is still running
lock = threading.Lock()
channel_stdout = [None] * len(server_machine)
channel_stdin = [None] * len(server_machine)

latencies = [0.0] * len(server_machine)  # Placeholder for latencies
throughputs = [0.0] * len(server_machine)  # Placeholder for throughputs

def ssh_connect(ip, user):
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    ssh.connect(ip, 22, user, None, key_filename=ssh_key_file)
    return ssh


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

def server_run(ssh, atn_i, stn_i, rr_i, sr_i, bus_i, i):
    """
    Execute the command on a server.
    """
    global node_running, channel_stdout, channel_stdin
    cmd = (
        "cd {0}/build && sudo ./{1} "
        "--node_num {2} --node_id {3} "
        "--appt_num {4} --syst_num {5} "
        "--read_ratio {6} --shared_ratio {7} "
        "--fpga_pci_bus {8} --macs {9} "
        "--warmup_type 2 --test_type 2".format(base, program_name, len(server_machine), i, atn_i, stn_i, rr_i, sr_i, bus_i, ",".join(fpga_mac))
    )
    print(f"Running command on node {i}: {cmd}")
    stdin, stdout, stderr = ssh.exec_command(cmd)
    channel_stdin[i] = stdin  # Save stdin for later use
    channel_stdout[i] = stdout


if __name__ == '__main__':
    ssh_server = [ssh_connect(server_machine[i], user) for i in range(len(server_machine))]

    for atn_i in app_thread_num:
        for stn_i in sys_thread_num:
            for rr_i in read_ratio:
                for sr_i in sharing_ratio:
                    server_cmd_list = [threading.Thread(target=server_run, args=(ssh_server[i], atn_i, stn_i, rr_i, sr_i, fpga_pcie_bus[i], i))  for i in range(len(ssh_server))]            
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

                    print(
                        "Finish: app_thread_num = {0}, sys_thread_num = {1}, read_ratio = {2}, sharing_ratio = {3}, "
                        "avg op Latency (us) = {4:.2f}, total op throughput (Mqps) = {5:.2f}\n".format(
                            atn_i, stn_i, rr_i, sr_i, avg_latency, total_throughput
                        )
                    )
