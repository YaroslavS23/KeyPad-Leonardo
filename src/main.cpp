#include <Arduino.h>
#include <U8g2lib.h>
U8G2_SH1106_128X32_VISIONOX_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


int pinA = 1;
int pinB = 0;
int pinButton = 4;
volatile int previous = 0;
volatile int counter = 0;
volatile bool one_clik_last,butt_one_clik = false;
volatile bool forv_one_clik,rev_one_clik = false;
volatile bool butt_forv_one_clik,butt_rev_one_clik = false;
int step_case = 0;

#include "HID-Project.h"
#define MEDIA_FAST_FORWARD	0xB3
#define MEDIA_REWIND	      0xB4
#define MEDIA_NEXT	        0xB5
#define MEDIA_PREVIOUS	    0xB6
#define MEDIA_STOP	        0xB7
#define MEDIA_PLAY_PAUSE	  0xCD
//#define MEDIA_VOLUME_MUTE	  0xE2
//#define MEDIA_VOLUME_UP	    0xE9
//#define MEDIA_VOLUME_DOWN	  0xEA

void clik (){

  butt_one_clik = false;
  forv_one_clik = false;
  rev_one_clik = false;
  butt_forv_one_clik = false;
  butt_rev_one_clik = false;
  //нажатие кнопки
  if (!digitalRead(pinButton) && !one_clik_last){butt_one_clik = true;}
  if (!digitalRead(pinButton)){
    one_clik_last = true;}
  else{
    one_clik_last = false;}
  //если кнопка не нажата
  if (counter >= 3 && digitalRead(pinButton)){
    forv_one_clik = true;
    counter = 0;
  }
  if (counter <= -3 && digitalRead(pinButton)){
    rev_one_clik = true;
    counter = 0;
  }
  //если кнопка нажата
  if (counter >= 3 && !digitalRead(pinButton)){
    butt_forv_one_clik = true;
    counter = 0;
  }
  if (counter <= -3 && !digitalRead(pinButton)){
    butt_rev_one_clik = true;
    counter = 0;
  }

}

void changed() {
  int A = digitalRead(pinA); 
  int B = digitalRead(pinB);

  int current = (A << 1) | B;
  int combined  = (previous << 2) | current;
   
  if(combined == 0b0010 || 
     combined == 0b1011 ||
     combined == 0b1101 || 
     combined == 0b0100
     ) {
    counter++;
  }  
  if(combined == 0b0001 ||
     combined == 0b0111 ||
     combined == 0b1110 ||
     combined == 0b1000
     ) {
    counter--;
  }
  previous = current;
}

void setup() {
  pinMode(pinA, INPUT_PULLUP); 
  pinMode(pinB, INPUT_PULLUP);
  pinMode(pinButton, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinA), changed, CHANGE); 
  attachInterrupt(digitalPinToInterrupt(pinB), changed, CHANGE);

  u8g2.begin();
  u8g2.enableUTF8Print(); 

  Keyboard.begin();
 
}//end setup

void loop() {
  String text_case;
  clik ();

  if (forv_one_clik == true) {step_case++;}
  if (rev_one_clik == true ) {step_case--;}

  switch (step_case){
    case -1:
      step_case = 5;
    break;   
    case 0:
      text_case = "CtrL+C";
      if (butt_one_clik){
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press('c');   
        delay(10);
        Keyboard.releaseAll();    
        }
    break;
    case 1:
      text_case = "CtrL+V";
      if (butt_one_clik){
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press('v');   
        delay(10);
        Keyboard.releaseAll();    
        }
    break;
    case 2:
      text_case = "CtrL+Z";
      if (butt_one_clik){
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press('z');   
        delay(10);
        Keyboard.releaseAll();    
        }
    break;
    case 3:
      text_case = "Deg";
      if (butt_one_clik){
       Keyboard.press(KEY_LEFT_ALT);
       Keyboard.press(KEYPAD_2);      
       Keyboard.press(KEYPAD_4);       
       Keyboard.press(KEYPAD_8);    
        delay(10);
        Keyboard.releaseAll();  
        }    
    break;
    case 4:
      text_case = "Mute";
      if (butt_one_clik){   
        Consumer.write(MEDIA_VOLUME_MUTE);   
        delay(10);
        Keyboard.releaseAll(); 
      }         
    break;
    case 5:
      text_case = "Volume";
      if (butt_forv_one_clik){
       Consumer.write(MEDIA_VOLUME_UP);  
        delay(5);
        Keyboard.releaseAll();  
        }
      if (butt_rev_one_clik){
       Consumer.write(MEDIA_VOLUME_DOWN);  
        delay(5);
        Keyboard.releaseAll();  
        } 
    break;
    case 6:
      step_case = 0;
    break;
    default:
      step_case = 0;
  }//end swith

  u8g2.clearBuffer();					        // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB18_tr);	// choose a suitable font
  u8g2.setCursor(15, 25);
  u8g2.print(text_case);
  u8g2.sendBuffer();
  delay(10); 


}//end loop
