with Text_IO;
with Ada.Text_IO;
with Ada.Integer_Text_IO;
with Interfaces;
with Interfaces.C;
With System;
with Screen;

use Interfaces;
use Interfaces.C;

package Image is

  type Bitmap is array (Integer range <>, Integer range <>) of Screen.Pixel;

  function Width (bm : Bitmap) return Integer;
  function Height(bm : Bitmap) return Integer;

  function  Load_Bitmap(filepath : String) return Bitmap;
  procedure Save_Bitmap(bm : Bitmap; filepath : String);

private

  type ImageInfo is record
    width  : Integer;
    height : Integer;
  end record;

  type PixArray is array (Integer range <>) of Screen.Pixel;

  function cpp_GetImageInfo(filename : Char_Array) return ImageInfo;
  pragma Import(C, cpp_GetImageInfo, "cpp_GetImageInfo");

  function cpp_LoadLDRImageFromFile(filename : Char_Array; dataptr : System.Address) return Integer;
  pragma Import(C, cpp_LoadLDRImageFromFile, "cpp_LoadLDRImageFromFile");

  procedure cpp_SaveLDRImageToFile(filename : Char_Array; w : Integer; h : Integer; dataptr : System.Address);
  pragma Import(C, cpp_SaveLDRImageToFile, "cpp_SaveLDRImageToFile");

end Image;
