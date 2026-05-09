#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// Configurações de Wi-Fi e MQTT (Padrão Wokwi)
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";

// Definição dos Pinos conforme sua montagem física no Wokwi
const int TRIG_PIN = 18;
const int ECHO_PIN = 19;
const int SERVO_PIN = 21;

// Objetos e Variáveis
Servo myservo;
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  myservo.attach(SERVO_PIN);
  myservo.write(0); // Tampa fechada inicialmente

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
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    // Nome do cliente único para evitar conflitos no broker
    if (client.connect("ESP32_Allan_RA10443092")) { 
      Serial.println("conectado com sucesso!");
    } else {
      Serial.print("falhou, rc=");
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
  long duracao = pulseIn(ECHO_PIN, HIGH);
  return (duracao * 0.0343) / 2;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long distancia = lerDistancia();
  
  // Lógica de abertura da tampa (Distância < 20cm conforme seu artigo)
  if (distancia < 20 && distancia > 0) {
    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" cm - ABRINDO TAMPA");
    myservo.write(180);
    client.publish("lixeira/status", "Aberta");
    delay(5000); // Tampa aberta por 5 segundos
  } else {
    myservo.write(0);
    client.publish("lixeira/status", "Fechada");
  }

  // Publicação do nível de preenchimento a cada 30 segundos
  unsigned long agora = millis();
  if (agora - lastMsg > 30000) {
    lastMsg = agora;
    
    // Mapeamento: 2cm (cheia/100%) até 50cm (vazia/0%)
    int nivel = map(distancia, 2, 50, 100, 0);
    if (nivel < 0) nivel = 0;
    if (nivel > 100) nivel = 100;
    
    char msg[10];
    sprintf(msg, "%d%%", nivel);
    Serial.print("Enviando nivel via MQTT: ");
    Serial.println(msg);
    client.publish("lixeira/nivel", msg);
  }
  
  delay(500); 
}
