
#include <DHT.h>
#include<WiFi.h>
#include "time.h"


const char* ssid     = "Redmi Note 10 Pro";
const char* password = "123456789";


const char* ntpServer = "np.pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 19800;//GMT+5:30

int UVOUT = 2; //Output from the sensor 
int REF_3V3 = 4; //3.3V power on the Arduino board
DHT dht(19,DHT11);//dht pin number
float sensor=35;//MQ5 gas sensor in number
float gas_value;



void setup(){
  pinMode(15,OUTPUT); 
  pinMode(sensor,INPUT);
  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);
  Serial.begin(9600);
  dht.begin();
  
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

}

void loop(){
  int potentiometer_value = analogRead(15);
  Serial.println(potentiometer_value);

  if( potentiometer_value< 2000){
    Serial.println("home_mode");
    home_mode();
    
    }
   else if(potentiometer_value > 2000 && potentiometer_value< 3500){
    Serial.println("agriculture_mode");
    agriculture_mode();
    
    }
   else {
     Serial.println("health_mode"); 
     health_mode();
      
    }

}


//agriculture mode 
void agriculture_mode(){
  delay(1000);
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  Serial.print("Temperature = ");
  Serial.print(temp);
  Serial.println();
  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println();
  gas_value=analogRead(sensor);
  Serial.print("GAS = ");
  Serial.print(gas_value);
  Serial.println();
}


//home mode
void home_mode(){
  printLocalTime();
  delay(1000);
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  Serial.print(temp);
  Serial.println();
  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println();
  gas_value=analogRead(sensor);
  Serial.println(gas_value);
}


//health mode
void health_mode(){
  delay(1000);
  int uvLevel = averageAnalogRead(UVOUT);
  int refLevel = averageAnalogRead(REF_3V3);
    //Use the 3.3V power pin as a reference to get a very accurate output value from sensor
  float outputVoltage = 3.3 / refLevel * uvLevel;
  
  float uvIntensity = mapfloat(outputVoltage, 0.99, 2.9, 0.0, 15.0);

  Serial.print("MP8511 output: ");
  Serial.print(uvLevel);

  Serial.print(" MP8511 voltage: ");
  Serial.print(outputVoltage);

  Serial.print(" UV Intensity (mW/cm^2): ");
  Serial.print(uvIntensity);  
  
  Serial.println();
  
//  delay(100);
//  if (uvIntensity>4 && uvIntensity<8){
//    Serial.println("Apply sunscreen man");
//  }
//  else if(uvIntensity>8){
//    Serial.println("Don't go outside unsafe level");
//  }
//  
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

}


int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);  
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
