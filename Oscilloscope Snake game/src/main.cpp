#include <Arduino.h>
#include <SPI.h>

// MCP4822 configuration
const int CS_PIN = 10;    // Chip select pin for MCP4822

// MCP4822 DAC control bits
#define MCP4822_CHANNEL_A 0x3000
#define MCP4822_CHANNEL_B 0xB000

void setup() {
  // SPI settings
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2); // Set SPI speed
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);  // Ensure CS pin is high
}

void loop() {
  // Example: Draw a circle
  drawCircle(2048, 2048, 2048);  // Center X=2048, Y=2048, Radius=2048
  drawLine(2048, 2048);
}

// Function to send data to the MCP4822 DAC
void writeDAC(uint16_t channel, uint16_t value) {
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
    writeDAC(MCP4822_CHANNEL_A, x);
    writeDAC(MCP4822_CHANNEL_B, y);
    delay(1);  // Adjust for speed; lower delay for smoother shapes
  }
}

void drawLine(int start, int end)
{
  for(int i = start; i < 2048; i++)
  {
    writeDAC(MCP4822_CHANNEL_A, i);
    delay(0.25);
  }


}