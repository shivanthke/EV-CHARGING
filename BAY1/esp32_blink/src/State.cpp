#include "State.h"
#include "config.h"

// ---------------------------------------------------------------------
// Live bay state
// ---------------------------------------------------------------------

String bayStatus = "FREE";

float voltage = 0.0f;
float current = 0.0f;
float power = 0.0f;
float energyWh = 0.0f;
float temperature = 0.0f;


// ---------------------------------------------------------------------
// Charging session
// ---------------------------------------------------------------------

unsigned long sessionStartMs = 0;
unsigned long lastEnergySampleMs = 0;


// ---------------------------------------------------------------------
// Edge AI
// ---------------------------------------------------------------------

float predictedArrivalProb = 0.0f;
float predictedDurationMin = 0.0f;

int lastHourOfDay = 12;


// ---------------------------------------------------------------------
// Optimization
// ---------------------------------------------------------------------

bool overloadActive = false;

float predictionThreshold = 0.50f;

float overloadCurrentA = MAX_CURRENT;
float maxStationLoadW = MAX_STATION_POWER;

bool manualOverrideActive = false;

int throttleLevel = 0;

String loadDecision = "IDLE";


// ---------------------------------------------------------------------
// Tariff / demand
// ---------------------------------------------------------------------

int peakTariffStartHr = 18;
int peakTariffEndHr = 22;