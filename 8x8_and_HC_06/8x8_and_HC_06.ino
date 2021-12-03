#include <LedControl.h>
#define DIN 10
#define CS 9
#define CLK 8
#define left_switch 13
#define right_switch 12

LedControl lc = LedControl(DIN, CLK, CS, 4);  //LedControl(dataPin,clockPin,csPin,numDevices)

String modes = "    ";  //'h' - heart, 'p' - pacman, 's' - stairs, ''

//patterns
byte heart1[7] = {0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18};
byte heart2[7] = {0x66, 0x99, 0x81, 0x81, 0x42, 0x24, 0x18};
byte pacman1[7] = {0x38, 0x44, 0x88, 0x90, 0x88, 0x44, 0x38};
byte pacman2[7] = {0x38, 0x44, 0x82, 0x82, 0x82, 0x44, 0x38};
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
byte *numbers[10] = {zero, one, two, three, four, five, six, seven, eight, nine};
byte dice5[3] = {0xA0, 0x40, 0xA0};
byte dice6[3] = {0xA0, 0xA0, 0xA0};

bool matrixes[4] = {0, 0, 0, 0};
bool heart_state = 0;
unsigned long heart_t = millis();
unsigned long stairs_t = millis();
unsigned long pacman_t = millis();
unsigned long random_num_t = millis();
uint8_t stairs_i = 0;
uint8_t pacman_i = 0;
uint8_t prescaler = 4; //gif changing freq prescaler

void setup() 
{
	for (int i=0; i<4; i++) 
  {
		lc.shutdown(i, false);  //wake up the MAX7219 from power-saving mode
		lc.setIntensity(i, 1);  //address, intensity (0-15)
		lc.clearDisplay(i);
	}
	Serial.begin(9600);
	pinMode(left_switch, INPUT_PULLUP);
	pinMode(right_switch, INPUT_PULLUP);
}

void loop() 
{
	if (Serial.available()) 
  {
		receive_data();
	}

  //GIFs
	for (int i = 0; i < modes.length(); i++) 
  { 
		if (modes.indexOf('s') != -1) //stairs
    {
      if(millis() - stairs_t >= 4000/prescaler)
      {
        stairs();
        stairs_i++;
        if(stairs_i == 8) stairs_i=0;
        stairs_t = millis();
      }
		} 
    if ((modes.indexOf('p') != -1))  //pacman
    { 
      if(millis() - pacman_t >= 2000/prescaler)
      {
        pacman();
        pacman_i++;
        if(pacman_i == 5) pacman_i=0;
        pacman_t = millis();
      }
		} 
    if (modes.indexOf('h') != -1) //heart
    {
      if(millis() - heart_t >= 2000/prescaler)
      {
        heart_t = millis();
        heart(heart_state);
      }
		}
   if (modes.indexOf('r') != -1)
   {
      if(millis() - random_num_t >= 2000/prescaler)
      {
        random_num_t = millis();
        random_pattern();
      }
   }
	}
}

void receive_data() 
{
	String received_data = Serial.readString();

  String cmd[4] = {"ch1", "ch2", "ch3", "ch4"};

  for(int i=0; i<4; i++)
  {
    if (received_data.indexOf(cmd[i]) != -1)  //TOGGLE SCREEN ON/OFF STATE 
    {
      matrixes[i] = !matrixes[i];
    } 
  }
  
  if (received_data.indexOf("sl") != -1) //SLIDER HANDLE
  {
    if(prescaler >= 1 and prescaler <=9)
    {
      prescaler = received_data[received_data.indexOf("sl") + 2] - 48; //from ACII table '0' = 48, '1' = 49, ...
    }
  }
  if (received_data.indexOf("dice") != -1) 
  {
    dice();
	} 
  else if (received_data.indexOf("snake") != -1) 
  {
		snake();	
	} 
  else if (received_data.indexOf("draw") != -1) 
  {
		draw();	
	} 
  else if (received_data.indexOf("heart") != -1 or received_data.indexOf("pacman") != -1 or received_data.indexOf("stairs") != -1 or received_data.indexOf("random") != -1) 
  {
    String tab[4] = {"heart", "pacman", "stairs", "random"};
    
    for(int j=0; j<4; j++)
    {
      if(received_data.indexOf(tab[j]) != -1)
      {
        for (int i = 0; i < 4; i++) 
        {
			    if (matrixes[i]) 
          {
				    modes[i] = received_data[received_data.indexOf(tab[j])];
			    }
		    }
      }
    }
	}
  else if(received_data.indexOf("clear") != -1) 
  {
    clear();
  }
}

