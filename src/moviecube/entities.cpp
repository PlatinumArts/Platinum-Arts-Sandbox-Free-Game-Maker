#include "game.h"

//TODO: handle trigger for mp and sp

//TODO: TRIGGER

namespace entities
{
    //declared in igame.h
    int extraentinfosize() { return sizeof(int); }       // size in bytes of what the 2 methods below read/write... so it can be skipped by other games
    //declared in igame.h
    void writeent(entity &e, char *buf)   // write any additional data to disk (except for ET_ ents)
    {
        memcpy(buf, &((MapEntities::StaticEntity &)e).uid, sizeof(int));
    }
    //declared in igame.h
    void readent(entity &e, char *buf, int ver)     // read from disk, and init
    {
// TODO (offtools#1#): check map type (moviecube or others)

//        int ver = getmapversion();
//        if(ver <= 10)
//        {
//            if(e.type >= 7) e.type++;
//        }
//        if(ver <= 12)
//        {
//            if(e.type >= 8) e.type++;
//        }
        memcpy(&((MapEntities::StaticEntity &)e).uid, buf, sizeof(int));
    }

#ifndef STANDALONE
    using namespace game;

    //declared in igame.h
    vector<extentity *> &getents() { return (vector<extentity *> &)MapEntities::ents; }
    //declared in igame.h
    bool mayattach(extentity &e) { return false; }
    //declared in igame.h
    bool attachent(extentity &e, extentity &a) { return false; }

    //move to cmode
    const char *itemname(int i) //TODO: item names
    {
//        int t = ents[i]->type;
//        if(t<I_SHELLS || t>I_QUAD) return NULL;
//        return itemstats[t-I_SHELLS].name;
    	return NULL;
    }

    //not needed - move to cmode
    const char *entmdlname(int type)
    {
        static const char *entmdlnames[] =
        {
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            "ammo/shells", "ammo/bullets", "ammo/rockets", "ammo/rrounds", "ammo/grenades", "ammo/cartridges",
            "health", "boost", "armor/green", "armor/yellow", "quad", "teleporter",
            NULL, NULL,
            "carrot",
            NULL, NULL,
            "checkpoint",
            NULL, NULL,
            NULL, NULL,
            NULL, NULL,
            NULL,
            NULL
        };
        return entmdlnames[type];
    }

    //declared in igame.h
    const char *entmodel(const entity &e)
    {
        if(e.type == TELEPORT)
        {
            if(e.attr[1] > 0) return mapmodelname(e.attr[1]);
            if(e.attr[1] < 0) return NULL;
        }
        return e.type < MAXENTTYPES ? entmdlname(e.type) : NULL;
    }

    //not needed - move to cmode
    void preloadentities()
    {
        /*loopi(MAXENTTYPES)
        {
            switch(i)
            {
                case I_SHELLS: case I_BULLETS: case I_ROCKETS: case I_ROUNDS: case I_GRENADES: case I_CARTRIDGES:
                    if(m_noammo) continue;
                    break;
                case I_HEALTH: case I_BOOST: case I_GREENARMOUR: case I_YELLOWARMOUR: case I_QUAD:
                    if(m_noitems) continue;
                    break;
                case CARROT: case RESPAWNPOINT:
                    if(!m_classicsp) continue;
                    break;
            }
            const char *mdl = entmdlname(i);
            if(!mdl) continue;
            preloadmodel(mdl);
        } */
    }

    //declared in igame.h
    void rumble(const extentity &e)
    {
        playsound(S_RUMBLE, &e.o);
    }
    //declared in igame.h
    void trigger(extentity &e)
    {
        switch(e.attr[2])
        {
            case 29:
                break;
        }
    }
    //move to cmode
    void addammo(int type, int &v, bool local)
    {
//        itemstat &is = itemstats[type-I_SHELLS];
//        v += is.add;
//        if(v>is.max) v = is.max;
//        if(local) msgsound(is.sound);
    }
    //not needed - move to cmode
    void repammo(DynamicEntity *d, int type, bool local)
    {
//        addammo(type, d->ammo[type-I_SHELLS+GUN_SG], local);
    }

