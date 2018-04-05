#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, A0, NEO_GRB + NEO_KHZ800); // initialization of LED strip

//////////////////////////////////

void setup() 
{
  strip.begin();
  strip.show();
}

//////////////////////////////////

void set_color(int R, int G, int B) // main function, which sets colors on LED strip
{
  int i = 0;
  for(i = 0; i < 8; i++)
  {
    strip.setBrightness(10);
    strip.setPixelColor(i, strip.Color(R, G, B));
    strip.show();
  }
}

void set_K(int k)
{
  
}

void program_R() // setting RED color
{
  set_color(255, 0, 0);
}

void program_G() // setting GREEN color
{
  set_color(0, 255, 0);
}

void program_B() // setting BLUE color
{
  set_color(0, 0, 255);
}

void program_RG() // setting RED and GREEN colors
{
  set_color(255, 255, 0);
}

void program_RB() // setting RED and BLUE colors
{
  set_color(255, 0, 255);
}

void program_GB() // setting GREEN and BLUE colors
{
  set_color(0, 255, 255);
}

void program_U1() // user program No. 1
{
  int i = 0;
  for(i = 0; i < 8; i++)
  {
    if(i % 2 == 0)
    {
      strip.setBrightness(5);
      strip.setPixelColor(i, strip.Color(255, 0, 0));
      strip.show();
      delay(250);
      strip.setBrightness(0);
    }
    else
    {
      strip.setBrightness(5);
      strip.setPixelColor(i, strip.Color(0, 0, 255));
      strip.show();
      delay(250);
      strip.setBrightness(0);
    }
  }
}

void program_U2() // user program No. 2
{
  while(true)
  {
    int i = 0;
    int rand_nr = random(0, 8);
    int rand_nrR = random(10, 50);
    int rand_nrG = random(10, 50);
    int rand_nrB = random(10, 50);
    for(i = 0; i < 8; i++)
    {
      strip.setBrightness(10);
      strip.setPixelColor(rand_nr, strip.Color(rand_nrR, rand_nrG, rand_nrB));
      strip.show();
      delay(10);
    }    
  }
}

void program_U3() // user program No. 3
{
  
}

//////////////////////////////////

void loop() 
{
  program_U1();
}
