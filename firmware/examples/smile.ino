/*
  Example
  
  Draw simple face
*/

// subdirectory is required for Spark WEB IDE
#include "ledmatrix-max7219-max7221/ledmatrix-max7219-max7221.h"

LEDMatrix *led;

void setup() {
    // create new library instance with one display
    led = new LEDMatrix();
    
    // register display on the position [0;0] without rotatation or mirroring
    led->addMatrix(0, 0);
  
    // set brightness (0-15)
    led->setIntensity(4);
    
    // shapes will be drawn by turning LEDs on
    bool color = 1;
  
    // the outline of the face
    led->drawRoundRect(0, 0, 8, 8, 3, color);
    // mouth
    led->drawLine(2, 4, 3, 5, color);
    led->drawLine(4, 5, 5, 4, color);
    // eyes
    led->drawPixel(2, 2, color);
    led->drawPixel(5, 2, color);
  
    // redraw display
    led->flush();
    
    // free resources
    delete led;
}