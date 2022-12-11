#include <Arduino.h>

#define PLOAD_PIN 26
#define CLOCK_EN_PIN 33
#define DATA_PIN 32 
#define CLOCK_PIN 25

#define PULSE_WIDTH 5
#define NUM_BYTES 8

void read_full_sensor_matrix(uint8_t currentBoardState[], uint8_t lastBoardState[]);
void fixHardwareMistakes(uint8_t currentBoardState[]);
void setup_sensor_matrix();