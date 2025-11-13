# ================================
# ESP32 MODEL CONVERTER - FLOAT32
# ================================

import numpy as np
import tensorflow as tf
from tensorflow import keras
import joblib
import os

class ESP32ModelConverter:
    def __init__(self, model_path, scaler_path=None, encoder_path=None):
        self.model = keras.models.load_model(model_path)
        self.scaler = joblib.load(scaler_path) if scaler_path else None
        self.encoder = joblib.load(encoder_path) if encoder_path else None
        
    def convert_to_tflite_float32(self):
        """
        Converte modelo para TensorFlow Lite mantendo float32
        """
        print("Convertendo modelo para TensorFlow Lite (Float32)...")
        
        # Converter para TensorFlow Lite
        converter = tf.lite.TFLiteConverter.from_keras_model(self.model)
        
        # Otimizações básicas (sem quantização)
        converter.optimizations = [tf.lite.Optimize.DEFAULT]
        
        # Manter float32
        converter.target_spec.supported_types = [tf.float32]
        
        # Converter
        tflite_model = converter.convert()
        
        # Salvar modelo
        tflite_filename = 'modelo_cor.tflite'
        with open(tflite_filename, 'wb') as f:
            f.write(tflite_model)
        
        print(f"Modelo TFLite salvo: {tflite_filename} ({len(tflite_model)} bytes)")
        
        # Gerar arquivo .cc para embedding
        self.generate_model_cc(tflite_model)
        
        # Gerar header com informações do modelo
        self.generate_model_header()
        
        return tflite_model
    
    def generate_model_cc(self, tflite_model):
        """
        Gera arquivo .cc com o modelo embedado
        """
        print("Gerando modelo_cor.cc...")
        
        # Converter bytes para array C++
        model_data = []
        for i, byte in enumerate(tflite_model):
            if i % 12 == 0:
                model_data.append('\n  ')
            model_data.append(f'0x{byte:02x},')
        
        model_array_str = ''.join(model_data).rstrip(',')
        
        cc_content = f'''// Modelo TensorFlow Lite gerado automaticamente
// Não edite este arquivo manualmente

#include <stdint.h>

// Dados do modelo
const unsigned char modelo_cor[] = {{{model_array_str}
}};

const unsigned int modelo_cor_len = {len(tflite_model)};
'''
        
        with open('modelo_cor.cc', 'w') as f:
            f.write(cc_content)
        
        print("Arquivo modelo_cor.cc gerado!")
    
    def generate_model_header(self):
        """
        Gera header com informações do modelo
        """
        print("Gerando modelo_cor.h...")
        
        # Obter informações do modelo
        input_shape = self.model.input_shape
        output_shape = self.model.output_shape
        
        # Classes (se disponível)
        classes_code = ""
        if self.encoder:
            classes = self.encoder.classes_
            classes_list = ',\n    '.join([f'"{cls}"' for cls in classes])
            classes_code = f'''
// Classes de cores
static const char* color_labels[] = {{
    {classes_list}
}};

#define NUM_CLASSES {len(classes)}
'''
        
        # Parâmetros do scaler (se disponível)
        scaler_code = ""
        if self.scaler:
            scaler_code = f'''
// Parâmetros do scaler disponíveis
// Use os arquivos .pkl para carregar em tempo de execução
// ou implemente normalização customizada
#define HAS_SCALER 1
#define SCALER_FEATURES {len(self.scaler.mean_)}
'''
        
        header_content = f'''#ifndef MODELO_COR_H
#define MODELO_COR_H

#include <stdint.h>

// Declarações do modelo
extern const unsigned char modelo_cor[];
extern const unsigned int modelo_cor_len;

// Configurações do modelo
#define INPUT_SIZE {input_shape[1] if len(input_shape) > 1 else 'UNKNOWN'}
#define OUTPUT_SIZE {output_shape[1] if len(output_shape) > 1 else 'UNKNOWN'}
#define CCD_FRAME_SIZE 1546

{classes_code}
{scaler_code}
#endif // MODELO_COR_H
'''
        
        with open('modelo_cor.h', 'w') as f:
            f.write(header_content)
        
        print("Arquivo modelo_cor.h gerado!")
    
    def test_conversion(self):
        """
        Testa o modelo convertido
        """
        print("Testando modelo convertido...")
        
        # Carregar modelo TFLite
        interpreter = tf.lite.Interpreter(model_path='modelo_cor.tflite')
        interpreter.allocate_tensors()
        
        # Obter detalhes dos tensors
        input_details = interpreter.get_input_details()
        output_details = interpreter.get_output_details()
        
        print(f"Input shape: {input_details[0]['shape']}")
        print(f"Input type: {input_details[0]['dtype']}")
        print(f"Output shape: {output_details[0]['shape']}")
        print(f"Output type: {output_details[0]['dtype']}")
        
        # Teste com dados sintéticos
        input_shape = input_details[0]['shape']
        test_input = np.random.randn(*input_shape).astype(np.float32)
        
        interpreter.set_tensor(input_details[0]['index'], test_input)
        interpreter.invoke()
        
        output_data = interpreter.get_tensor(output_details[0]['index'])
        print(f"Teste bem-sucedido! Output shape: {output_data.shape}")
        
        return True

# Função para uso direto
def convert_model_to_esp32(model_path, scaler_path=None, encoder_path=None):
    """
    Função simplificada para conversão
    """
    converter = ESP32ModelConverter(model_path, scaler_path, encoder_path)
    
    # Converter
    tflite_model = converter.convert_to_tflite_float32()
    
    # Testar
    converter.test_conversion()
    
    print("\n✅ Conversão concluída!")
    print("Arquivos gerados:")
    print("  - modelo_cor.tflite")
    print("  - modelo_cor.cc")
    print("  - modelo_cor.h")
    print("\nCopie os arquivos .cc e .h para seu projeto ESP32")
    
    return tflite_model

# Exemplo de uso
if __name__ == "__main__":
    # Ajuste os caminhos dos seus arquivos
    MODEL_PATH = 'improved_color_classifier_keras.h5'
    SCALER_PATH = 'improved_color_classifier_scaler.pkl'
    ENCODER_PATH = 'improved_color_classifier_encoder.pkl'
    
    # Verificar se os arquivos existem
    if not os.path.exists(MODEL_PATH):
        print(f"❌ Arquivo do modelo não encontrado: {MODEL_PATH}")
        exit(1)
    
    # Converter
    convert_model_to_esp32(MODEL_PATH, SCALER_PATH, ENCODER_PATH)
