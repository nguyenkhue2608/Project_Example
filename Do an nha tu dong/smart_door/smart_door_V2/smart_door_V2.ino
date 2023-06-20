//define GPIO
#define light1  12
#define light2  14
#define light3  26

#define led_xanh 4
#define led_do 16

#define speaker 15

#define button1 25
#define button2 33
#define button3 32

#define mq5_pin  17

#define SS_PIN    5  // ESP32 pin GIOP5 
#define RST_PIN   27 // ESP32 pin GIOP27 

#define servo 13

#include <Servo.h>
Servo door_servo;

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

#include <SPI.h>
#include <MFRC522.h>
MFRC522 rfid(SS_PIN, RST_PIN);
byte keyTagUID[4] = {0x99, 0x6D, 0x2C, 0xB9}; // key chia khoa

#include <WiFi.h>
#include <WiFiClient.h>
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp32.h>
char auth[] = "vqxVpK3EcaJF3ZwvbFOmOUkP6uWBcZiY";       
char ssid[] = "Nha Tao";                   
char pass[] = "25251325";

boolean checkData = false;
int VIRTUAL_PIN_0;
int VIRTUAL_PIN_1;
int VIRTUAL_PIN_2;
int VIRTUAL_PIN_3;
int OLD_VIRTUAL_PIN_0;
int OLD_VIRTUAL_PIN_1;
int OLD_VIRTUAL_PIN_2;
int OLD_VIRTUAL_PIN_3;

BLYNK_CONNECTED() 
{
  Blynk.syncAll();
}
BLYNK_WRITE(V0)
{
  VIRTUAL_PIN_0 = param.asInt();
  checkData = true;
}
BLYNK_WRITE(V1)
{
  VIRTUAL_PIN_1 = param.asInt();
  checkData = true;
}
BLYNK_WRITE(V2)
{
  VIRTUAL_PIN_2 = param.asInt();
  checkData = true;
}
WidgetLCD lcd_blynk(V5);
WidgetLED led_gas(V3);

void setup() {
  Serial.begin(9600);
  //GPIO init
  Serial.println("GPIO init");
  pinMode(light1, OUTPUT);
  pinMode(light2, OUTPUT);
  pinMode(light3, OUTPUT);
  pinMode(led_xanh, OUTPUT);
  pinMode(led_do, OUTPUT);
  pinMode(speaker, OUTPUT);

  pinMode(button1, INPUT_PULLUP); // sensor
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);

  pinMode(mq5_pin, INPUT);
  
  //servo init
  Serial.println("Servo init");
  door_servo.attach(servo);

  //lcd init
  Serial.println("LCD init");
  lcd.init();
  lcd.backlight();
  //rfid init
  Serial.println("RFID init");
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  ShowReaderDetails();
  
  Serial.println("Set defaul value");
  //khoi tao gia tri mac dinh
  digitalWrite(light1, LOW);
  digitalWrite(light2, LOW);
  digitalWrite(light3, LOW);
  digitalWrite(led_xanh, HIGH);
  digitalWrite(led_do, LOW);
  digitalWrite(speaker, LOW);

  door_servo.write(0);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smartconfig wifi...");

  WiFi.mode(WIFI_AP_STA);
  WiFi.beginSmartConfig();
  /* Wait for SmartConfig packet from mobile */
  Serial.println("Waiting for SmartConfig.");
  while (!WiFi.smartConfigDone()) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("SmartConfig done.");
  /* Wait for WiFi to connect to AP */
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("WiFi connected");
  lcd.setCursor(0, 0);
  lcd.print("Wifi connected");

  // Connect to Blynk server
  Blynk.begin(auth, ssid, pass);
  OLD_VIRTUAL_PIN_0 = VIRTUAL_PIN_0;
  OLD_VIRTUAL_PIN_1 = VIRTUAL_PIN_1;
  OLD_VIRTUAL_PIN_2 = VIRTUAL_PIN_2;

  Serial.println("All done, ready to work");
  lcd.setCursor(0, 0);
  lcd.print("Smart Door RFID");
  lcd.setCursor(0, 1);
  lcd.print("Scaning card...");
  lcd_blynk.clear();
}

