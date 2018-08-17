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

  enableInterrupt(PinA, pinChangeISR, CHANGE); // Set up pin-change interrupts
  enableInterrupt(PinB, pinChangeISR, CHANGE);
  enableInterrupt(PinC, pinChangeISR, CHANGE); // Set up pin-change interrupts
  enableInterrupt(PinD, pinChangeISR, CHANGE);
  
  abOld = P1_ypos = old_P1_ypos = cdOld = P2_ypos = old_P2_ypos = 0;

  // Servos attach to PWM pins
  P1_yservo.attach(3); 
  P1_xservo.attach(5);  
  P2_yservo.attach(6); 
  P2_xservo.attach(9); 
  ball_yservo.attach(11); 
  ball_xservo.attach(10); 

  pinMode(P1_button_pin, INPUT);
  pinMode(P2_button_pin, INPUT);
   
  Serial.begin(115200);
  Serial.println("Starting Pong");
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
  /*
    Calibrate
    Start game
    move ball
    check for collision with walls or paddles
    show score
   */
  counter += 1;


  

  P1_button_state = digitalRead(P1_button_pin);
  P2_button_state = digitalRead(P2_button_pin);
  // Check for simultaneous button press for 5 sec
  bool buttonsPressed = true;
  if (P1_button_state == 0 && P2_button_state == 0){
    for(int i=0; i<10; i++){
      if (digitalRead(P1_button_pin) == 1 || digitalRead(P2_button_pin) == 1){
        buttonsPressed = false;
        break;
      }
      delay(250);
    }
    if (buttonsPressed == true){
      //calibrate();
      drawRectangle();
    }
  }

  if (start_game){
  P1_yservo.write(P1_ypos);  
  P2_yservo.write(P2_ypos);
  P1_xservo.write(0);
  P2_xservo.write(display_width);

  // Update paddle positions
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


  drawPaddle();
  
  // move the ball every so many cycles
  // this prevents other parts of the code from being slowed down in order to slow the ball
  if (counter % game_speed == 0){
    moveBall();
  }
  
  checkCollision();
  }


  // DEBUGGING
  Serial.print("ball x,y: ");
  Serial.print(ball_x);
  Serial.print(", ");
  Serial.println(ball_y);
  Serial.print("P1 ymin,ymax: ");
  Serial.print(P1_paddle_bottom);
  Serial.print(", ");
  Serial.println(P1_paddle_top);
  Serial.print("P2 ymin,ymax: ");
  Serial.print(P2_paddle_bottom);
  Serial.print(", ");
  Serial.println(P2_paddle_top);
  Serial.print("Score: ");
  Serial.print(P1_score);
  Serial.print(" : ");
  Serial.println(P2_score);

  
  /* delay(200); */
}


void drawPaddle(){
  /*
   * Draw a rectangle with the servo
   */
  P1_paddle_bottom = P1_ypos - (paddle_height / 2);
  P1_paddle_top = P1_ypos + (paddle_height / 2);
  P2_paddle_bottom = P2_ypos - (paddle_height / 2);
  P2_paddle_top = P2_ypos + (paddle_height / 2);

  // draw p1 paddle
  for (int pos = 0; pos <= paddle_height; pos += 1) { 
    P1_yservo.write(P1_ypos + pos);           
  }
  for (int pos = 0; pos <= paddle_width; pos += 1) { 
    P1_xservo.write(P1_xpos + pos);      
  }
  for (int pos = 0; pos <= paddle_height; pos += 1) { 
    P1_yservo.write(P1_ypos + paddle_height - pos);             
  }
  for (int pos = 0; pos <= paddle_width; pos += 1) { 
    P1_xservo.write(P1_xpos + paddle_width - pos);              
  }

  // draw p2 paddle
  for (int pos = 0; pos <= paddle_height; pos += 1) { 
    P2_yservo.write(P2_ypos + pos);           
  }
  for (int pos = 0; pos <= paddle_width; pos += 1) { 
    P2_xservo.write(P2_xpos + pos);      
  }
  for (int pos = 0; pos <= paddle_height; pos += 1) { 
    P2_yservo.write(P2_ypos - pos);             
  }
  for (int pos = 0; pos <= paddle_width; pos += 1) { 
    P2_xservo.write(P2_xpos - pos);              
  }
}


void moveBall(){
  /*
    Set the ball's new position, speed, and direction
   */
  Serial.println(ball_dir);
  Serial.println(ball_speed);
  Serial.print("ystep = ");
  Serial.println(ball_speed * sin(ball_dir * 1000 / 57296));
  Serial.print("xstep = ");
  Serial.println(ball_speed * cos(ball_dir * 1000 / 57296));
  Serial.print("old x,y = ");
  Serial.print(ball_x);
  Serial.print(", ");
  Serial.println(ball_y);
  ball_y = ball_y + round(ball_speed * sin(ball_dir * 1000 / 57296)); // deg to rad
  ball_x = ball_x + round(ball_speed * cos(ball_dir * 1000 / 57296));
  Serial.print("new x,y = ");
  Serial.print(ball_x);
  Serial.print(", ");
  Serial.println(ball_y);
  ball_yservo.write(ball_y);  
  ball_xservo.write(ball_x);
}

