# Lixeira-Inteligente
IOT - Lixeira Inteligente (Mack)
# Lixeira Inteligente com ESP32 e MQTT

Protótipo de lixeira automatizada desenvolvido para a disciplina de Objetos Inteligentes Conectados (Mackenzie). O projeto foca na gestão de resíduos urbanos utilizando IoT para monitoramento em tempo real.

## 🚀 Funcionamento
O sistema utiliza um sensor ultrassônico para detectar a aproximação de usuários. Quando um objeto é detectado a menos de 20cm, a tampa é aberta automaticamente via servo motor. O nível de preenchimento da lixeira é medido e enviado via protocolo MQTT para um Broker público.

## 🛠️ Hardware Utilizado
* **ESP32**: Microcontrolador com Wi-Fi nativo.
* **HC-SR04**: Sensor de distância (nível e proximidade).
* **SG90**: Micro Servo motor (atuador da tampa).

## 📡 Protocolos e Interfaces
* **Comunicação**: Wi-Fi (Rede Wokwi-GUEST).
* **Protocolo de IoT**: MQTT (Publicação/Assinatura).
* **Broker**: broker.hivemq.com (Porta 1883).
* **Tópicos**: `lixeira/nivel` e `lixeira/status`.

## 💻 Como Reproduzir
1. Importe os arquivos `sketch.ino` e `diagram.json` no simulador [Wokwi](https://wokwi.com).
2. Instale as bibliotecas `PubSubClient` e `ESP32Servo`.
3. Execute a simulação.
## 🎥 Vídeo Demonstração

📺 [Assista no YouTube](https://youtu.be/pj_Zsv5g-1s)

## 🌍 Alinhamento ODS 11

Este projeto está alinhado ao **Objetivo de Desenvolvimento Sustentável 11** 
da ONU (Cidades e Comunidades Sustentáveis), contribuindo para a 
modernização da gestão de resíduos sólidos urbanos por meio de tecnologia IoT.

## 📚 Disciplina

- **Curso:** Análise e Desenvolvimento de Sistemas — 5º Semestre
- **Disciplina:** Objetos Inteligentes Conectados
- **Instituição:** Universidade Presbiteriana Mackenzie — EAD Polo Santana/SP
- **Aluno:** Allan Fernando Barbosa da Silveira | RA: 10443092
