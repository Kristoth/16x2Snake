#include <LiquidCrystal.h>
LiquidCrystal lcd(3, 4, 5, 6, 7, 9); // RS, EN, D4, D5, D6, D7



// Trzeci port mojego wonsza / Third port of my version of the Snake Game


const int pinInput = 2; // Button (PULLUP) pin
const int pinBuzzer = 8; // Buzzer pin

const unsigned int difficulty = 4; // How often the Snek moves, per second (max is 40);

// Uses a joystick connected to A0 and A1

// Click the button start, pause, unpause or reset the game




// Rozne debugi / Various debug functions
bool miniDebug = false;
bool fruitDebug = false;
bool debug = false;
bool debug_time = false;


// zmienne programowe / Program variables
boolean started = false;
boolean lost = false;
bool won = true;
bool paused = false;
bool pToggle = 0;
int anal0=500;
int anal1=500;

int posHead[2]={4,3};
int dir[3]={0,0};

byte snakeData[80][2];
int snake_len=4;

int moveCounter=-1;

int high = 0;
int last = 0;

unsigned char conv[8]= {0, 0, 0, 0, 0, 0, 0, 0};

unsigned char table[8][5];

int posFruit[2];
bool showFruit = 0;

int cycles = 1;

long time1 = 0;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);

  Serial.begin(9600);
  
  
  pinMode(pinInput, INPUT_PULLUP);
  pinMode(pinBuzzer, OUTPUT);


  if (difficulty > 40) cycles = 1;
  else cycles = 40 / difficulty; 
  

  //sendTable();


  resetGame();
  moveCounter=-1;
  
  
}

void loop()
{
  // put your main code here, to run repeatedly:

  if (debug_time) time1 = millis();
  
  if (started and !lost and !paused) moveCounter++;

  //if ((moveCounter >= cycles) and stateDebug) stateState();

  if (started and !lost and (moveCounter >= cycles)) tick();
  

  dealWithThePlayer();
  
  delay(25);

 

}

void tick(){
    moveCounter=0;
    bool impact=false;

    impact = ((posHead[0] == 0) and (dir[0]==-1)) or ((posHead[0] == 9) and (dir[0] == 1)) or ((posHead[1] == 0) and (dir[1] == -1)) + ((posHead[1] == 7) and (dir[1] == 1));
    if (debug) Serial.println("Wallcheck: "+String(impact));

    int i = 1;

    while (i<snake_len-1){
      if ((posHead[0] + dir[0]== snakeData[i][0] and posHead[1] == snakeData[i][1]) or (posHead[0] == snakeData[i][0] and posHead[1] + dir[1] == snakeData[i][1])) impact = impact + 1;
      i++;
    };
  if (debug) Serial.println("Bodycheck: "+String(impact));
  if (impact) defeat();
  else{
    posHead[0]+=dir[0];
    posHead[1]+=dir[1];



    if ((posHead[0] == posFruit[0]) and (posHead[1] == posFruit[1])){
        setFruit();
        snake_len++;
        if (snake_len == 80 and !won) victory();
        digitalWrite(pinBuzzer, HIGH);
        delay(50);
        digitalWrite(pinBuzzer,LOW);
        delay(50);
        digitalWrite(pinBuzzer,HIGH);
        delay(50);
        digitalWrite(pinBuzzer,LOW);
        
        moveCounter = 7;
        
      };

      if (miniDebug  && !debug and (posHead[0] == posFruit[0] and posHead[1] + dir[1] == posFruit[1])) {
        debug=1;
        debug_info();
        debug=0;
        
      }
      
    
    int i = (snake_len-1);
      
      while (i>=1){
        snakeData[i][0]=snakeData[i-1][0];
        snakeData[i][1]=snakeData[i-1][1];
        //snakeData[i][2]=snakeData[i-1][2];
        
        i--; 
      }


   
    snakeData[0][0] = posHead[0];
    snakeData[0][1] = posHead[1];

    project();

    render();
    
    debug_info();

    timer();

    
  };
};

