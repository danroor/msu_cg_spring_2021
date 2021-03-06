#include "common.h"
#include "Image.h"
#include "Player.h"

#include <vector>
#include <map>
#include <iostream>
#include <stdio.h>
#include <string>

#define GLFW_DLL
#include <GLFW/glfw3.h>

constexpr GLsizei WINDOW_WIDTH  = 1024,
                  WINDOW_HEIGHT = 1024,
                  BORDER_MARGIN = 4;

constexpr int X_TILES = WINDOW_WIDTH  / tileSize,
              Y_TILES = WINDOW_HEIGHT / tileSize;

constexpr int N_LEVELS = 2;
constexpr int ANIMATION_FREQUENCY = 50;
constexpr int SMASH_COOLDOWN = 100; // wall smashing cooldown 
                                    // (player is unable to break walls during cooldown)

class LevelMap {

public:

  char get(int x, int y) const {
    return symbols[y][x];
  }

  void set(int x, int y, char c) {
    if (c != ' ' && c != '.' && c != '#' && c != '%' && c != 'b' && c != 'x'&& c != '@') {
      throw std::runtime_error("No such tile");
    }
    symbols[y][x] = c;
  }

  // read map from file
  // returns player starting position
  Point read(const std::string &file) {

    FILE *f = fopen(file.c_str(), "r");
    if (f == nullptr) {
      throw std::runtime_error("Unable to open file");
    }

    char c;
    symbols.resize(Y_TILES);

    int x = 0, y = 0;
    Point starting_pos{ .x = WINDOW_WIDTH / 2, .y = WINDOW_HEIGHT / 2};

    while ( (c = (char) fgetc(f) ) != EOF ) {

      // ignore \n symbols so that a user could
      // write level map in multiple lines
      if (c == '\n') {
        continue;
      }

      if (c == '@') {
        starting_pos.x = x * tileSize;
        starting_pos.y = y * tileSize;
        c = '.'; // player is standing on the floor
      }
      symbols[y].push_back(c);

      x = (x + 1) % X_TILES;
      if (x == 0) {
        y++;
      }
    }

    if (x != 0 || y != Y_TILES) {
      fclose(f);
      throw std::runtime_error("Wrong number of characters in the file");
    }

    fclose(f);
    return starting_pos;
  };

  void draw(Image &screen, std::map <char, Image> &tile) {
    char tile_sym;

    for (int x = 0; x < X_TILES; ++x) {
      for (int y = 0; y < Y_TILES; ++y) {

        tile_sym = symbols[y][x]; 

        tile[ tile_sym ].set_x(x * tileSize);
        tile[ tile_sym ].set_y(y * tileSize);
        tile[ tile_sym ].Draw(screen);
      } 
    }

  };

  void animation(Image &screen, std::map <char, Image> &tile) {
    
    space_animation = (space_animation + 1) % ANIMATION_FREQUENCY;

    if (!space_animation) {

      for (int x = 0; x < X_TILES; ++x) {
        for (int y = 0; y < Y_TILES; ++y) {
          switch (symbols[y][x]) {
            case ' ':
              symbols[y][x] = '*'; 
              tile[ '*' ].set_x(x * tileSize);
              tile[ '*' ].set_y(y * tileSize);
              tile[ '*' ].Draw(screen);
              break;
            case '*':
              symbols[y][x] = ' ';
              tile[ ' ' ].set_x(x * tileSize);
              tile[ ' ' ].set_y(y * tileSize);
              tile[ ' ' ].Draw(screen); 
              break;
            default:
              break;
          }
        }
      }

    }

  }

  void reset() {
    for (auto v : symbols) {
      v.clear();
    }
    symbols.clear();
  };

private:
  std::vector < std::vector <char> > symbols;
  int space_animation = 0;
};

// redraw area near player
void redrawArea(Player &p, Image &screen, LevelMap &Level, std::map <char, Image> &tile) {
  auto coords = p.getCoords();
  int px = coords.x,
      py = coords.y,
      pv = p.getSpeed();

  int lx = px / tileSize - pv,
      rx = px / tileSize + pv,
      uy = py / tileSize + pv,
      dy = py / tileSize - pv;

  lx = lx >= 0 ? lx : 0;
  rx = rx < X_TILES ? rx : X_TILES - 1;
  dy = dy >= 0 ? dy : 0;
  uy = uy < Y_TILES ? uy : Y_TILES - 1;

  //std::cout << px / tileSize << " " << py / tileSize << "\n" << lx << " " << rx << "\n" << dy << " " << uy << "\n\n\n";  

  char tile_sym;
  for (int x = lx; x <= rx; ++x) {
    for (int y = dy; y <= uy; ++y) {
      //std::cout << "SDMVLKSKLSRBJK:SRBNJR:" << std::endl;
      tile_sym = Level.get(x,y); 
      //std::cout << "voknbkjnetjkbnerejba" << std::endl;
      tile[ tile_sym ].set_x(x * tileSize);
      tile[ tile_sym ].set_y(y * tileSize);
      tile[ tile_sym ].Draw(screen);
    }
  }
}

