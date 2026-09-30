#include <Arduino.h>
#include <DHT.h>

#include "State.h"
#include "Peripherals.h"
#include "config.h"


// =====================================================================
// DHT SENSOR
// =====================================================================

DHT dht(
    DHT_PIN,
    DHT_TYPE
);


// =====================================================================
// INTERNAL VARIABLES
// =====================================================================

static float lastValidTemperature = 25.0f;

static float currentSamples[5] =
{
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f
};

static int currentSampleIndex = 0;

static unsigned long lastSensorReadMs = 0;


// ---------------------------------------------------------------------
// Button states
// ---------------------------------------------------------------------

static bool previousPluginState = HIGH;

static bool previousPlugoutState = HIGH;


static unsigned long lastPluginPressMs = 0;

static unsigned long lastPlugoutPressMs = 0;


// =====================================================================
// ADC MAPPING FUNCTION
// =====================================================================

static float mapFloat(
    long x,
    long inMin,
    long inMax,
    float outMin,
    float outMax
)
{
    return
        (x - inMin) *
        (outMax - outMin) /
        (float)(inMax - inMin)
        + outMin;
}


// =====================================================================
// SENSOR READING
// =====================================================================

void printval(void)
{
    unsigned long now = millis();


    // -----------------------------------------------------------------
    // Sensor reading interval
    // -----------------------------------------------------------------

    if (
        now - lastSensorReadMs <
        SENSOR_INTERVAL_MS
    )
    {
        return;
    }


    lastSensorReadMs = now;


    // =================================================================
    // BAY FREE / CHARGING OFF
    // =================================================================

    if (bayStatus != "CHARGING")
    {
        // -------------------------------------------------------------
        // When charging is OFF:
        // Voltage = 0
        // Current = 0
        // Power   = 0
        // -------------------------------------------------------------

        voltage = 0.0f;

        current = 0.0f;

        power = 0.0f;


        // -------------------------------------------------------------
        // Store zero current
        // -------------------------------------------------------------

        currentSamples[currentSampleIndex] = 0.0f;

        currentSampleIndex++;

        if (currentSampleIndex >= 5)
        {
            currentSampleIndex = 0;
        }


        // -------------------------------------------------------------
        // Serial output
        // -------------------------------------------------------------

        Serial.println();

        Serial.println(
            "========== BAY 3 SENSORS =========="
        );


        Serial.print(
            "Voltage       : "
        );

        Serial.print(
            voltage,
            1
        );

        Serial.println(
            " V"
        );


        Serial.print(
            "Current       : "
        );

        Serial.print(
            current,
            1
        );

        Serial.println(
            " A"
        );


        Serial.print(
            "Power         : "
        );

        Serial.print(
            power,
            1
        );

        Serial.println(
            " W"
        );


        Serial.print(
            "Temperature   : "
        );

        Serial.print(
            temperature,
            1
        );

        Serial.println(
            " C"
        );


        Serial.print(
            "Bay Status    : "
        );

        Serial.println(
            bayStatus
        );


        Serial.println(
            "===================================="
        );


        // -------------------------------------------------------------
        // No charging -> exit sensor processing
        // -------------------------------------------------------------

        return;
    }


    // =================================================================
    // BAY IS CHARGING
    // =================================================================

    // -----------------------------------------------------------------
    // Read voltage ADC
    // -----------------------------------------------------------------

    int voltageADC =
        analogRead(
            VOLTAGE_PIN
        );


    // -----------------------------------------------------------------
    // Read current ADC
    // -----------------------------------------------------------------

    int currentADC =
        analogRead(
            CURRENT_PIN
        );


    // =================================================================
    // CONVERT ADC TO REAL VALUES
    // =================================================================

    voltage =
        mapFloat(
            voltageADC,
            ADC_MIN,
            ADC_MAX,
            VOLTAGE_MIN,
            VOLTAGE_MAX
        );


    current =
        mapFloat(
            currentADC,
            ADC_MIN,
            ADC_MAX,
            CURRENT_MIN,
            CURRENT_MAX
        );


    // =================================================================
    // POWER CALCULATION
    // =================================================================

    power =
        voltage *
        current;


    // =================================================================
    // TEMPERATURE
    // =================================================================

    float temp =
        dht.readTemperature();


    if (!isnan(temp))
    {
        temperature =
            temp;

        lastValidTemperature =
            temp;
    }
    else
    {
        temperature =
            lastValidTemperature;
    }


    // =================================================================
    // STORE CURRENT SAMPLE
    // =================================================================

    currentSamples[currentSampleIndex] =
        current;


    currentSampleIndex++;


    if (currentSampleIndex >= 5)
    {
        currentSampleIndex = 0;
    }


    // =================================================================
    // ENERGY CALCULATION
    // =================================================================

    if (
        bayStatus == "CHARGING"
    )
    {
        if (
            lastEnergySampleMs == 0
        )
        {
            lastEnergySampleMs =
                now;
        }


        float elapsedHours =
            (
                now -
                lastEnergySampleMs
            )
            / 3600000.0f;


        energyWh +=
            power *
            elapsedHours;


        lastEnergySampleMs =
            now;
    }


    // =================================================================
    // SERIAL OUTPUT
    // =================================================================

    Serial.println();

    Serial.println(
        "========== BAY 3 SENSORS =========="
    );


    Serial.print(
        "Voltage       : "
    );

    Serial.print(
        voltage,
        1
    );

    Serial.println(
        " V"
    );


    Serial.print(
        "Current       : "
    );

    Serial.print(
        current,
        1
    );

    Serial.println(
        " A"
    );


    Serial.print(
        "Power         : "
    );

    Serial.print(
        power,
        1
    );

    Serial.println(
        " W"
    );


    Serial.print(
        "Temperature   : "
    );

    Serial.print(
        temperature,
        1
    );

    Serial.println(
        " C"
    );


    Serial.print(
        "Bay Status    : "
    );

    Serial.println(
        bayStatus
    );


    Serial.println(
        "===================================="
    );
}


