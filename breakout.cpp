/* --------------------------------------------------------
 *    File: breakout.cpp
 *  Author: Vasil Ivanov
 *   Class: COP 2001 - CRN 10410
 * Purpose: 2-D Breakout arcade game
 * Audit:
 * 3.15.22 VI built and configured SFML with CLion IDE
 * 3.22.22 VI added basic directional movement
 * 3.27.22 VI added boundary walls to play area
 * 4.14.22 VI added collision detections
 * 4.19.22 VI added controllable paddle
 * 5.3.22  VI added bricks, paddle collision with side walls,
 *           and a playable game
 * -------------------------------------------------------- */

#include <cmath>                // trig and linear functions
#include <iostream>             // output of player's score
#include "breakout_defs.h"

using namespace std;            // just to make things a little easier for the score output

// Function declarations (prototypes)
// --------------------------------------------------------
void setup(Ball& rBall, Borders& rBorders, Brick bricks[BRICK_ROWS][BRICK_COLUMNS], MovingBlock& rPaddle);
Direction processInput() ;
bool update(Direction &input, bool& started, Ball &ball,
            Borders borders, MovingBlock& rPaddle,
            Brick bricks[BRICK_ROWS][BRICK_COLUMNS], float delta,
            int& bricksHit, int& score, bool& hitCount4, bool& hitCount12);
void render(sf::RenderWindow &window,
            Ball &ball, Borders borders, MovingBlock paddle,
            Brick bricks[BRICK_ROWS][BRICK_COLUMNS], float delta);
int getCollisionPoint(Ball* pBall, Block* pBlock);
bool checkCollision(Ball* pBall, Block* pBlock);
bool checkBlockCollision(Block moving, Block stationary);
bool doCollisionChecks(Ball& ball, MovingBlock& paddle,
                       Brick bricks[BRICK_ROWS][BRICK_COLUMNS],
                       Borders borders, int& bricksHit, int& score);


/**
 * The main application
 * @return OS status message (0=Success)
 */
int main() {

    // create a 2d graphics window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Hello SFML");
    window.clear(WINDOW_COLOR);

    // declare borders, ball, bricks, and paddle variables
    Borders theWalls;
    Ball theBall;
    Brick theBricks[BRICK_ROWS][BRICK_COLUMNS];
    MovingBlock paddle;

    // define borders, ball, and paddle properties
    setup(theBall, theWalls, theBricks, paddle);

    // EXTRA: define variables for number of bricks hit
    int bricksHit = 0;
    bool hitCount4 = false;
    bool hitCount12 = false;

    // EXTRA: define score variable
    int score = 0;

    // timing variables for the main game loop
    sf::Clock clock;
    sf::Time startTime = clock.getElapsedTime();
    sf::Time stopTime = startTime;
    float delta = 0.0;

    bool started = false;
    bool gameOver = false;
    while (!gameOver)
    {
        // calculate frame time
        stopTime = clock.getElapsedTime();
        delta += (stopTime.asMilliseconds() - startTime.asMilliseconds());
        startTime = stopTime;

        // process events
        sf::Event event;
        while (!gameOver && window.pollEvent(event)) {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                gameOver = true;
        }

        // Process user input
        // ------------------------------------------------
        Direction userInput = processInput();
        if (userInput == Exit)
            gameOver = true;

        // Process Updates
        // ------------------------------------------------
        if (delta >= FRAME_RATE) {    // if we have made it at least a full frame time

            gameOver = update(userInput, started, theBall, theWalls, paddle,
                              theBricks, delta, bricksHit, score,
                              hitCount4, hitCount12);

            // subtract the frame-rate from the current frame-time
            // for each full frame covered by this update
            while (delta >= FRAME_RATE)
                delta -= FRAME_RATE;
        }

        // Render the window
        // ------------------------------------------------
        render(window, theBall, theWalls, paddle, theBricks, delta);

    } // end main game loop

    // game ending, close the graphics window
    window.close();

    // EXTRA: print score to console
    cout << "Your score:  " << score << endl;

    return 0;   // return success to the OS
} // end main


