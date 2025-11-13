# Scripts MATLAB

Scripts para captura de dados espectrais via Bluetooth.

## `cap_por_Bluetooth.m`

Script principal para capturar dados do sensor CCD através de comunicação Bluetooth com o ESP32.

### Configuração

Antes de executar, configure os seguintes parâmetros no script:

```matlab
porta_serial = "COM4";        % Porta serial (Windows: COMx, Linux: /dev/ttyUSBx)
baud_rate = 115200;           % Taxa de comunicação
timeout_segundos = 60;        % Timeout de espera por quadro
N = 1546;                     % Número de pixels por quadro
M = 2;                        % Número de quadros por cor
```

### Cores Capturadas

O script captura dados de 18 cores diferentes:
- Chantilly, Eternidade, Quintal de Casa
- Romance Sereno, Vulcão Osorno, Néctar de Uva
- Brilhante Bruto, Banho de Lua, Coala
- Papel de Seda, Aromaterapia, Vênus
- Flor de Lis, Roxo Impecável, Cruzeiro Marítimo
- Azul Sereia, Vermelho Oriental, Timidez

### Uso

1. Conecte o ESP32 via Bluetooth ao computador
2. Identifique a porta serial correta
3. Configure a porta no script
4. Execute no MATLAB:

```matlab
cap_por_Bluetooth
```

5. Posicione cada cor quando solicitado
6. O script gerará arquivos `dados_[nome_da_cor].csv`

### Formato de Saída

Cada arquivo CSV contém:
- **Coluna 1:** `classe` - Nome da cor
- **Colunas 2-1547:** `pixel_1` até `pixel_1546` - Valores de intensidade

### Visualização

Durante a captura, o script exibe em tempo real:
- Gráfico da intensidade por pixel
- Progresso da captura
- Status da conexão

### Resolução de Problemas

**Erro de timeout:**
- Aumente o valor de `timeout_segundos`
- Verifique a conexão Bluetooth
- Certifique-se de que o ESP32 está transmitindo

**Porta serial não encontrada:**
- No Windows: Use o Gerenciador de Dispositivos
- No Linux: Execute `ls /dev/tty*`
- No Mac: Execute `ls /dev/cu.*`

**Dados corrompidos:**
- Verifique a taxa de baud_rate
- Certifique-se de que não há outras conexões abertas
- Execute `delete(instrfindall)` antes de reconectar

## Requisitos

- MATLAB R2019b ou superior
- Instrument Control Toolbox
- Conexão Bluetooth funcional
