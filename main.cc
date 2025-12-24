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

// else if statement for gameover in Game()

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

class Snakes {
public:
    Snakes(int inX, int inY) {
        xVal = inX;
        yVal = inY;
    }

    ~Snakes() {
        
    }

    int GetX();
    int GetY();
    eDirection GetDir();

    void SetX(int num);
    void SetY(int num);
    void SetDir(eDirection inDir);

private:
    int xVal;
    int yVal;
    
    eDirection nDir = STOP;
};

int Snakes::GetX() {
    return xVal;
}

int Snakes::GetY() {
    return yVal;
}

eDirection Snakes::GetDir() {
    return nDir;
}

void Snakes::SetX(int num) {
    xVal = num;
}

void Snakes::SetY(int num) {
    yVal = num;
}

void Snakes::SetDir(eDirection inDir) {
    nDir = inDir;
}

/////////////////////////////////////////////////////////////////////////////////

vector<Snakes> snakeList;
bool cancelDraw = false;
bool checked = false;
int counter = 0;

void NewAppleSpawn() {
    srand(time(NULL));

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
                if (k.GetX() == snakeList[0].GetX() && k.GetY() == snakeList[0].GetY()) {
                    if (i == snakeList[0].GetY() && j == snakeList[0].GetX()) {
                        cout << "O";
                        printed = true;
                    }
                }
                
                else if (i == k.GetY() && j == k.GetX()) {
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
        switch (snakeList[i].GetDir()) {
            case LEFT:
                snakeList[i].SetX(snakeList[i].GetX() - 1);
                break;
            case RIGHT:
                snakeList[i].SetX(snakeList[i].GetX() + 1);
                break;
            case UP:
                snakeList[i].SetY(snakeList[i].GetY() - 1);
                break;
            case DOWN:
                snakeList[i].SetY(snakeList[i].GetY() + 1);
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
    if (snakeList[0].GetY() == fruitX && snakeList[0].GetX() == fruitY) {
        snakeList.insert(snakeList.begin(), Snakes(fruitY, fruitX));
        NewAppleSpawn();
        //spawned = true;
    }
    
    for (int i = snakeList.size() - 1; i >= 0; i--) {
        if (i == 0) {
            snakeList[0].SetDir(dir);
        }
        else {
            snakeList[i].SetDir(snakeList[i-1].GetDir());
        }
    }

    if (dir != STOP && !spawned) {
        CalculateNextXY();
    }
    // cout << snakeList[0].GetX();
    // cout << snakeList.size();

    // game over logic
    if (snakeList[0].GetX() == 0) { // left
        gameOver = true;
    }
    if (snakeList[0].GetX() == width) { // right
        gameOver = true;
    }
    if (snakeList[0].GetY() == height) { // up
        gameOver = true;
    }
    if (snakeList[0].GetY() == 0) { // down
        gameOver = true;
    }

    for (int i = 1; i < snakeList.size(); i++) {
        if (snakeList[i].GetX() == snakeList[0].GetX() && snakeList[i].GetY() == snakeList[0].GetY()) {
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
    Snakes snakeHead((width/2), (height/2));

    snakeList.insert(snakeList.begin(), snakeHead);
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
    Setup();
}