void loop() {
  Blynk.run();
  //synch data app to device
  if (checkData == true) 
  {
    if(VIRTUAL_PIN_0 != OLD_VIRTUAL_PIN_0)
    {
      Serial.println("Nhan tren APP V0:" + String(VIRTUAL_PIN_0));
      digitalWrite(light1, VIRTUAL_PIN_0);
      OLD_VIRTUAL_PIN_0 = VIRTUAL_PIN_0;
    }
    if(VIRTUAL_PIN_1 != OLD_VIRTUAL_PIN_1)
    {
      Serial.println("Nhan tren APP V1:" + String(VIRTUAL_PIN_1));
      digitalWrite(light2, VIRTUAL_PIN_1);
      OLD_VIRTUAL_PIN_1 = VIRTUAL_PIN_1;
    }
    if(VIRTUAL_PIN_2 != OLD_VIRTUAL_PIN_2)
    {
      Serial.println("Nhan tren APP V2:" + String(VIRTUAL_PIN_2));
      digitalWrite(light3, VIRTUAL_PIN_2);
      OLD_VIRTUAL_PIN_2 = VIRTUAL_PIN_2;
    }
    checkData == false;
  }
  //scan button
  if (digitalRead(button1) == 0)
  {
    while (digitalRead(button1) == 0){}
      VIRTUAL_PIN_0 = !VIRTUAL_PIN_0;
      Blynk.virtualWrite(V0, VIRTUAL_PIN_0);
      digitalWrite(light1, VIRTUAL_PIN_0);
      Serial.println("Nhan tai Board V0:" + String(VIRTUAL_PIN_0));
  }
  if (digitalRead(button2) == 0)
  {
    while (digitalRead(button2) == 0){}
      VIRTUAL_PIN_1 = !VIRTUAL_PIN_1;
      Blynk.virtualWrite(V1, VIRTUAL_PIN_1);
      digitalWrite(light2, VIRTUAL_PIN_1);
      Serial.println("Nhan tai Board V1:" + String(VIRTUAL_PIN_1));
  }
  if (digitalRead(button3) == 0)
  {
    while (digitalRead(button3) == 0){}
      VIRTUAL_PIN_2 = !VIRTUAL_PIN_2;
      Blynk.virtualWrite(V2, VIRTUAL_PIN_2);
      digitalWrite(light3, VIRTUAL_PIN_2);
      Serial.println("Nhan tai Board V2:" + String(VIRTUAL_PIN_2));
  }
  //readGasMQ5();
  readRFID();
}

void readGasMQ5(void)
{
  if (digitalRead(mq5_pin) == 0)
  delay(200);
  if (digitalRead(mq5_pin) == 0)
  {
    //cap nhat len blynk
    led_gas.on();
    digitalWrite(speaker, HIGH);
    delay(3000);
    led_gas.off();
    digitalWrite(speaker, LOW);
  }
}

void readRFID(void)
{
  if (rfid.PICC_IsNewCardPresent()) 
  { // new tag is available
    digitalWrite(speaker, HIGH);
    delay(100);
    digitalWrite(speaker, LOW);
    
    if (rfid.PICC_ReadCardSerial()) 
    { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

      if (rfid.uid.uidByte[0] == keyTagUID[0] &&
          rfid.uid.uidByte[1] == keyTagUID[1] &&
          rfid.uid.uidByte[2] == keyTagUID[2] &&
          rfid.uid.uidByte[3] == keyTagUID[3] ) 
      {
        Serial.println("Access is granted");
        lcd.setCursor(0, 1);
        lcd.print("Access granted");
        lcd_blynk.print(0, 0, "Access granted");
        
        door_servo.write(180);
        digitalWrite(led_xanh, LOW);
        digitalWrite(led_do, HIGH);
        Blynk.virtualWrite(V4, 1);
        delay(3000);
        door_servo.write(0);
        digitalWrite(led_xanh, HIGH);
        digitalWrite(led_do, LOW);
        Blynk.virtualWrite(V4, 0);
        lcd.setCursor(0, 1);
        lcd.print("Scaning card...");
        lcd_blynk.print(0, 0, "Scaning card...");
      }
      else
      {
        Serial.print("Access denied, UID:");
        lcd.setCursor(0, 1);
        lcd.print("Access denied  ");
        lcd_blynk.print(0, 0, "Access denied  ");
        for (int i = 0; i < rfid.uid.size; i++)
        {
          Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
          Serial.print(rfid.uid.uidByte[i], HEX);
        }
        Serial.println();
        delay(2000);
        lcd.setCursor(0, 1);
        lcd_blynk.print(0, 0, "Scaning card...");
      }

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
}

void ShowReaderDetails() {
  // Get the MFRC522 software version
  byte v = rfid.PCD_ReadRegister(rfid.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown),probably a chinese clone?"));
    Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF))
  {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
    Serial.println(F("SYSTEM HALTED: Check RFID connections and reset"));
    lcd.setCursor(0, 0);
    lcd.print("Check RFID connect");
    //while (true); // khong lam gì cả, kiểm tra lại kết nối RFID
  }
}
