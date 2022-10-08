#include <OneWire.h>
#include <DallasTemperature.h>

#define PIN_RELAY_1  4 // the Arduino pin, which connects to the IN1 pin of relay module
#define PIN_RELAY_2  5 // the Arduino pin, which connects to the IN2 pin of relay module
#define PIN_RELAY_3  6 // the Arduino pin, which connects to the IN3 pin of relay module
#define PIN_RELAY_4  7 // the Arduino pin, which connects to the IN4 pin of relay module

// Data wire is plugged into digital pin 2 on the Arduino
#define TEMPA 9
#define TEMPB 2
#define TEMPC 11
#define TEMPD 10
// Charecter that marks difference in temps
#define TEMP_DIVIDER "|"
// Target temperature of tanks
#define TEMP_TARGET 63.0

// Packet Identifiers
#define PACKET_TEMP_IDEN "T"
#define PACKET_LUX_IDEN "L"

#include <BH1750.h>

BH1750 GY302; // initialize BH1750 object

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// Include if your want to reinclude loading screen
// #include "lc_chars.h"

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd2(0x26,20,4);
// Setup a oneWire instance to communicate with any OneWire device

/* Old one wire setup, why was it written like this?

OneWire oneWire(TEMPB);  
OneWire oneWire2(TEMPC);  
OneWire oneWire3(TEMPD);  
OneWire oneWire4(TEMPA);  
bool initialized = false; 
// Pass oneWire reference to DallasTemperature library
DallasTemperature atank(&oneWire4);
DallasTemperature btank(&oneWire);
DallasTemperature ctank(&oneWire2);
DallasTemperature dtank(&oneWire3);

*/
// New one wire setup
OneWire oneWire_A(TEMPA);  
OneWire oneWire_B(TEMPB);  
OneWire oneWire_C(TEMPC);  
OneWire oneWire_D(TEMPD);  
bool initialized = false; 
// Pass oneWire reference to DallasTemperature library
DallasTemperature atank(&oneWire_A);
DallasTemperature btank(&oneWire_B);
DallasTemperature ctank(&oneWire_C);
DallasTemperature dtank(&oneWire_D);

void setup(void)
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);

  /* idk if this is needed
  lcd.createChar(0, zero);
  lcd.createChar(1, one);
  lcd.createChar(2, two);
  lcd.createChar(3, three);
  lcd.createChar(4, four);
  lcd.createChar(5, five);
  */
  
  lcd2.init();
  lcd2.backlight();
  
 
  // Start up the libraries
  atank.begin();  
  btank.begin();  
  ctank.begin();
  dtank.begin();
  GY302.begin();

  // Designates IO device ports
  
  pinMode(PIN_RELAY_1, OUTPUT);
  pinMode(PIN_RELAY_2, OUTPUT);
  pinMode(PIN_RELAY_3, OUTPUT);
  pinMode(PIN_RELAY_4, OUTPUT);

  // Starts serial
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
}
void updateProgressBar(unsigned long count, unsigned long totalCount, int lineToPrintOn) {
    double factor = totalCount/80.0;          
    int percent = (count+1)/factor;
    int number = percent/5;
    int remainder = percent%5;
    if(number > 0)
    {
      for(int j = 0; j < number; j++)
      {
        lcd.setCursor(j,lineToPrintOn);
       lcd.write(5);
      }
    }
       lcd.setCursor(number,lineToPrintOn);
       lcd.write(remainder); 
     if(number < 16)  //If using a 20 character display, this should be 20!
    {
      for(int j = number+1; j <= 16; j++)  //If using a 20 character display, this should be 20!
      {
        lcd.setCursor(j,lineToPrintOn);
       lcd.write(0);
      }
    }  
 }

void InitLoop(){
  updateProgressBar(millis(), 15000, 1);
  if(millis() >= 15000){
    digitalWrite(PIN_RELAY_1, HIGH);
    delay(100);
    digitalWrite(PIN_RELAY_2, HIGH);
    delay(100);
    digitalWrite(PIN_RELAY_3, HIGH);
    delay(100);
    digitalWrite(PIN_RELAY_4, HIGH);
    delay(100);
    digitalWrite(PIN_RELAY_1, LOW);
    delay(100);
    digitalWrite(PIN_RELAY_2, LOW);
    delay(100);
    digitalWrite(PIN_RELAY_3, LOW);
    delay(100);
    digitalWrite(PIN_RELAY_4, LOW);
    delay(100);
    lcd.clear();
    lcd.home();
    lcd.setCursor(0,0);
    lcd.print("All Set!");    
    delay(1000);
    initialized = true;
  }

}


