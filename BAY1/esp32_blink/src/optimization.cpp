#include <Arduino.h>

#include "optimization.h"
#include "config.h"
#include "State.h"


// ---------------------------------------------------------------------
// Bay 1 charging optimization
// ---------------------------------------------------------------------

void runOptimization(void)
{
    // ---------------------------------------------------------------
    // Optimization is applicable ONLY when bay is charging
    // ---------------------------------------------------------------

    if (bayStatus != "CHARGING")
    {
        overloadActive = false;

        throttleLevel = 0;

        loadDecision = "IDLE";

        return;
    }


    // ---------------------------------------------------------------
    // Safety overload detection
    // ---------------------------------------------------------------

    bool voltageOverload =
        voltage > MAX_VOLTAGE;

    bool currentOverload =
        current > MAX_CURRENT;

    bool powerOverload =
        power > MAX_STATION_POWER;


    // ---------------------------------------------------------------
    // Overload has highest priority
    // ---------------------------------------------------------------

    if (
        voltageOverload ||
        currentOverload ||
        powerOverload
    )
    {
        overloadActive = true;

        throttleLevel = OVERLOAD_THROTTLE_LEVEL;

        loadDecision = "OVERLOAD_THROTTLE";


        Serial.println();
        Serial.println("!!!!!!!! OVERLOAD !!!!!!!!");

        if (voltageOverload)
        {
            Serial.println("Reason : HIGH VOLTAGE");
        }

        if (currentOverload)
        {
            Serial.println("Reason : OVERCURRENT");
        }

        if (powerOverload)
        {
            Serial.println("Reason : HIGH STATION POWER");
        }

        Serial.println("Throttle : 50%");
        Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!");

        return;
    }


    // ---------------------------------------------------------------
    // No safety overload
    // ---------------------------------------------------------------

    overloadActive = false;


    // ---------------------------------------------------------------
    // Charging-time optimization
    // ---------------------------------------------------------------

    if (
        predictedDurationMin <=
        CHARGING_TIME_LIMIT_MIN
    )
    {
        // Session can finish in <= 10 minutes.
        // Do NOT throttle.

        throttleLevel = 100;

        loadDecision = "ALLOW";

        Serial.println(
            "[OPT] Charging time <= 10 min -> FULL POWER"
        );
    }
    else
    {
        // Session takes > 10 minutes.
        // Apply normal throttling.

        throttleLevel = NORMAL_THROTTLE_LEVEL;

        loadDecision = "THROTTLE";

        Serial.println(
            "[OPT] Charging time > 10 min -> THROTTLE @ 70%"
        );
    }
}


// ---------------------------------------------------------------------
// Relay duty cycle
// ---------------------------------------------------------------------

void applyRelayDutyCycle(void)
{
    if (bayStatus != "CHARGING")
    {
        digitalWrite(RELAY_PIN, LOW);

        return;
    }


    // ---------------------------------------------------------------
    // Full power
    // ---------------------------------------------------------------

    if (throttleLevel >= 100)
    {
        digitalWrite(RELAY_PIN, HIGH);

        return;
    }


    // ---------------------------------------------------------------
    // No power
    // ---------------------------------------------------------------

    if (throttleLevel <= 0)
    {
        digitalWrite(RELAY_PIN, LOW);

        return;
    }


    // ---------------------------------------------------------------
    // Duty cycle
    // ---------------------------------------------------------------

    unsigned long phase =
        millis() % DUTY_CYCLE_WINDOW_MS;

    unsigned long onTime =
        (DUTY_CYCLE_WINDOW_MS * throttleLevel) / 100;

    if (phase < onTime)
    {
        digitalWrite(RELAY_PIN, HIGH);
    }
    else
    {
        digitalWrite(RELAY_PIN, LOW);
    }
}


// ---------------------------------------------------------------------
// LED status
// ---------------------------------------------------------------------

void updateLeds(void)
{
    // ---------------------------------------------------------------
    // FREE
    // Green ON
    // ---------------------------------------------------------------

    if (bayStatus == "FREE")
    {
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_YELLOW, LOW);
        digitalWrite(LED_RED, LOW);

        return;
    }


    // ---------------------------------------------------------------
    // CHARGING + OVERLOAD
    // Red ON
    // ---------------------------------------------------------------

    if (overloadActive)
    {
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_YELLOW, LOW);
        digitalWrite(LED_RED, HIGH);

        return;
    }


    // ---------------------------------------------------------------
    // CHARGING + NORMAL
    // Yellow ON
    // ---------------------------------------------------------------

    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED, LOW);
}