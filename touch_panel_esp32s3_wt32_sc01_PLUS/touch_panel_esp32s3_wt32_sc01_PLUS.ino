
// Smart switch, watch & weather station
// Dev Board : ESP32S3 WT32 SC01 PLUS
// Code by : ScitiveX

// Library : LovyanGFX
// https://github.com/lovyan03/LovyanGFX

#include "wt32setup.h" // setup for wt32 PLUS version
#include "sprites.h"
#include "icons.h"
#include "bg.h" // background image
#include "NotoSansMonoSCB20.h"  // font
#include "Latin_Hiragana_24.h"  // font

#define sans20 NotoSansMonoSCB20
#define latin Latin_Hiragana_24

LGFX tft;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 5 * 3600 + 30 * 60;       // GMT offset in seconds (+5 hours, +30 minutes)
const int   daylightOffset_sec = 0;                  // Daylight offset in seconds (India doesn't observe daylight saving time)


#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <time.h>
#include <HTTPClient.h>

const char* ssid = "-----"; // wifi name
const char* password = "-------"; // wifi password
const char* apikey = "-------"; // from openweather
const char* city = "--------"; // your city name

int temperature, humidity, clouds;
String weatherDescription, main_w ;
float latitude, longitude;

boolean tog = 0;
#define PIN_TOUCH_RES 4
#define PIN_TOUCH_INT 7

static int32_t x, y;
static LGFX_Sprite button1_sprite, button2_sprite, button3_sprite;
static LGFX_Sprite bg_sprite, iconSprite;
static LGFX_Sprite ind1, ind2, ind3;

const int led1 = 10, led2 = 11, led3 = 12;


#define bgcolor1 0x4A69  // color for background text
#define bgcolor2 0x632C  // color for background text

struct SpriteData {
  int x;
  int y;
  int width;
  int height;
  const char* label;
};

SpriteData buttonData[] = {
  {35, 35, 36, 59, "Button 1"},    // Button 1 coordinates and size
  {35, 35 + 97, 36, 59, "Button 2"}, // Button 2 coordinates and size
  {35, 35 + (2 * 97), 36, 59, "Button 3"}, // Button 3 coordinates and size
};

SpriteData indData[] = {
  {222, 55, 15, 15},
  {222, 55 + 97, 15, 15},
  {222, 55 + (2 * 97), 15, 15},
};


struct ImageData {
  const uint16_t* imageData;
  const char* imageName;
  int x;
  int y;
  int width;
  int height;
};

const ImageData images[] = {
  {ind_red_img, "Red Ind"},
  {ind_green_img, "Green Ind"},
  {temp_icon, "Temperature", 20, 50, 50, 48}, //2
  {hum_icon, "Humidity", 20, 80, 51, 48},//3
  {rain_icon, "Rain", 320, 185, 51, 48},//4
  {cloud_icon, "Cloud", 290, 185, 55, 39},//5
};

const String DevNames[] = {"Basement", "   Room  ", "   Hall  "};

const int numButtons = sizeof(buttonData) / sizeof(buttonData[0]); // Total number of buttons

// Function to initialize a button sprite with the provided image and coordinate
void initButtonSprite(LGFX_Sprite* buttonSprite, int x, int y) {
  buttonSprite->createSprite(buttonData[0].width, buttonData[0].height);
  buttonSprite->setSwapBytes(true);
  buttonSprite->fillSprite(TFT_PURPLE);
  buttonSprite->setTextColor(TFT_WHITE, TFT_PURPLE);
  buttonSprite->pushImage( 0, 0, buttonData[0].width, buttonData[0].height, offbulbImage);
}

void initIndSprite(LGFX_Sprite* indSprite, int x, int y) {
  indSprite->createSprite(indData[0].width, indData[0].height);
  indSprite->setSwapBytes(true);
  indSprite->fillSprite(TFT_PURPLE);
  indSprite->setTextColor(TFT_WHITE, TFT_PURPLE);
  indSprite->pushImage( 0, 0, indData[0].width, indData[0].height, ind_red_img);
}


