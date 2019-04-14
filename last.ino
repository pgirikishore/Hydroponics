

#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT11   // DHT 11

#define LED D0

#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE); 

//Woon Jun Shen
//#include <DHT.h>  // Including library for dht
//int input_val=0;
//const int LM_35 = D0;
#include <ESP8266WiFi.h>
#include <OneWire.h>
//#include <PubSubClient.h>

const char *ssid =  "Vaibhavi";        //Your Access Point or Personal Hotspot, cannot be longer than 32 characters!
const char *pass =  "12345678";    //Your Access Point or Personal Hotspot password
const char* serverTS = "api.thingspeak.com";
String apiKey = "97D4KIYCB23P319Y";          //Insert your Channel API Key here
//const int pingPin = 2;          //Ultrasonic connected to GPIO0
int TRIGGER = 5; //Pin D1 = TRIGGER
int ECHO = 4; //Pin D2 = ECHO
 int tempPin = 16;//pin D0=temp
void setup() 
{
  dht.begin();
  
  pinMode(0,OUTPUT);            //LED connected to GPIO2
  Serial.begin(9600);         //Recommended speed is 115200
  pinMode(TRIGGER,OUTPUT);
  pinMode(ECHO,INPUT);
  pinMode(LED, OUTPUT);
  connectWifi();
}
  void connectWifi()
{ 
  Serial.print("Connecting to "+*ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected");
  Serial.println("");  
}//end connect

void loop() 
{
  // establish variables for duration of the ping,
  // and the distance result in inches and centimeters:
  long duration, inches, cm,temp;
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
digitalWrite(LED, HIGH);
  float h = dht.readHumidity();
    float t = dht.readTemperature(); 
          
    Serial.print("Current humidity = ");
    Serial.print(h);
    Serial.print("%  ");
   
    Serial.print("temperature = ");
    Serial.print(t); 
    Serial.println("C  ");
   
    
 
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
digitalWrite(LED, LOW);
  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.

  duration = pulseIn(ECHO, HIGH);

  // convert the time into a distance
  //inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
/*input_val = analogRead(LM_35);             // Reading analog input
   temp = (5.0 * input_val * 100.0) / 1024;   // Some calculation to convert analog value to temperature
   Serial.print("Temperature is : " );                       
   Serial.println(temp);*/
   //float t = dht.readTemperature(); 
    //Serial.print("temperature = ");
    //Serial.print(t); 
 if(t<30)
 {
  digitalWrite(LED, HIGH);
  //delay(1000);
 }
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  // dht.readTemperature();
   //Serial.print("Temperature: ");
    //Serial.print(temp);
    //Serial.print(" degrees Celcius");
 Serial.println();
 // delay(2000);

  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(100);              // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
  //delay(2000);              // wait for a second

  sendHeight(cm,t,h);
}
long microsecondsToCentimeters(long microseconds) 
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

long microsecondsToCelsius(long microseconds) 
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  microseconds = microseconds * 0.48828125;
  return microseconds ;
}
long microsecondsToInches(long microseconds) 
{ // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}


 void sendHeight(float cm,float t,float h)
{  
  WiFiClient tclient;//not to be confused with "client" in PubSub{}, and wclient for mqtt
   if (tclient.connect(serverTS, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
   //Serial.println("WiFi Client connected ");
   
   //ThingSpeak.setField(1, cm);
//ThingSpeak.setField(2, temp);
String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(cm);
     postStr += "&field2=";
    postStr += String(t);
     postStr += "&field3=";
    postStr += String(h);
    postStr += "\r\n\r\n";
    tclient.print("POST /update HTTP/1.1\n");
    tclient.print("Host: api.thingspeak.com\n");
    tclient.print("Connection: close\n");
    tclient.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    tclient.print("Content-Type: application/x-www-form-urlencoded\n");
    tclient.print("Content-Length: ");
    tclient.print(postStr.length());
    tclient.print("\n\n");
    tclient.print(postStr);
    //Serial.println(postStr);
   //delay(500);



   /*String stringOne = String("http://api.thingspeak.com/update?key=97D4KIYCB23P319Y&field1=") + cm;
Serial.println(stringOne);*/
   
   }//end if

 tclient.stop();
}//end send to ts
