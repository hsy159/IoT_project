/* 
 *  사물 인터넷 설계
 *  24조 기말 프로젝트
 *  김근영 임채원 황서연
 */

#include "SoftwareSerial.h"
// 음계
#define DO  262
#define RE  294
#define ME  330
#define FA  349
#define SOL 392
#define RA  440
#define SI  494

SoftwareSerial BTSerial(2, 3);

int piano[8] = {SI,RE*2,FA*2.1,RE*2,(RA+SI)/2,DO*2.1,ME*2,RA*1.05}; // 비밀번호 틀렸을 시

//For BT
int passwordCount = 0;
String receivedPassword = ""; // 집 비밀번호 입력시 받을 string
String Password = "1234"; // 집 비밀번호 

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
// 시리얼 모니터의 입력에 따라 시작
    if (Serial.available() > 0)
    {
      BTSerial.write(Serial.read());
    }
  
    check_BTinput();
    check_Thief();
}

void check_BTinput()
{
  BTSerial.listen(); // 집 비밀번호 입력하는 것 체크
  
  if(BTSerial.available() > 0)
  {
    char tempPwd = BTSerial.read();
    
    receivedPassword += tempPwd;
    passwordCount++;

    Serial.println(receivedPassword);
    Serial.println(Password);
    
    if(passwordCount == 4)
    {
      if(receivedPassword == Password) // 입력받은 4자리 비밀번호 일치
      {
        Serial.println("Family Come to Home");
        ring_tone_success();
      }
      else // 입력받은 4자리 비밀번호 불일치
      {
        Serial.println("Enemy Detected"); // 도둑으로 간주
        ring_tone_fail();
      }

      //initialization
      receivedPassword = "";
      passwordCount = 0;
    }
    
  }
}

//가족 경쾌한 음악
void ring_tone_success()
{
  tone(speakerPin, DO * 1.5, 1000);
  delay(500);
  tone(speakerPin, ME * 1.5, 1000);
  delay(500);
  tone(speakerPin, SOL * 1.5, 1000);
  delay(500);
}

//도둑 침입 경고음
void ring_tone_fail()
{
  for(int i=0;i<8;i++){
      tone(speakerPin, piano[i] * 4,500);
      delay(500);
    }
}
// 창문으로 오는 도둑 체크
void check_Thief(){
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
//모터를 이용한 발걸기 함정
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
//모터 이용한 벽돌, 압정 함정
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