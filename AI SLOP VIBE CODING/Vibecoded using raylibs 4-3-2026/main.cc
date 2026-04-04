#include <raylib.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <string>

using namespace std;

// UPDATE
// 4/3/2026:
// After reviewing the code (and the lack of comments), I can confirm that AI will never be able to replicate this masterpiece.
// Graphics update: raylib with powerups, particles, eyes, glow, high score persistence

// ---- Constants ----
const int CELL     = 20;
const int OFFSET   = 40;
const int BOTTOM   = 50;
const int GRID_W   = 30;
const int GRID_H   = 30;
const int WIN_W    = OFFSET * 2 + GRID_W * CELL;
const int WIN_H    = OFFSET + GRID_H * CELL + BOTTOM;

// ---- Enums ----
enum eDirection  { STOP = 0, UP, DOWN, LEFT, RIGHT };
enum PowerupType { PU_NONE = 0, PU_SPEED, PU_SLOW, PU_SHRINK, PU_MULTIPLIER, PU_GHOST };

// ---- Direction state ----
eDirection dir     = STOP;
eDirection tailDir = STOP;
eDirection nextDir = STOP;

// ---- Game state ----
bool  gameOver       = false;
bool  doomsDay       = false;
int   fruitX, fruitY;
int   score          = 0;
int   highScore      = 0;
int   scoreMultiplier = 1;
bool  ghostMode      = false;
bool  ghostBorrowed  = false;
float gameSpeed      = 0.08f;
string flashMessage  = "";
float  flashEnd      = 0.0f;

// ---- Snake class ----
class Snakes {
public:
    Snakes(int inX, int inY)                    : xVal(inX), yVal(inY), nDir(STOP)  {}
    Snakes(int inX, int inY, eDirection inDir)  : xVal(inX), yVal(inY), nDir(inDir) {}
    ~Snakes() {}

    int        GetX()   { return xVal; }
    int        GetY()   { return yVal; }
    eDirection GetDir() { return nDir; }
    void SetX(int n)            { xVal = n; }
    void SetY(int n)            { yVal = n; }
    void SetDir(eDirection d)   { nDir = d; }

private:
    int xVal, yVal;
    eDirection nDir;
};

vector<Snakes> snakeList;

// ---- Particles ----
struct Particle {
    float x, y, vx, vy, life, maxLife;
    Color color;
};
vector<Particle> particles;

// ---- Powerups ----
struct Powerup {
    PowerupType type;
    int   x, y;
    float expireTime;
    bool  active;
};

Powerup     boardPowerup      = { PU_NONE, 0, 0, 0.0f, false };
PowerupType activePowerupType = PU_NONE;
float       activePowerupEnd  = 0.0f;
float       nextPowerupSpawn  = 5.0f;

/////////////////////////////////////////////////////////////////////////////////

// Grid cell to screen pixel
int gx(int col) { return OFFSET + col * CELL; }
int gy(int row) { return OFFSET + row * CELL; }

// ---- High score ----
void SaveHighScore(int s) {
    ofstream f("highscore.txt");
    if (f.is_open()) f << s;
}

int LoadHighScore() {
    ifstream f("highscore.txt");
    int s = 0;
    if (f.is_open()) f >> s;
    return s;
}

// ---- Particles ----
void SpawnParticles(int gridX, int gridY, Color col) {
    float cx = gx(gridX) + CELL * 0.5f;
    float cy = gy(gridY) + CELL * 0.5f;
    for (int i = 0; i < 18; i++) {
        float angle = (rand() % 360) * DEG2RAD;
        float spd   = 1.5f + (rand() % 300) / 100.0f;
        float life  = 0.4f + (rand() % 40) / 100.0f;
        Particle p;
        p.x = cx;  p.y = cy;
        p.vx = cosf(angle) * spd;
        p.vy = sinf(angle) * spd;
        p.life = life;  p.maxLife = life;
        p.color = col;
        particles.push_back(p);
    }
}

void UpdateParticles(float dt) {
    for (auto& p : particles) {
        p.x  += p.vx * dt * 60.0f;
        p.y  += p.vy * dt * 60.0f;
        p.vx *= 0.93f;
        p.vy *= 0.93f;
        p.life -= dt;
    }
    particles.erase(
        remove_if(particles.begin(), particles.end(), [](const Particle& p){ return p.life <= 0; }),
        particles.end()
    );
}

