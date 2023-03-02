#include <TroykaLight.h>
#include <DHT.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <microDS3231.h>
MicroDS3231 rtc;

 
const char* filename = "/samplefile.txt";
 
 
#define BOTtoken "сюда_токен_бота"
 
TroykaLight sensorLight(A0);
 
const char* ssid = "6GStation";
const char* password = "18234567";
 
String readings = "";
 
int minH = 120;
 
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
 
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
 
uint8_t DHTPin = D3;
 
DHT dht(DHTPin, DHT22);
 
 
int power = 0;
 
 
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
 
bool minHFlag = false;
unsigned long long WaterStartTime = 0;
 
unsigned long long LastHydrotime = 0;
unsigned long hydroDelay = 10;
 
unsigned long lastRead = 0;
 
char plant = 0;
 
float h;
float temp;
void handleNewMessages(int numNewMessages) {
  //  Serial.println("handleNewMessages");
  //  Serial.println(String(numNewMessages));
 
 
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    Serial.println(text);
    String from_name = bot.messages[i].from_name;
    Serial.println(from_name);
    
    if (text == "/start") {
      String welcome = "Привет " + from_name + ", я - теплица. \nВы можете выбрать растение, изначально выбран кактус. \nТакже вы можете узнать показатели датчиков теплицы. \nУдачного пользования!";
      String keyboardJson = "[[\"выбрать растение\", \"показатели датчиков\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, welcome, "", keyboardJson, true);
    }
    else if (text == "показатели датчиков") {
      bot.sendMessage(chat_id, readings, "");
      Serial.print(readings);
    }
    /*if (text[1] == 'w')
    {
      bool f = false;
      for (int i = 0; i < 256; ++i)
      {
        if (text == "/w " + String(i))
        {
          Serial1.print(i);
          bot.sendMessage(chat_id, "Оки", "");
          f = true;
          break;
        }
      }
      if(!f) bot.sendMessage(chat_id, "Ошибка, введите число от 0 до 255", "");
    }*/
    else if(text == "выбрать растение")
    {
      String keyboardJson = "[[\"алоэ\", \"кактус\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, "Можете выбрать растение", "", keyboardJson, true);
    }
    else if(text == "кактус")
    {
      SPIFFS.remove(filename);
      delay(500);
      File f = SPIFFS.open(filename, "w+");
      delay(100);
      f.print('1');
      f.close();
      String keyboardJson = "[[\"выбрать растение\", \"показатели датчиков\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, "Вы выбрали кактус, ваше растение будет поливаться каждые 10 минут", "", keyboardJson, true);
      hydroDelay = 10;
    }
    else if(text == "алоэ")
    {
      SPIFFS.remove(filename);
      delay(500);
      File f = SPIFFS.open(filename, "w+");
      delay(100);
      f.print('2');
      f.close();
      String keyboardJson = "[[\"выбрать растение\", \"показатели датчиков\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, "Вы выбрали алоэ, ваше растение будет поливаться каждые 5 минут", "", keyboardJson, true);
      hydroDelay = 5;
    }
  }
 
//    while(numNewMessages) {
//      Serial.println("got response");
//      handleNewMessages(numNewMessages);
//      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
//    }
  lastTimeBotRan = millis();
}
 
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(D0, OUTPUT);
  pinMode(D6, OUTPUT);
  dht.begin();
  client.setInsecure();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  LastHydrotime = millis();
 
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Подключение к Wi-Fi...");
  }
 
  
 
  //Initialize File System
  if(SPIFFS.begin())
  {
    Serial.println("SPIFFS Initialize....ok");
  }
  else
  {
    Serial.println("SPIFFS Initialization...failed");
  }
 
  //Create New File And Write Data to It
  //w=Write Open file for writing
  File f = SPIFFS.open(filename, "w+");
  
  if (!f) {
    Serial.println("file open failed");
  }
  else
  {
      //Write data to file
      if(f.size() > 0)
      {
        plant = f.read();
      }
      else
      {
        f.print('1');
      }
      f.close();  //Close file
  }
  if(plant == 1)
  {
    hydroDelay = 10;
  }
  else if(plant == 2)
  {
    hydroDelay = 5;
  }
  h = dht.readHumidity();
  temp = dht.readTemperature();
  
}
 
void loop() {
  // put your main code here, to run repeatedly:
  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
 
    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
  }
  if(millis() > lastRead + 5000)
  {
    h = dht.readHumidity();
    temp = dht.readTemperature();
    lastRead = millis();
  }
  int groundH = Serial.read() << 2;
  sensorLight.read();
  float light = sensorLight.getLightLux();
  if(light < 30.0){
    digitalWrite(D6, HIGH);
    Serial.println("ledon");
  }else{
    digitalWrite(D6, LOW);
  }
  readings = "Влажность воздуха = " + String(h) + "%\nТемпература = " + String(temp) + " градусов\nВлажность почвы = " + String((float)100.0 - ((float)groundH - 20) / 10.0) + "%\nОсвещенность = " + String(light) + " люксов";
  Serial.println(light);
  
  
  if (minHFlag == true &&  millis() - WaterStartTime > 5000)
  {
    Serial.println("NO");
    digitalWrite(D0, LOW);
    minHFlag = false;
  }
  if ((groundH > minH || rtc.getMinutes() % (60 - hydroDelay) > (LastHydrotime + hydroDelay) % (60 - hydroDelay)) && minHFlag == false)
  {
    Serial.println("YES");
    digitalWrite(D0, HIGH);
    minHFlag = true;
    WaterStartTime = millis();
 
    LastHydrotime = rtc.getMinutes();
  }
 
}
