#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <RCSwitch.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "config.h"
#include "connection.h"
#include "tv_remote_database.h"
//#define LED_BUILTIN 15 // Change this to the pin number of your board's

// Function prototypes
void sendRCCode(const String device[], bool on);
void handleRCCommand(const String device[], byte* payload, unsigned int length);
void handleAndSendIRCommand(byte* payload, unsigned int length);
void callback(char* topic, byte* payload, unsigned int length);
void subscribeToAllDevices();
void flashforever();
void logMessage(String message);

RCSwitch mySwitch = RCSwitch(); // Create an instance of the RCSwitch library
IRsend irsend(IR_PIN);
unsigned long lastPublish = 0;
float time_old;
float time_new;
int log_counter = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting up...");
  pinMode(LED_BUILTIN, OUTPUT);
  mySwitch.enableTransmit(TRANSMITTER_PIN);
  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
  subscribeToAllDevices();
  Serial.println("Setup finished");
}



void loop() {
  if (!client.connected()) {
    reconnect();
    subscribeToAllDevices();
  }
  client.loop();
  if (millis() - lastPublish > 5000) {
    client.publish(STATE_TOPIC, "online");
    lastPublish = millis();
  }
}


void flashforever() {
  while (true) {
    flashLed(1, 100, 100);
  }
}



void subscribeToAllDevices() {
  // Subscribe to all device topics
  for (int i = 0; i < numDevices; i++) {
    char topic[30];
    snprintf(topic, 30, "Sender/RC/%s", rc_devices[i][0].c_str());
    client.subscribe(topic);
    Serial.print("Subscribed to ");
    Serial.println(topic);
  }

  // Subscribe to the IR topic
  client.subscribe("Sender/IR");
  Serial.println("Subscribed to Sender/IR");
}



void callback(char* topic, byte* payload, unsigned int length) {
  // Find the device based on the topic for RC commands
  Serial.println("CALLBACK");
  digitalWrite(LED_BUILTIN, LOW);
  time_old = millis()/1000.0;

  
  int deviceIndex = -1;
  for (int i = 0; i < numDevices; i++) {
    std::string deviceTopic = std::string("Sender/RC/") + rc_devices[i][0].c_str();
    if (String(topic) == deviceTopic.c_str()) {
      deviceIndex = i;
      break;
    }
  }
  if (deviceIndex != -1) {
    handleRCCommand(rc_devices[deviceIndex], payload, length);
  } else if (String(topic) == "Sender/IR") {
    handleAndSendIRCommand(payload, length);
  } else {
    Serial.println("Received message for unknown device or topic");
  }
  logMessage("callback end");
  digitalWrite(LED_BUILTIN, HIGH);
}


void handleAndSendIRCommand(byte* payload, unsigned int length) {
  // Parse the message
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // Check if the message exists in the IR_CODES_MAP
  if (IR_CODES_MAP.find(message.c_str()) != IR_CODES_MAP.end()) {
    unsigned long irCode = IR_CODES_MAP.at(message.c_str());
    
    // Send the IR code
    IRsend irsend(IR_PIN);
    irsend.begin();

    irsend.sendSAMSUNG(irCode, 32, 1);
    Serial.print("Sent IR code: ");
    Serial.println(message); // Print the name of the IR code instead of the hex value
  } else {
    Serial.print("Received unknown IR command: ");
    Serial.println(message);
  }
}


void handleRCCommand(const String device[], byte* payload, unsigned int length) {
  logMessage("handleRCCommand");
  // Parse the message
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // Send the command
  if (message == "on" || message == "ON") {
    sendRCCode(device, true);
  } else if (message == "off" || message == "OFF") {
    sendRCCode(device, false);
  } else {
    Serial.println("Received unknown message for RC command");
  }
  logMessage("handleRCCommand end");
}

void sendRCCode(const String device[], bool on) {
  logMessage("sendRCCode");
  int protocol = device[4].toInt();
  int pulseLength = device[3].toInt();

  mySwitch.setProtocol(protocol);
  mySwitch.setPulseLength(pulseLength);

  int commandCode = on ? device[1].toInt() : device[2].toInt();
  logMessage("sendRCCode before send");
  mySwitch.send(commandCode, 24);
  logMessage("sendRCCode after send");

  Serial.print("Sent ");
  Serial.print(on ? "on" : "off");
  Serial.print(" command for ");
  Serial.println(device[0]);
  logMessage("sendRCCode end");
}

void logMessage(String message = ""){
  if (message != ""){
    Serial.println(message);
  }
  time_new = millis()/1000.0;
  Serial.println(log_counter);
  Serial.println(time_new-time_old);
  
  log_counter++;
}
