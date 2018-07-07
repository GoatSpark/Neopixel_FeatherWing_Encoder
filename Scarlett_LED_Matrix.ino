#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 15
#define encoderButton 5 // SW
#define outputB 4 // DT
#define outputA 2 // CLK
#define NEOPIXELNUM 32
#define BRIGHTNESS 5

//Encoder variables
int aState;
int aLastState;
int counter = 0;
int counterInit = 0;
boolean buttonState = false;
boolean lastButtonState = false;

// LED Ring variables
int analog5 = 0;
int posLED = 0;
int posLEDInit = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEOPIXELNUM, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(analog5, INPUT);
  pinMode(encoderButton, INPUT);
  pinMode(outputB, INPUT);
  pinMode(outputA, INPUT);
  aLastState = digitalRead(outputA);
  strip.setPixelColor(posLED, strip.Color(50, 0, 0));
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

// Makes 2 LEDs spin around the LED ring
// If they knock into eachother then 1 LED changes direction
void loop() {
  // put your main code here, to run repeatedly:
  encoder();
  button();
  matrix();
  strip.show();
}

void matrix() {
  if (abs(counter - counterInit) == 2) {
    //Serial.println("entered");
    if (counter > counterInit) {
      //Serial.println("counter > counterInit");
      // encoder counter
      counterInit = counter;
      posLEDInit = posLED;
      positionLED(1);
      strip.setPixelColor(posLED, strip.Color(random(0, 2)*BRIGHTNESS, random(0, 2)*BRIGHTNESS, random(1
      , 5)*BRIGHTNESS));
      strip.setPixelColor(posLEDInit, strip.Color(0, 0, 0));
    } else {
      counterInit = counter;
      posLEDInit = posLED;
      positionLED(-1);
      strip.setPixelColor(posLED, strip.Color(random(0, 2)*BRIGHTNESS, random(0, 2)*BRIGHTNESS, random(1, 5)*BRIGHTNESS));
      strip.setPixelColor(posLEDInit, strip.Color(0, 0, 0));
    }
  }
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

void saveColor() {
  if (lastButtonState == !buttonState) {
    Serial.print("Neopixel # :");
    Serial.print(posLED);
    Serial.println(" saved");
  }
}

void button() {
  delay(5);
  int buttonVal = analogRead(encoderButton);
  if (buttonVal == 0) {
    lastButtonState = buttonState;
    buttonState = true;
    saveColor();
  } else {
    buttonState = false;
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

// Only lets the LED change position if enough time has passed
// Wait time indicated by the wait variable, which is dependent upon
// analog input
boolean duration(uint32_t timeStamp, uint32_t wait) {
  if ((millis() - timeStamp) > wait) {
    return true;
  }
  return false;
}
