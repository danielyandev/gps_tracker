#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <AltSoftSerial.h>

#define DEBUG 1

// GSM serial pins
#define gsmRxPin 2
#define gsmTxPin 3

// connect GPS serial pins to d8 and d9, no need to define
// AltSoftSerial defines automatically
// #define gpsRxPin 8
// #define gpsTxPin 9


// Create a TinyGPS++ object
TinyGPSPlus gps;

// Create a software serials
// SoftwareSerial gpsSerial(gpsRxPin, gpsTxPin);
SoftwareSerial gsmSerial(gsmRxPin, gsmTxPin); 

//GPS Module RX pin to Arduino D9
//GPS Module TX pin to Arduino D8
AltSoftSerial gpsSerial;

unsigned long previousMillis = 0;
long interval = 5000;


void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);

  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(9600);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  
  //Begin serial communication with Arduino and SIM868
  gsmSerial.begin(9600);

  Serial.println("Initializing...");
  delay(5000);

  //Once the handshake test is successful, it will back to OK
  sendATcommand("AT", "OK", 500);
  sendATcommand("AT+CMGF=1", "OK", 500);
  
  blinkLed(3, 1000);
}

void loop()
{
  while(gsmSerial.available()){
    Serial.write(gsmSerial.read());
  }
  while(Serial.available())  {
    gsmSerial.write(Serial.read());
  }

  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
      previousMillis = currentMillis;
      sendGpsToServer();
  }
}

void sendGpsToServer() {     

  // wait for data
  for (unsigned long start = millis(); millis() - start < 2000;){
    while (gpsSerial.available()){
      gps.encode(gpsSerial.read());
      if (gps.location.isUpdated()){
        if (DEBUG) {
          Serial.print("Latitude= "); 
          Serial.print(gps.location.lat(), 6);
          Serial.print(" Longitude= "); 
          Serial.println(gps.location.lng(), 6);

          Serial.print(" Sats= "); 
          Serial.println(gps.satellites.value());
        }
        break;
      }
    }
  }

  String lat = getLat();

  // send only valid location
  if (lat == "NA") {
    if (DEBUG) {
      Serial.print("No coordinates. Sats count: ");
      Serial.println(gps.satellites.value());
    }

    return;
  }

  // indicate that there is data to send
  blinkLed(3, 300);

  String lng = getLng();
  String alt = getAltitude();
  String spd = getSpeed();
  String course = getCourse();
  
  String url = "http://danielyandev.000webhostapp.com/gpsdata.php?";
  url += "lat=" + lat;
  url += "&lng=" + lng;
  url += "&alt=" + alt;
  url += "&spd=" + spd;
  url += "&course=" + course;
 
  if (DEBUG) {
    Serial.println(url);
  }
      
  sendATcommand("AT+CFUN=1", "OK", 200);
  //AT+CGATT = 1 Connect modem is attached to GPRS to a network. AT+CGATT = 0, modem is not attached to GPRS to a network
  sendATcommand("AT+CGATT=1", "OK", 200);
  //Connection type: GPRS - bearer profile 1
  sendATcommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"", "OK", 200);
  //sets the APN settings for your network provider.
  sendATcommand("AT+SAPBR=3,1,\"APN\",\"internet\"", "OK", 200);
  //enable the GPRS - enable bearer 1
  sendATcommand("AT+SAPBR=1,1", "OK", 200);
  //Init HTTP service
  sendATcommand("AT+HTTPINIT", "OK", 200); 
  sendATcommand("AT+HTTPPARA=\"CID\",1", "OK", 100);
  //Set the HTTP URL sim800.print("AT+HTTPPARA="URL","http://danielyandev.000webhostapp.com/gpsdata.php?lat=222&lng=222"\r");
  gsmSerial.print("AT+HTTPPARA=\"URL\",\"");
  gsmSerial.print(url);
  sendATcommand("\"", "OK", 100);
  //Set up the HTTP action
  sendATcommand("AT+HTTPACTION=0", "0,200", 100);
  //Terminate the HTTP service
  sendATcommand("AT+HTTPTERM", "OK", 100);
  //shuts down the GPRS connection. This returns "SHUT OK".
  sendATcommand("AT+CIPSHUT", "SHUT OK", 100);
}

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

  uint8_t x=0,  answer=0;
  char response[100];
  unsigned long previous;

  //Initialice the string
  memset(response, '\0', 100);
  delay(100);
  
  //Clean the input buffer
  while( gsmSerial.available() > 0) gsmSerial.read();
  
  if (ATcommand[0] != '\0'){
    //Send the AT command 
    gsmSerial.println(ATcommand);
  }

  x = 0;
  previous = millis();

  //this loop waits for the answer with time out
  do{
      //if there are data in the UART input buffer, reads it and checks for the asnwer
      if(gsmSerial.available()){
          response[x] = gsmSerial.read();
          //Serial.print(response[x]);
          x++;
          // check if the desired answer (OK) is in the response of the module
          if(strstr(response, expected_answer) != NULL){
              answer = 1;
          }
      }
  }while((answer == 0) && ((millis() - previous) < timeout));
  // }while(answer == 0);

  
  if (DEBUG) {
    Serial.println(response);
  }

  return answer;
}


// MAIN START

String getLat() {
  String lat = "NA";
  if (gps.location.isValid()) {
    lat = String(gps.location.lat(), 6);
  }

  return lat;
}

String getLng() {
  String lng = "NA";
  if (gps.location.isValid()) {
    lng = String(gps.location.lng(), 6);
  }

  return lng;
}

String getAltitude() {
  String alt = "NA";
  if (gps.altitude.isValid()) {
    alt = String(gps.altitude.meters());
  }

  return alt;
}

String getCourse() {
  String course = "NA";

  if (gps.course.isValid()) {
    course = String(gps.course.deg());
  }

  return course;
}

String getSpeed() {
  String spd = "NA";

  if (gps.speed.isValid()) {
    spd = String(gps.speed.kmph());
  }

  return spd;
}

void blinkLed(int n, int timeout) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(timeout);
    digitalWrite(LED_BUILTIN, LOW);
    delay(timeout);
  }
}
