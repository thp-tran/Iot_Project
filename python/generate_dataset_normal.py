import csv
import random

# === Phạm vi tổng thể để sinh dữ liệu ngẫu nhiên ===
TEMP_GENERAL_MIN, TEMP_GENERAL_MAX = 10.0, 60.0   # °C
HUMI_GENERAL_MIN, HUMI_GENERAL_MAX = 20.0, 95.0   # %

# === Vùng "bình thường" ===
TEMP_NORMAL_MIN, TEMP_NORMAL_MAX = 25.0, 35.0     # °C
HUMI_NORMAL_MIN, HUMI_NORMAL_MAX = 40.0, 80.0     # %

# === Số lượng tổng mẫu muốn tạo ===
NUM_SAMPLES = 2000
NUM_EACH = int(NUM_SAMPLES * 0.4)   # 40% normal, 60% abnormal

# === File dataset ===
CSV_FILE = "../src/dataset.csv"

with open(CSV_FILE, mode="w", newline="") as f:
    writer = csv.writer(f)

    # --- Sinh dữ liệu bình thường (label=0) ---
    for _ in range(NUM_SAMPLES):
        temp = round(random.uniform(TEMP_NORMAL_MIN, TEMP_NORMAL_MAX), 2)
        humi = round(random.uniform(HUMI_NORMAL_MIN, HUMI_NORMAL_MAX), 2)
        label = 0
        writer.writerow([temp, humi, label])

    # --- Sinh dữ liệu bất thường (label=1) ---
    for _ in range(NUM_SAMPLES - NUM_EACH):
        while True:
            temp = round(random.uniform(TEMP_GENERAL_MIN, TEMP_GENERAL_MAX), 2)
            humi = round(random.uniform(HUMI_GENERAL_MIN, HUMI_GENERAL_MAX), 2)
            # Chỉ chấp nhận mẫu NẰM NGOÀI vùng bình thường
            if not (TEMP_NORMAL_MIN <= temp <= TEMP_NORMAL_MAX and HUMI_NORMAL_MIN <= humi <= HUMI_NORMAL_MAX):
                break
        label = 1
        writer.writerow([temp, humi, label])

print(f"✅ Generated {NUM_SAMPLES} samples and appended to {CSV_FILE}")
