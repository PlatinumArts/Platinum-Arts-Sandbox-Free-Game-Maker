#include "game.h"

namespace MapEntities
{

//    vector<extentity*> ents;
//    bool senduids = false;

//    int  getmapuid(extentity &e) { return ((StaticEntity&)e).getid(); }
//
//    StaticEntity *findmapuid(int id)
//    {
//        loopv(ents) if(ents[i]->getid() == id)
//        {
//            return ents[i];
//        }
//        return NULL;
//    }
//
//    StaticEntity *findmapuid(int id, int type)
//    {
//        loopv(ents) if(ents[i]->getid() == id && ents[i]->type == type)
//        {
//            return ents[i];
//        }
//        return NULL;
//    }

//    void setmapuid(int i, int uid)
//    {
//        if(!ents.inrange(i)) return;
//
//        if(ents[i]->uid >= 0)
//        {
//            if(uid == ents[i]->uid) return;
//            else fatal("Error: MapEntities::setmapuid - uid differs from server uid, uid: %d %d", ents[i]->uid, uid);
//        }
//        ents[i]->uid = uid;
//    }
//
//    void sendmapuids(bool send) { senduids = send; }

//    bool sendmapuids() { return senduids; }

    void putentities(packetbuf &p)
    {
//        putint(p, SV_MAPENTITIES);
//        loopv(ents)
//        {
//            putint(p, i);
//        }
//        putint(p, -2);
//        sendmapuids(false);
    }


//
//    void delentity(StaticEntity *e)
//    {
//        delete e;
//    }

//    void delentity(extentity *e)
//    {
//        delete e;
//    }
//
//    void clearents()
//    {
//        while(ents.length()) delentity(ents.pop());
//    }
//
//    void listwaypoints()
//    {
//        vector<char> buf;
//        string lst;
//        loopv(ents) if(ents[i]->type == WAYPOINT)
//        {
//        	StaticEntity& e = *ents[i];
//            formatstring(lst)("%d", e.getid());
//            buf.put(lst, strlen(lst));
//            if(i < (ents.length()-1)) buf.add(' ');
//        }
//        buf.add('\0');
//        result(buf.getbuf());
//    }
//    ICOMMAND(listwaypoints, "", (), listwaypoints());
}