/**
 * Define properties of borders, game ball, game bricks, and player-controlled paddle
 * @param rBall - defines radius, x- and y-coordinates for position, starting velocity, and color
 * @param rBorders - defines wall's position, width, height, color, and rectangle-shape properties
 * of each boundary wall
 * @param bricks - defines bricks' positions, widths, heights, colors, rectangle-shape properties,
 * point-values, and speed adjusting values
 * @param rPaddle - defines paddle's position, width, height, color, rectangle-shape properties,
 * and initial velocity
 */
void setup(Ball& rBall, Borders& rBorders, Brick bricks[BRICK_ROWS][BRICK_COLUMNS], MovingBlock& rPaddle) {
    // define the border walls properties
    // left wall
    rBorders.leftWall.left = 0.0;
    rBorders.leftWall.top = 0.0;
    rBorders.leftWall.width = WALL_THICKNESS;
    rBorders.leftWall.height = WINDOW_HEIGHT;
    rBorders.leftWall.color = WALL_COLOR;
    rBorders.leftWall.rectangle.setSize(sf::Vector2f(rBorders.leftWall.width, rBorders.leftWall.height));
    rBorders.leftWall.rectangle.setPosition(rBorders.leftWall.left, rBorders.leftWall.top);
    rBorders.leftWall.rectangle.setFillColor(WALL_COLOR);

    // top wall
    rBorders.topWall.left = 0.0;
    rBorders.topWall.top = 0.0;
    rBorders.topWall.width = WINDOW_WIDTH;
    rBorders.topWall.height = WALL_THICKNESS;
    rBorders.topWall.color = WALL_COLOR;
    rBorders.topWall.rectangle.setSize(sf::Vector2f(rBorders.topWall.width, rBorders.topWall.height));
    rBorders.topWall.rectangle.setPosition(rBorders.topWall.left, rBorders.topWall.top);
    rBorders.topWall.rectangle.setFillColor(WALL_COLOR);

    // right wall
    rBorders.rightWall.left = WINDOW_WIDTH - WALL_THICKNESS;
    rBorders.rightWall.top = 0.0;
    rBorders.rightWall.width = WALL_THICKNESS;
    rBorders.rightWall.height = WINDOW_HEIGHT;
    rBorders.rightWall.color = WALL_COLOR;
    rBorders.rightWall.rectangle.setSize(sf::Vector2f(rBorders.rightWall.width, rBorders.rightWall.height));
    rBorders.rightWall.rectangle.setPosition(rBorders.rightWall.left, rBorders.rightWall.top);
    rBorders.rightWall.rectangle.setFillColor(WALL_COLOR);

    // bottom wall
    rBorders.bottomWall.left = 0.0;
    rBorders.bottomWall.top = WINDOW_HEIGHT - WALL_THICKNESS;
    rBorders.bottomWall.width = WINDOW_WIDTH;
    rBorders.bottomWall.height = WALL_THICKNESS;
    rBorders.leftWall.color = WALL_COLOR;
    rBorders.bottomWall.rectangle.setSize(sf::Vector2f(rBorders.bottomWall.width, rBorders.bottomWall.height));
    rBorders.bottomWall.rectangle.setPosition(rBorders.bottomWall.left, rBorders.bottomWall.top);
    rBorders.bottomWall.rectangle.setFillColor(WALL_COLOR);


    // define the paddle parameters
    // general block properties
    rPaddle.block.left = (WINDOW_WIDTH - PADDLE_WIDTH) / 2.0;
    rPaddle.block.top = WINDOW_HEIGHT - (2.0 * PADDLE_THICKNESS);
    rPaddle.block.width = PADDLE_WIDTH;
    rPaddle.block.height = PADDLE_THICKNESS;
    rPaddle.block.color = PADDLE_COLOR;
    // rectangle properties
    rPaddle.block.rectangle.setSize(sf::Vector2f(rPaddle.block.width, rPaddle.block.height));
    rPaddle.block.rectangle.setFillColor(PADDLE_COLOR);
    // velocity properties
    rPaddle.velocityX = 0.0;
    rPaddle.velocityY = 0.0;


    // define the ball properties
    rBall.radius = BALL_RADIUS;
    rBall.coordinateX = rPaddle.block.left + (rPaddle.block.width / 2.0);
    rBall.coordinateY = rPaddle.block.top - rBall.radius - 1;
    rBall.velocityX = 0.0;
    rBall.velocityY = 0.0;
    rBall.color = BALL_COLOR;

    // define the bricks properties
    float bricksTop = FIRST_BRICK; // start at lowest brick row

    Brick* pNext = &bricks[0][0]; // get pointer to 1st brick
    for (int row = 0; row < BRICK_ROWS; row++) {

        float bricksLeft = BRICKS_LEFT; // start back at far left

        for (int column = 0; column < BRICK_COLUMNS; column++) {

            pNext->block.left = bricksLeft + 1; // 1 pixel right to leave blank pixel on left

            // offset left/top by 1 pixel to give each brick a border
            pNext->block.left = bricksLeft + 1;
            pNext->block.top = bricksTop + 1;

            // subtract 2 from width/height to make room for the 1-pixel border on each side
            pNext->block.width = BRICK_WIDTH - 2;
            pNext->block.height = BRICK_HEIGHT - 2;

            // set row specific properties
            if (row < 2) {
                pNext->block.color = sf::Color::Yellow;
                pNext->points = 1;
                pNext->speedAdjust = 0;
            }
            else if (row < 4) {
                pNext->block.color = sf::Color::Green;
                pNext->points = 3;
                pNext->speedAdjust = 0;
            }
            else if (row < 6) {
                pNext->block.color = sf::Color(255,165,0); // Orange
                pNext->points = 5;
                pNext->speedAdjust = 1;
            }
            else {
                pNext->block.color = sf::Color::Red;
                pNext->points = 7;
                pNext->speedAdjust = 2;
            }

            // set the drawing rectangle
            pNext->block.rectangle.setSize(sf::Vector2f(pNext->block.width, pNext->block.height));
            pNext->block.rectangle.setFillColor(pNext->block.color);
            pNext->block.rectangle.setPosition(pNext->block.left, pNext->block.top);

            // initialize hit flag off so brick is displayed
            pNext->hit = false;

            pNext++; // get next brick

            bricksLeft += BRICK_WIDTH; // move next brick to right

        } // brick columns

        bricksTop -= BRICK_HEIGHT; // move up to next row
    } // brick rows

} // end setup


