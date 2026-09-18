# 🌱 Estação IoT — Monitoramento Climático (Projeto Integrador · 4º Módulo ADS)

![Platform](https://img.shields.io/badge/plataforma-ESP32-blue)
![IDE](https://img.shields.io/badge/IDE-Arduino-teal)
![Cloud](https://img.shields.io/badge/nuvem-ThingSpeak-lightgrey)

Camada de captação de dados (IoT) do Projeto Integrador **"Dashboard de Monitoramento Climático e Logístico em Nuvem para Exportadoras de Frutas"**, para o agronegócio de exportação de frutas do Vale do São Francisco (Petrolina/Juazeiro).

Um **ESP32** com sensor **DHT11/DHT22** lê temperatura e umidade e publica os dados em tempo real, via **MQTT**, em um canal do **ThingSpeak**. O restante do sistema (Cloud, Data Science, Segurança) consome esses dados pela API REST do ThingSpeak.

```
[ ESP32 + DHT11 ]  --Wi-Fi-->  [ Broker MQTT ThingSpeak ]  -->  [ Canal ThingSpeak ]  --API REST-->  [ resto do sistema ]
```

## 🔧 Hardware

| Componente | Observação |
| --- | --- |
| ESP32 (ESP32-C3 Mini ou DevKit) | Microcontrolador com Wi-Fi embutido |
| Sensor DHT11 (ou DHT22) | Temperatura e umidade |
| Resistor 10kΩ | Só necessário no sensor "nu" de 4 pinos (módulos de 3 pinos já têm pull-up embutido) |
| Cabo USB **de dados** | Cabos só de carga não funcionam para upload/serial |

**Ligação (DHT11 → ESP32-C3 Mini):** VCC → 3V3 · DATA → GPIO4 · GND → GND

> ⚠️ O ESP32 trabalha em 3.3V, não 5V — 5V direto num pino pode danificar a placa.

## 🚀 Setup

1. **Instalar a Arduino IDE** → [arduino.cc/en/software](https://www.arduino.cc/en/software/)
2. **Adicionar suporte ao ESP32**: Preferências → URLs Adicionais para Gerenciadores de Placas →
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   Depois: Ferramentas → Placa → Gerenciador de Placas → instalar pacote **Espressif** → selecionar **"ESP32C3 Dev Module"**
3. **Instalar as bibliotecas** (Ferramentas → Gerenciar Bibliotecas):
   - **DHT sensor library** (Adafruit) + dependência **Adafruit Unified Sensor**
   - **PubSubClient**
   - `WiFi.h` já vem incluso no pacote da placa
   📎 Passo a passo com prints de todas as bibliotecas: *[link do Notion aqui]*
4. **Criar canal no ThingSpeak** → [thingspeak.mathworks.com/channels/new](https://thingspeak.mathworks.com/channels/new) → ativar Field1 (Temperatura) e Field2 (Umidade) → anotar o **Channel ID**
5. **Criar dispositivo MQTT**: Devices → MQTT → Add a new device → autorizar o canal → Download Credentials (Client ID, Username, Password — a senha só aparece uma vez)
6. **Configurar credenciais**: copie `firmware/arduino_secrets.h.example` para `arduino_secrets.h` (mesma pasta do sketch) e preencha com seus dados reais. **Nunca** faça commit desse arquivo — mantenha-o no `.gitignore`.

## 📂 Código por aula

O firmware evoluiu ao longo do módulo. Cada arquivo em [`firmware/`](firmware/) é uma versão completa e comentada do sketch daquela aula:

| Arquivo | Aula | O que foi adicionado |
| --- | --- | --- |
| [`aula1.ino`](firmware/aula1.ino) | Blink | LED piscando via GPIO (saída digital) |
| [`aula2.ino`](firmware/aula2.ino) | Entrada digital | Botão (`INPUT_PULLUP`) + Serial Monitor |
| [`aula3.ino`](firmware/aula3.ino) | Sensor DHT22 (Wokwi) | Leitura de temperatura/umidade via biblioteca DHT |
| [`aula4.ino`](firmware/aula4.ino) | Circuito físico | Migração do simulador para DHT11 + ESP32-C3 Mini real |
| [`aula5.ino`](firmware/aula5.ino) | JSON | Estrutura os dados em JSON; adiciona máxima registrada e flag de alerta |
| [`aula6.ino`](firmware/aula6.ino) | MQTT | Publica no ThingSpeak via Wi-Fi + MQTT (versão final) |

`aula6.ino` é o código que efetivamente roda em produção; os demais são o histórico pedagógico de como se chegou até ele.

**Formato da comunicação (Aula 6):**
- Tópico: `channels/<ID_DO_CANAL>/publish`
- Payload: `field1=24.50&field2=60.00` (o ThingSpeak **não** usa JSON no MQTT)
- Intervalo mínimo: ~15s entre publicações no plano gratuito

## 🧯 Troubleshooting

**Wi-Fi travado em `Status: ...`:**

| Código | Significado |
| --- | --- |
| 0 | Rádio ocioso, ainda não tentou conectar |
| 1 | SSID não encontrado |
| 2 | Varredura concluída |
| 3 | Conectado |
| 4 | Falha — geralmente senha incorreta |
| 5 | Conectou e perdeu a conexão |
| 6 | Desconectado |

Se persistir, verifique a alimentação: cabo USB fraco pode não entregar energia suficiente no pico de consumo do Wi-Fi.

**Caracteres estranhos no Serial (`#$%&*@`):** baud rate do Monitor Serial diferente do `Serial.begin()` — ambos devem estar em **115200**.

## ✅ Requisitos atendidos

| ID | Requisito | Status |
| --- | --- | --- |
| RF01 | Enviar dados de sensores via MQTT em intervalos regulares | ✔️ |
| RNF02 | Chaves de API protegidas, fora do código-fonte | ✔️ via `arduino_secrets.h` |
| RNF11 | Respeitar intervalo mínimo do plano gratuito (~15s) | ✔️ `delay(16000)` |

## 🗺️ Próximas etapas

- **Aula 7:** dashboard dos dados no Node-RED
- **Sprint 1:** primeira versão ponta a ponta (sensor → MQTT → dashboard)
- Persistência em nuvem, histórico, alertas e contrato de dados com o restante do Projeto Integrador

## 📁 Estrutura do repositório

```
.
├── README.md
└── firmware/
    ├── aula1.ino
    ├── aula2.ino
    ├── aula3.ino
    ├── aula4.ino
    ├── aula5.ino
    ├── aula6.ino
    └── arduino_secrets.h.example
```
