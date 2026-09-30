#include <Arduino.h>
#include "model.h"

// ============================================================
// ARRIVAL PROBABILITY
// ============================================================

float predictArrivalProbability(
    float temperature,
    float current,
    float voltage
)
{
    float probability = 0.20f;

    if (temperature >= 30.0f)
    {
        probability += 0.10f;
    }

    if (current > 10.0f)
    {
        probability += 0.15f;
    }

    if (voltage > 150.0f)
    {
        probability += 0.10f;
    }

    if (probability > 1.0f)
    {
        probability = 1.0f;
    }

    return probability;
}


// ============================================================
// CHARGING DURATION
// ============================================================

float predictChargingDuration(
    float current,
    float voltage
)
{
    float powerValue = voltage * current;

    // No charging power
    if (powerValue <= 1.0f)
    {
        return 20.0f;
    }

    /*
     * Simulation model:
     *
     * Higher charging power -> shorter predicted time
     *
     * 8000 W -> approximately 5 min
     * 6000 W -> approximately 8 min
     * 5000 W -> approximately 10 min
     * 3000 W -> approximately 17 min
     */

    float duration =
        60000.0f / powerValue;

    if (duration < 5.0f)
    {
        duration = 5.0f;
    }

    if (duration > 60.0f)
    {
        duration = 60.0f;
    }

    return duration;
}