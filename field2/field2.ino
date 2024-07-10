#include <SPI.h>
#include <MFRC522.h>
#include <WiFiS3.h>
#include <WiFiClient.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

// Replace with your network credentials
const char* ssid     ="Dp";
const char* password = "999999999";

// ThingSpeak Settings
char server[] = "api.thingspeak.com";
String writeAPIKey = "NAVQ37CF7NWE9AFF";

WiFiClient client;

unsigned long lastScanTime = 0;    // When the last NFC scan started
unsigned long scanInterval = 2000; // Scan for new NFC cards every 2 seconds (2000 milliseconds)

void setup() {
  Serial.begin(9600);        // Initialize serial communications with the PC
  while (!Serial);           // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  
  SPI.begin();               // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  unsigned long currentMillis = millis(); // Get the current time

    // If 2 seconds have passed since the last scan started, start a new scan
  if (currentMillis - lastScanTime >= scanInterval) {
      lastScanTime = currentMillis; // Remember when this scan started
       // Look for new cards
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      byte buffer[18];
      byte size = sizeof(buffer);
      
      String data1 = ""; // Initialize data string
      
      // Read pages from 6 to 11
      for (byte page = 6; page <= 11; page++){
        mfrc522.MIFARE_Read(page, buffer, &size);
        
        for (byte i = 1; i < size; i++) {
          if (buffer[i] >= 32 && buffer[i] <= 126) {
            data1 += String((char)buffer[i]);
          }
        }
      }
      
      if (client.connect(server,80)) { // Send data to ThingSpeak
        client.println("POST /update HTTP/1.1");
        client.println("Host: api.thingspeak.com");
        client.println("Connection: close");
        client.println("User-Agent: Arduino/1.0");
        client.println("X-THINGSPEAKAPIKEY: "+writeAPIKey);
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.print("Content-Length: ");
        client.println(data1.length());
        client.println();
        client.print("&field1=" + data1); // Send data as field1
      
      }
      delay(10000); // Wait for 10 seconds before sending the next request
    }
  }
}
 void loop() {
  unsigned long currentMillis = millis(); // Get the current time

    // If 2 seconds have passed since the last scan started, start a new scan
  if (currentMillis - lastScanTime >= scanInterval) {
      lastScanTime = currentMillis; // Remember when this scan started
       // Look for new cards
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      byte buffer[18];
      byte size = sizeof(buffer);
      
      String data2 = ""; // Initialize data string
      
      // Read pages from 6 to 11
      for (byte page = 6; page <= 11; page++){
        mfrc522.MIFARE_Read(page, buffer, &size);
        
        for (byte i = 1; i < size; i++) {
          if (buffer[i] >= 32 && buffer[i] <= 126) {
            data1 += String((char)buffer[i]);
          }
        }
      }
      
      if (client.connect(server,80)) { // Send data to ThingSpeak
        client.println("POST /update HTTP/1.1");
        client.println("Host: api.thingspeak.com");
        client.println("Connection: close");
        client.println("User-Agent: Arduino/1.0");
        client.println("X-THINGSPEAKAPIKEY: "+writeAPIKey);
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.print("Content-Length: ");
        client.println(data2.length());
        client.println();
        client.print("&field1=" + data2); // Send data as field1
      
      }
      delay(10000); // Wait for 10 seconds before sending the next request
    }
  }
}