void detectButtonClick(int x, int y) {
  for (int i = 0; i < numButtons; i++) {
    if (x >= buttonData[i].x && x < buttonData[i].x + buttonData[i].width &&
        y >= buttonData[i].y && y < buttonData[i].y + buttonData[i].height) {
      delay(10);
      tft.println("        ");
      if (i == 0) {
        digitalWrite(led1, !digitalRead(led1));
        static boolean ind1State = false;
        ind1State = !ind1State;
        if (ind1State) {
          ind1.pushImage(0, 0, indData[0].width, indData[0].height, images[1].imageData);
        } else {
          ind1.pushImage(0, 0, indData[0].width, indData[0].height, images[0].imageData);
        }

        static boolean button1State = false;
        button1State = !button1State;
        if (button1State) {
          button1_sprite.pushImage(0, 0, buttonData[0].width, buttonData[0].height, onbulbImage);
        } else {
          button1_sprite.pushImage(0, 0, buttonData[0].width, buttonData[0].height, offbulbImage);
        }
      } else if (i == 1) {
        digitalWrite(led2, !digitalRead(led2));
        static boolean ind2State = false;
        ind2State = !ind2State;
        if (ind2State) {
          ind2.pushImage(0, 0, indData[1].width, indData[1].height, images[1].imageData);
        } else {
          ind2.pushImage(0, 0, indData[1].width, indData[1].height, images[0].imageData);
        }

        static boolean button2State = false;
        button2State = !button2State;
        if (button2State) {
          button2_sprite.pushImage(0, 0, buttonData[1].width, buttonData[1].height, onbulbImage);
        } else {
          button2_sprite.pushImage(0, 0, buttonData[1].width, buttonData[1].height, offbulbImage);
        }
      } else if (i == 2) {
        digitalWrite(led3, !digitalRead(led3));
        static boolean ind3State = false;
        ind3State = !ind3State;
        if (ind3State) {
          ind3.pushImage(0, 0, indData[2].width, indData[2].height, images[1].imageData);
        } else {
          ind3.pushImage(0, 0, indData[2].width, indData[2].height, images[0].imageData);
        }

        static boolean button3State = false;
        button3State = !button3State;
        if (button3State) {
          button3_sprite.pushImage(0, 0, buttonData[2].width, buttonData[2].height, onbulbImage);
        } else {
          button3_sprite.pushImage(0, 0, buttonData[2].width, buttonData[2].height, offbulbImage);
        }
      }

      break;
    }
  }

}

void setup() {
  Serial.begin(57600);
  pinMode(led1, OUTPUT); pinMode(led2, OUTPUT); pinMode(led3, OUTPUT);
  digitalWrite(led1, 0); digitalWrite(led2, 0); digitalWrite(led3, 0);
  pinMode(PIN_TOUCH_RES, OUTPUT);
  digitalWrite(PIN_TOUCH_RES, LOW);
  delay(500);
  digitalWrite(PIN_TOUCH_RES, HIGH);
  tft.begin();
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_PURPLE);
  tft.setColorDepth(16);
  tft.setRotation(1);
  tft.pushImage(0, 0, 480, 320, bg_img);

  initButtonSprite(&button1_sprite, buttonData[0].x, buttonData[0].y);
  initButtonSprite(&button2_sprite, buttonData[1].x, buttonData[1].y);
  initButtonSprite(&button3_sprite, buttonData[2].x, buttonData[2].y);

  initIndSprite(&ind1, indData[0].x, indData[0].y);
  initIndSprite(&ind2, indData[1].x, indData[1].y);
  initIndSprite(&ind3, indData[2].x, indData[2].y);

  iconSprite.createSprite( images[5].width + 100, images[5].height);

  iconSprite.setSwapBytes(true);
  iconSprite.fillSprite(bgcolor1);
  iconSprite.setTextColor(TFT_WHITE, bgcolor1);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi!");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  fetchLocalTime();
  fetchWeatherData() ;
  draw(-1, -1);
}



void draw(int x, int y) {
  tft.pushImage(0, 0, 480, 320, bg_img);

  button1_sprite.pushSprite(&tft, buttonData[0].x, buttonData[0].y, TFT_BLACK);
  button2_sprite.pushSprite(&tft, buttonData[1].x, buttonData[1].y, TFT_BLACK);
  button3_sprite.pushSprite(&tft, buttonData[2].x, buttonData[2].y, TFT_BLACK);

  ind1.pushSprite(&tft, indData[0].x, indData[0].y, TFT_BLACK);
  ind2.pushSprite(&tft, indData[1].x, indData[1].y, TFT_BLACK);
  ind3.pushSprite(&tft, indData[2].x, indData[2].y, TFT_BLACK);

  tft.setCursor(0, 400);
  tft.setTextColor(TFT_WHITE, bgcolor1);
  tft.setTextSize(1);
  tft.unloadFont();
  tft.loadFont(sans20);
  tft.println((String)buttonData[2].x + ", " + (String)buttonData[2].y);

  dev_names();
  printWeatherData();
  printLocalTime();
}

