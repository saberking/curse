#include <PDCurses-3.8/curses.h>
#include <stdlib.h>
#include <string>
#include<string.h>
#include <vector>
#include<iostream>

#define noOfRooms 1
#define BUFF_LENGTH 100
using namespace std;
bool quit=FALSE;
WINDOW *wroom,  *wlog;

class ObjAction{
    public:
    string name;
    char command;
    ObjAction(string _name):name{_name}{command=_name.c_str()[0];};
};

ObjAction open("open");
class Obj{
    public:
    string name;
    string desc;
    vector<ObjAction*> actions;
    virtual void applyAction(char command){};

    Obj(string _name, string _desc, vector<ObjAction*> const &_actions=vector<ObjAction*>()){
        name=_name;
        desc=_desc;
        actions=_actions;
    };
};

Obj *selectedObject=NULL;
class Light: public Obj{
    public:
    bool powered;
    Light():Obj{"light","provides (some) illumination", }, powered{true}{
    }
};
class Wall:public Obj{
    Wall():Obj{"wall","hard and metallic"}{}

};
class Ceiling:public Obj{
    Ceiling():Obj{"ceiling","you can almost reach it"}{}
};
class Floor:public Obj{
    Floor():Obj{"floor","it is what you are standing on"}{}
};

class Door:public Obj{
    public:
    bool locked;
    void applyAction(char command){
        if(command=='o'){
            if(locked)waddstr(wlog,"To your dismay, the door seems to be locked\n");
            else waddstr(wlog,"You open the door. Unfortunately, what is behind it does not seem interesting\n");
        }
    }
    Door():Obj{"door","keeps the bad guys out"}, locked{true}{actions.push_back(&open);};

};

//comment

const char *inspectMessage="What would you like to inspect?\n";
const char *pickUpMessage="What would you like to pick up?\n";
class Room{
    public:
    string text;
    vector<Obj*> objects;
    bool visited;
};
Room startingRoom;
Door d1;
Light l1;
const char *actionList="What would you like to do?\n  [i]nspect something\n  [p]ick something up\n";

void initRooms(){
    string startingText("You find yourself in a small, dimly-lit room. \nThe walls, ceiling and floor are made of metal. \
    \nAround you are heaps of crumpled magazines and dusty circuit boards.\nThere is a metal door at one side of the room.");
    startingRoom.text=startingText;
    startingRoom.objects.push_back(&l1);
    startingRoom.objects.push_back(&d1);
}

void initCurses(){
    initscr();
    cbreak();
    refresh();
    noecho();
    wroom=newwin(8, 110, 1, 1);
    wlog=newwin(12,110,10,1);
    nodelay(stdscr,FALSE);
    keypad(stdscr, TRUE);
    wmove(wlog,0,0);
    refresh();


}

char *input(int length=BUFF_LENGTH){
    echo();
    char *buff = (char*)malloc(length);
        wgetnstr(wlog, buff, length-1);
        noecho();
    return buff;
}
void roomInfo(Room &room){
    wclear(wroom);
        // box(wroom,0,0);

    waddstr(wroom, room.text.c_str());
    room.visited=true;
    wrefresh(wroom);
}
void objInfo(Obj &obj){


        wprintw(wlog,"%s\n",obj.desc.c_str());

    wrefresh(wlog);
}
Obj* getObjByName(string name){
    for(std::vector<Obj *>::iterator i=startingRoom.objects.begin(),end=startingRoom.objects.end();i!=end;++i){
        if(name.find((**i).name)!=-1)return *i;
    }
    return NULL;
}
Obj *selectObj(){
    Obj *obj=NULL;
    do{
        string buff=input();
        obj=getObjByName(buff);
    }while(obj==NULL);
    return obj;
}
void inspect(){
    waddstr(wlog,inspectMessage);
    selectedObject=selectObj();
    objInfo(*selectedObject);
}
void pickUp(){
    waddstr(wlog,pickUpMessage);
    selectObj();
    waddstr(wlog,"it is too heavy\n");
}

void logInfo(){
        // box(wlog,0,0);

    char availableCommands[30];
    availableCommands[29]='\0';
    availableCommands[0]='i';availableCommands[1]='p';availableCommands[2]='q';
    int index=3;
            waddstr(wlog,actionList);

    if(selectedObject!=NULL){
        for(vector<ObjAction*>::const_iterator i=selectedObject->actions.begin(),end=selectedObject->actions.end();i!=end;++i){
            availableCommands[index++]=(*i)->command;
                        const char *cstr=(**i).name.c_str();
            char str[100];
            sprintf(str, "  [%c]%s %s\n", cstr[0], cstr+1, selectedObject->name.c_str());
            waddstr(wlog,str);
        }
    }
    wrefresh(wlog);
    char c=';';
    do{
        c=wgetch(wlog);
    }while (strchr(availableCommands, c)==NULL);
        wclear(wlog);

    switch(c){
        case 'i':
            inspect();
            break;
        case 'p':
            pickUp();
            break;
        case 'q': quit=true;
        break;
        default:   
        selectedObject->applyAction(c);
                    break;

    }
    wrefresh(wlog);
}
int main(){
    initRooms();
        initCurses();
    roomInfo(startingRoom);
    while(quit==FALSE){
        logInfo();
    }

    endwin();
}