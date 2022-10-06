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

int direction = -1;

int** bullets = new int*[3];

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
    
//    int indexOfBullet = -1;
//
//    if (bulletsOnTheScreen == 3) {
//        bullets[0] = new int[2];
//
//        for (int j = 0; j < 2; j++) {
//            bullets[0][j] = bullet[j];
//        }
//    } else {
//        for (int i = 0; i < 3; i++ ) {
//            if (!bullets[i]) {
//                bullets[i] = new int[2];
//                for (int j = 0; j < 2; j++) {
//                    bullets[i][j] = bullet[j];
//                }
//            }
//        }
//    }
    
//    int numberOfBullets = sizeof(bullets) / sizeof(bullets[0]);
    
//    for (int i = 0; i < numberOfBullets; i++) {
//        if (bullets[i + 1]) {
//            bullets[i] = bullets[i + 1];
//        } else {
//            break;
//        }
//    }
//
//    bullets[numberOfBullets - 1] = { bulletX, bulletY };
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

        printw(" ");
    }
      printw("\n");
  }
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
//            CreateBullet();
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
}

int main() {
    initscr();
    noecho();
    nodelay(stdscr, TRUE);
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
    // endwin();
    return 0;
}
