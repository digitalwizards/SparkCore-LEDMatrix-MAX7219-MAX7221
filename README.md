# SparkCore-LEDMatrix
Spark Core library for control 8x8 LED matrices using MAX7219 / MAX7221.<br>

## Features
* uses Adafruit_GFX library which provides basic graphics methods<br>
(draw/fill circles, triangles, chars, etc.)
* allows to set transformations (rotating, mirroring) and position for each display
* provides direct access to the whole bitmap, independent on display transformations
* transformation buffers and flushing whole bitmap at once avoid flickering
* possible to choose pins for CLK, CS, D_OUT (= CLK, SS, MOSI)
* bitmaps are implemented as bit arrays for minimalizing memory usage<br>
(some memory has been sacrificed for trans. buffers and direct access to the unchanged bitmap)

### Using with [Spark WEB IDE](https://www.spark.io/build)
1. Include library **LEDMATRIX-MAX7219-MAX7221**

### Using with [Spark Dev](https://www.spark.io/dev)
1. Create a project folder.
2. The folder must contains **ledmatrix-max7219-max7221.cpp**, **ledmatrix-max7219-max7221.h**<br>
and **Adafruit_GFX.cpp**, **Adafruit_GFX.h**, **glcdfont.cpp** from [SparkCore-Adafruit-GFX-Library](https://github.com/digitalwizards/SparkCore-Adafruit-GFX-Library)<br>
(everything included in [firmware](firmware) folder).
3. Include [license.txt](license.txt) and [text required by Adafruit GFX library](#text-required-by-adafruit-gfx-library).

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
[heart.ino](firmware/examples/heart.ino) contains the implementation of a simple scrolling text and function for drawing a heart symbol.

### Inspiration
* [markruys/arduino-Max72xxPanel](https://github.com/markruys/arduino-Max72xxPanel)
* [chrisloris/LedControl-MAX7219-MAX7221](https://github.com/chrisloris/LedControl-MAX7219-MAX7221)

## Feedback
Suggestions for new features, reporting bugs and other contributions will be very much appreciated!

<br><hr>
### Text required by Adafruit GFX library:
```
This is the core graphics library for all our displays, providing a common set of graphics primitives (points, lines, circles, etc.).  It needs to be paired with a hardware-specific library for each display device we carry (to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
BSD license, check license.txt for more information.
```
