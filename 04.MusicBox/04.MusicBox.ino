#include <Wire.h>             // I2C pins: nano → SDA A4, SCL A5
                              //           mega → SDA 20, SCL 21
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

//Oled Screen
#define ANIMATION_DELAY   42
#define ANIMATION_COUNT  (sizeof(animationSequence) / sizeof(animationSequence[0]))
#define DURATION     50 
byte frame = 0;

//DFplayer
#define RX_PIN      7 // Si se usan con la biblio softserial, no usar 0 y 1, ya que se utilian para Serial y
                      // el programador
#define TX_PIN      6
#define PLAY_BUTTON 3
#define FW_BUTTON   2
#define RW_BUTTON   4
#define VOL_PIN     A0
#define BAT_PIN     A1

//Timers
unsigned long bateryTimer = 0;
unsigned long bateryDelay = 50000;
unsigned long debounceTimer = 0;
byte debounceDelay = 300;
unsigned long readerTimer = 0;
int readerDelay = 1000;
//Read files and states
bool reproduciendo = true;
bool isBateryOk = true;
bool fwPress = false;
bool rwPress = false;
bool playPress = false;
bool rdmPRess = false;
uint16_t currentPoteValue = 0;
uint16_t prevPoteValue = 0;
byte voLevel;
int totalFiles;
int currentFile;
uint8_t message;

Adafruit_SSD1306 oledScreen(128,64,&Wire,4);
SoftwareSerial softSerial (RX_PIN,TX_PIN);
DFRobotDFPlayerMini mp3Player;

const byte PROGMEM initialAnimation[6][512] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,1,0,0,128,1,0,0,128,1,0,0,128,1,0,0,128,1,0,0,128,1,0,8,128,1,16,8,136,17,16,8,136,17,16,8,136,17,16,136,137,145,17,136,137,145,17,136,137,145,17,136,137,145,17,8,136,17,16,8,136,17,16,8,136,17,16,8,128,1,16,0,128,1,0,0,128,1,0,0,128,1,0,0,128,1,0,0,128,1,0,0,128,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,1,0,0,128,1,0,0,128,1,16,0,136,1,16,0,136,1,16,8,136,1,16,8,136,17,17,136,137,145,17,136,137,145,17,136,137,145,17,136,137,145,17,136,137,145,17,136,137,145,17,8,136,17,17,8,136,17,16,0,136,1,16,0,136,1,16,0,128,1,16,0,128,1,0,0,128,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,128,9,128,1,128,9,128,17,128,9,128,17,128,9,128,17,128,9,128,17,136,137,145,17,136,137,145,17,136,137,145,17,136,137,145,17,136,137,145,17,136,137,145,17,136,137,145,17,136,137,145,17,128,9,128,17,128,9,128,17,128,9,128,17,128,9,128,17,128,9,128,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,16,0,8,0,16,0,8,0,16,0,136,1,144,1,136,1,144,1,136,129,145,1,136,129,145,1,136,137,145,17,136,137,145,17,136,137,145,17,136,137,145,17,136,137,145,17,136,137,145,17,136,129,145,1,136,129,145,1,136,1,144,1,136,1,144,1,8,0,16,0,8,0,16,0,8,0,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,16,0,8,0,16,0,8,0,16,0,8,128,17,0,8,128,17,0,136,128,17,1,136,129,145,17,136,137,145,17,136,137,145,17,136,137,145,17,136,137,145,17,136,137,145,17,136,137,145,17,136,129,145,17,136,128,17,1,8,128,17,0,8,128,17,0,8,0,16,0,8,0,16,0,8,0,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,1,0,0,128,1,0,0,128,1,0,0,128,1,0,8,128,1,0,8,128,17,16,8,136,17,16,8,136,17,16,136,137,145,17,136,137,145,17,136,137,145,17,136,137,145,17,136,137,145,17,136,137,17,17,8,136,17,16,8,136,17,16,8,128,17,16,8,128,1,0,0,128,1,0,0,128,1,0,0,128,1,0,0,128,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

