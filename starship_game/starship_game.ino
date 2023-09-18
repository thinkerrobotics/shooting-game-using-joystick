#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);

#include <EEPROM.h>

byte c1[8]={B10000,B10100,B01110,B10101,B01110,B10100,B10000,B00000}; //Starship
byte c2[8]={B00100,B01000,B01010,B10100,B01010,B01000,B00100,B00000}; //Enemy
byte c3[8]={B00000,B00000,B00000,B00110,B00110,B00000,B00000,B00000}; //Bullet

String lcd_array[2][16] = 
  {{"}"," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "},
   {" "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "}}; 
/*
} - Starship
> - Bullet
< - Enemy
*/

const unsigned int MAX_MESSAGE_LENGTH = 12;
int starship_possiton = 0;

bool game_is_in_progress = false;
unsigned long game_score = 0;
unsigned long game_start = 0;

bool bullet_is_in_progress = false;
int bullet_possiton[2];
unsigned long bullet_last_move = 0;
unsigned long bullet_speed = 100;

bool enemies_array[5] = {false,false,false,false,false};//{false,true,true,true,true};//
long randNumber;
int enemies_possiton[5][2] = {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}};
unsigned long enemies_last_move[5] = {0,0,0,0,0};
unsigned long enemies_overall_last_move = 0;
unsigned long enemies_speed = 200;


//char serial_message[MAX_MESSAGE_LENGTH] = ""; //w - UP, s - Down, f - Fire
char message[MAX_MESSAGE_LENGTH] = ""; //w - UP, s - Down, f - Fire

/*
Commands:
Up
Down
Fire
*/
// Arduino pin numbers
const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 1; // analog pin connected to X output




void setup(){
  Serial.begin(9600);
  lcd.begin();
  //Creating custom characters in CG-RAM
  lcd.createChar(1 , c1);
  lcd.createChar(2 , c2);
  lcd.createChar(3 , c3);

  //initiate random
  randomSeed(analogRead(0));

  //joystick initiation 
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH); //default value is 1


  //Starter screen of the game
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Starship game");
  lcd.setCursor(0,1);
  lcd.print(char(1));
  lcd.print(" Press any key to start");

}


