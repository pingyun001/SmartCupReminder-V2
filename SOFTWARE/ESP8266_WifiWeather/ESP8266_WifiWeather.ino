#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// ============ 用户配置区 ============
#define WEATHER_API_KEY "SlpKSOC0woAJJEMbR"  // 替换为你的实际心知天气API密钥
// ===================================

String wifiName = "";
String wifiPassword = "";
bool wifiNameReceived = false;
bool wifiPasswordReceived = false;
const unsigned long WIFI_TIMEOUT = 30000;
String cityName = "";
bool cityNameReceived = false;

// NTP配置
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp1.aliyun.com", 8 * 3600, 60000);
// 参数说明：NTP服务器，时区偏移（秒），更新间隔（毫秒）

void waitForWiFiConfig(void);
void connectToWiFi();
void waitForPositionConfig(void);
void getTimeSimple(int &hour, int &minute);

void setup() {
  /* 初始化串口 */
  Serial.begin(9600);
  Serial.println("\n\n---Lime ESP8266 wifi weather kit start...");
  Serial.println("---version:1.0.0");
  Serial.println("---CompiledTime:" + String(__DATE__) + " at " + String(__TIME__));

  /* 等待通过串口获取位置信息 */
  waitForPositionConfig();
    
  /* 等待通过串口获取wifi信息 */
  waitForWiFiConfig();

  /* 尝试连接wifi */
  connectToWiFi();

  /* 首次获取时间 */
  timeClient.begin();
  fetchCurrentTime();
  
  /* 首次获取天气 */
  fetchCurrentWeather();  // 当前天气
  fetchDailyWeather();    // 未来天气
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim(); // 去除首尾空白字符
    
    if (input.equals("set_refreshnow")) {
      Serial.println("Weather refresh command received, starting refresh...");

      fetchCurrentTime();     // 当前时间
      fetchCurrentWeather();  // 当前天气
      fetchDailyWeather();    // 未来天气
    }
  }
  
  
  delay(1000);
}

// 获取当前实时时间
void fetchCurrentTime() {
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("---nowtime:");
  Serial.println(formattedTime);
}

// 获取当前实时天气
void fetchCurrentWeather() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return;
  }

  String url = "https://api.seniverse.com/v3/weather/now.json?key=" + 
               String(WEATHER_API_KEY) + "&location=" + cityName + "&language=zh-Hans&unit=c";

  WiFiClientSecure client;
  client.setInsecure();
  
  HTTPClient http;
  http.begin(client, url);
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    String payload = http.getString();
    
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error) {
      JsonObject results0 = doc["results"][0];
      JsonObject location = results0["location"];
      JsonObject now = results0["now"];
      
      // 从JSON中提取实际存在的字段
      String cityName = location["name"].as<String>();
      String weatherText = now["text"].as<String>();
      String temperature = now["temperature"].as<String>();
      String updateTime = results0["last_update"].as<String>();
      
      Serial.println("=== Current Weather Details ===");
      Serial.println("---City:" + cityName);
//      Serial.println("---Weather:" + weatherText);
      Serial.println("---Temperature:" + temperature);
      
      // 只打印JSON中实际存在的字段
      if (now.containsKey("feels_like")) {
        String feelsLike = now["feels_like"].as<String>();
        Serial.println("Feels Like: " + feelsLike + "℃");
      }
      
      if (now.containsKey("humidity")) {
        String humidity = now["humidity"].as<String>();
        Serial.println("Humidity: " + humidity + "%");
      }
      
      if (now.containsKey("wind_direction")) {
        String windDirection = now["wind_direction"].as<String>();
        Serial.println("Wind Direction: " + windDirection);
      }
      
      if (now.containsKey("wind_scale")) {
        String windScale = now["wind_scale"].as<String>();
        Serial.println("Wind Scale: " + windScale + "级");
      }
      
      if (now.containsKey("visibility")) {
        String visibility = now["visibility"].as<String>();
        Serial.println("Visibility: " + visibility + "km");
      }
      
      if (now.containsKey("pressure")) {
        String pressure = now["pressure"].as<String>();
        Serial.println("Pressure: " + pressure + "hPa");
      }
      
      Serial.println("Update Time: " + updateTime);
    } else {
      Serial.println("JSON Parsing Error");
    }
  } else {
    Serial.println("HTTP Request Failed: " + String(httpCode));

    Serial.println("Daily weather HTTP request failed: " + String(httpCode));
    String payload = http.getString();
    
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);

    Serial.println("Payload:3");
    Serial.println(payload);
  }
  
  http.end();
}

