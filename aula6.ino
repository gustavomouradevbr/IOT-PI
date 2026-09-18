/*
  Aula 6 — Primeira comunicação IoT (ESP32 publicando via MQTT no ThingSpeak)

  MQTT é um protocolo de mensagens leve pra dispositivos com pouca
  energia/banda: o ESP32 (client) publica mensagens num broker (servidor
  do ThingSpeak), endereçadas a um tópico: channels/<ID_DO_CANAL>/publish

  Importante: o ThingSpeak NÃO usa JSON no MQTT (diferente da Aula 5) —
  usa um formato tipo parâmetro de URL, campos separados por &:
  field1=24.50&field2=60.00

  As credenciais (Wi-Fi e MQTT) ficam separadas em "arduino_secrets.h"
  (veja arduino_secrets.h.example nesta pasta) para não expor senhas no
  código-fonte.

  Atenção: o ESP32 só conecta em redes Wi-Fi de 2.4GHz.
*/

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
    // Se travar aqui imprimindo "Status: ..." sem parar, veja a tabela de
    // códigos no README (0=ocioso, 1=SSID não encontrado, 4=senha errada...)
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
  // Reconecta sozinho se cair o Wi-Fi ou a sessão MQTT
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