/**
 * Calculate and return a cardinal (North, East, South, West) heading in degrees for the point on the outside of the
 * ball closest to the check point calculated between ball's center and rectangle
 * @param pBall  - pointer to Ball controlled on screen
 * @param pBlock - pointer to Block being tested against
 * @return int - cardinal heading in degrees (returns 0 if no collision occurs)
 */
int getCollisionPoint(Ball* pBall, Block* pBlock) {
    int heading = 0; // default, no collision occurs

    float checkX;
    // --- if ball is to left of block ---
    if(pBall->coordinateX < pBlock->left)
        checkX = pBlock->left;
        // --- if ball is to right of block ---
    else if(pBall->coordinateX > pBlock->left + pBlock->width)
        checkX = pBlock->left + pBlock->width;
        // --- ball is between block's left and right points ---
    else
        checkX = pBall->coordinateX;

    float checkY;
    // --- if ball above block ---
    if(pBall->coordinateY < pBlock->top) // < because y starts at 0 on top and increases going down window
        checkY = pBlock->top;
        // --- if ball is below block ---
    else if(pBall->coordinateY > pBlock->top + pBlock->height)
        checkY = pBlock->top + pBlock->height;
        // --- ball is between block's top and bottom points ---
    else
        checkY = pBall->coordinateY;

    // horizontal difference between check point and ball's center
    float differenceX = checkX - pBall->coordinateX;
    // vertical difference between check point and ball's center
    float differenceY = (WINDOW_HEIGHT - checkY) - (WINDOW_HEIGHT - pBall->coordinateY);

    // some Pythagorean theorem to get distance between ball and check point
    double distance = std::sqrt(pow(differenceX, 2.0) + pow(differenceY, 2.0));
    if(distance <= pBall->radius) {
        // calculate theta using an arc tangent function
        double theta = std::atan2(differenceY, differenceX);
        // turn theta into an angle in degrees
        double degrees = 90.0 - theta * 180 / M_PI;
        if(degrees <= 0)
            degrees += 360;
        heading = int(degrees);
    }

    return heading;
}


