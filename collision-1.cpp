/* --------------------------------------------------------
 *    File: collision.cpp
 *  Author: Vasil Ivanov
 *   Class: COP 2001 CRN 10410
 * Purpose: detect and handle collisions of walls and ball
 * -------------------------------------------------------- */

#include <SFML/Graphics.hpp>
#include <cmath>

// Global Constants
// --------------------------------------------------------

// window properties
const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const sf::Color WINDOW_COLOR = sf::Color::White;

// drawing properties
const float FRAME_RATE = (1.0/30.0) * 1000.0;       // FPS in ms
const sf::Color BALL_COLOR = sf::Color::Blue;

// These are just for fun
// speed that the can accelerate at to span window in
// n-seconds (in ms) broken up for each frame
const float SPEED_TIME = (3.0 * 1000.0) * 30.0;     //
const float SPEED_X = WINDOW_WIDTH / SPEED_TIME;    // speed horizontally
const float SPEED_Y = WINDOW_HEIGHT / SPEED_TIME;   // span  vertically

// wall properties
const float WALL_THICKNESS = 10.0;
const sf::Color WALL_COLOR = sf::Color::Red;


// Type definitions
// --------------------------------------------------------
enum Direction {
    Exit = -1,
    None,
    Left,
    Up,
    Right,
    Down
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

// border properties
struct Borders {
    Block leftWall;
    Block topWall;
    Block rightWall;
    Block bottomWall;
};


// Function declarations (prototypes)
// --------------------------------------------------------
Direction processInput() ;
void update(Direction &input, Ball &ball, Borders borders, float delta);
void render(sf::RenderWindow &window, Ball &ball, Borders borders, float delta);
int getCollisionPoint(Ball* pBall, Block* pBlock);
bool checkCollision(Ball* pBall, Block* pBlock);



/**
 * The main application
 * @return OS status message (0=Success)
 */
int main() {

    // create a 2d graphics window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Hello SFML");
    window.clear(WINDOW_COLOR);

    // declare a borders variable and define the border walls
    Borders theWalls;
    // left wall
    theWalls.leftWall.left = 0.0;
    theWalls.leftWall.top = 0.0;
    theWalls.leftWall.width = WALL_THICKNESS;
    theWalls.leftWall.height = WINDOW_HEIGHT;
    theWalls.leftWall.color = WALL_COLOR;
    theWalls.leftWall.rectangle.setSize(sf::Vector2f(theWalls.leftWall.width,theWalls.leftWall.height));
    theWalls.leftWall.rectangle.setPosition(theWalls.leftWall.left, theWalls.leftWall.top);
    theWalls.leftWall.rectangle.setFillColor(WALL_COLOR);

    // top wall
    theWalls.topWall.left = 0.0;
    theWalls.topWall.top = 0.0;
    theWalls.topWall.width = WINDOW_WIDTH;
    theWalls.topWall.height = WALL_THICKNESS;
    theWalls.topWall.color = WALL_COLOR;
    theWalls.topWall.rectangle.setSize(sf::Vector2f(theWalls.topWall.width,theWalls.topWall.height));
    theWalls.topWall.rectangle.setPosition(theWalls.topWall.left, theWalls.topWall.top);
    theWalls.topWall.rectangle.setFillColor(WALL_COLOR);

    // right wall
    theWalls.rightWall.left = WINDOW_WIDTH - WALL_THICKNESS;
    theWalls.rightWall.top = 0.0;
    theWalls.rightWall.width = WALL_THICKNESS;
    theWalls.rightWall.height = WINDOW_HEIGHT;
    theWalls.rightWall.color = WALL_COLOR;
    theWalls.rightWall.rectangle.setSize(sf::Vector2f(theWalls.rightWall.width,theWalls.rightWall.height));
    theWalls.rightWall.rectangle.setPosition(theWalls.rightWall.left, theWalls.rightWall.top);
    theWalls.rightWall.rectangle.setFillColor(WALL_COLOR);

    // bottom wall
    theWalls.bottomWall.left = 0.0;
    theWalls.bottomWall.top = WINDOW_HEIGHT - WALL_THICKNESS;
    theWalls.bottomWall.width = WINDOW_WIDTH;
    theWalls.bottomWall.height = WALL_THICKNESS;
    theWalls.leftWall.color = WALL_COLOR;
    theWalls.bottomWall.rectangle.setSize(sf::Vector2f(theWalls.bottomWall.width,theWalls.bottomWall.height));
    theWalls.bottomWall.rectangle.setPosition(theWalls.bottomWall.left, theWalls.bottomWall.top);
    theWalls.bottomWall.rectangle.setFillColor(WALL_COLOR);


    // declare a ball variable and populate it in the center of the window
    Ball theBall;
    theBall.radius = 20.0;
    theBall.coordinateX = WINDOW_WIDTH / 2.0;
    theBall.coordinateY = WINDOW_HEIGHT / 2.0;
    theBall.velocityX = 0.0;
    theBall.velocityY = 0.0;
    theBall.color = BALL_COLOR;


    // timing variables for the main game loop
    sf::Clock clock;
    sf::Time startTime = clock.getElapsedTime();
    sf::Time stopTime = startTime;
    float delta = 0.0;

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

            update(userInput, theBall, theWalls, delta);

            // subtract the frame-rate from the current frame-time
            // for each full frame covered by this update
            while (delta >= FRAME_RATE)
                delta -= FRAME_RATE;
        }

        // Render the window
        // ------------------------------------------------
        render(window, theBall, theWalls, delta);

    } // end main game loop

    // game ending, close the graphics window
    window.close();

    return 0;   // return success to the OS
} // end main


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
bool checkCollision(Ball* pBall, Block* pBlock) { // ? Why do I need a pointer for the Block here?
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

    return input;
} // end getUserInput


/**
 * update the state of game objects
 * @param input - user keyboard input
 * @param ball  - update ball position and speed
 * @param delta - current frame time
 */
void update(Direction &input, Ball &ball, Borders borders, float delta) {

    // adjust velocity directions for user input
    if (input) {
        switch (input) {
            case Left: // Left
                ball.velocityX -= SPEED_X;
                break;
            case Up: // Up
                ball.velocityY -= SPEED_Y;
                break;
            case Right: // Right
                ball.velocityX += SPEED_X;
                break;
            case Down: // Down
                ball.velocityY += SPEED_Y;
        }
        // clear input
        input = None;
    }

    // adjust the location of the ball for speed * time
    ball.coordinateX += ball.velocityX * delta;
    ball.coordinateY += ball.velocityY * delta;

    // check for ball collisions with the play boundaries
    checkCollision(&ball, &borders.leftWall);
    checkCollision(&ball, &borders.topWall);
    checkCollision(&ball, &borders.rightWall);
    checkCollision(&ball, &borders.bottomWall);

} // end update


/**
 * draw the ball on the graphics window
 * @param window - handle to open graphics window
 * @param ball   - structure variable with properties for the ball
 * @param delta  - amount of frame time plus lag (in ms)
 */
void render(sf::RenderWindow &window, Ball &ball, Borders borders, float delta) {
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

    // show the new window
    // ------------------------------------------------
    window.display();
} // end render
