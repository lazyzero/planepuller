#define YAW_INPUT 2 //channel1
#define PITCH_INPUT 3 //channel2
#define KILL_INPUT 4 //channel3
#define WINCH_INPUT 5 //channel4
#define LEFT_PWM_OUTPUT 10
#define RIGHT_PWM_OUTPUT 11
#define LEFT_REVERSE_OUTPUT 8
#define RIGHT_REVERSE_OUTPUT 9
#define WINCH_LEFT_OUTPUT 6
#define WINCH_RIGHT_OUTPUT 7
#define ESC_ACTIVATION_OUTPUT 13

#define FORWARD HIGH
#define BACKWARD LOW
#define CENTER1 1520
#define CENTER2 1510

int leftPWM = 0;
int rightPWM = 0;
boolean leftDirection = FORWARD;
boolean rightDirection = FORWARD;
boolean enableWinch = false;

int pwm_value_1 = 0; //channel1
int pwm_value_2 = 0; //channel2
int pwm_value_3 = 0; //channel3
int pwm_value_4 = 0; //channel4

void setup() {
  pinMode(YAW_INPUT, INPUT);
  pinMode(PITCH_INPUT, INPUT);
  pinMode(KILL_INPUT, INPUT);
  pinMode(WINCH_INPUT, INPUT);
  
  pinMode(LEFT_PWM_OUTPUT, OUTPUT);
  pinMode(RIGHT_PWM_OUTPUT, OUTPUT);
  pinMode(LEFT_REVERSE_OUTPUT, OUTPUT);
  pinMode(RIGHT_REVERSE_OUTPUT, OUTPUT);
  pinMode(WINCH_LEFT_OUTPUT, OUTPUT);
  pinMode(WINCH_RIGHT_OUTPUT, OUTPUT);
  pinMode(ESC_ACTIVATION_OUTPUT, OUTPUT);

  digitalWrite(WINCH_LEFT_OUTPUT, HIGH);
  digitalWrite(WINCH_RIGHT_OUTPUT, HIGH);

  analogWrite(LEFT_PWM_OUTPUT, leftPWM);
  analogWrite(RIGHT_PWM_OUTPUT, rightPWM);

  digitalWrite(LEFT_REVERSE_OUTPUT, leftDirection);
  digitalWrite(RIGHT_REVERSE_OUTPUT, rightDirection);

  Serial.begin(115200);
  Serial.println("Init done.");
}

void loop() {
  pwm_value_1 = pulseIn(YAW_INPUT, HIGH);
  pwm_value_2 = pulseIn(PITCH_INPUT, HIGH);
  pwm_value_3 = pulseIn(KILL_INPUT, HIGH);
  pwm_value_4 = pulseIn(WINCH_INPUT, HIGH);

  if (pwm_value_1 == 0 || pwm_value_2 == 0 || pwm_value_3 == 0 || pwm_value_4 == 0 ) {
    digitalWrite(WINCH_LEFT_OUTPUT, HIGH);
    digitalWrite(WINCH_RIGHT_OUTPUT, HIGH);
    digitalWrite(ESC_ACTIVATION_OUTPUT, LOW);
    leftPWM = 0;
    rightPWM = 0;
  } else {

  //mixing the channels
  long value_1 = pwm_value_1-CENTER1;
  long value_2 = pwm_value_2-CENTER2;

  //curve manipulation depending stick position
  /*if (value_1 > -200 && value_1 < 200) {
    leftPWM = (2*value_1+value_2/2);
    rightPWM = (2*value_1-value_2/2);
  } else {*/
    leftPWM = (value_1+value_2);
    rightPWM = (value_1-value_2);
  //}
  
  leftPWM = map(leftPWM, -500, 500, -255, 255);
  rightPWM = map(rightPWM, -500, 500, -255, 255);

  //smoothing the signal around zero
  if (leftPWM > -10 && leftPWM < 10) leftPWM = 0;
  if (rightPWM > -10 && rightPWM < 10) rightPWM = 0;

  //switch forward backward
  if (leftPWM < 0) {
    leftDirection = BACKWARD;
    leftPWM *= -1;
  } else {
    leftDirection = FORWARD;
  }
  if (rightPWM < 0) {
    rightDirection = BACKWARD;
    rightPWM *= -1;
  } else {
    rightDirection = FORWARD;
  }

  leftPWM = constrain(leftPWM, 0, 220);
  rightPWM = constrain(rightPWM, 0, 220);

  //killswitch
  if (pwm_value_3>1700) { //Switch all off
     leftPWM = 0;
     rightPWM = 0;
     digitalWrite(ESC_ACTIVATION_OUTPUT, LOW);
     enableWinch = false;
  } else if (pwm_value_3<1700 && pwm_value_3>1400) {
    digitalWrite(ESC_ACTIVATION_OUTPUT, LOW);
    enableWinch = true;
  } else {
     digitalWrite(ESC_ACTIVATION_OUTPUT, HIGH);
     enableWinch = false;
  }

  if (enableWinch) {
     if (pwm_value_4 > 1700) {
       digitalWrite(WINCH_LEFT_OUTPUT, HIGH);
       digitalWrite(WINCH_RIGHT_OUTPUT, LOW);
     } else if (pwm_value_4 < 1300) { 
       digitalWrite(WINCH_RIGHT_OUTPUT, HIGH);
       digitalWrite(WINCH_LEFT_OUTPUT, LOW);
     } else {
        digitalWrite(WINCH_LEFT_OUTPUT, HIGH);
        digitalWrite(WINCH_RIGHT_OUTPUT, HIGH);
     }
  }
  }
  
  digitalWrite(LEFT_REVERSE_OUTPUT, leftDirection);
  digitalWrite(RIGHT_REVERSE_OUTPUT, rightDirection);
  
  analogWrite(LEFT_PWM_OUTPUT, leftPWM);
  analogWrite(RIGHT_PWM_OUTPUT, rightPWM);
  
  Serial.print(pwm_value_1);
  Serial.print("\t");
  Serial.print(pwm_value_2);
  Serial.print("\t");
  Serial.print(pwm_value_3);
  Serial.print("\t");
  Serial.print(pwm_value_4);
  Serial.print("\t");
  Serial.print(leftDirection);
  Serial.print("\t"); 
  Serial.print(leftPWM);
  Serial.print("\t");
  Serial.print(rightDirection);
  Serial.print("\t");
  Serial.println(rightPWM);
}


