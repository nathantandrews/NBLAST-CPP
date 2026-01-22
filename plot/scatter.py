import os
import numpy as np
import matplotlib.pyplot as plt

SCORE_COUNT = 1000

def load_scores(filename: str, score_col: int) -> list[float]:
    scores: list[float] = []
    with open(filename, "r") as f:
        next(f)
        i = 0
        for line in f:
            if i > SCORE_COUNT:
                return scores
            line = line.rstrip("\n")
            if not line:
                continue
            fields = line.split()
            try:
                scores.append(float(fields[score_col - 1]))
            except (IndexError, ValueError):
                print("invalid index: ", score_col, line, file="scatter.err")
            i += 1
    return scores

def main():
    output_dir = os.path.join(os.path.dirname(__file__), "output")
    os.makedirs(output_dir, exist_ok=True)

    x = load_scores("banc-fafb-simple.tsv", 4)
    y = load_scores("fafb-to-banc.txt", 3)

    mean_x, mean_y = np.mean(x), np.mean(y)
    std_x, std_y = np.std(x), np.std(y)
    correlation = np.corrcoef(x, y)[0, 1]

    plt.figure(figsize=(8, 8))
    plt.scatter(x, y, s=10, alpha=0.6, label="Points")

    line = np.linspace(0.9, 1.0, 100)
    plt.plot(line, line, color="red", linewidth=2, label="y = x")

    # Add labels and stats text
    plt.xlabel("NBLAST")
    plt.ylabel("NBLAST++")
    plt.title("NBLAST++ Scores against Original NBLAST")

    stats_text = (
        f"Mean X: {mean_x:.4f}\n"
        f"Mean Y: {mean_y:.4f}\n"
        f"Std X: {std_x:.4f}\n"
        f"Std Y: {std_y:.4f}\n"
        f"Correlation: {correlation:.4f}"
    )
    plt.text(0.905, 0.905, stats_text, fontsize=10, bbox=dict(facecolor='white', alpha=0.7))

    plt.legend()

    # Output file path
    output_file = os.path.join(output_dir, "scatter_plot.png")
    plt.savefig(output_file, dpi=300)
    plt.close()

    print(f"Scatter plot saved to: {output_file}")


if __name__ == "__main__":
    main()

