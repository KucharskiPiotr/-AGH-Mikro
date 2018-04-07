#include <Adafruit_NeoPixel.h>  // LED strip
#include <RotaryEncoder.h>       // Rotary encoder

// PORTS
#define ROTARY_A 5
#define ROTARY_B 6
#define BUTTON 7
#define N_LED 8
#define LED_PORT A0

// MAX/MIN VALUES
#define MAX_BRIGHT 100
#define MIN_BRIGHT 10
#define MAX_LEVELS_SIZE 64    // pow(2, 6)

/************************ Global variables *************************/
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LED, LED_PORT, NEO_GRB + NEO_KHZ800);
RotaryEncoder encoder(ROTARY_A, ROTARY_B);

float button_down_counter = 0;  
int last_pos = -1;
float K = 2;                  // K is in range 2 to 6
int led_program = 0;

bool is_changing_K = false;
bool U1 = false;
bool U2 = false;
bool U3 = false;

int bright_level[MAX_LEVELS_SIZE];
int curr_bright_level = 0;
int max_curr_bright_level = pow(2, (int)K) - 1;

/**************************** MAIN *********************************/
void setup() 
{
  // Serial communication is for debugging
  Serial.begin(9600);

  // Button from encoder initialization
  pinMode(BUTTON, INPUT);

  // Brightness level initialization
  calc_brightness_levels();

  // Led initialization
  strip.begin();
  strip.setBrightness(bright_level[curr_bright_level]);
  strip.show();

  // Set first program to leds
  set_program();
}

void loop() 
{
  // Set max brightness index for table
  max_curr_bright_level = pow(2, (int)K);

  // Input check
  check_button();
  check_encoder();

  // User programs
  if(!is_changing_K)
  {
    if(U1) {  Serial.print("loop: U1, button_down_counter = "); Serial.println(button_down_counter); program_U1(); }
    if(U2) {  Serial.print("loop: U2, button_down_counter = "); Serial.println(button_down_counter); program_U2(); }
    if(U3) {  Serial.print("loop: U3, button_down_counter = "); Serial.println(button_down_counter); program_U3(); } 
  }
}

/*******************************************************************/
void calc_brightness_levels()
{
  Serial.print("In calc_brightness_levels, size: ");
  Serial.println(pow(2, (int)K));
  
  for(int i = 0; i < pow(2, (int)K); i++)
  {
    bright_level[i] = MIN_BRIGHT + (i * ((MAX_BRIGHT - MIN_BRIGHT) / (pow(2, (int)K) - 1)));  
    Serial.print(i);
    Serial.print(": ");
    Serial.println(bright_level[i]);
  }
}

/*******************************************************************/
void check_button()
{
  // Check if K control is triggered
  if(digitalRead(BUTTON) == 0)
  {
    // Applay changes to K after button was clicked to approve
    if(is_changing_K)
    {
      // DEBUG
      Serial.println("K change sequence ended.");

      button_down_counter = 0;
      delay(200);
      is_changing_K = false;
    }
    else
    {
      // Button is pushed without K control
      if(U1 || U2 || U3)
      {
        button_down_counter += 500;
      }
      else { button_down_counter += 0.1; }

      Serial.println(button_down_counter);
  
      // Button is down for long push (K control)
      if(button_down_counter >= 30)
      {
        // DEBUG
        Serial.println("K change sequence triggered");
        delay(1000);

        is_changing_K = true;
        button_down_counter = 0;
        // K change sequence
        set_K();
      } 
    }
  }
  else
  {
    if(!is_changing_K)
    {
      // Short click for LED program
      if(button_down_counter > 2)
      {
        // DEBUG
        Serial.println("Change LED mode triggered");
  
        // Change LED program
        led_program++;
        button_down_counter = 0;
        set_program();
        //change_program = true;
        //thr_controller.remove(&led_thr);
        
      }
    }
    else
    {
      is_changing_K = true;
    }
  }
}

