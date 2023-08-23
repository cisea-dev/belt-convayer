/**
 * Task to read distance measurements from three TF Mini LiDAR sensors and send data to a server.
 * @param pvParameter - A pointer to the task parameter (not used in this function).
 * @return void
 */
void ReadTFMini(void *pvParameter) {
  // Initialize lidar with baud rate
  Serial.println("TF Lidar Baudrate");
  tf_first.begin(115200);
  tf_second.begin(115200);
  tf_third.begin(115200);
  Serial.println("TF Lidar Command Data");
  // this command for setting the format data
  // 0x00, 0x00, 0x00, 0x5A, 0x05, 0x05, 0x01, 0x65    for centimeter
  // 0x00, 0x00, 0x00, 0x5A, 0x05, 0x05, 0x02, 0x66    for Pixhawk (meter)
  // 0x00, 0x00, 0x00, 0x5A, 0x05, 0x05, 0x06, 0x6A    for milimeter
  byte dataFormat[]= { 0x00, 0x00, 0x00, 0x5A, 0x05, 0x05, 0x06, 0x6A }; 
  int dataSize = sizeof(dataFormat);
  for (int i = 0; i < dataSize; i++) {
    tf_first.write(dataFormat[i]);
    tf_second.write(dataFormat[i]);
    tf_third.write(dataFormat[i]);
  }
  
  //setting the frameRate, see value frameRate on header.hpp
  int n = 1000 / frameRate;
  byte HH = n >> 8;
  byte LL = n & 0xFF;
  byte checksum = (0x5A + 0x06 + 0x03 + LL + HH) & 0xFF;
  // Send the command
  byte dataCommand[] = { 0x00, 0x00, 0x5A + 0x06 + 0x03, LL, HH, checksum }; 
  dataSize = sizeof(dataCommand);
  for (int i = 0; i < dataSize; i++) {
    tf_first.write(dataCommand[i]);
    tf_second.write(dataCommand[i]);
    tf_third.write(dataCommand[i]);
  }
  
  static int ms = millis();
  static int mss = millis();
  int tmp = 0;
  while (1) {
    //*
    while (updateOTA) {
      //do nothing
      vTaskDelay(pdMS_TO_TICKS(10));
    }
    unsigned int distance_f = readTF_First();
    unsigned int distance_s = readTF_Second();
    unsigned int distance_t = readTF_Third();
    if (distance_f != 0 || distance_s != 0 || distance_t != 0) {
      counter++;
      // Serial.printf("%d. \t%d \t%d \t%d\n", counter, distance_f, distance_s, distance_t);
    }
    if (distance_f != 0) {
      JsonObject doc_0 = doc.createNestedObject();
      doc_0["id"] = 1;
      JsonArray doc_0_val = doc_0.createNestedArray("val");
      doc_0_val.add(distance_f);
    }

    if (distance_s != 0) {
      JsonObject doc_1 = doc.createNestedObject();
      doc_1["id"] = 2;
      JsonArray doc_1_val = doc_1.createNestedArray("val");
      doc_1_val.add(distance_s);
    }

    if (distance_s != 0) {
      JsonObject doc_2 = doc.createNestedObject();
      doc_2["id"] = 3;
      JsonArray doc_2_val = doc_2.createNestedArray("val");
      doc_2_val.add(distance_t);
    }

    if (millis() - mss >= 1000) {
      Serial.printf("Total Perdetik : %d\n", counter - tmp);
      mss = millis();
      tmp = counter;
    }
    if (millis() - ms >= 10000) {
      Serial.printf("Total : %d\n", counter);
      counter = 0;
      tmp = counter;
      ms = millis();
      serializeJson(doc, outputJSON);
      sending_to_server = true;
      doc.clear();
    }
    vTaskDelay(pdMS_TO_TICKS(1));  // Non-blocking delay
                                   //*/
  }
}

/**
 * Task to control the stepper motor for rotation.
 * @param pvParameter - A pointer to the task parameter (not used in this function).
 * @return void
 */
