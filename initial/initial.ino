#include <SPI.h>
#include <Wire.h>  // Added Wire library
#include <MFRC522.h>
#include <WiFiS3.h>
#include <WiFiClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

// Replace with your network credentials
const char* ssid     ="Dp";
const char* password = "12345678";

// ThingSpeak Settings
char server[] = "api.thingspeak.com";
String writeAPIKey = "NAVQ37CF7NWE9AFF";

WiFiClient client;

unsigned long lastScanTime = 0;    // When the last NFC scan started
unsigned long scanInterval = 2000; // Scan for new NFC cards every 2 seconds (2000 milliseconds)

// OLED display TWI address
#define OLED_ADDR   0x3C
Adafruit_SSD1306 display(-1);

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

  // Initialize the OLED display using Wire library
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
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
      
      String data = ""; // Initialize data string
      
      // Read pages from 6 to 11
      for (byte page = 6; page <= 11; page++) {
        mfrc522.MIFARE_Read(page, buffer, &size);
        
        for (byte i = 1; i < size; i++) {
          if (buffer[i] >= 32 && buffer[i] <= 126) {
            data += String((char)buffer[i]);
          }
        }
      }
      
      // Print NFC card details to the serial monitor
      Serial.print("NFC Card Details: ");
      Serial.println(data);

      if (client.connect(server, 80)) { // Send data to ThingSpeak
        client.println("POST /update HTTP/1.1");
        client.println("Host: api.thingspeak.com");
        client.println("Connection: close");
        client.println("User-Agent: Arduino/1.0");
        client.println("X-THINGSPEAKAPIKEY: " + writeAPIKey);
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.print("Content-Length: ");
        client.println(data.length());
        client.println();
        client.print("&field2=" + data); // Send data as field2

        // Print the ThingSpeak response to the serial monitor
        while (client.available()) {
          char c = client.read();
          Serial.print(c);
        }
        Serial.println();

        client.stop();
      }

      delay(10000); // Wait for 10 seconds before sending the next request
    }
  }
}
