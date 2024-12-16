#include <Arduino.h>
#include <SPI.h>
#include <time.h>
#include <stdlib.h>


#define CS_PIN 10
#define LDAC 8
#define MCP4822_CHANNEL_1  0x3000  // Select channel A (bit 14)
#define MCP4822_CHANNEL_2  0xB000  // Select channel B (bit 14)


#define RIGHT_BUTTON_PIN 7
#define LEFT_BUTTON_PIN 6
#define UP_BUTTON_PIN 5
#define DOWN_BUTTON_PIN 4
#define INTERRUPT_PIN 2
#define TIME_PIN 9

#define RIGHT 0
#define LEFT 1
#define UP 2
#define DOWN 3

#define BOARD_SIZE 256
#define EATING_RADIUS 10
#define SNAKE_STEP 2

#define CLOCK_FREQ 2000
#define FRAMES_PER_SECOND 60
#define NT 4
int cur_task = NT;

uint16_t snake[BOARD_SIZE][2];
uint16_t fruit[2] = {150, 150};
int snakeLength = 20;

SPISettings spiSets (20000000, MSBFIRST, SPI_MODE0);

int directionState = UP;


typedef struct {
  /* period in ticks */
  int period;
  /* ticks until next activation */
  int delay;
  /* function pointer */
  void (*func)(void);
  /* activation counter */
  int exec;
} Sched_Task_t;

Sched_Task_t Tasks[NT];

void Sched_Init(void)
{
  for(int x=0; x<NT; x++)
    Tasks[x].func = 0;

  /* Also configures interrupt that periodically calls Sched_Schedule(). */
  noInterrupts(); // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
 
  //OCR1A = 6250; // compare match register 16MHz/256/10Hz
  //OCR1A = 31250; // compare match register 16MHz/256/2Hz
  OCR1A = 31;    // compare match register 16MHz/256/2kHz
  //OCR1A = 1563;    // compare match register 16MHz/256/40Hz
  TCCR1B |= (1 << WGM12); // CTC mode
  TCCR1B |= (1 << CS12); // 256 prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  interrupts(); // enable all interrupts  
}

int Sched_AddT(void (*f)(void), int d, int p){
  for(int x=0; x<NT; x++)
  {
    if (!Tasks[x].func)
    {
      Tasks[x].period = p;
      Tasks[x].delay = d;
      Tasks[x].exec = 0;
      Tasks[x].func = f;
      return x;
    }
  }

  return -1;
}

void Sched_Schedule(void)
{
  for(int x=0; x<NT; x++) {
    if(Tasks[x].func)
    {
      if(Tasks[x].delay)
      {
        Tasks[x].delay--;
      } 
      else
      {
        /* Schedule Task */
        Tasks[x].exec++;
        Tasks[x].delay = Tasks[x].period-1;
      }
    }
  }
}

void Sched_Dispatch(void){
  int prev_task = cur_task;
  for(int x=0; x<cur_task; x++)
  {
    if((Tasks[x].func)&&(Tasks[x].exec))
    {
      Tasks[x].exec=0;
      cur_task = x;
      interrupts();
      Tasks[x].func();
      noInterrupts();
      cur_task = prev_task;
      /* Delete task if one-shot */
      if(!Tasks[x].period) Tasks[x].func = 0;
    }
  }
}


// Buttons
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

void drawPoint(uint16_t x, uint16_t y)
{

  x = x << 3;
  y = y << 3;

  SPI.beginTransaction(spiSets);
  digitalWrite(CS_PIN, LOW);
  SPI.transfer16(MCP4822_CHANNEL_1 | (x & 0x0FFF));
  digitalWrite(CS_PIN, HIGH);

  digitalWrite(CS_PIN, LOW);
  SPI.transfer16(MCP4822_CHANNEL_2 | (y & 0x0FFF));
  digitalWrite(CS_PIN, HIGH);
  SPI.endTransaction();

  // Apply changes simultaneously by pulsing LDAC low
  digitalWrite(LDAC, LOW);
  delayMicroseconds(1);  // Ensure LDAC pulse is at least 1 microsecond
  digitalWrite(LDAC, HIGH);
    

}

void drawSnake()
{
  for (int i = 0; i < snakeLength; i++)
  {
    drawPoint(snake[i][0], snake[i][1]);
  }

}

// DRAWING BOARD
void drawVerticalLine(int x, int y_start, int y_end)
{
  for(int i = y_start; i < y_end; i+=3)
  {
    drawPoint(x, i);
  }
}

void drawHorizontalLine(int y, int x_start, int x_end)
{
  for(int i = x_start; i < x_end; i+=3)
  {
    drawPoint(y, i);
  }
}

// Function to draw the boundary square
// Function to draw the boundary square
void drawSquare() 
{
  drawVerticalLine(0, 0, BOARD_SIZE); // Left side
  drawHorizontalLine(BOARD_SIZE, 0, BOARD_SIZE); // Top side
  drawVerticalLine(BOARD_SIZE, 0, BOARD_SIZE);  // Right side
  drawHorizontalLine(0, 0, BOARD_SIZE); // Bot side
}