// 获取未来3天天气预报
void fetchDailyWeather() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return;
  }

  String url = "https://api.seniverse.com/v3/weather/daily.json?key=" + 
               String(WEATHER_API_KEY) + "&location=" + cityName + "&language=zh-Hans&unit=c&start=0&days=3";

  WiFiClientSecure client;
  client.setInsecure();
  
  HTTPClient http;
  http.begin(client, url);
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    String payload = http.getString();
    
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);

//    Serial.println("Payload:2");
//    Serial.println(payload);
    
    if (!error) {
      JsonArray daily = doc["results"][0]["daily"];
      
      Serial.println("=== 3-Day Weather Forecast ===");
    
      for (int i = 0; i < daily.size(); i++) {
        JsonObject day = daily[i];
        
        // 检查并提取实际存在的字段
        if (day.containsKey("date")) {
          Serial.println("---Date: " + day["date"].as<String>());
        }

        if (day.containsKey("code_day")) {
          Serial.println("---code_day:" + day["code_day"].as<String>() + "");
        }
        
        if (day.containsKey("text_day")) {
          Serial.println("---Day:" + day["text_day"].as<String>());
        }
        
//        if (day.containsKey("text_night")) {
//          Serial.println("---Night: " + day["text_night"].as<String>());
//        }
        
        if (day.containsKey("high") && day.containsKey("low")) {
          Serial.println("---TemperatureLow:" + day["low"].as<String>());
          Serial.println("---TemperatureHigh:" + day["high"].as<String>());
        } else if (day.containsKey("high")) {
          Serial.println("---TemperatureHigh:" + day["high"].as<String>());
        } else if (day.containsKey("low")) {
          Serial.println("---TemperatureLow:" + day["low"].as<String>());
        }
        
        if (day.containsKey("humidity")) {
          Serial.println("---Humidity:" + day["humidity"].as<String>());
        }
        
//        if (day.containsKey("wind_direction") && day.containsKey("wind_scale")) {
//          Serial.println("---Wind: " + day["wind_direction"].as<String>() + " " + day["wind_scale"].as<String>() + "级");
//        } else if (day.containsKey("wind_direction")) {
//          Serial.println("---Wind Direction: " + day["wind_direction"].as<String>());
//        } else if (day.containsKey("wind_scale")) {
//          Serial.println("---Wind Scale: " + day["wind_scale"].as<String>() + "级");
//        }
//        
//        if (day.containsKey("wind_speed")) {
//          Serial.println("---Wind Speed: " + day["wind_speed"].as<String>() + "km/h");
//        }
        
//        if (day.containsKey("rainfall")) {
//          Serial.println("---Rainfall: " + day["rainfall"].as<String>() + "mm");
//        }
//        
//        if (day.containsKey("precip")) {
//          Serial.println("---Precipitation: " + day["precip"].as<String>() + "mm");
//        }
        
        Serial.println("---end");
      }
      Serial.println("=====================");
    } else {
      Serial.println("Daily weather JSON parsing error");
    }
  } else {
    Serial.println("Daily weather HTTP request failed: " + String(httpCode));
    String payload = http.getString();
    
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);

    Serial.println("Payload:4");
    Serial.println(payload);
  }
  
  http.end();
}

