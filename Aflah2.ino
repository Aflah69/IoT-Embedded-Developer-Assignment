#include <WiFi.h>
#include <HTTPClient.h>
#include <SD.h>

const char* ssid = "wifi";//ssid
const char* password = "12345678";//password
const char* apiURL = "https://fastag-internal.parkzap.com/account/mockable_test/";
const char* dataFile = "/data.txt";

File file;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  //Initialize SD card
  if (!SD.begin()) {
    Serial.println("SD card initialization failed!");
  return;
  }

  // Download data from API and store in SD card
  
}

void loop() {
  if (!downloadData()) {
    Serial.println("Failed to download data from API. Using stored data...");
  }

  // Open data file
 file = SD.open(dataFile);
  if (!file) {
  Serial.println("Failed to open data file!");
  return;
  }
  // Read user input from Serial
  if (Serial.available()) {
    String userInput = Serial.readStringUntil('\n');
    userInput.trim();

    // Search for the key in the data file
    String value = findValue(userInput);
    if (value != "") {
      Serial.println("Value: " + value);
    } else {
      Serial.println("Key not found!");
    }
  }
}

bool downloadData() {
  bool success = false;

  HTTPClient http;
  http.begin(apiURL);

  int httpResponseCode = http.GET();
  if (httpResponseCode == HTTP_CODE_OK) {
    String payload = http.getString();

    // Open data file in write mode
    file = SD.open(dataFile, FILE_WRITE);
    if (file) {
      file.println(payload);
      file.close();
      success = true;
      Serial.println("Data downloaded and stored successfully.");
    } else {
      Serial.println("Failed to open data file!");
    }
  } else {
    Serial.print("Failed to download data. Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
  return success;
}

String findValue(const String& key) {
  String value = "";
  unsigned long startTime = millis();

  file.seek(0);
  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();

    int separatorIndex = line.indexOf(':');
    if (separatorIndex != -1) {
      String currentKey = line.substring(0, separatorIndex);
      currentKey.trim();

      if (compareStrings(currentKey, key)) {
        value = line.substring(separatorIndex + 1);
        value.trim();
        break;
      }
    }
  }

  unsigned long queryTime = millis() - startTime;
  Serial.println("Time for query: " + String(queryTime) + "ms");
  return value;
}

bool compareStrings(const String& str1, const String& str2) {
  if (str1.length() != str2.length()) {
    return false;
  }

  for (size_t i = 0; i < str1.length(); i++) {
    if (str1[i] != str2[i]) {
      return false;
    }
  }

  return true;
}