/*******************************************************************/
void check_encoder()
{
  encoder.tick();

  // Read position from encoder
  int new_pos = encoder.getPosition();

  // Position has changed
  if(new_pos != last_pos)
  {
    if(new_pos < last_pos)    // Clockwise turn
    {
      Serial.println(new_pos);
      if(is_changing_K)       // K is being set -> K++
      {
        if(K < 6) { Serial.println("K++"); K += 0.5; set_K(); }
      }
      else                    // Brightnes is being set -> Bright++
      {
        if(curr_bright_level < max_curr_bright_level) 
        {
          Serial.print("Bright++: "); 
          curr_bright_level++; 
        }
      }
    }
    else                      // Counterclockwise turn
    {
      Serial.println(new_pos);
      if(is_changing_K)       // K is being set -> K--
      {
        if(K > 2) { Serial.println("K--"); K -= 0.5; set_K(); }
      }
      else                    // Brightnes is being set -> Bright--
      {
        if(curr_bright_level > 0) 
        {
          Serial.print("Bright--: ");
          curr_bright_level--; 
        }
      }
    }
    Serial.println(curr_bright_level);
    last_pos = new_pos;       // Set current position as old one bor next turn
  }
}

/*******************************************************************/
void set_K()
{
  is_changing_K = true;

  // Debug
  Serial.print("In setK(): K is currently: ");
  Serial.println(K);
  Serial.print("Current isChangingK:");
  Serial.println(is_changing_K);

  for(int i = 0; i < N_LED; i++)
  {
    strip.setPixelColor(i, 0,0,0);
  }

  for(int i = 0; i < (int)K; i++)
  {
    strip.setPixelColor(i, 255,255,255);
    strip.show();
  }

  calc_brightness_levels();
}

/*******************************************************************/
void set_program()
{
  switch(led_program % 9)
    {
      case 0:
        Serial.println("program_R");
        U1 = false;
        U2 = false;
        U3 = false;
        program_R();
        break;
      case 1:
        Serial.println("program_G");
        U1 = false;
        U2 = false;
        U3 = false;
        program_G();
        break;
      case 2:
        Serial.println("program_B");
        U1 = false;
        U2 = false;
        U3 = false;
        program_B();
        break;
      case 3:
        Serial.println("program_RG");
        U1 = false;
        U2 = false;
        U3 = false;
        program_RG();
        break;
      case 4:
        Serial.println("program_RB");
        U1 = false;
        U2 = false;
        U3 = false;
        program_RB();
        break;
      case 5:
        Serial.println("program_GB");
        U1 = false;
        U2 = false;
        U3 = false;
        program_GB(); 
        break;
      case 6:
        Serial.println("program_U1");
        U1 = true;
        U2 = false;
        U3 = false;
        //program_U1();
        break;
      case 7:
        Serial.println("program_U2");
        U1 = false;
        U2 = true;
        U3 = false;
        //program_U2();
        break;
      case 8:
        Serial.println("program_U3");
        U1 = false;
        U2 = false;
        U3 = true;
        //program_U3();
        break;
      default:
        Serial.println("Nie mam programu o tym numerze");
        break;
    }  
}

/*******************************************************************/
/************************ Led Programs ****************************/
/*******************************************************************/
void set_color(int R, int G, int B) // main function, which sets colors on LED strip
{
  int i = 0;
  for(i = 0; i < 8; i++)
  {
    strip.setBrightness(bright_level[curr_bright_level]);
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
  //change_program = false;
  //while(true)
  //{
  for(i = 0; i < 8; i++)
  {
    if(i % 2 == 0)
    {
      strip.setBrightness(bright_level[curr_bright_level]);
      strip.setPixelColor(i, strip.Color(255, 0, 0));
      strip.show();
      check_button();
      check_encoder();
      delay(50);
      strip.setBrightness(0);
    }
    else
    {
      strip.setBrightness(bright_level[curr_bright_level]);
      strip.setPixelColor(i, strip.Color(0, 0, 255));
      strip.show();
      check_button();
      check_encoder();
      delay(50);
      strip.setBrightness(0);
    }
    check_button();
  }
  //}
  //change_program = false;
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
      strip.setBrightness(bright_level[curr_bright_level]);
      strip.setPixelColor(rand_nr, strip.Color(rand_nrR, rand_nrG, rand_nrB));
      strip.show();
      delay(10);
    }    
 
}

void program_U3() // user program No. 3
{
  
}




