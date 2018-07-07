//#include <FastLED.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

//#define NUM_LEDS 32
//#define DATA_PIN 15
#define PIN 15
#define encoderButton 4 // SW
#define outputB 5 // DT
#define outputA 2 // CLK
#define NEOPIXELNUM 32
#define BRIGHTNESS 1
#define EEPROM_OFFSET 50

//CRGB leds[NUM_LEDS];

//Encoder variables
int aState;
int aLastState;
int counter = 0;
int counterInit = 0;
int colorCounter = 0;
int colorCounterInit = 0;
boolean buttonState = false;
boolean lastButtonState = false;
boolean chooseColorState = false;
boolean chooseColorStateLast = false;

// LED Ring variables
int analog5 = 0;
int posLED = 0;
int posLEDInit = 0;
int colorSearchRow = 0;
int colorSearchRowInit = 0;
int colorPalette = 0;
byte colors[10][3] = {
  {5, 0, 0}, // red
  {5, 1, 0}, // orange
  {5, 5, 0}, // yellow
  {0, 5, 0}, // green
  {0, 5, 2}, // cyan
  {0, 0, 5}, // blue
  {1, 0, 5}, // indigo
  {5, 0, 5}, // violet
  {5, 5, 5}, // white
  {0, 0, 0}, // off
};

byte setColors [32][3] = {
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEOPIXELNUM, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(analog5, INPUT);
  pinMode(encoderButton, INPUT);
  pinMode(outputB, INPUT);
  pinMode(outputA, INPUT);
  //FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  aLastState = digitalRead(outputA);
  strip.setPixelColor(posLED, strip.Color(50, 0, 0));
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.print("Color palette array has rows = ");
  colorPalette = sizeof(colors) / sizeof(colors[0]);
  Serial.println(colorPalette);
}

// Set color of individual neopixels on the Neopixel Feather Wing
void loop() {
  // put your main code here, to run repeatedly:
  button();           // detects button presses
  if (chooseColorState) {
    //chooseColor();  // does nothing
    encoderColor();   // detects increments by the rotary encoder 
    colorMatrix();    // increments through the color palette
    //button();       
  } else {
    encoder();
    matrix();
  }
  loadColors();
  strip.show();
}

// Use colorSearchRow and the colors[][] array to set neopixel colors
void matrix() {
  if (abs(counter - counterInit) == 2) {
    //Serial.println("entered");
    if (counter > counterInit) {
      //Serial.println("counter > counterInit");
      // encoder counter
      counterInit = counter;
      posLEDInit = posLED;
      positionLED(1);
      strip.setPixelColor(posLED, strip.Color(colors[
                          colorSearchRow][0]*BRIGHTNESS,
                          colors[colorSearchRow][1]*BRIGHTNESS,
                          colors[colorSearchRow][2]*BRIGHTNESS));
      if (setColors[posLEDInit][0] == 0 && 
          setColors[posLEDInit][1] == 0 && 
          setColors[posLEDInit][2] == 0) {
        strip.setPixelColor(posLEDInit, strip.Color(0, 0, 0));
      }
    } else {
      counterInit = counter;
      posLEDInit = posLED;
      positionLED(-1);
      strip.setPixelColor(posLED, strip.Color(colors[
                          colorSearchRow][0]*BRIGHTNESS,
                          colors[colorSearchRow][1]*BRIGHTNESS,
                          colors[colorSearchRow][2]*BRIGHTNESS));
      if (setColors[posLEDInit][0] == 0 && 
          setColors[posLEDInit][1] == 0 && 
          setColors[posLEDInit][2] == 0) {
        strip.setPixelColor(posLEDInit, strip.Color(0, 0, 0));
      }
    }
  }
}