// Snake Logic
void initializeSnake()
{
  int j = 200;
  for(int i = snakeLength; i >= 0; i--)
  { 
    snake[i][0] = 150;
    snake[i][1] = j;
    j++;
  }  
}

void drawFruit()
{
  drawPoint(fruit[0], fruit[1]);
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
    snake[0][1] = snake[1][1]+SNAKE_STEP;

    if(((snake[0][0] >= fruit[0] - EATING_RADIUS && snake[0][0] <= fruit[0] + EATING_RADIUS)) && (snake[0][1] >= fruit[1] - EATING_RADIUS && snake[0][1] <= fruit[1] + EATING_RADIUS))
    {

      snakeLength += 10;
      fruit[0] = random(EATING_RADIUS, BOARD_SIZE - EATING_RADIUS);
      fruit[1] = random(EATING_RADIUS, BOARD_SIZE - EATING_RADIUS);
    }


    if (snake[0][1] == BOARD_SIZE)
    {
      snake[0][1] = 0;
    }

  }

  else if(directionState == DOWN)
  {
    snake[0][0] = snake[1][0];
    snake[0][1] = snake[1][1]-SNAKE_STEP;

    if(((snake[0][0] >= fruit[0] - EATING_RADIUS && snake[0][0] <= fruit[0] + EATING_RADIUS)) && (snake[0][1] >= fruit[1] - EATING_RADIUS && snake[0][1] <= fruit[1] + EATING_RADIUS))
    {
      snakeLength += 10;
      fruit[0] = random(EATING_RADIUS, BOARD_SIZE - EATING_RADIUS);
      fruit[1] = random(EATING_RADIUS, BOARD_SIZE - EATING_RADIUS);
    }


    if (snake[0][1] == 0)
    {
      snake[0][1] = BOARD_SIZE;
    }

  }

  else if(directionState == RIGHT)
  {
    snake[0][0] = snake[1][0]+SNAKE_STEP;
    snake[0][1] = snake[1][1];


    if(((snake[0][0] >= fruit[0] - EATING_RADIUS && snake[0][0] <= fruit[0] + EATING_RADIUS)) && (snake[0][1] >= fruit[1] - EATING_RADIUS && snake[0][1] <= fruit[1] + EATING_RADIUS))
    {
      snakeLength += 10;
      fruit[0] = random(EATING_RADIUS, BOARD_SIZE - EATING_RADIUS);
      fruit[1] = random(EATING_RADIUS, BOARD_SIZE - EATING_RADIUS);
    }

    if (snake[0][0] == BOARD_SIZE)
    {
      snake[0][0] = 0;
    }
  }

  else if(directionState == LEFT)
  {
    snake[0][0] = snake[1][0]-SNAKE_STEP;
    snake[0][1] = snake[1][1];

    
    if(((snake[0][0] >= fruit[0] - EATING_RADIUS && snake[0][0] <= fruit[0] + EATING_RADIUS)) && (snake[0][1] >= fruit[1] - EATING_RADIUS && snake[0][1] <= fruit[1] + EATING_RADIUS))
    {
      snakeLength += 10;
      fruit[0] = random(EATING_RADIUS, BOARD_SIZE - EATING_RADIUS);
      fruit[1] = random(EATING_RADIUS, BOARD_SIZE - EATING_RADIUS);
    }

    if (snake[0][0] == 0)
    {
      snake[0][0] = BOARD_SIZE;
    }

  }
}


void setup() {
  // To create random numbers
  randomSeed(analogRead(A0));

  // SPI settings
  SPI.begin();
  //SPI.setClockDivider(SPI_CLOCK_DIV2); // Set SPI speed
  pinMode(LDAC, OUTPUT);
  digitalWrite(LDAC, HIGH);

  pinMode(CS_PIN, OUTPUT);  
  digitalWrite(CS_PIN, HIGH);  // Ensure CS pin is high


  pinMode(RIGHT_BUTTON_PIN, INPUT);
  pinMode(LEFT_BUTTON_PIN, INPUT);
  pinMode(UP_BUTTON_PIN, INPUT);
  pinMode(DOWN_BUTTON_PIN, INPUT);

  pinMode(TIME_PIN, OUTPUT);

  // BUTTONS
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), inputInterrupt, FALLING);

  // SNAKE
  initializeSnake();

  // Tasks
  Sched_Init();
  // Task, delay, period
  Sched_AddT(nextSnake, 0, 50);
  Sched_AddT(drawFruit, 0,  50);
  Sched_AddT(drawSnake, 0,  50);
  Sched_AddT(drawSquare, 0,  50);
}

ISR(TIMER1_COMPA_vect){//timer1 interrupt
  Sched_Schedule();
  Sched_Dispatch();
}

void loop()
{

}



