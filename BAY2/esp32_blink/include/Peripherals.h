#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include <Arduino.h>
#include <DHT.h>

extern DHT dht;

void printval(void);
void plug_status(void);

float recentAvgCurrent(void);

#endif