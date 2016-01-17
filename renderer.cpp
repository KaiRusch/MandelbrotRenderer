#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
#include <cstdint>

class BMP_Header
{
private:

  int16_t  magicNumber;
  int32_t size;
  int16_t reserved1;
  int16_t reserved2;
  int32_t imageOffset;
  int32_t headerSize;  
  int32_t width;
  int32_t height;
  int16_t planes;  
  int16_t bitsPerPixel;
  int32_t compression;
  int32_t imageDataSize;
  int32_t horizontalRes;
  int32_t verticalRes;
  int32_t coloursInImage;
  int32_t importantColours;

  
public:
  BMP_Header(int width, int height);
  void write(std::ofstream &file);
};

BMP_Header::BMP_Header(int width, int height)
{
  magicNumber = 0x4D42;
  size = width*height*3 + 54;
  reserved1 = 0;
  reserved2 = 0;
  imageOffset = 54;
  headerSize = 40;
  this->width = width;
  this->height = height;
  planes = 1;
  bitsPerPixel = 24;
  compression = 0;
  imageDataSize = width*height*3;
  horizontalRes = 10;
  verticalRes = 0;
  coloursInImage = 0;
  importantColours = 0;  
}

void BMP_Header::write(std::ofstream &file)
{
  file.write(reinterpret_cast<char*>(&magicNumber),2);
  file.write(reinterpret_cast<char*>(&size),4);
  file.write(reinterpret_cast<char*>(&reserved1),2);
  file.write(reinterpret_cast<char*>(&reserved2),2);
  file.write(reinterpret_cast<char*>(&imageOffset),4);
  file.write(reinterpret_cast<char*>(&headerSize),4);
  file.write(reinterpret_cast<char*>(&width),4);
  file.write(reinterpret_cast<char*>(&height),4);
  file.write(reinterpret_cast<char*>(&planes),2);
  file.write(reinterpret_cast<char*>(&bitsPerPixel),2);
  file.write(reinterpret_cast<char*>(&compression),4);
  file.write(reinterpret_cast<char*>(&imageDataSize),4);
  file.write(reinterpret_cast<char*>(&horizontalRes),4);
  file.write(reinterpret_cast<char*>(&verticalRes),4);
  file.write(reinterpret_cast<char*>(&coloursInImage),4);
  file.write(reinterpret_cast<char*>(&importantColours),4);
}

class Pixel
{
public:
  unsigned char red;
  unsigned char green;
  unsigned char blue;
  Pixel();
  
  void set(unsigned char r, unsigned char g, unsigned char b);

};

Pixel::Pixel()
{
  red = 0;
  green = 0;
  blue = 0;
}

void Pixel::set(unsigned char r, unsigned char g, unsigned char b)
{
  red = r;
  green = g;
  blue = b;
}

double get_act_coord(int coord, int dimension)
{
  return double(coord)/double(dimension)*4 - 2;
}


int it_to_escape(double x0, double y0, int maxIt)
{
  double x = 0, y = 0;

  int it = 0;
  while(it < maxIt && (x*x + y*y) <= 4.0)
    {
      double xTemp = x;
      x = x*x - y*y + x0;
      y = 2*xTemp*y + y0;
      it++;
    }
  return it;
}

int main()
{
  int width, height, maxIts;

  std::cout << "Enter the width (in pixels) of the image:\n";
  std::cin >> width;

  std::cout << "Enter the height (in pixels) of the image:\n";
  std::cin >> height;

  std::cout << "Enter the maximum number of iterations:\n";
  std::cin >> maxIts;

  BMP_Header imageHeader(width,height);
  

  //Create the image buffer
  std::unique_ptr<std::vector<Pixel> > pixelArray(new std::vector<Pixel>(width*height,Pixel()));


 
  //Draw the image
  for(int pixel = 0; pixel < width*height; pixel++)
    {
      int x = pixel%width;
      int y = (pixel - pixel%width)/width;

      float actX = get_act_coord(x,width);
      float actY = get_act_coord(y,height);

      int itsToEscape = it_to_escape(actX,actY,maxIts);

      (*pixelArray)[pixel].set(255-itsToEscape/maxIts*700,250-itsToEscape*500,150-itsToEscape*400);

    }
  
  std::ofstream imageFile;
  imageFile.open("out.bmp", std::ios::binary);

  imageHeader.write(imageFile);
  
  for(Pixel pixel : *pixelArray)
    {
      imageFile.put(pixel.blue);
      imageFile.put(pixel.green);
      imageFile.put(pixel.red);
    }
  
  imageFile.close();


  /*
  //Write the file
  FILE *imageFile = fopen("out.bmp","wb");
  fwrite(headerBuffer,1,54,imageFile);
  fwrite(imageBuffer,1,width*height*3,imageFile);
  fclose(imageFile);*/

  return 0;
}