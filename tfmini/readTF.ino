/**
 * Reads distance data from the first TFMini LiDAR sensor 1 and returns the measured distance.
 * @return The measured distance in milimeter, or 0 if the data is invalid or the checksum is incorrect.
 */
unsigned int readTF_First() {
  int pos = 0;
  int numOfData = 0;
  int localDistance = 0;

  // Continue reading while there's enough data available
  while (tf_first.available() >= MIN_DATA_SIZE) {
    int n = tf_first.available();
    int ret = tf_first.readBytes(&serialBufferFirst[pos], n);
    numOfData += ret;
    pos = numOfData;
  }
  vTaskDelay(pdMS_TO_TICKS(1));  // Non-blocking delay

  // Check if the received data has a valid header and checksum
  if (serialBufferFirst[0] == HEADER_BYTE && serialBufferFirst[1] == HEADER_BYTE) {
    int totalCS = 0;
    for (int i = 0; i < 8; i++) {
      totalCS += serialBufferFirst[i];
    }
    byte checksum = (totalCS)&0xFF;
    if (checksum == serialBufferFirst[8]) {
      localDistance = (uint16_t)serialBufferFirst[3] << 8 | serialBufferFirst[2];
      return localDistance;
    }
  }
  return 0;
}

/**
 * Reads distance data from the second TFMini LiDAR sensor 2 and returns the measured distance.
 * @return The measured distance in milimeter, or 0 if the data is invalid or the checksum is incorrect.
 */
unsigned int readTF_Second() {
  int pos = 0;
  int numOfData = 0;
  int localDistance = 0;

  // Continue reading while there's enough data available
  while (tf_second.available() >= MIN_DATA_SIZE) {
    int n = tf_second.available();
    int ret = tf_second.readBytes(&serialBufferSecond[pos], n);
    numOfData += ret;
    pos = numOfData;
  }
  vTaskDelay(pdMS_TO_TICKS(1));  // Non-blocking delay

  // Check if the received data has a valid header and checksum
  if (serialBufferSecond[0] == HEADER_BYTE && serialBufferSecond[1] == HEADER_BYTE) {
    int totalCS = 0;
    for (int i = 0; i < 8; i++) {
      totalCS += serialBufferSecond[i];
    }
    byte checksum = (totalCS)&0xFF;
    if (checksum == serialBufferSecond[8]) {
      localDistance = (uint16_t)serialBufferSecond[3] << 8 | serialBufferSecond[2];
      return localDistance;
    }
  }
  return 0;
}

/**
 * Reads distance data from the third TFMini LiDAR sensor 3 and returns the measured distance.
 * @return The measured distance in milimeter, or 0 if the data is invalid or the checksum is incorrect.
 */
unsigned int readTF_Third() {
  int pos = 0;
  int numOfData = 0;
  int localDistance = 0;
  // Continue reading while there's enough data available
  while (tf_third.available() >= MIN_DATA_SIZE) {
    int n = tf_third.available();
    int ret = tf_third.readBytes(&serialBufferThird[pos], n);
    numOfData += ret;
    pos = numOfData;
  }
  vTaskDelay(pdMS_TO_TICKS(1));  // Non-blocking delay

  // Check if the received data has a valid header and checksum
  if (serialBufferThird[0] == HEADER_BYTE && serialBufferThird[1] == HEADER_BYTE) {
    int totalCS = 0;
    for (int i = 0; i < 8; i++) {
      totalCS += serialBufferThird[i];
    }
    byte checksum = (totalCS)&0xFF;
    if (checksum == serialBufferThird[8]) {
      localDistance = (uint16_t)serialBufferThird[3] << 8 | serialBufferThird[2];
      return localDistance;
    }
  }
  return 0;
}