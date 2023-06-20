#include <Arduino.h>
//#include <SoftwareSerial.h>
#include <DHT.h>

//SoftwareSerial Serial1(7,6);

#define DHTTYPE DHT11
#define DHTPIN 2

#define SIM_PWR 3
#define Relay   8
#define Buzz    9

String inputString;
uint32_t count=0,count2=0;
float Tem=0;
float Hum=0;
DHT dht(DHTPIN, DHTTYPE);

bool SerialEvent() 
{
  count2=millis();
  bool stringComplete = false;
  inputString="";
  while (Serial1.available()) 
  {
    char inChar = Serial1.read();  
    if(inChar!=0xa)
    {
      inputString += inChar;
    }
    else  
    {
      stringComplete = true;
      Serial.println(inputString);
      break;
    }  
    if(millis()-count2>1500)
    {
      break;
    }  
  }
  
  return stringComplete;
}

void gsm_init()
{
    Serial.println("GSM config");
    Serial.println("Ready...");
    delay(100);
    Serial1.println("ATE0\r\n");
    while(!SerialEvent());
    Serial1.println("AT+CLIP=1\r\n"); 
    while(!SerialEvent());
    Serial1.println("AT+CMGF=1\r\n"); 
    while(!SerialEvent());
    Serial1.println("AT+CNMI=2,2,0,0,0\r"); 
    while(!SerialEvent());
//    callSomeone("+84352937023");
//    delay(1000);
    Serial.println("da gui");
    delay(1000);
}

void gps_init()
{
    Serial.println("GPS config");
    Serial1.print("AT+CGPSPWR=1\r\n");
    while(!SerialEvent());
}

void gprs_init()
{
  Serial.println("GPRS config");
  // Serial1.println("AT+CGATT = 1\r");
  // while(!SerialEvent());
  // Serial1.println("AT+SAPBR=3,1,\"m3-world\",\"mms\",\"mms\"");
  // while(!SerialEvent());
  Serial1.println("AT+SAPBR =1,1\r");
  while(!SerialEvent());
  delay(3000);
  Serial1.print("AT+HTTPINIT\r");
  while(!SerialEvent());
}

void readGPS()
{
  Serial1.println("AT+CGPSINF=0\r\n");
  while(!SerialEvent());
  // Serial1.println("AT+CIPGSMLOC=1,1");
  // while(!SerialEvent());
}

bool readHum()
{
  Hum=dht.readHumidity();
  if(isnan(Hum)) 
  {
    Serial.println("Read DHT Failed.");
    return false;
  }
  else  return true;
}

bool readTem()
{
  Tem=dht.readTemperature();
  if(isnan(Tem)) 
  {
    Serial.println("Read DHT Failed.");
    return false;
  }
  else  return true;
}

void Request_HUM_HTTP(float data)
{
  Serial1.println("AT+HTTPPARA=\"URL\",\"http://api.thingspeak.com/update?api_key=N5X9W6QXF864G3QB&field1="+(String)data+"\"\r");
  delay(500);
  while(!SerialEvent());
  Serial1.println("AT+HTTPACTION=0\r");
  delay(500);
  while(!SerialEvent());
  Serial.println("AT+HTTPACTION=0\r");
  delay(700);
  while(!SerialEvent());
}

void Request_TEM_HTTP(float data)
{
  Serial1.println("AT+HTTPPARA=\"URL\",\"http://api.thingspeak.com/update?api_key=N5X9W6QXF864G3QB&field2="+(String)data+"\"\r");
  delay(500);
  while(!SerialEvent());
  Serial1.println("AT+HTTPACTION=0\r");
  delay(500);
  while(!SerialEvent());
  Serial.println("AT+HTTPACTION=0\r");
  delay(700);
  while(!SerialEvent());
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  Serial1.begin(57600);
    Serial1.println("Turn on GPS..."); 
  Serial.println("config GPIO...");
  pinMode(SIM_PWR,OUTPUT);
  pinMode(Relay,OUTPUT);
  pinMode(Buzz,OUTPUT);
  digitalWrite(SIM_PWR,LOW);
  digitalWrite(Relay,LOW);
  digitalWrite(Buzz,HIGH);
  Serial.println("Config SIM...");  
  digitalWrite(SIM_PWR,HIGH);
  delay(3000);
  digitalWrite(SIM_PWR,LOW);
  gsm_init();
  dht.begin();
 
  //gps_init();
  gprs_init();
  count==millis();
}
int Value;
void loop() 
{
  SerialEvent();
  if(millis()-count >21000)
  {
    Value++;
    Request_HUM_HTTP(Value);
//    if(readHum())
//    {
//      Serial.println("Send HUM.");
//      Serial.println(Hum);
//      Request_HUM_HTTP(Hum);
//    }  
//    delay(21000);
//    if(readTem())
//    {
//      Serial.println("Send TEM.");
//      Serial.println(Tem);
//      Request_TEM_HTTP(Tem);
//    }  
    count=millis();
  }
}
