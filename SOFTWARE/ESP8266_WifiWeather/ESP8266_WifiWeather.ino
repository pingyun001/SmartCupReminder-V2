#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// ============ 用户配置区 ============
#define WIFI_SSID "ziroom1802"
#define WIFI_PASS "4001001111"
#define CITY_NAME "beijing"
#define WEATHER_API_KEY "SlpKSOC0woAJJEMbR"  // 替换为你的实际API密钥
// ===================================

void setup() {
  Serial.begin(115200);
  
  // 连接WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("正在连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功");
  
  // 首次获取天气
  fetchCurrentWeather();  // 当前天气
  fetchDailyWeather();    // 未来天气
}

// 获取当前实时天气（包含湿度等信息）
void fetchCurrentWeather() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi未连接");
    return;
  }

  String url = "https://api.seniverse.com/v3/weather/now.json?key=" + 
               String(WEATHER_API_KEY) + "&location=" + CITY_NAME + "&language=zh-Hans&unit=c";

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
      JsonObject now = doc["results"][0]["now"];
      String weatherText = now["text"];
      String temperature = now["temperature"];
      String humidity = now["humidity"];        // 湿度
      String windDirection = now["wind_direction"]; // 风向
      String windScale = now["wind_scale"];     // 风力等级
      String feelsLike = now["feels_like"];     // 体感温度
      String visibility = now["visibility"];    // 能见度
      String pressure = now["pressure"];        // 气压
      String updateTime = doc["results"][0]["last_update"];
      
      Serial.println("=== 当前天气详情 ===");
      Serial.println("城市: " + String(CITY_NAME));
      Serial.println("天气: " + weatherText);
      Serial.println("温度: " + temperature + "℃");
      Serial.println("体感: " + feelsLike + "℃");
      Serial.println("湿度: " + humidity + "%");
      Serial.println("风向: " + windDirection);
      Serial.println("风力: " + windScale + "级");
      Serial.println("能见度: " + visibility + "km");
      Serial.println("气压: " + pressure + "hPa");
      Serial.println("更新: " + updateTime);
      Serial.println("===================");
    } else {
      Serial.println("JSON解析错误");
    }
  } else {
    Serial.println("HTTP请求失败: " + String(httpCode));
  }
  
  http.end();
}

// 获取未来3天天气预报
void fetchDailyWeather() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi未连接");
    return;
  }

  String url = "https://api.seniverse.com/v3/weather/daily.json?key=" + 
               String(WEATHER_API_KEY) + "&location=" + CITY_NAME + "&language=zh-Hans&unit=c&start=0&days=3";

  WiFiClientSecure client;
  client.setInsecure();
  
  HTTPClient http;
  http.begin(client, url);
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    String payload = http.getString();
    
    DynamicJsonDocument doc(2048); // 需要更大的内存来存储多天数据
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error) {
      JsonArray daily = doc["results"][0]["daily"];
      
      Serial.println("=== 未来3天天气预报 ===");
      for (int i = 0; i < daily.size(); i++) {
        JsonObject day = daily[i];
        String date = day["date"];
        String textDay = day["text_day"];
        String textNight = day["text_night"];
        String high = day["high"];
        String low = day["low"];
        String humidity = day["humidity"];
        String windDirection = day["wind_direction"];
        String windScale = day["wind_scale"];
        
        Serial.println("日期: " + date);
        Serial.println("  白天: " + textDay + " | 夜间: " + textNight);
        Serial.println("  温度: " + low + "℃ ~ " + high + "℃");
        Serial.println("  湿度: " + humidity + "%");
        Serial.println("  风向: " + windDirection + " " + windScale + "级");
        Serial.println("  ---");
      }
      Serial.println("=====================");
    } else {
      Serial.println("未来天气JSON解析错误");
    }
  } else {
    Serial.println("未来天气HTTP请求失败: " + String(httpCode));
  }
  
  http.end();
}

// 获取生活指数（可选）
void fetchLifeSuggestion() {
  if (WiFi.status() != WL_CONNECTED) return;

  String url = "https://api.seniverse.com/v3/life/suggestion.json?key=" + 
               String(WEATHER_API_KEY) + "&location=" + CITY_NAME + "&language=zh-Hans";

  WiFiClientSecure client;
  client.setInsecure();
  
  HTTPClient http;
  http.begin(client, url);
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    String payload = http.getString();
    
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error) {
//      JsonObject suggestion = doc["results"][0]["suggestion"];
//      
//      Serial.println("=== 生活指数建议 ===");
//      Serial.println("穿衣: " + suggestion["dressing"]["brief"]);
//      Serial.println("紫外线: " + suggestion["uv"]["brief"]);
//      Serial.println("洗车: " + suggestion["car_washing"]["brief"]);
//      Serial.println("感冒: " + suggestion["flu"]["brief"]);
//      Serial.println("运动: " + suggestion["sport"]["brief"]);
//      Serial.println("===================");
    }
  }
  
  http.end();
}

void loop() {
  // 每10分钟更新一次当前天气
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 600000) {
    fetchCurrentWeather();
    
    // 每30分钟更新一次未来天气（减少API调用）
    static unsigned long lastDailyUpdate = 0;
    if (millis() - lastDailyUpdate > 1800000) {
      fetchDailyWeather();
      fetchLifeSuggestion(); // 生活指数更新频率可以更低
      lastDailyUpdate = millis();
    }
    
    lastUpdate = millis();
  }
  
  delay(1000);
}
