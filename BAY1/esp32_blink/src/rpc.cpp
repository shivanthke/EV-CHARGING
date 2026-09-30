#include <Arduino.h>
#include <ArduinoJson.h>

#include "rpc.h"
#include "network.h"
#include "State.h"
#include "config.h"

// ================================================================
// RPC COMMAND HANDLER
// ThingsBoard -> ESP32
// ================================================================

void handleRpc(String requestId, char* payload)
{
    // ------------------------------------------------------------
    // ArduinoJson 7
    // ------------------------------------------------------------

    JsonDocument doc;

    DeserializationError error =
        deserializeJson(doc, payload);

    if (error)
    {
        Serial.println();
        Serial.println("[RPC] JSON parsing failed.");

        Serial.print("[RPC] Error: ");
        Serial.println(error.c_str());

        return;
    }

    // ------------------------------------------------------------
    // Read RPC method
    // ------------------------------------------------------------

    String method =
        doc["method"] | "";

    JsonObject params =
        doc["params"].as<JsonObject>();

    // ------------------------------------------------------------
    // Response document
    // ------------------------------------------------------------

    JsonDocument response;

    // ============================================================
    // SET RELAY STATE
    // ============================================================

    if (method == "setRelayState")
    {
        bool state = false;

        if (!params.isNull())
        {
            state =
                params["state"] | false;
        }

        // --------------------------------------------------------
        // Enable manual cloud control
        // --------------------------------------------------------

        manualOverrideActive = true;

        // --------------------------------------------------------
        // CLOUD ON
        // --------------------------------------------------------

        if (state == true)
        {
            throttleLevel = 100;
            loadDecision = "MANUAL_ON";

            digitalWrite(
                RELAY_PIN,
                HIGH
            );

            Serial.println();
            Serial.println("==========================================");
            Serial.println("       CLOUD RPC: BAY MANUALLY ON");
            Serial.println("==========================================");

            Serial.println("Relay       : ON");
            Serial.println("Throttle    : 100%");
            Serial.println("Override    : ACTIVE");

            Serial.println("==========================================");
        }

        // --------------------------------------------------------
        // CLOUD OFF
        // --------------------------------------------------------

        else
        {
            throttleLevel = 0;
            loadDecision = "MANUAL_OFF";

            digitalWrite(
                RELAY_PIN,
                LOW
            );

            Serial.println();
            Serial.println("==========================================");
            Serial.println("       CLOUD RPC: BAY MANUALLY OFF");
            Serial.println("==========================================");

            Serial.println("Relay       : OFF");
            Serial.println("Throttle    : 0%");
            Serial.println("Override    : ACTIVE");

            Serial.println("==========================================");
        }

        // --------------------------------------------------------
        // RPC response
        // --------------------------------------------------------

        response["success"] = true;
        response["relayState"] = state;
        response["throttleLevel"] = throttleLevel;
        response["manualOverride"] = true;
    }

    // ============================================================
    // SET THROTTLE
    // ============================================================

    else if (method == "setThrottle")
    {
        int level = 100;

        if (!params.isNull())
        {
            level =
                params["level"] | 100;
        }

        // Limit 0-100
        level =
            constrain(
                level,
                0,
                100
            );

        manualOverrideActive = true;

        throttleLevel = level;

        loadDecision =
            "MANUAL_THROTTLE";

        // --------------------------------------------------------
        // 0% = relay OFF
        // --------------------------------------------------------

        if (level == 0)
        {
            digitalWrite(
                RELAY_PIN,
                LOW
            );

            Serial.println();
            Serial.println("[RPC] Manual throttle = 0%");
            Serial.println("[RPC] Relay = OFF");
        }

        // --------------------------------------------------------
        // 100% = relay ON
        // --------------------------------------------------------

        else if (level == 100)
        {
            digitalWrite(
                RELAY_PIN,
                HIGH
            );

            Serial.println();
            Serial.println("[RPC] Manual throttle = 100%");
            Serial.println("[RPC] Relay = ON");
        }

        // --------------------------------------------------------
        // Intermediate throttle
        // --------------------------------------------------------

        else
        {
            Serial.println();
            Serial.print("[RPC] Manual throttle = ");
            Serial.print(level);
            Serial.println("%");

            Serial.println(
                "[RPC] Intermediate level controlled by duty cycle."
            );
        }

        // --------------------------------------------------------
        // RPC response
        // --------------------------------------------------------

        response["success"] = true;
        response["throttleLevel"] = level;
        response["manualOverride"] = true;
    }

    // ============================================================
    // UNKNOWN METHOD
    // ============================================================

    else
    {
        response["success"] = false;
        response["error"] = "unknown method";

        Serial.println();
        Serial.println("[RPC] Unknown RPC method:");

        Serial.println(method);
    }

    // ============================================================
    // SEND RESPONSE TO THINGSBOARD
    // ============================================================

    char buffer[512];

    size_t responseLength =
        serializeJson(
            response,
            buffer,
            sizeof(buffer)
        );

    if (responseLength == 0)
    {
        Serial.println(
            "[RPC] Failed to serialize response."
        );

        return;
    }

    // ------------------------------------------------------------
    // ThingsBoard RPC response topic
    // ------------------------------------------------------------

    String responseTopic =
        "v1/devices/me/rpc/response/" +
        requestId;

    bool published =
        mqtt.publish(
            responseTopic.c_str(),
            buffer
        );

    Serial.println();
    Serial.println("------------------------------------------");
    Serial.println("           RPC RESPONSE");
    Serial.println("------------------------------------------");

    Serial.print("Topic   : ");
    Serial.println(responseTopic);

    Serial.print("Response: ");
    Serial.println(buffer);

    if (published)
    {
        Serial.println(
            "Status  : Response published successfully."
        );
    }
    else
    {
        Serial.println(
            "Status  : Response publish FAILED."
        );
    }

    Serial.println("------------------------------------------");
}