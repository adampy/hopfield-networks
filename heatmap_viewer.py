"""
Visualize experiment 3 heatmaps and critical noise curves and
experiment 4 (using experiment 3 heatmaps to determine critical
noise levels)
"""

import argparse
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


# Color map for the heatmaps
CMAP = "RdYlGn"


def load_heatmap_csv(path: str) -> tuple[pd.DataFrame, pd.DataFrame]:
    # Load CSV and validate cols
    df = pd.read_csv(path)
    required_cols = {"numPatterns", "noiseLevel", "P_success"}
    missing = required_cols.difference(df.columns)
    if missing:
        raise ValueError(f"Missing columns in {path}: {sorted(missing)}")

    # Ensure numeric, and stable sorting
    df["numPatterns"] = pd.to_numeric(df["numPatterns"], errors="coerce").astype("Int64")
    df["noiseLevel"] = pd.to_numeric(df["noiseLevel"], errors="coerce")
    df["P_success"] = pd.to_numeric(df["P_success"], errors="coerce")
    df = df.dropna(subset=["numPatterns", "noiseLevel", "P_success"]).copy()

    # Reindex for heatmap
    pivot = df.pivot(index="numPatterns", columns="noiseLevel", values="P_success").sort_index()
    pivot = pivot.reindex(sorted(pivot.columns), axis=1)
    return df, pivot


def main() -> None:
    # Parse args
    parser = argparse.ArgumentParser(description="Visualize Experiment 3 heatmaps and critical noise curves")
    parser.add_argument(
        "base_dir",
        type=str,
        help="Run directory containing the CSVs (e.g. data/experiment3/<params>)",
    )
    parser.add_argument("--n", type=int, default=100, help="Network size N used to compute alpha = P/N")
    parser.add_argument("--p_success", type=float, default=0.95, help="Success threshold theta_success")
    args = parser.parse_args()
    network_size_n = args.n

    # Load in each heatmap
    base_dir = Path(args.base_dir)
    files: list[tuple[str, Path]] = [
        ("Hebbian", base_dir / "Hebbian.csv"),
        ("PseudoInverse", base_dir / "PseudoInverse.csv"),
        ("Storkey", base_dir / "Storkey.csv"),
    ]

    pivots: list[tuple[str, pd.DataFrame]] = []
    for label, path in files:
        if not path.exists():
            raise FileNotFoundError(f"Missing required file: {path}")
        _, pivot = load_heatmap_csv(str(path))
        pivots.append((label, pivot))

    # Make heatmaps on same plot
    fig, axes = plt.subplots(1, len(pivots), figsize=(18, 6), dpi=150, sharey=True)
    if len(pivots) == 1:
        axes = [axes]

    # One curve per learning rule: for each noise level q, compute the maximum saturation
    # where retrieval succeeds (>= threshold).
    critical_curves: list[tuple[str, list[float], list[float]]] = []

    last_im = None
    for ax, (name, pivot) in zip(axes, pivots):
        # pivot index/columns are grid axes - values are success rates
        z = pivot.to_numpy(dtype=float)
        x = [float(c) for c in pivot.columns]
        p_values = [int(i) for i in pivot.index]
        alpha_values = [p / float(network_size_n) for p in p_values]

        # calculate cell sizes so imshow's axes match numeric x/y values
        if len(x) > 1:
            dx = x[1] - x[0]
        else:
            dx = 1.0

        if len(alpha_values) > 1:
            dy = alpha_values[1] - alpha_values[0]
        else:
            dy = 1.0
        # extent puts pixel centers on the correct x/y coordinates.
        extent = [
            x[0] - dx / 2.0,
            x[-1] + dx / 2.0,
            alpha_values[0] - dy / 2.0,
            alpha_values[-1] + dy / 2.0,
        ]

        # heatmap
        last_im = ax.imshow(
            z,
            origin="lower",
            aspect="auto",
            interpolation="nearest",
            cmap=CMAP,
            vmin=0.0,
            vmax=1.0,
            extent=extent,
        )
        ax.set_title(name)
        ax.set_xlabel(r"Noise Level ($q$)")

        # For each noise level q (column), take the set of saturations alpha where
        # P(retrieval) >= threshold and keep the max.
        max_alpha: list[float] = []
        for col_idx in range(z.shape[1]):
            mask = z[:, col_idx] >= args.p_success
            if not np.any(mask):
                max_alpha.append(np.nan)
                continue
            vals = np.array(alpha_values)[mask]
            max_alpha.append(float(np.max(vals)))

        critical_curves.append((name, x, max_alpha))

        # make x-axis labels readable (avoid too many ticks)
        if len(x) > 1:
            xticks = np.linspace(x[0], x[-1], num=min(8, len(x)))
            ax.set_xticks(xticks)
            ax.set_xticklabels([f"{t:.2f}" for t in xticks], rotation=45, ha="right")

        if len(alpha_values) > 1:
            yticks = np.linspace(alpha_values[0], alpha_values[-1], num=min(10, len(alpha_values)))
            ax.set_yticks(yticks)
            ax.set_yticklabels([f"{t:.2f}" for t in yticks])

    axes[0].set_ylabel(r"Saturation ($\alpha = P / N$)")

    # shared colorbar for the three heatmaps
    fig.subplots_adjust(left=0.08, right=0.88, bottom=0.14, top=0.90, wspace=0.25) # padding so colourbar doesnt overlap heatmaps
    if last_im is not None:
        cax = fig.add_axes([0.90, 0.14, 0.02, 0.76])
        cbar = fig.colorbar(last_im, cax=cax)
        cbar.set_label("P(Retrieval Success)")

    # plot critical-noise curves on separate figure
    fig2, ax2 = plt.subplots(figsize=(7, 5), dpi=150)
    for name, q, alpha_max in critical_curves:
        ax2.plot(q, alpha_max, linewidth=2, label=name)

    ax2.set_xlabel(r"Noise Level ($q$)")
    ax2.set_ylabel(f"Max Saturation where P(success) >= {args.p_success:.2f}")
    ax2.set_title(f"Max Saturation vs Noise")
    # ax2.set_ylim(0.0, 1.0)
    ax2.grid(True, alpha=0.3)
    ax2.legend()

    fig2.tight_layout()

    plt.show()


if __name__ == "__main__":
    main()
