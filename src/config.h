#ifndef CONFIG_H
#define CONFIG_H


extern const char *MQTT_DEVICE_NAME;

extern const char *STATE_TOPIC;
extern const char *DURATION_TOPIC;

extern const char* ssid;
extern const char* password;
extern const char* mqtt_server;
extern const int mqtt_port;
extern const char* mqtt_user;
extern const char* mqtt_pass;

extern const int TRANSMITTER_PIN;
extern const int IR_PIN;
extern const String rc_devices[][5];
extern const int numDevices;


#endif
