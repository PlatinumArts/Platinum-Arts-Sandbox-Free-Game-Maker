#include "game.h"

namespace MapEntities
{
    VARP(showlighting, 0, 1, 1);
    VARP(showdynlights, 0, 1, 1);


    bool dynlightsactive[32767];
    vector<StaticEntity *> ents;
    bool senduids = false;

    int  getmapuid(extentity &e) { return ((StaticEntity&)e).uid; }

    StaticEntity *findmapuid(int uid)
    {
        loopv(ents) if(ents[i]->uid == uid)
        {
            return ents[i];
        }
        return NULL;
    }

    StaticEntity *findmapuid(int uid, int type)
    {
        loopv(ents) if(ents[i]->uid == uid && ents[i]->type == type)
        {
            return ents[i];
        }
        return NULL;
    }

    void setmapuid(int i, int uid)
    {
        if(!ents.inrange(i)) return;

        if(ents[i]->uid >= 0)
        {
            if(uid == ents[i]->uid) return;
            else fatal("Error: MapEntities::setmapuid - uid differs from server uid, uid: %d %d", ents[i]->uid, uid);
        }
        ents[i]->uid = uid;
    }

    void sendmapuids(bool send) { senduids = send; }

    bool sendmapuids() { return senduids; }

    void putentities(packetbuf &p)
    {
        putint(p, SV_MAPENTITIES);
        loopv(ents)
        {
            putint(p, i);
            putint(p, ents[i]->uid);
        }
        putint(p, -2);
        sendmapuids(false);
    }

    void renderentities()
    {
        loopv(ents)
        {
            extentity &e = *ents[i];
            int revs = 10;
            switch(e.type)
            {
                case CARROT:
                case RESPAWNPOINT:
                    if(e.attr[1]) revs = 1;
                    break;
                case TELEPORT:
                    if(e.attr[1] < 0) continue;
                    break;
                default:
                    if(!e.spawned || e.type < I_SHELLS || e.type > I_QUAD) continue;
            }
            const char *mdlname = entities::entmodel(e);
            if(mdlname)
            {
                vec p = e.o;
                p.z += 1+sinf(lastmillis/100.0+e.o.x+e.o.y)/20;
				rendermodel(&e.light, mdlname, ANIM_MAPMODEL|ANIM_LOOP, p, lastmillis/(float)revs, 0, 0, MDL_SHADOW | MDL_CULL_VFC | MDL_CULL_DIST | MDL_CULL_OCCLUDED, NULL, NULL, 0, 0, 1, 0xFFFFFF);
               //rendermodel(&e.light, mdlname, ANIM_MAPMODEL|ANIM_LOOP, p, lastmillis/(float)revs, MDL_SHADOW | MDL_CULL_VFC | MDL_CULL_DIST | MDL_CULL_OCCLUDED);
            }
        }
    }

    void initdynlights()
    {
        for(int i = 0; i != 32767; i++)
        {
            MapEntities::dynlightsactive[i] = true;
        }
    }

    void setdynlightactivity(int *who, int *active)
    {
       if (*who > 32767 || *who <= 0) return;
       else if (!*active) dynlightsactive[*who] = false;
       else dynlightsactive[*who] = true;
    }
    COMMAND(setdynlightactivity, "ii");

    #define renderfocus(i,f) { StaticEntity &e = *ents[i]; f; }
    void renderentlight(StaticEntity &e)
    {
        vec color(e.attr[1], e.attr[2], e.attr[3]);
        color.div(255.f);
        adddynlight(vec(e.o), float(e.attr[0] != 0 ? e.attr[0] > 0 ? e.attr[0] : 0 : getworldsize() ), color);
    }

    void adddynlights()
    {
        loopv(ents)
        {
            if((ents[i]->attr[4] > -1 && ents[i]->type == DYNLIGHT && dynlightsactive[ents[i]->attr[4]] && (!editmode || showdynlights)) //dynlights
                || ((entgroup.find(i) >= 0 || enthover == i) &&    (ents[i]->type == LIGHT) && (editmode && showlighting))) //normal lights
            {
                renderfocus(i, renderentlight(e));
            }
        }
    }

    void delentity(StaticEntity *e)
    {
        delete e;
    }

    void delentity(extentity *e)
    {
        delete (StaticEntity*)e;
    }

    void clearents()
    {
        while(ents.length()) delentity(ents.pop());
    }

    void listwaypoints()
    {
        vector<char> buf;
        string lst;
        loopv(ents) if(ents[i]->type == WAYPOINT)
        {
            formatstring(lst)("%d", ents[i]->uid);
            buf.put(lst, strlen(lst));
            if(i < (ents.length()-1)) buf.add(' ');
        }
        buf.add('\0');
        result(buf.getbuf());
    }
    ICOMMAND(listwaypoints, "", (), listwaypoints());
}
