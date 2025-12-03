import pandas as pd
from sklearn.model_selection import train_test_split
import tensorflow as tf

PREFIX = "dht_anomaly_model"

data = pd.read_csv('src/dataset.csv', names=['temp', 'humidity', 'label'])
X = data[['temp', 'humidity']]
y = data['label']

X_Train, X_Test, Y_Train, Y_Test = train_test_split(X, y, test_size=0.2, random_state=42)

model = tf.keras.Sequential([
    tf.keras.layers.Input(shape=(2,)),
    tf.keras.layers.Dense(8, activation='relu'),
    tf.keras.layers.Dense(1, activation='sigmoid'),
])

model.compile(optimizer='adam', loss='binary_crossentropy', metrics=['accuracy'])
model.fit(X_Train, Y_Train, epochs=500, batch_size=64, validation_data=(X_Test, Y_Test))
model.save(PREFIX + '.keras')

converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
tflite_model = converter.convert()

with open(PREFIX + '.tflite', 'wb') as f:
    f.write(tflite_model)
    
tflite_path = PREFIX + '.tflite'
output_header_path = 'include/' + PREFIX + '.h'

with open(tflite_path, 'rb') as tflite_file:
    tflite_content = tflite_file.read()
    
hex_lines = [', '.join([f'0x{byte:02x}' for byte in tflite_content[i:i+12]]) for i in range(0, len(tflite_content), 12)]

hex_array = ',\n  '.join(hex_lines)

with open(output_header_path, 'w') as header_file:
    header_file.write('const unsigned char ' + PREFIX + '_tflite[] = {\n')
    header_file.write(f'{hex_array}\n')
    header_file.write('};\n\n')