unsigned long lastWeatherFetchTime = 0;
unsigned long lastTimeFetchTime = 0;
unsigned long anim = 0;
unsigned long weatherFetchInterval = 10000; // Fetch weather every 60 seconds
unsigned long timeFetchInterval = 1000; // Fetch time every 1 second

int prevX = -1;
int prevY = -1;
unsigned long prevTime = 0;
const unsigned long debounceDelay = 100;


void loop() {
  unsigned long currentMillis = millis();

  if (WiFi.status() == WL_CONNECTED && currentMillis - lastWeatherFetchTime >= weatherFetchInterval) {
    fetchWeatherData();
    lastWeatherFetchTime = currentMillis;
  }

  if (WiFi.status() == WL_CONNECTED && currentMillis - lastTimeFetchTime >= timeFetchInterval) {
    fetchLocalTime();
    lastTimeFetchTime = currentMillis;
  }

  int x, y;

  if (tft.getTouch(&x, &y)) {
    if (millis() - prevTime >= debounceDelay) {
      prevTime = millis();

      if (x != prevX || y != prevY) {
        prevX = x;
        prevY = y;

        detectButtonClick(x, y);
        draw(x, y);
      }
    }
  }
}

void fetchWeatherData() {
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "&appid=" + String(apikey);

  http.begin(url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    // Parse JSON response
    String payload = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    // Extract weather data
    temperature = doc["main"]["temp"].as<float>() - 273.15; // Convert temperature from Kelvin to Celsius
    humidity = doc["main"]["humidity"].as<int>();
    weatherDescription = doc["weather"][0]["description"].as<String>();
    main_w = doc["weather"][0]["main"].as<String>();
    clouds = doc["clouds"]["all"].as<int>();
    latitude = doc["coord"]["lat"].as<float>();
    longitude = doc["coord"]["lon"].as<float>();

    // Print weather data
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("Weather: ");
    Serial.println(weatherDescription);

    Serial.print("Clouds: ");
    Serial.print(clouds);
    Serial.println(" %");

    Serial.print("Latitude: ");
    Serial.println(latitude, 6); // Display latitude with 6 decimal places

    Serial.print("Longitude: ");
    Serial.println(longitude, 6); // Display longitude with 6 decimal places

  } else {
    Serial.println("Error in HTTP request");
  }

  http.end();

  printWeatherData();

}

void printWeatherData() {
  tft.setTextSize(2);
  tft.unloadFont();
  tft.loadFont(latin);
  tft.setTextColor(TFT_RED, bgcolor2);
  tft.setCursor(345, 45);
  tft.print(humidity); tft.print((char)37);
  tft.setTextColor(TFT_WHITE, bgcolor2);
  tft.setCursor(335, 110);
  tft.print(temperature);
  tft.setFont(&fonts::Font2);
  tft.print((char)96);
  tft.loadFont(latin);
  tft.print("C");
  tft.setTextSize(1);
  tft.unloadFont();
  tft.setFont(&fonts::Font0);
  tft.fillRect(280, 235 - 1, 160, 10, bgcolor1);
  tft.setTextColor(TFT_WHITE, bgcolor1);
  tft.drawString(weatherDescription, 300, 235);
  
    if (main_w == "Rain") {
      iconSprite.pushImage(0, 0, images[4].width, images[4].height, images[4].imageData);
    }
    else if (main_w == "Clouds") {
      iconSprite.pushImage(0, 0, images[5].width, images[5].height, images[5].imageData);
    }
    iconSprite.pushSprite(&tft, images[5].x, images[5].y, TFT_BLACK);

}

struct tm timeinfo;

void fetchLocalTime()
{
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  printLocalTime();
}


void printLocalTime() {
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  tft.unloadFont();
  tft.loadFont(sans20);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, bgcolor2);
  tft.setCursor(320, 263);
  tft.println(&timeinfo, "%H:%M:%S");
  //tft.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}


void dev_names() {
  tft.setTextSize(1);
  tft.setCursor(105, 53);
  tft.println(DevNames[0]);
  tft.setCursor(105, 53 + 97);
  tft.println(DevNames[1]);
  tft.setCursor(105, 53 + (2 * 97));
  tft.println(DevNames[2]);
}
