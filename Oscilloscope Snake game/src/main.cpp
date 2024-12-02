#include <Arduino.h>
#include <SPI.h>

// MCP4822 configuration
const int CS_PIN = 10;    // Chip select pin for MCP4822

const int sideLength = 256;  // Side length of the square
const int centerX = sideLength / 2;    // Center X position (mid-range of DAC)
const int centerY = sideLength / 2;    // Center Y position (mid-range of DAC)

const int stepSize = 16;

uint16_t snake[256][256];

SPISettings spiSets (20000000, MSBFIRST, SPI_MODE0);

// MCP4822 DAC control bits
#define MCP4822_CHANNEL_1 0x3000
#define MCP4822_CHANNEL_2 0xB000
#define RIGHT_BUTTON_PIN 7
#define LEFT_BUTTON_PIN 6
#define UP_BUTTON_PIN 5
#define DOWN_BUTTON_PIN 4

#define RIGHT 0
#define LEFT 1
#define UP 2
#define DOWN 3

int directionState = UP;



// Function to send data to the MCP4822 DAC
void writeDAC(uint16_t channel, uint16_t value) {
  SPI.beginTransaction(spiSets);
  digitalWrite(CS_PIN, LOW);
  SPI.transfer16(channel | (value & 0x0FFF));  // Send channel + value
  digitalWrite(CS_PIN, HIGH);
  SPI.endTransaction();
}

void drawPoint(int x, int y)
{
  writeDAC(MCP4822_CHANNEL_1, x);
  writeDAC(MCP4822_CHANNEL_2, y);
}

void drawSnake(uint16_t **snake, int snakeLength)
{
  for (int i = 0; i < snakeLength; i++)
  {
    drawPoint(snake[i][0], snake[i][1]);
  }
}

void drawVerticalLine(int x, int y_start, int y_end)
{
  writeDAC(MCP4822_CHANNEL_2, x);
  for(int i = y_start; i < y_end; i++)
  {
    writeDAC(MCP4822_CHANNEL_1, i);
    delay(0.1);
  }
}

void drawHorizontalLine(int y, int x_start, int x_end)
{
  writeDAC(MCP4822_CHANNEL_1, y);
  for(int i = x_start; i < x_end; i++)
  {
    writeDAC(MCP4822_CHANNEL_2, i);
    delay(0.1);
  }
}

// Function to draw the boundary square
void drawSquare() {
  drawVerticalLine(0, 0, 256); // Left side
  drawHorizontalLine(256, 0, 256); // Top side
  drawVerticalLine(256, 0, 256);  // Right side
  drawHorizontalLine(0, 0, 256); // Bot side
}

/* void moveSnake ()
{
  if (directionState == UP)
  {
    snakeEndY += stepSize;
    snakeStartY += stepSize;

    // Collison with square's top boundary
    if (snakeEndY >= sideLength)
    {
      snakeEndY = 0;
    }

    if (snakeStartY >= sideLength)
    {
      snakeStartY = 0;
    }

    // Redraw snake in new position
    drawVerticalLine(snakeStartX, snakeStartY, snakeEndY);
  }

  if (directionState == DOWN)
  {
    snakeEndY -= stepSize;
    snakeStartY -= stepSize;

    // Collison with square's bottom boundary
    if (snakeEndY <= 0)
    {
      snakeEndY = sideLength;
    }

    if (snakeStartY <= 0)
    {
      snakeStartY = sideLength;
    }

    // Redraw snake in new position
    drawVerticalLine(snakeStartX, snakeStartY, snakeEndY);
  }

  if (directionState == RIGHT)   // change snake coordinates - this if isn't finalized
  {
    snakeEndY -= stepSize;
    snakeStartY -= stepSize;

    // Collison with square's bottom boundary
    if (snakeEndY <= 0)
    {
      snakeEndY = sideLength;
    }

    if (snakeStartY <= 0)
    {
      snakeStartY = sideLength;
    }

    // Redraw snake in new position
    drawHorizontalLine(snakeStartX, snakeStartY, snakeEndY);
  }

  if (directionState == LEFT)   // change snake coordinates - this if isn't finalized
  {
    snakeEndY -= stepSize;
    snakeStartY -= stepSize;

    // Collison with square's bottom boundary
    if (snakeEndY <= 0)
    {
      snakeEndY = sideLength;
    }

    if (snakeStartY <= 0)
    {
      snakeStartY = sideLength;
    }

    // Redraw snake in new position
    drawHorizontalLine(snakeStartX, snakeStartY, snakeEndY);
  }

  delay(10);  // better visibility... for now, might delete later
}  */


void setup() {
  // DEBUGGING
  Serial.begin(9600);

  // SPI settings
  SPI.begin();
  //SPI.setClockDivider(SPI_CLOCK_DIV2); // Set SPI speed

  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);  // Ensure CS pin is high


  // BUTTONS
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RIGHT_BUTTON_PIN), updateDirectionStateRight, FALLING);

  pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LEFT_BUTTON_PIN), updateDirectionStateLeft, FALLING);

  pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(UP_BUTTON_PIN), updateDirectionStateUp, FALLING);

  pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(DOWN_BUTTON_PIN), updateDirectionStateDown, FALLING); 

  drawSnake();
}

void loop() {
  drawSquare();
  //drawVerticalLine(snakeStartX, snakeStartY, snakeEndY);
  //drawHorizontalLine(2048, 1900, 2048);

  //moveSnake();
}