void dealWithThePlayer(){
  // #############################################################################
  // Analiza wejscia uzytkownika / User input analisys


  
  if (!paused and (digitalRead(pinInput) == LOW) and !pToggle and started and !lost){
    pToggle=1;
    paused=1;
    if (miniDebug  && !debug){
      debug = 1;
      debug_info();
      debug = 0;
    }
  }

  if (paused and (digitalRead(pinInput) == LOW) and !pToggle){
    pToggle=1;
    paused=0;
  }

  if (digitalRead(pinInput)==HIGH && pToggle) pToggle=0;
  
  
  
  bool dirChange=0;
  if ((lost) and (digitalRead(pinInput)==LOW)){
    pToggle = 1;
    resetGame();
  }

  
  
  anal0=analogRead(A0);
  anal1=analogRead(A1);

  

  if ( anal0 < 300 and anal1 >= 300 and anal1 <= 700 and dir[2] != 3 and posHead[0] !=  0 and posHead[0] - 1 != snakeData[1][0] ){dir[0] = -1; dir[1] =  0; dir[2] = 3; dirChange = 1;}
  if ( anal0 > 700 and anal1 >= 300 and anal1 <= 700 and dir[2] != 1 and posHead[0] != 13 and posHead[0] + 1 != snakeData[1][0] ){dir[0] =  1; dir[1] =  0; dir[2] = 1; dirChange = 1;}
  
  if ( anal1 < 300 and anal0 >= 300 and anal0 <= 700 and dir[2] != 0 and posHead[1] !=  0 and posHead[1] - 1 != snakeData[1][1] ){dir[0] =  0; dir[1] = -1; dir[2] = 0; dirChange = 1;}
  if ( anal1 > 700 and anal0 >= 300 and anal0 <= 700 and dir[2] != 2 and posHead[1] !=  6 and posHead[1] + 1 != snakeData[1][1] ){dir[0] =  0; dir[1] =  1; dir[2] = 2; dirChange = 1;}
  


  if ((miniDebug or debug) and dirChange) Serial.println("Rotation is: "+String(dir[2]));
  

  
  if ((!started) and (dirChange)){
    if (debug) Serial.println("Game, start!");
    moveCounter = 0;
    started = 1;
    showFruit = 1;
    
    randomSeed(millis());
    setFruit();
    
    
  }
  if (dirChange) dirChange=0;

  // Koniec analizy wejscia uzytkownika / User Input analisys end here
  // ######################################################
}

void timer(){ // Debugowy pomiar czasu
  if (debug_time){
    long time2 = millis();

    Serial.print("Czas trwania jednego cyklu: ");
    Serial.print(time2 - time1);
    Serial.println("ms");
  }
}

void project(){ // rzucaj swoje wyrzuty sumienia na inne osoby
  
  bool snake_matrix_flat[20][4];
  

  for (int i = 0; i < 80; i++){
    snake_matrix_flat[i / 4][i % 4] = 0;
  };

  for (int i = 0; i < snake_len; i++){
    byte x = snakeData[i][0];
    byte y = snakeData[i][1];
    if (y < 4 ) snake_matrix_flat[x][y] = 1;
    else snake_matrix_flat[10 + x][y - 4] = 1;

  };
  
    if (posFruit[1] < 4 and showFruit) snake_matrix_flat[posFruit[0]][posFruit[1]] = 1;
    else if (showFruit) snake_matrix_flat[10 + posFruit[0]][posFruit[1] - 4] = 1;

  
  for (int i = 0; i < 20; i++){
    table[(i * 2)/ 5][(i * 2) % 5] = 0;
    
    table[(i * 2)/ 5][(i * 2) % 5] += snake_matrix_flat[i][0] * 1;
    table[(i * 2)/ 5][(i * 2) % 5] += snake_matrix_flat[i][0] * 2;

    table[(i * 2)/ 5][(i * 2) % 5] += snake_matrix_flat[i][1] * 4;
    table[(i * 2)/ 5][(i * 2) % 5] += snake_matrix_flat[i][1] * 8;

    table[(i * 2)/ 5][(i * 2) % 5] += snake_matrix_flat[i][2] * 16;
    table[(i * 2)/ 5][(i * 2) % 5] += snake_matrix_flat[i][2] * 32;

    table[(i * 2)/ 5][(i * 2) % 5] += snake_matrix_flat[i][3] * 64;
    table[(i * 2)/ 5][(i * 2) % 5] += snake_matrix_flat[i][3] * 128;

    table[((i * 2) + 1) / 5][((i * 2) + 1) % 5] = table[(i * 2)/ 5][(i * 2) % 5];
     
  };
}


void render(){ // Renderuje tablice na LCD
  
  
  for (int i = 0; i < 8; i++){
    convert(table[i]);
    lcd.createChar(byte(i), conv);
  }

  delay(2);

  
}


void convert(unsigned char from[5]){ // obrot danych z tabeli na znaki
  
  for (int i = 0; i < 8; i++){
    conv[i] = 0;
  
    char num = 0b1 << i;

    conv[i] += ((from[0] & num) >> i) * 16; 
    conv[i] += ((from[1] & num) >> i) * 8 ; 
    conv[i] += ((from[2] & num) >> i) * 4 ; 
    conv[i] += ((from[3] & num) >> i) * 2 ; 
    conv[i] += ((from[4] & num) >> i) * 1 ; 
  };

}








