with Window;

package body Player is

  procedure Draw is
  begin
    if PlayerMoved then
      for i in old_coords.x .. old_coords.x + Screen.g_tilesize loop
        for j in old_coords.y .. old_coords.y + Screen.g_tilesize loop
          Screen.Put_Pixel(x => i, y => j, pix => Screen.background_color);
        end loop;
      end loop;
      old_coords := coords;
    end if;

    for i in coords.x .. coords.x + Screen.g_tilesize loop
      for j in coords.y .. coords.y + Screen.g_tilesize loop
        Screen.Put_Pixel(x => i, y => j, pix => player_color);
      end loop;
    end loop;

  end Draw;

  procedure ProcessInput is
  begin
    old_coords := coords;
    if Window.Up_Pressed then
      coords.y := coords.y + move_speed;
    elsif Window.Down_Pressed then
      coords.y := coords.y - move_speed;
    end if;
    if Window.Left_Pressed then
      coords.x := coords.x - move_speed;
    elsif Window.Right_Pressed then
      coords.x := coords.x + move_speed;
    end if;
  end ProcessInput;

  function PlayerMoved return Boolean is
  begin
    if coords.x = old_coords.x and coords.y = old_coords.y then
      return False;
    else
      return True;
    end if;

  end PlayerMoved;


end Player;
