import xml.etree.ElementTree as ET
import pandas as pd
import os

ORK_FILE = "rocket.ork"
OUTPUT_DIR = "rocket_simulations"
KEEP_COLUMNS = ["timestamp", "Air pressure", "Total acceleration", "Altitude"]


def extract_flight_data(ork_path, keep_columns, output_dir):
    os.makedirs(output_dir, exist_ok=True)

    tree = ET.parse(ork_path)
    root = tree.getroot()

    simulations = root.findall(".//simulation")

    if not simulations:
        print("No simulations")
        return

    for sim_i, sim in enumerate(simulations, start=1):
        sim_name = sim.findtext("name", f"Simulation_{sim_i}")

        branches = sim.findall(".//databranch")
        if not branches:
            print(f"No databranch found in {sim_name}")
            continue

        for branch_i, branch in enumerate(branches, start=1):
            branch_name = branch.attrib.get("name", f"Branch_{branch_i}")

            col_names = [c.strip() for c in branch.attrib["types"].split(",")]
            data_rows = [dp.text.strip().split(",") for dp in branch.findall("datapoint")]

            if not data_rows:
                print(f"No datapoints found in {sim_name} / {branch_name}")
                continue

            df = pd.DataFrame(data_rows, columns=col_names).apply(pd.to_numeric, errors="ignore")

            if "Time" in df.columns:
                df.rename(columns={"Time": "timestamp"}, inplace=True)

            keep_cols = []
            for c in keep_columns:
                matches = [col for col in df.columns if c.lower() in col.lower()]
                if matches:
                    keep_cols.append(matches[0])
                else:
                    print(f"Column not found: {c} in {sim_name}/{branch_name}")

            if not keep_cols:
                print(f"No matching columns found for {sim_name}/{branch_name}")
                continue

            cleaned = df[keep_cols]

            safe_sim = sim_name.replace(" ", "_")
            safe_branch = branch_name.replace(" ", "_")
            out_path = os.path.join(output_dir, f"{safe_sim}__{safe_branch}.csv")

            cleaned.to_csv(out_path, index=False)
            print(f"Saved {sim_name}/{branch_name} → {out_path} ({len(cleaned)} rows)")


if __name__ == "__main__":
    extract_flight_data(ORK_FILE, KEEP_COLUMNS, OUTPUT_DIR)
