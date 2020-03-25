#include <PDCurses-3.8/curses.h>
#include <stdlib.h>
char *buff;
#define noOfRooms 1
struct obj{
    char *name;
    char *desc;
    int weight;
};

struct room{
    const char *text;
    struct obj *objects;
    bool visited;
};
const char *rooms[noOfRooms]={
    "You find yourself in a small, dimly-lit room. The walls, ceiling and floor are made of metal. \
    \nAround you are heaps of crumpled magazines and dusty circuit boards.\nThere is a metal door at one side of the room.\n\
    What would you like to do?\n\
    [i]nspect something\n\
    [p]ick something up\n"
};

int main(){
     char c;
    initscr();
    cbreak();
    nodelay(stdscr,FALSE);
    mvaddstr(0,0,rooms[0]);
    do{
        c=getch();
    }while (c!='i' &&c!='p');
    

    endwin();
}