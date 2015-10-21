// Basic code shamelessly stolen from the Adafruit NeoPixle demo code.

#include <Adafruit_NeoPixel.h>
#include <Bounce2.h>


#define UP_PIN     2
#define SELECT_PIN 3
#define DOWN_PIN   4

#define PIXEL_PIN    6    // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 40

// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Instantiate a Bounce object
Bounce up_button = Bounce(); 
Bounce select_button = Bounce(); 
Bounce down_button = Bounce(); 

bool oldState = HIGH;
int showType = 0;
int brightness = 30;
int defaultBrightness = 30;
int stepSize = 10;

// 0 = Normal
// 1 = RED ONLY
// 2 = Attract mode
// 3 = SOS

void setup() {
  
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(SELECT_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);

  // Set the show type at the beginning (before setting up debounce)
  if ( digitalRead(DOWN_PIN) == LOW ) {
    showType = 3;
  }
  if ( digitalRead(SELECT_PIN) == LOW ) {
    showType = 2;
  }
  if ( digitalRead(UP_PIN) == LOW ) {
    showType = 1;
  }
  
  up_button.attach(UP_PIN);
  up_button.interval(5); // interval in ms

  select_button.attach(SELECT_PIN);
  select_button.interval(5); // interval in ms

  down_button.attach(DOWN_PIN);
  down_button.interval(5); // interval in ms
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // Get current button state.
  up_button.update();
  select_button.update();
  down_button.update();
  
  int up = up_button.read();
  int select = select_button.read();
  int down = down_button.read();

  if ( up == LOW ) {
    brightness = brightness + stepSize;
  }
  
  if ( select == LOW ) {
    brightness = defaultBrightness;
  }

  if ( down == LOW ) {
    brightness = brightness - stepSize;
  }
  
  if ( brightness > 250 ) {
    brightness = 250;
  }
  
  if ( brightness < 0 ) {
    brightness = 0;
  }

  startShow(showType);
  
  while ( up==LOW || select==LOW || down==LOW ) {
    up_button.update();
    select_button.update();
    down_button.update();
    
    up = up_button.read();
    select = select_button.read();
    down = down_button.read();
  }

}

void startShow(int i) {
  switch(i){
    // Normal
    case 0: colorWipe(strip.Color(brightness, brightness, brightness), 0);    // Black/off
            break;
    // RED
    case 1: colorWipe(strip.Color(brightness, 0, 0), 0);  // Red
            break;
    // Attract
    case 2: colorWipe(strip.Color(0, brightness, 0), 0);  // Green
            break;
    // SOS
    case 3: colorWipe(strip.Color(0, 0, brightness), 0);  // Blue
            break;

    // ALL OTHERS UNUSED FOR NOW
    case 4: theaterChase(strip.Color(127, 127, 127), 50); // White
            break;
    case 5: theaterChase(strip.Color(127,   0,   0), 50); // Red
            break;
    case 6: theaterChase(strip.Color(  0,   0, 127), 50); // Blue
            break;
    case 7: rainbow(20);
            break;
    case 8: rainbowCycle(20);
            break;
    case 9: theaterChaseRainbow(50);
            break;
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
