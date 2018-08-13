/* 
   Giant Pong
   Vector Space
   8/2018
*/

#include <EnableInterrupt.h>
#include <Servo.h>

enum { PinA=9, PinB=10, PinC=2, PinD=3, IPINMODE=INPUT };

static  byte abOld;     // Initialize state
static  byte cdOld;     // Initialize state

volatile int P1_ypos;     // current rotary P1_ypos
         int old_P1_ypos;     // old rotary P1_ypos
volatile int P2_ypos;     // current rotary P1_ypos
         int old_P2_ypos;     // old rotary P1_ypos
volatile int P1_xpos;
volatile int P2_xpos;

Servo P1_yservo;  
Servo P1_xservo;  
Servo P2_yservo;  
Servo P2_xservo;
Servo ball_yservo;  
Servo ball_xservo;

int display_width = 0;
int display_height = 0;

const int P1_button_pin = 12;  
const int P2_button_pin = 13; 
int P1_button_state = 0;       
int P2_button_state = 0;    

void setup() {
  pinMode(PinA, IPINMODE);
  pinMode(PinB, IPINMODE);
  pinMode(PinC, IPINMODE);
  pinMode(PinD, IPINMODE);

  enableInterrupt(PinA, pinChangeISR, CHANGE); // Set up pin-change interrupts
  enableInterrupt(PinB, pinChangeISR, CHANGE);
  enableInterrupt(PinC, pinChangeISR, CHANGE); // Set up pin-change interrupts
  enableInterrupt(PinD, pinChangeISR, CHANGE);
  
  abOld = P1_ypos = old_P1_ypos = cdOld = P2_ypos = old_P2_ypos = 0;

   P1_yservo.attach(6); 
   P1_xservo.attach(7);  
   P2_yservo.attach(8); 
   //P2_xservo.attach(9); 
   //ball_yservo.attach(10); 
   //ball_xservo.attach(11); 

   pinMode(P1_button_pin, INPUT);
   pinMode(P2_button_pin, INPUT);
   
  Serial.begin(115200);
  Serial.println("Starting Rotary Encoder Test");
}

// On interrupt, read input pins, compute new state, and adjust P1_ypos
void pinChangeISR() {
  enum { upMask = 0x66, downMask = 0x99 };
  byte abNew = (digitalRead(PinA) << 1) | digitalRead(PinB);
  byte cdNew = (digitalRead(PinC) << 1) | digitalRead(PinD);

  byte criterion = abNew^abOld;
  byte criterion2 = cdNew^cdOld;

  
  if (criterion==1 || criterion==2) {
    if (upMask & (1 << (2*abOld + abNew/2)))
      P1_ypos++;
    else P1_ypos--;       // upMask = ~downMask
  }
  abOld = abNew;        // Save new state


  if (criterion2==1 || criterion2==2) {
    if (upMask & (1 << (2*cdOld + cdNew/2)))
      P2_ypos++;
    else P2_ypos--;       // upMask = ~downMask
  }
  cdOld = cdNew;        // Save new state
}



void loop() {
  P1_yservo.write(P1_ypos);  
  P2_yservo.write(P2_ypos); 
  P1_button_state = digitalRead(P1_button_pin);
  P2_button_state = digitalRead(P2_button_pin);

  // Check for simultaneous button press for 5 sec
  bool buttonsPressed = true;
  if (P1_button_state == HIGH && P2_button_state == HIGH){
    for(int i=0; i<10; i++){
      if (P1_button_state == LOW || P2_button_state == LOW){
        buttonsPressed = false;
      }
      delay(500);
    }
    if (buttonsPressed == true){
      Calibrate();
    }
  }

  if (old_P1_ypos != P1_ypos) {
    Serial.print(millis());
    Serial.print("  ");
    Serial.print(P1_ypos);
    Serial.print("  ");
    Serial.println(P2_ypos);
    if (P1_ypos < 0){
      P1_ypos = 0;
    }
    else if (P1_ypos > 180){
      P1_ypos = 180;
    }
        old_P1_ypos = P1_ypos;

  }
  if (old_P2_ypos != P2_ypos) {
    Serial.print(millis());
    Serial.print("  ");
    Serial.print(P1_ypos);
    Serial.print("  ");
    Serial.println(P2_ypos);
    if (P2_ypos < 0){
      P2_ypos = 0;
    }
    else if (P2_ypos > 180){
      P2_ypos = 180;
    }
        old_P2_ypos = P2_ypos;
  }
}


void DrawPaddle(){
  /*
   * Draw a rectangle with the servo
   */
  for (int pos = 0; pos <= 10; pos += 2) { 
    P1_yservo.write(P1_ypos + pos);           
  }
  for (int pos = 0; pos <= 4; pos += 2) { 
    P1_xservo.write(P1_xpos + pos);      
  }
  for (int pos = 0; pos <= 10; pos += 2) { 
    P1_yservo.write(P1_ypos - pos);             
  }
  for (int pos = 0; pos <= 4; pos += 2) { 
    P1_yservo.write(P1_xpos - pos);              
  }
}

void UpdateScore(){
  
}

void Calibrate(){
  /*
   * Read the rotary encoder position and use those values to modify the display_width and display_height values.
   */
   do while (stop == false){
       display_width = P1_ypos;
       display_height = P2_ypos;
   }
   
}
