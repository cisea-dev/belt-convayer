#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <SoftwareSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#define SERIAL_BUFFER_SIZE 255
#define UPDATE_BUF_SIZE 1024

const char *updateTime = "00:01";
const char *ssid = "Detector-Vibration";
const char *password = "@DV2023_";
const char *serverUrl = "https://plane.bukitasam.co.id/";  // Replace with your server URL
const char* updateServerHost = "s3.bukitasam.co.id";
const char* firmwarePath = "/firmware.bin";
const int updateServerPort = 80;
const int MIN_DATA_SIZE = 9;
const byte HEADER_BYTE = 0x59;
unsigned long wifiReconnectStartTime = 0;  // Store the start time for WiFi reconnection
const unsigned long wifiReconnectInterval = 5000;  // Reconnection interval in milliseconds
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7 * 3600; //Jakarta is GMT + 7, multiply by 3600 seconds or 1 hour
const int   daylightOffset_sec = 0;
const int   frameRate = 500;
/*ESP32*/
int PUL = 22;  //define Pulse pin
int DIR = 19;  //define Direction pin
int ENA = 21;  //define Enable Pin
int counter = 0;

/*Store Value from TFLidar*/
byte *serialBufferFirst = NULL;
byte *serialBufferSecond = NULL;
byte *serialBufferThird = NULL;

bool sending_to_server = false;
bool updateOTA = false;
String outputJSON;
String datetime;
