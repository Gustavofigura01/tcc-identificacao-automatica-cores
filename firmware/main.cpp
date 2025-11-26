/********************************************************************
 *  CCD  →  CNN COLOR CLASSIFIER  ·  ESP32-S3 + TFLite Micro
 *  Gustavo Figura · 2025
 *******************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "ble_server.h"
#include "scaler.h"   

/* ---------- TFLite Micro ---------- */
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

/* ---------- Modelo e configurações ---------- */
#include "modelo_cor.h"

/* ---------- CCD / UART config ---------- */
#define UART_PORT       UART_NUM_1
#define TXD_PIN         GPIO_NUM_17
#define RXD_PIN         GPIO_NUM_18
#define UART_BAUD       115200
#define BUF_SIZE        1024

#define CCD_FRAME_SIZE  1546
#define CCD_DATA_SIZE   (CCD_FRAME_SIZE * 2)

/* ---------- Buffers ---------- */
static uint16_t raw_frame[CCD_FRAME_SIZE];
static float    norm_frame[CCD_FRAME_SIZE];
static float    features[INPUT_SIZE];  // Buffer para features processadas

/* ---------- TFLM globals ---------- */
#include "esp_heap_caps.h"      //  <<–– novo

#define kArenaSize  (128 * 1024)        // 512 kB – ajuste se precisar
static uint8_t* tensor_arena = nullptr; //  <<–– agora é ponteiro

constexpr int kOpCount = 64;
static tflite::MicroMutableOpResolver<kOpCount> resolver;
static tflite::MicroInterpreter* interpreter;
static TfLiteTensor* input;
static TfLiteTensor* output;


/* ---------- Forward declarations ---------- */
static bool init_tflite(void);
static void preprocess_ccd_advanced(void);
static void extract_spectral_features(void);
static int  classify_color(void);
static void uart_event_task(void*);
static void print_frame_info(const uint16_t *frame, size_t len);
static void print_model_info(void);
static void zscore_features(void);


/********************************************************************/
extern "C" void app_main(void)
{
    ESP_LOGI("APP", "=== CCD Color Classifier v2.0 ===");
    ESP_LOGI("APP", "Inicializando sistema...");

    // Configurar UART
    uart_config_t cfg = {};
    cfg.baud_rate  = UART_BAUD;
    cfg.data_bits  = UART_DATA_8_BITS;
    cfg.parity     = UART_PARITY_DISABLE;
    cfg.stop_bits  = UART_STOP_BITS_1;
    cfg.flow_ctrl  = UART_HW_FLOWCTRL_DISABLE;
    cfg.source_clk = UART_SCLK_DEFAULT;

    uart_driver_install(UART_PORT, BUF_SIZE*2, BUF_SIZE*2, 0, nullptr, 0);
    uart_param_config(UART_PORT, &cfg);
    uart_set_pin(UART_PORT, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    ESP_LOGI("APP", "UART configurado - Baud: %d", UART_BAUD);
    ble_server_init();
    // Inicializar TensorFlow Lite
    if (!init_tflite()) {
        ESP_LOGE("APP", "❌ Falha ao inicializar TFLite. Reiniciando em 5s...");
        vTaskDelay(pdMS_TO_TICKS(5000));
        esp_restart();
        return;
    }

    // Mostrar informações do modelo
    print_model_info();

    // Iniciar tasks
    xTaskCreate(uart_event_task, "uart_task", 12288, nullptr, 10, nullptr);
    
  
    ESP_LOGI("APP", "✅ Sistema inicializado com sucesso!");
}

/********************************************************************
 *  TFLite Micro – Inicialização
 ********************************************************************/
static bool init_tflite(void)
{
    ESP_LOGI("TFLM", "Inicializando TensorFlow Lite Micro...");

/* -------- Aloca arena em RAM interna -------- */
if (!tensor_arena) {
    tensor_arena = static_cast<uint8_t*>(
        heap_caps_malloc(kArenaSize, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));

    if (!tensor_arena) {
        ESP_LOGE("MEM", "❌ Falha ao alocar %d bytes na RAM interna", kArenaSize);
        return false;
    }
    ESP_LOGI("MEM", "Arena alocada @ %p (%d bytes, RAM interna)", tensor_arena, kArenaSize);
}


    /* -------- Registro de operadores -------- */
    resolver.AddReshape();
    resolver.AddDepthwiseConv2D();
    resolver.AddConv2D();
    resolver.AddMul();
    resolver.AddAdd();
    resolver.AddQuantize();
    resolver.AddDequantize();
    resolver.AddAveragePool2D();
    resolver.AddMaxPool2D();
    resolver.AddFullyConnected();
    resolver.AddSoftmax();
    resolver.AddShape();
    resolver.AddStridedSlice();
    resolver.AddPack();
    resolver.AddUnpack();
    resolver.AddSplit();
    resolver.AddExpandDims();
    resolver.AddMean();
    resolver.AddRelu();
    resolver.AddRelu6();
    resolver.AddTanh();
    resolver.AddLogistic();
    resolver.AddLeakyRelu();

    /* -------- Carrega modelo -------- */
    const tflite::Model* model = tflite::GetModel(modelo_cor);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        ESP_LOGE("TFLM", "❌ Versão do schema incompatível (%u ≠ %u)",
                 (unsigned)model->version(), (unsigned)TFLITE_SCHEMA_VERSION);
        return false;
    }

    /* -------- Cria interpretador -------- */
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kArenaSize, nullptr);
    interpreter = &static_interpreter;

    /* -------- Aloca tensores -------- */
    if (interpreter->AllocateTensors() != kTfLiteOk) {
        printf("Arena used (min): %zu bytes\n",
       interpreter->arena_used_bytes());
        ESP_LOGE("TFLM", "❌ AllocateTensors falhou");
        return false;
    }
