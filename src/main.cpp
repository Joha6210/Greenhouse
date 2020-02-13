/* Sensor test sketch
  for more information see http://www.ladyada.net/make/logshield/lighttemp.html
  */

#include "Arduino.h"
#include "Esp.h"

#include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>

// Multiplexer pins

int select1 = 13;
int select2 = 14;

//Multiplexer conn
/*
    IC P12 = temp
    IC P14 = light
    IC P15 = O2
    IC P11 = humidity

*/

const char * ssid = "HTX-IT-LOKALE-2G"; // your network SSID (name)
const char * pass = "hcoersted";  // your network password

const char* serverName = "http://192.168.11.11:80/iot/post-esp-data.php?data=";

//ADC Pin Variables
int adcPin = A0;        

//the analog pin the TMP36's Vout (sense) pin is connected to
//the resolution is 10 mV / degree centigrade with a
//500 mV offset to allow for negative temperatures


int tempReading;        // the analog reading from the sensor

void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(9600);   
  pinMode(select1,OUTPUT_OPEN_DRAIN);
  pinMode(select2,OUTPUT_OPEN_DRAIN);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

}
 
float readTemp(){
  digitalWrite(select1,LOW);
  digitalWrite(select2,LOW);
  delay(100);
  tempReading = analogRead(adcPin);  
 
  // converting that reading to voltage, which is based off the reference voltage
  float voltage = tempReading * 1;
  voltage /= 1024.0; 

 
  // now print out the temperature
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                               //to degrees ((volatge - 500mV) times 100)
  return temperatureC;
}

float readLight(){
  digitalWrite(select1,HIGH);
  digitalWrite(select2,LOW);
  delay(100);
  tempReading = analogRead(adcPin);  
 
  // converting that reading to voltage, which is based off the reference voltage
  float voltage = tempReading * 1;
  voltage /= 1024.0; 

 
  // now print out the temperature
  //float light = (1.25*10000000)*voltage ;  //converting from 10 mv per degree wit 500 mV offset
                                               //to degrees ((volatge - 500mV) times 100)
  return tempReading;
}

float readCon(){
  digitalWrite(select1,LOW);
  digitalWrite(select2,HIGH);
  long sum = 0;
    for(int i=0; i<32; i++)
    {
        sum += analogRead(adcPin);
    }

    sum >>= 5;

    float MeasuredVout = sum * (1 / 1023.0);

    //float Concentration = FmultiMap(MeasuredVout, VoutArray,O2ConArray, 6);
    //when its output voltage is 2.0V,
    float Concentration = MeasuredVout * 0.41 / 1.0;
    float Concentration_Percentage=Concentration*100;

    return Concentration_Percentage;
}

float readHum(){
  digitalWrite(select1,HIGH);
  digitalWrite(select2,HIGH);
  float relativeHumidity  = analogRead(adcPin);

 float av=0.0048875*relativeHumidity;
 
  float res=(av-0.86)/0.03;

  return res;
}

void loop(void) {

  int temp = readTemp();
  Serial.print(temp); Serial.println(" degrees C");


  int light = readLight();
  Serial.print(light); Serial.println(" light level");
  

  int o2Concentration = readCon();
  Serial.print(o2Concentration); Serial.println(" O2 Concentration");
  

  int humidity = readHum();
  Serial.print(humidity); Serial.println(" Humidity ");

  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    
    
    // Prepare your HTTP POST request data
    String httpRequestData = String(serverName) + "&lys=" + String(light)
                          + "&temperatur=" + String(temp) + "&fugtighed=" + String(humidity)
                          + "&iltniveau=" + String(o2Concentration);

    http.begin(httpRequestData);
    
    // Specify content-type header
    http.addHeader("Content-Type", "text/plain");

    int httpCode = http.GET();

    Serial.println(httpCode);
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

    delay(2000);
  }

}