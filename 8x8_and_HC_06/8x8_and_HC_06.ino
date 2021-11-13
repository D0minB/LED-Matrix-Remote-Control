#include <LedControl.h>

#define DIN 10
#define CS 9
#define CLK 8

#define left_switch 13
#define right_switch 12

LedControl lc = LedControl(DIN, CLK, CS, 1); //LedControl(dataPin,clockPin,csPin,numDevices)

String mode, prev_mode, received_data;
int pos_x = 4;
int pos_y = 4;
int prev_x, prev_y;
bool set_LED = false;
bool paint_move = false;

//patterns
byte heart1[8] = {0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00};
byte heart2[8] = {0x66, 0x99, 0x81, 0x81, 0x42, 0x24, 0x18, 0x00};
byte pacman1[8] = {0x38, 0x44, 0x88, 0x90, 0x88, 0x44, 0x38, 0x00};
byte pacman2[8] = {0x38, 0x44, 0x82, 0x82, 0x82, 0x44, 0x38, 0x00};
byte zero[5] = {0x07, 0x05, 0x05, 0x05, 0x07};
byte one[5] = {0x01, 0x01, 0x01, 0x01, 0x01};
byte two[5] = {0x07, 0x01, 0x07, 0x04, 0x07};
byte three[5] = {0x07, 0x01, 0x07, 0x01, 0x07};
byte four[5] = {0x05, 0x05, 0x07, 0x01, 0x01};
byte five[5] = {0x07, 0x04, 0x07, 0x01, 0x07};
byte six[5] = {0x07, 0x04, 0x07, 0x05, 0x07};
byte seven[5] = {0x07, 0x01, 0x01, 0x01, 0x01};
byte eight[5] = {0x07, 0x05, 0x07, 0x05, 0x07};
byte nine[5] = {0x07, 0x05, 0x07, 0x01, 0x07};
byte* numbers[10] = {zero, one, two, three, four, five, six, seven, eight, nine};

byte dice5[3] = {0xA0, 0x40, 0xA0};
byte dice6[3] = {0xA0, 0xA0, 0xA0};

/*AVAILABLE COMMANDS
  paint
  set, reset - in paint mode
  heart
  pacman
  draw
  stairs
  dice
  snake
*/

void setup()
{
  lc.shutdown(0, false);      ///wake up the MAX7219 from power-saving mode
  lc.setIntensity(0, 3);     //address, intensity (0-15)
  lc.clearDisplay(0);
  Serial.begin(9600);
  pinMode(left_switch, INPUT_PULLUP);
  pinMode(right_switch, INPUT_PULLUP);
}

void loop()
{
  prev_mode = mode;

  if (Serial.available())
  {
    receive_data();
  }

  if (mode == "paint")
  {
    paint();
  }
  else if (mode == "heart")
  {
    heart();
  }
  else if (mode == "pacman")
  {
    pacman();
  }
  else if (mode == "stairs")
  {
    stairs();
  }
}

void receive_data()
{
  received_data = Serial.readString();

  if (sizeof(received_data) <= 3)
  {
    int intensity = received_data.toInt();
    if (intensity >= 0 && intensity <= 15)
    {
      lc.setIntensity(0, intensity);
    }
  }
  else
  {
    mode = received_data;
  }

  if (mode == "up" or mode == "down" or mode == "left" or mode == "right")
  {
    prev_x = pos_x;
    prev_y = pos_y;
    paint_move = true;

    if (mode == "right" and pos_y < 7)
    {
      pos_y += 1;
    }
    else if (mode == "left" and pos_y >= 1)
    {
      pos_y -= 1;
    }
    else if (mode == "up" and pos_x >= 1)
    {
      pos_x -= 1;
    }
    else if (mode == "down" and pos_x < 7)
    {
      pos_x += 1;
    }
  }

  else if (mode == "set")
  {
    set_LED = true;
  }

  else if (mode == "reset")
  {
    set_LED = false;
  }
  else if (mode == "draw")
  {
    draw();
  }
  else if (mode == "clear")
  {
    lc.clearDisplay(0);
    mode = "";
  }
  else if (mode == "dice")
  {
    dice();
  }
  else if (mode == "snake")
  {
    snake();
  }
}

void print_pattern(byte value[])
{
  for (int i = 0; i < 8; i++)
  {
    lc.setRow(0, i, value[i]);
  }
}

void heart()
{
  print_pattern(heart1);
  delay(500);
  print_pattern(heart2);
  delay(500);
}

void pacman()
{
  print_pattern(pacman1);
  for (int i = 0; i < 5; i++)
  {
    lc.setLed(0, 3, 7 - i, 1);
    if (i > 0)
    {
      lc.setLed(0, 3, 7 - i + 1, 0);
    }
    if (i != 4)
    {
      delay(500);
    }
    else
    {
      delay(50);
    }
  }
  print_pattern(pacman2);
  delay(500);
}

