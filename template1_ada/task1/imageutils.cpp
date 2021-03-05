#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <FreeImage.h>

#include <errno.h>
#include <math.h>

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
{
  std::cout << "\n***\n";
  std::cout << message;
  std::cout << "\n***\n";
}


struct ImageInfo
{
  int width;
  int height;
};


extern "C" ImageInfo cpp_GetImageInfo(const char* a_fileName)
{
  ImageInfo res{0,0};

  FREE_IMAGE_FORMAT fif = FIF_PNG; // image format

  fif = FreeImage_GetFileType(a_fileName, 0);

  if (fif == FIF_UNKNOWN)
    fif = FreeImage_GetFIFFromFilename(a_fileName);

  FIBITMAP* dib = nullptr;
  if (FreeImage_FIFSupportsReading(fif))
    dib = FreeImage_Load(fif, a_fileName);
  else
  {
    std::cout << "cpp_GetImageInfo() : FreeImage_FIFSupportsReading/FreeImage_Load failed!" << std::endl;
    return res;
  }

  res.width  = FreeImage_GetWidth(dib);
  res.height = FreeImage_GetHeight(dib);
  FreeImage_Unload(dib);

  return res;
}


extern "C" int cpp_LoadLDRImageFromFile(const char* a_fileName, unsigned int* a_data)
{
  FREE_IMAGE_FORMAT fif = FIF_PNG; // image format

  fif = FreeImage_GetFileType(a_fileName, 0);

  if (fif == FIF_UNKNOWN)
    fif = FreeImage_GetFIFFromFilename(a_fileName);

  FIBITMAP* dib = nullptr;
  if (FreeImage_FIFSupportsReading(fif))
    dib = FreeImage_Load(fif, a_fileName);
  else
  {
    std::cout << "cpp_LoadLDRImageFromFile() : FreeImage_FIFSupportsReading/FreeImage_Load failed!" << std::endl;
    return 0;
  }

  FIBITMAP* converted = FreeImage_ConvertTo32Bits(dib);
  BYTE* bits          = FreeImage_GetBits(converted);
  auto width          = FreeImage_GetWidth(converted);
  auto height         = FreeImage_GetHeight(converted);
  auto bitsPerPixel   = FreeImage_GetBPP(converted);

  //a_data.resize(width*height);
  BYTE* data = (BYTE*)&a_data[0];

  for (unsigned int y = 0; y<height; y++)
  {
    int lineOffset1 = y*width;
    int lineOffset2 = y*width;

    for (unsigned int x = 0; x<width; x++)
    {
      int offset1 = lineOffset1 + x;
      int offset2 = lineOffset2 + x;

      data[4 * offset1 + 0] = bits[4 * offset2 + 2];
      data[4 * offset1 + 1] = bits[4 * offset2 + 1];
      data[4 * offset1 + 2] = bits[4 * offset2 + 0];
      data[4 * offset1 + 3] = bits[4 * offset2 + 3];
    }
  }

  FreeImage_Unload(dib);
  FreeImage_Unload(converted);

  //(*pW) = width;
  //(*pH) = height;

  return 1;
}

extern "C" void cpp_SaveLDRImageToFile(const char* a_fileName, int w, int h, int32_t* data)
{
  FIBITMAP* dib = FreeImage_Allocate(w, h, 32);

  BYTE* bits = FreeImage_GetBits(dib);
  //memcpy(bits, data, w*h*sizeof(int32_t));
  BYTE* data2 = (BYTE*)data;
  for (int i = 0; i<w*h; i++)
  {
    bits[4 * i + 0] = data2[4 * i + 2];
    bits[4 * i + 1] = data2[4 * i + 1];
    bits[4 * i + 2] = data2[4 * i + 0];
    bits[4 * i + 3] = 255; // data2[4 * i + 3]; // 255 to kill alpha channel
  }

	auto imageFileFormat = FIF_PNG;

	std::string fileName(a_fileName);
	if (fileName.size() > 4)
	{
		std::string resolution = fileName.substr(fileName.size() - 4, 4);

		if (resolution.find(".bmp") != std::string::npos || resolution.find(".BMP") != std::wstring::npos)
			imageFileFormat = FIF_BMP;
	}

  if (!FreeImage_Save(imageFileFormat, dib, a_fileName))
  {
    FreeImage_Unload(dib);
    std::cout << "cpp_SaveLDRImageToFile(): FreeImage_Save error on " << a_fileName << std::endl;
    return;
  }

  FreeImage_Unload(dib);
  return;
}
