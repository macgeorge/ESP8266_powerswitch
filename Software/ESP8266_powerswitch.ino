#include <ESP8266WiFi.h>

const char* ssid = "Your SSID";
const char* password = "Your Password";

int val=1;  //the value of the output GPIO2
int flag=0; //checks if there is a transition of GPIO0, to engage manual override

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  WiFiConnect(); //connect to the WiFi network
  
  // Start the server
  server.begin();
  Serial.println("Server started");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost, trying to reconnect");
    setup();
  }

  if (digitalRead(0) == LOW && flag==0) { //enable manual override
    val=0;
    flag=1;
    digitalWrite(2, val);
    Serial.println("Manual");
    delay(1);
    return;
  } 

  else if (digitalRead(0) == HIGH && flag==1) { //disable manual override
    flag=0;
    val=1;
    Serial.println("Auto");
    delay(1);
    return;
  }
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  if (req.indexOf("/gpio/0") != -1)
    val = 1;
  else if (req.indexOf("/gpio/1") != -1)
    val = 0;
  else if (req.indexOf("/gpio/status") != -1)
    Serial.println("getting status");
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  // Set GPIO2 according to the request
  digitalWrite(2, val);
  
  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n";
  s += (val)?"OFF":"ON";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}


void WiFiConnect(){
  pinMode(2, OUTPUT);
  digitalWrite(2, val);
  pinMode(0, INPUT);
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Print the IP address
  Serial.println(WiFi.localIP());
  IPAddress IPadd = WiFi.localIP(); 
  if (IPadd[2] == 0) {
    Serial.println("Connected to repeater, disconnecting");
    WiFi.disconnect();
  }
}
