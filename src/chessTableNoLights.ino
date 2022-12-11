// #include <cstdio.h>
#include <matrixDriver.h>
#include <bluetoothSetup.h>
// shift register pins
#define PLOAD_PIN 26
#define CLOCK_EN_PIN 33
#define DATA_PIN 32 
#define CLOCK_PIN 25

#define PULSE_WIDTH 5
#define NUM_BYTES 8

#define UPDATE_RATE_IN_MS 50

#define DEBUG_SERIAL true 
#define BLUETOOTH_ENABLED true
uint8_t lastBoardState[NUM_BYTES] = {0};
uint8_t currentBoardState[NUM_BYTES] = {0};
uint32_t lastCheck = 0;
#if DEBUG_SERIAL == true
// used for debugging
void printBits(byte myByte)
{
  for (byte mask = 0x80; mask; mask >>= 1)
  {
    if (mask & myByte)
      Serial.print("1 ");
    else
      Serial.print("0 ");
  }
}
#endif

void send_values()
{
#if BLUETOOTH_ENABLED 
send_bluetooth_packet(currentBoardState);
#else 
  for (int i = 0; i < NUM_BYTES; i++)
  {
#if DEBUG_SERIAL
    printBits(currentBoardState[i]);
    Serial.println();
#else
    Serial.print(currentBoardState[i], BIN); // get state of button i
    Serial.println();
#endif
    Serial.flush();
  }
#endif
  Serial.println("--------------------------");
  Serial.flush();
}
// same as read, but it returns whether something has changed or not
boolean is_board_changed()
{
  return memcmp(currentBoardState, lastBoardState, 8) != 0;
}

void setup()
{
  Serial.begin(115200);
  configure_bluetooth();
  setup_sensor_matrix();
}

void loop()
{

  if (millis() - lastCheck > UPDATE_RATE_IN_MS)
  {
    read_full_sensor_matrix(currentBoardState , lastBoardState);
    if (is_board_changed()) send_values();
    lastCheck = millis();
  }
}
