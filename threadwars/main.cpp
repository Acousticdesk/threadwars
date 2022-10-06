#include <iostream>
#include <ncurses.h>
#include <thread>
#include <chrono>

using namespace std;

int width = 50;
int height = 25;

int gunX = width / 2;
int gunY = height - 1;
int debug = 0;
int chanceOfEnemySpawn = 20;

int enemyTimerMs = 1000;

int direction = -1;

int** bullets = new int*[3];
int** enemies = new int*[25];

void CreateBullet() {
    int bullet[2] = { gunX, gunY };
    
    int bulletsOnTheScreen = 0;
    
    // check how many bullets do we already have
    for (int i = 0; i < 3; i++) {
        if (bullets[i]) {
            bulletsOnTheScreen++;
        }
    }
    
    // no need to fire new bullets is the maximum amount was reached
    if (bulletsOnTheScreen == 3) {
        return;
    }
        
    // Adding a next bullet to the array (fire the bullet)
    
    // max amount of bullets is 3, todo: move it to a constant and initialize during the setup phase
    int indexOfBullet = -1;
    
    for (int i = 0; i < 3; i++ ) {
        if (!bullets[i]) {
            indexOfBullet = i;
            bullets[i] = new int[2];
            for (int j = 0; j < 2; j++) {
                bullets[i][j] = bullet[j];
            }
            break;
        }
    }
    
    bool isBulletOnScreen = true;
    
    while ((isBulletOnScreen = bullets[indexOfBullet][1] > 0)) {
        bullets[indexOfBullet][1]--;
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    
     bullets[indexOfBullet] = NULL;
}

void CreateEnemy() {
    while (true) {
        // calculate chance of enemy spawn
        int chance = rand() % 101;
        
        debug = chance;
        
        if (chance < chanceOfEnemySpawn) {
        
            // todo: increase a chance of enemy spawn
            int enemiesCount = 0;
            
            for (int i = 0; i < 25; i++) {
                if (enemies[i]) {
                    enemiesCount++;
                }
            }
            
            // we don't need to create more than MAX_ENEMIES enemies
            if (enemiesCount == 25) {
                return;
            }
            
            for (int i = 0; i < 25; i++) {
                if (!enemies[i]) {
                    enemies[i] = new int[2];
                    enemies[i][0] = rand() % width;
                    enemies[i][1] = 1 + (rand() % 2);
                    // add a single enemy, then sleep for 1 second
                    break;
                }
            }
        }
            
        this_thread::sleep_for(chrono::milliseconds(1000));
        
        for (int i = 0; i < 25; i++) {
            if (enemies[i]) {
                int maxDirectionNumber = 1;
                int minDirectionNumber = -1;
                // todo: move to the application config
                // nextDirection shall be random so that a user doesn't know where to move the gun preventively
                int nextDirection = rand() % (maxDirectionNumber + 1 - minDirectionNumber) + minDirectionNumber;
                if (enemies[i][0] + nextDirection < width) {
                    enemies[i][0] += nextDirection;
                }
                enemies[i][1] += 1;
            }
        }
    }
}

void Draw()
{
  for (int i = 0; i <= height; i += 1)
  {
    for (int j = 0; j <= width; j += 1)
    {
        if (i == gunY && j == gunX) {
          printw("X");
          continue;
        }
 
        if (i == 0 || i == height || j == 0 || j == width) {
          printw("#");
          continue;
        }
        
        bool isBulletFound = false;
        
        for (int k = 0; k < 3; k++) {
            if (bullets[k] && bullets[k][1] == i && bullets[k][0] == j) {
                isBulletFound = true;
                printw("x");
            }
        }
        
        if (isBulletFound) {
            continue;
        }
        
        bool isEnemyFound = false;
        
        for (int k = 0; k < 25; k++) {
            if (enemies[k] && enemies[k][1] == i && enemies[k][0] == j) {
                isEnemyFound = true;
                printw("o");
            }
        }
        
        if (isEnemyFound) {
            continue;
        }

        printw(" ");
    }
      printw("\n");
  }
    printw("%d", debug);
    for (int i = 0; i < 3; i++) {
        if (bullets[i]) {
            printw("%d ", bullets[i][0]);
            printw("%d\n\n", bullets[i][1]);
        }
    }
}

void Input()
{
    switch(getch()) {
        case 67:    // key right
            direction = 1;
            break;
        case 68:    // key left
            direction = 0;
            break;
        case 32: {
            thread trackBullet(CreateBullet);
            trackBullet.detach();
            return;
        }
        default:
            direction = -1;
    }
}

void Logic()
{
    // move the gun
    switch (direction) {
        case 1:
            gunX++;
            break;
        case 0:
            gunX--;
            break;
        default:
            break;
    }
    
    // check if the enemies are destroyed
    for (int i = 0; i < 25; i++) {
        if (enemies[i]) {
            for (int j = 0; j < 3; j++) {
                if (bullets[j]) {
                    if (enemies[i][0] == bullets[j][0] && enemies[i][1] == bullets[j][1]) {
                        enemies[i] = NULL;
                    }
                }
            }
        }
    }
}

void Setup() {
    // make sure rand() returns new values each time
    srand((unsigned int)time(NULL));
}

int main() {
    initscr();
    noecho();
    nodelay(stdscr, TRUE);
    // todo: probably create a thread that will create threads
    thread createEnemy(CreateEnemy);
    createEnemy.detach();
    Setup();
    while (true) {
        // First get input from the user, then claculate the changes, then draw the updates
        Input();
        erase();
        // Erase shall be kept under the Input call. Otherwise flickering happens. This is because
        // erase should never be called after the wgetch();
        Logic();
        Draw();
        refresh();
    }
//     endwin();
//    free(bullets);
//    free(enemies);
    return 0;
}
