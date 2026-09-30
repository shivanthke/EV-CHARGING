#ifndef NETWORK_H
#define NETWORK_H

#include <WiFi.h>
#include <PubSubClient.h>

extern WiFiClient espClient;

extern PubSubClient mqtt;

void connectWiFi(void);

void connectMQTT(void);

void mqttCallback(
    char* topic,
    byte* payload,
    unsigned int length
);

#endif