/**
 * Check if a collision was detected.  If one was, flip the ball's velocity so it doesn't continue heading towards
 * the block (make it "bounce off" the block).
 * @param pBall  - pointer to Ball controlled on screen
 * @param pBlock - pointer to Block to get x and y values for calculations
 * @return bool - returns true if collision was detected, false if no collision
 */
bool checkCollision(Ball* pBall, Block* pBlock) {
    bool collision = false; // default

    // get angle that ball collides with block
    int heading = getCollisionPoint(pBall, pBlock);
    // if there is a collision happening (heading != 0)
    if(heading) {
        collision = true;
        if(heading > 225 && heading < 315) {
            // if ball's left side hits block, make ball bounce off and head right
            pBall->velocityX *= -1;
            pBall->coordinateX = pBlock->left + pBlock->width + pBall->radius + 1;
        }
        else if(heading > 45 && heading < 135) {
            // if ball's right side hits block, make ball bounce off and head left
            pBall->velocityX *= -1;
            pBall->coordinateX = pBlock->left - pBall->radius - 1;
        }

        if(heading >= 315 || heading <= 45) {
            // if ball's top side hits block, make ball bounce off and head down
            pBall->velocityY *= -1;
            pBall->coordinateY = pBlock->top + pBlock->height + pBall->radius + 1;
        }
        else if(heading >= 135 && heading <= 225) {
            // if ball's bottom side hits block, make ball bounce off and head up
            pBall->velocityY *= -1;
            pBall->coordinateY = pBlock->top - pBall->radius - 1;
        }
    }

    return collision;
}


/**
 * Check if a moving block (like a paddle) collides with a stationary block (like a wall)
 * @param moving - moving block, such as a paddle
 * @param stationary - stationary block, such as a wall
 * @return bool - returns true if collision was detected, false if no collision
 */
bool checkBlockCollision(Block moving, Block stationary) {
    bool collision = false; // default

    // declare local float variables for block properties
    // for moving block...
    float movingLeft = moving.left;
    float movingRight = moving.left + moving.width;
    float movingTop = moving.top;
    float movingBottom = moving.top + moving.height;

    // and for stationary block...
    float stationaryLeft = stationary.left;
    float stationaryRight = stationary.left + stationary.width;
    float stationaryTop = stationary.top;
    float stationaryBottom = stationary.top + stationary.height;

    // decide if collision occurred
    if (movingLeft < stationaryRight &&
        movingRight > stationaryLeft &&
        movingTop < stationaryBottom &&
        movingBottom > stationaryTop) {
        collision = true;
    }

    return collision;
}


/**
 * This function holds and handles all of the different types of collision checks that may occur while playing the game
 * @param ball - game ball, used for brick collisions and wall collisions
 * @param paddle - player-controlled paddle, used for ball collisions and side wall collisions
 * @param bricks - breakable bricks, used for ball collisions (they "break" if the ball collides with them)
 * @param borders - play-window borders, used for paddle collisions and ball collisions
 * @param bricksHit - number of bricks hit, incremented for every brick hit
 * @param score - player's score; brick's points value gets added when said brick gets hit
 * @return bool - returns status of game-running (true = game over, false = game's still running)
 */
