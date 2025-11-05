#!/usr/bin/env python3
"""
Flight data analyzer
Reads a telemetry CSV/TSV and plots key quantities:
- Vertical acceleration (m/s²)
- Pressure and baro-derived altitude
- FSM states (colored bands)
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

INPUT_FILE = "drone_test_results/drone_test_1.csv"   # or .csv
DELIMITER = ","                 # use "\t" for tab-separated, "," for CSV
SAMPLE_RATE_HZ = 50              # optional: for time axis normalization

R_CONST = 287.052874
G_CONST = 9.80665
L_RATE = 0.0065
ALPHA = (R_CONST * L_RATE) / G_CONST
DIFF_C_K = 273.15

print(f"Loading {INPUT_FILE} ...")
df = pd.read_csv(INPUT_FILE, sep=DELIMITER)

# Optional: verify column names
expected_cols = [
    "seq","timestamp","acc_x","acc_y","acc_z",
    "gyro_x","gyro_y","gyro_z","pressure","altitude",
    "fsm_state","fsm_state_str"
]
missing = [c for c in expected_cols if c not in df.columns]
if missing:
    raise ValueError(f"Missing columns in input: {missing}")

# Compute acceleration magnitude (mg → m/s²)
df["acc_z_m_s2"] = df["acc_z"] / 1000.0 * G_CONST
df["acc_mag_m_s2"] = np.sqrt(df["acc_x"]**2 + df["acc_y"]**2 + df["acc_z"]**2) / 1000.0 * G_CONST

# Compute derived altitude from pressure
ground_pres = df["pressure"].iloc[0]
ground_temp_k = 288.15  # or use real temp if available
df["alt_from_pres_m"] = (ground_temp_k / L_RATE) * (1 - (df["pressure"] / ground_pres) ** ALPHA)

# Compute moving averages for smoothing
df["alt_avg"] = df["alt_from_pres_m"].rolling(window=8, min_periods=1).mean()
df["acc_z_avg"] = df["acc_z_m_s2"].rolling(window=8, min_periods=1).mean()

# Create a time axis (ms → s)
t0 = df["timestamp"].iloc[0]
df["time_s"] = (df["timestamp"] - t0) / 1000.0

# ============================================================
# === PLOTS ==================================================
# ============================================================
fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)

# ---- 1) Vertical acceleration
axs[0].plot(df["time_s"], df["acc_z_m_s2"], label="acc_z (m/s²)", linewidth=0.8)
axs[0].plot(df["time_s"], df["acc_z_avg"], label="acc_z avg", linewidth=1.5)
axs[0].set_ylabel("acc_z (m/s²)")
axs[0].legend()
axs[0].grid(True)

# ---- 2) Pressure
axs[1].plot(df["time_s"], df["pressure"], label="pressure (hPa)", color='tab:orange')
axs[1].set_ylabel("Pressure (hPa)")
axs[1].grid(True)

# ---- 3) Altitude
axs[2].plot(df["time_s"], df["alt_avg"], label="alt (baro)", color='tab:green')
axs[2].set_ylabel("Altitude (m)")
axs[2].set_xlabel("Time (s)")
axs[2].grid(True)

# ---- FSM state coloring (on all subplots)
states = df["fsm_state_str"].unique()
colors = plt.cm.tab10(np.linspace(0, 1, len(states)))
for ax in axs:
    for s, c in zip(states, colors):
        mask = df["fsm_state_str"] == s
        if mask.any():
            ax.fill_between(df["time_s"], ax.get_ylim()[0], ax.get_ylim()[1],
                            where=mask, color=c, alpha=0.1, label=s)

# Deduplicate legends
for ax in axs:
    handles, labels = ax.get_legend_handles_labels()
    by_label = dict(zip(labels, handles))
    ax.legend(by_label.values(), by_label.keys(), loc='upper right', fontsize='x-small')

plt.suptitle("Flight Data Analysis")
plt.tight_layout()
plt.show()

# ============================================================
# === BASIC STATS ============================================
# ============================================================
print("\n--- Summary ---")
print(df[["acc_z_m_s2","alt_avg","pressure"]].describe())
print(f"Detected FSM states: {list(states)}")
