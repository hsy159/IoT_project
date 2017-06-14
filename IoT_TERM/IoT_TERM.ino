#include "WiFiEsp.h"

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial WifiSerial(4, 5); // RX, TX
SoftwareSerial BTSerial(2, 3);
#endif

char ssid[] = "hotspot24";            // your network SSID (name)
char pass[] = "24242424";        // your network password

int status = WL_IDLE_STATUS;     // the Wifi radio's status
int reqCount = 0;                // number of requests received

//For BT
int passwordCount = 0;
String receivedPassword = "";
String Password = "1234";

WiFiEspServer server(80);

//거리 측정 센서
const int pingPin = 4;
const int EchoPin = 6;
//부저
int speakerPin = 8;
//LED 1개
int led1 = 13;
// motor 2개
int motor1 = 5;
int motor2 = 7;
int motor2_1 = 11;
int motor2_2 = 12;

void setup(){
  pinMode(pingPin,OUTPUT);
  pinMode(EchoPin,INPUT);
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  
  Serial.begin(9600);
  WifiSerial.begin(9600);
  BTSerial.begin(9600);
}

void loop(){

  if(Serial.available()){
    Serial.println("TEXT IN");
    byte received = Serial.read();
    if(received == '1'){
      Serial.println(received);
      distance_use();
    }
    else if(received == '2'){      
      Serial.println(received);
      Wifi_setup();
      Wifi_connect();
      Serial.end();
      Serial.begin(9600);
    }    
    else if(received == '3'){
      Serial.println(received);
      BT_setup();
      BT_loop();
    }    
    else if(received == '4'){
      Serial.println(received);
      Motor_loop();
    }   
    else {
      LED_setup();
      LED_loop();
    }

  }
  
}

void check_BTinput()
{
  BTSerial.listen();
  
  if(BTSerial.available() > 0)
  {
    byte tempPwd = BTSerial.read();

    receivedPassword += tempPwd;
    passwordCount++;
    
    if(passwordCount == 4)
    {
      if(receivedPassword == "1234")
      {
        
      }
      else
      {
        
      }

      //initialization
      receivedPassword = "";
      passwordCount = 0;
    }
    
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
    Serial.println("5 이내");
    delay(50);
  }
  else if(inches < 10){ // 10인치 이내로 들어오면 경고음 발생
    tone(speakerPin, 2620, 100); // 경고음은 거리에 따라 다르게 주파수 설정
    delay(100);
  }
  else if(inches < 20){ // 20인치 이내로 들어오면 경고음 발생
    tone(speakerPin, 2620, 400); // 경고음은 거리에 따라 다르게 주파수 설정
    delay(500);
  }
  else if(inches < 30){ // 30인치 이내로 들어오면 경고음 발생
    tone(speakerPin, 2620, 800); // 경고음은 거리에 따라 다르게 주파수 설정
    delay(1000);
  }
}

long microsecondsToInches(long microseconds){
  return microseconds / 74 / 2;
} // 시간에 따라 거리 inch를 구하는 함수

void Wifi_setup(){
  
  Serial.end();
  Serial.begin(115200);

  WifiSerial.listen();
  WiFi.init(&WifiSerial);
  
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");
  printWifiStatus();
  
  // start the web server on port 80
  server.begin();
}

void Wifi_connect(){
  
    WiFiEspClient client = server.available();
   
  if (client) {
    Serial.println("New client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          Serial.println("Sending response");
          
          // send a standard http response header
          // use \r\n instead of many println statements to speedup data send
          client.print(
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"  // the connection will be closed after completion of the response
            "Refresh: 20\r\n"        // refresh the page automatically every 20 sec
            "\r\n");
          client.print("<!DOCTYPE HTML>\r\n");
          client.print("<html>\r\n");
          client.print("<h1>Hello World!</h1>\r\n");
          client.print("Requests received: ");
          client.print(++reqCount);
          client.print("<br>\r\n");
          client.print("Analog input A0: ");
          client.print(analogRead(0));
          client.print("<br>\r\n");
          client.print("</html>\r\n");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(10);

    // close the connection:
    client.stop();
    Serial.println("Client disconnected");
  }  
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  
  // print where to go in the browser
  Serial.println();
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
  Serial.println();
}

//블루투스 읽어오기용
void BT_setup(){
  BTSerial.listen();
}

void BT_loop(){
  if(Serial.available() > 0){
    BTSerial.write(Serial.read());
  }
  if(Serial.available() > 0){
    Serial.write(BTSerial.read());
  }
}

// 모터 1개 돌리기
void Motor_loop(){
    Serial.println("Motor:::::::");
    digitalWrite(motor1, HIGH);
    digitalWrite(motor2, LOW);
    digitalWrite(motor2_1, LOW);
    digitalWrite(motor2_1, HIGH);
    delay(1000);

    digitalWrite(motor1, LOW);
    digitalWrite(motor2, LOW);
    digitalWrite(motor2_1, LOW);
    digitalWrite(motor2_1, LOW);

}

void LED_setup(){
  pinMode(led1, OUTPUT);
  digitalWrite(led1, LOW);
}
void LED_loop(){
  digitalWrite(led1, HIGH);
  
}