#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>

const int RECV_PIN = 11;
const int BTN_PIN = D8;
int buttonState = 0;
boolean isOn = false;
boolean isBtnOn = false;

// IR
IRsend irsend(4); // An IR LED is controlled by GPIO pin 4 (D2)

// wifi connection variables
const char* ssid = "polyeffect";
const char* password = "12420165hh";
boolean wifiConnected = false;

// UDP variables
unsigned int localPort = 8888;
WiFiUDP UDP;
boolean udpConnected = false;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged"; // a string to send back
String command = "";

void setup() {
  irsend.begin();
  Serial.begin(9600);
  while (!Serial) {

  }

  wifiConnected = connectWifi();

  // only proceed if wifi connection successful
  if (wifiConnected) {
    udpConnected = connectUDP();
    if (udpConnected) {
      // initialise pins
      pinMode(BTN_PIN, INPUT);
      pinMode(BUILTIN_LED, OUTPUT);
    }
  }
}

void loop() {
  buttonState = digitalRead(BTN_PIN);

  if (buttonState == HIGH && !isBtnOn) {
    isBtnOn = true;
    Serial.println("HIGH");
    digitalWrite(BUILTIN_LED, LOW);
    isIROn();
  } else if (buttonState == LOW && isBtnOn) {
    isBtnOn = false;
    Serial.println("LOW");
    digitalWrite(BUILTIN_LED, HIGH);

  }

  if (wifiConnected) {
    if (udpConnected) {

      // if there’s data available, read a packet
      int packetSize = UDP.parsePacket();
      if (packetSize)
      {
        Serial.println("");
        Serial.print("Received packet of size ");
        Serial.println(packetSize);
        Serial.print("From ");
        IPAddress remote = UDP.remoteIP();
        for (int i = 0; i < 4; i++) {
          Serial.print(remote[i], DEC);
          if (i < 3) {
            Serial.print(".");
          }
        }
        Serial.print(", port ");
        Serial.println(UDP.remotePort());

        // read the packet into packetBufffer
        UDP.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
        Serial.print("Message1: ");
        int WhichLEDPin = packetBuffer[0];
        Serial.println(WhichLEDPin);
        Serial.print("Message2: ");
        int packet2 = packetBuffer[1];
        Serial.println(packet2);
        Serial.print("UDP from Android: ");
        Serial.println(packetBuffer);

        if (packetBuffer[1] == 110) {
          Serial.println("on");
          isIROn();
        } else if (packetBuffer[1] == 102) {
          Serial.println("off");
          isIROn();
        }

        char packetBuffer[UDP_TX_PACKET_MAX_SIZE] = "";

        // send a reply, to the IP address and port that sent us the packet we received
        /*
          UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
          UDP.print(WiFi.localIP());
          UDP.endPacket();
        */

        delay(10);
      }
    }

    delay(10);
  }
}

void isIROn() {
  if (!isOn) {
    isOn = true;
    for (int i = 0; i < random(3, 10) ; i++) {
      irsend.sendLG(0x8800347, 28);
    }
  } else {
    isOn = false;
    for (int i = 0; i < random(3, 10) ; i++) {
      irsend.sendLG(0x88C0051, 28);
    }
  }
}



// connect to UDP – returns true if successful or false if not
boolean connectUDP() {
  boolean state = false;

  Serial.println("");
  Serial.println("Connecting to UDP");

  if (UDP.begin(localPort) == 1) {
    Serial.println("Connection successful");
    state = true;
  }
  else {
    Serial.println("Connection failed");
  }

  return state;
}
// connect to wifi – returns true if successful or false if not
boolean connectWifi() {
  boolean state = true;
  int i = 0;
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10) {
      state = false;
      break;
    }
    i++;
  }
  if (state) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  return state;
}
