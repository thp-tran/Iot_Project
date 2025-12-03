#include "tinyml.h"

// Globals, for the convenience of one-shot setup.
namespace
{
    tflite::ErrorReporter *error_reporter = nullptr;
    const tflite::Model *model = nullptr;
    tflite::MicroInterpreter *interpreter = nullptr;
    TfLiteTensor *input = nullptr;
    TfLiteTensor *output = nullptr;
    constexpr int kTensorArenaSize = 8 * 1024; // Adjust size based on your model
    uint8_t tensor_arena[kTensorArenaSize];
} // namespace

void setupTinyML()
{
    Serial.println("TensorFlow Lite Init....");
    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

    model = tflite::GetModel(dht_anomaly_model_tflite); // g_model_data is from model_data.h
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        error_reporter->Report("Model provided is schema version %d, not equal to supported version %d.",
                               model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    static tflite::AllOpsResolver resolver;
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
    interpreter = &static_interpreter;

    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        error_reporter->Report("AllocateTensors() failed");
        return;
    }

    input = interpreter->input(0);
    output = interpreter->output(0);

    Serial.println("TensorFlow Lite Micro initialized on ESP32.");
}

void tiny_ml_task(void *pvParameters)
{

    setupTinyML();

    while (1)
    {
        SensorData recv0;

        if(xSemaphoreTake(semSensorData, portMAX_DELAY) == pdTRUE){
            if(xQueuePeek(qSensorData, &recv0, 0) == pdTRUE){
                xSemaphoreGive(semSensorData);
                // Prepare input data (e.g., sensor readings)
                // For a simple example, let's assume a single float input
                input->data.f[0] = recv0.temperature; // Example temperature value
                input->data.f[1] = recv0.humidity; // Example humidity value
        
                // Run inference
                TfLiteStatus invoke_status = interpreter->Invoke();
                if (invoke_status != kTfLiteOk)
                {
                    error_reporter->Report("Invoke failed");
                    return;
                }
        
                // Get and process output
                float result = output->data.f[0];
                Serial.println("Inference result: " + String(result * 100.0f) + "% - Temperature: " + String(recv0.temperature) + " C, Humidity: " + String(recv0.humidity) + " %");
        
                vTaskDelay(5000);
            }
        }
    }
}