void DrawParticles() {
    for (auto& p : particles) {
        float alpha = p.life / p.maxLife;
        Color c = { p.color.r, p.color.g, p.color.b, (unsigned char)(255 * alpha) };
        DrawCircle((int)p.x, (int)p.y, 3.5f * alpha, c);
    }
}

// ---- Powerup helpers ----
Color PowerupColor(PowerupType t) {
    switch (t) {
        case PU_SPEED:      return { 255, 80,  80,  255 };
        case PU_SLOW:       return { 80,  150, 255, 255 };
        case PU_SHRINK:     return { 255, 220, 50,  255 };
        case PU_MULTIPLIER: return { 210, 60,  255, 255 };
        case PU_GHOST:      return { 160, 240, 255, 255 };
        default:            return WHITE;
    }
}

const char* PowerupLabel(PowerupType t) {
    switch (t) {
        case PU_SPEED:      return "SPEED!";
        case PU_SLOW:       return "SLOW";
        case PU_SHRINK:     return "SHRINK";
        case PU_MULTIPLIER: return "2X SCORE!";
        case PU_GHOST:      return "GHOST!";
        default:            return "";
    }
}

void SpawnPowerup() {
    boardPowerup.type = (PowerupType)(1 + rand() % 5);
    boardPowerup.x    = 1 + rand() % 28;
    boardPowerup.y    = 1 + rand() % 28;

    bool overlap = true;
    while (overlap) {
        overlap = false;
        if (boardPowerup.x == fruitY && boardPowerup.y == fruitX) { overlap = true; }
        for (auto& s : snakeList) {
            if (s.GetX() == boardPowerup.x && s.GetY() == boardPowerup.y) { overlap = true; break; }
        }
        if (overlap) {
            boardPowerup.x = 1 + rand() % 28;
            boardPowerup.y = 1 + rand() % 28;
        }
    }

    boardPowerup.expireTime = GetTime() + 7.0f;
    boardPowerup.active     = true;
}

void ApplyPowerup(PowerupType t) {
    // Cancel previous if active
    if (activePowerupType == PU_SPEED || activePowerupType == PU_SLOW) gameSpeed = 0.08f;
    if (activePowerupType == PU_MULTIPLIER) scoreMultiplier = 1;
    if (activePowerupType == PU_GHOST)      ghostMode = false;

    activePowerupType = t;
    activePowerupEnd  = GetTime() + 5.0f;

    switch (t) {
        case PU_SPEED:      gameSpeed = 0.055f; break;
        case PU_SLOW:       gameSpeed = 0.22f;  break;
        case PU_MULTIPLIER: scoreMultiplier = 2; break;
        case PU_GHOST:      ghostMode = true;    break;
        case PU_SHRINK:
            if ((int)snakeList.size() > 3) snakeList.erase(snakeList.begin() + snakeList.size() / 2, snakeList.end());
            flashMessage = "SHRINK!";
            flashEnd = GetTime() + 1.0f;
            activePowerupType = PU_NONE; // instant, no timer
            break;
        default: break;
    }
}

void ExpirePowerup() {
    switch (activePowerupType) {
        case PU_SPEED:
        case PU_SLOW:       gameSpeed = 0.08f;   break;
        case PU_MULTIPLIER: scoreMultiplier = 1;  break;
        case PU_GHOST:      ghostMode = false; ghostBorrowed = false; break;
        default: break;
    }
    activePowerupType = PU_NONE;
}

// ---- Apple spawn ----
void NewAppleSpawn() {
    fruitX = 1 + rand() % 28;
    fruitY = 1 + rand() % 28;
    for (int i = 0; i < (int)snakeList.size(); i++) {
        while (snakeList[i].GetY() == fruitX && snakeList[i].GetX() == fruitY) {
            fruitX = 1 + rand() % 28;
            fruitY = 1 + rand() % 28;
        }
    }
}