void reqTemps(float &ATEMP, float &BTEMP, float &CTEMP, float &DTEMP) {
  atank.requestTemperatures();
  btank.requestTemperatures();
  ctank.requestTemperatures();
  dtank.requestTemperatures();

  ATEMP = atank.getTempFByIndex(0);
  BTEMP = btank.getTempFByIndex(0);
  CTEMP = ctank.getTempFByIndex(0);
  DTEMP = dtank.getTempFByIndex(0);
}

// Simple checksum for data integrity
// Adapted from: 
// - https://forum.arduino.cc/t/simple-checksum-that-a-noob-can-use/300443
// - https://www.techiedelight.com/iterate-over-characters-string-cpp/
char checksum(const String &s)
{
    byte ch = 0;
    for (char const &c: s) {ch ^= c;} 
    return ch;
}

// Function for sending packet data w/ integrity checksum
// Iden is a 1 charecter long string meant to distinguish packets
void sendSerialPacket(const String &msg, const String &iden) {
  const String msg_with_iden = iden[0] + msg;                    // Pads message with identifying tags
  Serial.println(msg_with_iden + "C" + checksum(msg_with_iden)); // Sends Message
}


void TempBank(){
  float _ATEMP, _BTEMP, _CTEMP, _DTEMP;
  reqTemps(_ATEMP, _BTEMP, _CTEMP, _DTEMP);

  // Convert temperatures to string
  String ATEMP = String(_ATEMP, 2); 
  String BTEMP = String(_BTEMP, 2);
  String CTEMP = String(_CTEMP, 2);
  String DTEMP = String(_DTEMP, 2);

  // String representation of temps
  String ATEMP_FORMATTED = "A" + ATEMP + "F" + " ";
  String BTEMP_FORMATTED = "B" + BTEMP + "F" + " ";
  String CTEMP_FORMATTED = "C" + CTEMP + "F" + " ";
  String DTEMP_FORMATTED = "D" + DTEMP + "F" + " ";

  lcd.setCursor(0,0);
  lcd.print(String(ATEMP_FORMATTED + BTEMP_FORMATTED));
  lcd.setCursor(0,1);
  lcd.print(String(CTEMP_FORMATTED + DTEMP_FORMATTED));

  // Construct message to be sent over USB
  String SERIAL_MSG = String(ATEMP) + TEMP_DIVIDER +
                      String(BTEMP) + TEMP_DIVIDER +  
                      String(CTEMP) + TEMP_DIVIDER +
                      String(DTEMP);

  // Send temperature data over serial 
  sendSerialPacket(SERIAL_MSG, PACKET_TEMP_IDEN);
}

void TempLoop(float temp){
  float ATEMP, BTEMP, CTEMP, DTEMP;
  reqTemps(ATEMP, BTEMP, CTEMP, DTEMP);

  ATEMP > temp ? digitalWrite(PIN_RELAY_1, LOW) : digitalWrite(PIN_RELAY_1, HIGH);
  BTEMP > temp ? digitalWrite(PIN_RELAY_2, LOW) : digitalWrite(PIN_RELAY_2, HIGH);
  CTEMP > temp ? digitalWrite(PIN_RELAY_3, LOW) : digitalWrite(PIN_RELAY_3, HIGH);
  DTEMP > temp ? digitalWrite(PIN_RELAY_4, LOW) : digitalWrite(PIN_RELAY_4, HIGH);
}

void RelayBank(){
  String AState, BState, CState, DState;

  digitalRead(PIN_RELAY_1) == HIGH ? AState = "OFF" : AState = "ON";
  digitalRead(PIN_RELAY_2) == HIGH ? BState = "OFF" : BState = "ON";
  digitalRead(PIN_RELAY_3) == HIGH ? CState = "OFF" : CState = "ON";
  digitalRead(PIN_RELAY_4) == HIGH ? DState = "OFF" : DState = "ON";

  lcd2.clear();
  lcd2.setCursor(0,0);
  lcd2.print("A:" + String(AState) + " B:" + String(BState));
  lcd2.setCursor(0,1);
  lcd2.print("C:" + String(CState) + " D:" + String(DState));
}


void loop(void)
{
  TempBank();             // Sends & prints temps
  TempLoop(TEMP_TARGET);  // Sets relay state based on temp
  RelayBank();            // Prints relay state
  
  // get reading from light module, send over serial
  sendSerialPacket(String(GY302.readLightLevel()), PACKET_LUX_IDEN);
}
