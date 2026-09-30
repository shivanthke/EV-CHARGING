#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "network.h"
#include "config.h"


WiFiClient espClient;

PubSubClient mqtt(espClient);


// ---------------------------------------------------------------------
// WiFi
// ---------------------------------------------------------------------

void connectWiFi(void)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return;
    }


    Serial.println();

    Serial.println(
        "Connecting to WiFi..."
    );


    WiFi.mode(WIFI_STA);

    WiFi.begin(
        WIFI_SSID,
        WIFI_PASS
    );


    unsigned long startTime =
        millis();


    while (
        WiFi.status() != WL_CONNECTED &&
        millis() - startTime < 15000UL
    )
    {
        delay(300);

        Serial.print(".");
    }


    Serial.println();


    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println(
            "WiFi connected!"
        );

        Serial.print(
            "IP: "
        );

        Serial.println(
            WiFi.localIP()
        );
    }
    else
    {
        Serial.println(
            "WiFi connection failed."
        );
    }
}


// ---------------------------------------------------------------------
// MQTT callback
// ---------------------------------------------------------------------

void mqttCallback(
    char* topic,
    byte* payload,
    unsigned int length
)
{
    Serial.println();

    Serial.println(
        "========== MQTT MESSAGE =========="
    );

    Serial.print(
        "Topic: "
    );

    Serial.println(topic);

    Serial.print(
        "Payload: "
    );


    for (
        unsigned int i = 0;
        i < length;
        i++
    )
    {
        Serial.print(
            (char)payload[i]
        );
    }


    Serial.println();

    Serial.println(
        "=================================="
    );
}


// ---------------------------------------------------------------------
// ThingsBoard MQTT
// ---------------------------------------------------------------------

void connectMQTT(void)
{
    if (
        WiFi.status() != WL_CONNECTED
    )
    {
        return;
    }


    mqtt.setServer(
        TB_SERVER,
        TB_PORT
    );


    mqtt.setCallback(
        mqttCallback
    );


    // Increase PubSubClient packet size
    mqtt.setBufferSize(1024);


    if (mqtt.connected())
    {
        return;
    }


    Serial.println();

    Serial.println(
        "Connecting to ThingsBoard MQTT..."
    );


    bool connected =
        mqtt.connect(
            BAY_ID,
            TB_TOKEN,
            ""
        );


    if (connected)
    {
        Serial.println(
            "ThingsBoard MQTT connected!"
        );


        if (
            mqtt.subscribe(
                MQTT_RPC_TOPIC
            )
        )
        {
            Serial.println(
                "RPC subscription successful."
            );
        }
        else
        {
            Serial.println(
                "RPC subscription failed."
            );
        }
    }
    else
    {
        Serial.print(
            "MQTT failed, rc="
        );

        Serial.println(
            mqtt.state()
        );
    }
}