// ---- Draw ----
void Draw() {
    float t = GetTime();
    BeginDrawing();
    ClearBackground({ 12, 12, 12, 255 });

    // Grid cells
    for (int row = 0; row < GRID_H; row++) {
        for (int col = 0; col < GRID_W; col++) {
            DrawRectangle(gx(col), gy(row), CELL, CELL, { 22, 22, 22, 255 });
            DrawRectangleLines(gx(col), gy(row), CELL, CELL, { 32, 32, 32, 255 });
        }
    }

    // Ghost mode tint
    if (ghostMode) {
        DrawRectangle(OFFSET, OFFSET, GRID_W * CELL, GRID_H * CELL, { 100, 200, 255, 18 });
    }

    // Glowing apple
    float pulse = 0.5f + 0.5f * sinf(t * 5.0f);
    DrawCircle(gx(fruitY) + CELL/2, gy(fruitX) + CELL/2, CELL/2 + 7 + (int)(3*pulse), { 255, 50, 50, 35 });
    DrawCircle(gx(fruitY) + CELL/2, gy(fruitX) + CELL/2, CELL/2 + 3 + (int)(2*pulse), { 255, 80, 80, 55 });
    DrawRectangleRounded({ (float)gx(fruitY)+2, (float)gy(fruitX)+2, CELL-4, CELL-4 }, 0.5f, 8, RED);

    // Board powerup
    if (boardPowerup.active) {
        float remaining = boardPowerup.expireTime - t;
        bool visible = remaining > 2.0f || (int)(t * 8) % 2 == 0; // blink last 2 sec
        if (visible) {
            Color pc   = PowerupColor(boardPowerup.type);
            float pglow = 4.0f + 3.0f * sinf(t * 4.0f);
            DrawCircle(gx(boardPowerup.x)+CELL/2, gy(boardPowerup.y)+CELL/2, CELL/2 + pglow, { pc.r, pc.g, pc.b, 50 });
            DrawRectangleRounded({ (float)gx(boardPowerup.x)+1, (float)gy(boardPowerup.y)+1, CELL-2, CELL-2 }, 0.45f, 8, pc);
            int labelW = MeasureText(PowerupLabel(boardPowerup.type), 10);
            DrawText(PowerupLabel(boardPowerup.type), gx(boardPowerup.x) + CELL/2 - labelW/2, gy(boardPowerup.y) - 14, 10, pc);
        }
    }

    // Snake body
    for (int i = 1; i < (int)snakeList.size(); i++) {
        Color bodyCol = ghostMode ? Color{ 100, 200, 255, 170 } : Color{ 45, 175, 45, 255 };
        DrawRectangleRounded({ (float)gx(snakeList[i].GetX())+1, (float)gy(snakeList[i].GetY())+1, CELL-2, CELL-2 }, 0.4f, 8, bodyCol);
    }

    // Snake head
    if (!snakeList.empty()) {
        Color headCol = ghostMode ? Color{ 150, 230, 255, 220 } : Color{ 80, 255, 80, 255 };
        DrawRectangleRounded({ (float)gx(snakeList[0].GetX())+1, (float)gy(snakeList[0].GetY())+1, CELL-2, CELL-2 }, 0.5f, 8, headCol);

        // Eyes
        int hx = gx(snakeList[0].GetX());
        int hy = gy(snakeList[0].GetY());
        int e1x = hx+5, e1y = hy+5, e2x = hx+13, e2y = hy+5;
        switch (dir) {
            case RIGHT: e1x=hx+13; e1y=hy+4;  e2x=hx+13; e2y=hy+12; break;
            case LEFT:  e1x=hx+4;  e1y=hy+4;  e2x=hx+4;  e2y=hy+12; break;
            case UP:    e1x=hx+4;  e1y=hy+4;  e2x=hx+13; e2y=hy+4;  break;
            case DOWN:  e1x=hx+4;  e1y=hy+14; e2x=hx+13; e2y=hy+14; break;
            default: break;
        }
        DrawCircle(e1x, e1y, 2.5f, BLACK);
        DrawCircle(e2x, e2y, 2.5f, BLACK);
        DrawCircle(e1x, e1y, 1.2f, WHITE);
        DrawCircle(e2x, e2y, 1.2f, WHITE);
    }

    // Particles
    DrawParticles();

    // Flash message (shrink etc.)
    if (!flashMessage.empty() && t < flashEnd) {
        float alpha = 1.0f - (t - (flashEnd - 1.0f));
        if (alpha > 1.0f) alpha = 1.0f;
        int fw = MeasureText(flashMessage.c_str(), 36);
        DrawText(flashMessage.c_str(), WIN_W/2 - fw/2, WIN_H/2 - 80, 36, { 255, 220, 50, (unsigned char)(255 * alpha) });
    }

    // UI — score top left, high score top right
    DrawText(TextFormat("Score: %d", score),     OFFSET, 10, 20, WHITE);
    int hsW = MeasureText(TextFormat("Best: %d", highScore), 20);
    DrawText(TextFormat("Best: %d", highScore),  WIN_W - OFFSET - hsW, 10, 20, GOLD);

    // Active powerup timer bar
    if (activePowerupType != PU_NONE) {
        float pct     = (activePowerupEnd - t) / 5.0f;
        if (pct < 0) pct = 0;
        Color pc      = PowerupColor(activePowerupType);
        int barX = OFFSET, barY = OFFSET + GRID_H * CELL + 14, barW = GRID_W * CELL, barH = 10;

        DrawRectangle(barX, barY, barW, barH, { 35, 35, 35, 255 });
        DrawRectangle(barX, barY, (int)(barW * pct), barH, pc);
        DrawRectangleLines(barX, barY, barW, barH, { 60, 60, 60, 255 });

        int lw = MeasureText(PowerupLabel(activePowerupType), 14);
        DrawText(PowerupLabel(activePowerupType), barX + barW/2 - lw/2, barY - 18, 14, pc);
    }

    EndDrawing();
}