void checkCollision(){
  
  
  // Bounce off top and bottom
  if (ball_y >= ymax || ball_y <= ymin){
    ball_dir = 360 - ball_dir;
    Serial.println("BOUNCE top/bottom!");
    return;
  }

  // Bounce off P1 paddle
  if (ball_x == P1_xpos){
    if (ball_y >= P1_paddle_bottom && ball_y <= P1_paddle_top){
      ball_dir = 360 - ball_dir;
      Serial.println("BOUNCE off P1!");
      return;
    }
  }
  // Bounce off P2 paddle
  if (ball_x == P2_xpos){
    if (ball_y >= P2_paddle_bottom && ball_y <= P2_paddle_top){
      ball_dir = 360 - ball_dir;
      Serial.println("BOUNCE off P2!");
      return;
    }
  }
  
  // Scoring
  if (ball_x < P1_xpos){
    P2_score++;
    Serial.println("P2 SCORE!");
    resetBoard();
  }
  else if (ball_x > P2_xpos){
    P1_score++;
    Serial.println("P1 SCORE!");
    resetBoard();
  }

  if (P1_score == 3 || P2_score == 3){
    gameOver();
  }
}


void drawScore(){
  /*
    Use P1 laser to draw P1's score
    Use P2 laser to draw P2's score
   */
}


void gameOver(){
  Serial.println("GAME OVER");
  drawScore();
  delay(4000);
  start();
}

void resetBoard(){
  drawScore();
  ball_x = display_width / 2;
  ball_y = display_height / 2;
  P1_ypos = display_height / 2;
  P2_ypos = display_height / 2;
  delay(3000);
}


/* void UpdateScore(){ */
/* } */

void drawRectangle(){
  int miny = 550;
  int minx = 600;
  int maxy=700;
  int maxx=650;
  for(int j=0;j<50;j++){
    for(int i=1;i<=maxx;i++){
      ball_xservo.writeMicroseconds(i);
      delayMicroseconds(5);
    }
    delay(90);
    for(int i=1;i<=maxy;i++){
      ball_yservo.writeMicroseconds(i);
      delayMicroseconds(5);
    }
    delay(90);
    for(int i=maxx-1; i>=0 ; i--){
      ball_xservo.writeMicroseconds(i);
      delayMicroseconds(5);
    }
    delay(90);
    for(int i=maxy-1;i>=0;i--){
      ball_yservo.writeMicroseconds(i);
      delayMicroseconds(5);
    }
    delay(90);
  }
}


void calibrate(){
  /*
   * Read the rotary encoder position and use those values to modify the display_width and display_height values.
   */
  digitalWrite(statusLED, HIGH);
  Serial.println("Entering calibration mode");
  delay(1500);
  bool stop = false;
  int xcount = 0;
  int xchange = 1;
  int ycount = 0;
  int ychange = 1;
  while (stop == false){
    Serial.print("Width, height: ");
    Serial.print(display_width);
    Serial.print(", ");
    Serial.println(display_height);

    Serial.print(xcount);
    Serial.print(", ");
    Serial.println(ycount);
    
    display_width = P1_ypos;
    display_height = P2_ypos;

    // Sweep height and width back and forth
    // 90 is center
    P1_xservo.write(xcount);
    P2_yservo.write(ycount);
    if (xcount > display_width || xcount < 0){
      xchange = xchange * -1;
    }
    xcount += xchange;
    if (ycount > display_height || ycount < 0){
      ychange = ychange * -1;
    }
    ycount += ychange;

    if (xchange == 1){
      digitalWrite(statusLED, HIGH);
    }
    else{
      digitalWrite(statusLED, LOW);  
    }

    delay(2);
 
      if (digitalRead(P1_button_pin) == 0){
        Serial.println("CALIBRATION DONE");
        digitalWrite(statusLED, LOW);
        delay(1000);
        stop = true;
        
      }

      //move ball around the bounding area for testing
      for(int i=0; i<display_width; i++){
        ball_xservo.write(i);
      }
      for(int j=0; j<display_width; j++){
        ball_yservo.write(j);
      }
    }

  // position the ball
  ball_x = display_width / 2;
  ball_y = display_height / 2;

  // position paddles
  P1_xpos = 0;
  P2_xpos = display_width;
  P1_ypos = display_height / 2;
  P2_ypos = display_height / 2;
  ymin = 0;
  ymax = display_height;

  calibrated = true;
  
  start();
}

void start(){
  Serial.println("Starting Game");
  delay(500);
  P1_score = 0;
  P2_score = 0;
  ball_speed = 2;
  ball_dir = random(0,360);
  digitalWrite(statusLED, 0);
  start_game = true;
  while (digitalRead(A0) == 1){
    delay(40);
  }
  digitalWrite(statusLED, 1);
}
