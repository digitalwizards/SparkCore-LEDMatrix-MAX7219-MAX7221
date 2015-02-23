/*
  Example
  
  Scrolling text and simple vector drawing
*/

// subdirectory is required for Spark WEB IDE
#include "ledmatrix-max7219-max7221/ledmatrix-max7219-max7221.h"

LEDMatrix *led;

int bitmapWidth = 8;
// cycle counter (incremented after each heart drawing)
int counter = 0;
String text = "Heart";
int textLength = text.length();
// default position of the text is outside and then scrolls left
int textX = bitmapWidth;
int fontWidth = 5, space = 1;

// draw text
void drawText(String s, int x)
{
  int y = 0;
  for(int i = 0; i < s.length(); i++) {
    // Adafruit_GFX method
    led->drawChar(x + i*(fontWidth+space), y, s[i], true, false, 1);
  }
}

// draw symbol of heart
void drawHeart() {
  int x = 1, y = 2;
  // methods from Adafruit_GFX
  led->fillCircle(x+1, y+1, 1, true);
  led->fillCircle(x+5, y+1, 1, true);
  led->fillTriangle(x+0, y+1, x+6, y+1, x+3, y+4, true);
  // direct access to the main bitmap (instance of LEDMatrix::Bitmap)
  led->bitmap->setPixel(x+3, y+1, false);
}

void setup() {
  // setup pins and library
  // 1 display per row, 1 display per column
  // optional pin settings - default: CLK = A0, CS = A1, D_OUT = A2
  // (pin settings is independent on HW SPI)
  led = new LEDMatrix(1, 1, A0, A1, A2);
  // > add every matrix in the order in which they have been connected <
  // the first matrix in a row, the first matrix in a column
  // vertical orientation (-90°) and no mirroring - last three args optional
  led->addMatrix(0, 0, 270, false, false);
}

void loop() {
  // do only two cycles of animations
  if(led != NULL && counter < 2) {
    drawText(text, textX--);
    // text animation is ending when the whole text is outside the bitmap
    if(textX < textLength*(fontWidth+space)*(-1)) {
      // set default text position
      textX = bitmapWidth;
      drawHeart();
      // show heart
      led->flush();
      delay(2000);
      // turn all pixels off (takes effect after led->flush())
      led->fillScreen(false);
      counter++;
    }
    // draw text
    led->flush();
    delay(250);
  }
  // animations end
  else if (led != NULL) {
    // shutdown all displays
    led->shutdown(true);
    // free memory
    delete led;
    led = NULL;
  }
}
