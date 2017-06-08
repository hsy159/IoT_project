/* 
 *  사물 인터넷 설계
 *  24조 기말 프로젝트
 *  김근영 임채원 황서연
 */

 
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

#include <dht11.h>
dht11 DHT11;
#define DHT11PIN 2

bool value = HIGH;
void setup()
{
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
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


void loop()
{
  // listen for incoming clients
  WiFiEspClient client = server.available();
  
  if (client) {

    Serial.println("New client");

    boolean currentLineIsBlank = true;

    while (client.connected()) {
      if (client.available()) {

        String request = client.readStringUntil('\r');

        // Match the request
        if (request.indexOf("GET") != -1) {
          Serial.println(request);
          if (request.indexOf("/MP3=PLAY") != -1) {
            value = HIGH;
          }
          if (request.indexOf("/MP3=STOP") != -1) {
            value = LOW;
          }
        }

        char c = client.read();
        Serial.write(c);

        if (c == '\n' && currentLineIsBlank) {
          Serial.println("Sending response");

          // send a standard http response header
          // use \r\n instead of many println statements to speedup data send
          client.print(
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"  // the connection will be closed after completion of the response
            "Refresh: 30\r\n"        // refresh the page automatically every 20 sec
            "\r\n");
          client.print("<!DOCTYPE HTML>\r\n");
          client.print("<html>\r\n");
          client.print("<h1>Hello World!</h1>\r\n");
          client.print("Requests received: ");
          client.print(++reqCount);

          if(value == HIGH)
          {
            client.print("<audio src=\"TEST.mp3\" controls autoplay loop>\r\n");
            client.print("</audio>\r\n");
          }
          client.println("<a href=\"/MP3=PLAY\"\"><button>Play </button></a>");
          client.println("<a href=\"/MP3=STOP\"\"><button>Stop </button></a><br />");
          
          client.print("</html>\r\n");
          break;
        }
        
        if (c == '\n') {
          currentLineIsBlank = false;
        }
      }
    }

    // give the web browser time to receive the data
    delay(2000);

    // close the connection:
    client.stop();
    Serial.println("Client disconnected");
    delay(100);
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
