#include <Arduino.h>
#include <ArduinoJson.h>

#include "telemetry.h"
#include "network.h"
#include "State.h"
#include "config.h"


// =====================================================================
// BAY 1 TELEMETRY
// =====================================================================

void publishTelemetry(void)
{
    // -----------------------------------------------------------------
    // Check MQTT connection
    // -----------------------------------------------------------------

    if (!mqtt.connected())
    {
        Serial.println("[MQTT] Not connected");
        return;
    }


    // -----------------------------------------------------------------
    // Create JSON document
    // ArduinoJson 7
    // -----------------------------------------------------------------

    JsonDocument doc;


    // -----------------------------------------------------------------
    // Bay information
    // -----------------------------------------------------------------

    doc["bayId"] = BAY_ID;

    doc["bayStatus"] = bayStatus;


    // -----------------------------------------------------------------
    // Sensor values
    // -----------------------------------------------------------------

    doc["voltage"] =
        roundf(voltage * 10.0f) / 10.0f;

    doc["current"] =
        roundf(current * 10.0f) / 10.0f;

    doc["power"] =
        roundf(power * 10.0f) / 10.0f;

    doc["temperature"] =
        roundf(temperature * 10.0f) / 10.0f;


    // -----------------------------------------------------------------
    // Energy
    // -----------------------------------------------------------------

    doc["energyWh"] =
        roundf(energyWh * 100.0f) / 100.0f;


    // -----------------------------------------------------------------
    // Edge AI
    // -----------------------------------------------------------------

    doc["predictedArrivalProb"] =
        roundf(predictedArrivalProb * 100.0f) / 100.0f;

    doc["predictedDurationMin"] =
        roundf(predictedDurationMin * 10.0f) / 10.0f;


    // -----------------------------------------------------------------
    // Optimization
    // -----------------------------------------------------------------

    doc["overloadActive"] =
        overloadActive;

    doc["throttleLevel"] =
        throttleLevel;

    doc["loadDecision"] =
        loadDecision;


    // -----------------------------------------------------------------
    // Charging session time
    // -----------------------------------------------------------------

    float sessionMinutes = 0.0f;


    if (
        bayStatus == "CHARGING" &&
        sessionStartMs > 0
    )
    {
        sessionMinutes =
            (millis() - sessionStartMs)
            / 60000.0f;
    }


    doc["sessionMinutes"] =
        roundf(sessionMinutes * 10.0f) / 10.0f;


    // -----------------------------------------------------------------
    // Convert JSON to text
    // -----------------------------------------------------------------

    char buffer[1024];

    size_t length =
        serializeJson(
            doc,
            buffer,
            sizeof(buffer)
        );


    if (length == 0)
    {
        Serial.println(
            "[MQTT] JSON serialization failed"
        );

        return;
    }


    // -----------------------------------------------------------------
    // Display telemetry
    // -----------------------------------------------------------------

    Serial.print("[MQTT >>] ");

    Serial.println(buffer);


    // -----------------------------------------------------------------
    // Publish to ThingsBoard
    // -----------------------------------------------------------------

    bool success =
        mqtt.publish(
            MQTT_TELEMETRY_TOPIC,
            buffer
        );


    if (success)
    {
        Serial.println(
            "[MQTT] Telemetry published successfully."
        );
    }
    else
    {
        Serial.println(
            "[MQTT] Telemetry publish FAILED."
        );
    }
}