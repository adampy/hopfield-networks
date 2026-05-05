import argparse
from pathlib import Path
import pandas as pd
import matplotlib.pyplot as plt

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="View experiment 1 results")
    parser.add_argument("folder", type=str, help="Input folder")
    args = parser.parse_args()

    # Load in each data file
    base_dir = Path(args.folder)
    files: list[tuple[str, Path]] = [
        ("Hebbian", base_dir / "Hebbian_capacity.csv"),
        ("PseudoInverse", base_dir / "PseudoInverse_capacity.csv"),
        ("Storkey", base_dir / "Storkey_capacity.csv"),
    ]

    for label, path in files:
        if not path.exists():
            raise FileNotFoundError(f"Missing required file: {path}")

        df = pd.read_csv(path)
        plt.plot(df["networkSize"], df["alpha_c"], label=label)
    
    plt.xlabel("Network Size")
    plt.ylabel("Critical Saturation")
    plt.legend()
    plt.show()
        