void print_pattern(byte *value, uint8_t n, uint8_t addr) 
{
	for (int i = 0; i < n; i++) 
  {
		lc.setRow(addr, i, value[i]);
	}
}

void heart(bool state) 
{
  for (int i = 0; i < 4; i++) 
  {
    if (modes[i] == 'h') 
    {
      state == 1 ? print_pattern(heart2, sizeof(heart2), i) : print_pattern(heart1, sizeof(heart1), i);
    }
  }	
  heart_state = !state; //TOGGLE HEART STATE
}

void pacman() 
{
  if(pacman_i == 0)
  {
    for (int i=0; i<4; i++) 
    {
      if (modes[i] == 'p') print_pattern(pacman1, sizeof(pacman1), i); //FIRST FRAME
    } 
  }
  else if(pacman_i >= 0 and pacman_i < 4)
  {
    for (int j = 0; j < 4; j++) 
    {
		  if (modes[j] == 'p') 
      {
				lc.setLed(j, 3, 7 - pacman_i, 1);
				lc.setLed(j, 3, 7 - pacman_i + 1, 0);
			}
    }
  }
  else
  {
    for (int i=0; i<4; i++) 
    {
      if (modes[i] == 'p') print_pattern(pacman2, sizeof(pacman2), i); //END FRAME
    }
  }
}

