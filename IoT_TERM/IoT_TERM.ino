#include "WiFiEsp.h"

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(8, 9); // RX, TX
#endif

char ssid[] = "hotspot24";            // your network SSID (name)
char pass[] = "24242424";        // your network password

int status = WL_IDLE_STATUS;     // the Wifi radio's status
int reqCount = 0;                // number of requests received

WiFiEspServer server(80);

const int pingPin = 7;
const int EchoPin = 6;
int speakerPin = 8;

void setup(){
  pinMode(pingPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  Serial.begin(115200);
  Serial1.begin(9600);
  WiFi.init(&Serial1);
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

void loop(){

  distance_cal();
  Wifi_connect();

}

void distance_cal(){
    long duration, inches, cm;
  
  // 거리 측정 pulse 생성
  //pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  
  // 핀 설정 후 거리 측정
  //pinMode(pingPin, INPUT);
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
