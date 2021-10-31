#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <DHT.h>

const char* ssid = "";
const char* password = "";

#define BOT_TOKEN ""
#define CHAT_ID ""
#define DHTPIN 16
#define RelayPIN 32

DHT dht(DHTPIN, DHT11);

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));
  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      break;
    }
    String text = bot.messages[i].text;
    Serial.println(text);
    String from_name = bot.messages[i].from_name;
    if (text == "/start") {
      String welcome = "Welcome " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs:\n";
      welcome += "/temperature to get live temperature of the room\n";
      welcome += "/humidity to get the live humidity of the room\n";
      welcome += "/waterfallon to switch on the waterfall decoration\n";
      welcome += "/waterfalloff to switch off the waterfall decoration\n";
      bot.sendMessage(chat_id, welcome, "");
    }
    if (text == "/temperature") {
      float temp = dht.readTemperature();
      String temp_text = "The present room temperature is : " + String(temp) + "\n";
      bot.sendMessage(chat_id, temp_text, "");
      Serial.println(temp_text);
    }
    if (text == "/humidity") {
      float humi = dht.readHumidity();
      String humi_text = "The present room humidity is : " + String(humi) + "\n";
      bot.sendMessage(chat_id, humi_text, "");
      Serial.println(humi_text);
    }
    if (text == "/waterfallon") {
      String waterfallon_text = "The Waterfall has been Switched on";
      digitalWrite(RelayPIN, LOW);
      bot.sendMessage(chat_id, waterfallon_text, "");
      Serial.println(waterfallon_text);
    }
    if (text == "/waterfalloff") {
      String waterfalloff_text = "The Waterfall has been Switched off";
      digitalWrite(RelayPIN, HIGH);
      bot.sendMessage(chat_id, waterfalloff_text, "");
      Serial.println(waterfalloff_text);
    }
  }
}

void setupStartMessage() {
  String setupStartMessage = "Hello and Welcome,\nyour bot and esp32 have been initialised. Send /start to get more info";
  bot.sendMessage(CHAT_ID, setupStartMessage, "");
}

void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(DHTPIN, INPUT);
  pinMode(RelayPIN, OUTPUT);
  digitalWrite(RelayPIN, HIGH);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected");
  
  secured_client.setInsecure();
  dht.begin();
  setupStartMessage();
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while(numNewMessages) {
      Serial.println("New Message Recieved");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