void draw() 
{
  reset_mode();	
	for (int i = 0; i < 10; i++) //10 RANDOM NUMBERS
  {
    for (int j = 0; j < 4; j++) 
    {
		  if (matrixes[j]) 
      {
				int v = random(0, 100); //<0;99>
				int v1 = v / 10;
				int v2 = v - v1 * 10;
				byte x[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
				for (int i = 0; i < 5; i++) 
        {
					x[i + 2] = 16 * numbers[v1][i] + numbers[v2][i]; // *16 - shift register - 4 bits left
				}
				print_pattern(x, sizeof(x), j);
			}
    }
    delay(100); 
  }
}

void dice() 
{
  reset_mode();
	for (int j = 0; j < 7; j++) //7 DICES DRAWING 
  {
		for (int k = 0; k < 4; k++) 
    {
			if (matrixes[k]) 
      {
				lc.clearDisplay(k);
				int v[2] = {(int) random(1, 7), (int) random(1, 7)}; // 2x <1; 6>

				for (int i = 0; i < 2; i++) 
        {
					switch (v[i]) 
          {
						case 1:
						lc.setLed(k, 1 + 5 *i, 1 + 5 *i, 1);
						break;
						case 2:
						lc.setLed(k, 0 + 5 *i, 0 + 5 *i, 1);
						lc.setLed(k, 2 + 5 *i, 2 + 5 *i, 1);
						break;
						case 3:
						lc.setLed(k, 0 + 5 *i, 0 + 5 *i, 1);
						lc.setLed(k, 1 + 5 *i, 1 + 5 *i, 1);
						lc.setLed(k, 2 + 5 *i, 2 + 5 *i, 1);
						break;
						case 4:
						lc.setLed(k, 0 + 5 *i, 0 + 5 *i, 1);
						lc.setLed(k, 2 + 5 *i, 0 + 5 *i, 1);
						lc.setLed(k, 0 + 5 *i, 2 + 5 *i, 1);
						lc.setLed(k, 2 + 5 *i, 2 + 5 *i, 1);
						break;
						case 5:
						lc.setRow(k, 0 + 5*i, dice5[0] / pow(32,i));
						lc.setRow(k, 1 + 5*i, dice5[1] / pow(32,i));
						lc.setRow(k, 2 + 5*i, dice5[2] / pow(32,i));
						break;
						case 6:
						lc.setRow(k, 0 + 5*i, dice6[0] / pow(32,i));
						lc.setRow(k, 1 + 5*i, dice6[1] / pow(32,i));
						lc.setRow(k, 2 + 5*i, dice6[2] / pow(32,i));
						break;
					}
				}
			}
		}
		delay(200);
	}
}

void stairs() 
{
  if(stairs_i == 0) //FIRST FRAME - CLEAR DISPLAY
  {
    for (int i=0; i<4; i++)
    {
      if(modes[i] == 's')
      {
        lc.clearDisplay(i);
      }
    }
  }

	for (int j = 7; j >= stairs_i; j--)  //NEXT FRAMES
  {
		for (int k = 0; k < 4; k++) 
    {
			if (modes[k] == 's') 
      {
					lc.setLed(k, j, stairs_i, 1);
		  }
		}
  }
}

void snake() 
{
	uint8_t pos_x[30] = {3, 4};
	uint8_t pos_y[30] = {4, 4};
	uint8_t fruit[2] = {(uint8_t) random(0, 8), (uint8_t) random(0, 8)};
	uint8_t snake_size = 2;
	String dir = "left";
	bool right = false;
	bool left = false;
	unsigned long prev_time = millis();
	bool end_game = false;
	while (!end_game) {
		if (millis() - prev_time >= (unsigned long) 500) {
			prev_time = millis();
			lc.clearDisplay(0);
			if (digitalRead(right_switch) == 0) right = true;
			if (digitalRead(left_switch) == 0) left = true;
			if (right == true) {
				if (dir == "up") dir = "right"; else if (dir == "down") dir = "left"; else if (dir == "right") dir = "down"; else dir = "up";
				right = false;
			} else if (left == true) {
				if (dir == "up") dir = "left"; else if (dir == "down") dir = "right"; else if (dir == "right") dir = "up"; else dir = "down";
				left = false;
			}
			lc.setLed(0, fruit[1], fruit[0], 1);
			//SNAKE MOVEMENT
			uint8_t last_x = pos_x[snake_size - 1];
			uint8_t last_y = pos_y[snake_size - 1];
			for (int i = 1; i < snake_size; i++) {
				pos_x[i] = pos_x[i - 1];
				pos_y[i] = pos_y[i - 1];
			}
			if (fruit[1] == pos_y[0] && fruit[0] == pos_x[0])	//SNAKE EATS FRUIT {
				snake_size += 1;
				fruit[0] = random(0, 8);
				fruit[1] = random(0, 8);
				pos_x[snake_size] = last_x;
				pos_y[snake_size] = last_y;
			}
			if (dir == "left") {
				pos_x[0] - 1 >= 0 ? pos_x[0] -= 1 : pos_x[0] = 7;
			} else if (dir == "right") {
				pos_x[0] + 1 <= 7 ? pos_x[0] += 1 : pos_x[0] = 0;
			} else if (dir == "up") {
				pos_y[0] - 1 >= 0 ? pos_y[0] -= 1 : pos_y[0] = 7;
			} else if (dir == "down") {
				pos_y[0] + 1 <= 7 ? pos_y[0] += 1 : pos_y[0] = 0;
			}
			for (int i = 0; i < snake_size; i++) 
      {
				lc.setLed(0, pos_y[i], pos_x[i], 1);
			}
		}
}

void reset_mode()
{
  for (int i=0; i<4; i++)
  {
    if(matrixes[i])
    {
      modes[i] = ' ';
    }
  }
}

void clear()
{
  for (int i=0; i<4; i++)
  {
    if(matrixes[i])
    {
      modes[i] = ' ';
      lc.clearDisplay(i);
    }
  }
}

void random_pattern()
{
  byte rand_num[8] = {byte(random(0, 255)),byte(random(0, 255)),byte(random(0, 255)),byte(random(0, 255)),byte(random(0, 255)),byte(random(0, 255)),byte(random(0, 255)),byte(random(0, 255))};

  for(int i=0; i<4; i++)
  {
    if(modes[i] == 'r')
    {
      print_pattern(rand_num, sizeof(rand_num), i); 
    }
  }
}
  