    // these two functions are called when the server acknowledges that you really
    // picked up the item (in multiplayer someone may grab it before you).

    //not needed - move to cmode
    void pickupeffects(int n, DynamicEntity *d)
    {
//        if(!ents.inrange(n)) return;
//        int type = ents[n]->type;
//        if(type<I_SHELLS || type>I_QUAD) return;
//        ents[n]->spawned = false;
//        if(!d) return;
//        itemstat &is = itemstats[type-I_SHELLS];
//        if(d!=player1 || isthirdperson()) particle_text(d->abovehead(), is.name, PART_TEXT, 2000, 0xFFC864, 4.0f, -8);
//        playsound(itemstats[type-I_SHELLS].sound, d!=player1 ? &d->o : NULL);
//        d->pickup(type);
//        if(d==player1) switch(type)
//        {
//            case I_BOOST:
//                conoutf(CON_GAMEINFO, "\f2you have a permanent +10 health bonus! (%d)", d->maxhealth);
//                playsound(S_V_BOOST);
//                break;
//
//            case I_QUAD:
//                conoutf(CON_GAMEINFO, "\f2you got the quad!");
//                playsound(S_V_QUAD);
//                break;
//        }
//        else if(d->ai) ai::pickup(d, *ents[n]);
    }

    // these functions are called when the client touches the item
    //not needed - move to cmode
    void teleport(int n, DynamicEntity *d)     // also used by monsters
    {
        int e = -1, tag = MapEntities::ents[n]->attr[0], beenhere = -1;
        for(;;)
        {
            e = findentity(TELEDEST, e+1);
            if(e==beenhere || e<0) { conoutf(CON_WARN, "no teleport destination for tag %d", tag); return; }
            if(beenhere<0) beenhere = e;
            if(MapEntities::ents[e]->attr[1]==tag)
            {
                d->o = MapEntities::ents[e]->o;
                d->yaw = MapEntities::ents[e]->attr[0];
                d->pitch = 0;
                d->vel = vec(0, 0, 0);//vec(cosf(RAD*(d->yaw-90)), sinf(RAD*(d->yaw-90)), 0);
                entinmap(d);
                updatedynentcache(d);
//                ai::inferwaypoints(d, ents[n]->o, ents[e]->o, 16.f);
                msgsound(S_TELEPORT, d);
                break;
            }
        }
    }
    //not needed - move to cmode
    void trypickup(int n, DynamicEntity *d)
    {
        switch(MapEntities::ents[n]->type)
        {
            default:
                if(d->canpickup(MapEntities::ents[n]->type))
                {
                    addmsg(SV_ITEMPICKUP, "rci", d, n);
                    MapEntities::ents[n]->spawned = false; // even if someone else gets it first
                }
                break;

            case TELEPORT:
            {
                if(d->lastpickup==MapEntities::ents[n]->type && lastmillis-d->lastpickupmillis<500) break;
                d->lastpickup = MapEntities::ents[n]->type;
                d->lastpickupmillis = lastmillis;
                teleport(n, d);
                break;
            }

            case RESPAWNPOINT:
                if(d!=player1) break;
                if(n==respawnent) break;
                respawnent = n;
                conoutf(CON_GAMEINFO, "\f2respawn point set!");
                playsound(S_V_RESPAWNPOINT);
                break;

            case JUMPPAD:
            {
                if(d->lastpickup==MapEntities::ents[n]->type && lastmillis-d->lastpickupmillis<300) break;
                d->lastpickup = MapEntities::ents[n]->type;
                d->lastpickupmillis = lastmillis;
                vec v((int)(char)MapEntities::ents[n]->attr[2]*10.0f, (int)(char)MapEntities::ents[n]->attr[1]*10.0f, MapEntities::ents[n]->attr[0]*12.5f);
                d->timeinair = 0;
//		if(d->ai) d->ai->becareful = true;
                d->falling = vec(0, 0, 0);
//                d->vel = v;
                d->vel.z = 0;
                d->vel.add(v);
                msgsound(S_JUMPPAD, d);
                break;
            }
        }
    }
    //not needed - move to cmode
    void checkitems(DynamicEntity *d)
    {
        if(d->state!=CS_ALIVE) return;
        vec o = d->feetpos();
        loopv(MapEntities::ents)
        {
            extentity &e = *MapEntities::ents[i];
            if(e.type==NOTUSED) continue;
            if(!e.spawned && e.type!=TELEPORT && e.type!=JUMPPAD && e.type!=RESPAWNPOINT) continue;
            float dist = e.o.dist(o);
            if(dist<(e.type==TELEPORT ? 16 : 12)) trypickup(i, d);
        }
    }
    //not needed - move to cmode
    void putitems(packetbuf &p)            // puts items in network stream and also spawns them locally
    {
//        putint(p, SV_ITEMLIST);
//        loopv(ents) if(ents[i]->type>=I_SHELLS && ents[i]->type<=I_QUAD && (!m_noammo || ents[i]->type<I_SHELLS || ents[i]->type>I_CARTRIDGES))
//        {
//            putint(p, i);
//            putint(p, ents[i]->type);
//        }
//        putint(p, -1);
    }
    //not needed - move to cmode
    void resetspawns()
    {
        loopv(MapEntities::ents) MapEntities::ents[i]->spawned = false;
    }
    //not needed - move to cmode
    void spawnitems()
    {
//        if(m_noitems) return;
//        loopv(ents) if(ents[i]->type>=I_SHELLS && ents[i]->type<=I_QUAD && (!m_noammo || ents[i]->type<I_SHELLS || ents[i]->type>I_CARTRIDGES))
//        {
//            ents[i]->spawned = m_sp || (ents[i]->type!=I_QUAD && ents[i]->type!=I_BOOST);
//        }
    }
    //not needed - move to cmode
    void setspawn(int i, bool on)
    {
        if(MapEntities::ents.inrange(i)) MapEntities::ents[i]->spawned = on;
    }