struct InputState
{
  bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата
  GLfloat lastX = 400, lastY = 300; //исходное положение мыши
  bool firstMouse = true;
  bool captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
  bool capturedMouseJustNow = false;
} static Input;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	switch (key)
	{
  case GLFW_KEY_1:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case GLFW_KEY_2:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
	default:
		if (action == GLFW_PRESS) {
      Input.keys[key] = true;
      if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GL_TRUE);
      }
    }
		else if (action == GLFW_RELEASE)
      Input.keys[key] = false;
	}
}

bool mayGo(int x, int y, MovementDir dir, LevelMap &Level) {

  // if % tileSize != 0
  // then two blocks above/below/to the left/to the right
  // are on the player's way
  // otherwise there is only one obstacle
  bool x2 = x % tileSize != 0,
       y2 = y % tileSize != 0;

  int xt = x / tileSize, // x_tiles
      yt = y / tileSize; // y_tiles

  switch(dir)
  {
    case MovementDir::UP:
      if (y + tileSize >= WINDOW_HEIGHT - BORDER_MARGIN) {
        return false;
      }

      if (Level.get(xt, yt + 1) == '#' || Level.get(xt, yt + 1) == '%') {
        return false;
      }

      if (x2 && (Level.get(xt + 1, yt + 1) == '#' || Level.get(xt + 1, yt + 1) == '%') ) {
        return false;
      }

      break;

    case MovementDir::DOWN:
      if (y <= BORDER_MARGIN) {
        return false;
      }

      // the player is far enough from the next tile below
      if (y2) {
        return true;
      }

      if (Level.get(xt, yt - 1) == '#' || Level.get(xt, yt - 1) == '%') {
        return false;
      }

      if (x2 && (Level.get(xt + 1, yt - 1) == '#' || Level.get(xt + 1, yt - 1) == '%') ) {
        return false;
      }

      break;
    
    case MovementDir::LEFT:
      if (x <= BORDER_MARGIN) {
        return false;
      }

      // the player is far enough from the next tile to the left
      if (x2) {
        return true;
      }

      if (Level.get(xt - 1, yt) == '#' || Level.get(xt - 1, yt) == '%') {
        return false;
      }

      if (y2 && (Level.get(xt - 1, yt + 1) == '#' || Level.get(xt - 1, yt + 1) == '%') ) {
        return false;
      }

      break;
    
    case MovementDir::RIGHT:
      if (x + tileSize >= WINDOW_WIDTH - BORDER_MARGIN) {
        return false;
      }

      if (Level.get(xt + 1, yt) == '#' || Level.get(xt + 1, yt) == '%') {
        return false;
      }

      if (y2 && (Level.get(xt + 1, yt + 1) == '#' || Level.get(xt + 1, yt + 1) == '%') ) {
        return false;
      }

      break;
    
    default:
      break;
  }

  return true;
}

void breakWall(Player &player, LevelMap &Level) {
  // check the tile the player is standing on right now
  auto coords = player.getCoords();
  int x = coords.x;
  int y = coords.y;

  // choosing the tile covered by the
  // biggest fraction of player tile's
  // square area
  x = x / tileSize + int(x % tileSize > tileSize / 2);
  y = y / tileSize + int(y % tileSize > tileSize / 2);

  if (Level.get(x - 1, y) == '%') {
    Level.set(x - 1,y,'b');
    player.smash_cooldown = SMASH_COOLDOWN;
  }
  if (Level.get(x + 1, y) == '%') {
    Level.set(x + 1,y,'b');
    player.smash_cooldown = SMASH_COOLDOWN;
  }
  if (Level.get(x, y - 1) == '%') {
    Level.set(x,y - 1,'b');
    player.smash_cooldown = SMASH_COOLDOWN;
  }
  if (Level.get(x, y + 1) == '%') {
    Level.set(x,y + 1,'b');
    player.smash_cooldown = SMASH_COOLDOWN;
  }
}

