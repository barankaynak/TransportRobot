#include <ESP8266WiFi.h>    
#include <PubSubClient.h>
#include <Wire.h>
#include <SparkFun_APDS9960.h>
//Renk sensörü değişkenleri
SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint16_t ambient_light = 0;
uint16_t red_light = 0;
uint16_t green_light = 0;
uint16_t blue_light = 0;
const char* renk;
/*
int solMotor2 = 4;
int solMotor1 = 5;
int sagMotor2 = 16;
int sagMotor1 = 1;
int sensorSol = 13;
int sensoryeni = 12;*/

//Pin Tanımlamaları
#define trigPin 9
#define echoPin 8
#define solMotor2 4
#define solMotor1 5
#define sagMotor2 16
#define sagMotor1 1
#define sensorSol 13
#define sensorSag 12

long sure, mesafe;

String hedefIstasyon;
String sonuc = "NOT";
//MQTT konfigrasyon bilgileri
const char* ssid = "Network Name";
const char* password = "dsdfsdfasd";
const char* mqtt_server = "mqtt.dioty.co";
const char* mqtt_user = "username";
const char* mqtt_password = "password";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);   //Seri Haberleşme
  setup_wifi();     //Wi-Fi bağlantısı
  client.setServer(mqtt_server, 1883);    //MQTT Server bağlantısı
  client.setCallback(callback);       //MQTT callback fonksiyonu
  
 //HCSR-04 Pinleri
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
 //Sensör pinleri
  pinMode(sensorSol, INPUT);
  pinMode(sensorSag, INPUT);
 //Motor Pinleri
  pinMode(solMotor1, OUTPUT);
  pinMode(solMotor2, OUTPUT);
  pinMode(sagMotor1, OUTPUT);
  pinMode(sagMotor2, OUTPUT);
 
 //Renk sensörü
 /* Serial.println();
  Serial.println(F("--------------------------------"));
  Serial.println(F("SparkFun APDS-9960 - ColorSensor"));
  Serial.println(F("--------------------------------"));*/
  
  if ( apds.init() ) {
   // Serial.println(F("APDS-9960 initialization complete"));
  } else {
   // Serial.println(F("Something went wrong during APDS-9960 init!"));
  }
  
  if ( apds.enableLightSensor(false) ) {
   // Serial.println(F("Light sensor is now running"));
  } else {
   // Serial.println(F("Something went wrong during light sensor init!"));
  }
    delay(500);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    delay(1);
    setup_wifi();
  }
  
  if (!client.connected()) {
  reconnect();
  }
  mesafeolc();
  if(mesafe <= 5) {
    client.publish("/path/transportrobot/engel","ENGELVAR");
    if (sonuc == "OK") {
      geridon();
      dur();
    } 
    else dur();
  }
  else {
  cizgitakip();
  renktani();
  if (renk == "RED" && "GREEN" && "BLUE") {
    if (hedefIstasyon == "TORNA")
      while(renk != "RED") don();
    else if (hedefIstasyon == "CNC")
      while(renk != "GREEN") don();
    else if (hedefIstasyon == "KAYNAK")
      while(renk != "BLUE") don();
    cizgitakip();
  }
  }
  
  client.loop();
}

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  //Serial.println("Aga Baglanildi");
}

void reconnect() {
  while (!client.connected()) {
    if (WiFi.status() != WL_CONNECTED) {
      delay(1);
      setup_wifi();
    }
    if (client.connect("ControlNode", mqtt_user, mqtt_password)) {
      client.subscribe("/path/transportrobot/sonuc"); 
      client.subscribe("/path/transportrobot/hedef"); 
    } 
    else {
    delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String messageValue = "";
  for (int i = 0; i < length; i++) {
    messageValue += (char)payload[i];
  }
  int tmpAddress;
  if (strcmp(topic, "/path/transportrobot/sonuc") == 0)
  {
    sonuc = messageValue;
  }
      if (strcmp(topic, "/path/transportrobot/hedef") == 0)
  {
    hedefIstasyon = messageValue;
  }
} 

void renktani() {
  if (  !apds.readAmbientLight(ambient_light) ||
  !apds.readRedLight(red_light) ||
  !apds.readGreenLight(green_light) ||
  !apds.readBlueLight(blue_light) ) {
 // Serial.println("Error reading light values");
  } else {
      if (red_light > green_light && red_light > blue_light) {
    renk = "RED";
   // Serial.println(renk);}
    else if (green_light > red_light && green_light >blue_light){
    renk = "GREEN";
   // Serial.println(renk);}
    else if (blue_light > green_light && blue_light > red_light){
    renk = "BLUE";
   // Serial.println(renk);}
    else if(ambient_light > green_light && ambient_light >red_light && ambient_light > blue_light){ renk = "AMBIENT";
   // Serial.println(renk);}
    delay(400);
  } 
}

void mesafeolc() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  sure = pulseIn(echoPin, HIGH);
  mesafe = (sure/2) / 29.1; 
}

void cizgitakip() {
  if(digitalRead(sensorSol) == 1 && digitalRead(sensorSag) == 1)ilerigit();
    else if(digitalRead(sensorSol) == 1) sol();
    else if(digitalRead(sensorSag) == 1) sag();
    else dur();
}

void ilerigit() {
  digitalWrite(sagMotor1, LOW);
  digitalWrite(sagMotor2, HIGH);

  digitalWrite(solMotor1, HIGH);
  digitalWrite(solMotor2, LOW);
  }

void gerigit() {
  digitalWrite(sagMotor1, HIGH);
  digitalWrite(sagMotor2, LOW);

  digitalWrite(solMotor1, LOW);
  digitalWrite(solMotor2, HIGH);
  }

void sag() {
  digitalWrite(sagMotor1, LOW);
  digitalWrite(sagMotor2, HIGH);

  digitalWrite(solMotor1, HIGH);
  digitalWrite(solMotor2, HIGH);
  }

void sol() {
  digitalWrite(sagMotor1, HIGH);
  digitalWrite(sagMotor2, HIGH);

  digitalWrite(solMotor1, HIGH);
  digitalWrite(solMotor2, LOW);
  }

void dur() {
  digitalWrite(sagMotor1, HIGH);
  digitalWrite(sagMotor2, HIGH);

  digitalWrite(solMotor1, HIGH);
  digitalWrite(solMotor2, HIGH);
  }
  
void geridon() {
  digitalWrite(sagMotor1, LOW);
  digitalWrite(sagMotor2, HIGH);

  digitalWrite(solMotor1, LOW);
  digitalWrite(solMotor2, HIGH);
  
  delay(500);
  }

void don() {
  digitalWrite(sagMotor1, LOW);
  digitalWrite(sagMotor2, HIGH);

  digitalWrite(solMotor1, LOW);
  digitalWrite(solMotor2, HIGH);
  }  
