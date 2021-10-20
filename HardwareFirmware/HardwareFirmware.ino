#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h> 
#define IN1 14
#define IN2 12
#define IN3 13
#define IN4 15
#define PWM 5
#define IR 4

// MQTT Broker
const char *mqtt_broker = "192.168.0.100";
const char *topic = "Detection";
const int mqtt_port = 1883;

WiFiManager wifiManager;
WiFiClient espClient;
PubSubClient client(espClient);
bool flag=false;

void setup() {
  Serial.begin(115200);
  // connecting to a WiFi network
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(IR,INPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  wifiManager.autoConnect("ESP-12F WiFi Manager");
  Serial.println("Connected!");
  while (WiFi.status() != WL_CONNECTED) {
      Serial.println("Connecting to WiFi..");
      digitalWrite(LED_BUILTIN,LOW);
      delay(1000);
      digitalWrite(LED_BUILTIN,HIGH);
      delay(1000);
  }
  digitalWrite(LED_BUILTIN,HIGH);
  Serial.println("Connected to the WiFi network");
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str())) {
          Serial.println("Local mosquitto mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
  client.subscribe(topic);
}
void takeDown()
{
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  delay(1000);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
}
void takeUp()
{
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  delay(1000);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
}
void callback(char *topic, byte *payload, unsigned int length) {
  digitalWrite(LED_BUILTIN,LOW);
  delay(200);
  digitalWrite(LED_BUILTIN,HIGH);
  delay(200);
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  String s="";
  for (int i = 0; i < length; i++) {
      Serial.print((char) payload[i]);
      s+=(char)payload[i];
  }
  flag = true;
  
  Serial.println();
  Serial.println("-----------------------");
}

void loop() {
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  analogWrite(PWM,220);
  Serial.print("IR:");
  Serial.print(digitalRead(IR));
  client.loop();
  if(!digitalRead(IR) && flag)
  {
    Serial.print("Pushing");
    delay(4000);
    takeUp();
    delay(1000);
    takeDown();
    flag = false;
  }
}