ESP_LOGI("TFLM", "Arena usada: %d / %d bytes",
         interpreter->arena_used_bytes(), kArenaSize);
    input  = interpreter->input(0);
    output = interpreter->output(0);
    if (!input || !output) {
        ESP_LOGE("TFLM", "❌ Falha ao obter tensors de entrada/saída");
        return false;
    }

    ESP_LOGI("TFLM", "✅ TensorFlow Lite Micro inicializado! Arena usada: %d / %d bytes",
             interpreter->arena_used_bytes(), kArenaSize);
    return true;
}


/********************************************************************
 *  Informações do modelo
 *******************************************************************/
static void print_model_info(void)
{
    ESP_LOGI("MODEL", "=== INFORMAÇÕES DO MODELO ===");
    ESP_LOGI("MODEL", "Tamanho do modelo: %u bytes", modelo_cor_len);
    ESP_LOGI("MODEL", "Arena de tensors: %d KB", kArenaSize / 1024);
    
    if (input && output) {
        ESP_LOGI("MODEL", "Input tensor: %d elementos (tipo: %d)", 
                 input->bytes / sizeof(float), input->type);
        ESP_LOGI("MODEL", "Output tensor: %d elementos (tipo: %d)", 
                 output->bytes / sizeof(float), output->type);
        
        // Dimensões do tensor de entrada
        ESP_LOGI("MODEL", "Dimensões de entrada:");
        for (int d = 0; d < input->dims->size; ++d) {
            ESP_LOGI("MODEL", "  dim[%d]: %d", d, input->dims->data[d]);
        }
        
        // Dimensões do tensor de saída
        ESP_LOGI("MODEL", "Dimensões de saída:");
        for (int d = 0; d < output->dims->size; ++d) {
            ESP_LOGI("MODEL", "  dim[%d]: %d", d, output->dims->data[d]);
        }
    }
    
    #ifdef NUM_CLASSES
    ESP_LOGI("MODEL", "Número de classes: %d", NUM_CLASSES);
    #endif
    
    ESP_LOGI("MODEL", "==========================");
}

/********************************************************************
 *  UART Task - Recepção de dados do CCD
 *******************************************************************/
static void uart_event_task(void*)
{
    uint8_t buf[BUF_SIZE];
    size_t idx = 0;
    uint32_t frame_count = 0;
    
    ESP_LOGI("UART", "Task UART iniciada - aguardando dados CCD...");

    while (true) {
        int len = uart_read_bytes(UART_PORT, buf, BUF_SIZE, pdMS_TO_TICKS(100));
        
        if (len > 0) {
            for (int i = 0; i < len; ++i) {
                uint16_t pos = idx++;
                
                if (pos & 1)  // byte baixo
                    raw_frame[pos >> 1] |= buf[i];
                else          // byte alto
                    raw_frame[pos >> 1] = buf[i] << 8;

                if (idx >= CCD_FRAME_SIZE * 2) {  // frame completo
                    frame_count++;
                    ESP_LOGI("CCD", "Frame #%lu recebido", frame_count);
                    
                    // Debug do frame
                    print_frame_info(raw_frame, CCD_FRAME_SIZE);
                    
                    // Pré-processamento avançado
                    preprocess_ccd_advanced();
                    
                    // Extração de features
                    extract_spectral_features();
                    zscore_features();       
                    // Classificação
                    int cls = classify_color();
                    
                    #ifdef NUM_CLASSES
                    if (cls >= 0 && cls < NUM_CLASSES) {
                        ESP_LOGI("AI", "🎨 Cor detectada: %s (classe %d)", 
                                color_labels[cls], cls);
                                ble_server_send(color_labels[cls]);
                    } else {
                        ESP_LOGE("AI", "❌ Erro na classificação: %d", cls);
                    }
                    #else
                    ESP_LOGI("AI", "Classe predita: %d", cls);
                    #endif
                    
                    idx = 0;  // reset para próximo frame
                }
            }
        }
    }
}

