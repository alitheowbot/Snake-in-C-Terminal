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
// add a clear screen method

// rework the way movement is calculated and maybe even how tails are spawned

// After you get this working, you can work on spawning in the tails for each apple aten (maybe just a set X and Y value for now) 
// After you get this working, you can work on having the tails follow the node in front of it

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

    void SetX(int num);
    void SetY(int num);

private:
    int xVal;
    int yVal;
};

int Snakes::GetX() {
    return xVal;
}

int Snakes::GetY() {
    return yVal;
}

void Snakes::SetX(int num) {
    xVal = num;
}

void Snakes::SetY(int num) {
    yVal = num;
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

    for (Snakes ma: snakeList) {
        cout << ma.GetX() << " " << ma.GetY() << endl;
    }
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

void CalculateNextXY(vector<Snakes> inList, eDirection inDir) {
    // change x/y values based on our direction

    if (inList[0].GetX() != snakeList[0].GetX() && inList[0].GetY() != snakeList[0].GetY()) inDir = tailDir;
    
    int tmpX = inList[0].GetX();
    int tmpY = inList[0].GetY();
    // prob can delete this ^

    int pos = 0;
    pos = (counter - snakeList.size());
    pos = abs(pos);

    int newX = tmpX;
    int newY = tmpY;

    if (inDir == LEFT) {
        newX -= 1;
    }
    if (inDir == RIGHT) {
        newX += 1;
    }
    if (inDir == UP) {
        newY -= 1;
    }
    if (inDir == DOWN) {
        newY += 1;
    }

    if (newY == fruitX && newX == fruitY) {
        snakeList.insert(snakeList.begin(), Snakes(fruitY, fruitX));
        NewAppleSpawn();
        spawned = true;
    }

    if (!spawned) {
        snakeList[pos].SetX(newX);
        snakeList[pos].SetY(newY);
    }
    else {
        snakeList[pos].SetX(tmpX);
        snakeList[pos].SetY(tmpX);
    }
    

    if (inList.size() > 1) {
        vector<Snakes> tmpList = inList;
        tmpList.erase(tmpList.begin());
        counter -= 1;
        CalculateNextXY(tmpList, inDir);
    }
    
    
}

void Game() {
    counter = snakeList.size();
    spawned = false;

    // spawns a tail
    /*if (snakeList[0].GetY() == fruitX && snakeList[0].GetX() == fruitY) {
        snakeList.insert(snakeList.begin(), Snakes(fruitY, fruitX));
        NewAppleSpawn();
        spawned = true;
    }*/
    

    if (dir != STOP && !spawned) {
        CalculateNextXY(snakeList, dir);
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
        Sleep(20);
        Movement();
        Game();
    }
}

int main() {
    Setup();
    
}