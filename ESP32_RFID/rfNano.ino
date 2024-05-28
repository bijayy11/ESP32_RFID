#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SS_PIN 5
#define RST_PIN 22

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

const char* ssid = ""; // Replace with your network SSID
const char* password = ""; // Replace with your network password
const char* server_address = ""; //flask server port

void setup() {
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();          // Initiate SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Place your card near the reader...");
  Serial.println();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  // Show UID on serial monitor
  Serial.print("UID tag: ");
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message: ");
  content.toUpperCase();
  Serial.println(content);

  // Send the UID to the server
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String serverPath = String(server_address) + "/card_detected";

    http.begin(serverPath);
    http.addHeader("Content-Type", "application/json");

    String httpRequestData = "{\"uid\":\"" + content + "\"}";
    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  delay(3000);
}
