//https://github.com/NicoHood/HID/blob/master/src/HID-APIs/ConsumerAPI.h  
//https://www.arduino.cc/en/Reference/KeyboardModifiers
//https://github.com/arduino-libraries/Keyboard/blob/master/src/Keyboard.h

// Read the full tutorial at prusaprinters.org
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <HID-Project.h>

//Encoder
#define ENCODER_CLK A0 // Change A0 to, for example, A5 if you want to use analog pin 5 instead
#define ENCODER_DT A1
#define ENCODER_SW A2
//#define ENCODER_STEPS_PER_NOTCH    4   // Change this depending on which encoder is used
#define ENC_DOUBLECLICKTIME  600  // second click within 600ms
#define stepsPerNotch 10;

//Oled
#define OLED_RESET -1
Adafruit_SSD1306 display(128, 32, &Wire, OLED_RESET);
#define NumLed 6
boolean OledClear = true;
#define BLACK SSD1306_BLACK    ///< Draw 'off' pixels
#define WHITE SSD1306_WHITE    ///< Draw 'on' pixels
#include "Fonts/Roboto_Condensed_16.h" 
#include "Fonts/Roboto_Condensed_Bold_16.h" 
#include "Fonts/Roboto_Condensed_Light_9.h"  

// Menu
int menuitem = 1;                   // define cual menuitem es preseleccionado
int page = 1;                       // page 1 muestra el menu, page 2 muestra la vita de edicion
int lastMenuItem = 1;
String menuItem1 = "Modo";
String language[3] = { "Multimedia", "Gaming", "Meets" };
int selectedLanguage = 0;
boolean up = false;
boolean down = false;
boolean middle = false;

// Defining each pin
#define Top_Left 9         
#define Top_Center 8          
#define Top_Right 7         
#define Bottom_Left 6        
#define Bottom_Center 5       
#define Bottom_Right 4       
const int pinLed = LED_BUILTIN;

//Variable for modes
int estado;

ClickEncoder *encoder; // variable representing the rotary encoder
int16_t last, last_value_for_volume, value, value_for_volume; // variables for current and last rotation value

void setup() {
  Serial.begin(9600); // Opens the serial connection used for communication with the PC. 
  Consumer.begin(); // Initializes the media keyboard
  encoder = new ClickEncoder(ENCODER_DT, ENCODER_CLK, ENCODER_SW, 10); // Initializes the rotary encoder with the mentioned pins
  encoder->setAccelerationEnabled(false);
  
  pinMode (Top_Left , INPUT_PULLUP);
  pinMode (Top_Center, INPUT_PULLUP);
  pinMode (Top_Right, INPUT_PULLUP);
  pinMode (Bottom_Left, INPUT_PULLUP);
  pinMode (Bottom_Center, INPUT_PULLUP);
  pinMode (Bottom_Right, INPUT_PULLUP);

  last_value_for_volume = encoder->getValue();
  last = encoder->getValue();

  Timer1.initialize(1000); // Initializes the timer, which the rotary encoder uses to detect rotation
  Timer1.attachInterrupt(timerIsr); 
  

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
} 

