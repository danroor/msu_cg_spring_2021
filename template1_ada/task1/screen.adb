with gl_h; use gl_h;

package body Screen is

  procedure Put_Pixel (x : Integer; y : Integer; pix : Pixel) is
  begin
    g_fbdata (y * g_width + x) := pix;
  end Put_Pixel;

  function Get_Pixel (x : Integer; y : Integer) return Pixel is
  begin
    return g_fbdata (y * g_width + x);
  end Get_Pixel;
   
  procedure Fill_Screen (pix : Pixel) is
  begin

    for y in 0 .. g_height - 1 loop
      for x in 0 .. g_width - 1 loop
        Put_Pixel (x, y, pix);
      end loop;
    end loop;

  end Fill_Screen;
   
  procedure Draw is
  begin
    glClearColor (0.0, 0.0, 1.0, 0.0);
    glClear (GL_COLOR_BUFFER_BIT or GL_DEPTH_BUFFER_BIT or GL_STENCIL_BUFFER_BIT);

    glDrawPixels (g_width, g_height, GL_RGBA, GL_UNSIGNED_BYTE, g_fbdata (0)'Address);
  end Draw;
   
end Screen;
