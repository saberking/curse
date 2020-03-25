#include <PDCurses-3.8/curses.h>
#include <stdlib.h>
#include <string>
#include<string.h>
#include <list>
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
class Light: public Obj{
    public:
    bool powered;
    Light():Obj{"light","provides (some) illumination", }, powered{true}{
    }
};
//comment
const char *inspectMessage="What would you like to inspect?\n";
const char *pickUpMessage="What would you like to pick up?\n";
class Room{
    public:
    string text;
    list<Obj> objects;
    bool visited;
};
Room startingRoom;
const char *actionList="    [i]nspect something\n\
    [p]ick something up\n";

void initRooms(){
    string startingText("You find yourself in a small, dimly-lit room. The walls, ceiling and floor are made of metal. \
    \nAround you are heaps of crumpled magazines and dusty circuit boards.\nThere is a metal door at one side of the room.\n\
    What would you like to do?\n");
    startingRoom.text=startingText;
    Obj *objects;
    Light l1;
    startingRoom.objects.push_back(l1);

}
char *input(int length=BUFF_LENGTH){
    char *buff = (char*)malloc(length);
    echo();
        wgetnstr(stdscr, buff, length-1);
        noecho();
    return buff;
}
void roomInfo(Room &room){
    mvaddstr(0,0,room.text.c_str());
    addstr(actionList);
    char c;
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
    string buff=input();
    for(std::list<Obj>::const_iterator i=room.objects.begin(),end=room.objects.end();i!=end;++i){
        if(buff.find(i->name)!=-1)
            addstr(i->desc.c_str());
    }
}
int main(){
     char c;
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr,FALSE);
initRooms();
    roomInfo(startingRoom);
    getch();

    endwin();
}