void colorMatrix() {
  if (abs(colorCounter - colorCounterInit) == 2) {
    //Serial.println("entered");
    if (colorCounter > colorCounterInit) {
      //Serial.println("counter > counterInit");
      // encoder counter
      colorCounterInit = colorCounter;
      colorSearchRowInit = colorSearchRow;
      positionColor(1);
      strip.setPixelColor(posLED, strip.Color(colors[
                          colorSearchRow][0]*BRIGHTNESS,
                          colors[colorSearchRow][1]*BRIGHTNESS,
                          colors[colorSearchRow][2]*BRIGHTNESS));
    } else {
      colorCounterInit = colorCounter;
      colorSearchRowInit = colorSearchRow;
      positionColor(-1);
      strip.setPixelColor(posLED, strip.Color(colors[
                          colorSearchRow][0]*BRIGHTNESS,
                          colors[colorSearchRow][1]*BRIGHTNESS,
                          colors[colorSearchRow][2]*BRIGHTNESS));
    }
  }
}

// The three addresses used store R, G, and B values respectively.
void saveColor() {
    /*
    EEPROM.put(EEPROM_OFFSET, RED_VALUE);
    EEPROM.put(EEPROM_OFFSET, GREEN_VALUE);
    EEPROM.put(EEPROM_OFFSET, BLUE_VALUE);


    */
    Serial.print("Neopixel # :");
    Serial.print(posLED);
    Serial.println(" saved");
}

void button() {
  delay(5);
  int buttonVal = analogRead(encoderButton);
  if (buttonVal == 0) {
    lastButtonState = buttonState;
    buttonState = true;
    //saveColor();
  } else {
    buttonState = false;
    lastButtonState = buttonState;
  }
  if (lastButtonState != buttonState) {
    if (chooseColorState) {
      chooseColorState = false;
      setColors[posLED][0] = colors[colorSearchRow][0];
      setColors[posLED][1] = colors[colorSearchRow][1];
      setColors[posLED][2] = colors[colorSearchRow][2];
    } else {
      chooseColorState = true;
    }
    Serial.print("Color State = ");
    Serial.println(chooseColorState, DEC);
  } 
}

void encoder() {
  aState = digitalRead(outputA);
  if (aState != aLastState) {
    if (digitalRead(outputB) != aState) {
      counter --;
    } else {
      counter ++;
    }
    //Serial.print("Position: ");
    //Serial.println(counter);
  }
  aLastState = aState;
}

void encoderColor() {
  aState = digitalRead(outputA);
  if (aState != aLastState) {
    if (digitalRead(outputB) != aState) {
      colorCounter --;
    } else {
      colorCounter ++;
    }
    //Serial.print("Position: ");
    //Serial.println(counter);
  }
  aLastState = aState;
}

// Only lets the LED change position if enough time has passed
// Wait time indicated by the wait variable, which is dependent upon
// analog input
boolean duration(uint32_t timeStamp, uint32_t wait) {
  if ((millis() - timeStamp) > wait) {
    return true;
  }
  return false;
}

void positionColor(int change) {
  if (change == 1) {
    if (colorSearchRow == (colorPalette - 1)) {
      colorSearchRow = 0;
    } else {
      colorSearchRow = colorSearchRow + change;
    }
  } else {
    if (colorSearchRow == 0) {
      colorSearchRow = (colorPalette - 1);
    } else {
      colorSearchRow = colorSearchRow + change;
    }
  }
  Serial.println(colorSearchRow);
}

void positionLED(int change) {
  if (change == 1) {
    if (posLED == (NEOPIXELNUM -1)) {
      posLED = 0;
    } else {
      posLED = posLED + change;
    }
  } else {
    if (posLED == 0) {
      posLED = (NEOPIXELNUM - 1);
    } else {
      posLED = posLED + change;
    }
  }
  Serial.println(posLED);
}

void loadColors() {
  for(int i = 0; i < NEOPIXELNUM; i++) {
    strip.setPixelColor(i, strip.Color(setColors[i][0]*BRIGHTNESS, setColors[i][1]*BRIGHTNESS,
                        setColors[i][2]*BRIGHTNESS));
  }
  strip.setPixelColor(posLED, strip.Color(colors[
                          colorSearchRow][0]*BRIGHTNESS,
                          colors[colorSearchRow][1]*BRIGHTNESS,
                          colors[colorSearchRow][2]*BRIGHTNESS));
}

