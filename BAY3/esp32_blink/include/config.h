#ifndef CONFIG_H
#define CONFIG_H

// ---------------------------------------------------------------------
// WiFi configuration
// ---------------------------------------------------------------------

extern const char* WIFI_SSID;
extern const char* WIFI_PASS;


// ---------------------------------------------------------------------
// ThingsBoard MQTT configuration
// ---------------------------------------------------------------------

extern const char* TB_SERVER;
extern const int TB_PORT;
extern const char* TB_TOKEN;


// ---------------------------------------------------------------------
// Bay identification
// ---------------------------------------------------------------------

extern const char* BAY_ID;


// ---------------------------------------------------------------------
// Pin configuration — SRS Section 8.1
// ---------------------------------------------------------------------

#define VOLTAGE_PIN   34
#define CURRENT_PIN   35

#define DHT_PIN       15

#define RELAY_PIN     26

#define BTN_PLUGIN    32
#define BTN_PLUGOUT   33

#define LED_GREEN     18
#define LED_YELLOW    19
#define LED_RED       21

#define DHT_TYPE DHT22


// ---------------------------------------------------------------------
// ADC configuration
// ---------------------------------------------------------------------

#define ADC_MIN 0
#define ADC_MAX 4095

#define VOLTAGE_MIN 0.0f
#define VOLTAGE_MAX 250.0f

#define CURRENT_MIN 0.0f
#define CURRENT_MAX 32.0f


// ---------------------------------------------------------------------
// Charging safety limits
// ---------------------------------------------------------------------

#define MAX_VOLTAGE        240.0f
#define MAX_CURRENT        30.0f
#define MAX_STATION_POWER  7000.0f


// ---------------------------------------------------------------------
// Charging optimization
// ---------------------------------------------------------------------

#define CHARGING_TIME_LIMIT_MIN 10.0f

#define NORMAL_THROTTLE_LEVEL 70

#define OVERLOAD_THROTTLE_LEVEL 50


// ---------------------------------------------------------------------
// MQTT topics
// ---------------------------------------------------------------------

#define MQTT_TELEMETRY_TOPIC \
    "v1/devices/me/telemetry"

#define MQTT_RPC_TOPIC \
    "v1/devices/me/rpc/request/+"


// ---------------------------------------------------------------------
// Timing
// ---------------------------------------------------------------------

#define TELEMETRY_INTERVAL_MS 5000UL

#define SENSOR_INTERVAL_MS 2000UL

#define DUTY_CYCLE_WINDOW_MS 1000UL

#define BUTTON_DEBOUNCE_MS 250UL

#endif