void draw()
{
  for (int i = 0; i < 10; i++)
  {
    int v = random(0, 100); // <0;99>
    int v1 = v / 10;
    int v2 = v - v1 * 10;

    byte x[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    for (int i = 0; i < 5; i++)
    {
      x[i + 2] = 16 * numbers[v1][i] + numbers[v2][i]; // *16 - shift register left 4 bits
    }
    print_pattern(x);
    delay(100);
  }
}

void dice()
{
  for (int i = 0; i < 7; i++)
  {
    int v1 = random(1, 7); // <1;6>
    int v2 = random(1, 7);
    lc.clearDisplay(0);

    switch (v1)
    {
      case 1:
        lc.setLed(0, 1, 1, 1);
        break;
      case 2:
        lc.setLed(0, 0, 0, 1);
        lc.setLed(0, 2, 2, 1);
        break;
      case 3:
        lc.setLed(0, 0, 0, 1);
        lc.setLed(0, 1, 1, 1);
        lc.setLed(0, 2, 2, 1);
        break;
      case 4:
        lc.setLed(0, 0, 0, 1);
        lc.setLed(0, 2, 0, 1);
        lc.setLed(0, 0, 2, 1);
        lc.setLed(0, 2, 2, 1);
        break;
      case 5:
        lc.setRow(0, 0, dice5[0]);
        lc.setRow(0, 1, dice5[1]);
        lc.setRow(0, 2, dice5[2]);
        break;
      case 6:
        lc.setRow(0, 0, dice6[0]);
        lc.setRow(0, 1, dice6[1]);
        lc.setRow(0, 2, dice6[2]);
        break;
    }
    switch (v2)
    {
      case 1:
        lc.setLed(0, 6, 6, 1);
        break;
      case 2:
        lc.setLed(0, 5, 5, 1);
        lc.setLed(0, 7, 7, 1);
        break;
      case 3:
        lc.setLed(0, 5, 5, 1);
        lc.setLed(0, 6, 6, 1);
        lc.setLed(0, 7, 7, 1);
        break;
      case 4:
        lc.setLed(0, 7, 7, 1);
        lc.setLed(0, 5, 7, 1);
        lc.setLed(0, 7, 5, 1);
        lc.setLed(0, 5, 5, 1);
        break;
      case 5:
        lc.setRow(0, 5, dice5[0] / 32); //shift register - right 5 bits
        lc.setRow(0, 6, dice5[1] / 32);
        lc.setRow(0, 7, dice5[2] / 32);
        break;
      case 6:
        lc.setRow(0, 5, dice6[0] / 32);
        lc.setRow(0, 6, dice6[1] / 32);
        lc.setRow(0, 7, dice6[2] / 32);
        break;
    }
    delay(200);
  }

}

void stairs()
{
  lc.clearDisplay(0);

  for (int i = 7; i >= 0; i--)
  {
    for (int j = 7; j >= i; j--)
    {
      lc.setLed(0, j, i, 1);
    }
    delay(500);
  }
}

void paint()
{
  if (prev_mode != "paint")
  {
    lc.clearDisplay(0);
  }
  if (paint_move == true)
  {
    lc.setLed(0, prev_x, prev_y, set_LED);
    set_LED = false;
    paint_move = false;
  }
  else if (set_LED == true)
  {
    lc.setLed(0, pos_x, pos_y, 1);
  }
  else if (set_LED == false)
  {
    lc.setLed(0, pos_x, pos_y, 1);
    delay(300);
    lc.setLed(0, pos_x, pos_y, 0);
    delay(300);
  }
}

void snake()
{
  uint8_t pos_x[30] = {3, 4};
  uint8_t pos_y[30] = {4, 4};
  uint8_t fruit[2] = {(uint8_t)random(0, 8), (uint8_t)random(0, 8)};
  uint8_t snake_size = 2;
  String dir = "left";
  bool right = false;
  bool left = false;

  unsigned long prev_time = millis();


  bool end_game = false;
  while (!end_game)
  {

    if (millis() - prev_time >= (unsigned long)500)
    {
      prev_time = millis();
      lc.clearDisplay(0);

      if (digitalRead(right_switch) == 0) right = true;
      if (digitalRead(left_switch) == 0) left = true;

      if (right == true)
      {
        if (dir == "up") dir = "right";
        else if (dir == "down") dir = "left";
        else if (dir == "right") dir = "down";
        else dir = "up";
        right = false;
      }

      else if (left == true)
      {
        if (dir == "up") dir = "left";
        else if (dir == "down") dir = "right";
        else if (dir == "right") dir = "up";
        else dir = "down";
        left = false;
      }

      lc.setLed(0, fruit[1], fruit[0], 1);
      //SNAKE MOVEMENT
      uint8_t last_x = pos_x[snake_size - 1];
      uint8_t last_y = pos_y[snake_size - 1];

      for (int i = 1; i < snake_size; i++)
      {
        pos_x[i] = pos_x[i - 1];
        pos_y[i] = pos_y[i - 1];
      }

      if (fruit[1] == pos_y[0] && fruit[0] == pos_x[0]) //SNAKE EATS FRUIT
      {
        snake_size += 1;
        fruit[0] = random(0, 8);
        fruit[1] = random(0, 8);
        pos_x[snake_size] = last_x;
        pos_y[snake_size] = last_y;
      }



      if (dir == "left")
      {
        pos_x[0] - 1 >= 0 ? pos_x[0] -= 1 : pos_x[0] = 7;
      }
      else if (dir == "right")
      {
        pos_x[0] + 1 <= 7 ? pos_x[0] += 1 : pos_x[0] = 0;
      }
      else if (dir == "up")
      {
        pos_y[0] - 1 >= 0 ? pos_y[0] -= 1 : pos_y[0] = 7;
      }
      else if (dir == "down")
      {
        pos_y[0] + 1 <= 7 ? pos_y[0] += 1 : pos_y[0] = 0;
      }

      for (int i = 0; i < snake_size; i++)
      {
        lc.setLed(0, pos_y[i], pos_x[i], 1);
      }
    }
  }

}
