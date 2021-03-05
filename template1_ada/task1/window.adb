with Glfw.Errors;
with Glfw.Windows.Context;
with Glfw.Windows.Hints;
with Glfw.Input.Mouse;
with Glfw.Input.Keys;

with Ada.Text_IO;
with Screen;

package body Window is

  -- type Test_Window is new Glfw.Windows.Window with null record;

   type Test_Window is new Glfw.Windows.Window with record
      win_X, win_Y : Float;
   end record;
   type Main_Window_Type is not null access Test_Window;

   overriding procedure Mouse_Position_Changed (Object : not null access Test_Window; X, Y   : Glfw.Input.Mouse.Coordinate);
   overriding
   procedure Mouse_Button_Changed(Object : not null access Test_Window;
                                   Button  : Glfw.Input.Mouse.Button;
                                   State   : Glfw.Input.Button_State;
                                   Mods    : Glfw.Input.Keys.Modifiers
                                  );

   overriding
   procedure Close_Requested (Object : not null access Test_Window) is
   begin
      Object.Destroy;
   end Close_Requested;

   Main_Window : constant Main_Window_Type := new Test_Window;

   overriding
   procedure Mouse_Position_Changed (Object : not null access Test_Window; X, Y   : Glfw.Input.Mouse.Coordinate) is
      --use type Glfw.Input.Button_State;
   begin
      Object.Set_Title ("(" & X'Img & ", " & Y'Img & ")");
   end Mouse_Position_Changed;

   overriding
   procedure Mouse_Button_Changed (Object : not null access Test_Window;
                                   Button  : Glfw.Input.Mouse.Button;
                                   State   : Glfw.Input.Button_State;
                                   Mods    : Glfw.Input.Keys.Modifiers
                                  ) is
    use type Glfw.Input.Mouse.Button;
    use     type Glfw.Input.Button_State;
    X, Y    : Glfw.Input.Mouse.Coordinate;
    pix: Screen.Pixel;
   begin
      --if Button /= 0 or else State /= Glfw.Input.Pressed then
      --   return;
      --end if;

      if Mods.Shift then
         Object.Get_Cursor_Pos (X, Y);
         Object.win_X := Float (X);
         Object.win_Y := Float (Y);
         pix := (255, 255, 255, 0);
         Screen.Put_Pixel(Integer(X), Integer(Y), pix);
      end if;
   end Mouse_Button_Changed;

   procedure Print_Error (Code : Glfw.Errors.Kind; Description : String) is
   begin
      Ada.Text_IO.Put_Line ("Error occured (" & Code'Img & "): " & Description);
   end Print_Error;

   procedure Enable_Print_Errors is
   begin
      Glfw.Errors.Set_Callback (Print_Error'Access);
   end Enable_Print_Errors;

   procedure Init is
   begin
      Enable_Print_Errors;
      Glfw.Init;
  end Init;


   procedure Open_Window (Width, Height : Natural; Depth_Bits : Natural := 0) is
   begin
      if not Main_Window.Initialized then
         Glfw.Windows.Hints.Set_Depth_Bits (Depth_Bits);
         Main_Window.Init (Glfw.Size (Width), Glfw.Size (Height),
                           "task 1 template");
      end if;
      Main_Window.Show;
      Main_Window.Enable_Callback (Glfw.Windows.Callbacks.Close);
      Main_Window.Enable_Callback (Glfw.Windows.Callbacks.Mouse_Position);
      Main_Window.Enable_Callback (Glfw.Windows.Callbacks.Mouse_Button);
      Glfw.Windows.Context.Make_Current (Main_Window);
   end Open_Window;

   procedure Swap_Buffers is
   begin
      Glfw.Windows.Context.Swap_Buffers (Main_Window);
   end Swap_Buffers;

   procedure Poll_Events is
   begin
      Glfw.Input.Poll_Events;
   end Poll_Events;

   procedure Wait_For_Events is
   begin
      Glfw.Input.Wait_For_Events;
   end Wait_For_Events;

   procedure Set_Window_Title (Value : String) is
   begin
      Main_Window.Set_Title (Value);
   end Set_Window_Title;

   function Escape_Pressed return Boolean is
      use type Glfw.Input.Button_State;
   begin
      return Main_Window.Initialized and then
        Main_Window.Key_State (Glfw.Input.Keys.Escape) = Glfw.Input.Pressed;
   end Escape_Pressed;

   function Up_Pressed return Boolean is
      use type Glfw.Input.Button_State;
   begin
      if Main_Window.Initialized then
        if Main_Window.Key_State (Glfw.Input.Keys.W) = Glfw.Input.Pressed or
           Main_Window.Key_State (Glfw.Input.Keys.Up) = Glfw.Input.Pressed then
          return True;
        else
          return False;
        end if;
      else
        return False;
      end if;

      --return Main_Window.Initialized and then
       -- Main_Window.Key_State (Glfw.Input.Keys.W) = Glfw.Input.Pressed;
   end Up_Pressed;

   function Down_Pressed return Boolean is
      use type Glfw.Input.Button_State;
   begin
      if Main_Window.Initialized then
        if Main_Window.Key_State (Glfw.Input.Keys.S) = Glfw.Input.Pressed or
           Main_Window.Key_State (Glfw.Input.Keys.Down) = Glfw.Input.Pressed then
          return True;
        else
          return False;
        end if;
      else
        return False;
      end if;
   end Down_Pressed;

   function Left_Pressed return Boolean is
      use type Glfw.Input.Button_State;
   begin
      if Main_Window.Initialized then
        if Main_Window.Key_State (Glfw.Input.Keys.A) = Glfw.Input.Pressed or
           Main_Window.Key_State (Glfw.Input.Keys.Left) = Glfw.Input.Pressed then
          return True;
        else
          return False;
        end if;
      else
        return False;
      end if;
   end Left_Pressed;

   function Right_Pressed return Boolean is
      use type Glfw.Input.Button_State;
   begin
      if Main_Window.Initialized then
        if Main_Window.Key_State (Glfw.Input.Keys.D) = Glfw.Input.Pressed or
           Main_Window.Key_State (Glfw.Input.Keys.Right) = Glfw.Input.Pressed then
          return True;
        else
          return False;
        end if;
      else
        return False;
      end if;
   end Right_Pressed;

   function Window_Opened return Boolean is
   begin
      return Main_Window.Initialized and then Main_Window.Visible;
   end Window_Opened;

   procedure Close_Window is
   begin
      Main_Window.Destroy;
   end Close_Window;

   procedure Shutdown renames Glfw.Shutdown;

   procedure Configure_Minimum_OpenGL_Version (Major, Minor : Natural) is
   begin
      Glfw.Windows.Hints.Set_Minimum_OpenGL_Version (Major, Minor);
      -- needed for OSX
      if Major >= 3 then
         Glfw.Windows.Hints.Set_Forward_Compat (True);
         Glfw.Windows.Hints.Set_Profile (Glfw.Windows.Context.Core_Profile);
      end if;
   end Configure_Minimum_OpenGL_Version;

end Window;
