#include <iostream>
#include <string>
#include <cstdlib>
#include <Windows.h>
#include <vector>
#include <conio.h>
#include <cmath>
#include <synchapi.h>

int main();

// TODO: 

// fix the tail stacking on top of itself when a new one spawns

// remove classes and just make it a struct

enum eDirection {
    STOP = 0,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

eDirection dir;
eDirection tailDir;


using namespace std;

bool gameOver = false;
bool spawned = false;
int width = 30;
int height = 30;
int fruitX, fruitY;

struct Snakes {
    int x;
    int y;
    eDirection inDir = STOP;
};

/////////////////////////////////////////////////////////////////////////////////

vector<Snakes> snakeList;
bool cancelDraw = false;
bool checked = false;
int counter = 0;

void NewAppleSpawn() {
    fruitX = rand () % 28;
    fruitX += 1;

    fruitY = rand () % 28;
    fruitY += 1;
}

void Draw() {
    // top line of board
    cout << " ";
    for (int i = 0; i < width-1; i++) {
        cout << "#";
    }

    cout << endl;

    /*for (Snakes ma: snakeList) {
        cout << ma.GetX() << " " << ma.GetY() << endl;
    }*/

    // body/game part of board
    for (int i = 0; i < width; i++) {
        checked = false;
        
        cout << "#";
        for (int j = 0; j < height; j++) {
            bool printed = false;
            for (Snakes k: snakeList) {
                if (k.x == snakeList[0].x && k.y == snakeList[0].y) {
                    if (i == snakeList[0].y && j == snakeList[0].x) {
                        cout << "O";
                        printed = true;
                    }
                }
                
                else if (i == k.y && j == k.x) {
                    cout << "o";
                    
                    printed = true;
                }
            }
            if (i == fruitX && j == fruitY) {
                    cout << "A";
                }
            else if (j != 0) {
                if (!printed) cout << " ";
                
            }
            
        }
        cout << "#";
        cout << endl;
    }
        


    // bottom line of board
    cout << " ";
    for (int i = 0; i < width - 1; i++) {
        cout << "#";
    }
}

void CalculateNextXY() {
    for (int i = 0; i < snakeList.size(); i++) {
        switch (snakeList[i].inDir) {
            case LEFT:
                snakeList[i].x = (snakeList[i].x - 1);
                break;
            case RIGHT:
                snakeList[i].x = (snakeList[i].x + 1);
                break;
            case UP:
                snakeList[i].y = (snakeList[i].y - 1);
                break;
            case DOWN:
                snakeList[i].y = (snakeList[i].y + 1);
                break;
            case STOP:
                break;
        }
    }
}

void Game() {
    counter = snakeList.size();
    spawned = false;

    // spawns a tail
    if (snakeList[0].y == fruitX && snakeList[0].x == fruitY) {
        snakeList.insert(snakeList.begin(), {fruitY, fruitX});
        NewAppleSpawn();
        //spawned = true;
    }
    
    for (int i = snakeList.size() - 1; i >= 0; i--) {
        if (i == 0) {
            snakeList[0].inDir = (dir);
        }
        else {
            snakeList[i].inDir = (snakeList[i-1].inDir);
        }
    }

    if (dir != STOP && !spawned) {
        CalculateNextXY();
    }
    // cout << snakeList[0].GetX();
    // cout << snakeList.size();

    // game over logic
    if (snakeList[0].x == 0) { // left
        gameOver = true;
    }
    if (snakeList[0].x == width) { // right
        gameOver = true;
    }
    if (snakeList[0].y == height) { // up
        gameOver = true;
    }
    if (snakeList[0].y == 0) { // down
        gameOver = true;
    }

    for (int i = 1; i < snakeList.size(); i++) {
        if (snakeList[i].x == snakeList[0].x && snakeList[i].y == snakeList[0].y) {
            gameOver = true;
        }
    }
    
    

    if (gameOver) main();
    else Draw();
}

void Movement() {
    tailDir = dir;
    if (_kbhit()) {
        switch (_getch()) {
            case 'a':
                if (dir == RIGHT) {
                    gameOver = true;
                }
                else {
                    dir = LEFT;
                    break;
                }
            case 'd':
                if (dir == LEFT) {
                    gameOver = true;
                }
                else {
                    dir = RIGHT;
                    break;
                }
            case 'w':
                if (dir == DOWN) {
                    gameOver = true;
                }
                else {
                    dir = UP;
                    break;
                }
            case 's': 
                if (dir == UP) {
                    gameOver = true;
                }
                else {
                    dir = DOWN;
                    break;
                }
        }
    }
}

void Setup() {
    snakeList.push_back({width/2, height/2});

    //snakeList.insert(snakeList.end(), Snakes(14, 15)); // test tail
    //snakeList.insert(snakeList.end(), Snakes(13, 15)); // test tail
    NewAppleSpawn();
    if (fruitX == width / 2) {
        fruitX += 1;
    }

    while (!gameOver) {
        system("cls");

        Movement();
        Game();
        Sleep(10);
    }
}

int main() {
    srand(time(NULL));
    Setup();
}