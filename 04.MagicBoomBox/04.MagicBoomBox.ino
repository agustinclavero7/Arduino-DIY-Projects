#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>


//For initial animation
#define FRAME_DELAY1  42
#define FRAME_COUNT1 (sizeof(frames) / sizeof(frames[0]));
#define FRAME_WIDTH1  64
#define FRAME_HEIGHT1 64
int frame = 0;

//Botones
#define PLAY_BUTTON 1
bool reproduciendo = true;

Adafruit_SSD1306 oledScreen(128,64,&Wire,4);
SoftwareSerial softSerial (0,1); // chequear pines UART para NANO, MEGA es 10 y 11.
DFRobotDFPlayerMini mp3Player;



void setup() {
  Wire.begin();
  oledScreen.begin (SSD1306_SWITCHCAPVCC,0x3C);
//Presentación
  oledScreen.clearDisplay();
  oledScreen.setTextColor(WHITE);
  initialAnimation();
}

void loop() {

}
