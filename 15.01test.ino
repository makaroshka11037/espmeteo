#include <ESP8266WiFi.h>
#include "DHT.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <FastBot.h>
#include <ESP8266WebServer.h>

#include "index.h"

FastBot bot("token");

#define WIFI_SSID "ssid"
#define WIFI_PASS "pass"
#define DHTTYPE DHT22

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 18000, 60000);

uint8_t DHTPin = 4;
DHT dht(DHTPin, DHTTYPE);

ESP8266WebServer server(80);

String sensorxml = "";

void setup() {
  Serial.begin(115200);
  delay(100);

  server.on("/", handleRoot);
  server.on("/gettemp", HTTP_GET, []() {
  zamerixml();
  server.send(200, "text/plain", sensorxml);
  });
  server.onNotFound(handle_NotFound);
  Serial.println("HTTP server started");


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
  
  bot.setChatID("1266681408");
  bot.sendMessage("Запуск бота...");

  timeClient.begin();
  timeClient.update();
  Serial.println("Запуск системы времени...");

  bot.showMenu("Показатели \t Время \t Статус \n Информация");


  // server.on("/led_set", led_control);
  server.on("/readtemp", zamerihttp);
  server.begin();
}





void loop() {
  server.handleClient();
  timeClient.update();
  
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  // отладка
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 30000) {
    lastPrint = millis();
    Serial.print("Текущее время: ");
    Serial.print(currentHour);
    Serial.print(":");
    if (currentMinute < 10) Serial.print("0");
    Serial.println(currentMinute);
  }
  
  if (currentHour == 12 && currentMinute == 0) {
    Serial.println("12 часов!");
    
    float temp = dht.readTemperature();
    Serial.println("температура "  + String(temp));
    float humi = dht.readHumidity();
    Serial.println("Влажность "  + String(humi));

    bot.sendMessage("12 часов дня!");

    bot.sendMessage("Temperature " + String(temp));
    bot.sendMessage("humidty " + String(humi));
    
    delay(60000);
  }

  // Обработка сообщений
  bot.tick();
  
  delay(1000);
}

void newMsg(FB_msg& msg) {
  if (msg.text == "Open Menu" || msg.isBot) {
    return; // убираем реакцию на служебное соо
  }
  
  // отладка

  // Serial.println("Текст: " + msg.text);
  // Serial.println("От: " + msg.chatID);
  // Serial.println("Username: " + msg.username);


  
  // Обработка команд из текстовых сообщений
  if (msg.text == "Показатели") {
    zameri();
    bot.sendMessage("Замер температуры и влажности выполнен", msg.chatID);
  }
  else if (msg.text == "Время") {
    timeClient.update();
    String timeStr = "Текущее время: " + String(timeClient.getHours()) + ":" + 
                    (timeClient.getMinutes() < 10 ? "0" : "") + String(timeClient.getMinutes());
    bot.sendMessage(timeStr, msg.chatID);
  }
  else if (msg.text == "Статус") {
    String status = "Статус системы:\n";
    status += "WiFi: " + String(WiFi.SSID()) + "\n";
    status += "Сигнал: " + String(WiFi.RSSI()) + " dBm\n";
    status += "Память: " + String(ESP.getFreeHeap()) + " байт";
    bot.sendMessage(status, msg.chatID);
  }
  else if (msg.text == "Информация" || msg.text == "информация") {
    bot.sendMessage("Доступные команды:\nПоказатели - замер температуры\nВремя - функция 2\nСтатус - функция 3\nИнформация - справка", msg.chatID);
  }
  // Обработка текстовых команд
  else if (msg.text == "/start" || msg.text == "/help") {
    bot.sendMessage("Бот запущен! Используйте меню ниже:", msg.chatID);
    bot.showMenu("Показатели \t Время \t Статус \n Информация", msg.chatID);
  }
  else if (msg.text == "температура" || msg.text == "Температура") {
    zameri();
    bot.sendMessage("Замер температуры выполнен", msg.chatID);
  }
  else if (msg.text == "время" || msg.text == "Время") {
    timeClient.update();
    String timeStr = "Текущее время: " + String(timeClient.getHours()) + ":" + 
                    (timeClient.getMinutes() < 10 ? "0" : "") + String(timeClient.getMinutes());
    bot.sendMessage(timeStr, msg.chatID);
  }
  else if (msg.text == "статус" || msg.text == "Статус") {
    String status = "Статус системы:\n";
    status += "WiFi: " + String(WiFi.SSID()) + "\n";
    status += "Сигнал: " + String(WiFi.RSSI()) + " dBm\n";
    status += "Память: " + String(ESP.getFreeHeap()) + " байт";
    bot.sendMessage(status, msg.chatID);
  }
  else {
    Serial.println("Неизвестная команда: " + msg.text);
    bot.sendMessage("Неизвестная команда. Используйте /help для списка команд", msg.chatID);
  }
}

void zameri() {
  float temp = dht.readTemperature();
  float humi = dht.readHumidity();
  
  Serial.println("температура "  + String(temp));
  Serial.println("Влажность "  + String(humi));
  
  // Отправляем результаты измерений
  String sensorData = "Результаты измерений:\n";
  sensorData += "Температура: " + String(temp) + "°C\n";
  sensorData += "Влажность: " + String(humi) + "%";
  bot.sendMessage(sensorData);
}

//  веб часть


void handleRoot() 
{
//  String s = webpage;
 server.send_P(200, "text/html", webpage);
}


void zamerihttp() {
  float temp = dht.readTemperature();
  float humi = dht.readHumidity();
  
  Serial.println("температура "  + String(temp));
  Serial.println("Влажность "  + String(humi));
  
  // Отправляем результаты измерений
  String sensorData = "Результаты измерений:\n";
  sensorData += "Температура: " + String(temp) + "°C\n";
  sensorData += "Влажность: " + String(humi) + "%";
  server.send(200, "text/plain", sensorData);
}

void zamerixml() {
  float temp = dht.readTemperature();
  float humi = dht.readHumidity();
  
  Serial.println("температура "  + String(temp));
  Serial.println("Влажность "  + String(humi));
  
  // Отправляем результаты измерений
  sensorxml = "Результаты измерений:\n";
  sensorxml += "Температура: " + String(temp) + "°C\n";
  sensorxml += "Влажность: " + String(humi) + "%";
}
  
/*void handle_OnConnect() {
  String s = webpage;
  Serial.println("succesfully");
  server.send(200, "text/html", s);
}
*/


void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

