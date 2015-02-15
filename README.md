# SparkCore-LEDMatrix 0.1
Spark Core library for control 8x8 LED matrices using MAX7219 / MAX7221.<br>
Depends on [digitalwizards/SparkCore-Adafruit-GFX-Library](https://github.com/digitalwizards/SparkCore-Adafruit-GFX-Library).

## Features
* uses Adafruit_GFX library which provides basic graphics methods<br>
* (draw/fill circles, triangles, chars, etc.)
* allows to set transformations (rotating, mirroring) and position for each display
* provides direct access to the whole bitmap, independent on display transformations
* transformation buffers and flushing whole bitmap at once avoid flickering
* possible to choose pins for CLK, CS, D_OUT (= CLK, SS, MOSI)
* bitmaps are implemented as byte arrays for minimalizing memory usage<br>
(some memory has been sacrificed for trans. buffers and direct access to the unchanged bitmap)

### Using with [Spark Dev](https://www.spark.io/dev)
1. Create a project folder.
2. The folder must contains **LEDMatrix.cpp**, **LEDMatrix.h** from this repository<br>
and **Adafruit_GFX.cpp**, **Adafruit_GFX.h**, **glcdfont.c** from [SparkCore-Adafruit-GFX-Library](https://github.com/digitalwizards/SparkCore-Adafruit-GFX-Library).
3. Include licenses.

### Initializing
1) Include header file:
```C++
#include "LEDMatrix.h"
```
2) Create instance:
```C++
// 1 display per row, 1 display per column
// optional pin settings - default: CLK = A0, CS = A1, D_OUT = A2
// (pin settings is independent on HW SPI)
LEDMatrix *led = new LEDMatrix(1, 1, A0, A1, A2);
```
3) Setup matrices:
```C++
// > add every matrix in the order in which they have been connected <
// the first matrix in a row, the first matrix in a column
// vertical orientation (270°) and no mirroring (x, y axes) - last three args optional
led->addMatrix(0, 0, 270, false, false);
```

### Methods
It is possible to use almost all inhereted methods from Adafruit_GFX library:
```C++
led->fillCircle(5, 3, 2, true);
```
Use built-in methods:
```C++
// set intensity of the third display (0-15)  
led->setIntensity(7, 2);
// invert displays
led->invert();
// shutdown all displays
led->shutdown(true);
```
Direct access to the bitmap:
```C++
// turn off pixel
led->bitmap->setPixel(3, 1, false);
bool val = led->bitmap->getPixel(3, 1);
led->bitmap->togglePixel(3, 1);
led->bitmap->getWidth();
led->bitmap->getHeight();
```

### Bitmap changes
Pixel changes do not take effect immediately. Update displays:
```C++
led->flush();
```

### Closing
You can shutdown display(s):
```C++
led->shutdown(true);
```
And free memory:
```C++
delete led;
```

### Example
**Example.ino** contains the implementation of a simple scrolling text and function for drawing a heart symbol.

### Inspiration
* [markruys/arduino-Max72xxPanel](https://github.com/markruys/arduino-Max72xxPanel)
* [chrisloris/LedControl-MAX7219-MAX7221](https://github.com/chrisloris/LedControl-MAX7219-MAX7221)

## Feedback
Suggestions for new features, reporting bugs and other contributions will be very much appreciated!
