#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// Configurações de Wi-Fi e MQTT
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";

// Definição dos Pinos
const int TRIG_PIN = 18;
const int ECHO_PIN = 19;
const int SERVO_PIN = 21;

Servo myservo;
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
unsigned long lastTampaOpen = 0;
bool tampaAberta = false;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  myservo.attach(SERVO_PIN);
  myservo.write(0);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT com broker HiveMQ...");
    if (client.connect("ESP32_Allan_RA10443092")) { 
      Serial.println(" CONECTADO com sucesso!");
      // Publica status inicial
      client.publish("lixeira/status", "Sistema Iniciado");
      client.publish("lixeira/nivel", "0%");
    } else {
      Serial.print(" Falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

long lerDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duracao = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout de 30ms
  if (duracao == 0) return 999; // Retorna valor inválido se sem eco
  return (duracao * 0.0343) / 2;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long distancia = lerDistancia();
  
  // Exibe distância no monitor serial
  if (distancia < 200 && distancia > 0) {
    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" cm");
  }
  
  // Controle da tampa SEM bloquear o loop
  if (distancia < 20 && distancia > 0 && !tampaAberta) {
    Serial.println("*** OBJETO DETECTADO - ABRINDO TAMPA ***");
    myservo.write(180);
    tampaAberta = true;
    lastTampaOpen = millis();
    
    // Publica status via MQTT
    if (client.publish("lixeira/status", "Aberta")) {
      Serial.println("MQTT: Status 'Aberta' publicado com sucesso!");
    } else {
      Serial.println("MQTT: Falha ao publicar 'Aberta'");
    }
  }
  
  // Fecha a tampa após 5 segundos
  if (tampaAberta && (millis() - lastTampaOpen > 5000)) {
    Serial.println("*** FECHANDO TAMPA ***");
    myservo.write(0);
    tampaAberta = false;
    
    if (client.publish("lixeira/status", "Fechada")) {
      Serial.println("MQTT: Status 'Fechada' publicado com sucesso!");
    } else {
      Serial.println("MQTT: Falha ao publicar 'Fechada'");
    }
  }

  // Publicação do nível de preenchimento a cada 30 segundos
  unsigned long agora = millis();
  if (agora - lastMsg > 30000) {
    lastMsg = agora;
    
    int nivel;
    if (distancia <= 2) {
      nivel = 100;
    } else if (distancia >= 50) {
      nivel = 0;
    } else {
      nivel = map(distancia, 2, 50, 100, 0);
    }
    
    char msg[10];
    sprintf(msg, "%d%%", nivel);
    Serial.print("Enviando nivel via MQTT: ");
    Serial.print(msg);
    
    if (client.publish("lixeira/nivel", msg)) {
      Serial.println(" - ENVIADO COM SUCESSO!");
    } else {
      Serial.println(" - FALHA NO ENVIO!");
    }
  }
  
  delay(100); // Delay reduzido para não travar o sistema
}
