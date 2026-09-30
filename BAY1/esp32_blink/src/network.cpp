#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "network.h"
#include "config.h"
#include "rpc.h"

// ================================================================
// MQTT CLIENT
// ================================================================

WiFiClient espClient;
PubSubClient mqtt(espClient);


// ================================================================
// MQTT CALLBACK
// Receives RPC commands from ThingsBoard
// ================================================================

void mqttCallback(
    char* topic,
    byte* payload,
    unsigned int length
)
{
    Serial.println();
    Serial.println("==========================================");
    Serial.println("[MQTT] MESSAGE RECEIVED");
    Serial.println("==========================================");

    Serial.print("Topic   : ");
    Serial.println(topic);

    Serial.print("Length  : ");
    Serial.println(length);

    // ------------------------------------------------------------
    // Copy payload into a null-terminated character array
    // ------------------------------------------------------------

    char message[1024];

    if (length >= sizeof(message))
    {
        Serial.println("[MQTT] ERROR: Message too large.");
        Serial.println("==========================================");
        return;
    }

    memcpy(
        message,
        payload,
        length
    );

    message[length] = '\0';

    Serial.print("Payload : ");
    Serial.println(message);

    // ------------------------------------------------------------
    // Convert topic to String
    // ------------------------------------------------------------

    String topicString = String(topic);

    // ------------------------------------------------------------
    // Check whether this is an RPC request
    // ------------------------------------------------------------

    const String rpcPrefix =
        "v1/devices/me/rpc/request/";

    if (topicString.startsWith(rpcPrefix))
    {
        // --------------------------------------------------------
        // Extract RPC request ID
        // Example:
        //
        // v1/devices/me/rpc/request/123
        //
        // requestId = 123
        // --------------------------------------------------------

        String requestId =
            topicString.substring(
                rpcPrefix.length()
            );

        Serial.println();
        Serial.println("------------------------------------------");
        Serial.println("           RPC REQUEST");
        Serial.println("------------------------------------------");

        Serial.print("Request ID : ");
        Serial.println(requestId);

        // --------------------------------------------------------
        // Pass RPC request to rpc.cpp
        // --------------------------------------------------------

        handleRpc(
            requestId,
            message
        );

        Serial.println("------------------------------------------");
        Serial.println("RPC request handled.");
        Serial.println("------------------------------------------");
    }
    else
    {
        Serial.println();
        Serial.println("[MQTT] Message is not an RPC request.");
    }

    Serial.println("==========================================");
}


// ================================================================
// WIFI CONNECTION
// ================================================================

void connectWiFi()
{
    Serial.println();
    Serial.println("==========================================");
    Serial.println("             WIFI CONNECTION");
    Serial.println("==========================================");

    WiFi.mode(WIFI_STA);

    WiFi.begin(
        WIFI_SSID,
        WIFI_PASS
    );

    unsigned long startTime = millis();

    while (
        WiFi.status() != WL_CONNECTED &&
        millis() - startTime < 15000UL
    )
    {
        delay(500);

        Serial.print(".");
    }

    Serial.println();

    // ------------------------------------------------------------
    // Wi-Fi connected
    // ------------------------------------------------------------

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("WiFi connected!");

        Serial.print("SSID      : ");
        Serial.println(WIFI_SSID);

        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());

        Serial.print("RSSI      : ");
        Serial.print(WiFi.RSSI());

        Serial.println(" dBm");
    }
    else
    {
        Serial.println("WiFi connection FAILED.");
    }

    Serial.println("==========================================");
}


// ================================================================
// THINGSBOARD MQTT CONNECTION
// ================================================================

void connectMQTT()
{
    // ------------------------------------------------------------
    // Check Wi-Fi first
    // ------------------------------------------------------------

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(
            "[MQTT] Cannot connect because WiFi is not connected."
        );

        return;
    }

    // ------------------------------------------------------------
    // Configure MQTT server
    // ------------------------------------------------------------

    mqtt.setServer(
        TB_SERVER,
        TB_PORT
    );

    // ------------------------------------------------------------
    // Configure MQTT callback
    // ------------------------------------------------------------

    mqtt.setCallback(
        mqttCallback
    );

    // ------------------------------------------------------------
    // MQTT buffer
    // ------------------------------------------------------------

    mqtt.setBufferSize(1024);

    Serial.println();
    Serial.println("==========================================");
    Serial.println("       THINGSBOARD MQTT CONNECTION");
    Serial.println("==========================================");

    Serial.print("Server      : ");
    Serial.println(TB_SERVER);

    Serial.print("Port        : ");
    Serial.println(TB_PORT);

    Serial.print("Bay ID      : ");
    Serial.println(BAY_ID);

    Serial.print("Token length: ");
    Serial.println(strlen(TB_TOKEN));

    Serial.println();
    Serial.println("Connecting to ThingsBoard...");

    // ------------------------------------------------------------
    // Connect using:
    //
    // Client ID = BAY_ID
    // Username  = TB_TOKEN
    // Password  = ""
    // ------------------------------------------------------------

    bool connected =
        mqtt.connect(
            BAY_ID,
            TB_TOKEN,
            ""
        );

    // ------------------------------------------------------------
    // Connection successful
    // ------------------------------------------------------------

    if (connected)
    {
        Serial.println();
        Serial.println("**************************************");
        Serial.println(" ThingsBoard MQTT CONNECTED!");
        Serial.println("**************************************");

        // --------------------------------------------------------
        // Subscribe to ThingsBoard RPC
        // --------------------------------------------------------

        bool subscribed =
            mqtt.subscribe(
                MQTT_RPC_TOPIC
            );

        if (subscribed)
        {
            Serial.println(
                "[MQTT] RPC subscription successful."
            );

            Serial.print(
                "[MQTT] Subscribed topic: "
            );

            Serial.println(
                MQTT_RPC_TOPIC
            );
        }
        else
        {
            Serial.println(
                "[MQTT] RPC subscription FAILED."
            );
        }

        Serial.println();
    }

    // ------------------------------------------------------------
    // Connection failed
    // ------------------------------------------------------------

    else
    {
        Serial.println();
        Serial.println(
            "ThingsBoard MQTT connection FAILED."
        );

        Serial.print(
            "MQTT state = "
        );

        Serial.println(
            mqtt.state()
        );

        // --------------------------------------------------------
        // Explain MQTT error
        // --------------------------------------------------------

        switch (mqtt.state())
        {
            case -4:

                Serial.println(
                    "Connection timeout"
                );

                break;

            case -3:

                Serial.println(
                    "Connection lost"
                );

                break;

            case -2:

                Serial.println(
                    "Connect failed"
                );

                break;

            case -1:

                Serial.println(
                    "Disconnected"
                );

                break;

            case 1:

                Serial.println(
                    "Bad MQTT protocol"
                );

                break;

            case 2:

                Serial.println(
                    "Bad client ID"
                );

                break;

            case 3:

                Serial.println(
                    "Server unavailable"
                );

                break;

            case 4:

                Serial.println(
                    "Bad username/password"
                );

                break;

            case 5:

                Serial.println(
                    "NOT AUTHORIZED - check device access token"
                );

                break;

            default:

                Serial.println(
                    "Unknown MQTT error"
                );

                break;
        }
    }

    Serial.println(
        "=========================================="
    );
}