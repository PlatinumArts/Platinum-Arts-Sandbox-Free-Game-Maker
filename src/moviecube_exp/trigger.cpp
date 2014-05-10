#include "game.h"
#include "extentity.h"

//TODO: handle trigger for mp and sp

using namespace game;

//   enum
//    {
//        TRIG_COLLIDE    = 1<<0,
//        TRIG_TOGGLE     = 1<<1,
//        TRIG_ONCE       = 0<<2,
//        TRIG_MANY       = 1<<2,
//        TRIG_DISAPPEAR  = 1<<3,
//        TRIG_AUTO_RESET = 1<<4,
//        TRIG_RUMBLE     = 1<<5,
//        TRIG_LOCKED     = 1<<6,
//        TRIG_ENDSP      = 1<<7
//    };
//
//    static const int NUMTRIGGERTYPES = 32;
//
//    static const int triggertypes[NUMTRIGGERTYPES] =
//    {
//        0,
//        TRIG_ONCE,                    // 1
//        TRIG_RUMBLE,                  // 2
//        TRIG_TOGGLE,                  // 3
//        TRIG_TOGGLE | TRIG_RUMBLE,    // 4
//        TRIG_MANY,                    // 5
//        TRIG_MANY | TRIG_RUMBLE,      // 6
//        TRIG_MANY | TRIG_TOGGLE,      // 7
//        TRIG_MANY | TRIG_TOGGLE | TRIG_RUMBLE,    // 8
//        TRIG_COLLIDE | TRIG_TOGGLE | TRIG_RUMBLE, // 9
//        TRIG_COLLIDE | TRIG_TOGGLE | TRIG_AUTO_RESET | TRIG_RUMBLE, // 10
//        TRIG_COLLIDE | TRIG_TOGGLE | TRIG_LOCKED | TRIG_RUMBLE,     // 11
//        TRIG_DISAPPEAR,               // 12
//        TRIG_DISAPPEAR | TRIG_RUMBLE, // 13
//        TRIG_DISAPPEAR | TRIG_COLLIDE | TRIG_LOCKED, // 14
//        0 /* reserved 15 */,
//        0 /* reserved 16 */,
//        0 /* reserved 17 */,
//        0 /* reserved 18 */,
//        0 /* reserved 19 */,
//        0 /* reserved 20 */,
//        0 /* reserved 21 */,
//        0 /* reserved 22 */,
//        0 /* reserved 23 */,
//        0 /* reserved 24 */,
//        0 /* reserved 25 */,
//        0 /* reserved 26 */,
//        0 /* reserved 27 */,
//        0 /* reserved 28 */,
//        TRIG_DISAPPEAR | TRIG_RUMBLE | TRIG_ENDSP, // 29
//        0 /* reserved 30 */,
//        0 /* reserved 31 */,
//    };
//
//    #define validtrigger(type) (triggertypes[(type) & (NUMTRIGGERTYPES-1)]!=0)
//    #define checktriggertype(type, flag) (triggertypes[(type) & (NUMTRIGGERTYPES-1)] & (flag))
//
//    static inline void setuptriggerflags(MapEntities::StaticEntity &e)
//    {
//        e.flags = extentity::F_ANIM;
//        if(checktriggertype(e.attr[2], TRIG_COLLIDE|TRIG_DISAPPEAR)) e.flags |= extentity::F_NOSHADOW;
//        if(!checktriggertype(e.attr[2], TRIG_COLLIDE)) e.flags |= extentity::F_NOCOLLIDE;
//        switch(e.triggerstate)
//        {
//            case TRIGGERING:
//                if(checktriggertype(e.attr[2], TRIG_COLLIDE) && lastmillis-e.lasttrigger >= 500) e.flags |= extentity::F_NOCOLLIDE;
//                break;
//            case TRIGGERED:
//                if(checktriggertype(e.attr[2], TRIG_COLLIDE)) e.flags |= extentity::F_NOCOLLIDE;
//                break;
//            case TRIGGER_DISAPPEARED:
//                e.flags |= extentity::F_NOVIS | extentity::F_NOCOLLIDE;
//                break;
//        }
//    }

