#include "GameEngine.h"
#include "Resource.h"
#include "Sprite.h"
#include "BitMap.h"
#include <windows.h>

#define ROWS 5
#define COLS 9
#define BRICK_ROW_SPACING 10
#define BRICK_COL_SPACING 6.5

GameEngine* game;
Sprite* paddle;
Sprite* ball;
Sprite* bricks[ROWS][COLS];
int playerLives = 3;

HDC offScreen;
HBITMAP offScreenBitMap;

BOOL GameInitialize(HINSTANCE currInstance) {
    game = new GameEngine(currInstance, L"BrickBreaker", L"Brick Breaker", IDI_FACE, IDI_FACE_SM, 600, 400);
    if (game == NULL) {
        return FALSE;
    }
    game->setFrameRate(30);
    return TRUE;
}

void ResetGame() {
    playerLives = 3;
    paddle->setPosition(300 - (paddle->getWidth() / 2), 380);
    ball->setPosition(290, 370);
    ball->setVelocity(4, -4);
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            bricks[row][col]->setHidden(false); // Reset bricks
        }
    }
}

void GameStart(HWND hwnd) {
    srand(GetTickCount());
    offScreen = CreateCompatibleDC(GetDC(hwnd));
    offScreenBitMap = CreateCompatibleBitmap(GetDC(hwnd), game->getWidth(), game->getHeight());
    SelectObject(offScreen, offScreenBitMap);
    HDC hdc = GetDC(hwnd);

    // Load resources (check file paths)
    BitMap* paddleBitmap = new BitMap(hdc, L"Res/paddle1.bmp");
    BitMap* ballBitmap = new BitMap(hdc, L"Res/ball1.bmp");
    BitMap* brickBitmap = new BitMap(hdc, L"Res/brick1.bmp");

    // Initialize paddle
    RECT paddleBounds = { 0, 380, 600, 400 };
    paddle = new Sprite(paddleBitmap, paddleBounds, BA_STOP);
    game->addSprite(paddle);

    // Initialize ball
    RECT ballBounds = { 0, 0, 600, 400 };
    ball = new Sprite(ballBitmap, ballBounds, BA_BOUNCE);
    ball->setPosition(290, 300);
    ball->setVelocity(4, 4);
    game->addSprite(ball);

    // Initialize bricks
    int brickWidth = 60;
    int brickHeight = 30;

    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            int left = col * (brickWidth + BRICK_COL_SPACING);
            int right = left + brickWidth;
            int top = row * (brickHeight + BRICK_ROW_SPACING);
            int bottom = top + brickHeight;
            RECT brickBounds = { left-1, top-1, right+1, bottom+1 };
            bricks[row][col] = new Sprite(brickBitmap, brickBounds, BA_STOP);
            bricks[row][col]->setPosition(left, top);
            game->addSprite(bricks[row][col]);
        }
    }

    ResetGame(); // Reset player lives and ball/paddle position
}

void GamePaint(HDC hdc) {
    HBRUSH whiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
    RECT rect = { 0, 0, game->getWidth(), game->getHeight() };
    FillRect(hdc, &rect, whiteBrush);

    // Draw the sprites
    game->drawSprites(hdc);
}

void GameLoop() {
    game->updateSprites();
    HWND hwnd = game->getWnd();
    HDC hdc = GetDC(hwnd);
    GamePaint(offScreen);
    BitBlt(hdc, 0, 0, game->getWidth(), game->getHeight(), offScreen, 0, 0, SRCCOPY);

    // Check if the ball touches the bottom
    if (ball->getPosition().bottom >= game->getHeight()) {
        playerLives--; // Lose one life
        if (playerLives <= 0) {
            int response = MessageBox(hwnd, L"You have lost! Would you like to restart?", L"Game Over", MB_YESNO | MB_ICONQUESTION);
            if (response == IDYES) {
                ResetGame(); // Restart game
            }
            else {
                PostQuitMessage(0); // Exit game
            }
            return;
        }
        else {
            wchar_t message[100];
            swprintf(message, 100, L"You have %d lives remaining.", playerLives);
            MessageBox(hwnd, message, L"Life Lost", MB_OK | MB_ICONINFORMATION);

            ball->setPosition(290, 350); // Reset ball position
            ball->setVelocity(4, -4); // Reset ball velocity
        }
    }

    // Check if all bricks are hidden (win condition)
    bool allBricksHit = true;
    for (int row = 0; row < ROWS && allBricksHit; ++row) {
        for (int col = 0; col < COLS && allBricksHit; ++col) {
            if (!bricks[row][col]->isHidden()) {
                allBricksHit = false;
            }
        }
    }

    if (allBricksHit) {
        int response = MessageBox(hwnd, L"You have won the game! Would you like to restart?", L"Congratulations", MB_YESNO | MB_ICONINFORMATION);
        if (response == IDYES) {
            ResetGame(); // Restart game
        }
        else {
            PostQuitMessage(0); // Exit game
        }
        return; // Exit loop to prevent further checks after the game is won
    }

    ReleaseDC(hwnd, hdc);
}

bool SpriteCollision(Sprite* hitter, Sprite* hittee) {
    RECT hitterBounds = hitter->getCollision();
    RECT hitteeBounds = hittee->getCollision();

    // Check for collision between ball and paddle
    if (hitter == ball && hittee == paddle) {
        POINT vel = hitter->getVelocity();
        vel.y = -vel.y; // Reverse vertical velocity
        hitter->setVelocity(vel);
        return true;
    }

    // Check for collision between ball and bricks
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (hitter == ball && hittee == bricks[row][col] && !bricks[row][col]->isHidden()) {
                POINT vel = hitter->getVelocity();
                vel.y = -vel.y; // Reverse vertical velocity
                hitter->setVelocity(vel);
                bricks[row][col]->setHidden(true); // Hide brick after being hit
                return true;
            }
        }
    }

    // Check for collision between ball and screen boundaries
    if (hitter == ball) {
        POINT vel = hitter->getVelocity();
        if (hitterBounds.right >= game->getWidth() || hitterBounds.left <= 0) {
            // Collided with left or right boundary, reverse horizontal velocity
            vel.x = -vel.x;
            hitter->setVelocity(vel);
            return true;
        }
        if (hitterBounds.bottom >= game->getHeight() || hitterBounds.top <= 0) {
            // Collided with top or bottom boundary, reverse vertical velocity
            vel.y = -vel.y;
            hitter->setVelocity(vel);
            return true;
        }
    }

    return false;
}

void GameEnd() {
    DeleteObject(offScreenBitMap);
    DeleteDC(offScreen);
    delete paddle;
    delete ball;
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            delete bricks[row][col];
        }
    }
    delete game;
}

void MouseButtonDown(int x, int y, bool left) {}
void MouseButtonUp(int x, int y, bool left) {}
void HandleKeys() {}

void MouseMove(int x, int y) {
    // Move paddle with the mouse, ensuring it doesn't go outside the game bounds
    int newX = x - (paddle->getWidth() / 2);
    newX = max(0, min(game->getWidth() - paddle->getWidth(), newX));
    paddle->setPosition(newX, paddle->getPosition().top);
    InvalidateRect(game->getWnd(), NULL, FALSE);
}

void GameActivate(HWND hwnd) {}
void GameDeactivate(HWND hwnd) {}
