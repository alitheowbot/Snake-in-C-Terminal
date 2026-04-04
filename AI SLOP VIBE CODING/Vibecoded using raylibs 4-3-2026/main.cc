#include <raylib.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// UPDATE
// 4/3/2026:
// After reviewing the code (and the lack of comments), I can confirm that AI will never be able to replicate this masterpiece.
// I have cleaned up the class code so functions are one line, and just overall code clean up

// final update as of 1/4/26:
// fixed all of the things in this todo list

// Graphics update: replaced terminal rendering with raylib window



enum eDirection {
    STOP = 0,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

eDirection dir;
eDirection tailDir;

bool gameOver = false;
bool spawned = false;
int width = 30;
int height = 30;
int fruitX, fruitY;

class Snakes {
public:
    Snakes(int inX, int inY) {
        xVal = inX;
        yVal = inY;
        nDir = STOP;
    }

    Snakes(int inX, int inY, eDirection inDir) {
        xVal = inX;
        yVal = inY;
        nDir = inDir;
    }

    ~Snakes() {}

    int GetX() {return xVal;};
    int GetY() {return yVal;};
    eDirection GetDir() {return nDir;};

    void SetX(int num) {xVal = num;};
    void SetY(int num) {yVal = num;};
    void SetDir(eDirection inDir) {nDir = inDir;};

private:
    int xVal;
    int yVal;
    eDirection nDir;
};

/////////////////////////////////////////////////////////////////////////////////

vector<Snakes> snakeList;
bool doomsDay = false;
int counter = 0;

const int CELL = 20;       // pixels per grid cell
const int OFFSET = 40;     // top/left border offset for score display

void NewAppleSpawn() {
    srand(time(NULL));

    fruitX = rand() % 28 + 1;
    fruitY = rand() % 28 + 1;

    for (int i = 0; i <= (int)snakeList.size() - 1; i++) {
        while (snakeList[i].GetY() == fruitX && snakeList[i].GetX() == fruitY) {
            fruitX = rand() % 28 + 1;
            fruitY = rand() % 28 + 1;
        }
    }
}

void Draw() {
    BeginDrawing();
    ClearBackground({20, 20, 20, 255});

    // Draw grid background
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            Rectangle cell = {
                (float)(OFFSET + col * CELL),
                (float)(OFFSET + row * CELL),
                (float)CELL,
                (float)CELL
            };
            DrawRectangleRec(cell, {30, 30, 30, 255});
            DrawRectangleLinesEx(cell, 1, {40, 40, 40, 255});
        }
    }

    // Draw fruit
    DrawRectangle(
        OFFSET + fruitY * CELL + 2,
        OFFSET + fruitX * CELL + 2,
        CELL - 4, CELL - 4,
        RED
    );

    // Draw snake body
    for (int i = 1; i < (int)snakeList.size(); i++) {
        DrawRectangle(
            OFFSET + snakeList[i].GetX() * CELL + 1,
            OFFSET + snakeList[i].GetY() * CELL + 1,
            CELL - 2, CELL - 2,
            {50, 200, 50, 255}
        );
    }

    // Draw snake head
    if (!snakeList.empty()) {
        DrawRectangle(
            OFFSET + snakeList[0].GetX() * CELL + 1,
            OFFSET + snakeList[0].GetY() * CELL + 1,
            CELL - 2, CELL - 2,
            {100, 255, 100, 255}
        );
    }

    // Score
    DrawText(TextFormat("Score: %d", (int)snakeList.size()), OFFSET, 10, 20, WHITE);

    EndDrawing();
}

void CalculateNextXY() {
    for (int i = snakeList.size() - 1; i >= 0; i--) {
        switch (snakeList[i].GetDir()) {
            case LEFT:  snakeList[i].SetX(snakeList[i].GetX() - 1); break;
            case RIGHT: snakeList[i].SetX(snakeList[i].GetX() + 1); break;
            case UP:    snakeList[i].SetY(snakeList[i].GetY() - 1); break;
            case DOWN:  snakeList[i].SetY(snakeList[i].GetY() + 1); break;
            case STOP:  break;
        }
    }
}

void Movement() {
    tailDir = dir;
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
        if (dir != RIGHT) dir = LEFT;
        else gameOver = true;
    }
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
        if (dir != LEFT) dir = RIGHT;
        else gameOver = true;
    }
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        if (dir != DOWN) dir = UP;
        else gameOver = true;
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        if (dir != UP) dir = DOWN;
        else gameOver = true;
    }
}

// Returns true if we should restart
bool Game() {
    vector<Snakes> clonedList = snakeList;
    spawned = false;

    if (dir != STOP) {
        CalculateNextXY();
    }

    if (snakeList[0].GetY() == fruitX && snakeList[0].GetX() == fruitY) {
        doomsDay = true;
        snakeList = clonedList;
    }

    if (!doomsDay) {
        for (int i = snakeList.size() - 1; i >= 0; i--) {
            if (i == 0) snakeList[0].SetDir(dir);
            else        snakeList[i].SetDir(snakeList[i-1].GetDir());
        }

        if (snakeList[0].GetX() < 0)       gameOver = true;
        if (snakeList[0].GetX() >= width)  gameOver = true;
        if (snakeList[0].GetY() >= height) gameOver = true;
        if (snakeList[0].GetY() < 0)       gameOver = true;

        for (int i = 1; i < (int)snakeList.size(); i++) {
            if (snakeList[i].GetX() == snakeList[0].GetX() &&
                snakeList[i].GetY() == snakeList[0].GetY()) {
                gameOver = true;
            }
        }

        if (gameOver) return true;
        Draw();
    } else {
        snakeList.insert(snakeList.begin(), Snakes(fruitY, fruitX, dir));
        NewAppleSpawn();
        Draw();
        doomsDay = false;
    }

    return false;
}

void ResetGame() {
    snakeList.clear();
    dir = STOP;
    tailDir = STOP;
    gameOver = false;
    doomsDay = false;
    counter = 0;
    snakeList.insert(snakeList.begin(), Snakes(width / 2, height / 2));
    NewAppleSpawn();
    if (fruitX == width / 2) fruitX += 1;
}

int main() {
    InitWindow(OFFSET * 2 + width * CELL, OFFSET * 2 + height * CELL, "Snake");
    SetTargetFPS(15);

    ResetGame();

    while (!WindowShouldClose()) {
        Movement();

        bool restart = Game();
        if (restart) {
            // Show game over screen until key press
            while (!WindowShouldClose()) {
                BeginDrawing();
                ClearBackground({20, 20, 20, 255});
                int score = (int)snakeList.size();
                DrawText("GAME OVER", OFFSET + width * CELL / 2 - 80, OFFSET + height * CELL / 2 - 30, 40, RED);
                DrawText(TextFormat("Score: %d", score), OFFSET + width * CELL / 2 - 50, OFFSET + height * CELL / 2 + 20, 24, WHITE);
                DrawText("Press ENTER to restart", OFFSET + width * CELL / 2 - 110, OFFSET + height * CELL / 2 + 60, 20, GRAY);
                EndDrawing();
                if (IsKeyPressed(KEY_ENTER)) break;
            }
            ResetGame();
        }
    }

    CloseWindow();
    return 0;
}
