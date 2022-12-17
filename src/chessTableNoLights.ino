// #include <cstdio.h>
#include <matrixDriver.h>
#include <bluetoothSetup.h>
// shift register pins

#define UPDATE_RATE_IN_MS 20 

#define DEBUG_SERIAL true  
#define BLUETOOTH_ENABLED true 
uint8_t lastBoardState[MATRIX_ARRAY_WIDTH] = {0};
uint8_t currentBoardState[MATRIX_ARRAY_WIDTH] = {0};
uint32_t lastCheck = 0;
#if DEBUG_SERIAL == true
// used for debugging
void print_bits(byte myByte)
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
  Serial.println("--------------------------");
  Serial.flush();
#endif
}

void setup()
{
#if BLUETOOTH_ENABLED 
  configure_bluetooth();
#else
  Serial.begin(115200);
#endif
  setup_sensor_matrix();
}

void loop()
{
  if (  millis() - lastCheck > UPDATE_RATE_IN_MS )
  {
    read_full_sensor_matrix(currentBoardState , lastBoardState);
    // check if board state has changed, compare 8 bytes of continuous memory
    if (memcmp(currentBoardState, lastBoardState, MATRIX_ARRAY_WIDTH) != 0 ) 
    {
       send_values();
    }
    lastCheck = millis();
  }
}
