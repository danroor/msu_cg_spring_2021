with Screen;

package Player is

  type coords_t is record
    x, y : Integer;
  end record;

  coords: coords_t := (x => Screen.g_width / 2, y => Screen.g_height / 2);

  old_coords: coords_t := coords;

  player_color: Screen.pixel := (255, 255, 0, 0);

  move_speed: Integer := 4;

  procedure ProcessInput;
  procedure Draw;

  function PlayerMoved return Boolean;

end Player;
