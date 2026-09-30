#ifndef STATE_H
#define STATE_H

#include <Arduino.h>

// ---------------------------------------------------------------------
// Live bay state
// ---------------------------------------------------------------------

extern String bayStatus;

extern float voltage;
extern float current;
extern float power;
extern float energyWh;
extern float temperature;


// ---------------------------------------------------------------------
// Charging session
// ---------------------------------------------------------------------

extern unsigned long sessionStartMs;
extern unsigned long lastEnergySampleMs;


// ---------------------------------------------------------------------
// Edge AI
// ---------------------------------------------------------------------

extern float predictedArrivalProb;
extern float predictedDurationMin;

extern int lastHourOfDay;


// ---------------------------------------------------------------------
// Optimization
// ---------------------------------------------------------------------

extern bool overloadActive;

extern float predictionThreshold;
extern float overloadCurrentA;
extern float maxStationLoadW;

extern int throttleLevel;

extern String loadDecision;


// ---------------------------------------------------------------------
// Tariff / demand information
// ---------------------------------------------------------------------

extern int peakTariffStartHr;
extern int peakTariffEndHr;

#endif