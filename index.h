#pragma once

const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Метеостанция</title>
    <style type="text/css">
        .button {
            background-color: #3498db;
            border: none;
            color: white;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin: 10px;
            cursor: pointer;
            border-radius: 5px;
        }
        body {
            background-color: #f9e79f;
            font-family: Arial, sans-serif;
        }
        h1 {
            color: #2c3e50;
        }
        h2 {
            color: #34495e;
        }
    </style>
</head>
<body>
    <center>
        <div>
            <h1>METEOSTATION PANEL</h1>
            <button class="button" id="knopka" onclick="requestCustomData()">Получить данные</button>
        </div>
        <br>
        <div>
            <h2>
                <span id="custom_data">Привет!</span>
            </h2>
        </div>

        <script>
    const button = document.getElementById('knopka');

    function requestCustomData() {
        button.innerText = 'В процессе...';

        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4) {
                if (this.status == 200) {
                    document.getElementById("custom_data").innerHTML = this.responseText;
                } else {
                    document.getElementById("custom_data").innerHTML = "Ошибка";
                }
                button.innerText = 'Получить данные'; // ← ТОЛЬКО ТУТ
            }
        };

        xhttp.open("GET", "/gettemp", true);
        xhttp.send();
    }
</script>
    </center>
</body>
</html>
)rawliteral";