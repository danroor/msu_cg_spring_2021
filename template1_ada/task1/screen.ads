with Interfaces;
--with Interfaces.C;

use Interfaces;
package Screen is

  g_width  : Integer   := 512;
  g_height : Integer   := 512;
  g_tilesize : Integer := 16;
     
   
  type Pixel is record
    r, g, b, a : Unsigned_8;
  end record;

  background_color: Pixel := (0, 0, 20, 0);
  
  type ScreenBuffer is array (0 .. g_width * g_height - 1) of Pixel;

  g_fbdata : ScreenBuffer;
 
  procedure Put_Pixel (x : Integer; y : Integer; pix : Pixel);

  function Get_Pixel (x : Integer; y : Integer) return Pixel;
   
  procedure Fill_Screen (pix : Pixel);
   
  procedure Draw; 

end Screen;
