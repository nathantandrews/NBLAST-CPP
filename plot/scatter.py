import os
import sys
import argparse
import numpy as np
import matplotlib.pyplot as plt


def load_scores(filename: str) -> dict[tuple[str, str], float]:
    """
    Loads a TSV/space-separated file with columns:
    query target score

    Returns:
        dict[(query, target)] = score
    """
    scores = {}

    with open(filename, "r") as f:
        header = f.readline().strip().split()

        # Try to find score column automatically
        try:
            score_idx = header.index("score")
        except ValueError:
            score_idx = 2  # fallback: third column

        for line in f:
            if not line.strip():
                continue

            fields = line.strip().split()
            if len(fields) <= score_idx:
                continue

            query = fields[0]
            target = fields[1]

            try:
                score = float(fields[score_idx])
                scores[(query, target)] = score
            except ValueError:
                continue

    return scores

def spearman_corr(x, y):
    """Compute Spearman correlation without scipy."""
    rx = np.argsort(np.argsort(x))
    ry = np.argsort(np.argsort(y))
    return np.corrcoef(rx, ry)[0, 1]

def main():
    parser = argparse.ArgumentParser(
        description="Compare two score files and generate scatter plot"
    )
    parser.add_argument("file1", help="First score file")
    parser.add_argument("file2", help="Second score file")
    parser.add_argument("--label1", default="Known", help="Label for first file")
    parser.add_argument("--label2", default="Experimental", help="Label for second file")
    parser.add_argument("--out", default="scatter_plot.png", help="Output filename")

    args = parser.parse_args()

    scores1 = load_scores(args.file1)
    scores2 = load_scores(args.file2)

    # Match only shared (query, target)
    common_keys = sorted(set(scores1.keys()) & set(scores2.keys()))

    if not common_keys:
        print("No overlapping (query, target) pairs found.")
        sys.exit(1)

    x = np.array([scores1[k] for k in common_keys])
    y = np.array([scores2[k] for k in common_keys])

    # Statistics
    mean_x, mean_y = np.mean(x), np.mean(y)
    std_x, std_y = np.std(x), np.std(y)
    pearson = np.corrcoef(x, y)[0, 1]
    spearman = spearman_corr(x, y)

    print(f"Matched pairs: {len(common_keys)}")
    print(f"Pearson r:  {pearson:.4f}")
    print(f"Spearman ρ: {spearman:.4f}")

    # Plot
    plt.figure(figsize=(8, 8))
    plt.scatter(x, y, s=8, alpha=0.5)

    # Diagonal reference line
    min_val = min(x.min(), y.min())
    max_val = max(x.max(), y.max())
    plt.plot([min_val, max_val], [min_val, max_val], linestyle="--")

    plt.xlabel(args.label1)
    plt.ylabel(args.label2)
    plt.title("Score Comparison")

    stats_text = (
        f"Pairs: {len(common_keys)}\n"
        f"Pearson r: {pearson:.4f}\n"
        f"Spearman p: {spearman:.4f}\n"
        f"Mean X: {mean_x:.4f}\n"
        f"Mean Y: {mean_y:.4f}"
    )

    plt.text(
        0.05,
        0.95,
        stats_text,
        transform=plt.gca().transAxes,
        verticalalignment="top",
        bbox=dict(facecolor="white", alpha=0.7),
    )

    plt.tight_layout()
    plt.savefig(args.out, dpi=300)
    plt.close()

    print(f"Scatter plot saved to: {args.out}")


if __name__ == "__main__":
    main()