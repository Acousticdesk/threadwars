#include <iostream>
#include <ncurses.h>

using namespace std;

int width = 50;
int height = 25;

int gunX = width / 2;
int gunY = height - 1;

int direction = -1;

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

        printw(" ");
    }
      printw("\n");
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
        // Erase shall be kept under the Input call. Otherwise flickering happens. This is because
        // erase should never be called after the wgetch();
        erase();
        Logic();
        Draw();
        refresh();
        
    }
    return 0;
}