void sendTable(){ // debug - tablica gry
  if (debug){  

    Serial.println("Table:");
    for (int i = 0; i < 8; i++){
  
      sendChar(table[i]);
    };
  };
  
};

void sendChar(unsigned char what[5]){
  Serial.print("[");
    
    Serial.print(what[0]);
    Serial.print(", ");
    
    Serial.print(what[1]);
    Serial.print(", ");

    Serial.print(what[2]);
    Serial.print(", ");

    Serial.print(what[3]);
    Serial.print(", ");

    Serial.print(what[4]);
    Serial.println("],");

};

void resetGame(){
  Serial.println("(Re)-Set!");

  lost = false;
  started = false;
  won = false;
  moveCounter = 0;
  snake_len = 4;

  posHead[0] = 3;
  posHead[1] = 3;
  
  snakeData[0][0] = 3;
  snakeData[0][1] = 3;

  snakeData[1][0] = 4;
  snakeData[1][1] = 3;;
  
  snakeData[2][0] = 5;
  snakeData[2][1] = 3;
  
  snakeData[3][0] = 6;
  snakeData[3][1] = 3;
  
  snakeData[4][0] = 8;
  snakeData[4][1] = 3;

  dir[2] = 1;
  

  showFruit = 0;
  
  lcd.setCursor(5, 0);
  lcd.write(']');
  lcd.write(byte(0));
  lcd.write(byte(1));
  lcd.write(byte(2));
  lcd.write(byte(3));
  lcd.write('[');
  

  lcd.setCursor(5, 1);
  lcd.write(']');
  lcd.setCursor(6, 1);
  lcd.write(byte(4));
  lcd.write(byte(5));
  lcd.write(byte(6));
  lcd.write(byte(7));
  lcd.write('[');

  lcd.setCursor(0, 0);
  lcd.print("High:");


  lcd.setCursor(11, 0);
  lcd.print("Last:");

  scores();

  
  
  debug_info();

  project();
  render();

  
};

void defeat(){
  moveCounter=-1;
  lost=true;
  Serial.println("You Lose");    
  Serial.println("Score: "+String(snake_len - 4));
  
  debug_info();
  
  
  tone(pinBuzzer,247,100);
  delay(150);
  tone(pinBuzzer,233,100);
  delay(150);
  tone(pinBuzzer,220,100);
  delay(150);
  tone(pinBuzzer,208,100);
  delay(150);
  tone(pinBuzzer,196,100);

  last = snake_len - 4;
  high = max(high, last);
  
  scores();

}

void victory(){
  moveCounter=-1;
  lost=true;
  won=true;
  Serial.println(":-) You Win! :-)");    
  Serial.println("Score: "+String(snake_len-4));
  debug_info();
  tone(pinBuzzer,587,200);
  delay(250);
  tone(pinBuzzer,523,200);
  delay(250);
  tone(pinBuzzer,587,200);
  delay(250);
  tone(pinBuzzer,659,200);
  delay(250);
  tone(pinBuzzer,587,200);

  last = 99;
  high = 99;

  scores();
}


void scores(){
  lcd.setCursor(1, 1);
  if (high <= 9) lcd.write('0');
  lcd.print(high);

  lcd.setCursor(12, 1);
  if (last <= 9) lcd.write('0');
  lcd.print(last);

}

void debug_info(){
  if (debug or miniDebug){
    Serial.println("Snake lenght is: "+String(snake_len));
    int j=0;
    while (j<snake_len){
      Serial.println("Snake module "+String(j) +": "+String(snakeData[j][0])+", "+String(snakeData[j][1]));
      j++; 
    }
    Serial.println("\n");
      
  }

}



void setFruit(){
  if (fruitDebug) Serial.print("New fruit generated at: ");
  boolean conOut=0;
  int counter=0;
  while (!conOut and counter<1000){
    conOut=1;
    posFruit[0]=random(0,6)+random(0,5);
    posFruit[1]=random(0,8);

    if (posFruit[0] == posHead[0] and posFruit[1] == posHead[1]) conOut=0;
  
    int i=0;
    while (i<snake_len){
      if ((posFruit[0]==snakeData[i][0]) and (posFruit[1]==snakeData[i][1])) conOut=0;
      i++; 
    }
    counter++;
  }
  if (counter>=999){posFruit[0]=0,posFruit[1]=0;}
  if (fruitDebug) Serial.print(String(posFruit[0])+", "+String(posFruit[1]));
  if (fruitDebug) Serial.println(", "+String(counter)+"'th try");

}
