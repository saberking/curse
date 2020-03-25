#include <PDCurses-3.8/curses.h>
#include <stdlib.h>
#include <string>
#include<string.h>
#define noOfRooms 1
#define BUFF_LENGTH 100
using namespace std;

class Obj{
    public:
    string name;
    string desc;
    bool fixed;
    int weight;
    Obj(string _name, string _desc, bool _fixed=true, int _weight=1){
        name=_name;
        desc=_desc;
        fixed=_fixed;
        weight=_weight;
    }
};
//comment
const char *inspectMessage="What would you like to inspect?\n";
const char *pickUpMessage="What would you like to pick up?\n";
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

char *input(int length=BUFF_LENGTH){
    char *buff = (char*)malloc(length);
    echo();
        wgetnstr(stdscr, buff, length-1);
        noecho();
    return buff;
}
int main(){
    Obj light("light", "provides (some) illumination");
     char c;
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr,FALSE);
    mvaddstr(0,0,rooms[0]);
    do{
        c=getch();
    }while (c!='i' &&c!='p');
    switch(c){
        case 'i':
            addstr(inspectMessage);
            break;
        case 'p':
            addstr(pickUpMessage);
            break;
    }
    char *buff=input();
    if(strstr(buff, "light")){
        addstr(light.desc.c_str());
    }
    getch();

    endwin();
}