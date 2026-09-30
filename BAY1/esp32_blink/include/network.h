#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

// ================================================================
// MQTT CLIENT OBJECTS
// ================================================================

extern WiFiClient espClient;
extern PubSubClient mqtt;


// ================================================================
// WIFI FUNCTIONS
// ================================================================

void connectWiFi();


// ================================================================
// MQTT FUNCTIONS
// ================================================================

void connectMQTT();


// ================================================================
// MQTT CALLBACK
// Receives RPC commands from ThingsBoard
// ================================================================

void mqttCallback(
    char* topic,
    byte* payload,
    unsigned int length
);

#endif