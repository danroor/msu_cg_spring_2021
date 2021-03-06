#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"

struct Point
{
  int x;
  int y;
};

enum class MovementDir
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

enum class playerStatus {
  OK, DEAD, ESCAPED
};

struct Player
{
  explicit Player(Point pos = {.x = 10, .y = 10}) :
                 coords(pos), old_coords(coords) {};

  bool Moved() const;
  void ProcessInput(MovementDir dir);
  void Draw(Image &screen);

  Point getCoords() { return coords; }
  int getSpeed() { return move_speed; }
  void setPos(int x, int y) {
    coords.x = x;
    coords.y = y;
  }

  playerStatus status = playerStatus::OK;

private:
  Point coords {.x = 10, .y = 10};
  Point old_coords {.x = 10, .y = 10};
  Pixel color {.r = 255, .g = 0, .b = 0, .a = 0};
  int move_speed = 4;

};

#endif //MAIN_PLAYER_H
