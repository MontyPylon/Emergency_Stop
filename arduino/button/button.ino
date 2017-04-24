#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define ssid "MontyPylon"
#define pass "soccer_robots"
WiFiUDP Udp;
unsigned int localPort = 6000; // local port to listen on
char packetBuffer[255]; //buffer to hold incoming packet
char  ReplyBuffer[] = "stop"; // a string to send back

const int buttonPin = 4;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status
bool isPressed = false;
int downCounter = 0;

void setup() {
  Serial.begin(11500);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }  
  // initialize the LED pin as an output:
  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, LOW);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  WiFi.begin(ssid, pass);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to wifi");
  printWifiStatus();

  Serial.println("\nWaiting for incoming messages...");
  // if you get a connection, report back via serial:
  Udp.begin(localPort);
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  //Serial.println(buttonState);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH && !isPressed) {
    // turn LED off:

    //digitalWrite(LED_BUILTIN, HIGH);
    
    Udp.beginPacket("192.168.0.102", localPort);
    Udp.write("stop1");
    Udp.endPacket();
    delay(100);
    Udp.beginPacket("192.168.0.102", localPort);
    Udp.write("stop2");
    Udp.endPacket();
    delay(100);
    Udp.beginPacket("192.168.0.102", localPort);
    Udp.write("stop3");
    Udp.endPacket();
    
    isPressed = true;
  } else if(buttonState == LOW) {
    
    isPressed = false;
    delay(500);
    // turn LED on:
    //digitalWrite(LED_BUILTIN, LOW);
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
