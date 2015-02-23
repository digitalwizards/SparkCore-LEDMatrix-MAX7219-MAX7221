#include "ledmatrix-max7219-max7221.h"

LEDMatrix::LEDMatrix(int horizDisplays, int vertDisplays, int pinCLK, int pinCS, int pinD_OUT)
 : Adafruit_GFX(horizDisplays*8, vertDisplays*8)
{
  this->pinCLK = pinCLK;
  this->pinCS = pinCS;
  this->pinD_OUT = pinD_OUT;

  pinMode(pinD_OUT,OUTPUT);
  pinMode(pinCLK,OUTPUT);
  pinMode(pinCS,OUTPUT);

  matrixCount = horizDisplays * vertDisplays;

  // deactivate displaytest
  spiTransfer(OP_DISPLAYTEST, 0);
  // activate all rows
  spiTransfer(OP_SCANLIMIT, 7);
  // we don't use 7-segment display
  spiTransfer(OP_DECODEMODE, 0);
  // average light intensity
  spiTransfer(OP_INTENSITY, 7);
  // turn off all LEDs
  for(int j = OP_DIGIT0; j <= OP_DIGIT7; j++)
    spiTransfer(j, 0);
  // activate displays
  spiTransfer(OP_SHUTDOWN, 1);

  matrixList = new MatrixList;
  matrixList->parent = this;

  bitmap = new Bitmap(horizDisplays*8, vertDisplays*8);
}

void LEDMatrix::addMatrix(int x, int y, int rotation, bool mirrorX, bool mirrorY)
{
  Matrix *matrix = new Matrix(x, y, rotation, mirrorX, mirrorY);
  matrixList->addMatrix(matrix);
}

void LEDMatrix::setIntensity(int intensity, int display)
{
  spiTransfer(OP_INTENSITY, intensity, display);
}

void LEDMatrix::shutdown(bool shutdown, int display)
{
  spiTransfer(OP_SHUTDOWN, !shutdown, display);
}

void LEDMatrix::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  bitmap->setPixel(x, y, (bool)color);
}

void LEDMatrix::invert()
{
  for(int i = 0; i < bitmap->getWidth(); i++)
    for(int j = 0; j < bitmap->getHeight(); j++)
      bitmap->togglePixel(i, j);
}

void LEDMatrix::spiTransfer(byte opCode, byte data, int display)
{
  // begin writing
  digitalWrite(pinCS,LOW);

  for(int i = matrixCount - 1; i >= 0; i--)
  {
    // send opcode and data or only no-op (skip display)?
    bool write = display == ALL_DISPLAYS || display == i;

    shiftOut(pinD_OUT, pinCLK, MSBFIRST, write ? opCode : OP_NOOP);
    shiftOut(pinD_OUT, pinCLK, MSBFIRST, write ? data : OP_NOOP);
  }
  // stop writing - let chips process new commands
  digitalWrite(pinCS,HIGH);
}


void LEDMatrix::transform(Matrix *matrix, int index)
{
  int x = matrix->firstX;
  int y = matrix->firstY;

  int xx, yy;

  for(int i = 0; i < 8; i++)
  {
        for(int j = 0; j < 8; j++)
        {
            bool val = bitmap->getPixel(x, y);
            switch(matrix->rotation)
            {
              case 90:
                xx = 7-j;
                yy = i;
                break;

              case 180:
                xx = 7-i;
                yy = 7-j;
                break;

              case 270:
                xx = j;
                yy = 7-i;
                break;

              // 0 and invalid values
              default:
                xx = i;
                yy = j;
            }

            if(matrix->mirrorX)
              xx = 7-xx;
            if(matrix->mirrorY)
              yy = 7-yy;

            matrix->bitmap->setPixel(xx, yy, val);

            y++;
        }
        x++;
        y = 0;
  }
}

void LEDMatrix::sendMatrix(Matrix *matrix, int index)
{
  for(int i = 0; i < 8; i++)
  {
    spiTransfer(OP_DIGIT0 + i, matrix->bitmap->getByte(i), index);
  }
}

void LEDMatrix::flush()
{
  matrixList->forEach(&LEDMatrix::transform);
  matrixList->forEach(&LEDMatrix::sendMatrix);
}

LEDMatrix::~LEDMatrix()
{
  delete bitmap;
  delete matrixList;
}


LEDMatrix::Matrix::Matrix(int x, int y, int rotation, bool mirrorX, bool mirrorY, Matrix *next)
{
  this->firstX = x * 8;
  this->firstY = y * 8;
  this->rotation = rotation;
  this->mirrorX = mirrorX;
  this->mirrorY = mirrorY;
  this->next = next;
  bitmap = new Bitmap(8, 8);
}

LEDMatrix::Matrix::~Matrix()
{
  delete bitmap;
}

void LEDMatrix::MatrixList::addMatrix(Matrix *matrix)
{
  if(first == NULL)
    first = matrix;
  else
  {
    matrix->next = first;
    first = matrix;
  }
  count++;
}


void LEDMatrix::MatrixList::forEach(callback func)
{
  int index = count-1;
  if(first != NULL)
  {
    Matrix *matrix = first;
    do
    {
      // parent is instance of LEDMatrix
      (parent->*func)(matrix, index--);
      matrix = matrix->next;
    }
    while(matrix != NULL);
  }
}


LEDMatrix::MatrixList::~MatrixList()
{
    Matrix *nextMatrix;
    while(first != NULL)
    {
      nextMatrix = first->next;
      delete first;
      first = nextMatrix;
    }
}


LEDMatrix::Bitmap::Bitmap(int width, int height)
{
  // "+(8-1)" is an alternative to rounding up to an integer (whole byte)
  int byteCount = ((width*height) + 7) / 8;
  array = new byte[ byteCount ];
  // set all pixels to 0
  for(int i = 0; i < byteCount; i++)
    array[i] = 0;
  this->width = width;
  this->height = height;
}

bool LEDMatrix::Bitmap::containsPixel(int x, int y, int *bytePos, int *posInByte)
{
  if(x >= width || y >= height || x < 0 || y < 0)
    return false;

  int pos1D = width * y + x;

  if(posInByte != NULL)
    *posInByte = pos1D % 8;

  if(bytePos != NULL)
    *bytePos = pos1D / 8;

  return true;
}

void LEDMatrix::Bitmap::setPixel(int x, int y, bool val)
{
  int posInByte, bytePos;
  if(containsPixel(x, y, &bytePos, &posInByte))
  {
    // bytes and bits are written from left to right
    // => right shift is used (128 = 0b10000000)
    if(val)
      array[bytePos] |= 128 >> posInByte;
    else
      array[bytePos] &= ~(128 >> posInByte);
  }
}

bool LEDMatrix::Bitmap::getPixel(int x, int y)
{
  int posInByte, bytePos;
  if(containsPixel(x, y, &bytePos, &posInByte))
    return (bool)(array[bytePos] & (128 >> posInByte));
  return false;
}

void LEDMatrix::Bitmap::togglePixel(int x, int y)
{
  int posInByte, bytePos;
  if(containsPixel(x, y, &bytePos, &posInByte))
    array[bytePos] ^= 128 >> posInByte;
}

byte LEDMatrix::Bitmap::getByte(int id)
{
  return array[id];
}

int LEDMatrix::Bitmap::getWidth()
{
  return width;
}

int LEDMatrix::Bitmap::getHeight()
{
  return height;
}

LEDMatrix::Bitmap::~Bitmap()
{
  delete[] array;
}
