#include <Arduino.h>
#include <ESP8266WiFi.h>

extern "C" {
  #include <espnow.h>
  #include <user_interface.h>
}

// Custom MAC address for this node (optional)
uint8_t mac[] = {0x36, 0x33, 0x33, 0x33, 0x33, 0x33};

// LED pin (D6 corresponds to GPIO12 on NodeMCU)
int Led1 = 12;

//===========================================================================================
void initVariant() {
  WiFi.mode(WIFI_AP);
  wifi_set_macaddr(SOFTAP_IF, mac);
}

//===========================================================================================
#define WIFI_CHANNEL 4

// Data structure (must match sender)
struct __attribute__((packed)) DataStruct {
  char text[32];
  unsigned int time;
};
DataStruct myData;

// Callback function prototype
void receiveCallBackFunction(uint8_t *senderMac, uint8_t *incomingData, uint8_t len);

//===========================================================================================
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting ESP-NOW Receiver (Slave)");
  Serial.print("AP MAC: ");
  Serial.println(WiFi.softAPmacAddress());
  Serial.print("STA MAC: ");
  Serial.println(WiFi.macAddress());

  pinMode(Led1, OUTPUT);
  digitalWrite(Led1, LOW);

  // Initialize ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("*** ESP-NOW init failed!");
    while (true) delay(100);
  }

  // Set device role
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);

  // Register receive callback
  esp_now_register_recv_cb(receiveCallBackFunction);

  Serial.println("Setup complete - waiting for messages...");
}

//===========================================================================================
void loop() {
  // Nothing here — everything handled in callback
}

//===========================================================================================
void receiveCallBackFunction(uint8_t *senderMac, uint8_t *incomingData, uint8_t len) {
  if (len == sizeof(myData)) {
    memcpy(&myData, incomingData, sizeof(myData));

    Serial.print("Received message: ");
    Serial.println(myData.text);
    Serial.print("Time value: ");
    Serial.println(myData.time);

    // Toggle LED on message
    digitalWrite(Led1, !digitalRead(Led1));
  } else {
    Serial.println("Received data size mismatch!");
  }
}
