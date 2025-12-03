import numpy as np
import matplotlib.pyplot as plt
import os
from sklearn.metrics import precision_score, recall_score, f1_score

PREFIX = "dht_anomaly_model"

def plot_metrics():
    # Load saved history
    history = np.load("metrics/history.npy", allow_pickle=True).item()

    # Extract Accuracy (validation)
    accuracy = history.get("val_accuracy", [])
    final_accuracy = accuracy[-1] if len(accuracy) > 0 else 0

    # Load true/pred
    try:
        y_true = np.load("metrics/y_true.npy")
        y_pred = np.load("metrics/y_pred.npy")

        precision = precision_score(y_true, y_pred)
        recall = recall_score(y_true, y_pred)
        f1 = f1_score(y_true, y_pred)

    except:
        print("⚠️ Missing y_true.npy or y_pred.npy → Precision/Recall/F1 skipped")
        precision = 0
        recall = 0
        f1 = 0

    # Create output folder
    output_dir = "images"
    os.makedirs(output_dir, exist_ok=True)

    # -----------------------------
    # Combined Metrics Bar Chart
    # -----------------------------
    metrics = ["Accuracy", "Precision", "Recall", "F1 Score"]
    values = [final_accuracy, precision, recall, f1]
    colors = ["orange", "blue", "green", "purple"]

    plt.figure(figsize=(10, 6))
    bars = plt.bar(metrics, values, color=colors)

    # Add value labels
    for bar in bars:
        h = bar.get_height()
        plt.text(
            bar.get_x() + bar.get_width() / 2,
            h + 0.02,
            f"{h:.3f}",
            ha="center",
            fontsize=12
        )

    plt.ylim(0, 1)
    plt.title("Model Performance Comparison (Validation Metrics)")
    plt.ylabel("Score (0–1)")
    plt.grid(axis="y", linestyle="--", alpha=0.6)

    combined_path = os.path.join(output_dir, f"{PREFIX}_metrics_compare.png")
    plt.savefig(combined_path, dpi=200)
    print(f"Saved combined metrics chart → {combined_path}")
    plt.close()


if __name__ == "__main__":
    plot_metrics()
