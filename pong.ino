/* 
   Giant Pong
   Vector Space
   8/2018

   D2, P1 dial
   D3, P1 dial
   D9, P2 dial
   D10, P2 dial
   D12, P1 button
   D13, P2 button



*/

#include <EnableInterrupt.h>
#include <Servo.h>

enum { PinA=7, PinB=8, PinC=2, PinD=4, IPINMODE=INPUT };

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

int display_width = 10;
int display_height = 10;
int xmin;
int xmax;
int ymin;
int ymax;

int P1_score;
int P2_score;

int P1_paddle_bottom;
int P1_paddle_top;
int P2_paddle_bottom;
int P2_paddle_top;

int ball_x;
int ball_y;
float ball_dir;
int ball_speed = 0;

unsigned long counter =0;

const int paddle_height = 10;
const int paddle_width = 5;

const int statusLED = 13;

const int P1_button_pin = A0;  
const int P2_button_pin = A1;
int P1_button_state = 0;       
int P2_button_state = 0;

bool calibrated = false;
bool start_game = false;

int game_speed = 100; // higher is slower


void setup() {
  pinMode(PinA, IPINMODE);
  pinMode(PinB, IPINMODE);
  pinMode(PinC, IPINMODE);
  pinMode(PinD, IPINMODE);

  pinMode(statusLED, OUTPUT);
  
  abOld = P1_ypos = old_P1_ypos = cdOld = P2_ypos = old_P2_ypos = 0;

  // Servos attach to PWM pins
  P1_xservo.attach(3); 
  P1_yservo.attach(5);  
  P2_yservo.attach(9); 
  P2_xservo.attach(6); 
  ball_yservo.attach(11); 
  ball_xservo.attach(10); 

  pinMode(P1_button_pin, INPUT);
  pinMode(P2_button_pin, INPUT);

  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);

   
  Serial.begin(115200);
  Serial.println("Starting Pong");
}



void loop() {
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  digitalWrite(A4, HIGH);
  //drawCircle();
  drawOctogon();
  //drawRectangle();
  drawLine();
  
  //testRange();
  
}
void center(){
  int minpos = 1000;
  int maxpos = 2000;
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);
  P1_xservo.writeMicroseconds((minpos+maxpos) / 2);
  P1_yservo.writeMicroseconds((minpos+maxpos) / 2);
  P2_xservo.writeMicroseconds((minpos+maxpos) / 2);
  P2_yservo.writeMicroseconds((minpos+maxpos) / 2);
  ball_xservo.writeMicroseconds((minpos+maxpos) / 2);
  ball_yservo.writeMicroseconds((minpos+maxpos) / 2);
  delay(1000);
}
void testRange(){
  int minpos = 1000;
  int maxpos = 2000;
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);
  P1_xservo.writeMicroseconds(minpos);
  P1_yservo.writeMicroseconds(minpos);
  P2_xservo.writeMicroseconds(minpos);
  P2_yservo.writeMicroseconds(maxpos);
  ball_xservo.writeMicroseconds(minpos);
  ball_yservo.writeMicroseconds(minpos);
  delay(1000);
  P1_xservo.writeMicroseconds(maxpos);
  P1_yservo.writeMicroseconds(maxpos);
  P2_xservo.writeMicroseconds(maxpos);
  P2_yservo.writeMicroseconds(minpos);
  ball_xservo.writeMicroseconds(maxpos);
  ball_yservo.writeMicroseconds(maxpos);
  delay(1000);
}