void processPlayerMovement(Player &player, LevelMap &Level) {
  auto coords = player.getCoords();
  int x = coords.x;
  int y = coords.y;

  if (player.smash_cooldown > 0) {
    player.smash_cooldown--;
  }

  if (Input.keys[GLFW_KEY_W]) { 
    auto dir = MovementDir::UP;
    if (mayGo(x,y,dir,Level)) {
      player.ProcessInput(dir);
    }
  }

  if (Input.keys[GLFW_KEY_S]) {
    auto dir = MovementDir::DOWN;
    if (mayGo(x,y,dir,Level)) {
      player.ProcessInput(dir);
    }
  }
  
  if (Input.keys[GLFW_KEY_A]) {
    auto dir = MovementDir::LEFT;
    if (mayGo(x,y,dir,Level)) {
      player.ProcessInput(dir);
    }
    player.changeDir(MovementDir::LEFT);
  }
  
  if (Input.keys[GLFW_KEY_D]) {
    auto dir = MovementDir::RIGHT;
    if (mayGo(x,y,dir,Level)) {
      player.ProcessInput(dir);
    }
    player.changeDir(MovementDir::RIGHT);
  }

  if (Input.keys[GLFW_KEY_SPACE] && player.smash_cooldown == 0) {
    breakWall(player, Level);
  }

  // check the tile the player is standing on right now
  coords = player.getCoords();
  x = coords.x;
  y = coords.y;

  // choosing the tile covered by the
  // biggest fraction of player tile's
  // square area
  x = x / tileSize + int(x % tileSize > tileSize / 2);
  y = y / tileSize + int(y % tileSize > tileSize / 2);

  switch (Level.get(x,y))  {
    case ' ':
    case '*':
      player.status = playerStatus::DEAD;
      break;
    case 'x':
      player.status = playerStatus::ESCAPED;
      break;
    default:
      break;
  }
  
}

void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    Input.captureMouse = !Input.captureMouse;

  if (Input.captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Input.capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (Input.firstMouse)
  {
    Input.lastX = float(xpos);
    Input.lastY = float(ypos);
    Input.firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - Input.lastX;
  GLfloat yoffset = Input.lastY - float(ypos);

  Input.lastX = float(xpos);
  Input.lastY = float(ypos);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  // ...
}


int initGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  std::cout << "Controls: "<< std::endl;
  std::cout << "press right mouse button to capture/release mouse cursor  "<< std::endl;
  std::cout << "W, A, S, D - movement  "<< std::endl;
  std::cout << "Spacebar - break green wall  "<< std::endl;
  std::cout << "press ESC to exit" << std::endl;

	return 0;
}

void Win(Image &screen, Image &victory, LevelMap &Level, std::map <char, Image> &tile, Player &player, GLFWwindow*  window) {
  victory.Draw(screen);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
  glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screen.Data()); GL_CHECK_ERRORS;
  glfwSwapBuffers(window);
  while (!Input.keys[GLFW_KEY_R] && !Input.keys[GLFW_KEY_ESCAPE]) {
    glfwPollEvents();
  }

  Level.reset();
  Point starting_pos;
  starting_pos = Level.read("../resources/levels/1.txt");   

  player.setPos(starting_pos.x, starting_pos.y);
  player.setOldPos(starting_pos.x, starting_pos.y);
  player.status = playerStatus::OK;
  player.smash_cooldown = 0;

  Level.draw(screen, tile);
}

void gameOver(Image &screen, Image &game_over, LevelMap &Level, std::map <char, Image> &tile, Player &player, Point starting_pos, GLFWwindow*  window) {
  game_over.Draw(screen);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
  glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screen.Data()); GL_CHECK_ERRORS;
  glfwSwapBuffers(window);
  while (!Input.keys[GLFW_KEY_R] && !Input.keys[GLFW_KEY_ESCAPE]) {
    glfwPollEvents();
  }

  // replaying current level

  // restoring broken walls
  for (int x = 0; x < X_TILES; ++x) {
    for (int y = 0; y < Y_TILES; ++y) {
      if (Level.get(x,y) == 'b') {
        Level.set(x,y,'%');
      }
    }
  }

  // restoring starting position and status
  player.status = playerStatus::OK;
  player.smash_cooldown = 0;
  player.setPos(starting_pos.x, starting_pos.y);
  player.setOldPos(starting_pos.x, starting_pos.y);

  Level.draw(screen, tile);
}