void loop() {  
  drawMenu();
  ClickEncoder::Button b = encoder->getButton();

  if (b != ClickEncoder::Open) {
   switch (b) {
      case ClickEncoder::Clicked:
         middle=true;
        break;
    }
  }

  // ALTERNATE BETWEEN MODE AND MODE SELECTION
  if (middle){                                  // Cuando el boton del encoder es presionado dependendiendo de dond esté
    middle = false;
    if (page == 1){                             // Esto lleva a la pantalla de seleccion de modo
      page=2;
    }
    else if (page == 2){                        // Luego de seleccionar un modo esto devuelve al inicio
      page=1; 
    }
   } 


  else if (down && page == 2 ) {                //Seleccion del modo
    down = false;
    selectedLanguage--;
    if(selectedLanguage == -1)
    {
      selectedLanguage = 2;
    }
  }


  // ALL THAT HAPPENS ON MODE SELECTED
  if ( page == 1){
      // MULTIMEDIA //////////////////////////////////
      if (selectedLanguage == 0){ 

          if (!digitalRead(Top_Left)) {
            //digitalWrite(pinLed, HIGH);
            
            Consumer.write(HID_CONSUMER_MUTE);
            Serial.println("Mute");
            Keyboard.releaseAll();
            delay(500);
            //Keyboard.releaseAll();
            //delay(75);
            
            // Simple debounce
            //delay(300);
            //digitalWrite(pinLed, LOW);
          }
          
          if (!digitalRead(Top_Center)) {
            Keyboard.press(KEY_UP_ARROW);
            Serial.println("Up");
            Keyboard.releaseAll();
            delay(75);
          }     
          
          if (!digitalRead(Top_Right)) {
            Keyboard.press(KEY_F);
             Keyboard.releaseAll();
            delay(75);
          }
          
          if (!digitalRead(Bottom_Left)) {
          Keyboard.press(KEY_LEFT_ARROW);
          //Consumer.write(MEDIA_PREVIOUS);
          Serial.println("Rewind");
          Keyboard.releaseAll();
          delay(75);
          }
          
          if (!digitalRead(Bottom_Center)) {
          //Keyboard.press(MEDIA_PLAY_PAUSE);
          Consumer.write(MEDIA_PLAY_PAUSE);
          Serial.println("Down");
          Keyboard.releaseAll();
          delay(500);
          }
          
          if (!digitalRead(Bottom_Right)) {
          Keyboard.press(KEY_RIGHT_ARROW);
          Serial.println("Right");
          Keyboard.releaseAll();
          delay(75);
          }
    
          // VOLUME CONTROL ///////////////////////////
          volume_control();
      }
    
      // GAMING ///////////////////////////////////////
      else if (selectedLanguage == 1){          
    
          if (!digitalRead(Top_Left)) {
            //digitalWrite(pinLed, HIGH);     
            //Keyboard.press(KEY_UP_ARROW);
             Serial.println("_________");
            //Keyboard.releaseAll();
            //delay(75);         
            // Simple debounce
            //delay(300);
            //digitalWrite(pinLed, LOW);
          }
          
          if (!digitalRead(Top_Center)) {
            Keyboard.press(KEY_UP_ARROW);
            Serial.println("Up");
            Keyboard.releaseAll();
            delay(75);
          }
          
          if (!digitalRead(Top_Right)) {
            //Keyboard.press(KEY_UP_ARROW);
            //Serial.println("_________");
            //Keyboard.releaseAll();
            //delay(75);
          }
          
          if (!digitalRead(Bottom_Left)) {
          Keyboard.press(KEY_LEFT_ARROW);
          Serial.println("Left");
          Keyboard.releaseAll();
          delay(75);
          }
          
          if (!digitalRead(Bottom_Center)) {
          Keyboard.press(KEY_DOWN_ARROW);
          Serial.println("Down");
          Keyboard.releaseAll();
          delay(75);
          }
          
          if (!digitalRead(Bottom_Right)) {
          Keyboard.press(KEY_RIGHT_ARROW);
          Serial.println("Right");
          Keyboard.releaseAll();
          delay(75);
          }
    
          // VOLUME CONTROL ///////////////////////////
          volume_control();
      }

      // GOOGLE MEET //////////////////////////////////
      else if (selectedLanguage == 2){          
    
          if (!digitalRead(Top_Left)) {
             Serial.println("_________");
          }
          
          if (!digitalRead(Top_Center)) {
            Keyboard.press(KEY_UP_ARROW);
            Serial.println("Up");
            Keyboard.releaseAll();
            delay(75);
          }
          
          if (!digitalRead(Top_Right)) {
             Keyboard.press(KEY_F11);
             Serial.println("Fullscreen");
             Keyboard.releaseAll();
             delay(75);
            //Keyboard.press(KEY_UP_ARROW);
            //Serial.println("_________");
            //Keyboard.releaseAll();
            //delay(75);
          }
          
          if (!digitalRead(Bottom_Left)) {
          Keyboard.press(KEY_LEFT_CTRL);
          delay(75);
          Keyboard.press('d');
          Serial.println("Mute on/off");
          Keyboard.releaseAll();
          delay(75);
          }
          
          if (!digitalRead(Bottom_Center)) {
          Keyboard.press(KEY_LEFT_CTRL);
          delay(10);
          Keyboard.press('e');
          Serial.println("Camera on/off");
          Keyboard.releaseAll();
          delay(75);
          }
          
          if (!digitalRead(Bottom_Right)) {
          Keyboard.press(KEY_RIGHT_ARROW);
          Serial.println("Right");
          Keyboard.releaseAll();
          delay(75);
          }
    
          // VOLUME CONTROL ///////////////////////////
          volume_control();
      }
  } 

  // ALL THATS HAPPENS ON MODE SELECTION
  else if(page == 2){
    navigate_modes();
   }
  
  display.display();
  delay(10); // Wait 10 milliseconds, we definitely don't need to detect the rotary encoder any faster than that
}

void timerIsr() {
  encoder->service();
}

void navigate_modes(){
  value += encoder->getValue();
  if (value/2 > last) {
    last = value/2;
    down = true;
    delay(150);
  }
  else if (value/2 < last) {
    last = value/2;
    up = true;
    delay(150);
  }
}

