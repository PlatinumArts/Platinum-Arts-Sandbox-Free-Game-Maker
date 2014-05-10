#ifndef __MAP_ENTITIES_H__
#define __MAP_ENTITIES_H__

#include "game.h"
#include "map.h"

namespace MapEntities
{
//    class StaticEntity :  public extentity, public MapObject
//    {
//	public:
//        int triggerstate, lasttrigger;
//
//        StaticEntity() : triggerstate(TRIGGER_RESET), lasttrigger(0) {}
//		~StaticEntity() {}
//
//        void setuid(int i) { }
//        int  getuid() { return uid; }
//    };

//    extern vector<extentity *> ents;

//    int  getmapuid(extentity &e);
//    StaticEntity* findmapuid(int uid);
//    StaticEntity* findmapuid(int uid, int type);
//    void setmapuid(int i, int uid);
//    void sendmapuids(bool send);        //set flag
//    bool sendmapuids();                 //query flag
    void putentities(packetbuf &p);     //send map entities with uid to server

//    void initdynlights();
//    void adddynlights();
//    void renderentlight(StaticEntity &e);

    //wrapper functions - see entities.h
//    void delentity(StaticEntity *e);
//    void delentity(extentity *e);
//    void clearents();
}
#endif // __MAP_ENTITIES_H__
