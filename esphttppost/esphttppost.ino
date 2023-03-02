#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <TroykaLight.h>
#include <DHT.h>

uint8_t DHTPin = D1;

DHT dht(DHTPin, DHT22);

int minH = 200;
bool minHFlag = false;
unsigned long WaterStartTime = 0;


TroykaLight sensorLight(A0);


const char* ssid = "6GStation";
const char* password = "18234567";

HTTPClient http;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Подключение к Wi-Fi...");
  }

  
  pinMode(D0, OUTPUT);
  dht.begin();
}

void loop() {
  WiFiClientSecure client;
  HTTPClient http;  

  http.begin(client, "https://homeplant.herokuapp.com/");      
  http.addHeader("Content-Type", "application/json"); 

  float h = dht.readHumidity();
  float temp = dht.readTemperature();
  int groundH = Serial.read() << 2;
  sensorLight.read();
  float light = sensorLight.getLightLux();

  if (minHFlag && millis() > WaterStartTime + 2500)
  {
    digitalWrite(D0, LOW);
    minHFlag = false;
  }
  if (groundH > minH)
  {
    digitalWrite(D0, HIGH);
    minHFlag = true;
    WaterStartTime = millis();
  }



  int httpCode = http.POST("{\"password\":12345678," "\"h\":" + String(h) + "," + "\"temp\":" + String(temp) + "," + "\"groundH\":" + String(groundH) + "," + "\"light\":" + String(light) + "}");  
  String payload = http.getString();                  

  Serial.print(httpCode);   
  Serial.print("  ");   
  Serial.println(payload);  
  
  http.end();  
}
