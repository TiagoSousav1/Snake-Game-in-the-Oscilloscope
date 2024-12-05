#include <Arduino.h>
#include <SPI.h>

// MCP4822 DAC control bits
#define MCP4822_CHANNEL_1 0x3000
#define MCP4822_CHANNEL_2 0xB000
#define CS_PIN 10

#define RIGHT_BUTTON_PIN 7
#define LEFT_BUTTON_PIN 6
#define UP_BUTTON_PIN 5
#define DOWN_BUTTON_PIN 4
#define INTERRUPT_PIN 2

#define RIGHT 0
#define LEFT 1
#define UP 2
#define DOWN 3

#define BOARD_SIZE 256

const int stepSize = 16;

uint16_t snake[BOARD_SIZE][2];
int snakeLength = 100;

SPISettings spiSets (20000000, MSBFIRST, SPI_MODE0);

int directionState = UP;

void inputInterrupt()
{
  //Serial.println("I WAS INTERRUPTED");
  if(digitalRead(RIGHT_BUTTON_PIN) == LOW)
  {
    directionState = RIGHT;
    digitalWrite(LED_BUILTIN, HIGH); 
  }
  if(digitalRead(LEFT_BUTTON_PIN) == LOW)
  {
    directionState = LEFT;
    digitalWrite(LED_BUILTIN, HIGH); 
  }
    
  if(digitalRead(UP_BUTTON_PIN) == LOW)
  {
    directionState = UP;
    digitalWrite(LED_BUILTIN, HIGH); 
  }
    
  if(digitalRead(DOWN_BUTTON_PIN) == LOW)
  {
    directionState = DOWN;
    digitalWrite(LED_BUILTIN, HIGH);
  }
    
}

// Function to send data to the MCP4822 DAC
void writeDAC(uint16_t channel, uint16_t value) {
  SPI.beginTransaction(spiSets);
  digitalWrite(CS_PIN, LOW);
  SPI.transfer16(channel | (value & 0x0FFF));  // Send channel + value
  digitalWrite(CS_PIN, HIGH);
  SPI.endTransaction();
}

// DRAW SNAKE
void initializeSnake()
{
  int j = 100;
  for(int i = snakeLength; i >= 0; i--)
  { 
    snake[i][0] = 100;
    snake[i][1] = j;
    j++;
  }  
}

void drawPoint(int x, int y)
{
  writeDAC(MCP4822_CHANNEL_1, x);
  writeDAC(MCP4822_CHANNEL_2, y);
}

void drawSnake()
{
  for (int i = 0; i < snakeLength; i++)
  {
    drawPoint(snake[i][0], snake[i][1]);
  }

}

void nextSnake()
{
  // Shift all elements back by one
  for (int i = snakeLength; i > 0; i--)
  {
      snake[i][0] = snake[i - 1][0];
      snake[i][1] = snake[i - 1][1];
  }

  if(directionState == UP)
  {
    snake[0][0] = snake[1][0];
    snake[0][1] = snake[1][1]+1;

    if (snake[0][1] == BOARD_SIZE)
    {
      snake[0][1] = 0;
    }
    return;
  }

  if(directionState == DOWN)
  {
    snake[0][0] = snake[1][0];
    snake[0][1] = snake[1][1]-1;

    if (snake[0][1] < 0)
    {
      snake[0][1] = BOARD_SIZE;
    }
    return;
  }

  if(directionState == RIGHT)
  {
    snake[0][0] = snake[1][0]+1;
    snake[0][1] = snake[1][1];

    if (snake[0][0] > BOARD_SIZE)
    {
      snake[0][0] = 0;
    }
    return;
  }

  if(directionState == LEFT)
  {
    snake[0][0] = snake[1][0]-1;
    snake[0][1] = snake[1][1];

    if (snake[0][0] < 0)
    {
      snake[0][0] = BOARD_SIZE;
    }
    return;
  }
}

// DRAWING BOARD
void drawVerticalLine(int x, int y_start, int y_end)
{
  writeDAC(MCP4822_CHANNEL_2, x);
  for(int i = y_start; i < y_end; i+=3)
  {
    writeDAC(MCP4822_CHANNEL_1, i);
  }
}

void drawHorizontalLine(int y, int x_start, int x_end)
{
  writeDAC(MCP4822_CHANNEL_1, y);
  for(int i = x_start; i < x_end; i+=3)
  {
    writeDAC(MCP4822_CHANNEL_2, i);
  }
}

// Function to draw the boundary square
// Function to draw the boundary square
void drawSquare() {
  drawVerticalLine(0, 0, BOARD_SIZE); // Left side
  drawHorizontalLine(BOARD_SIZE, 0, BOARD_SIZE); // Top side
  drawVerticalLine(BOARD_SIZE, 0, BOARD_SIZE);  // Right side
  drawHorizontalLine(0, 0, BOARD_SIZE); // Bot side
}


void setup() {

  //Serial.begin(9600);
  // SPI settings
  SPI.begin();
  //SPI.setClockDivider(SPI_CLOCK_DIV2); // Set SPI speed

  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);  // Ensure CS pin is high

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RIGHT_BUTTON_PIN, INPUT);
  pinMode(LEFT_BUTTON_PIN, INPUT);
  pinMode(UP_BUTTON_PIN, INPUT);
  pinMode(DOWN_BUTTON_PIN, INPUT);


  // BUTTONS
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), inputInterrupt, FALLING);

  //SNAKE
  initializeSnake();

}

void loop() {


  //drawPoint(0, BOARD_SIZE);
  //delay(1000);
  //drawPoint(0, 0);
  //delay(1000);
  drawSquare();
  drawSnake();
  nextSnake();
  //Serial.println(directionState);


}


