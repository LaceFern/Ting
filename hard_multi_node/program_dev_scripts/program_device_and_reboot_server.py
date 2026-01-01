import paramiko
import subprocess
import threading
import os
import time



target_machine = ["192.168.189.11", "192.168.189.13", "192.168.189.14"] #, "192.168.189.12", "192.168.189.14"
target_base = "/home/zxy/nfs/DSM_prj/FPGA-DSM/hard_multi_node/program_dev_scripts/"
PREFERRED_DRIVER_PATHS = [
    "/home/amax/qdma_driver/src/qdma-pf.ko",
]
bit_file = "/home/zxy/2024_vivado_pro/DSM/bitstream/StaticU280Top.bit"
ltx_file = "/home/zxy/2024_vivado_pro/DSM/bitstream/StaticU280Top.ltx"
if_reboot = True

cs_server_commands = [
    "cs_server -s TCP::3051 -I 60&",
    "cs_server -s TCP::3052 -I 60&",
    "cs_server -s TCP::3053 -I 60&",
    "cs_server -s TCP::3054 -I 60&",
    "cs_server -s TCP::3055 -I 60&",
    "cs_server -s TCP::3056 -I 60&",
    "cs_server -s TCP::3057 -I 60&",
    "cs_server -s TCP::3058 -I 60&"
]

vivado_commands = [
    "vivado -mode tcl -nolog -nojournal -source ./program_amax1.tcl -tclargs {0} {1}".format(bit_file, ltx_file),
    "vivado -mode tcl -nolog -nojournal -source ./program_amax2.tcl -tclargs {0} {1}".format(bit_file, ltx_file),
    "vivado -mode tcl -nolog -nojournal -source ./program_amax3.tcl -tclargs {0} {1}".format(bit_file, ltx_file),
    "vivado -mode tcl -nolog -nojournal -source ./program_amax4.tcl -tclargs {0} {1}".format(bit_file, ltx_file),
    "vivado -mode tcl -nolog -nojournal -source ./program_amax5.tcl -tclargs {0} {1}".format(bit_file, ltx_file),
    "vivado -mode tcl -nolog -nojournal -source ./program_amax6.tcl -tclargs {0} {1}".format(bit_file, ltx_file),
    "vivado -mode tcl -nolog -nojournal -source ./program_amax7.tcl -tclargs {0} {1}".format(bit_file, ltx_file),
    "vivado -mode tcl -nolog -nojournal -source ./program_amax8.tcl -tclargs {0} {1}".format(bit_file, ltx_file)
]
user = "zxy"
ssh_key_file = "/home/zxy/.ssh/id_rsa"
def calculate_machine_number(ip):
    return int(ip.split('.')[-1]) - 7  # Adjust this logic as per your needs


burn_status = {}

def burn_fpga(ip, machine_number):
    """Run FPGA programming commands for a single machine"""
    global burn_status

    try:
        print(f"[{ip}] Starting cs_server...")
        subprocess.run(cs_server_commands[machine_number], shell=True, executable="/bin/bash", check=True)

        print(f"[{ip}] Programming FPGA...")
        subprocess.run(vivado_commands[machine_number], shell=True, executable="/bin/bash", check=True)

        print(f"[{ip}] Programming completed successfully.")
        burn_status[ip] = True  # Mark success
    except subprocess.CalledProcessError as e:
        print(f"[{ip}] Error during programming: {e}")
        burn_status[ip] = False  # Mark failure


def program_device(target_ips):
    """Launch programming commands for all devices in parallel"""


    subprocess.run("pkill -9 -f {0}".format("cs_server"), shell=True, executable="/bin/bash")
    subprocess.run("pkill -9 -f {0}".format("vivado"), shell=True, executable="/bin/bash")

    global burn_status

    threads = []
    for ip in target_ips:
        machine_number = calculate_machine_number(ip)  # Assume IP last segment -7 gives the dev index
        burn_status[ip] = False  # Initialize status as not completed
        t = threading.Thread(target=burn_fpga, args=(ip, machine_number))
        threads.append(t)
        t.start()

    for t in threads:
        t.join()


def ping_host(ip):
    """Ping the host to check if it's back online after reboot"""
    while True:
        response = os.system(f"ping -c 1 {ip} > /dev/null 2>&1")
        if response == 0:
            print(f"{ip} is back online!")
            break
        else:
            print(f"Waiting for {ip} to come online...")
            time.sleep(5)  # Wait for 5 seconds before retrying


def ssh_connect(ip, user):
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    try:
        ssh.connect(ip, 22, user, None, key_filename=ssh_key_file, timeout=10)  # Set 10 second timeout
        return ssh
    except paramiko.ssh_exception.NoValidConnectionsError as e:
        print(f"[ERROR] Unable to connect to {ip}: No valid connections. Error: {e}")
    except BrokenPipeError:
        print(f"[ERROR] Connection to {ip} broken (Broken pipe). Retrying...")
    except paramiko.SSHException as e:
        print(f"[ERROR] General SSH error with {ip}: {e}")
    except Exception as e:
        print(f"[ERROR] Unexpected error while connecting to {ip}: {e}")
    return None


def check_ssh_connection(ip, user):
    try:
        ssh = ssh_connect(ip, user)
        if ssh:
            ssh.close() 
            return True
    except Exception as e:
        print(f"[WARNING] SSH check failed for {ip}: {e}")
    return False  




if __name__ == "__main__":
    program_device(target_machine)

    if if_reboot:
        for machine in target_machine:
            if burn_status[machine]:  # Only reboot if programming succeeded
                ssh = None
                try:
                    ssh = ssh_connect(machine, user)
                    print(f"Rebooting machine {machine}...")
                    stdin, stdout, stderr = ssh.exec_command("sudo reboot")
                    print(stdout.read().decode('utf-8'))
                    print(stderr.read().decode('utf-8'))
                except paramiko.ssh_exception.SSHException as e:
                    print(f"SSH connection error with {machine}: {e}")
                except Exception as e:
                    print(f"Failed to reboot machine {machine}: {e}")
                finally:
                    if ssh:
                        ssh.close()
            else:
                print(f"Skipping reboot for {machine} due to burn failure.")

        print("Waiting for machines to reboot...")
        time.sleep(10)  # Adjust this time as needed


        for machine in target_machine:
            while not check_ssh_connection(machine, user):  # Wait until the machine is back online via SSH
                print(f"{machine} is not online yet (SSH check), retrying...")
                time.sleep(5)  # Retry every 5 seconds
            print(f"{machine} is back online via SSH!")