void loop(){

  //Check to see if anything is available in the serial receive buffer
  while (Serial.available() > 0){
    //Create a place to hold the incoming message
    //static char message[MAX_MESSAGE_LENGTH];
    static unsigned int message_pos = 0;
    
    //Read the next available byte in the serial receive buffer
    char inByte = Serial.read();
    
    //Message coming in (check not terminating character) and guard for over message size
    if ( inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH - 1) ){
      //Add the incoming byte to our message
      message[message_pos] = inByte;
      message_pos++;
    }else{//Full message received...
      //Add null character to string
      message[message_pos] = '\0';
      
      //Print the message (or do other things)
      Serial.print("[[");
      Serial.print(message);
      Serial.println("]]");
      print_array_to_serial();
      
      //Reset for the next message
      message_pos = 0;
    }
  }

  //Joystick input to commands:
  if(digitalRead(SW_pin)==LOW){
    message[0] = 'f';
  }
  if(analogRead(X_pin)>612){
    message[0] = 'w';
  }
  if(analogRead(X_pin)<412){
    message[0] = 's';
  }

  


  //Start game
  if (game_is_in_progress==false && (message[0] == 'w' || message[0] == 's' || message[0] == 'f')){
    game_is_in_progress = true;
    game_start = millis();
  }

  if(game_is_in_progress){

    //Processing input
    if(message[0] == 'w'){ // Up command
      starship_possiton = 0;
      /*lcd_array[0][0] = "}";
      lcd_array[1][0] = " ";*/
    }else if(message[0] == 's'){ // Down command
      starship_possiton = 1;
      /*lcd_array[1][0] = "}";
      lcd_array[0][0] = " ";*/
    }else if(message[0] == 'f' && bullet_is_in_progress == false){ //Fire command
      bullet_possiton[0] = starship_possiton;
      bullet_possiton[1] = 1;
    
      bullet_is_in_progress = true;
      bullet_last_move = millis();
//      Serial.println(bullet_last_move);
    }

    
    //moving bullet
    if(bullet_is_in_progress && bullet_last_move+bullet_speed <= millis()){
      
      
      if(bullet_possiton[1] != 15){
        Serial.println("moving bullet");
        bullet_last_move = millis();
        bullet_possiton[1] = bullet_possiton[1]+1;
      }else if(bullet_possiton[1] == 15){
        bullet_possiton[1] = -1;
        bullet_is_in_progress = false;
      }
      /*
      for(int i=0;i<2;i++){
        for(int j=0;j<16;j++){
          if(lcd_array[i][j] == ">" && j != 15){
            Serial.println("moving bullet");
            bullet_last_move = millis();
            lcd_array[i][j+1] = ">";
            lcd_array[i][j] = " "; 
            //print_array_to_serial();
            break;
          }else if(lcd_array[i][j] == ">" && j == 15){
            lcd_array[i][j] = " "; 
            bullet_is_in_progress = false;
          }
          
        }
      }*/
    }

    
    
    //Enemies initiation
    if((enemies_array[0]==false || enemies_array[1]==false || 
       enemies_array[2]==false || enemies_array[3]==false || enemies_array[4]==false) &&
       enemies_overall_last_move+enemies_speed*3 <= millis() ){
      // print a random number from 0 to 6
      randNumber = random(0, 6);
//      Serial.print("randNumber: ");
//      Serial.println(randNumber);
      if(randNumber==0 || randNumber==1){
//        Serial.print("Enemies initiation: "); Serial.println(randNumber);
        for(int i=0; i<5; i++){
          if(enemies_array[i]==false){
            lcd_array[randNumber][15]="<";
            enemies_array[i]=true;
            enemies_possiton[i][0] = randNumber;
            enemies_possiton[i][1] = 15;
            enemies_last_move[i] = millis();
            enemies_overall_last_move = millis();
            
//            Serial.print("enemies_possiton[i][0],[1]: ");
//            Serial.print(i);Serial.print(", ");
//            Serial.print(enemies_possiton[i][0]);Serial.print(", ");       
//            Serial.println(enemies_possiton[i][1]);
            
            
            break;
          }
        }
      }
    }

    


    //moving enemies
    
    for(int i=0; i<5; i++){
      if(enemies_array[i]==true && enemies_last_move[i]+enemies_speed <= millis()){
        enemies_possiton[i][1] = enemies_possiton[i][1] - 1;
        enemies_last_move[i] = millis();
      }
      //if enemy passed through starship
      if(enemies_possiton[i][1]==-1){
        enemies_array[i]=false;
      }

    }
    

    //Update lcd_array and check crushes
    for(int i=0;i<2;i++){
      
      for(int j=0;j<16;j++){

        if(game_is_in_progress){
          lcd_array[i][j] = " ";//by default all cells are blank
          
          
          //drawing starship
          if(starship_possiton==i && j==0){
            lcd_array[i][j] = "}";
          }
  
          //drawing bullet
          if(bullet_is_in_progress == true && bullet_possiton[0] == i && 
          bullet_possiton[1] == j){
            lcd_array[i][j] = ">";
          }
  
          //drawing enemies
          for(int k=0; k<5; k++){
            if(enemies_array[k]==true && enemies_possiton[k][0] == i 
            && enemies_possiton[k][1] == j){
              lcd_array[i][j]="<";
            }
          }
  
          //bullet enemy crush
          for(int k=0; k<5; k++){
            if(bullet_is_in_progress == true && bullet_possiton[0] == i && 
            bullet_possiton[1] == j &&
            ((enemies_array[k]==true && enemies_possiton[k][0] == i 
            && enemies_possiton[k][1] == j) || 
            (enemies_array[k]==true && enemies_possiton[k][0] == i 
            && enemies_possiton[k][1] == j-1) )
            ){
              Serial.println("bullet enemy crush");
              enemies_array[k] = false;
              enemies_possiton[k][0] = -1; 
              enemies_possiton[k][1] = -1;
              bullet_is_in_progress = false;
              bullet_possiton[0] = -1;
              bullet_possiton[1] = -1;
              
              lcd_array[i][j]=" ";
            }
            
          }
          
          //starship enemy crush
          if(j==0 && starship_possiton==i){
            for(int k=0; k<5; k++){
              if(enemies_array[k]==true && enemies_possiton[k][0] == i 
              && enemies_possiton[k][1] == j){
                Serial.println("starship enemy crush");
                //Game Over. Your score. High Score
                game_score = millis() - game_start;
                
                //need to reset all game values
                starship_possiton = 0;
                game_is_in_progress = false;
                bullet_is_in_progress = false;

                for(int z=0; z<5; z++){
                  enemies_array[z] = false;
                  enemies_possiton[z][0] = -1;
                  enemies_possiton[z][1] = -1;
                }
                
                
                enemies_speed = 200;
                message[MAX_MESSAGE_LENGTH] = ""; //w - UP, s - Down, f - Fire

                break;
            }
            
          }
          }
        }

        
      }
    }
    
    //Printing game to lcd
    for(int i=0;i<2;i++){
      lcd.setCursor(0,i);
      for(int j=0;j<16;j++){
        
        if(lcd_array[i][j] == "}"){
          lcd.print(char(1));
        }else if(lcd_array[i][j] == "<"){
          lcd.print(char(2));
        }else if(lcd_array[i][j] == ">"){
          lcd.print(char(3));
        }else{
          lcd.print(lcd_array[i][j]);
        }
        
      }
    }
    
  }else{
    if(game_score!=0){
      EEPROM.get(0, game_start);
      
      Serial.print("High score: ");
      Serial.println(game_start);
      Serial.print("Score: ");
      Serial.println(game_score);
      
      
      //Game over screen
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Record: ");
      lcd.print(game_start);
      
      lcd.setCursor(0,1);
      lcd.print("Score:  ");
      lcd.print(game_score);
      if(game_score > game_start){
         EEPROM.put(0, game_score);
      }
      game_score = 0;//reset game score for next game
    }
  }
  

  delay(50);
  message[0] = ' '; //reset command

}


void print_array_to_serial(){
  //Printing game to Serial Monitor:
  Serial.println("lcd_array:");
  for(int i=0;i<2;i++){
    for(int j=0;j<16;j++){
      Serial.print(lcd_array[i][j]);
    }
    Serial.println("");
  }
}
