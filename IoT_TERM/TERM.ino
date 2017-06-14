/* 
 *  사물 인터넷 설계
 *  24조 기말 프로젝트
 *  김근영 임채원 황서연
 */

#include "SoftwareSerial.h"

#define DO  262
#define RE  294
#define ME  330
#define FA  349
#define SOL 392
#define RA  440
#define SI  494

SoftwareSerial BTSerial(2, 3);

int piano[8] = {SI,RE*2,FA*2.1,RE*2,(RA+SI)/2,DO*2.1,ME*2,RA*1.05};

//For BT
int passwordCount = 0;
String receivedPassword = "";
String Password = "49505152";

//거리 측정 센서
const int pingPin = 4;
const int EchoPin = 6;

//부저
int speakerPin = 8;

// motor 2개
int motor1 = 5;
int motor2 = 7;
int motor3 = 11;
int motor4 = 12;

//START
void setup(){
  pinMode(pingPin,OUTPUT);
  pinMode(EchoPin,INPUT);
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  
  Serial.begin(9600);
  BTSerial.begin(9600);
}

void loop(){

    if (Serial.available() > 0)
    {
      BTSerial.write(Serial.read());
    }
  
    check_BTinput();
    check_Thief();
}

void check_BTinput()
{
  BTSerial.listen();
  
  if(BTSerial.available() > 0)
  {
    byte tempPwd = BTSerial.read();
    
    receivedPassword += tempPwd;
    passwordCount++;

    Serial.println(receivedPassword);
    Serial.println(Password);
    
    if(passwordCount == 4)
    {
      if(receivedPassword == Password)
      {
        Serial.println("Family Come to Home");
        ring_tone_success();
      }
      else
      {
        Serial.println("Enemy Detected");
        ring_tone_fail();
      }

      //initialization
      receivedPassword = "";
      passwordCount = 0;
    }
    
  }
}

void check_Thief()
{
  distance_use();
}

void ring_tone_success()
{
  tone(speakerPin, DO * 1.5, 1000);
  delay(500);
  tone(speakerPin, ME * 1.5, 1000);
  delay(500);
  tone(speakerPin, SOL * 1.5, 1000);
  delay(500);
}

void ring_tone_fail()
{
  for(int i=0;i<8;i++){
      tone(speakerPin, piano[i] * 4,500);
      delay(500);
    }
}

void distance_use(){
  long duration, inches, cm;
  
  // 거리 측정 pulse 생성
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  
  // 핀 설정 후 거리 측정
  duration = pulseIn(EchoPin, HIGH);
  inches = microsecondsToInches(duration);
    
  if(inches < 5){ // 5인치 이내로 들어오면 경고음 발생
    tone(speakerPin, 2620, 100); // 경고음은 거리에 따라 다르게 주파수 설정
    Serial.print(inches);
    Serial.println(" inches away");
    trap_two();
    delay(50);
  }
  else if(inches < 10){ // 10인치 이내로 들어오면 경고음 발생
    tone(speakerPin, 2620, 100); // 경고음은 거리에 따라 다르게 주파수 설정
    Serial.print(inches);
    Serial.println(" inches away");
    trap_one();
    delay(100);
  }
}

void trap_one()
{
  digitalWrite(motor3, LOW);
  digitalWrite(motor4, LOW);

  digitalWrite(motor3, HIGH);
  digitalWrite(motor4, LOW);
  delay(500);
  digitalWrite(motor3, LOW);
  digitalWrite(motor4, LOW);

  Serial.println("There is a thief at the Window");
}

void trap_two()
{
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, LOW);

  digitalWrite(motor1, HIGH);
  digitalWrite(motor2, LOW);
  delay(500);
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, LOW);

  Serial.println("I will break your H. E. A. D");
}

long microsecondsToInches(long microseconds){
  return microseconds / 74 / 2;
} // 시간에 따라 거리 inch를 구하는 함수
