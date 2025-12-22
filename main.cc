#include <iostream>
#include <string>
#include <cstdlib>
#include <Windows.h>
#include <vector>

// TODO: 
// Make the head be able to move!

// After you get this working, you can work on spawning in the tails for each apple aten (maybe just a set X and Y value for now) 
// After you get this working, you can work on having the tails follow the node in front of it


using namespace std;

bool gameOver = false;
int width = 30;
int height = 30;
int uniNum = 0;
int fruitX, fruitY;

class Snakes {
public:
    Snakes() {
        xVal = 0;
        yVal = 0;

        number = uniNum;
        nextNodeRef = nullptr;
    }

    Snakes(int inX, int inY) {
        xVal = inX;
        yVal = inY;

        uniNum += 1;
        number = uniNum;
        nextNodeRef = nullptr;
    }

    ~Snakes() {
        cout << "Deconstructed";
    }

    int GetX();
    int GetY();

private:
    int xVal;
    int yVal;

    int number;
    Snakes* nextNodeRef;
};

int Snakes::GetX() {
    return xVal;
}

int Snakes::GetY() {
    return yVal;
}


/////////////////////////////////////////////////////////////////////////////////

vector<int> snakeX;
vector<int> snakeY;
bool cancelDraw = false;
bool checked = false;

void NewAppleSpawn() {
    srand(time(NULL));

    fruitX = rand () % 28;
    fruitX += 1;

    fruitY = rand () % 28;
    fruitY += 1;
}

/*bool canDraw() {
    uniNum += 1;
    if (uniNum == 2) {
        return true;
    }
    else {
        return false;
    }
}*/

void Draw() {
    // top line of board
    cout << " ";
    for (int i = 0; i < width-1; i++) {
        cout << "#";
    }

    cout << endl;

    // body/game part of board
    for (int i = 0; i < width; i++) {
        checked = false;
        cout << "#";
        for (int j = 0; j < height; j++) {
            for (int node = 0; node < uniNum; node++) {
                if (i == fruitX && j == fruitY) {
                    cout << "A";
                }
                else if (i == snakeX.at(node) && j == snakeY.at(node)) {
                    cout << "O";
                }
                else if (j != 0) {
                    cout << " ";
                }
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

void Game(Snakes *inHead, Snakes *inTails) {
    if ((inHead->GetX() == fruitX && inHead->GetY() == fruitY)) {
        snakeX.push_back(fruitX);
        snakeY.push_back(fruitY);

        NewAppleSpawn();
    }
    // TODO: else if head x and y == outside of bounds (0 or width/height) GAMEOVER
    Draw();
}

void Setup() {
    Snakes *headNode;
    Snakes *tailNode;

    headNode = new Snakes();
    tailNode = new Snakes(width / 2, height / 2);
    snakeX.push_back(tailNode->GetX());
    snakeY.push_back(tailNode->GetY());
    NewAppleSpawn();
    if (fruitX == width / 2) {
        fruitX += 1;
    }

    if (!gameOver) {
        Game(headNode, tailNode);
    }
}

int main() {
    Setup();
    
}