    extentity *newentity() {
    	return new MapEntities::StaticEntity();
    }

    //declared in igame.h
    void deleteentity(extentity *e) { MapEntities::delentity(e); }

    //declared in igame.h
    void clearents() { MapEntities::clearents(); }

    //declared in igame.h
    void fixentity(extentity &e)
    {
        switch(e.type)
        {
            case FLAG:
            case BOX:
            case BARREL:
            case PLATFORM:
            case ELEVATOR:
                e.attr[4] = e.attr[3];
                e.attr[3] = e.attr[2];
                e.attr[2] = e.attr[1];
            case TELEDEST:
                e.attr[1] = e.attr[0];
            case RESPAWNPOINT:
                e.attr[0] = (int)player1->yaw;
                break;
            case WAYPOINT:
        		e.attr[0] = (int)player1->yaw;
        		e.attr[1] = 25.0f;
                break;
        }
    }
    //declared in igame.h
    void entradius(extentity &e, bool &color)
    {
        switch(e.type)
        {
            case DYNLIGHT:
                if(color) glColor3f(e.attr[1]/255.0f, e.attr[2]/255.0f, e.attr[3]/255.0f);
                renderentsphere(e, e.attr[0]);
                break;
            case TELEPORT:
                loopv(MapEntities::ents) if(MapEntities::ents[i]->type == TELEDEST && e.attr[0]==MapEntities::ents[i]->attr[1])
                {
                    renderentarrow(e, vec(MapEntities::ents[i]->o).sub(e.o).normalize(), e.o.dist(MapEntities::ents[i]->o));
                    break;
                }
                break;

            case JUMPPAD:
                renderentarrow(e, vec((int)(char)e.attr[2]*10.0f, (int)(char)e.attr[1]*10.0f, e.attr[0]*12.5f).normalize(), 4);
                break;

            case FLAG:
            case TELEDEST:
            case MAPMODEL:
            case RESPAWNPOINT:
            case BOX:
            case BARREL:
            case PLATFORM:
            case ELEVATOR:
            {
                vec dir;
                vecfromyawpitch(e.attr[0], 0, 1, 0, dir);
                renderentarrow(e, dir, 4);
                break;
            }
            case WAYPOINT:
            {
                renderentsphere(e, e.attr[1]);
                vec dir;
                vecfromyawpitch(e.attr[0], 0, 1, 0, dir);
                renderentarrow(e, dir, e.attr[1]);
                break;
            }
        }
    }
    //declared in igame.h
    bool radiusent(extentity &e)
    {
    	switch(e.type)
    	{
    		case LIGHT:
    		case ENVMAP:
    		case MAPSOUND:
    		case DYNLIGHT:
    		case WAYPOINT:
    			return true;
    			break;
    		default:
    			return false;
    			break;
    	}
    }
    //declared in igame.h
    bool dirent(extentity &e)
    {
    	switch(e.type)
    	{
    		case MAPMODEL:
    		case PLAYERSTART:
    		case SPOTLIGHT:
    		case TELEDEST:
    		case TELEPORT:
    		case JUMPPAD:
    		case BOX:
    		case BARREL:
    		case PLATFORM:
    		case ELEVATOR:
    		case RESPAWNPOINT:
    		case FLAG:
    		case WAYPOINT:
    			return true;
    			break;
    		default:
    			return false;
    			break;
    	}
    }
    //declared in igame.h
    bool printent(extentity &e, char *buf)
    {
        return false;
    }
    //declared in igame.h
    const char *entnameinfo(entity &e) { return ""; }
    //declared in igame.h
    const char *entname(int i)
    {
        static const char *entnames[] =
        {
                "none?", "light", "mapmodel", "playerstart", "envmap", "particles", "sound", "spotlight",
                "shells", "bullets", "rockets", "riflerounds", "grenades", "cartridges",
                "health", "healthboost", "greenarmour", "yellowarmour", "quaddamage",
                "teleport", "teledest",
                "creature", "carrot", "jumppad",
                "banana", "respawnpoint",
                "box", "barrel",
                "platform", "elevator",
                "flag",
                "waypoint", "dynlight", "camera", "", "", "",
        };
        return i>=0 && size_t(i)<sizeof(entnames)/sizeof(entnames[0]) ? entnames[i] : "";
    }

