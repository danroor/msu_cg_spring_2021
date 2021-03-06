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
  explicit Player(Point pos, Image &l, Image &r) :
                 coords(pos), old_coords(coords) {

    left = l;
    right = r;
  };

  bool Moved() const;
  void ProcessInput(MovementDir dir);
  void Draw(Image &screen);

  Point getCoords() { return coords; }
  int getSpeed() { return move_speed; }
  void setPos(int x, int y) {
    coords.x = x;
    coords.y = y;
  }
  void setOldPos(int x, int y) {
    old_coords.x = x;
    old_coords.y = y;
  }
  void changeDir(MovementDir new_dir) {
    dir = new_dir;
  }

  playerStatus status = playerStatus::OK;
  int smash_cooldown = 0;

private:
  Point coords {.x = 10, .y = 10};
  Point old_coords {.x = 10, .y = 10};
  Pixel color {.r = 255, .g = 0, .b = 0, .a = 255};
  int move_speed = 4;
  Image left, right;
  MovementDir dir = MovementDir::LEFT;
};

#endif //MAIN_PLAYER_H
