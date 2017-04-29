#include <ros.h>
#include <std_msgs/Bool.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "emergency_stop";
const char* password = "emergency_stop";

WiFiUDP Udp;
unsigned int localUdpPort = 6000;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacekt[] = "Hi there! Got the message :-)";  // a reply string to send back

ros::NodeHandle nh;
std_msgs::Bool pushed_msg;
ros::Publisher pub_button("emergency_pushed", &pushed_msg);

void setup()
{
  nh.initNode();
  nh.advertise(pub_button);
  //Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  //Serial.println();
  //Serial.println("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);
  //Serial.println("Access Point Created:");
  //Serial.print("SSID: ");
  //Serial.println(ssid);
  //Serial.print("pass: ");
  //Serial.println(password);
  IPAddress myIP = WiFi.softAPIP();

  Udp.begin(localUdpPort);
  
  //Serial.print("AP IP and port address: ");
  //Serial.print(myIP);
  //Serial.print(":");
  //Serial.println(localUdpPort);
}


void loop()
{
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    digitalWrite(LED_BUILTIN, LOW);
    // receive incoming UDP packets
    //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    //Serial.printf("UDP packet contents: %s\n", incomingPacket);

    if(len >= 4) {
      if(incomingPacket[0] == 's'
          && incomingPacket[1] == 't'
          && incomingPacket[2] == 'o'
          && incomingPacket[3] == 'p') {
        //Serial.println("Publishing stop command");
        pushed_msg.data = true;
        pub_button.publish(&pushed_msg);
      }
    }

    // send back a reply, to the IP address and port we got the packet from
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(replyPacekt);
    Udp.endPacket();
    delay(10);
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    pushed_msg.data = false;
    pub_button.publish(&pushed_msg);
    delay(10);
  }
  nh.spinOnce();
}
