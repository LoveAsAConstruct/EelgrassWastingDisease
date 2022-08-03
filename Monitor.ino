#include <OneWire.h>
#include <DallasTemperature.h>

#define PIN_RELAY_1  4 // the Arduino pin, which connects to the IN1 pin of relay module
#define PIN_RELAY_2  5 // the Arduino pin, which connects to the IN2 pin of relay module
#define PIN_RELAY_3  6 // the Arduino pin, which connects to the IN3 pin of relay module
#define PIN_RELAY_4  7 // the Arduino pin, which connects to the IN4 pin of relay module

// Data wire is plugged into digital pin 2 on the Arduino
#define TEMPB 2
#define TEMPC 11
#define TEMPD 10
#define TEMPA 9

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd2(0x26,20,4);
// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(TEMPB);  
OneWire oneWire2(TEMPC);  
OneWire oneWire3(TEMPD);  
OneWire oneWire4(TEMPA);  

// Pass oneWire reference to DallasTemperature library
DallasTemperature atank(&oneWire4);
DallasTemperature btank(&oneWire);
DallasTemperature ctank(&oneWire2);
DallasTemperature dtank(&oneWire3);
byte customChar[] = {
  B00100,
  B01010,
  B00100,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
void setup(void)
{
 
  lcd.init();                      // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  btank.begin();  // Start up the library
  ctank.begin();
  atank.begin();  // Start up the library
  dtank.begin();
  Serial.begin(9600);
  pinMode(PIN_RELAY_1, OUTPUT);
  pinMode(PIN_RELAY_2, OUTPUT);
  pinMode(PIN_RELAY_3, OUTPUT);
  pinMode(PIN_RELAY_4, OUTPUT);
  lcd.createChar(10000, customChar);

}

void loop(void)
{
  // Send the command to get temperatures
  btank.requestTemperatures();
  ctank.requestTemperatures();
  atank.requestTemperatures();
  dtank.requestTemperatures();
  lcd.setCursor(0,0);
  String BTEMP = String("B"+(String((btank.getTempCByIndex(0) * 9.0) / 5.0 + 32.0)))+"F" +String((char)10000);
  String CTEMP = String("C"+(String((ctank.getTempCByIndex(0) * 9.0) / 5.0 + 32.0)))+"F" +String((char)10000);
  String ATEMP = String("A"+(String((atank.getTempCByIndex(0) * 9.0) / 5.0 + 32.0)))+"F" +String((char)10000);
  String DTEMP = String("D"+(String((dtank.getTempCByIndex(0) * 9.0) / 5.0 + 32.0)))+"F" +String((char)10000);
  lcd.print(String(ATEMP+BTEMP));
  lcd.setCursor(0,1);
  lcd.print(String(CTEMP+DTEMP));
  //print the temperature in Celsius
  Serial.print("Temperature: ");
  Serial.print(btank.getTempCByIndex(0));
  Serial.print((char)176);//shows degrees character
  Serial.print("C  |  ");
 
  //print the temperature in Fahrenheit
  Serial.print((btank.getTempCByIndex(0) * 9.0) / 5.0 + 32.0);
  Serial.print((char)176);//shows degrees character
  Serial.println("F");
  Serial.println("Turn on all");
  digitalWrite(PIN_RELAY_1, HIGH);
  digitalWrite(PIN_RELAY_2, HIGH);
  digitalWrite(PIN_RELAY_3, HIGH);
  digitalWrite(PIN_RELAY_4, HIGH);
  delay(1000);

  Serial.println("Turn off all");
  digitalWrite(PIN_RELAY_1, LOW);
  digitalWrite(PIN_RELAY_2, LOW);
  digitalWrite(PIN_RELAY_3, LOW);
  digitalWrite(PIN_RELAY_4, LOW);
  delay(1000);
  delay(500);
}
