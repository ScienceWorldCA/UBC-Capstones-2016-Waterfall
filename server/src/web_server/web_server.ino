#include <SPI.h>
#include <Ethernet2.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0xC6, 0x97 }; //physical mac address
byte ip[] = { 192, 168, 0, 2 }; // local area network ip
byte gateway[] = { 192, 168, 1, 254 }; // found via route get default in terminal
byte subnet[] = { 255, 255, 255, 0 }; // standard subnet mask
EthernetServer server(80); //server port

String readString; 

//////////////////////

void setup(){

  pinMode(13, OUTPUT); // pin selected to control. Built-in LED for demo.
  digitalWrite(13, LOW); // set it to low to it starts off
  //start Ethernet
  Ethernet.begin(mac, ip, gateway, subnet); // start the Ethernet
  server.begin(); // start the server

  //enable serial data print 
  Serial.begin(9600); 
}

void loop(){
  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //read char by char HTTP request
        if (readString.length() < 100) {

          //store characters to string 
          readString += c; 
          //Serial.print(c);
        } 

        //if HTTP request has ended
        if (c == '\n') {

          ///////////////
          Serial.println(readString); //print to serial monitor for debuging 

          //now output HTML data header
          if(readString.indexOf('?') >=0) {
           client.println("HTTP/1.1 204 Null");
           client.println();
           client.println();  
          }
          else {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
  
            client.println("<HTML>");
            client.println("<HEAD>");
            client.println("</HEAD>");
            client.println("<BODY>");
  
            client.println("<H1>Digital Waterfall Test</H1>");
            
            client.println("<a href=\"/?on\" target=\"inlineframe\">ON</a>");
  
            client.println("<IFRAME name=inlineframe style=\"display:none\" >");          
            client.println("</IFRAME>");
  
            client.println("</BODY>");
            client.println("</HTML>");
          }

          delay(1);
          //stopping client
          client.stop();

          ///////////////////// control arduino pin
          if(readString.indexOf("on") >0)//checks for on
          {
            digitalWrite(13, HIGH);    // set pin 4 high
            Serial.println("Led On");
            delay(5000);
            digitalWrite(13, LOW);
            Serial.println("Led Off");
            break;
          }
          //clearing string for next read
          readString="";

        }
      }
    }
  }
} 
