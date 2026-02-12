# espmeteo

С помощью этого проекта вы можете измерять давление и температуру в вашем доме. Отправка данных будет происходить в определенное время, в Telegram-бота.

## Компоненты
1. Wemos D1 mini.
2. DHT22.

## Сборка
Подключение dht22:

  Plus   -> 3v3
  
  out -> D2
  
  Minus   -> GND

## Установка 
Для установки и настройки бота:
  Пока что доступна только solo версия(без шлюза), скачиваем 1.0stable.zip
  Откройте main.ino файл. Отредактируйте переменные токена бота, часового пояса, имени сети и пароля вайфай.

  Установите библиотеки указанные [внизу ReadMe](#Libraries).

  Загрузите код на плату.

## Libraries
 [ESP8266WiFI](https://github.com/sergionorenap/ESP8266WiFi.h)
 [DHT](https://github.com/adafruit/DHT-sensor-library)
 [NTPClient](https://github.com/arduino-libraries/NTPClient)
 [WIfiUdp](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiUdp.h)
 [FastBot](https://github.com/GyverLibs/FastBot)
 [ESP8266WebServer](https://github.com/esp8266/ESPWebServer/blob/master/src/ESP8266WebServer.h)
 
