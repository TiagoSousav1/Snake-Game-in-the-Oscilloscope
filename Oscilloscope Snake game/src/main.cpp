#include <Arduino.h>
#include <SPI.h>

// MCP4822 configuration
const int CS_PIN = 10;    // Chip select pin for MCP4822

const int centerX = 2048;    // Center X position (mid-range of DAC)
const int centerY = 2048;    // Center Y position (mid-range of DAC)
const int sideLength = 2048; // Side length of the square

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

static int directionState = UP;

// Function to send data to the MCP4822 DAC
void writeDAC(uint16_t channel, uint16_t value)
{
  digitalWrite(CS_PIN, LOW);
  SPI.transfer16(channel | (value & 0x0FFF));  // Send channel + value
  digitalWrite(CS_PIN, HIGH);
}


// Function to draw a circle by calculating X and Y points
void drawCircle(int centerX, int centerY, int radius) {
  for (int angle = 0; angle < 360; angle += 5) {
    float radians = angle * 3.14159 / 180.0;
    int x = centerX + radius * cos(radians);
    int y = centerY + radius * sin(radians);
    writeDAC(MCP4822_CHANNEL_1, x);
    writeDAC(MCP4822_CHANNEL_2, y);
    delay(10);  // Adjust for speed; lower delay for smoother shapes
  }
}

void drawVerticalLine(int x, int y_start, int y_end)
{
  writeDAC(MCP4822_CHANNEL_2, x);
  for(int i = y_start; i < y_end; i++)
  {

    writeDAC(MCP4822_CHANNEL_1, i);
    //delay(0.1);
  }

}

void drawHorizontalLine(int y, int x_start, int x_end)
{
  writeDAC(MCP4822_CHANNEL_1, y);
  for(int i = x_start; i < x_end; i++)
  {
    
    writeDAC(MCP4822_CHANNEL_2, i);
    //delay(0.1);
  }

}

// Function to draw a square
void drawSquare() {
  // Define square corners
  int x1 = centerX - sideLength / 2;
  int y1 = centerY - sideLength / 2;
  
  int x2 = centerX + sideLength / 2;
  int y2 = y1;
  
  int x3 = x2;
  int y3 = centerY + sideLength / 2;
  
  int x4 = x1;
  int y4 = y3;

  // Draw the square by moving between corners
  // Start at (x1, y1) -> (x2, y2) -> (x3, y3) -> (x4, y4) -> back to (x1, y1)
  
  // Move from (x1, y1) to (x2, y2)
  writeDAC(MCP4822_CHANNEL_1, x1);
  writeDAC(MCP4822_CHANNEL_2, y1);
  delay(10);
  
  writeDAC(MCP4822_CHANNEL_1, x2);
  writeDAC(MCP4822_CHANNEL_2, y2);
  delay(10);

  // Move from (x2, y2) to (x3, y3)
  writeDAC(MCP4822_CHANNEL_1, x2);
  writeDAC(MCP4822_CHANNEL_2, y2);
  delay(10);
  
  writeDAC(MCP4822_CHANNEL_1, x3);
  writeDAC(MCP4822_CHANNEL_2, y3);
  delay(10);

  // Move from (x3, y3) to (x4, y4)
  writeDAC(MCP4822_CHANNEL_1, x3);
  writeDAC(MCP4822_CHANNEL_2, y3);
  delay(10);
  
  writeDAC(MCP4822_CHANNEL_1, x4);
  writeDAC(MCP4822_CHANNEL_2, y4);
  delay(10);

  // Move from (x4, y4) back to (x1, y1)
  writeDAC(MCP4822_CHANNEL_1, x4);
  writeDAC(MCP4822_CHANNEL_2, y4);
  delay(10);
  
  writeDAC(MCP4822_CHANNEL_1, x1);
  writeDAC(MCP4822_CHANNEL_2, y1);
  delay(10);
}

void updateDirectionStateRight()
{
  directionState = RIGHT;
  Serial.print(directionState);
}

void updateDirectionStateLeft()
{
  directionState = LEFT;
  Serial.print(directionState);
}

void updateDirectionStateUp()
{
  directionState = UP;
  Serial.print(directionState);
}

void updateDirectionStateDown()
{
  directionState = DOWN;
  Serial.print(directionState);
}

void setup() {
  // DEBUGGING
  Serial.begin(9600);


  // SPI settings
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2); // Set SPI speed
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
}

void loop() {
  // Example: Draw a circle
  //drawCircle(2048, 2048, 2048);  // Center X=2048, Y=2048, Radius=2048
  drawVerticalLine(0, 0, 2048); // Left side
  drawHorizontalLine(2048, 0, 4096); // Top side
  drawVerticalLine(4096, 0, 2048);  // Right side
  drawHorizontalLine(0, 0, 4096); // Bot side
  //drawLine(1024, 4096);
  //drawSquare();
}