//    void resettriggers()
//    {
//        loopv(MapEntities::ents)
//        {
//            MapEntities::StaticEntity &e = *MapEntities::ents[i];
//            if(e.type != ET_MAPMODEL || !validtrigger(e.attr[2])) continue;
//            e.triggerstate = TRIGGER_RESET;
//            e.lasttrigger = 0;
//            setuptriggerflags(e);
//        }
//    }
//
//    void unlocktriggers(int tag, int oldstate = TRIGGER_RESET, int newstate = TRIGGERING)
//    {
//        loopv(MapEntities::ents)
//        {
//            MapEntities::StaticEntity &e = *MapEntities::ents[i];
//            if(e.type != ET_MAPMODEL || !validtrigger(e.attr[2])) continue;
//            if(e.attr[3] == tag && e.triggerstate == oldstate && checktriggertype(e.attr[2], TRIG_LOCKED))
//            {
//				if(newstate == TRIGGER_RESETTING && checktriggertype(e.attr[2], TRIG_COLLIDE) && overlapsdynent(e.o, e.attr[4] ? e.attr[4] : 20)) continue;
//                e.triggerstate = newstate;
//                e.lasttrigger = lastmillis;
//                if(checktriggertype(e.attr[2], TRIG_RUMBLE)) playsound(S_RUMBLE, &e.o);
//            }
//        }
//    }
//
//    ICOMMAND(trigger, "ii", (int *tag, int *state),
//    {
//        if(*state) unlocktriggers(*tag);
//        else unlocktriggers(*tag, TRIGGERED, TRIGGER_RESETTING);
//    });
//
//    VAR(triggerstate, -1, 0, 1);
//
//    void doleveltrigger(int trigger, int state)
//    {
//        defformatstring(aliasname)("level_trigger_%d", trigger);
//        if(identexists(aliasname))
//        {
//            triggerstate = state;
//            execute(aliasname);
//        }
//    }
//
//    void checktriggers()
//    {
//        if(player1->state != CS_ALIVE) return;
//        vec o = player1->feetpos();
//        loopv(MapEntities::ents)
//        {
//            MapEntities::StaticEntity &e = *MapEntities::ents[i];
//            if(e.type != ET_MAPMODEL || !validtrigger(e.attr[2])) continue;
//            switch(e.triggerstate)
//            {
//                case TRIGGERING:
//                case TRIGGER_RESETTING:
//                    if(lastmillis-e.lasttrigger>=1000)
//                    {
//                        if(e.attr[3])
//                        {
//                            if(e.triggerstate == TRIGGERING) unlocktriggers(e.attr[3]);
//                            else unlocktriggers(e.attr[3], TRIGGERED, TRIGGER_RESETTING);
//                        }
//                        if(checktriggertype(e.attr[2], TRIG_DISAPPEAR)) e.triggerstate = TRIGGER_DISAPPEARED;
//                        else if(e.triggerstate==TRIGGERING && checktriggertype(e.attr[2], TRIG_TOGGLE)) e.triggerstate = TRIGGERED;
//                        else e.triggerstate = TRIGGER_RESET;
//                    }
//                    setuptriggerflags(e);
//                    break;
//                case TRIGGER_RESET:
//                    if(e.lasttrigger)
//                    {
//						if(checktriggertype(e.attr[2], TRIG_AUTO_RESET|TRIG_MANY|TRIG_LOCKED) && e.o.dist(o)-player1->radius>= (e.attr[4] ? e.attr[4] : (checktriggertype(e.attr[2], TRIG_COLLIDE)  ? 20 : 12)))
//                            e.lasttrigger = 0;
//                        break;
//                    }
//                    else if(e.o.dist(o)-player1->radius>= (e.attr[4] ? e.attr[4] :(checktriggertype(e.attr[2], TRIG_COLLIDE) ? 20 : 12))) break;
//                    else if(checktriggertype(e.attr[2], TRIG_LOCKED))
//                    {
//                        if(!e.attr[3]) break;
//                        doleveltrigger(e.attr[3], -1);
//                        e.lasttrigger = lastmillis;
//                        break;
//                    }
//                    e.triggerstate = TRIGGERING;
//                    e.lasttrigger = lastmillis;
//                    setuptriggerflags(e);
//                    if(checktriggertype(e.attr[2], TRIG_RUMBLE)) playsound(S_RUMBLE, &e.o);
//                    //if(checktriggertype(e.attr[2], TRIG_ENDSP)) endsp(false);
//                    if(e.attr[3]) doleveltrigger(e.attr[3], 1);
//                    break;
//                case TRIGGERED:
//					if(e.o.dist(o)-player1->radius< (e.attr[4] ? e.attr[4] : (checktriggertype(e.attr[2], TRIG_COLLIDE) ? 20 : 12)))
//                    {
//                        if(e.lasttrigger) break;
//                    }
//                    else if(checktriggertype(e.attr[2], TRIG_AUTO_RESET))
//                    {
//                        if(lastmillis-e.lasttrigger<6000) break;
//                    }
//                    else if(checktriggertype(e.attr[2], TRIG_MANY))
//                    {
//                        e.lasttrigger = 0;
//                        break;
//                    }
//                    else break;
//                    if(checktriggertype(e.attr[2], TRIG_COLLIDE) && overlapsdynent(e.o, e.attr[4] ? e.attr[4] : 20)) break;
//                    e.triggerstate = TRIGGER_RESETTING;
//                    e.lasttrigger = lastmillis;
//                    setuptriggerflags(e);
//                    if(checktriggertype(e.attr[2], TRIG_RUMBLE)) playsound(S_RUMBLE, &e.o);
////                    if(checktriggertype(e.attr[2], TRIG_ENDSP)) endsp(false);
//                    if(e.attr[3]) doleveltrigger(e.attr[3], 0);
//                    break;
//            }
//        }
//    }

