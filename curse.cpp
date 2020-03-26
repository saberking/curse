#include <PDCurses-3.8/curses.h>
#include <stdlib.h>
#include <string>
#include<string.h>
#include <vector>
#include<list>
#include<iostream>

#define noOfRooms 1
#define BUFF_LENGTH 100
using namespace std;
bool quit=FALSE;
WINDOW *wroom,  *wlog;

struct ObjAction{
    public:
    string name;
    char command;
    ObjAction(string _name):name{_name}{command=_name.c_str()[0];};
};

struct Obj{
    string name;
    string desc;
    bool item;
    vector<ObjAction*> actions;
    // vector<Obj *>hiddenObjects;
    virtual void applyAction(char command){};

    Obj(string _name, string _desc, vector<ObjAction*> *_actions=NULL){
        name=_name;
        desc=_desc;
        item=FALSE;
        if(_actions==NULL)actions=*(new vector<ObjAction*>);
        else actions=*_actions;
        // hiddenObjects=_hiddenObjects;
    };
};

struct Room{
    public:
    string text;
    list<Obj*> objects;
    bool visited;
    Room(string _text,list<Obj *>_objects):text{_text},objects{_objects}{
        visited=false;
    }
};
Room *startingRoom, *currentRoom, *passage;

Obj *selectedObject=NULL;
struct Item:Obj{
    int weight;
    Item(string _name, string _desc, vector<ObjAction*> *_actions=NULL, int _weight=1):Obj{_name,_desc,_actions}{
    weight=_weight;
    item=TRUE;
    }
};
list<Item*> inventory;
struct Light: public Obj{
    public:
    bool powered;
    Light():Obj{"light","provides (some) illumination", }, powered{true}{
    }
};
struct Wall:public Obj{
    Wall():Obj{"wall","hard and metallic"}{}

};

struct Ceiling:public Obj{
    Ceiling():Obj{"ceiling","you can almost reach it"}{}
};
struct Floor:public Obj{
    Floor():Obj{"floor","it is what you are standing on"}{}
};

struct Door:public Obj{
    public:
    bool locked;
    void applyAction(char command){
        if(command=='o'){
            if(locked)waddstr(wlog,"To your dismay, the door seems to be locked\n");
            else waddstr(wlog,"You open the door. Unfortunately, what is behind it does not seem interesting\n");
            wrefresh(wlog);
        }
    }
    Door():Obj{"door","keeps the bad guys out"}, locked{true}{actions.push_back(new ObjAction("open"));};

};
struct Duct:Obj {
    Duct():Obj{"duct","just wide enough to crawl through", new vector<ObjAction *>{new ObjAction{"crawl"}}}{};
    void applyAction(char command){
        if(command=='c'){
            currentRoom=passage;
            wclear(wroom);
            wrefresh(wroom);
            waddstr(wlog,"You crawl into the duct and start moving along\
            \nIt is pitch dark but you can see a faint glimmer in the distance.\
            \nSoon you reach the the end, which is also covered by a panel.\
            \nYou punch the panel loose and climb out.\n");
            selectedObject=NULL;
            wrefresh(wlog);
            wgetch(wlog);
            wclear(wlog);
            wrefresh(wlog);
        }
    };
};
Duct duct;
struct RemovablePanel: Obj{
    RemovablePanel():Obj{"panel","It looks like you could easily remove this", new vector<ObjAction*>{new ObjAction("remove")}}{
    
    };
    void applyAction(char command){
        if(command=='r'){
            startingRoom->objects.push_back(&duct);
            desc="lying on the floor";
            actions=*(new vector<ObjAction*>);
            waddstr(wlog,"You remove the panel. Behind it is a ventilation duct\n");
        }
    }
};
RemovablePanel panel;
//comment

const char *inspectMessage="What would you like to inspect?\n";
const char *pickUpMessage="What would you like to pick up?\n";

const char *actionList="What would you like to do?\n  [i]nspect something\n  [p]ick something up\n";

void initRooms(){
    string startingText("You find yourself in a small, dimly-lit room. \nThe walls, ceiling and floor are made of metal. \
    \nAround you are heaps of crumpled magazines and dusty circuit boards.\nThere is a metal door at one side of the room.");
    list<Obj *> *objects=new list<Obj *>{
        new Light, new Door, new Ceiling, new Floor, 
        new Obj("wall", "As you examine the wall, you notice a loose ventilation panel held in by a single screw"),
        &panel,
        new Item("magazine", "the magazines are mostly centred on fashion tips for various species"),
        new Item("circuit board", "even without specialist knowledge, you can tell that these will not work")
        };
    startingRoom=new Room(startingText, *objects);
    currentRoom=startingRoom;
    list<Obj *> *pobjects=new list<Obj *>{
        new Wall, new Light, new Ceiling, new Floor
    };
    passage=new Room("You emerge into a wide corridor similar to the previous room", *pobjects);
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

char *input(){
    echo();
    char *buff=(char*)malloc(BUFF_LENGTH);
        wgetnstr(wlog, buff, BUFF_LENGTH-1);
        noecho();
    return buff;
}
void roomInfo(Room *room){
    wclear(wroom);
        // box(wroom,0,0);

    waddstr(wroom, room->text.c_str());
    room->visited=true;
    wrefresh(wroom);
}
void objInfo(Obj &obj){


        wprintw(wlog,"%s\n",obj.desc.c_str());

    wrefresh(wlog);
}
Obj* getObjByName(string name){
    for(std::list<Obj *>::iterator i=currentRoom->objects.begin(),end=currentRoom->objects.end();i!=end;++i){
        if(name.find((**i).name)!=-1)return *i;
    }
    return NULL;
}
Obj *selectObj(){
    Obj *obj=NULL;
    do{
        char * buff=input();
        obj=getObjByName(buff);
        free(buff);
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
    Obj *obj=selectObj();
    if(obj->item){
        wprintw(wlog, "You pick up the %s\n",obj->name.c_str());
        inventory.push_back((Item*)obj);
        currentRoom->objects.remove(obj);
    }else
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
    while(quit==FALSE){
            roomInfo(currentRoom);

        logInfo();
    }

    endwin();
}