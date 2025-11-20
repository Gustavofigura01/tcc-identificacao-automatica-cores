# Sistema de Identificação Automática de Cores

**Trabalho de Conclusão de Curso - Engenharia de Computação**  
**UTFPR*

## 📋 Sobre o Projeto

Sistema inteligente de identificação automática de cores que integra sensores ópticos, algoritmos de inteligência artificial e interface móvel. O sistema captura espectros de cores através de um sensor CCD conectado a um microcontrolador ESP32, processa os dados com redes neurais e exibe os resultados em tempo real através de um aplicativo móvel.

## 🎯 Objetivos

- Desenvolver um sistema embarcado de aquisição de dados espectrais de cores
- Treinar modelos de machine learning para classificação precisa de cores
- Implementar inferência de IA diretamente no microcontrolador (Edge AI)
- Criar interface mobile intuitiva para visualização dos resultados

## 🔧 Componentes do Sistema

### Hardware
- **Microcontrolador:** ESP32
- **Sensor:** Sensor óptico CCD (1546 pixels)
- **Comunicação:** Bluetooth para transmissão de dados

### Software
- **Captura de Dados:** Scripts MATLAB para aquisição via Bluetooth
- **Processamento:** Python com TensorFlow/Keras para treinamento
- **Firmware:** C/C++ para ESP32 com TensorFlow Lite
- **Interface:** Aplicativo móvel (Android/iOS)

## 📁 Estrutura do Projeto

```
├── firmware/           # Código do ESP32
├── matlab/            # Scripts de captura de dados
│   └── cap_por_Bluetooth.m
├── python/            # Scripts de processamento e IA
│   ├── agrupar_e_normalizar.py
│   ├── esp32_converter_float.py
│   └── scaler.py
├── docs/              # Documentação do projeto
└── README.md
```

## 🚀 Como Usar

### 1. Captura de Dados

```matlab
% Configure a porta serial no arquivo cap_por_Bluetooth.m
% Execute no MATLAB para capturar dados das cores
cap_por_Bluetooth
```

### 2. Processamento dos Dados

```bash
# Agrupe os arquivos CSV capturados
python python/agrupar_e_normalizar.py

# Treine seu modelo (adicione seu script de treinamento)
python python/treinar_modelo.py
```

### 3. Conversão para ESP32

```bash
# Converta o modelo treinado para TensorFlow Lite
python python/esp32_converter_float.py

# Gere os parâmetros do scaler
python python/scaler.py
```

### 4. Deploy no ESP32

- Copie os arquivos `.cc` e `.h` gerados para o projeto do firmware
- Compile e faça upload para o ESP32

## 📊 Cores Suportadas

O sistema foi treinado para identificar 18 cores distintas:

- Chantilly
- Eternidade
- Quintal de Casa
- Romance Sereno
- Vulcão Osorno
- Néctar de Uva
- Brilhante Bruto
- Banho de Lua
- Coala
- Papel de Seda
- Aromaterapia
- Vênus
- Flor de Lis
- Roxo Impecável
- Cruzeiro Marítimo
- Azul Sereia
- Vermelho Oriental
- Timidez

## 🛠️ Tecnologias Utilizadas

- **Python** - Processamento e ML
  - TensorFlow / Keras
  - NumPy, Pandas
  - scikit-learn
  - joblib
- **MATLAB** - Aquisição de dados
- **C/C++** - Firmware ESP32
- **TensorFlow Lite** - Inferência embarcada
- **Kotlin** - Desenvolvimento mobile

## 📦 Dependências Python

```bash
pip install tensorflow numpy pandas scikit-learn joblib
```

## 🔬 Especificações Técnicas

- **Resolução do sensor:** 1546 pixels
- **Taxa de comunicação:** 115200 baud
- **Formato do modelo:** TensorFlow Lite (Float32)
- **Tamanho do modelo:** ~12 KB
- **Entrada do modelo:** Vetor de 1577 features (1546 pixels + 31 features derivadas)

## 📝 Fluxo de Trabalho

```
1. Captura via Bluetooth (MATLAB)
   ↓
2. Pré-processamento dos dados (Python)
   ↓
3. Treinamento do modelo (TensorFlow)
   ↓
4. Conversão para TFLite (Python)
   ↓
5. Integração no ESP32 (C++)
   ↓
6. Visualização no App (Mobile)
```

## 👨‍💻 Autor

**Gustavo**  
Engenharia de Computação - UTFPR Toledo  
Previsão de conclusão: Dezembro/2026

## 📄 Licença

Este projeto é parte de um Trabalho de Conclusão de Curso (TCC) da UTFPR.

## 🤝 Contribuições

Sugestões e melhorias são bem-vindas! Abra uma issue ou envie um pull request.

## 📧 Contato

Para dúvidas ou mais informações sobre o projeto, entre em contato através do GitHub.

---

**UTFPR - Universidade Tecnológica Federal do Paraná**  
*Campus Toledo - Paraná, Brasil*
