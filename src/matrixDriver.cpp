#include <matrixDriver.h>
void fixHardwareMistakes(uint8_t currentBoardState[])
{

  uint8_t temp = currentBoardState[0];
  currentBoardState[0] = (currentBoardState[0] & ~0x10) | (currentBoardState[1] & 0x10);
  currentBoardState[1] = (currentBoardState[1] & ~0x10) | (temp & 0x10);

  temp = currentBoardState[4];
  currentBoardState[4] = (currentBoardState[4] & ~0xF0) | (currentBoardState[5] & 0xF0);
  currentBoardState[5] = (currentBoardState[5] & ~0xF0) | (temp & 0xF0);
}

void setup_sensor_matrix()
{
  pinMode(PLOAD_PIN, OUTPUT);
  pinMode(CLOCK_EN_PIN, OUTPUT);
  pinMode(DATA_PIN, INPUT);
  pinMode(CLOCK_PIN, OUTPUT);
}
void read_full_sensor_matrix(uint8_t currentBoardState[], uint8_t lastBoardState[])
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
  fixHardwareMistakes(currentBoardState); // fix mistakes in soldering.
}