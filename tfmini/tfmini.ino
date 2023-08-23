#include "header.hpp"  // Include any required header files

// Create SoftwareSerial objects for communication with sensors
SoftwareSerial tf_first(17, 16);
SoftwareSerial tf_second(18, 5);
SoftwareSerial tf_third(26, 27);

DynamicJsonDocument doc(98304);  // Create a JSON document to store sensor data
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

void setup() {
  Serial.begin(115200);  // Initialize the serial monitor

  WiFi.begin(ssid, password);  // Connect to Wi-Fi network
  Serial.print("Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi : ");

  // Allocate memory for serial buffers
  Serial.println("Allocating memory");
  serialBufferFirst = (byte *)malloc(SERIAL_BUFFER_SIZE);
  serialBufferSecond = (byte *)malloc(SERIAL_BUFFER_SIZE);
  serialBufferThird = (byte *)malloc(SERIAL_BUFFER_SIZE);

  // Initialize NTP client
  Serial.println("NTP BEGIN");
  timeClient.begin();
  timeClient.update();  // Fetch initial time
                        // Command data for setting

  // Create ReadTFMini Task on Core 1
  Serial.println("Create ReadTFMini Task");
  xTaskCreatePinnedToCore(ReadTFMini, "ReadTFMini", 10000, NULL, 10, NULL, 1);

  // Create Stepper Task on Core 1
  Serial.println("Create Stepper Task");
  xTaskCreatePinnedToCore(Stepper, "Stepper", 2048, NULL, 2, NULL, 1);

  // Create Sending Task on Core 0
  Serial.println("Create Sending Task");
  xTaskCreatePinnedToCore(Sending, "Sending", 10000, NULL, 1, NULL, 0);
}

void loop() {
  // The loop is not used as tasks are running concurrently.
}
