#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Servo.h>

const char* ssid = "YOUR WIFI";  // change this to the name of your wifi<br>const char* password = "YOUR PASSWORD"; // change this to the password of your wifi
const char* password = "YOUR PASSWORD"; // change this to the password of your wifi
const char* deviceName = "helloservo"; // pick a name that you can use to find this on the network

ESP8266WebServer server(80);

Servo myservo; // a servo object
int gPos = 0;  // a position to move to
int nPos = 0; // our current servo position

void ShowSlider() {

  String message = "<div align=\"center\"> <h1>Slide It</h1> <input style=\"width: 90%\" type=\"range\" min=\"3\" max=\"180\" oninput=\"httpGet(this.value)\"></div>";
  message += "<script>function httpGet(theVal){ console.log(theVal); ";
  message += "var xmlHttp = new XMLHttpRequest();";
  message += "xmlHttp.open( \"GET\", \"/set?val=\" + theVal, false );"   ;
  message += "xmlHttp.send( null );";
  message += "return xmlHttp.responseText;";
  message += "} </script> " ;
  server.send(200, "text/html", message);

}

void handleBadRequest() {
  server.send(404, "text/plain", "404 nope\n\n");
}

void setup(void) {

  Serial.begin(115200); // Serial communication for debugging
  myservo.attach(D3);  // attaches the servo on pin D3

  WiFi.begin(ssid, password); // set up the wifi connection
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) { // Wait for wifi connection
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin(deviceName)) {
    Serial.println("MDNS responder started");
  }

  server.on("/", ShowSlider); // go to the main url to show the slider
  server.on("/slider", ShowSlider); // or go to /slider to show the slider

  server.on("/set", []() {  // go to /set?val=[0-180] example : "/set?val=120"
    String valString = server.arg("val"); // get the input string of 'val' in the URL
    int val = valString.toInt();  // parse the int value of val
    Serial.print("got val :" ); 
    Serial.println(val);
    gPos = val; // set our global goto position to the incomming value
    server.send(200, "text/plain", "now setting to val: " + valString );
  });

  server.onNotFound(handleBadRequest);
  
  server.begin();  // start up the http server
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient(); // first, each loop, make sure clients can connect

  // below is a simple smoothing code that slows the motions a bit
  if (nPos < gPos) { // if the go current position is less then the goto posotion
    nPos += 1; // add one degree onto the current position
    myservo.write(nPos); // set our servo to move to the new position
    delay(15); // pause for a second to catch up 
  }
  else   if (nPos > gPos) { // if the current position is more then the goto position
    nPos -= 1; // subtract one degree from the current position
    myservo.write(nPos);// set our servo to move to the new position
    delay(15);// pause for a second to catch up
  }
}
