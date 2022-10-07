#include <iostream>
#include <ncurses.h>
#include <thread>
#include <chrono>
#include <dispatch/dispatch.h>

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

int score = 0;
int misses = 0;

bool isGameStarted = false;
bool shouldExit = false;

int timeBeforeGameStarted = 15000;

//sem_t bulletSemaphore;
dispatch_semaphore_t bulletSemaphore = dispatch_semaphore_create(3);

void CreateBullet() {
    dispatch_semaphore_wait(bulletSemaphore, DISPATCH_TIME_FOREVER);
    int bullet[2] = { gunX, gunY };
        
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
    
    dispatch_semaphore_signal(bulletSemaphore);
}

void CreateEnemy() {
    while (true) {
        if (!isGameStarted) {
            continue;
        }
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
                if (!enemies[i] || !enemies[i][0]) {
                    enemies[i] = new int[2];
                    enemies[i][0] = rand() % width;
                    enemies[i][1] = 1 + (rand() % 2);
                    // add a single enemy, then sleep for 1 second
                    break;
                }
            }
        }
            
        this_thread::sleep_for(chrono::milliseconds(1000));
        
        // enemies advance logic
        for (int i = 0; i < 25; i++) {
            if (enemies[i] && enemies[i][0]) {
                int maxDirectionNumber = 1;
                int minDirectionNumber = -1;
                // todo: move to the application config
                // nextDirection shall be random so that a user doesn't know where to move the gun preventively
                int nextDirection = rand() % (maxDirectionNumber + 1 - minDirectionNumber) + minDirectionNumber;
                // 1 because we need to account for the wall
                if (enemies[i][0] + nextDirection < width && enemies[i][0] + nextDirection > 1) {
                    enemies[i][0] += nextDirection;
                }
                enemies[i][1] += 1;
            }
        }
        
        // check if there are misses
        for (int i = 0; i < 25; i++) {
            if (enemies[i] && enemies[i][0]) {
                if (enemies[i][1] == height - 1) {
                    misses++;
                    enemies[i] = new int[2];
                }
            }
        }
    }
}

void StartGame() {
    while (true) {
        if (timeBeforeGameStarted == 0) {
            isGameStarted = true;
            return;
        }
        
        timeBeforeGameStarted -= 1000;
        
        this_thread::sleep_for(chrono::milliseconds(1000));
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
    printw("Debug: ");
    printw("%d\n", debug);
    
    printw("Hits: ");
    printw("%d\n", score);
    
    printw("Misses: ");
    printw("%d\n", misses);
    
    if (!isGameStarted) {
        printw("GAME STARTS IN ");
        printw("%d\n", timeBeforeGameStarted / 1000);
    } else {
        printw("GAME STARTED\n");
    }
//    for (int i = 0; i < 3; i++) {
//        if (bullets[i]) {
//            printw("%d ", bullets[i][0]);
//            printw("%d\n\n", bullets[i][1]);
//        }
//    }
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
        case 32: { // space
            thread trackBullet(CreateBullet);
            trackBullet.detach();
            return;
        }
        case 10: // enter
            isGameStarted = true;
            break;
        case 88: // esc
            shouldExit = true;
            break;
        default:
            direction = -1;
    }
}

void Logic()
{
    // move the gun
    switch (direction) {
        case 1:
        {
            // account for the walls
            if (gunX < width - 1) {
                gunX++;
            }
            break;
        }
        case 0:
        {
            // account for the walls
            if (gunX > 1) {
                gunX--;
            }
            break;
        }
        default:
            break;
    }
    
    // check if the enemies are destroyed
    for (int i = 0; i < 25; i++) {
        if (enemies[i]) {
            for (int j = 0; j < 3; j++) {
                if (bullets[j]) {
                    if (enemies[i][0] == bullets[j][0] && enemies[i][1] == bullets[j][1]) {
                        // todo: Find out why the application crashes if we set the value to NULL;
                        enemies[i] = new int[2];
                        score++;
                    }
                }
            }
        }
    }
    
    if (shouldExit || misses == 30) {
        exit(0);
    }
    
    // increase chance of the enemy spawn with each 5 eliminated enemies
    if (score != 0 && score % 5 == 0) {
        chanceOfEnemySpawn += 10;
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
    thread startGame(StartGame);
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
    return 0;
}
