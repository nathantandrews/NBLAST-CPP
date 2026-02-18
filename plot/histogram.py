import argparse
import numpy as np
import matplotlib.pyplot as plt


def load_scores(filename: str) -> list[float]:
    """
    Load scores from a file with columns:
    query target score
    (whitespace or tab separated)
    """
    scores = []

    with open(filename, "r") as f:
        header = f.readline().strip().split()

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

            try:
                scores.append(float(fields[score_idx]))
            except ValueError:
                continue

    return scores


def main():
    parser = argparse.ArgumentParser(
        description="Plot overlaid histograms of two score files"
    )
    parser.add_argument("file1", help="First score file")
    parser.add_argument("file2", help="Second score file")
    parser.add_argument("--label1", default="File 1")
    parser.add_argument("--label2", default="File 2")
    parser.add_argument("--bins", type=int, default=50)
    parser.add_argument("--out", default="histogram_comparison.png")

    args = parser.parse_args()

    scores1 = load_scores(args.file1)
    scores2 = load_scores(args.file2)

    if not scores1 or not scores2:
        print("One of the files has no valid scores.")
        return

    print(f"{args.label1}: {len(scores1)} scores")
    print(f"{args.label2}: {len(scores2)} scores")

    plt.figure(figsize=(8, 6))

    plt.hist(scores1, bins=args.bins, alpha=0.5, density=True, label=args.label1)
    plt.hist(scores2, bins=args.bins, alpha=0.5, density=True, label=args.label2)

    plt.xlabel("Score")
    plt.ylabel("Density")
    plt.title("Score Distribution Comparison")
    plt.legend()

    plt.tight_layout()
    plt.savefig(args.out, dpi=300)
    plt.close()

    print(f"Histogram saved to: {args.out}")


if __name__ == "__main__":
    main()
