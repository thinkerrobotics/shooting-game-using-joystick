#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);


byte c1[8]={B00000,B01010,B00000,B00000,B10001,B01110,B00000,B00000}; //Smile-1
byte c2[8]={B10000,B10100,B01110,B10101,B01110,B10100,B10000,B00000}; //Starship-2


void setup()
{

  lcd.begin();
  lcd.createChar(0 , c1);   //Creating custom characters in CG-RAM
  lcd.createChar(1 , c2);   //Creating custom characters in CG-RAM
  

  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Hello World!");
  lcd.setCursor(0,1);
  lcd.print(char(0));
  lcd.print(char(1));
  
  
}

void loop()
{
}