void nextLevel(Image &screen, Image &next_level, LevelMap &Level, std::map <char, Image> &tile, Player &player, GLFWwindow*  window, int curLevel) {

  next_level.Draw(screen);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
  glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screen.Data()); GL_CHECK_ERRORS;
  glfwSwapBuffers(window);
  while (!Input.keys[GLFW_KEY_P] && !Input.keys[GLFW_KEY_ESCAPE]) {
    glfwPollEvents();
  }

  Level.reset();
  Point starting_pos;
  starting_pos = Level.read("../resources/levels/" + std::to_string(curLevel) + ".txt");   

  player.setPos(starting_pos.x, starting_pos.y);
  player.setOldPos(starting_pos.x, starting_pos.y);
  player.status = playerStatus::OK;
  player.smash_cooldown = 0;

  Level.draw(screen, tile);
}

int main(int argc, char** argv)
{
	if(!glfwInit())
    return -1;

//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow*  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "IGRA GODA POEDU S NEI NA E3", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); 

	glfwSetKeyCallback        (window, OnKeyboardPressed);  
	glfwSetCursorPosCallback  (window, OnMouseMove); 
  glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
	glfwSetScrollCallback     (window, OnMouseScroll);

	if(initGL() != 0) 
		return -1;
	  
  //Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

	Image screen(WINDOW_WIDTH, WINDOW_HEIGHT, 4);

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;

  Image game_over("../resources/tiles/game_over.png");
  Image next_level("../resources/tiles/next_level.png");
  Image victory("../resources/tiles/victory.png");
  
  game_over.set_x(220);
  game_over.set_y(350);

  next_level.set_x(220);
  next_level.set_y(350);

  victory.set_x(220);
  victory.set_y(350);

  auto tile = std::map <char, Image>();


  tile[' '] = Image("../resources/tiles/floor.png");
  tile['*'] = Image("../resources/tiles/floor.png");
  tile['x'] = Image("../resources/tiles/floor.png");
  tile['#'] = Image("../resources/tiles/floor.png");
  tile['%'] = Image("../resources/tiles/floor.png");
  tile['b'] = Image("../resources/tiles/floor.png");
  tile['.'] = Image("../resources/tiles/floor.png");


  // drawing everything on the floor
  Image("../resources/tiles/space_1.png").Draw(tile[' ']);
  Image("../resources/tiles/space_2.png").Draw(tile['*']);
  Image("../resources/tiles/exit.png").Draw(tile['x']);
  Image("../resources/tiles/unbreakable_wall.png").Draw(tile['#']);
  Image("../resources/tiles/breakable_wall.png").Draw(tile['%']); 
  // broken wall appears
  // after breaking a breakable wall.
  Image("../resources/tiles/broken_wall.png").Draw(tile['b']);


  Point starting_pos;
  LevelMap Level;

  try {
    starting_pos = Level.read("../resources/levels/1.txt");    
  } catch (std::runtime_error &exc) {
    std::cout << exc.what() << std::endl;
    glfwTerminate();
    return 0;
  }

  Image left = Image("../resources/tiles/floor.png");
  Image right = Image("../resources/tiles/floor.png");

  Image("../resources/tiles/knight_left.png").Draw(left);
  Image("../resources/tiles/knight_right.png").Draw(right);

  Player player(starting_pos, left, right);

  Level.draw(screen, tile);
  int curLevel = 1;

  //game loop
	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
    glfwPollEvents();

    processPlayerMovement(player, Level);
    if (player.Moved() || player.smash_cooldown == SMASH_COOLDOWN) {
      redrawArea(player, screen, Level, tile);      
    }
    Level.animation(screen, tile);

    player.Draw(screen);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;

    glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screen.Data()); GL_CHECK_ERRORS;

    if (player.status == playerStatus::ESCAPED) {
      curLevel++;
      if (curLevel > N_LEVELS) {
        Win(screen, victory, Level, tile, player, window);
        continue;
      } 

      try { 
        nextLevel(screen, next_level, Level, tile, player, window, curLevel);
      } catch (std::runtime_error &exc) {
        std::cout << exc.what() << std::endl;
        glfwTerminate();
        return 0;
      }
  
    }

    if (player.status == playerStatus::DEAD) {
      gameOver(screen, game_over, Level, tile, player, starting_pos, window);
    }

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
