import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt
import os

PREFIX = "dht_anomaly_model"
MODEL_DIR = "saved_model"

def plot_decision_region():
    # Load model
    model = tf.keras.models.load_model(os.path.join(MODEL_DIR, PREFIX + ".keras"))

    # Load dataset to infer min–max ranges
    data = np.loadtxt("../src/dataset.csv", delimiter=",")
    temps = data[:, 0]
    hums = data[:, 1]

    temp_min, temp_max = temps.min() - 5, temps.max() + 5
    hum_min, hum_max = hums.min() - 5, hums.max() + 5

    # Create grid
    xx, yy = np.meshgrid(
        np.linspace(temp_min, temp_max, 300),
        np.linspace(hum_min, hum_max, 300)
    )

    grid_points = np.c_[xx.ravel(), yy.ravel()].astype(np.float32)

    # Predict on grid
    preds = model.predict(grid_points, verbose=0)
    preds = preds.reshape(xx.shape)

    # Create directory
    out_dir = "images"
    os.makedirs(out_dir, exist_ok=True)

    plt.figure(figsize=(10, 8))
    plt.contourf(xx, yy, preds, levels=[0, 0.5, 1], alpha=0.4, colors=["green", "red"])
    plt.colorbar(label="Model Output (0=Normal, 1=Anomaly)")

    labels = data[:, 2]
    plt.scatter(temps[labels == 0], hums[labels == 0], s=12, color="blue", label="Normal")
    plt.scatter(temps[labels == 1], hums[labels == 1], s=12, color="red", label="Anomaly")

    plt.title("Decision Boundary — Model Normal vs Anomaly Region")
    plt.xlabel("Temperature (°C)")
    plt.ylabel("Humidity (%)")
    plt.xlim(10, 60)
    plt.legend()

    out_path = os.path.join(out_dir, f"{PREFIX}_decision_region.png")
    plt.savefig(out_path, dpi=200)
    plt.close()

    print(f"✅ Saved decision region plot → {out_path}")


if __name__ == "__main__":
    plot_decision_region()
