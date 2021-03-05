with Window;
with Screen;
with Player;
with Image;

procedure task1 is
begin
   Window.Init;

   Window.Open_Window (Width => Screen.g_width, Height => Screen.g_height);

   Screen.Fill_Screen (Screen.background_color);
   while not Window.Escape_Pressed and
         Window.Window_Opened loop
      Player.ProcessInput;
      Player.Draw;
      Screen.Draw;

      Window.Swap_Buffers;

      Window.Poll_Events;
   end loop;

   Window.Shutdown;

end task1;