const byte PROGMEM casetteTape[6][512] = {
  {0,0,0,0,0,0,0,0,0,7,192,0,0,60,32,0,0,96,48,0,1,128,48,0,3,7,192,0,2,4,3,224,6,7,254,32,4,0,0,48,4,0,0,16,127,255,255,254,104,127,254,22,104,31,248,22,108,0,0,54,104,240,0,22,107,252,7,150,110,14,24,246,108,2,50,54,108,243,39,182,105,153,108,150,105,25,104,214,105,153,108,150,108,243,39,182,100,2,48,102,102,4,31,198,97,248,7,134,96,96,0,6,96,0,0,6,127,255,255,254,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,7,192,0,0,60,32,0,0,96,48,0,1,128,48,0,3,7,192,0,2,4,3,224,6,7,254,32,4,0,0,48,4,0,0,16,127,255,255,254,104,127,254,22,104,31,248,22,108,0,0,54,104,240,0,22,107,252,7,150,110,14,24,246,108,2,50,54,108,243,39,182,105,153,108,150,105,25,104,214,105,153,108,150,108,243,39,182,100,2,48,102,102,4,31,198,97,248,7,134,96,96,0,6,96,0,0,6,127,255,255,254,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,1,192,0,0,31,224,0,0,112,16,0,1,192,48,0,1,1,224,0,2,3,0,0,6,1,255,224,4,0,112,48,4,0,0,16,127,255,255,254,104,127,254,22,104,31,248,22,108,0,0,54,104,240,0,22,107,252,7,150,110,14,24,246,108,2,50,54,108,243,39,182,105,153,108,150,105,25,104,214,105,153,108,150,108,243,39,182,100,2,48,102,102,4,31,198,97,248,7,134,96,96,0,6,96,0,0,6,127,255,255,254,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,248,0,128,6,7,1,192,12,1,246,32,127,255,255,254,104,127,254,22,104,31,248,22,108,0,0,54,104,240,0,22,107,252,7,150,110,14,24,246,108,2,50,54,108,243,39,182,105,153,108,150,105,25,104,214,105,153,108,150,108,243,39,182,100,2,48,102,102,4,31,198,97,248,7,134,96,96,0,6,96,0,0,6,127,255,255,254,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,127,255,255,254,108,127,254,22,104,31,248,22,108,0,0,54,104,240,0,22,107,252,7,150,110,14,24,246,108,2,48,54,108,243,39,182,105,153,109,150,105,25,104,214,105,153,109,150,108,243,39,182,100,2,48,102,102,4,31,198,97,248,7,134,96,96,0,6,96,0,0,6,127,255,255,254,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,127,255,255,254,108,127,254,22,104,31,248,22,108,0,0,54,104,240,0,22,107,252,7,150,110,14,24,246,108,2,48,54,108,243,39,182,105,153,109,150,105,25,104,214,105,153,109,150,108,243,39,182,100,2,48,102,102,4,31,198,97,248,7,134,96,96,0,6,96,0,0,6,127,255,255,254,0,0,0,0,0,0,0,0},
};  

const byte PROGMEM bateryIcon[6][512] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,255,255,240,48,0,0,24,37,40,0,24,45,40,0,8,45,40,0,8,45,40,0,14,45,40,0,6,45,40,0,6,45,40,0,6,45,40,0,6,45,40,0,14,45,40,0,8,45,40,0,8,37,40,0,24,48,0,0,24,31,255,255,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,255,255,240,48,0,0,24,37,40,0,24,45,40,0,8,45,40,0,8,45,40,0,14,45,40,0,6,45,40,0,6,45,40,0,6,45,40,0,6,45,40,0,14,45,40,0,8,45,40,0,8,37,40,0,24,48,0,0,24,31,255,255,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,255,255,240,48,0,0,24,37,32,0,24,45,40,0,8,45,40,0,8,45,40,0,14,45,40,0,6,45,40,0,6,45,40,0,6,45,40,0,6,45,40,0,14,45,40,0,8,45,40,0,8,37,32,0,24,48,0,0,24,31,255,255,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,255,255,240,48,0,0,24,37,32,0,24,45,32,0,8,45,32,0,8,45,32,0,14,45,32,0,6,45,32,0,6,45,32,0,6,45,32,0,6,45,32,0,14,45,32,0,8,45,32,0,8,37,32,0,24,48,0,0,24,31,255,255,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,255,255,240,48,0,0,24,37,32,0,24,45,32,0,8,45,32,0,8,45,32,0,14,45,32,0,6,45,32,0,6,45,32,0,6,45,32,0,6,45,32,0,14,45,32,0,8,45,32,0,8,37,32,0,24,48,0,0,24,31,255,255,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,255,255,240,48,0,0,24,37,32,0,24,45,32,0,8,45,32,0,8,45,32,0,14,45,32,0,6,45,32,0,6,45,32,0,6,45,32,0,6,45,32,0,14,45,32,0,8,45,32,0,8,37,32,0,24,48,0,0,24,31,255,255,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

const byte PROGMEM animationSequence[] = {0,0,1,2,3,4,5,0,0};

void eqAnimation(const byte PROGMEM frames[][512], const byte PROGMEM sequence[], byte frameCount, int frameWidth, int frameHeight, int frameDelay,int xpos, int ypos){
  oledScreen.clearDisplay();
  oledScreen.setTextColor(WHITE);
  byte frameIndex = pgm_read_byte(&sequence[frame]);
  oledScreen.drawBitmap(xpos, ypos, frames[frameIndex], frameWidth, frameHeight, 1);
  oledScreen.setCursor(30, 55);
  oledScreen.setTextSize(1);
  oledScreen.print(F("Iniciando..."));
  oledScreen.display();
  frame = (frame + 1) % frameCount;
  delay(frameDelay);
}

void casetteAnimation(const byte PROGMEM frames[][512], const byte PROGMEM sequence[], byte frameCount, int frameWidth, int frameHeight, int frameDelay,int xpos, int ypos){
  oledScreen.clearDisplay();
  oledScreen.setTextColor(WHITE);
  byte frameIndex = pgm_read_byte(&sequence[frame]);
  oledScreen.drawBitmap(xpos, ypos, frames[frameIndex], frameWidth, frameHeight, 1);
  oledScreen.setCursor(0, 0);
  oledScreen.setTextSize(1);
  oledScreen.print(F("Cancion:"));
  oledScreen.print(currentFile);
  oledScreen.print("/Tot:");
  oledScreen.print(totalFiles);
  oledScreen.display();
  frame = (frame + 1) % frameCount;
  delay(frameDelay);
}