    const int numattrs(int type)
    {
	    static const int num[] =
	    {
		    0, //shells
		    0, //bullets
		    0, //rockets
		    0, //riflerounds
		    0, //grenades
		    0, //cartridges
		    0, //health
		    0, //healthboost
		    0, //greenarmour
		    0, //yellowarmour
		    0, //quaddamage
		    3, //teleport
		    2, //teledest
		    3, //creature
		    1, //carrot
		    3, //jumppad
		    2, //banana/base
		    1, //respawnpoint
		    4, //box
		    4, //barrel
		    4, //platform
		    4, //elevator
		    2, //flag
		    3, //waypoint
		    3, //camera yaw, pitch, distance
		    5, //dynlight
		    4, //camera
		    0, //null x 2
		    0
	    };

	    type -= ET_GAMESPECIFIC;
	    return type >= 0 && size_t(type) < sizeof(num)/sizeof(num[0]) ? num[type] : 0;
    }
    //declared in igame.h
	void renderhelpertext(extentity &e, int &colour, vec &pos, string &tmp)
    {
    	switch(e.type)
    	{
    	case TELEPORT:
    		pos.z += 3.0;
    		formatstring(tmp)("uid: %d\nTeledest Tag: %i\nModel: %s (%i)",
                    MapEntities::getmapuid(e),
    				e.attr[0],
    				mapmodelname(e.attr[1]),
    				e.attr[1]
    		);
    		return;
    	case TELEDEST:
    		pos.z += 3;
    		formatstring(tmp)("uid: %d\nYaw: %i\nTeleport Tag: %i\n",
                    MapEntities::getmapuid(e),
    				e.attr[0],
    				e.attr[1]
    		);
    		return;
    	case JUMPPAD:
    		pos.z += 4.5;
    		formatstring(tmp)("uid: %d\nZ: %i\nY: %i\nX: %i",
                    MapEntities::getmapuid(e),
    				e.attr[0],
    				e.attr[1],
    				e.attr[2]
    		);
    		return;
    	case BASE:
    		pos.z += 3.0f;
    		formatstring(tmp)("uid: %d\nAmmo: %i\nTag: base_%i",
                    MapEntities::getmapuid(e),
    				e.attr[0],
    				e.attr[1]
    		);
    		return;
    	case RESPAWNPOINT:
    		pos.z += 1.5;
    		formatstring(tmp)("Yaw: %i",
    				e.attr[0]
    		);
    		return;
    	case BOX:
    	case BARREL:
    		pos.z += 6.0;
    		formatstring(tmp)("uid: %d\nYaw: %i\nModel: %s (%i)\nWeight: %i\nHealth: %i",
                    MapEntities::getmapuid(e),
    				e.attr[0],
    				mapmodelname(e.attr[1]), e.attr[1],
    				e.attr[2],
    				e.attr[3]
    		);
    		return;
    	case PLATFORM:
    	case ELEVATOR:
    		pos.z += 6.0;
    		formatstring(tmp)("uid: %d\nYaw: %i\nModel: %s (%i)\nTag: %i\nSpeed: %i",
                    MapEntities::getmapuid(e),
    				e.attr[0],
    				mapmodelname(e.attr[1]), e.attr[1],
    				e.attr[2],
    				e.attr[3]
    		);
    		return;
    	case FLAG:
    		pos.z += 3.0f;
    		formatstring(tmp)("uid: %d\nYaw: %i\nTeam: %i",
                    MapEntities::getmapuid(e),
    				e.attr[0],
    				e.attr[1]
    		);
    		return;
    	case DYNLIGHT:
    		pos.z += 9.0;
    		formatstring(tmp)("uid: %d\nRadius %i\n\fs\fRRed: %i\n\fJGreen: %i\n\fDBlue: %i\fr\nTag: %i",
                    MapEntities::getmapuid(e),
    				e.attr[0],
    				e.attr[1],
    				e.attr[2],
    				e.attr[3],
    				e.attr[4]
    		);
    		return;
    	case WAYPOINT:
    		pos.z += 4.5f;
    		formatstring(tmp)("uid: %i\nDirection %i\nRadius %i",
    				MapEntities::getmapuid(e),
    				e.attr[0],
    				e.attr[1]
    		);
    		return;
    	case CAMERA:
    		pos.z += 6.f;
    		formatstring(tmp)("uid: %i\ntag: %i\nyaw: %i\npitch: %i\nroll: %i",
    				MapEntities::getmapuid(e),
    				e.attr[0],
    				e.attr[1],
    				e.attr[2],
    				e.attr[3]
    		);
    		return;
    	}
    }
    //declared in igame.h
    void editent(int i, bool local)
    {
        MapEntities::StaticEntity &e = *MapEntities::ents[i];
        //if(!local) //also when local, we need send to send map entity uids
			addmsg(SV_EDITENT, "riii3ii8", e.uid, i, (int)(e.o.x*DMF), (int)(e.o.y*DMF), (int)(e.o.z*DMF), e.type, e.attr[0], e.attr[1], e.attr[2], e.attr[3], e.attr[4], e.attr[5], e.attr[6], e.attr[7]);
    }
    //declared in igame.h
    float dropheight(entity &e)
    {
        if(e.type==BASE || e.type==FLAG) return 0.0f;
        return 4.0f;
    }
    //declared in igame.h
	int *getmodelattr(extentity &e)
	{
		switch(e.type)
		{
			case TELEPORT:
			case PLATFORM:
			case ELEVATOR:
			case BARREL:
			case BOX:
				return &e.attr[1];
			default:
				return NULL;
		}
	}
    //declared in igame.h
	bool checkmodelusage(extentity &e, int i)
	{
		switch(e.type)
		{
			case TELEPORT:
				if(e.attr[1] == 0)
					return false;
			case PLATFORM:
			case ELEVATOR:
			case BARREL:
			case BOX:
				return (e.attr[1] == i);
			default:
				return false;
		}
	}

	void animatemapmodel(const extentity &e, int &anim, int &basetime)
    {
    	return;
    }
#endif
}
