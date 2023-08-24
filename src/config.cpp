#include <Arduino.h>
#include "config.h"

//! IMPORTANT! RENAME!
const char *MQTT_DEVICE_NAME = "Sender";

const char *STATE_TOPIC = "Sender/state";


const int TRANSMITTER_PIN = 12;
const int IR_PIN = 11;

// RC device settings
const String rc_devices[][5] = {
  {"A", "1361", "1364", "300", "1"},
  {"B", "4433", "4436", "300", "1"},
  {"C", "5201", "5204", "300", "1"},
  {"D", "5393", "5396", "300", "1"},
  {"E", "4195665", "4195668", "300", "1"},
  {"PC", "13637", "13637", "300", "1"}
};

const int numDevices = sizeof(rc_devices) / sizeof(rc_devices[0]);