//    void animatemapmodel(const extentity &e, int &anim, int &basetime)
//    {
//        const MapEntities::StaticEntity &f = (const MapEntities::StaticEntity &)e;
//        if(validtrigger(f.attr[2])) switch(f.triggerstate)
//        {
//            case TRIGGER_RESET: anim = ANIM_TRIGGER|ANIM_START; break;
//            case TRIGGERING: anim = ANIM_TRIGGER; basetime = f.lasttrigger; break;
//            case TRIGGERED: anim = ANIM_TRIGGER|ANIM_END; break;
//            case TRIGGER_RESETTING: anim = ANIM_TRIGGER|ANIM_REVERSE; basetime = f.lasttrigger; break;
//        }
//    }

void teleport(int n, DynamicEntity *d)     // also used by monsters
{
	int e = -1, tag = entities::getents()[n]->attr[0], beenhere = -1;
	for(;;)
	{
		e = findentity(TELEDEST, e+1);
		if(e==beenhere || e<0) { conoutf(CON_WARN, "no teleport destination for tag %d", tag); return; }
		if(beenhere<0) beenhere = e;
		if(entities::getents()[e]->attr[1]==tag)
		{
			d->o = entities::getents()[e]->o;
			d->yaw = entities::getents()[e]->attr[0];
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

void trypickup(int n, DynamicEntity *d)
{
	switch(entities::getents()[n]->type)
	{
		default:
			if(d->canpickup(entities::getents()[n]->type))
			{
				addmsg(SV_ITEMPICKUP, "rci", d, n);
				entities::getents()[n]->spawned = false; // even if someone else gets it first
			}
			break;

		case TELEPORT:
		{
			if(d->lastpickup==entities::getents()[n]->type && lastmillis-d->lastpickupmillis<500) break;
			d->lastpickup = entities::getents()[n]->type;
			d->lastpickupmillis = lastmillis;
			teleport(n, d);
			break;
		}

//            case RESPAWNPOINT:
//                if(d!=player1) break;
//                if(n==respawnent) break;
//                respawnent = n;
//                conoutf(CON_GAMEINFO, "\f2respawn point set!");
//                playsound(S_V_RESPAWNPOINT);
//                break;

		case JUMPPAD:
		{
			if(d->lastpickup==entities::getents()[n]->type && lastmillis-d->lastpickupmillis<300) break;
			d->lastpickup = entities::getents()[n]->type;
			d->lastpickupmillis = lastmillis;
			vec v((int)(char)entities::getents()[n]->attr[2]*10.0f, (int)(char)entities::getents()[n]->attr[1]*10.0f, entities::getents()[n]->attr[0]*12.5f);
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

void checkitems(DynamicEntity *d)
{
	if(d->state!=CS_ALIVE) return;
	vec o = d->feetpos();
	loopv(entities::getents())
	{
		extentity &e = *entities::getents()[i];
		if(e.type==NOTUSED) continue;
		if(!e.spawned && e.type!=TELEPORT && e.type!=JUMPPAD) continue;
		float dist = e.o.dist(o);
		if(dist<(e.type==TELEPORT ? 16 : 12)) trypickup(i, d);
	}
}
//    void resetspawns() { loopv(entities::getents()) entities::getents()[i]->spawned = false; }
//    void spawnitems() {}
//    void setspawn(int i, bool on)
//    {
//        if(entities::getents().inrange(i)) entities::getents()[i]->spawned = on;
//    }
