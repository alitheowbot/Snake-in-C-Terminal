#include <iostream>
#include <string>
#include <cstdlib>
#include <Windows.h>

using namespace std;

bool gameOver = false;
int width = 30;
int height = 30;
int uniNum = 1;

class Snakes {
public:
    Snakes() {
        xVal = width / 2;
        yVal = height / 2;

        number = uniNum;
        nextNodeRef = nullptr;
    }

    Snakes(int inX, int inY) {
        xVal = inX;
        yVal = inY;

        number = uniNum;
        nextNodeRef = nullptr;
    }

private:
    int xVal;
    int yVal;

    int number;
    Snakes* nextNodeRef;
};

bool canDraw() {
    uniNum += 1;
    if (uniNum == 2) {
        return true;
    }
    else {
        return false;
    }
}

void Draw() {
    // top line of board
    cout << " ";
    for (int i = 0; i < width-1; i++) {
        cout << "#";
    }

    cout << endl;

    // body/game part of board
    for (int i = 0; i < width; i++) {
        cout << "#";
        for (int j = 0; j < height; j++) {
            if (j != 0) {
                cout << " ";
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

void Game() {
    /*
        Handle spawning Apple, Head, Tails
            - Apples are randomly spawned using srand!
        Checking game over or not? 
        Allowing the game to draw or not (Dont draw if we just ate an apple, because we are gonna spawn the new head node there!)

    */
    
    Snakes* headNode;
    Snakes* tailNode;

    headNode = new Snakes();
    
    if (canDraw()) {
        Draw();
    }
}

int main() {
    while (!gameOver) {
        Game();
    }
}