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
        ("Hebbian", base_dir / "Hebbian_curve.csv"),
        ("PseudoInverse", base_dir / "PseudoInverse_curve.csv"),
        ("Storkey", base_dir / "Storkey_curve.csv"),
    ]

    for label, path in files:
        if not path.exists():
            raise FileNotFoundError(f"Missing required file: {path}")

        df = pd.read_csv(path)
        plt.plot(df["alpha"], df["P_success"], label=label)
    
    plt.title("Probability of retrieval success vs saturation")
    plt.xlabel("Saturation")
    plt.ylabel("Probability of retrieval success")
    plt.legend()
    plt.show()
        