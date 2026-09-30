#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

#include "config.h"
#include "State.h"
#include "Peripherals.h"
#include "network.h"
#include "telemetry.h"
#include "optimization.h"
#include "edge_ai.h"


// ---------------------------------------------------------------------
// Timer
// ---------------------------------------------------------------------

unsigned long lastTelemetryMs = 0;


// ---------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------

void setup(void)
{
    Serial.begin(115200);

    delay(1000);


    Serial.println();

    Serial.println(
        "=========================================="
    );

    Serial.println(
        "       BAY 1 EV CHARGING SYSTEM"
    );

    Serial.println(
        "=========================================="
    );


    // ---------------------------------------------------------------
    // GPIO configuration
    // ---------------------------------------------------------------

    pinMode(
        BTN_PLUGIN,
        INPUT_PULLUP
    );

    pinMode(
        BTN_PLUGOUT,
        INPUT_PULLUP
    );


    pinMode(
        RELAY_PIN,
        OUTPUT
    );


    pinMode(
        LED_GREEN,
        OUTPUT
    );

    pinMode(
        LED_YELLOW,
        OUTPUT
    );

    pinMode(
        LED_RED,
        OUTPUT
    );


    // ---------------------------------------------------------------
    // Initial outputs
    // ---------------------------------------------------------------

    digitalWrite(
        RELAY_PIN,
        LOW
    );

    digitalWrite(
        LED_GREEN,
        HIGH
    );

    digitalWrite(
        LED_YELLOW,
        LOW
    );

    digitalWrite(
        LED_RED,
        LOW
    );


    // ---------------------------------------------------------------
    // ADC
    // ---------------------------------------------------------------

    analogReadResolution(12);


    // ---------------------------------------------------------------
    // DHT
    // ---------------------------------------------------------------

    dht.begin();


    Serial.println(
        "Peripherals initialized."
    );


    // ---------------------------------------------------------------
    // WiFi
    // ---------------------------------------------------------------

    connectWiFi();


    // ---------------------------------------------------------------
    // Time
    // ---------------------------------------------------------------

    configTime(
        19800,
        0,
        "pool.ntp.org",
        "time.nist.gov"
    );


    // ---------------------------------------------------------------
    // MQTT
    // ---------------------------------------------------------------

    mqtt.setServer(
        TB_SERVER,
        TB_PORT
    );

    mqtt.setCallback(
        mqttCallback
    );

    mqtt.setBufferSize(1024);


    connectMQTT();


    // ---------------------------------------------------------------
    // Timers
    // ---------------------------------------------------------------

    lastTelemetryMs = millis();

    lastEnergySampleMs = millis();


    Serial.println();

    Serial.println(
        "Bay 1 system started."
    );

    Serial.println(
        "Bay status: FREE"
    );

    Serial.println(
        "Press GREEN button to plug in."
    );

    Serial.println(
        "Press RED button to plug out."
    );

    Serial.println();
}


// ---------------------------------------------------------------------
// Main Loop
// ---------------------------------------------------------------------

void loop(void)
{
    // ---------------------------------------------------------------
    // WiFi reconnection
    // ---------------------------------------------------------------

    if (
        WiFi.status() != WL_CONNECTED
    )
    {
        connectWiFi();
    }


    // ---------------------------------------------------------------
    // MQTT reconnection
    // ---------------------------------------------------------------

    if (
        WiFi.status() == WL_CONNECTED &&
        !mqtt.connected()
    )
    {
        connectMQTT();
    }


    // ---------------------------------------------------------------
    // MQTT processing
    // ---------------------------------------------------------------

    mqtt.loop();


    // ---------------------------------------------------------------
    // Check buttons
    // ---------------------------------------------------------------

    plug_status();


    // ---------------------------------------------------------------
    // Read sensors
    // ---------------------------------------------------------------

    printval();


    // ---------------------------------------------------------------
    // 5-second processing and telemetry
    // ---------------------------------------------------------------

    unsigned long now = millis();


    if (
        now - lastTelemetryMs >=
        TELEMETRY_INTERVAL_MS
    )
    {
        lastTelemetryMs = now;


        // ===========================================================
        // EDGE AI
        // ===========================================================

        runEdgeAIInference();


        // ===========================================================
        // OPTIMIZATION
        // ===========================================================

        runOptimization();


        // ===========================================================
        // PRINT FINAL DATA TO SERIAL
        // ===========================================================

        Serial.println();

        Serial.println(
            "=========================================="
        );

        Serial.println(
            "        BAY 1 CLOUD TELEMETRY"
        );

        Serial.println(
            "=========================================="
        );

        Serial.print(
            "Bay ID              : "
        );

        Serial.println(
            BAY_ID
        );


        Serial.print(
            "Bay Status          : "
        );

        Serial.println(
            bayStatus
        );


        Serial.print(
            "Voltage             : "
        );

        Serial.print(
            voltage,
            1
        );

        Serial.println(
            " V"
        );


        Serial.print(
            "Current             : "
        );

        Serial.print(
            current,
            1
        );

        Serial.println(
            " A"
        );


        Serial.print(
            "Power               : "
        );

        Serial.print(
            power,
            1
        );

        Serial.println(
            " W"
        );


        Serial.print(
            "Temperature         : "
        );

        Serial.print(
            temperature,
            1
        );

        Serial.println(
            " C"
        );


        Serial.print(
            "Energy              : "
        );

        Serial.print(
            energyWh,
            2
        );

        Serial.println(
            " Wh"
        );


        Serial.print(
            "Arrival Probability : "
        );

        Serial.print(
            predictedArrivalProb * 100.0f,
            1
        );

        Serial.println(
            " %"
        );


        Serial.print(
            "Predicted Duration  : "
        );

        Serial.print(
            predictedDurationMin,
            1
        );

        Serial.println(
            " min"
        );


        Serial.print(
            "Overload Active     : "
        );

        Serial.println(
            overloadActive
                ? "YES"
                : "NO"
        );


        Serial.print(
            "Throttle Level      : "
        );

        Serial.print(
            throttleLevel
        );

        Serial.println(
            " %"
        );


        Serial.print(
            "Load Decision       : "
        );

        Serial.println(
            loadDecision
        );


        float sessionMinutes = 0.0f;

        if (
            bayStatus == "CHARGING" &&
            sessionStartMs > 0
        )
        {
            sessionMinutes =
                (
                    millis() -
                    sessionStartMs
                )
                / 60000.0f;
        }


        Serial.print(
            "Session Time        : "
        );

        Serial.print(
            sessionMinutes,
            1
        );

        Serial.println(
            " min"
        );


        Serial.println(
            "=========================================="
        );


        // ===========================================================
        // PUBLISH EXACT SAME DATA TO THINGSBOARD
        // ===========================================================

        publishTelemetry();
    }


    // ---------------------------------------------------------------
    // Relay control
    // ---------------------------------------------------------------

    applyRelayDutyCycle();


    // ---------------------------------------------------------------
    // LED control
    // ---------------------------------------------------------------

    updateLeds();


    // ---------------------------------------------------------------
    // Small delay
    // ---------------------------------------------------------------

    delay(10);
}