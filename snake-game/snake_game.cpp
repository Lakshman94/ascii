#include <stdio.h>
#include <ncurses.h>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <array>
#include <iostream>

using namespace std;

/*
 1. Move around with one char w/ edge rollover
 2. randomize food coordinates; reappear after eating
 3. increase snake size
 4. die if self eat
 */

void move_head (WINDOW *win, int *y, int *x, int crawl) {

    int ymax, xmax, agsx, agsy;
    
    getmaxyx(win, ymax, xmax);
    agsx=xmax-2; agsy=ymax-2;
    int c;
    int j = *y;
    int i = *x;
    if (crawl==1) {
        i++;
        if (i>agsx) i-=agsx;
    } else if (crawl==-1) {
        i--;
        if (i<1) i+=agsx;
    } else if (crawl==2) {
        j--;
        if (j<1) j+=agsy;
    } else if (crawl==-2) {
        j++;
        if (j>agsy) j-=agsy;
    } else {
        wrefresh(win);
    }

    *y=j; *x=i;
}

int main(int argc, char **argv) {

    initscr();
    cbreak();
    noecho();

    WINDOW *win;
    int c;
    int killed=0;
    int y,x, ybeg, xbeg, ymax, xmax;
    const char *snake = "*";
    int highlight=0;

    int game_size_y, game_size_x, agsy, agsx;
    int sy, sx;
    int xdirection=0; // 1 is right; -1 is left;
    int ydirection=0; // 1 is up; -1 is down;
    int i,j;
    int delay=1;
    int crawl=1;
    int prev_crawl=0;
    int eggx, eggy;
    int no_egg=1;
    int chain=1;
    int body_j[100] = {0};
    int body_i[100] = {0};

    // Get the terminal size
    getyx(stdscr, y, x);
    getbegyx(stdscr, ybeg, xbeg);
    getmaxyx(stdscr, ymax, xmax);

    // Test size: 20x40
    game_size_y = 20; game_size_x = 40;
    agsy = game_size_y-2; agsx=game_size_x-2;
    sy = ybeg+2; sx = xbeg+2;

    win = newwin(game_size_y, game_size_x, ybeg+2, xbeg+2);
    box(win, 0, 0);
    refresh();

    keypad(win, true);
    nodelay(win, true);

    // i & j coordinates of the snake head;
    i = 2;//rand() % agsx;
    j = 2;//rand() % agsy;

    while (1) {
        // move right at start and rollover if hit an edge
        // TODO: use enum for directions.


        int c = wgetch(win);
        switch (c) {
            case KEY_RIGHT:
                if (crawl==-1) break;
                crawl=1; move_head(win, &j, &i, crawl);break;
            case KEY_LEFT:
                if (crawl==1) break;
                crawl=-1; move_head(win, &j, &i, crawl);break;
            case KEY_UP:
                if (crawl==-2) break;
                crawl=2; move_head(win, &j, &i, crawl);break;
            case KEY_DOWN:
                if (crawl==2) break;
                crawl=-2; move_head(win, &j, &i, crawl);break;
            default:
                move_head(win, &j, &i, crawl);break;
        }

        // kill condition
        for (int cnt=0; cnt<chain; cnt++) {
            if (i==body_i[cnt] && j==body_j[cnt]) {
                killed=1;
                break;
            }
        }

        if (killed) break;

        if (chain==1) {
            mvwprintw(win, body_j[0], body_i[0], " ");
        } else {
            mvwprintw(win, body_j[chain-1], body_i[chain-1], " ");
        }

        for (int a=chain-1;a>0;a--) {
            body_j[a]=body_j[a-1];
            body_i[a]=body_i[a-1];
            mvwprintw(win, body_j[a], body_i[a], "*");
        }
        body_j[0]=j; body_i[0]=i;
        mvwprintw(win, j, i, "*");


        if (no_egg) {
            eggx = rand()%agsx; if (!eggx) eggx++;
            eggy = rand()%agsy; if (!eggy) eggy++;
            mvwprintw(win, eggy, eggx, "@");
            no_egg=0;
        }

        if (j==eggy && i==eggx) {
             eggx = rand() % agsx;
             eggy = rand() % agsy;
             chain++;
             no_egg=1;
        }

        wrefresh(win);
        usleep(100000);

    }
    mvprintw(j, i, "The snake is dead af");
    c = getch();
    endwin();
    return 0;
}