void drawOctogon(){
  int gameWidth = 800;
  int gameHeight = 600;
  unsigned int radius = gameWidth * 0.05;

  int x = 1500;
  int y = 1300;
  int thedelay = 30;

  ball_xservo.writeMicroseconds(x);
  ball_yservo.writeMicroseconds(y);
  delay(2000);

    for(int n=0;n<5;n++){
      ball_yservo.writeMicroseconds(y+10);
      delay(thedelay);
      ball_xservo.writeMicroseconds(x+10);
      ball_yservo.writeMicroseconds(y+20);
      delay(thedelay);
      ball_xservo.writeMicroseconds(x+20);
      delay(thedelay);
      ball_xservo.writeMicroseconds(x+30);
      ball_yservo.writeMicroseconds(y+10);
      delay(thedelay);
      ball_yservo.writeMicroseconds(y);
      delay(thedelay);
      ball_xservo.writeMicroseconds(x+20);
      ball_yservo.writeMicroseconds(y-10);
      delay(thedelay);
      ball_xservo.writeMicroseconds(x+10);
      delay(thedelay);
      ball_xservo.writeMicroseconds(x);
      ball_yservo.writeMicroseconds(y);
      delay(thedelay);
  }
}




void drawCircle(){
  int gameWidth = 800;
  int gameHeight = 600;
  unsigned int radius = gameWidth * 0.05;

  int x = 1500;
  int y = 1500;
  int thedelay;
  int j;

  ball_xservo.writeMicroseconds(x);
  ball_yservo.writeMicroseconds(y);
  delay(2000);

    for(int n=0;n<10;n++){

       for(int i = x - radius; i < (x + radius); i++)
       {
          j = y + sqrt((radius * radius) - ((i - x) * (i - x)));
          ball_xservo.writeMicroseconds(i);
          ball_yservo.writeMicroseconds(j);
          delay(3);
       }
       
       for(int i = x + radius; i > x - radius; i--)
       {
          j = y - sqrt((radius * radius) - ((i - x) * (i - x)));
          ball_xservo.writeMicroseconds(i);
          ball_yservo.writeMicroseconds(j);
          delay(3);

       }  
    }
}


// draw a rectangle using vertical zigzags
void drawLine(){
  int gameWidth = 800;
  int gameHeight = 600;
  int height=gameHeight * 0.15;
  int width= height * 0.21;
  int miny = 1100;
  int minx = 1000;
  int maxy=miny+height;
  int maxx=minx+width;
  int thedelay;

  P1_xservo.writeMicroseconds(minx);
  P1_yservo.writeMicroseconds(miny);
  delay(2000);
  byte dir = 0;
  for(int n=0;n<3;n++){
    for(int i=1; i<=width; i++){
      if(dir == 0){
        for(int j=0; j<height; j++){
           P1_yservo.writeMicroseconds(miny+j);
           thedelay = max(2000-30*(height-j), 200);
            Serial.print(j);
            Serial.print(", ");
            Serial.println(thedelay);
            delayMicroseconds(thedelay);
        }
        dir = 1;
      }
      else{
        for(int j=0; j<height; j++){
           P1_yservo.writeMicroseconds(maxy-j);
           thedelay = max(2000-30*(height-j), 200);
           delayMicroseconds(thedelay);
        }
        dir = 0;
      }
    }
  }
}


void drawRectangle(){
  int gameWidth = 800;
  int gameHeight = 600;
  int height=gameHeight * 0.15;
  int width= height * 0.21;
  int miny = 1500;
  int minx = 1500;
  int maxy=miny+height;
  int maxx=minx+width;
  int thedelay;

  ball_xservo.writeMicroseconds(minx);
  ball_yservo.writeMicroseconds(miny);
  delay(1000);
  for(int j=0;j<5;j++){
    for(int i=1; i<=width; i++){
      ball_xservo.writeMicroseconds(minx+i);
      thedelay = max(1000-40*(width-i), 200);
      Serial.print(i);
      Serial.print(", ");
      Serial.println(thedelay);
      delayMicroseconds(thedelay);
    }
    for(int i=1; i<=height; i++){
      ball_yservo.writeMicroseconds(miny+i);
      thedelay = max(1000-40*(width-i), 200);
      delayMicroseconds(thedelay);
    }
    for(int i=1; i<=width; i++){
      ball_xservo.writeMicroseconds(maxx-i);
      thedelay = max(1000-40*(width-i), 200);
      delayMicroseconds(thedelay);
    }
    for(int i=1; i<=height; i++){
      ball_yservo.writeMicroseconds(maxy-i);
      thedelay = max(1000-40*(width-i), 200);
      delayMicroseconds(thedelay);
    }
  }
}