// MENU
void drawMenu(){
  if (page==1){   
      display.clearDisplay();
//      display.setCursor(5, 7);
//      display.setFont(&Roboto_Condensed_Light_9);
//      display.println("Modo");



//      display.drawPixel(21,0,SSD1306_WHITE);
//      display.drawPixel(23,5,SSD1306_WHITE);
//      display.drawPixel(23,10,SSD1306_WHITE);
//      display.drawPixel(23,15,SSD1306_WHITE);
//      display.drawPixel(23,20,SSD1306_WHITE);
//      display.drawPixel(23,25,SSD1306_WHITE);
//      display.drawPixel(23,30,SSD1306_WHITE);
//      display.drawPixel(25,31,SSD1306_WHITE);

//      display.drawRect(25, 17, 5, 15, SSD1306_WHITE);

//      display.drawLine(58, 17, 58, 31, SSD1306_WHITE);
//      display.drawLine(56, 24, 60, 24, SSD1306_WHITE);
//      display.drawLine(91, 17, 91, 31, SSD1306_WHITE);
//      display.drawLine(89, 24, 93, 24, SSD1306_WHITE);


      
      display.fillRoundRect(0, 0, 16, 16, 2, SSD1306_WHITE);
      display.setCursor(4, 14);
      display.setFont(&Roboto_Condensed_Bold_16);
      display.setTextColor(SSD1306_BLACK);
      display.println(selectedLanguage + 1);

      display.setCursor(24, 14);
      display.setFont(&Roboto_Condensed_16);
      display.setTextColor(SSD1306_WHITE);
      display.println(language[selectedLanguage]);
  }
  if (page==2){    
   displayStringMenuPage(menuItem1, language[selectedLanguage]);      
  }
}

void displayMenuItem(String item, int position, boolean selected){
    if(selected){
      display.setTextColor(BLACK, WHITE);
    }
    else{
      display.setTextColor(BLACK, WHITE);
    }
    display.setCursor(0, position);
    display.print(">"+item);
}

void displayStringMenuPage(String menuItem, String value){      //Titulo y valor a modificar en vista de edicion
    display.clearDisplay();
    display.setTextColor(WHITE, BLACK);
    display.setCursor(5, 7);
    display.setFont(&Roboto_Condensed_Light_9);
    display.print("Seleccione Modo");
    display.setTextColor(WHITE, BLACK);
    display.setCursor(5, 26);
    display.setFont(&Roboto_Condensed_16);
    display.print(value);
}

void volume_control(){
  // VOLUME CONTROL ///////////////////////////
  value_for_volume += encoder->getValue();
  //stepsPerNotch = 10;
  if (value_for_volume != last_value_for_volume) { // New value is different than the last one, that means to encoder was rotated
    if(last_value_for_volume<value_for_volume) // Detecting the direction of rotation
      Consumer.write(MEDIA_VOLUME_UP); // Replace this line to have a different function when rotating counter-clockwise
      else
      Consumer.write(MEDIA_VOLUME_DOWN); // Replace this line to have a different function when rotating clockwise
    last_value_for_volume = value_for_volume; // Refreshing the "last" varible for the next loop with the current value
    Serial.print("Volumen: "); // Text output of the rotation value used manily for debugging (open Tools - Serial Monitor to see it)
    Serial.println(value_for_volume);
  }  
}

void brightness_control(){
  // BRIGHTNESS CONTROL ///////////////////////////
//  value += encoder->getValue();
//  if (value != last) { // New value is different than the last one, that means to encoder was rotated
//    if(last<value) // Detecting the direction of rotation
//      Consumer.write(CONSUMER_BRIGHTNESS_UP); // Replace this line to have a different function when rotating counter-clockwise
//      else
//      Consumer.write(CONSUMER_BRIGHTNESS_DOWN); // Replace this line to have a different function when rotating clockwise
//    last = value; // Refreshing the "last" varible for the next loop with the current value
//    Serial.print("Encoder Value: "); // Text output of the rotation value used manily for debugging (open Tools - Serial Monitor to see it)
//    Serial.println(value);
//  }  
}



///////////////////
void unused_code(){

  /*
    This is just a long comment
    Here are some fun functions you can use to replace the default behaviour 
    Consumer.write(CONSUMER_BRIGHTNESS_UP);
    Consumer.write(CONSUMER_BRIGHTNESS_DOWN);
    Consumer.write(CONSUMER_BROWSER_HOME);
    Consumer.write(CONSUMER_SCREENSAVER);
    Consumer.write(HID_CONSUMER_AL_CALCULATOR); //launch calculator :)
    Consumer.write(HID_CONSUMER_AC_ZOOM_IN);
    Consumer.write(HID_CONSUMER_AC_SCROLL_UP);
    CONSUMER_SLEEP = 0x32,

    FULL LIST CAN BE FOUND HERE:
    https://github.com/NicoHood/HID/blob/master/src/HID-APIs/ConsumerAPI.h
*/
  
  // This next part handles the rotary encoder BUTTON
  //  ClickEncoder::Button b = encoder->getButton(); // Asking the button for it's current state
  //  if (b != ClickEncoder::Open) { // If the button is unpressed, we'll skip to the end of this if block
  //    //Serial.print("Button: "); 
  //    //#define VERBOSECASE(label) case label: Serial.println(#label); break;
  //    switch (b) {
  //      case ClickEncoder::Clicked: // Button was clicked once
  //        Consumer.write(MEDIA_PLAY_PAUSE); // Replace this line to have a different function when clicking button once
  //      break;      
  //       
  //       case ClickEncoder::DoubleClicked: // Button was double clicked
  //         Consumer.write(MEDIA_NEXT); // Replace this line to have a different function when double-clicking
  //      break;       
  //    }
  //  }  
}
        