bool doCollisionChecks(Ball& ball, MovingBlock& paddle, Brick bricks[BRICK_ROWS][BRICK_COLUMNS],
                       Borders borders, int& bricksHit, int& score) {
    bool gameOver = false; // default, because the game is running

    // check for ball collisions with the play boundaries
    // --- check vertical collisions ---
    if (!checkCollision(&ball, &paddle.block)) {
        if (!checkCollision(&ball, &borders.topWall)) {
            gameOver = checkCollision(&ball, &borders.bottomWall);
        }
    }

    // --- check horizontal collisions ---
    if (!checkCollision(&ball, &borders.leftWall)) {
        checkCollision(&ball, &borders.rightWall);
    }

    // --- check paddle collision with walls ---
    // first, the left wall...
    if (checkBlockCollision(paddle.block, borders.leftWall)) {
        paddle.velocityX = 0.0;
        paddle.block.left = borders.leftWall.left + borders.leftWall.width + 1.0;
    }
    // then, the right wall if there's no left wall collision...
    else if (checkBlockCollision(paddle.block, borders.rightWall)) {
        paddle.velocityX = 0.0;
        paddle.block.left = borders.rightWall.left - paddle.block.width - 1.0;
    }

    // --- check ball and brick collisions ---
    Brick *pBrick = &bricks[0][0];
    for (int row = 0; row < BRICK_ROWS; row++) {
        for (int column = 0; column < BRICK_COLUMNS; column++) {
            if (!pBrick->hit) {
                pBrick->hit = checkCollision(&ball, &pBrick->block);
                if (pBrick->hit) {
                    bricksHit++; // EXTRA: increment if ball collides with brick
                    score += pBrick->points; // EXTRA: add brick's points value to score
                }
            }
            pBrick++;
        } // columns
    } // rows
    
    return gameOver;
}


/**
 * convert user keyboard input into recognized integer values
 * for left=1/up=2/right=3/down=4
 * @return int - user input (default no-input=0, quit=-1)
 */
Direction processInput() {
    Direction input = None;  // no input

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        input = Left;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        input = Up;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        input = Right;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        input = Down;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
        input = Exit;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        input = Start;
    }

    return input;
} // end getUserInput


/**
 * update the state of game objects
 * @param input - user keyboard input
 * @param started - tells program if the game is currently being played (ball's moving, game's running)
 * @param ball  - update ball position and speed
 * @param borders - used for passing into collision checking
 * @param rPaddle - update paddle position and speed
 * @param bricks - update hit registry (For each brick: is the brick hit or not?)
 * @param delta - current frame time
 * @param bricksHit - used for increasing ball's velocity when 4 and 12 bricks are broken respectively
 * @param score - passed to doCollisionChecks function for updating player's score
 * @param hitCount4 - determines if 4 bricks have been hit (ONLY FOR FIRST TIME 4 BRICKS ARE HIT)
 * @param hitCount12 - determines if 12 bricks have been hit (ONLY FOR FIRST TIME 12 BRICKS ARE HIT)
 * @return bool - returns status of game-running (true = game over, false = game's still running)
 */
