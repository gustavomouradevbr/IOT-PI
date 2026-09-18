# 🌱 Estação IoT — Monitoramento Climático (Projeto Integrador · 4º Módulo ADS)

![Platform](https://img.shields.io/badge/plataforma-ESP32-blue)
![IDE](https://img.shields.io/badge/IDE-Arduino-teal)
![Cloud](https://img.shields.io/badge/nuvem-ThingSpeak-lightgrey)

Camada de captação de dados (IoT) do Projeto Integrador **"Dashboard de Monitoramento Climático e Logístico em Nuvem para Exportadoras de Frutas"**, desenvolvido para o agronegócio de exportação de frutas do Vale do São Francisco (Petrolina/Juazeiro).

Este repositório contém o firmware do dispositivo: um **ESP32** com sensor **DHT11/DHT22** que lê temperatura e umidade e publica os dados em tempo real, via **MQTT**, em um canal do **ThingSpeak**. O restante do sistema (Cloud, Data Science, Segurança) consome esses dados pela API REST do ThingSpeak — o dispositivo nunca envia dados diretamente para as outras camadas.

## 📐 Arquitetura

```
[ ESP32 + DHT11 ]  --Wi-Fi-->  [ Broker MQTT ThingSpeak ]  -->  [ Canal ThingSpeak ]
   (Device/Edge)                mqtt3.thingspeak.com            Field1 = Temperatura
                                                                 Field2 = Umidade
                                                                       |
                                                                       v
                                                        [ API REST → resto do sistema ]
                                                     (Cloud / Data Science / Dashboard BI)
```

## 🔧 Hardware utilizado

| Componente | Observação |
| --- | --- |
| ESP32 (ESP32-C3 Mini ou DevKit) | Microcontrolador com Wi-Fi embutido |
| Sensor DHT11 (ou DHT22) | Temperatura e umidade |
| Resistor 10kΩ | Só necessário se o sensor for o "nu" de 4 pinos (módulos de 3 pinos já vêm com pull-up embutido) |
| Cabo USB **de dados** | Cabos só de carga não funcionam para upload/serial |
| Protoboard + jumpers | Montagem do circuito |

### Ligação física (DHT11 → ESP32-C3 Mini)

| DHT11 | ESP32-C3 Mini |
| --- | --- |
| VCC (+) | 3V3 |
| DATA (sinal) | GPIO4 |
| GND (–) | GND |

> ⚠️ O ESP32 trabalha em **3.3V**, não 5V. Ligar 5V direto num pino pode danificar a placa.

## 🚀 Guia de instalação e configuração

### 1. Instalar a Arduino IDE

Baixe e instale a versão mais recente da IDE em:
👉 **[arduino.cc/en/software](https://www.arduino.cc/en/software/)**

Escolha o instalador correspondente ao seu sistema operacional (Windows, macOS ou Linux).

### 2. Adicionar o suporte à placa ESP32

1. Abra a Arduino IDE → **Preferências**.
2. No campo **"URLs Adicionais para Gerenciadores de Placas"**, cole:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Vá em **Ferramentas → Placa → Gerenciador de Placas**, busque por `esp32` e instale o pacote da **Espressif**.
4. Ainda em **Ferramentas → Placa**, selecione **"ESP32C3 Dev Module"** (ajuste conforme a variante da sua placa).
5. Conecte o cabo USB e selecione a porta correta em **Ferramentas → Porta**.

### 3. Instalar as bibliotecas necessárias

Pelo **Gerenciador de Bibliotecas** (Ferramentas → Gerenciar Bibliotecas), instale:

- **DHT sensor library** (Adafruit) — aceite instalar também a dependência **Adafruit Unified Sensor**
- **PubSubClient** — comunicação MQTT

> `WiFi.h` já vem incluso no pacote da placa ESP32, não precisa instalar separadamente.

📎 **Passo a passo detalhado (com prints) de todas as bibliotecas:** *[link do Notion aqui]*

### 4. Criar conta e canal no ThingSpeak

1. Acesse **[thingspeak.mathworks.com/channels/new](https://thingspeak.mathworks.com/channels/new)** e crie uma conta gratuita (ou entre, se já tiver).
2. Ative **Field1** (Temperatura) e **Field2** (Umidade) → **Save**.
3. Anote o **Channel ID**, disponível na URL do canal (`thingspeak.com/channels/SEU_NUMERO`) e em **Channel Settings**.

### 5. Criar um dispositivo MQTT no ThingSpeak

1. Vá em **Devices → MQTT → Add a new device**.
2. Dê um nome ao dispositivo e, em **Authorize Channels**, selecione o canal criado → **Add Channel**.
3. Clique em **Add Device** → **Download Credentials**.
4. Guarde imediatamente **Client ID**, **Username** e **Password** — a senha só é exibida uma vez.

### 6. Configurar as credenciais do projeto

Crie, dentro do sketch na Arduino IDE, uma nova aba chamada exatamente `arduino_secrets.h` (seta ▼ no canto das abas → **New Tab**):

```cpp
#define SECRET_SSID "NOME_DO_WIFI"
#define SECRET_PASS "SENHA_DO_WIFI"
#define SECRET_MQTT_CLIENT_ID "SEU_CLIENT_ID"
#define SECRET_MQTT_USERNAME "SEU_USERNAME"
#define SECRET_MQTT_PASSWORD "SUA_SENHA_MQTT"
#define SECRET_CHANNEL_ID SEU_CHANNEL_ID
```

> ⚠️ O ESP32 só conecta em redes Wi-Fi de **2.4GHz**. Se estiver usando o roteador do celular, ajuste a banda (no iPhone: "Maximizar Compatibilidade").
>
> 🔒 **Nunca** faça commit do `arduino_secrets.h` com credenciais reais — adicione o arquivo ao `.gitignore`.

### 7. Montar o circuito e carregar o código

1. Monte o circuito conforme a tabela de ligação acima.
2. Abra o sketch principal, confirme que `arduino_secrets.h` está preenchido.
3. Clique em **Carregar** (seta →).
4. Abra o **Monitor Serial** (lupa, canto superior direito) e confira que está em **115200 baud**.

## 📄 Código de referência do dispositivo

```cpp
#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "arduino_secrets.h"

#define DHT_PIN 4
#define DHT_TYPE DHT11

const char* ssid = SECRET_SSID;
const char* senhaWifi = SECRET_PASS;

const char* mqttServer = "mqtt3.thingspeak.com";
const int mqttPort = 1883;
const char* mqttClientID = SECRET_MQTT_CLIENT_ID;
const char* mqttUser = SECRET_MQTT_USERNAME;
const char* mqttPass = SECRET_MQTT_PASSWORD;

unsigned long channelID = SECRET_CHANNEL_ID;

DHT dht(DHT_PIN, DHT_TYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void conectarWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(ssid, senhaWifi);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Status: ");
    Serial.println(WiFi.status());
  }
  Serial.println("\nWiFi conectado!");
}

void conectarMQTT() {
  while (!client.connected()) {
    Serial.println("Conectando ao MQTT do ThingSpeak...");
    if (client.connect(mqttClientID, mqttUser, mqttPass)) {
      Serial.println("Conectado ao MQTT!");
    } else {
      Serial.println("Falha na conexao MQTT, tentando de novo em 5s...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  conectarWiFi();
  client.setServer(mqttServer, mqttPort);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }
  if (!client.connected()) {
    conectarMQTT();
  }
  client.loop();

  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  if (isnan(temperatura) || isnan(umidade) || (temperatura == 0 && umidade == 0)) {
    Serial.println("Erro ao ler o sensor DHT11!");
  } else {
    char payload[100];
    sprintf(payload, "field1=%.2f&field2=%.2f", temperatura, umidade);

    String topico = "channels/" + String(channelID) + "/publish";
    client.publish(topico.c_str(), payload);

    Serial.print("Publicado: ");
    Serial.println(payload);
  }

  delay(16000); // ThingSpeak (plano gratuito) exige mínimo de ~15s entre mensagens
}
```

### Formato da comunicação

- **Tópico MQTT:** `channels/<ID_DO_CANAL>/publish`
- **Payload:** o ThingSpeak **não** usa JSON no MQTT — usa formato de parâmetros de URL: `field1=24.50&field2=60.00`
- **Intervalo mínimo:** ~15 segundos entre publicações no plano gratuito do ThingSpeak

## 🧯 Troubleshooting

**Wi-Fi travado imprimindo `Status: ...` sem parar:**

| Código | Significado |
| --- | --- |
| 0 | Rádio ocioso, ainda não tentou conectar |
| 1 | SSID não encontrado no ar |
| 2 | Varredura de redes concluída |
| 3 | Conectado com sucesso |
| 4 | Falha → geralmente senha incorreta |
| 5 | Conectou e depois perdeu a conexão |
| 6 | Desconectado |

Se persistir, verifique a alimentação: um cabo USB fraco pode não entregar energia suficiente no pico de consumo do Wi-Fi.

**Caracteres estranhos no Monitor Serial (`#$%&*@`):** o baud rate do Monitor Serial está diferente do `Serial.begin()` do código — ambos precisam estar em **115200**.

## ✅ Requisitos do Projeto Integrador atendidos por este dispositivo

| ID | Requisito | Status |
| --- | --- | --- |
| RF01 | Enviar dados de sensores (temperatura/umidade) via MQTT em intervalos regulares | ✔️ Implementado |
| RNF02 | Chaves de API do ThingSpeak protegidas e não expostas no código-fonte | ✔️ Via `arduino_secrets.h` |
| RNF11 | Respeitar o intervalo mínimo de atualização do plano gratuito (~15s) | ✔️ `delay(16000)` |

## 🗺️ Próximas etapas do cronograma

- **Aula 7:** dashboard exibindo os dados do ESP32 no Node-RED
- **Sprint 1:** primeira versão funcional ponta a ponta (sensor → MQTT → dashboard)
- **Aulas seguintes:** persistência em nuvem, histórico, alertas inteligentes e contrato de dados/API com o restante do Projeto Integrador

## 📁 Estrutura sugerida do repositório

```
.
├── README.md
├── firmware/
│   ├── estacao_iot.ino
│   └── arduino_secrets.h.example   # modelo sem credenciais reais
└── docs/
    └── diagrama_circuito.png
```
