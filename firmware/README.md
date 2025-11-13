# Firmware ESP32

Código para o microcontrolador ESP32 responsável por:
- Leitura do sensor CCD
- Inferência do modelo TensorFlow Lite
- Comunicação Bluetooth
- Interface com o aplicativo móvel

## Estrutura (Planejada)

```
firmware/
├── src/
│   ├── main.cpp              # Código principal
│   ├── sensor_ccd.cpp        # Driver do sensor
│   ├── color_inference.cpp   # Inferência TFLite
│   └── bluetooth_comm.cpp    # Comunicação BT
├── include/
│   ├── sensor_ccd.h
│   ├── color_inference.h
│   ├── bluetooth_comm.h
│   ├── modelo_cor.h          # Gerado por Python
│   └── scaler.h              # Gerado por Python
├── lib/
│   ├── modelo_cor.cc         # Gerado por Python
│   └── tensorflow-lite/      # Biblioteca TFLite
└── platformio.ini            # Configuração do projeto
```

## Componentes

### TensorFlow Lite Micro
- Versão otimizada para microcontroladores
- Inferência em tempo real
- Baixo consumo de memória

### Sensor CCD
- Resolução: 1546 pixels
- Interface: SPI/I2C
- Taxa de amostragem configurável

### Comunicação
- Bluetooth Classic ou BLE
- Protocolo customizado
- Taxa: 115200 baud

## Desenvolvimento

### Configuração do Ambiente

1. Instale o PlatformIO:
```bash
pip install platformio
```

2. Clone o repositório e navegue até o firmware:
```bash
cd firmware
```

3. Compile o projeto:
```bash
pio run
```

4. Upload para o ESP32:
```bash
pio run --target upload
```

### Dependências

- ESP32 Dev Module
- TensorFlow Lite Micro
- ArduinoBLE (se usar BLE)
- BluetoothSerial (se usar BT Classic)

## Integração do Modelo

1. Treine e converta seu modelo usando os scripts Python
2. Copie os arquivos gerados para o firmware:
   - `modelo_cor.cc` → `lib/`
   - `modelo_cor.h` → `include/`
   - `scaler.h` → `include/`

3. Compile e faça upload

## Pinout (Exemplo)

```
ESP32          Sensor CCD
-----          ----------
GPIO 18  →     SCK
GPIO 19  →     MISO
GPIO 23  →     MOSI
GPIO 5   →     CS
3.3V     →     VCC
GND      →     GND
```

## Memória

- Flash: ~4MB disponível
- SRAM: ~520KB
- Modelo TFLite: ~12KB
- Buffer de dados: ~3KB

## Testes

Execute testes unitários:
```bash
pio test
```

## Troubleshooting

**Erro de compilação com TFLite:**
- Verifique a versão da biblioteca
- Certifique-se de que os arquivos .cc e .h foram copiados

**Overflow de memória:**
- Reduza o tamanho do tensor arena
- Otimize o modelo (quantização)

**Falha na comunicação:**
- Verifique a configuração de baud rate
- Teste com monitor serial