/********************************************************************
 *  Pré-processamento avançado do CCD
 *******************************************************************/
static void preprocess_ccd_advanced(void)
{
    // 1. Filtro de mediana 3-pixels (remove ruído impulsivo)
    uint16_t prev = raw_frame[0];
    for (int i = 1; i < CCD_FRAME_SIZE - 1; ++i) {
        uint16_t a = prev, b = raw_frame[i], c = raw_frame[i + 1];
        prev = b;
        // Mediana de 3 valores
        raw_frame[i] = (a > b) ? ((b > c) ? b : (a > c ? c : a)) : 
                                 ((a > c) ? a : (b > c ? c : b));
    }

    // 2. Normalização robusta usando percentis (mais estável que z-score)
    // Converter para float
    for (int i = 0; i < CCD_FRAME_SIZE; ++i) {
        norm_frame[i] = (float)raw_frame[i];
    }
    
    // Calcular percentis 25, 50, 75
    float sorted[CCD_FRAME_SIZE];
    memcpy(sorted, norm_frame, sizeof(float) * CCD_FRAME_SIZE);
    
    // Ordenação simples (bubble sort para arrays pequenos)
    for (int i = 0; i < CCD_FRAME_SIZE - 1; i++) {
        for (int j = 0; j < CCD_FRAME_SIZE - i - 1; j++) {
            if (sorted[j] > sorted[j + 1]) {
                float temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }
    
    float p25 = sorted[CCD_FRAME_SIZE / 4];
    float p50 = sorted[CCD_FRAME_SIZE / 2];
    float p75 = sorted[3 * CCD_FRAME_SIZE / 4];
    float iqr = p75 - p25;
    
    // Normalização robusta
    if (iqr > 1e-6f) {
        for (int i = 0; i < CCD_FRAME_SIZE; ++i) {
            norm_frame[i] = (norm_frame[i] - p50) / iqr;
            // Clipping para evitar outliers extremos
            if (norm_frame[i] > 5.0f) norm_frame[i] = 5.0f;
            if (norm_frame[i] < -5.0f) norm_frame[i] = -5.0f;
        }
    }
    
    ESP_LOGD("PREPROC", "Normalização: P25=%.2f, P50=%.2f, P75=%.2f, IQR=%.2f", 
             p25, p50, p75, iqr);
}

/********************************************************************
 *  Extração de features espectrais
 *******************************************************************/
static void extract_spectral_features(void)
{
    int feature_idx = 0;

    /* -----------------------------------------------------------------
     * 1. Copia o espectro normalizado (1 546 pontos)
     * -----------------------------------------------------------------*/
    for (int i = 0; i < CCD_FRAME_SIZE && feature_idx < INPUT_SIZE; ++i) {
        features[feature_idx++] = norm_frame[i];
    }

    /* -----------------------------------------------------------------
     * 2. Features estatísticas globais (agora são 7)
     *    mean, std, range, pct_above_mean, max, min, peak_pos_norm
     * -----------------------------------------------------------------*/
    float sum = 0, sum_sq = 0;
    float min_val = norm_frame[0], max_val = norm_frame[0];
    float peak_val = norm_frame[0];
    int   peak_pos = 0, above_mean = 0;

    for (int i = 0; i < CCD_FRAME_SIZE; ++i) {
        float v = norm_frame[i];
        sum     += v;
        sum_sq  += v * v;

        if (v < min_val)  min_val  = v;
        if (v > max_val)  max_val  = v;
        if (v > peak_val) { peak_val = v; peak_pos = i; }
    }

    float mean      = sum / CCD_FRAME_SIZE;
    float variance  = (sum_sq / CCD_FRAME_SIZE) - mean * mean;
    float std_dev   = sqrtf(variance + 1e-8f);

    for (int i = 0; i < CCD_FRAME_SIZE; ++i)
        if (norm_frame[i] > mean) above_mean++;

    /* grava os 7 campos */
    features[feature_idx++] = mean;
    features[feature_idx++] = std_dev;
    features[feature_idx++] = max_val - min_val;                 // range
    features[feature_idx++] = (float)above_mean / CCD_FRAME_SIZE;
    features[feature_idx++] = max_val;
    features[feature_idx++] = min_val;
    features[feature_idx++] = (float)peak_pos / CCD_FRAME_SIZE;  // 0-1

    /* -----------------------------------------------------------------
     * 3. Features regionais – 8 regiões × 3 = 24
     * -----------------------------------------------------------------*/
    int region_size = CCD_FRAME_SIZE / 8;
    for (int r = 0; r < 8 && feature_idx + 3 <= INPUT_SIZE; ++r) {
        int start = r * region_size;
        int end   = (r == 7) ? CCD_FRAME_SIZE : start + region_size;

        float region_sum = 0, region_max = norm_frame[start];
        int   region_max_idx = start;

        for (int i = start; i < end; ++i) {
            float v = norm_frame[i];
            region_sum += v;
            if (v > region_max) { region_max = v; region_max_idx = i; }
        }

        features[feature_idx++] = region_sum / (end - start);     // média
        features[feature_idx++] = region_max;                     // pico
        features[feature_idx++] = (float)region_max_idx / CCD_FRAME_SIZE;
    }

    /* Preenche com zero caso ainda sobrem slots (garantia de segurança) */
    while (feature_idx < INPUT_SIZE) {
        features[feature_idx++] = 0.0f;
    }

    ESP_LOGD("FEATURES", "Extraídas %d features", feature_idx);
}


static void zscore_features(void)
{
    for (int i = 0; i < INPUT_SIZE; ++i) {
        float sigma = INPUT_STD[i];
        /* evita divisão por zero */
        if (sigma < 1e-8f) sigma = 1e-8f;
        features[i] = (features[i] - INPUT_MEAN[i]) / sigma;
        /* opcional: clip para evitar valores gigantes */
        if (features[i] > 6.0f)  features[i] = 6.0f;
        if (features[i] < -6.0f) features[i] = -6.0f;
    }
}


/********************************************************************
 *  Classificação de cor usando TensorFlow Lite
 *******************************************************************/
static int classify_color(void)
{
    // Copiar features para tensor de entrada
    memcpy(input->data.f, features, sizeof(float) * INPUT_SIZE);

    
    // Executar inferência
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
        ESP_LOGE("AI", "❌ Falha na inferência: %d", invoke_status);
        return -1;
    }
    
    // Encontrar classe com maior probabilidade
    int best_class = 0;
    float best_confidence = output->data.f[0];
    int num_classes = output->bytes / sizeof(float);
    
    for (int i = 1; i < num_classes; ++i) {
        if (output->data.f[i] > best_confidence) {
            best_confidence = output->data.f[i];
            best_class = i;
        }
    }
    
    ESP_LOGI("AI", "Confiança: %.3f (%.1f%%)", best_confidence, best_confidence * 100.0f);
    
    // Log das top 3 classes
    ESP_LOGD("AI", "Top 3 classes:");
    for (int i = 0; i < 3 && i < num_classes; ++i) {
        float max_conf = -1.0f;
        int max_idx = -1;
        for (int j = 0; j < num_classes; ++j) {
            if (output->data.f[j] > max_conf) {
                // Verificar se já foi usado
                bool used = false;
                for (int k = 0; k < i; ++k) {
                    // Implementação simplificada
                }
                if (!used) {
                    max_conf = output->data.f[j];
                    max_idx = j;
                }
            }
        }
        if (max_idx >= 0) {
            ESP_LOGD("AI", "  %d: %.3f", max_idx, max_conf);
        }
    }
    
    return best_class;
}

/********************************************************************
 *  Debug - Informações do frame CCD
 *******************************************************************/
static void print_frame_info(const uint16_t *frame, size_t len)
{
    if (len < 20) {
        ESP_LOGW("CCD", "Frame muito pequeno: %zu pixels", len);
        return;
    }

    // Estatísticas básicas
    uint32_t sum = 0;
    uint16_t min_val = frame[0], max_val = frame[0];
    
    for (size_t i = 0; i < len; ++i) {
        sum += frame[i];
        if (frame[i] < min_val) min_val = frame[i];
        if (frame[i] > max_val) max_val = frame[i];
    }
    
    float mean = (float)sum / len;
    
    ESP_LOGI("CCD", "Frame: %zu px | Min: %u | Max: %u | Média: %.1f", 
             len, min_val, max_val, mean);
    
    // Encontrar pico
    uint16_t peak_val = frame[0];
    int peak_pos = 0;
    for (int i = 1; i < (int)len; ++i) {
        if (frame[i] > peak_val) {
            peak_val = frame[i];
            peak_pos = i;
        }
    }
    
    ESP_LOGI("CCD", "Pico: %u na posição %d (%.1f%%)", 
             peak_val, peak_pos, (float)peak_pos * 100.0f / len);
}