// ---- Move snake segments ----
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

// ---- Input ----
void Movement() {
    tailDir = dir;
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))  { if (dir != RIGHT) nextDir = LEFT;  }
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) { if (dir != LEFT)  nextDir = RIGHT; }
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))    { if (dir != DOWN)  nextDir = UP;    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))  { if (dir != UP)    nextDir = DOWN;  }
}

// ---- Game logic ----
bool Game() {
    float t = GetTime();

    if (nextDir != STOP) { dir = nextDir; nextDir = STOP; }

    vector<Snakes> clonedList = snakeList;

    if (dir != STOP) CalculateNextXY();

    if (snakeList[0].GetY() == fruitX && snakeList[0].GetX() == fruitY) {
        doomsDay = true;
        snakeList = clonedList;
    }

    if (!doomsDay) {
        for (int i = snakeList.size() - 1; i >= 0; i--) {
            if (i == 0) snakeList[0].SetDir(dir);
            else        snakeList[i].SetDir(snakeList[i-1].GetDir());
        }

        // Wall collision — ghost wraps
        if (!ghostMode) {
            if (snakeList[0].GetX() < 0 || snakeList[0].GetX() >= GRID_W) return true;
            if (snakeList[0].GetY() < 0 || snakeList[0].GetY() >= GRID_H) return true;
        } else if (ghostBorrowed &&
                   (snakeList[0].GetX() < 0 || snakeList[0].GetX() >= GRID_W ||
                    snakeList[0].GetY() < 0 || snakeList[0].GetY() >= GRID_H)) {
            return true;
        } else {
            for (auto& s : snakeList) {
                if (s.GetX() < 0)       s.SetX(GRID_W - 1);
                if (s.GetX() >= GRID_W) s.SetX(0);
                if (s.GetY() < 0)       s.SetY(GRID_H - 1);
                if (s.GetY() >= GRID_H) s.SetY(0);
            }
        }

        // Self collision
        for (int i = 1; i < (int)snakeList.size(); i++) {
            if (snakeList[i].GetX() == snakeList[0].GetX() &&
                snakeList[i].GetY() == snakeList[0].GetY()) return true;
        }

        // Collect board powerup
        if (boardPowerup.active &&
            snakeList[0].GetX() == boardPowerup.x &&
            snakeList[0].GetY() == boardPowerup.y) {
            SpawnParticles(boardPowerup.x, boardPowerup.y, PowerupColor(boardPowerup.type));
            ApplyPowerup(boardPowerup.type);
            boardPowerup.active = false;
            nextPowerupSpawn = t + 8.0f;
        }

        // Board powerup expiry
        if (boardPowerup.active && t > boardPowerup.expireTime) {
            boardPowerup.active = false;
            nextPowerupSpawn = t + 5.0f;
        }

        // Active powerup expiry
        if (activePowerupType != PU_NONE && t > activePowerupEnd) {
            if (activePowerupType == PU_GHOST) {
                bool tailsStillWrapping = false;
                for (int i = 1; i < (int)snakeList.size(); i++) {
                    int dx = abs(snakeList[i].GetX() - snakeList[i-1].GetX());
                    int dy = abs(snakeList[i].GetY() - snakeList[i-1].GetY());
                    if (dx > 2 || dy > 2) { tailsStillWrapping = true; break; }
                }
                ghostBorrowed = true;
                if (!tailsStillWrapping) ExpirePowerup();
            } else {
                ExpirePowerup();
            }
        }

        // Spawn new board powerup
        if (!boardPowerup.active && t > nextPowerupSpawn) SpawnPowerup();

    } else {
        // Ate the apple
        SpawnParticles(fruitY, fruitX, { 255, 100, 100, 255 });
        snakeList.insert(snakeList.begin(), Snakes(fruitY, fruitX, dir));
        score += scoreMultiplier;
        if (score > highScore) {
            highScore = score;
            SaveHighScore(highScore);
        }
        NewAppleSpawn();
        doomsDay = false;
    }

    Draw();
    return false;
}

