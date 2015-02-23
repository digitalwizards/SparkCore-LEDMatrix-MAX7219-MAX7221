#ifndef LEDMATRIX_h
#define LEDMATRIX_h

#include "Adafruit_GFX.h"

class LEDMatrix : public Adafruit_GFX
{
  public:
    class Bitmap
    {
      public:
        Bitmap(int width, int height);
        int getWidth();
        int getHeight();
        void setPixel(int x, int y, bool val);
        bool getPixel(int x, int y);
        void togglePixel(int x, int y);
        // allows get the whole row of pixels at once
        byte getByte(int id);
        ~Bitmap();

      private:
        // pixels are saved as 1D byte array
        byte *array;
        int width, height;
        // param bytePos = which byte contains the pixel
        // param posInByte = index of pixel(bit) in that byte
        bool containsPixel(int x, int y,
            int *bytePos = NULL, int *posInByte = NULL);
    };

    // = 8x8 matrix / one chip
    struct Matrix
    {
      // 0, 90, 180, 270
      int rotation;
      bool mirrorX, mirrorY;
      // pixel [0, 0] is [firstX, firstY] for the main bitmap
      int firstX, firstY;
      // bitmap 8x8 - it's a copy of the part of the main bitmap
      // before transforming and flushing
      Bitmap *bitmap;
      Matrix *next;

      Matrix(int x = 0, int y = 0, int rotation = 0, bool mirrorX = false,
          bool mirrotY = false, Matrix *next = NULL);
      ~Matrix();
    };

    // a simple linked list
    class MatrixList
    {
      // defines the callback functions signature for method 'forEach'
      // it allows to transform matrixes and flush to chips
      typedef void (LEDMatrix::*callback)(Matrix*, int index);

      private:
        Matrix *first = NULL;
        int count = 0;

      public:
        void
          addMatrix(Matrix *matrix),
          forEach(callback func);
        int getCount();
        LEDMatrix *parent;
        ~MatrixList();
    };

    // allows to send same opcode and data to all displays in spiTransfer call
    static const int ALL_DISPLAYS = -1;
    // the main bitmap - contains all pixels
    Bitmap *bitmap;
    // (pin settings is independent on HW SPI)
    LEDMatrix(int horizDisplays = 1, int vertDisplays = 1,
        int pinCLK = A0, int pinCS = A1, int pinD_OUT = A2);
    void
      // color: 0/1 or true/false
      drawPixel(int16_t x, int16_t y, uint16_t color),
      // invert the main bitmap
      invert(),
      // display: 0 is the nearest to the Spark Core
      shutdown(bool shutdown, int display = ALL_DISPLAYS),
      // intensity: 0-15
      setIntensity(int intensity, int display = ALL_DISPLAYS),
      // rotatin: 0,90,180,270 degrees
      addMatrix(int x, int y, int rotation = 0,
        bool mirrorX = false, bool mirrorY = false),
      // flush the main bitmap to chips
      flush();
    ~LEDMatrix();


  private:
    // register address map for each MAX7219/7221 (opCodes)
    static const int OP_NOOP = 0,
    OP_DIGIT0 = 1, // == one row (byte)
    OP_DIGIT1 = 2,
    OP_DIGIT2 = 3,
    OP_DIGIT3 = 4,
    OP_DIGIT4 = 5,
    OP_DIGIT5 = 6,
    OP_DIGIT6 = 7,
    OP_DIGIT7 = 8,
    OP_DECODEMODE = 9, // no decode == 0 (useful only for 7-segment display)
    OP_INTENSITY = 10, // 0 - 15 (different steps for each MAX72XX)
    OP_SCANLIMIT = 11, // all rows == 7 (sets number of active rows / digits)
    OP_SHUTDOWN = 12, // turn display on == 1
    OP_DISPLAYTEST = 15; // turn all LEDs on == 1

    // = CLK, SS, MOSI
    int pinCLK, pinCS, pinD_OUT;
    int matrixCount;
    MatrixList *matrixList;
    void
      // transform 8x8 parts of the main matrix for the output
      // (rotating, mirroring)
      transform(Matrix *matrix, int index),
      // send transformed matrix to the output
      sendMatrix(Matrix *matrix, int index),
      // flush address with data to chips
      spiTransfer(byte opCode, byte data, int display = ALL_DISPLAYS);
};

#endif //LEDMATRIX_h
