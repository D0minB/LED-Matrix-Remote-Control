#include <LedControl.h>

#define DIN 10
#define CS 9
#define CLK 8

LedControl lc = LedControl(DIN, CLK, CS, 1); //LedControl(dataPin,clockPin,csPin,numDevices)

String received_data, mode, prev_mode;
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
byte one[8] = {0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03};
byte two[8] = {0xFF,0xFF,0x03,0xFF,0xFF,0xC0,0xFF,0xFF};
byte three[8] = {0xFF,0xFF,0x03,0xFF,0xFF,0x03,0xFF,0xFF};
byte four[8] = {0xC3,0xC3,0xC3,0xFF,0xFF,0x03,0x03,0x03};
byte five[8] = {0xFF,0xFF,0xC0,0xFF,0xFF,0x03,0xFF,0xFF};
byte six[8] = {0xFF,0xFF,0xC0,0xFF,0xFF,0xC3,0xFF,0xFF};
byte seven[8] = {0xFF,0xFF,0x03,0x06,0x0C,0x18,0x30,0x60};
byte eight[8] = {0xFF,0xFF,0xC3,0xFF,0xFF,0xC3,0xFF,0xFF};
byte nine[8] = {0xFF,0xFF,0xC3,0xFF,0xFF,0x03,0xFF,0xFF};
byte* numbers[9] = {one, two, three, four, five, six, seven, eight, nine};

byte cube5[3] = {0x14, 0x08, 0x14};
byte cube6[3] = {0x14, 0x14, 0x14};

void setup()
{
  lc.shutdown(0, false);      ///wake up the MAX7219 from power-saving mode
  lc.setIntensity(0, 1);     //address, intensity (0-15)
  lc.clearDisplay(0);
  Serial.begin(9600);
}

void loop()
{ prev_mode = mode;
  if (Serial.available())
  {
    received_data = Serial.readString();

    if (received_data == "up" or received_data == "down" or received_data == "left" or received_data == "right")
    {
      prev_x = pos_x;
      prev_y = pos_y;
      paint_move = true;

      if (received_data == "right" and pos_y < 7)
      {
        pos_y += 1;
      }
      else if (received_data == "left" and pos_y >= 1)
      {
        pos_y -= 1;
      }
      else if (received_data == "up" and pos_x >= 1)
      {
        pos_x -= 1;
      }
      else if (received_data == "down" and pos_x < 7)
      {
        pos_x += 1;
      }
    }
    else if (received_data == "heart" or received_data == "pacman" or
             received_data == "count" or received_data == "paint" or received_data == "stairs")
    {
      mode = received_data;
    }

    else if (received_data == "set")
    {
      set_LED = true;
    }

    else if (received_data == "reset")
    {
      set_LED = false;
    }

    else if (received_data == "clear")
    {
      lc.clearDisplay(0);
    }

    else if (received_data == "clear all")
    {
      lc.clearDisplay(0);
      mode = "";
    }
    else if (received_data == "cube")
    {
      cube();
    }
  }

  if (mode == "paint")
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
  else if (mode == "heart")
  {
    heart();
  }
  else if (mode == "pacman")
  {
    pacman();
  }
  else if (mode == "count")
  {
    count();
  }
  else if (mode == "stairs")
  {
    stairs();
  }
  delay(5);
}


void print_pattern(byte value[])
{
  for (int i = 0 ; i < 8 ; i++)
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

void count()
{
  for (int i = 0; i < sizeof(numbers) / sizeof(numbers[0]); i++)
  {
    print_pattern(numbers[i]);
    delay(1000);
  }
}

void cube()
{
  int v = random(1, 7); // <1;6>
  lc.clearDisplay(0);

  switch (v)
  {
    case 1:
      lc.setLed(0, 3, 4, 1);
      break;
    case 2:
      lc.setLed(0, 4, 3, 1);
      lc.setLed(0, 2, 5, 1);
      break;
    case 3:
      lc.setLed(0, 4, 3, 1);
      lc.setLed(0, 2, 5, 1);
      lc.setLed(0, 3, 4, 1);
      break;
    case 4:
      lc.setLed(0, 4, 3, 1);
      lc.setLed(0, 2, 5, 1);
      lc.setLed(0, 4, 5, 1);
      lc.setLed(0, 2, 3, 1);
      break;
    case 5:
      lc.setRow(0, 2, cube5[0]);
      lc.setRow(0, 3, cube5[1]);
      lc.setRow(0, 4, cube5[2]);
      break;
    case 6:
      lc.setRow(0, 2, cube6[0]);
      lc.setRow(0, 3, cube6[1]);
      lc.setRow(0, 4, cube6[2]);
      break;
  }
}
void stairs()
{
  lc.clearDisplay(0);
  
  for(int i=7; i>=0; i--)
  {
    for(int j=7; j>=i; j--)
    {
      lc.setLed(0,j,i,1);
    }
    delay(500);
  }
}
