// #include <cstdio.h>
// shift register pins
#define PLOAD_PIN 6
#define CLOCK_EN_PIN 4
#define DATA_PIN 3
#define CLOCK_PIN 5

#define PULSE_WIDTH 5
#define NUM_BYTES 8

#define UPDATE_RATE_IN_MS 50

#define DEBUG_SERIAL true

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

void fixHardwareMistakes()
{

  uint8_t temp = currentBoardState[0];
  currentBoardState[0] = (currentBoardState[0] & ~0x10) | (currentBoardState[1] & 0x10);
  currentBoardState[1] = (currentBoardState[1] & ~0x10) | (temp & 0x10);

  temp = currentBoardState[4];
  currentBoardState[4] = (currentBoardState[4] & ~0xF0) | (currentBoardState[5] & 0xF0);
  currentBoardState[5] = (currentBoardState[5] & ~0xF0) | (temp & 0xF0);
}

void setup_shift_registers()
{
  pinMode(PLOAD_PIN, OUTPUT);
  pinMode(CLOCK_EN_PIN, OUTPUT);
  pinMode(DATA_PIN, INPUT);
  pinMode(CLOCK_PIN, OUTPUT);
}
void read_full_sensor_array()
{

  // read new board state into shift registers
  digitalWrite(CLOCK_EN_PIN, HIGH);
  digitalWrite(PLOAD_PIN, LOW);
  delayMicroseconds(PULSE_WIDTH);
  digitalWrite(PLOAD_PIN, HIGH);
  digitalWrite(CLOCK_EN_PIN, LOW);

  uint8_t value = 0;
  uint8_t currentReadRow = 0;

  for (int i = 0; i < NUM_BYTES; i++)
  {
    currentReadRow = 0;
    lastBoardState[i] = currentBoardState[i];
    for (int j = 0; j < 8; j++)
    {
      value = !digitalRead(DATA_PIN);
      currentReadRow |= (value << ((7) - j));
      // shift to next bit
      digitalWrite(CLOCK_PIN, HIGH);
      delayMicroseconds(PULSE_WIDTH);
      digitalWrite(CLOCK_PIN, LOW);
    }
    currentBoardState[i] = currentReadRow;
  }
  fixHardwareMistakes(); // fix mistakes in soldering.
}

void send_values()
{
  for (int i = 0; i < NUM_BYTES; i++)
  {
#if DEBUG_SERIAL == true
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
}
// same as read, but it returns whether something has changed or not
boolean is_board_changed()
{
  return memcmp(currentBoardState, lastBoardState, 8) != 0;
}

void setup()
{
  Serial.begin(115200);
  setup_shift_registers();
}

void loop()
{

  if (millis() - lastCheck > UPDATE_RATE_IN_MS)
  {
    read_full_sensor_array();
    if (is_board_changed()) send_values();
    lastCheck = millis();
  }
}