bool update(Direction &input, bool& started, Ball &ball,
            Borders borders, MovingBlock& rPaddle,
            Brick bricks[BRICK_ROWS][BRICK_COLUMNS], float delta,
            int& bricksHit, int& score, bool& hitCount4, bool& hitCount12) {
    bool gameOver = false; // default; the game is running
    // adjust velocity directions for user input
    if (input) {
        switch (input) {
            case Left: // Left
                rPaddle.velocityX -= PADDLE_SPEED;
                break;
            case Right: // Right
                rPaddle.velocityX += PADDLE_SPEED;
                break;
            case Start: // Start game
                if(!started) {
                    ball.velocityX = BALL_SPEED_X;
                    ball.velocityY = BALL_SPEED_Y * -1; // start moving upwards

                    /* randomly set horizontal velocity to positive or negative
                     * by seeing if the tenths place of current delta
                     * (i.e. 1st digit after the decimal) is odd or even.  */
                    if ((int(delta * 10) & 1) % 2) {
                        ball.velocityX *= -1;  // ball goes left if odd
                    }
                    started = true;
                }
        }
        // clear input
        input = None;
    }
    // adjust paddle location
    rPaddle.block.left += rPaddle.velocityX * delta;

    if(started) {
        // adjust the location of the ball for speed * time
        ball.coordinateX += ball.velocityX * delta;
        ball.coordinateY += ball.velocityY * delta;
    }
    else {
        ball.coordinateX = rPaddle.block.left + (rPaddle.block.width / 2.0);
        ball.coordinateY = rPaddle.block.top - ball.radius - 1;
    }

    gameOver = doCollisionChecks(ball, rPaddle, bricks, borders, bricksHit, score);

    // --- EXTRA: increase ball velocity when bricksHit FIRST becomes 4 and 12 ---
    // First, for 4 bricks hit...
    if(bricksHit == 4 && !hitCount4) {
        ball.velocityX += 0.1;
        ball.velocityY += 0.1;
        hitCount4 = true;
    }
    // Then, for 12 bricks hit...
    if(bricksHit == 12 && !hitCount12) {
        ball.velocityX += 0.1;
        ball.velocityY += 0.1;
        hitCount12 = true;
    }

    return gameOver;
} // end update


/**
 * draw the ball, borders, paddle, and bricks on the graphics window
 * @param window - handle to open graphics window
 * @param ball   - structure variable with properties for the ball
 * @param borders - structure variable with properties for the border walls
 * @param paddle - structure variable with properties for the paddle
 * @param bricks - structure variable with properties for the bricks
 * @param delta  - amount of frame time plus lag (in ms)
 */
void render(sf::RenderWindow &window,
            Ball &ball, Borders borders, MovingBlock paddle,
            Brick bricks[BRICK_ROWS][BRICK_COLUMNS], float delta) {
    // Render drawing objects
    // ------------------------------------------------
    // clear the window with the background color
    window.clear(WINDOW_COLOR);

    // draw the walls
    window.draw(borders.leftWall.rectangle);
    window.draw(borders.topWall.rectangle);
    window.draw(borders.rightWall.rectangle);
    window.draw(borders.bottomWall.rectangle);

    // draw the ball
    // ------------------------------------------------
    sf::CircleShape circle;
    circle.setFillColor(ball.color);
    circle.setRadius(ball.radius);
    // set screen coordinates relative to the center of the circle
    circle.setOrigin( ball.radius, ball.radius);
    // calculate current drawing location relative to speed and frame-time
    float xPosition = ball.coordinateX + ball.velocityX * delta;
    float yPosition = ball.coordinateY + ball.velocityY * delta;
    circle.setPosition(xPosition, yPosition);
    window.draw(circle);

    // draw the paddle
    // ------------------------------------------------
    // calculate current drawing location relative to speed and frame-time
    float paddleX = paddle.block.left + paddle.velocityX * delta;
    float paddleY = paddle.block.top + paddle.velocityY * delta;
    paddle.block.rectangle.setPosition(paddleX, paddleY);
    window.draw(paddle.block.rectangle);

    // draw the bricks
    // ------------------------------------------------
    Brick *pBrick = &bricks[0][0];
    for (int row = 0; row < BRICK_ROWS; row++) {
        for (int column = 0; column < BRICK_COLUMNS; column++) {
            if (!pBrick->hit) {
                window.draw(pBrick->block.rectangle);
            }
            pBrick++;
        } // columns
    } // rows

    // show the new window
    // ------------------------------------------------
    window.display();
} // end render
