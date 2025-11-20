#include <ESP8266WiFi.h>
#include "DHT.h"

#include <NTPClient.h>
#include <WiFiUdp.h>



#include <FastBot.h>
FastBot bot("yourtoken");

#define WIFI_SSID "ssid"
#define WIFI_PASS "pass"

#define DHTTYPE DHT22

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 18000, 60000); // change 60000 to 3600 * your utc




uint8_t DHTPin = 4;

DHT dht(DHTPin, DHTTYPE);






void setup() {
  Serial.begin(115200);

  delay(100);
  pinMode(DHTPin, INPUT);
  dht.begin();             

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");
  Serial.println(WiFi.localIP());

  bot.attach(newMsg);


  Serial.println("Start");

  
  bot.setChatID("your personal chat id");

  bot.sendMessage("Запусск бота...");


  timeClient.begin();
  timeClient.update();
  Serial.println("Запуск системы времени...");
  // запуск датчика и проверка на работоспособность
}

void loop() {
  // температура

  timeClient.update();
  
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  // Правильная запись для минуты 03

  // Вывод текущего времени для отладки
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 30000) {
    lastPrint = millis();
    Serial.print("Текущее время: ");
    Serial.print(currentHour);
    Serial.print(":");
    if (currentMinute < 10) Serial.print("0");
    Serial.println(currentMinute);
  }

  if (currentHour == 17 && currentMinute == 21) {
    Serial.println("12 часов!");

  float temp = dht.readTemperature();
  Serial.println("температура "  + String(temp));
  float humi = dht.readHumidity();
  Serial.println("Влажность "  + String(humi));

  bot.sendMessage("Temperature " + String(temp));

  // влажность
  
  bot.sendMessage("humidty " + String(humi));

  // давление
  
  delay(60000);
  }


  Serial.println();
  delay(10000);
  
}

void newMsg(FB_msg& msg) {
  // отправить сообщение обратно
  bot.sendMessage(msg.text, msg.chatID);  
}