void bateryWarning (){
  int bateryLevel = constrain(map(analogRead(BAT_PIN),337,430,0,100),0,100);
  if (bateryLevel < 75){
    isBateryOk = false;
    //animation(bateryIcon,animationSequence,ANIMATION_COUNT,32,32,ANIMATION_DELAY,48, 16);
    char bateryText[30];
    sprintf(bateryText,"Nivel de bat: %d", bateryLevel);
    oledScreen.setCursor(50, 45);
    oledScreen.setTextSize(1);
    oledScreen.print(bateryText);
    oledScreen.display();
    mp3Player.advertise(2);
    mp3Player.sleep();
  }
  else{
    isBateryOk = true;
  }
}

void setup() {
  Wire.begin();
  softSerial.begin(9600);
  Serial.begin(115200);
  oledScreen.begin (SSD1306_SWITCHCAPVCC,0x3C);
  pinMode(PLAY_BUTTON,INPUT_PULLUP);
  pinMode(FW_BUTTON,INPUT_PULLUP);
  pinMode(RW_BUTTON,INPUT_PULLUP);
//Presentación
  if(!mp3Player.begin(softSerial,true,true)){
    oledScreen.setCursor(5,32);
    oledScreen.setTextSize(3);
    oledScreen.print(F("FAIL"));
    oledScreen.display();
    while(true);
  }
  delay(3000);
  currentPoteValue = analogRead(VOL_PIN);
  prevPoteValue = currentPoteValue;
  mp3Player.advertise(1);
  mp3Player.volume(15);
  for(byte count = 0; count < DURATION; count ++){
    eqAnimation(initialAnimation,animationSequence,ANIMATION_COUNT,32,32,ANIMATION_DELAY,45, 8);
  }
  totalFiles = mp3Player.readFileCounts();
  mp3Player.randomAll();
}

void loop() {
  unsigned long timeNow = millis();
  //Reproducción
  if(isBateryOk){
    if(reproduciendo == true){
      casetteAnimation(casetteTape,animationSequence,ANIMATION_COUNT,32,32,ANIMATION_DELAY,45,15);
    }
    //Leo estado de botones    
    if(timeNow - debounceTimer > debounceDelay){
      if(digitalRead(PLAY_BUTTON) == LOW){
        debounceTimer = timeNow;
        playPress = true;
      }
      if(digitalRead(FW_BUTTON) == LOW){
        debounceTimer = timeNow;
        fwPress = true;
      }
      if(digitalRead(RW_BUTTON) == LOW){
        debounceTimer = timeNow;
        rwPress = true;
      }
      currentPoteValue = analogRead(VOL_PIN);
      if(abs(currentPoteValue - prevPoteValue) > 5){
        debounceTimer = timeNow;
        voLevel = map(currentPoteValue,0,1023,0,30);
        mp3Player.volume(voLevel);
        prevPoteValue = currentPoteValue;
      } 
    }
    //Acciones de botones
    if(playPress == true && reproduciendo == true){
      reproduciendo = false;
      playPress = false;
      mp3Player.pause();
    }
    else if(playPress == true && reproduciendo == false){
      reproduciendo = true;
      playPress = false;
      mp3Player.start();
    }
    else if(fwPress == true && reproduciendo == true){
      fwPress = false;
      mp3Player.next();
    }
    else if(rwPress == true && reproduciendo == true){
      rwPress = false;
      mp3Player.previous();
    }
    //Lectura de N° de archivo
    if(timeNow- readerTimer > readerDelay){
      readerTimer = timeNow;
      currentFile = mp3Player.readCurrentFileNumber();
    }   
  } 
  //Monitoreo de bateria
  if(timeNow - bateryTimer > bateryDelay){
    bateryTimer += bateryDelay;
    bateryWarning();
  }
  //Manejar estados de error
  if (mp3Player.available()){
    message = mp3Player.readType();
    Serial.println(message);
  }
  switch (message){
    case DFPlayerError:{
      mp3Player.reset();
      delay(1000);
      mp3Player.start();
      reproduciendo = true;
      isBateryOk = true;
      break;
    }
    case TimeOut:{
      mp3Player.reset();
      delay(1000);
      mp3Player.start();
      reproduciendo = true;
      isBateryOk = true;
      break;
    }
    case WrongStack:{
      mp3Player.reset();
      delay(1000);
      mp3Player.start();
      reproduciendo = true;
      isBateryOk = true;
      break;
    }
    case DFPlayerCardRemoved:{ 
      oledScreen.clearDisplay();
      oledScreen.setTextColor(WHITE);
      oledScreen.setCursor(10, 30);
      oledScreen.setTextSize(2);
      oledScreen.print("Insertar SD!");
      oledScreen.display();
      while (true);
      break;
    }
    default:{
      break;
    }
  }
}