// =====================================================================
// AVERAGE CURRENT
// =====================================================================

float recentAvgCurrent(void)
{
    float sum =
        0.0f;


    for (
        int i = 0;
        i < 5;
        i++
    )
    {
        sum +=
            currentSamples[i];
    }


    return sum / 5.0f;
}


// =====================================================================
// PLUG-IN / PLUG-OUT BUTTONS
// =====================================================================

void plug_status(void)
{
    unsigned long now =
        millis();


    bool pluginState =
        digitalRead(
            BTN_PLUGIN
        );


    bool plugoutState =
        digitalRead(
            BTN_PLUGOUT
        );


    // =================================================================
    // PLUG-IN BUTTON
    // =================================================================

    if (
        previousPluginState == HIGH &&
        pluginState == LOW &&
        (
            now -
            lastPluginPressMs
        ) >= BUTTON_DEBOUNCE_MS
    )
    {
        lastPluginPressMs =
            now;


        if (
            bayStatus == "FREE"
        )
        {
            bayStatus =
                "CHARGING";


            sessionStartMs =
                now;


            energyWh =
                0.0f;


            lastEnergySampleMs =
                now;


            overloadActive =
                false;


            throttleLevel =
                100;


            loadDecision =
                "STARTING";


            Serial.println();

            Serial.println(
                "************************************"
            );

            Serial.println(
                "       VEHICLE PLUGGED IN"
            );

            Serial.println(
                "       BAY STATUS: CHARGING"
            );

            Serial.println(
                "************************************"
            );

            Serial.println();
        }
    }


    // =================================================================
    // PLUG-OUT BUTTON
    // =================================================================

    if (
        previousPlugoutState == HIGH &&
        plugoutState == LOW &&
        (
            now -
            lastPlugoutPressMs
        ) >= BUTTON_DEBOUNCE_MS
    )
    {
        lastPlugoutPressMs =
            now;


        if (
            bayStatus == "CHARGING"
        )
        {
            bayStatus =
                "FREE";


            sessionStartMs =
                0;


            lastEnergySampleMs =
                0;


            overloadActive =
                false;


            throttleLevel =
                0;


            loadDecision =
                "IDLE";


            // ---------------------------------------------------------
            // Turn relay OFF immediately
            // ---------------------------------------------------------

            digitalWrite(
                RELAY_PIN,
                LOW
            );


            // ---------------------------------------------------------
            // Force electrical values to zero immediately
            // ---------------------------------------------------------

            voltage =
                0.0f;

            current =
                0.0f;

            power =
                0.0f;


            Serial.println();

            Serial.println(
                "************************************"
            );

            Serial.println(
                "       VEHICLE PLUGGED OUT"
            );

            Serial.println(
                "       BAY STATUS: FREE"
            );

            Serial.println(
                "       VOLTAGE: 0.0 V"
            );

            Serial.println(
                "       CURRENT: 0.0 A"
            );

            Serial.println(
                "       POWER: 0.0 W"
            );

            Serial.println(
                "************************************"
            );

            Serial.println();
        }
    }


    // =================================================================
    // SAVE BUTTON STATES
    // =================================================================

    previousPluginState =
        pluginState;


    previousPlugoutState =
        plugoutState;
}