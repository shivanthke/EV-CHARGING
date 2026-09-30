#include <Arduino.h>
#include <time.h>

#include "edge_ai.h"
#include "State.h"
#include "Peripherals.h"
#include "model.h"


// =====================================================================
// HISTORICAL ARRIVAL RATE
// =====================================================================

float historicalArrivalRate(int hour)
{
    // Morning peak
    if (hour >= 8 && hour <= 10)
    {
        return 0.62f;
    }

    // Evening peak
    if (hour >= 18 && hour <= 21)
    {
        return 0.58f;
    }

    // Very low demand
    if (hour >= 0 && hour <= 5)
    {
        return 0.06f;
    }

    // Normal daytime demand
    if (hour >= 11 && hour <= 17)
    {
        return 0.30f;
    }

    // Default demand
    return 0.18f;
}


// =====================================================================
// EDGE AI INFERENCE
// =====================================================================

void runEdgeAIInference(void)
{
    // -----------------------------------------------------------------
    // Get current time
    // -----------------------------------------------------------------

    struct tm timeinfo;

    int hourOfDay = 12;
    int dayOfWeek = 0;


    if (getLocalTime(&timeinfo, 100))
    {
        hourOfDay = timeinfo.tm_hour;
        dayOfWeek = timeinfo.tm_wday;
    }


    // Store current hour
    lastHourOfDay = hourOfDay;


    // -----------------------------------------------------------------
    // Bay occupied status
    // -----------------------------------------------------------------

    float bayOccupiedF = 0.0f;

    if (bayStatus == "CHARGING")
    {
        bayOccupiedF = 1.0f;
    }
    else
    {
        bayOccupiedF = 0.0f;
    }


    // -----------------------------------------------------------------
    // Charging session elapsed time
    // -----------------------------------------------------------------

    float sessionElapsedMin = 0.0f;

    if (
        bayStatus == "CHARGING" &&
        sessionStartMs > 0
    )
    {
        sessionElapsedMin =
            (millis() - sessionStartMs)
            / 60000.0f;
    }


    // -----------------------------------------------------------------
    // Historical arrival rate
    // -----------------------------------------------------------------

    float histRate =
        historicalArrivalRate(hourOfDay);


    // -----------------------------------------------------------------
    // Recent average current
    // -----------------------------------------------------------------

    float avgCurrent =
        recentAvgCurrent();


    // =================================================================
    // ARRIVAL PROBABILITY PREDICTION
    // =================================================================

    predictedArrivalProb =
        predictArrival(
            (float)hourOfDay,
            (float)dayOfWeek,
            bayOccupiedF,
            avgCurrent,
            sessionElapsedMin,
            histRate
        );


    // =================================================================
    // CHARGING DURATION PREDICTION
    // =================================================================

    // Charging duration is meaningful only when Bay 1 is charging.

    if (bayStatus == "CHARGING")
    {
        predictedDurationMin =
            predictDuration(
                (float)hourOfDay,
                (float)dayOfWeek,
                bayOccupiedF,
                avgCurrent,
                sessionElapsedMin,
                histRate
            );
    }
    else
    {
        predictedDurationMin = 0.0f;
    }


    // =================================================================
    // SERIAL MONITOR OUTPUT
    // =================================================================

    Serial.println();
    Serial.println("==========================================");
    Serial.println("             BAY 2 EDGE AI");
    Serial.println("==========================================");

    Serial.print("Hour of Day          : ");
    Serial.println(hourOfDay);

    Serial.print("Day of Week          : ");
    Serial.println(dayOfWeek);

    Serial.print("Bay Occupied         : ");

    if (bayOccupiedF == 1.0f)
    {
        Serial.println("YES");
    }
    else
    {
        Serial.println("NO");
    }

    Serial.print("Recent Avg Current   : ");
    Serial.print(avgCurrent, 2);
    Serial.println(" A");

    Serial.print("Historical Rate      : ");
    Serial.print(histRate * 100.0f, 1);
    Serial.println(" %");


    Serial.print("Arrival Probability  : ");
    Serial.print(
        predictedArrivalProb * 100.0f,
        1
    );
    Serial.println(" %");


    Serial.print("Predicted Duration   : ");
    Serial.print(
        predictedDurationMin,
        1
    );
    Serial.println(" min");


    Serial.print("Session Time         : ");
    Serial.print(
        sessionElapsedMin,
        1
    );
    Serial.println(" min");


    Serial.println("==========================================");
}