void Stepper(void *pvParameter) {
  /*
    200 step      ->  45 degree
    177.78 steps  ->  40 degree
    133.33 steps  ->  30 degree
    111.11 step   ->  25 degree
    88.89 step    ->  20 degree
  */

  pinMode(PUL, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(ENA, OUTPUT);
  while (1) {
    while (updateOTA) {
      //do nothing
      vTaskDelay(pdMS_TO_TICKS(10));
    }
    // Perform one revolution using 200 steps
    Serial.println("\nRotation Clockwise");
    for (int i = 0; i < 90; i++) {
      digitalWrite(DIR, LOW);        // Set direction for forward rotation
      digitalWrite(ENA, HIGH);       // Enable the motor
      digitalWrite(PUL, HIGH);       // Generate a step pulse
      vTaskDelay(pdMS_TO_TICKS(2));  // Delay to control step rate (adjust as needed)
      digitalWrite(PUL, LOW);        // Complete the step pulse
      vTaskDelay(pdMS_TO_TICKS(2));  // Delay to control step rate (adjust as needed)
    }
    vTaskDelay(pdMS_TO_TICKS(100));  // Delay between revolutions (adjust as needed)
                                     // Perform one revolution using 200 steps

    Serial.println("\nRotation Counter Clockwise");
    for (int i = 0; i < 90; i++) {
      digitalWrite(DIR, HIGH);       // Set direction for forward rotation
      digitalWrite(ENA, HIGH);       // Enable the motor
      digitalWrite(PUL, HIGH);       // Generate a step pulse
      vTaskDelay(pdMS_TO_TICKS(2));  // Delay to control step rate (adjust as needed)
      digitalWrite(PUL, LOW);        // Complete the step pulse
      vTaskDelay(pdMS_TO_TICKS(2));  // Delay to control step rate (adjust as needed)
    }
    vTaskDelay(pdMS_TO_TICKS(100));  // Delay between revolutions (adjust as needed)
  }
}

/**
 * Task to send data to a server.
 * @param pvParameter - A pointer to the task parameter (not used in this function).
 * @return void
 */
void Sending(void *pvParameter) {
  while (1) {
    if (WiFi.status() != WL_CONNECTED) {
      if (millis() - wifiReconnectStartTime >= wifiReconnectInterval) {
        wifiReconnectStartTime = millis();
        Serial.println("Reconnecting to WiFi...");
        WiFi.disconnect();
        WiFi.reconnect();
      }
    }
    if (sending_to_server && !updateOTA) {
      Serial.println("Sending data");
      HTTPClient http;

      // Make a POST request
      http.begin(serverUrl);                               // Specify the URL
      http.addHeader("Content-Type", "application/json");  // Set content type
      String postData = outputJSON;                        // JSON data to send

      // Send POST request with data
      int httpResponseCode = http.POST(postData);
      Serial.println("httpResponseCode : " + String(httpResponseCode));

      if (httpResponseCode > 200) {
        Serial.printf("HTTP Response code: %d\n", httpResponseCode);
        String response = http.getString();
        Serial.println(response);
        sending_to_server = false;
      }
      http.end();  // Close connection
      vTaskDelay(pdMS_TO_TICKS(100));
    }
    shouldUpdate();
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

/**
 * Checks if an OTA (Over-The-Air) update should be performed based on the current time.
 * If an update is required, it fetches firmware from the update server and initiates the update process.
 */
void shouldUpdate() {
  timeClient.update();
  String ts = timeClient.getHours() + ":" + timeClient.getMinutes();
  if (ts.equals(updateTime)) {
    updateOTA = true;
    vTaskDelay(pdMS_TO_TICKS(1000));  // Non-blocking delay
    Serial.println("Performing OTA update...");
    // Connect to the update server
    WiFiClient client;
    if (!client.connect(updateServerHost, updateServerPort)) {
      Serial.println("Connection to update server failed");
      return;
    }
    // Construct the firmware URL
    String firmwareUrl = "/esp32/firmware.bin";  // Change this path if needed

    // Request the firmware file
    client.print(String("GET ") + firmwareUrl + " HTTP/1.1\r\n" + "Host: " + updateServerHost + "\r\n" + "Connection: close\r\n\r\n");

    // Check if the server responded with success
    String response = client.readStringUntil('\n');
    if (response.indexOf("200 OK") == -1) {
      Serial.println("Failed to fetch firmware");
      return;
    }

    // Start update process
    size_t size = 0;
    Update.begin();
    uint8_t updateBuf[UPDATE_BUF_SIZE];
    while (client.connected()) {
      if (client.available()) {
        size_t len = client.readBytes(updateBuf, sizeof(updateBuf));
        Update.write(updateBuf, len);
        size += len;
      }
    }
    Update.end();

    if (size > 0) {
      Serial.println("OTA update completed");
      vTaskDelay(pdMS_TO_TICKS(50000));  // Non-blocking delay
      ESP.restart();
    } else {
      Serial.println("OTA update failed");
    }
  }
}