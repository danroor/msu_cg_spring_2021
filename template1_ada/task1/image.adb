with Text_IO;
with Ada.Text_IO;
with Ada.Integer_Text_IO;
with Interfaces.C;

use Ada.Integer_Text_IO;
use Ada.Text_IO;
use Text_IO;
use Interfaces.C;

package body Image is


  function Width (bm : Bitmap) return Integer is
  begin
    return bm'Last(1)+1;
  end Width;

  function Height(bm : Bitmap) return Integer is
  begin
    return bm'Last(2)+1;
  end Height;

  function Load_Bitmap(filepath : String) return Bitmap is
    info    : ImageInfo;
    emptyBm : Bitmap(0 .. 0, 0 .. 0) := (others => (others => (0,0,0,0)));
  begin

    -- get width and height
    --
    info := cpp_GetImageInfo(To_C(filepath));

    if info.width = 0 or info.height = 0 then
      return emptyBm;
    end if;

    declare
      arr : PixArray(0 .. info.width*info.height-1);
      bm  : Bitmap  (0 .. info.width-1, 0 .. info.height-1);
      res : Integer;
    begin

      -- load image from file to linear array
      --
      res := cpp_LoadLDRImageFromFile(To_C(filepath), arr(0)'Address);

      if res = 0 then
        return emptyBm;
      end if;

      -- copy pixels from linear array to 2D array
      --
      for y in bm'First(2) .. bm'Last(2) loop
        for x in bm'First(1) .. bm'Last(1) loop
          bm(x,y) := arr(y*info.width+x);
        end loop;
      end loop;

     return bm;
    end;

  end Load_Bitmap;

  procedure Save_Bitmap(bm : Bitmap; filepath : String) is
    w   : constant Integer := Width(bm);
    h   : constant Integer := Height(bm);
    arr : PixArray(0 .. w*h);
  begin

    if bm'Last(1) = 0 or bm'Last(2) = 0 then
      return;
    end if;

    -- copy pixels from bitmap to linear array
    --
    for y in bm'First(2) .. bm'Last(2) loop
      for x in bm'First(1) .. bm'Last(1) loop
        arr(y*w+x) := bm(x,y);
      end loop;
    end loop;

    cpp_SaveLDRImageToFile(To_C(filepath), w, h, arr(0)'Address);

  end Save_Bitmap;


end Image;
