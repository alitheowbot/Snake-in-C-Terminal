#include <iostream>
#include <string>
#include <cstdlib>
#include <Windows.h>
#include <vector>
#include <conio.h>
#include <cmath>
#include <synchapi.h>

int main();

// final update as of 1/4/26:
// fixed all of the things in this todo list

    // TODO: 

    // fix the tail stacking on top of itself when a new one spawns // 1/4/26: i made a thing that tells u the tail x and y and i found that i think im spawning 

    // else if statement for gameover in Game() // 1/4/26: WHAT DOES THIS MEAN? 

// very happy with the game. funny looking spaghetti code, but runs really smoothly.


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

        nDir = STOP;
    }

    Snakes(int inX, int inY, eDirection inDir) {
        xVal = inX;
        yVal = inY;

        nDir = inDir;
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
    
    eDirection nDir;
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
bool doomsDay = false;
bool cancelDraw = false;
bool checked = false;
int counter = 0;

void NewAppleSpawn() {
    srand(time(NULL));

    fruitX = rand () % 28;
    fruitX += 1;

    fruitY = rand () % 28;
    fruitY += 1;

    // check if its the same as snake tails
    for (int i = 0; i <= snakeList.size() - 1; i++) {
        while (snakeList[i].GetY() == fruitX && snakeList[i].GetX() == fruitY) {
            fruitX = rand () % 28;
            fruitX += 1;

            fruitY = rand () % 28;
            fruitY += 1;
        }
    }
    
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
    // Scoreboard/Debugging Information
    /*if (!checked) {
        cout << "\nHead/Tail information:\n";
        cout << "Fruit Location: X: " << fruitX << " Y: " << fruitY << endl;
        cout << "----   X | Y   ---- \n";
        for (int i = 0; i <= snakeList.size() - 1; i++) {
            cout << "Tail " << i << " " << snakeList[i].GetX() << " | " << snakeList[i].GetY() << endl; 
        }
        checked = true;
    }*/
}

void CalculateNextXY() {
    for (int i = snakeList.size() - 1; i >= 0; i--) {
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
                cout << "HEY OVER HERE!";
                break;
        }
    }
}

void Game() {
    vector<Snakes> clonedList = snakeList;
    spawned = false;

    if (dir != STOP /*&& !spawned*/) {
        CalculateNextXY();
    }
    if (snakeList[0].GetY() == fruitX && snakeList[0].GetX() == fruitY) {
        doomsDay = true;
        snakeList = clonedList;
        //spawned = true;
    }

    if (!doomsDay) {
            // spawns a tail
        
        
        for (int i = snakeList.size() - 1; i >= 0; i--) {
            if (i == 0) {
                snakeList[0].SetDir(dir);
            }
            else {
                snakeList[i].SetDir(snakeList[i-1].GetDir());
            }
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
        else {
            checked = false;
            Draw();
        }
    }
    else {
        snakeList.insert(snakeList.begin(), Snakes(fruitY, fruitX, dir));
        NewAppleSpawn();
        checked = false;
        Draw();
        doomsDay = false;
    }

    
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
        //system("cls");

        Movement();
        Game();
        Sleep(10);
    }
}

int main() {
    Setup();
}
