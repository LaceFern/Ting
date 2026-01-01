import os
import re
import csv

base_dir = "/home/zxy/2024_vivado_pro/DSM/test_pro_multi_node_2/rc4ml_static_u280/rc4ml_static_u280.srcs/dynamic_sources/ip_catalog"

output_file = "ip_resource_summary.csv"

fields = ["IP_Name", "BRAM_18K", "DSP", "FF", "LUT", "URAM", "Latency_cycles"]

total_pattern = re.compile(
    r"\|\s*Total\s*\|\s*(\d*)\s*\|\s*(\d*)\s*\|\s*(\d*)\s*\|\s*(\d*)\s*\|\s*(\d*)\s*\|"
)

latency_pattern = re.compile(
    r"Latency\s*\(cycles\)[\s\S]*?\|\s*(\d+)\s*\|\s*(\d+)\s*\|",
    re.IGNORECASE
)

rows = []

for ip_dir in os.listdir(base_dir):
    rpt_path = os.path.join(base_dir, ip_dir, "syn", "report", f"{ip_dir}_csynth.rpt")
    if not os.path.isfile(rpt_path):
        continue

    with open(rpt_path, "r", encoding="utf-8", errors="ignore") as f:
        content = f.read()

    match = total_pattern.search(content)
    if match:
        bram, dsp, ff, lut, uram = match.groups()
        row = {
            "IP_Name": ip_dir,
            "BRAM_18K": bram or "0",
            "DSP": dsp or "0",
            "FF": ff or "0",
            "LUT": lut or "0",
            "URAM": uram or "0",
        }
    else:
        row = {
            "IP_Name": ip_dir,
            "BRAM_18K": "N/A",
            "DSP": "N/A",
            "FF": "N/A",
            "LUT": "N/A",
            "URAM": "N/A",
        }

    lat_match = latency_pattern.search(content)
    if lat_match:
        lat_min, lat_max = lat_match.groups()
        if lat_min == lat_max:
            row["Latency_cycles"] = lat_min
        else:
            row["Latency_cycles"] = f"{lat_min}-{lat_max}"
    else:
        row["Latency_cycles"] = "N/A"

    rows.append(row)

with open(output_file, "w", newline="", encoding="utf-8") as tsvfile:
    writer = csv.DictWriter(tsvfile, fieldnames=fields, delimiter="\t")
    writer.writeheader()
    writer.writerows(rows)

print(f"✅ Extraction completed, {len(rows)} IPs total, results saved to {output_file}")