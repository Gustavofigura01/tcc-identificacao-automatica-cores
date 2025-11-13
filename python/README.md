# Scripts Python

Ferramentas para processamento de dados e conversão de modelos para o ESP32.

## Scripts Disponíveis

### `agrupar_e_normalizar.py`
Agrupa múltiplos arquivos CSV capturados pelo MATLAB em um único dataset.

**Uso:**
```bash
python agrupar_e_normalizar.py
```

**Entrada:** Arquivos `dados_*.csv` no diretório atual  
**Saída:** `dataset_cores.csv`

### `esp32_converter_float.py`
Converte modelo Keras treinado para TensorFlow Lite e gera arquivos C++ para embedding no ESP32.

**Uso:**
```bash
python esp32_converter_float.py
```

**Entrada:**
- `improved_color_classifier_keras.h5` - Modelo treinado
- `improved_color_classifier_scaler.pkl` - Scaler dos dados
- `improved_color_classifier_encoder.pkl` - Encoder das classes

**Saída:**
- `modelo_cor.tflite` - Modelo TFLite
- `modelo_cor.cc` - Dados do modelo em C++
- `modelo_cor.h` - Header com definições

### `scaler.py`
Gera arquivo header C++ com os parâmetros de normalização.

**Uso:**
```bash
python scaler.py
```

**Entrada:** `improved_color_classifier_scaler.pkl`  
**Saída:** `scaler.h`

## Instalação

```bash
pip install -r requirements.txt
```

## Dependências

- tensorflow >= 2.8.0
- numpy
- pandas
- scikit-learn
- joblib
