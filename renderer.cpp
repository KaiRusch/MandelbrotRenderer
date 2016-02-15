#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
#include <cstdint>
#include <string>

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

class BMP
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

  std::ofstream imageFile;

public:
  BMP(int width, int height, std::string filename);
  void write_header();
  void write(std::unique_ptr<std::vector<Pixel> > &pixelArray);
};

BMP::BMP(int width, int height, std::string filename)
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


  imageFile.open(filename, std::ios::binary);
}

void BMP::write_header()
{
  imageFile.write(reinterpret_cast<char*>(&magicNumber),2);
  imageFile.write(reinterpret_cast<char*>(&size),4);
  imageFile.write(reinterpret_cast<char*>(&reserved1),2);
  imageFile.write(reinterpret_cast<char*>(&reserved2),2);
  imageFile.write(reinterpret_cast<char*>(&imageOffset),4);
  imageFile.write(reinterpret_cast<char*>(&headerSize),4);
  imageFile.write(reinterpret_cast<char*>(&width),4);
  imageFile.write(reinterpret_cast<char*>(&height),4);
  imageFile.write(reinterpret_cast<char*>(&planes),2);
  imageFile.write(reinterpret_cast<char*>(&bitsPerPixel),2);
  imageFile.write(reinterpret_cast<char*>(&compression),4);
  imageFile.write(reinterpret_cast<char*>(&imageDataSize),4);
  imageFile.write(reinterpret_cast<char*>(&horizontalRes),4);
  imageFile.write(reinterpret_cast<char*>(&verticalRes),4);
  imageFile.write(reinterpret_cast<char*>(&coloursInImage),4);
  imageFile.write(reinterpret_cast<char*>(&importantColours),4);
}

void BMP::write(std::unique_ptr<std::vector<Pixel> > &pixelArray)
{
  write_header();
 
  for(Pixel pixel : *pixelArray)
    {
      imageFile.put(pixel.blue);
      imageFile.put(pixel.green);
      imageFile.put(pixel.red);
    } 
}

double get_act_coord(int coord, int dimension, double scale, double offset)
{
  return (double(coord)/double(dimension)*4 - 2)*scale + offset;
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



void render(std::unique_ptr<std::vector<Pixel> > &pixelArray, int width, int height, double xScale, double yScale, double xOffset, double yOffset, int maxIts)
{
  //Draw the image
  for(int pixel = 0; pixel < width*height; pixel++)
    {
      int x = pixel%width, y = (pixel - pixel%width)/width;
      double actX = get_act_coord(x,width,xScale,xOffset), actY = get_act_coord(y,height,yScale,yOffset);
      int itsToEscape = it_to_escape(actX,actY,maxIts);
      (*pixelArray)[pixel].set((255*itsToEscape)/maxIts,(255*itsToEscape)/maxIts,(255*itsToEscape)/maxIts);
    }
}

void get_image_dimensions(int &width, int &height, double &xScale, double &yScale, double &xOffset, double &yOffset, int&maxIts)
{
  std::cout << "Enter the width (in pixels) of the image:\n";
  std::cin >> width;

  std::cout << "Enter the height (in pixels) of the image:\n";
  std::cin >> height;

  std::cout << "Enter the X scaling factor:\n";
  std::cin >> xScale;

  std::cout << "Enter the Y scaling factor:\n";
  std::cin >> yScale;   

  std::cout << "Enter X offset:\n";
  std::cin >> xOffset;

  std::cout << "Enter Y offset:\n";
  std::cin >> yOffset;

  std::cout << "Enter the maximum number of iterations:\n";
  std::cin >> maxIts;
}

int main()
{
  int width, height, maxIts;
  double xScale, yScale, xOffset, yOffset;
  get_image_dimensions(width, height, xScale, yScale, xOffset, yOffset, maxIts);

  //Create the pixel array
  std::unique_ptr<std::vector<Pixel> > pixelArray(new std::vector<Pixel>(width*height,Pixel()));

  render(pixelArray, width, height, xScale, yScale, xOffset, yOffset, maxIts);
  
  BMP image(width,height,"out.bmp");
  image.write(pixelArray);
  

  return 0;
}