// ---- Reset ----
void ResetGame() {
    snakeList.clear();
    snakeList.insert(snakeList.begin(), Snakes(GRID_W/2, GRID_H/2));
    dir = STOP; tailDir = STOP; nextDir = STOP;
    gameOver = false; doomsDay = false;
    score = 0; scoreMultiplier = 1;
    ghostMode = false; ghostBorrowed = false; gameSpeed = 0.08f;
    particles.clear();
    flashMessage = "";
    boardPowerup.active = false;
    activePowerupType   = PU_NONE;
    nextPowerupSpawn    = GetTime() + 5.0f;
    NewAppleSpawn();
    if (fruitX == GRID_H / 2) fruitX += 1;
}

// ---- Game over screen ----
void GameOverScreen(int finalScore) {
    float start = GetTime();
    while (!WindowShouldClose()) {
        float t = GetTime() - start;
        BeginDrawing();
        ClearBackground({ 12, 12, 12, 255 });

        // Shaking GAME OVER text
        int shake = t < 0.6f ? (int)(sinf(t * 35.0f) * 7.0f * (1.0f - t / 0.6f)) : 0;
        DrawText("GAME OVER", WIN_W/2 - 115 + shake, WIN_H/2 - 70, 52, RED);

        DrawText(TextFormat("Score: %d", finalScore), WIN_W/2 - 65, WIN_H/2 + 5,  28, WHITE);
        DrawText(TextFormat("Best:  %d", highScore),  WIN_W/2 - 65, WIN_H/2 + 40, 28, GOLD);

        // Pulsing prompt
        float pulse = 0.5f + 0.5f * sinf(t * 3.5f);
        Color prompt = { 170, 170, 170, (unsigned char)(140 + 110 * pulse) };
        DrawText("Press ENTER to play again", WIN_W/2 - 148, WIN_H/2 + 100, 22, prompt);

        EndDrawing();
        if (IsKeyPressed(KEY_ENTER)) break;
    }
}

// ---- Main ----
int main() {
    srand(time(NULL));
    highScore = LoadHighScore();

    InitWindow(WIN_W, WIN_H, "Snake");
    SetTargetFPS(120);

    ResetGame();

    float lastTick = GetTime();

    while (!WindowShouldClose()) {
        float t = GetTime();
        UpdateParticles(GetFrameTime());
        Movement();

        if (t - lastTick >= gameSpeed) {
            bool dead = Game();
            lastTick = t;
            if (dead) {
                GameOverScreen(score);
                ResetGame();
                lastTick = GetTime();
            }
        }
    }

    CloseWindow();
    return 0;
}
