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
#include <BH1750.h>

BH1750 GY302; // initialize BH1750 object

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
byte zero[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte one[] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
};
byte two[] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000
};
byte three[] = {
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100
};
byte four[] = {
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110
};
byte five[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd2(0x26,20,4);
// Setup a oneWire instance to communicate with any OneWire device
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
byte customCharSmile[] = {
  B01010,
  B01010,
  B01010,
  B00000,
  B00000,
  B00000,
  B10001,
  B01110
};
void setup(void)
{
  lcd.init();
  lcd2.init();
  lcd2.backlight();
  GY302.begin();
//  lcd.begin();
  lcd.createChar(0, zero);
  lcd.createChar(1, one);
  lcd.createChar(2, two);
  lcd.createChar(3, three);
  lcd.createChar(4, four);
  lcd.createChar(5, five);
  
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
  //lcd.createChar(11, customChar);
  //lcd.createChar(12, customCharSmile);

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
void TempBank(){
  btank.requestTemperatures();
  ctank.requestTemperatures();
  atank.requestTemperatures();
  dtank.requestTemperatures();
  String BTEMP = (String("B"+(String((btank.getTempCByIndex(0) * 9.0) / 5.0 + 32.0)))+"F" +" ");
  String CTEMP = (String("C"+(String((ctank.getTempCByIndex(0) * 9.0) / 5.0 + 32.0)))+"F" +" ");
  String ATEMP = (String("A"+(String((atank.getTempCByIndex(0) * 9.0) / 5.0 + 32.0)))+"F" +" ");
  String DTEMP = (String("D"+(String((dtank.getTempCByIndex(0) * 9.0) / 5.0 + 32.0)))+"F" +" ");
  lcd.setCursor(0,0);
  lcd.print(String(ATEMP+BTEMP));
  lcd.setCursor(0,1);
  lcd.print(String(CTEMP+DTEMP));
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

void RelayBank(){
  String AState = "ON";
  String BState = "ON";
  String CState = "ON";
  String DState = "ON";
  if(digitalRead(PIN_RELAY_1) == HIGH){
    AState = "OFF ";
  }
  if(digitalRead(PIN_RELAY_2) == HIGH){
    BState = "OFF ";
  }
  if(digitalRead(PIN_RELAY_3) == HIGH){
    CState = "OFF ";
  }
  if(digitalRead(PIN_RELAY_4) == HIGH){
    DState = "OFF ";
  }
  lcd2.clear();
  lcd2.setCursor(0,0);
  lcd2.print(String("A:"+String(AState)+" B:"+String(BState)));
  lcd2.setCursor(0,1);
  lcd2.print(String("C:"+String(CState)+" D:"+String(DState)));
}
void TempLoop(float temp){
  btank.requestTemperatures();
  ctank.requestTemperatures();
  atank.requestTemperatures();
  dtank.requestTemperatures();
  if((btank.getTempCByIndex(0) * 9.0) / 5.0 + 32.0 > temp){
    digitalWrite(PIN_RELAY_2, LOW);
  }
  else{
    digitalWrite(PIN_RELAY_2, HIGH);
  }
  if((atank.getTempCByIndex(0) * 9.0) / 5.0 + 32.0 > temp){
    digitalWrite(PIN_RELAY_1, LOW);
  }
  else{
    digitalWrite(PIN_RELAY_1, HIGH);
  }
  if((ctank.getTempCByIndex(0) * 9.0) / 5.0 + 32.0 > temp){
    digitalWrite(PIN_RELAY_3, LOW);
  }
  else{
    digitalWrite(PIN_RELAY_3, HIGH);
  }
  if((dtank.getTempCByIndex(0) * 9.0) / 5.0 + 32.0 > temp){
    digitalWrite(PIN_RELAY_4, LOW);
  }
  else{
    digitalWrite(PIN_RELAY_4, HIGH);
  }
}
void loop(void)
{
  
  if(initialized){
    TempBank();
    RelayBank();
    TempLoop(63);
    //lcd2.print("HI:");
    // get reading from module
    uint16_t lux = GY302.readLightLevel();
    
    // display to Serial Monitor
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");
  } 
  else{
    InitLoop();
  }
}
