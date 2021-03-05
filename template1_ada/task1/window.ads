
package Window is


   procedure Init;

   procedure Open_Window (Width, Height : Natural; Depth_Bits : Natural := 0);

 -- procedure Mouse_Position_Changed (Object : not null access Main_Window_Type;
 --                                   X, Y   : Glfw.Input.Mouse.Coordinate);

 -- procedure Mouse_Button_Changed (Object : not null access Main_Window_Type;
 --                                 Button  : Glfw.Input.Mouse.Button;
 --                                 State   : Glfw.Input.Button_State;
 --                                 Mods    : Glfw.Input.Keys.Modifiers
 --                                 );

   procedure Swap_Buffers;

   procedure Poll_Events;

   procedure Wait_For_Events;

   procedure Set_Window_Title (Value : String);

   function Escape_Pressed return Boolean;

   function Up_Pressed return Boolean;
   function Down_Pressed return Boolean;
   function Left_Pressed return Boolean;
   function Right_Pressed return Boolean;

   function Window_Opened return Boolean;

   procedure Close_Window;

   procedure Shutdown;

   procedure Configure_Minimum_OpenGL_Version (Major, Minor : Natural);

end Window;
