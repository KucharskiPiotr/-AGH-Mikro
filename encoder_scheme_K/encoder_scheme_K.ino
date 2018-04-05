#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>

#include <Adafruit_NeoPixel.h>

// Defines for quick port change
#define OUTPUT_A 5
#define OUTPUT_B 6
#define BUTTON 7
#define N_LED 8
#define LED_PORT A0

/*************************GLOBAL VARIABLES**************************/

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LED, LED_PORT, NEO_GRB + NEO_KHZ800);
ThreadController thr_controller = ThreadController();
float buttonDownCounter = 0;
int encState;
int lastEncState;
float K = 2;      // K is in range 2 to 6
int ledProgram = 0;
bool isChangingK = false;
bool change_program = false;

Thread button_thr = Thread();
Thread turn_thr = Thread();
Thread led_thr = Thread();

/***************************MAIN PROGRAM****************************/

void setup() 
{
  // DEBUG
  Serial.begin(9600);

  // Set pin modes for encoder
  pinMode(OUTPUT_A, INPUT);
  pinMode(OUTPUT_B, INPUT);
  pinMode(BUTTON, INPUT);

  button_thr.onRun(checkButtonAction);
  button_thr.setInterval(10);

  turn_thr.onRun(checkTurnAction);
  turn_thr.setInterval(10);

  led_thr.onRun(setProgram);
  turn_thr.setInterval(10);

  thr_controller.add(&button_thr);
  thr_controller.add(&turn_thr);
  thr_controller.add(&led_thr);

  // Set pin modes for led strip
  strip.begin();
  strip.setBrightness(10);
  strip.show();
  setProgram();
}

void loop() 
{
  //checkButtonAction();
  //checkTurnAction();
  thr_controller.run();
}

/*******************************************************************/

void checkButtonAction()
{
  // Check if K control is triggered
  if(digitalRead(BUTTON) == 0)
  {
    // Applay changes to K after button was clicked to approve
    if(isChangingK)
    {
      // DEBUG
      Serial.println("K change sequence ended.");
      
      isChangingK = false;
    }
    else
    {
      // Button is pushed without K control
      buttonDownCounter += 0.1;
  
      // Button is down for long push
      if(buttonDownCounter >= 8000)
      {
        // DEBUG
        Serial.println("K change sequence triggered");
        delay(1000);

        isChangingK = true;
        buttonDownCounter = 0;
        // K change sequence
        setK();
      } 
    }
  }
  else
  {
    if(!isChangingK)
    {
      // Short click for LED program
      if(buttonDownCounter > 300)
      {
        // DEBUG
        Serial.println("Change LED mode triggered");
  
        // Change LED program
        ledProgram++;
        buttonDownCounter = 0;
        //setProgram();
        change_program = true;
        //thr_controller.remove(&led_thr);
        
      }
    }
    else
    {
      isChangingK = true;
    }
  }
}

/*******************************************************************/

void checkTurnAction()
{
  // Read current state of rotor
  encState = digitalRead(OUTPUT_A);

  // Change if state has changed
  if(encState != lastEncState)
  {
    // Check direction of rotating
    if(digitalRead(OUTPUT_B) != encState)
    {
      // Check if K is being changed
      if(isChangingK)
      {
        if(K < 6) 
        {
          // DEBUG
          Serial.println("K++");
          
          K += 0.5;
          setK();  
        }
      }
      else
      {
        // DEBUG
        Serial.println("Brightness up");

        // Brightness up
        // @TODO ...  
      }
    }
    else
    {
      // Check if K is being changed
      if(isChangingK)
      {
        if(K > 2)
        {
          // DEBUG
          Serial.println("K--");
          K -= 0.5;  
          setK();
        }
      }
      else
      {
        // DEBUG
        Serial.println("Brightness down");

        // Brightness down
        // @TODO ... 
      }
    }
  }

  // Store current state of rotor
  lastEncState = encState;
}

/*******************************************************************/

void setK()
{
  isChangingK = true;

  // Debug
  Serial.print("In setK(): K is currently: ");
  Serial.println(K);
  Serial.print("Current isChangingK:");
  Serial.println(isChangingK);

  for(int i = 0; i < N_LED; i++)
  {
    strip.setPixelColor(i, 0,0,0);
  }

  for(int i = 0; i < (int)K; i++)
  {
    strip.setPixelColor(i, 255,255,255);
    strip.show();
  }

  
}

/*******************************************************************/

void setProgram()
{
  //Serial.print("Jestem w setProgram(): ");
  //Serial.println(ledProgram % 9);
  
  switch(ledProgram % 9)
  {
    case 0:
      program_R();
      break;
    case 1:
      program_G();
      break;
    case 2:
      program_B();
      break;
    case 3:
      program_RG();
      break;
    case 4:
      program_RB();
      break;
    case 5:
      program_GB(); 
      break;
    case 6:
      program_U1();
      break;
    case 7:
      program_U2();
      break;
    case 8:
      program_U3();
      break;
    default:
      Serial.println("Nie mam programu o tym numerze");
      break;
  }
}

/******************************************MIKRO.INO****************************************/
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
  change_program = false;
  while(true)
  {
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
      checkButtonAction();
    }
  }
  change_program = false;
}

void program_U2() // user program No. 2
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

void program_U3() // user program No. 3
{
  
}


