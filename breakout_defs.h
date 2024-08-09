/* --------------------------------------------------------
 *    File: breakout_defs.h
 *  Author: Vasil Ivanov
 *   Class: COP 2001 CRN 10410
 * Purpose: definitions for breakout game
 * -------------------------------------------------------- */
#include <SFML/Graphics.hpp>

#ifndef PADDLE_BREAKOUT_DEFS_H
#define PADDLE_BREAKOUT_DEFS_H

// Global Constants
// --------------------------------------------------------

// window properties
const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const sf::Color WINDOW_COLOR = sf::Color::White;

// drawing properties
const float BALL_RADIUS = 15.0;
const float FRAME_RATE = (1.0/30.0) * 1000.0;       // FPS in ms
const sf::Color BALL_COLOR = sf::Color::Blue;

// These are just for fun
// speed that the ball can accelerate at to span window in
// n-seconds (in ms) broken up for each frame
const float SPEED_TIME = (3.0 * 1000.0) * 30.0;             //
const float BALL_SPEED_X = BALL_RADIUS * 20.0 / 1000.0;     // speed horizontally
const float BALL_SPEED_Y = BALL_RADIUS * 17.0 / 1000.0;     // span  vertically

// wall constants
const float WALL_THICKNESS = 10.0;
const sf::Color WALL_COLOR = sf::Color::Red;

// paddle constants
const float PADDLE_WIDTH = 160.0;
const float PADDLE_THICKNESS = 10.0;
const sf::Color PADDLE_COLOR = sf::Color::Black;
const float PADDLE_SPEED = PADDLE_WIDTH / 10.0 / 1000.0;

// --- brick constants ---
const int BRICK_ROWS = 8; // rows of bricks in rack
const int BRICK_COLUMNS = 14; // number of bricks in row
const float BRICK_WIDTH = WINDOW_WIDTH / BRICK_COLUMNS; // width of 1 brick
const float BRICK_HEIGHT = PADDLE_THICKNESS * 2; // height of 1 brick (2 x paddle)
const float BRICKS_HEIGHT = BRICK_ROWS * BRICK_HEIGHT; // height of rack of bricks
// top of the rack (shift up 3/4 from center)
const float BRICKS_TOP = WINDOW_HEIGHT / 2.0 - BRICKS_HEIGHT * 0.75;
const float BRICKS_LEFT = WALL_THICKNESS; // left of first brick in each row
// top of lowest brick in rack
const float FIRST_BRICK = BRICKS_TOP + (BRICK_ROWS - 1) * BRICK_HEIGHT;


// Type definitions
// --------------------------------------------------------
enum Direction {
    Exit = -1,
    None,
    Left,
    Up,
    Right,
    Down,
    Start
};

// ball properties
struct Ball {
    float radius;
    float coordinateX;
    float coordinateY;
    float velocityX;
    float velocityY;
    sf::Color color;
};

// block properties
struct Block {
    float left;
    float top;
    float width;
    float height;
    sf::Color color;
    sf::RectangleShape rectangle;
};

// moving block properties
struct MovingBlock {
    Block block;
    float velocityX;
    float velocityY;
};

// brick properties
struct Brick {
    Block block;
    bool hit;
    int points;
    float speedAdjust;
};

// border properties
struct Borders {
    Block leftWall;
    Block topWall;
    Block rightWall;
    Block bottomWall;
};

#endif //PADDLE_BREAKOUT_DEFS_H
