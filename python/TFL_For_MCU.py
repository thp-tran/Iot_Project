import os
import pandas as pd
from sklearn.model_selection import train_test_split
import tensorflow as tf
import matplotlib.pyplot as plt
import numpy as np

PREFIX = "dht_anomaly_model"

data = pd.read_csv('../src/dataset.csv', names=['temp', 'humidity', 'label'])
X = data[['temp', 'humidity']]
y = data['label']

X_Train, X_Test, Y_Train, Y_Test = train_test_split(X, y, test_size=0.2, random_state=42)

model = tf.keras.Sequential([
    tf.keras.layers.Input(shape=(2,)),
    tf.keras.layers.Dense(10, activation='relu'),
    tf.keras.layers.Dense(1, activation='sigmoid'),
])

model.compile(optimizer='adam', loss='binary_crossentropy', metrics=['accuracy'])
history = model.fit(X_Train, Y_Train, epochs=1000, batch_size=64, validation_data=(X_Test, Y_Test))
model.save('saved_model/' + PREFIX + '.keras')

converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
tflite_model = converter.convert()

with open('saved_model/' + PREFIX + '.tflite', 'wb') as f:
    f.write(tflite_model)
    
tflite_path = 'saved_model/' + PREFIX + '.tflite'
output_header_path = '../include/' + PREFIX + '.h'

with open(tflite_path, 'rb') as tflite_file:
    tflite_content = tflite_file.read()
    
hex_lines = [', '.join([f'0x{byte:02x}' for byte in tflite_content[i:i+12]]) for i in range(0, len(tflite_content), 12)]

hex_array = ',\n  '.join(hex_lines)

with open(output_header_path, 'w') as header_file:
    header_file.write('const unsigned char dht_anomaly_model_tflite[] = {\n  ')
    header_file.write(f'{hex_array}\n')
    header_file.write('};\n\n')

# --------------------------------------------------------
#   PLOT TRAINING HISTORY
# --------------------------------------------------------
np.save("metrics/history.npy", history.history)
# Generate predictions for test set
y_pred_prob = model.predict(X_Test)
y_pred = (y_pred_prob > 0.5).astype(int)

# Save for precision/recall
np.save("metrics/y_true.npy", Y_Test)
np.save("metrics/y_pred.npy", y_pred)

train_loss = history.history['loss']
val_loss = history.history['val_loss']
train_acc = history.history['accuracy']
val_acc = history.history['val_accuracy']

epochs = range(1, len(train_loss) + 1)

# Create images folder
output_dir = "images"
os.makedirs(output_dir, exist_ok=True)

# ---- Plot Loss ----
plt.figure(figsize=(8, 5))
plt.plot(epochs, train_loss, label="Train Loss")
plt.plot(epochs, val_loss, label="Validation Loss")
plt.title("Model Loss")
plt.xlabel("Epoch")
plt.ylabel("Loss")
plt.legend()
plt.grid(True)

loss_path = os.path.join(output_dir, f"{PREFIX}_loss.png")
plt.savefig(loss_path, dpi=200)
print(f"Saved loss curve → {loss_path}")
plt.close()

# ---- Plot Accuracy ----
plt.figure(figsize=(8, 5))
plt.plot(epochs, train_acc, label="Train Accuracy")
plt.plot(epochs, val_acc, label="Validation Accuracy")
plt.title("Model Accuracy")
plt.xlabel("Epoch")
plt.ylabel("Accuracy")
plt.legend()
plt.grid(True)

acc_path = os.path.join(output_dir, f"{PREFIX}_accuracy.png")
plt.savefig(acc_path, dpi=200)
print(f"Saved accuracy curve → {acc_path}")
plt.close()