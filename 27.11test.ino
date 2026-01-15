#include <ESP8266WiFi.h>
#include "DHT.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <FastBot.h>
#include <ESP8266WebServer.h>

FastBot bot("paste");

#define WIFI_SSID "paste"
#define WIFI_PASS "P@paste"
#define DHTTYPE DHT22

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 18000, 60000);

uint8_t DHTPin = 4;
DHT dht(DHTPin, DHTTYPE);

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  delay(100);

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
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
  
  bot.setChatID("paste");
  bot.sendMessage("Запуск бота...");

  timeClient.begin();
  timeClient.update();
  Serial.println("Запуск системы времени...");

  bot.showMenu("temperature \t func2 \t func3 \n info");
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
  
  if (currentHour == 17 && currentMinute == 21) {
    Serial.println("12 часов!");
    
    float temp = dht.readTemperature();
    Serial.println("температура "  + String(temp));
    float humi = dht.readHumidity();
    Serial.println("Влажность "  + String(humi));

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
  if (msg.text == "temperature") {
    zameri();
    bot.sendMessage("Замер температуры и влажности выполнен", msg.chatID);
  }
  else if (msg.text == "func2") {
    bot.sendMessage("Функция в разработке", msg.chatID);
  }
  else if (msg.text == "func3") {
    bot.sendMessage("Функция в разработке", msg.chatID);
  }
  else if (msg.text == "info") {
    bot.sendMessage("Доступные команды:\ntemperature - замер температуры\nfunc2 - функция 2\nfunc3 - функция 3\ninfo - справка", msg.chatID);
  }
  // Обработка текстовых команд
  else if (msg.text == "/start" || msg.text == "/help") {
    bot.sendMessage("Бот запущен! Используйте меню ниже:", msg.chatID);
    bot.showMenu("temperature \t func2 \t func3 \n info", msg.chatID);
  }
  else if (msg.text == "/temp" || msg.text == "температура") {
    zameri();
    bot.sendMessage("Замер температуры выполнен", msg.chatID);
  }
  else if (msg.text == "/time" || msg.text == "время") {
    timeClient.update();
    String timeStr = "Текущее время: " + String(timeClient.getHours()) + ":" + 
                    (timeClient.getMinutes() < 10 ? "0" : "") + String(timeClient.getMinutes());
    bot.sendMessage(timeStr, msg.chatID);
  }
  else if (msg.text == "/status" || msg.text == "статус") {
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

void handle_OnConnect() {

  Serial.println("succesfully");
  server.send(200, "text/html", SendHTML()); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}


String SendHTML() {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP32 Web Server</h1>\n";
    ptr +="<h3>Using Station(STA) Mode</h3>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}