void waitForWiFiConfig(void) {
  Serial.println("waiting wifi para...");
  
  while (!(wifiNameReceived && wifiPasswordReceived)) {
    if (Serial.available() > 0) {
      String input = Serial.readStringUntil('\n');
      input.trim(); // 去除首尾空白字符
      
      // 解析WiFi名称
      if (input.startsWith("set_wifiname:")) {
        wifiName = input.substring(13); // 跳过"set_wifiname:"共13个字符
        wifiNameReceived = true;
        Serial.print("wifi name setted: ");
        Serial.println(wifiName);
      }
      // 解析WiFi密码
      else if (input.startsWith("set_wifipasswd:")) {
        wifiPassword = input.substring(15); // 跳过"set_wifipasswd:"共15个字符
        wifiPasswordReceived = true;
        Serial.print("wifi password setted: ");
        Serial.println(wifiPassword);
      }
      // 无效命令提示
      else if (input.length() > 0) {
        Serial.println("wrong para, please use:");
        Serial.println("set_wifiname:<your wifi name>");
        Serial.println("set_wifipasswd:<your wifi password>");
      }
    }
    
    delay(10); // 短暂延时，避免CPU过载
  }
  
  Serial.println("wifi prar receive finished");
}

void connectToWiFi() {
  if (wifiName.length() == 0) {
    Serial.println("Error: WiFi name not set");
    return;
  }
  
  Serial.println("Connecting to WiFi...");
  Serial.print("SSID: ");
  Serial.println(wifiName);
  
  // 设置WiFi模式
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiName.c_str(), wifiPassword.c_str());
  
  unsigned long startTime = millis();
  bool connected = false;
  
  // 尝试连接，带超时检测
  while (millis() - startTime < WIFI_TIMEOUT) {
    if (WiFi.status() == WL_CONNECTED) {
      connected = true;
      break;
    }
    
    // 显示连接进度
    static int lastDotCount = 0;
    int dotCount = (millis() - startTime) / 500; // 每500ms显示一个点
    if (dotCount > lastDotCount) {
      Serial.print(".");
      lastDotCount = dotCount;
    }
    
    delay(100);
  }
  
  Serial.println(); // 换行
  
  // 显示连接结果
  if (connected) {
    Serial.println("---WiFi connected:ok");
    Serial.print("---IP address:");
    Serial.println(WiFi.localIP());
    Serial.print("---Signal strength (RSSI):");
    Serial.print(WiFi.RSSI());
    Serial.println("dBm");
  } else {
    Serial.println("---WiFi connection:failed");
    Serial.print("---Failure reason:");
    
    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        Serial.println("---Network not found");
        break;
      case WL_CONNECT_FAILED:
        Serial.println("---Incorrect password");
        break;
      case WL_CONNECTION_LOST:
        Serial.println("---Connection lost");
        break;
      case WL_DISCONNECTED:
        Serial.println("---Disconnected");
        break;
      default:
        Serial.print("---Error code:");
        Serial.println(WiFi.status());
        break;
    }
  }
  
  Serial.println("==================================");
}

void waitForPositionConfig(void) {
  Serial.println("Waiting for position configuration...");
  
  cityNameReceived = false;
  cityName = "";
  
  while (!cityNameReceived) {
    if (Serial.available() > 0) {
      String input = Serial.readStringUntil('\n');
      input.trim();
      
      if (input.startsWith("set_position:")) {
        cityName = input.substring(13); // 跳过"set_position:"共13个字符
        cityNameReceived = true;
        Serial.print("Position set: ");
        Serial.println(cityName);
        Serial.println("Position configuration received!");
        break;
      } else if (input.length() > 0) {
        Serial.println("Invalid command. Please use: set_position:your_city_name");
      }
    }
    
    delay(10);
  }
}
void getTimeSimple(int &hour, int &minute) {
  WiFiClient client;
  HTTPClient http;
  
  // 使用百度时间API
  http.begin(client, "https://aip.baidubce.com/oauth/2.0/timestamp");
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    String response = http.getString();
    // 简单的字符串处理提取时间戳
    int start = response.indexOf(':') + 1;
    int end = response.indexOf('}');
    String timestampStr = response.substring(start, end);
    
    unsigned long timestamp = timestampStr.toInt();
    timestamp += 8 * 3600; // 转换为北京时间
    
    // 计算小时和分钟
    hour = (timestamp % 86400) / 3600;
    minute = (timestamp % 3600) / 60;
  } else {
    hour = -1;
    minute = -1;
  }
  http.end();

  Serial.println("hour:"+String(hour)+"minute:"+String(minute));
}
