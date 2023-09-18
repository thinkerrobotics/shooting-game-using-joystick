#include <EEPROM.h>

unsigned long game_start;

void setup() {
  Serial.begin(9600);
  //EEPROM.put(0, 0);

  Serial.println("Start");
  EEPROM.get(0, game_start);
  Serial.print("was: ");
  Serial.println(game_start);
  
  delay(game_start+200);
  
  EEPROM.put(0, millis());
  EEPROM.get(0, game_start);
  Serial.print("new: ");
  Serial.println(game_start);
   
}
/*
Start
was: 0
new: 199
Start
was: 199
Start
was: 199
new: 398
Start
was: 398
Start
was: 398
new: 598
*/

void loop() {
  // put your main code here, to run repeatedly:

}
