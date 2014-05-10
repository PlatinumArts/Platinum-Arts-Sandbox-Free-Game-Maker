#include "game.h"

namespace entities
{
    using namespace game;

    int extraentinfosize() { return 0; }       // size in bytes of what the 2 methods below read/write... so it can be skipped by other games

    void writeent(entity &e, char *buf)   // write any additional data to disk (except for ET_ ents)
    {
    }

    void readent(entity &e, char *buf, int ver)     // read from disk, and init
    {
        if(ver <= 30) switch(e.type)
        {
            case FLAG:
            case MONSTER:
            case TELEDEST:
            case RESPAWNPOINT:
            case BOX:
            case BARREL:
            case PLATFORM:
            case ELEVATOR:
                e.attr[0] = (int(e.attr[0])+180)%360;
                break;
        }
        if(ver <= 31) switch(e.type)
        {
            case BOX:
            case BARREL:
            case PLATFORM:
            case ELEVATOR:
                int yaw = (int(e.attr[0])%360 + 360)%360 + 7;
                e.attr[0] = yaw - yaw%15;
                break;
        }
    }

#ifndef STANDALONE
    vector<extentity *> ents;

    vector<extentity *> &getents() { return ents; }

    bool mayattach(extentity &e) { return false; }
    bool attachent(extentity &e, extentity &a) { return false; }

    VARP(showlighting, 0, 1, 1);
    VARP(showdynlights, 0, 1, 1);

    const char *itemname(int i)
    {
        int t = ents[i]->type;
        if(t<I_SHELLS || t>I_QUAD) return NULL;
        return itemstats[t-I_SHELLS].name;
    }

    int itemicon(int i)
    {
        int t = ents[i]->type;
        if(t<I_SHELLS || t>I_QUAD) return -1;
        return itemstats[t-I_SHELLS].icon;
    }

    const char *entmdlname(int type)
    {
        static const char *entmdlnames[] =
        {
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            "ammo/shells", "ammo/bullets", "ammo/rockets", "ammo/rrounds", "ammo/grenades", "ammo/cartridges",
            "health", "boost", "armor/green", "armor/yellow", "quad", "teleporter",
            NULL, NULL,
            NULL, //"carrot",
            NULL, NULL,
            NULL, // "checkpoint",
            NULL, NULL,
            NULL, NULL,
            NULL, NULL
        };
        return entmdlnames[type];
    }

    const char *entmodel(const entity &e)
    {
        if(e.type == TELEPORT)
        {
            if(e.attr[1] > 0) return mapmodelname(e.attr[1]);
            if(e.attr[1] < 0) return NULL;
        }
        return e.type < MAXENTTYPES ? entmdlname(e.type) : NULL;
    }

    void preloadentities()
    {
        loopi(MAXENTTYPES)
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
        }
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
            const char *mdlname = entmodel(e);
            if(mdlname)
            {
                vec p = e.o;
                p.z += 1+sinf(lastmillis/100.0+e.o.x+e.o.y)/20;
                rendermodel(&e.light, mdlname, ANIM_MAPMODEL|ANIM_LOOP, p, lastmillis/(float)revs, 0, 0, MDL_SHADOW | MDL_CULL_VFC | MDL_CULL_DIST | MDL_CULL_OCCLUDED);
            }
        }
    }

    #define renderfocus(i,f) { extentity &e = *ents[i]; f; }

    void renderentlight(extentity &e)
    {
        vec color(e.attr[1], e.attr[2], e.attr[3]);
        color.div(255.f);
        adddynlight(vec(e.o), float(e.attr[0] != 0 ? e.attr[0] > 0 ? e.attr[0] : 0 : getworldsize() ), color);
    }

    bool dynlightsactive[32767];

    void adddynlights()
    {
        loopv(ents)
        {
            if((ents[i]->type == DYNLIGHT && ents[i]->attr[4] > -1 && dynlightsactive[ents[i]->attr[4]] && (!editmode || showdynlights)) //dynlights
                || ((entgroup.find(i) >= 0 || enthover == i) && (ents[i]->type == LIGHT) && (editmode && showlighting))) //normal lights
            {
                renderfocus(i, renderentlight(e));
            }
        }
    }

    void addammo(int type, int &v, bool local)
    {
        itemstat &is = itemstats[type-I_SHELLS];
        v += is.add;
        if(v>is.max) v = is.max;
        if(local) msgsound(is.sound);
    }

    void repammo(fpsent *d, int type, bool local)
    {
        addammo(type, d->ammo[type-I_SHELLS+GUN_SG], local);
    }

    // these two functions are called when the server acknowledges that you really
    // picked up the item (in multiplayer someone may grab it before you).

    void pickupeffects(int n, fpsent *d)
    {
        if(!ents.inrange(n)) return;
        int type = ents[n]->type;
        if(type<I_SHELLS || type>I_QUAD) return;
        ents[n]->spawned = false;
        if(!d) return;
        itemstat &is = itemstats[type-I_SHELLS];
        if(d!=player1 || isthirdperson())
        {
            //particle_text(d->abovehead(), is.name, PART_TEXT, 2000, 0xFFC864, 4.0f, -8);
            particle_icon(d->abovehead(), is.icon%4, is.icon/4, PART_HUD_ICON_GREY, 2000, 0xFFFFFF, 2.0f, -8);
        }
        playsound(itemstats[type-I_SHELLS].sound, d!=player1 ? &d->o : NULL, NULL, 0, 0, -1, 0, 1500);
        d->pickup(type);
        if(d==player1) switch(type)
        {
            case I_BOOST:
                conoutf(CON_GAMEINFO, "\f2you have a permanent +10 health bonus! (%d)", d->maxhealth);
                playsound(S_V_BOOST, NULL, NULL, 0, 0, -1, 0, 3000);
                break;

            case I_QUAD:
                conoutf(CON_GAMEINFO, "\f2you got the quad!");
                playsound(S_V_QUAD, NULL, NULL, 0, 0, -1, 0, 3000);
                break;
        }
    }

    // these functions are called when the client touches the item

    void teleporteffects(fpsent *d, int tp, int td, bool local)
    {
        if(d == player1) playsound(S_TELEPORT);
        else
        {
            if(ents.inrange(tp)) playsound(S_TELEPORT, &ents[tp]->o);
            if(ents.inrange(td)) playsound(S_TELEPORT, &ents[td]->o);
        }
        if(local && d->clientnum >= 0)
        {
            sendposition(d);
            packetbuf p(32, ENET_PACKET_FLAG_RELIABLE);
            putint(p, N_TELEPORT);
            putint(p, d->clientnum);
            putint(p, tp);
            putint(p, td);
            sendclientpacket(p.finalize(), 0);
            flushclient();
        }
    }

    void jumppadeffects(fpsent *d, int jp, bool local)
    {
        if(d == player1) playsound(S_JUMPPAD);
        else if(ents.inrange(jp)) playsound(S_JUMPPAD, &ents[jp]->o);
        if(local && d->clientnum >= 0)
        {
            sendposition(d);
            packetbuf p(16, ENET_PACKET_FLAG_RELIABLE);
            putint(p, N_JUMPPAD);
            putint(p, d->clientnum);
            putint(p, jp);
            sendclientpacket(p.finalize(), 0);
            flushclient();
        }
    }

    void teleport(int n, fpsent *d)     // also used by monsters
    {
        int e = -1, tag = ents[n]->attr[0], beenhere = -1;
        for(;;)
        {
            e = findentity(TELEDEST, e+1);
            if(e==beenhere || e<0) { conoutf(CON_WARN, "no teleport destination for tag %d", tag); return; }
            if(beenhere<0) beenhere = e;
            if(ents[e]->attr[1]==tag)
            {
                teleporteffects(d, n, e, true);
                d->o = ents[e]->o;
                d->yaw = ents[e]->attr[0];
                if(ents[e]->attr[2] > 0)
                {
                    vec dir;
                    vecfromyawpitch(d->yaw, 0, 1, 0, dir);
                    float speed = d->vel.magnitude2();
                    d->vel.x = dir.x*speed;
                    d->vel.y = dir.y*speed;
                }
                else d->vel = vec(0, 0, 0);
                entinmap(d);
                updatedynentcache(d);
                ai::inferwaypoints(d, ents[n]->o, ents[e]->o, 16.f);
                break;
            }
        }
    }

    void trypickup(int n, fpsent *d)
    {
        switch(ents[n]->type)
        {
            default:
                if(d->canpickup(ents[n]->type))
                {
                    addmsg(N_ITEMPICKUP, "rci", d, n);
                    ents[n]->spawned = false; // even if someone else gets it first
                }
                break;

            case TELEPORT:
            {
                if(d->lastpickup==ents[n]->type && lastmillis-d->lastpickupmillis<500) break;
                if(ents[n]->attr[2] > 0)
                {
                    defformatstring(hookname)("can_teleport_%d", ents[n]->attr[2]);
                    if(identexists(hookname) && !execute(hookname)) break;
                }
                d->lastpickup = ents[n]->type;
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
                if(d->lastpickup==ents[n]->type && lastmillis-d->lastpickupmillis<300) break;
                d->lastpickup = ents[n]->type;
                d->lastpickupmillis = lastmillis;
                jumppadeffects(d, n, true);
                vec v((int)(char)ents[n]->attr[2]*10.0f, (int)(char)ents[n]->attr[1]*10.0f, ents[n]->attr[0]*12.5f);
                d->timeinair = 0;
                if(d->ai) d->ai->becareful = true;
                d->falling = vec(0, 0, 0);
//                d->vel = v;
                d->vel.z = 0;
                d->vel.add(v);
                msgsound(S_JUMPPAD, d);
                break;
            }
        }
    }

    void checkitems(fpsent *d)
    {
        if(d->state!=CS_ALIVE) return;
        vec o = d->feetpos();
        loopv(ents)
        {
            extentity &e = *ents[i];
            if(e.type==NOTUSED) continue;
            if(!e.spawned && e.type!=TELEPORT && e.type!=JUMPPAD && e.type!=RESPAWNPOINT) continue;
            float dist = e.o.dist(o);
            if(dist<(e.type==TELEPORT ? 16 : 12)) trypickup(i, d);
        }
    }

    void checkquad(int time, fpsent *d)
    {
        if(d->quadmillis && (d->quadmillis -= time)<=0)
        {
            d->quadmillis = 0;
            playsound(S_PUPOUT, d==player1 ? NULL : &d->o);
            if(d==player1) conoutf(CON_GAMEINFO, "\f2quad damage is over");
        }
    }

    void putitems(packetbuf &p)            // puts items in network stream and also spawns them locally
    {
        putint(p, N_ITEMLIST);
        loopv(ents) if(ents[i]->type>=I_SHELLS && ents[i]->type<=I_QUAD && (!m_noammo || ents[i]->type<I_SHELLS || ents[i]->type>I_CARTRIDGES))
        {
            putint(p, i);
            putint(p, ents[i]->type);
        }
        putint(p, -1);
    }

    void resetspawns() { loopv(ents) ents[i]->spawned = false; }

    void spawnitems(bool force)
    {
        if(m_noitems) return;
        loopv(ents) if(ents[i]->type>=I_SHELLS && ents[i]->type<=I_QUAD && (!m_noammo || ents[i]->type<I_SHELLS || ents[i]->type>I_CARTRIDGES))
        {
            ents[i]->spawned = force || m_sp || !server::delayspawn(ents[i]->type);
        }
    }

    void setspawn(int i, bool on) { if(ents.inrange(i)) ents[i]->spawned = on; }

    extentity *newentity() { return new fpsentity(); }
    void deleteentity(extentity *e) { delete (fpsentity *)e; }

    void clearents()
    {
        while(ents.length()) deleteentity(ents.pop());
    }

    enum
    {
        TRIG_COLLIDE    = 1<<0,
        TRIG_TOGGLE     = 1<<1,
        TRIG_ONCE       = 0<<2,
        TRIG_MANY       = 1<<2,
        TRIG_DISAPPEAR  = 1<<3,
        TRIG_AUTO_RESET = 1<<4,
        TRIG_RUMBLE     = 1<<5,
        TRIG_LOCKED     = 1<<6,
        TRIG_ENDSP      = 1<<7
    };

    static const int NUMTRIGGERTYPES = 32;

    static const int triggertypes[NUMTRIGGERTYPES] =
    {
        -1,
        TRIG_ONCE,                    // 1
        TRIG_RUMBLE,                  // 2
        TRIG_TOGGLE,                  // 3
        TRIG_TOGGLE | TRIG_RUMBLE,    // 4
        TRIG_MANY,                    // 5
        TRIG_MANY | TRIG_RUMBLE,      // 6
        TRIG_MANY | TRIG_TOGGLE,      // 7
        TRIG_MANY | TRIG_TOGGLE | TRIG_RUMBLE,    // 8
        TRIG_COLLIDE | TRIG_TOGGLE | TRIG_RUMBLE, // 9
        TRIG_COLLIDE | TRIG_TOGGLE | TRIG_AUTO_RESET | TRIG_RUMBLE, // 10
        TRIG_COLLIDE | TRIG_TOGGLE | TRIG_LOCKED | TRIG_RUMBLE,     // 11
        TRIG_DISAPPEAR,               // 12
        TRIG_DISAPPEAR | TRIG_RUMBLE, // 13
        TRIG_DISAPPEAR | TRIG_COLLIDE | TRIG_LOCKED, // 14
        -1 /* reserved 15 */,
        -1 /* reserved 16 */,
        -1 /* reserved 17 */,
        -1 /* reserved 18 */,
        -1 /* reserved 19 */,
        -1 /* reserved 20 */,
        -1 /* reserved 21 */,
        -1 /* reserved 22 */,
        -1 /* reserved 23 */,
        -1 /* reserved 24 */,
        -1 /* reserved 25 */,
        -1 /* reserved 26 */,
        -1 /* reserved 27 */,
        -1 /* reserved 28 */,
        TRIG_DISAPPEAR | TRIG_RUMBLE | TRIG_ENDSP, // 29
        -1 /* reserved 30 */,
        -1 /* reserved 31 */,
    };

    #define validtrigger(type) (triggertypes[(type) & (NUMTRIGGERTYPES-1)]>=0)
    #define checktriggertype(type, flag) (triggertypes[(type) & (NUMTRIGGERTYPES-1)] & (flag))

    static inline void setuptriggerflags(fpsentity &e)
    {
        e.flags = extentity::F_ANIM;
        if(checktriggertype(e.attr[2], TRIG_COLLIDE|TRIG_DISAPPEAR)) e.flags |= extentity::F_NOSHADOW;
        if(!checktriggertype(e.attr[2], TRIG_COLLIDE)) e.flags |= extentity::F_NOCOLLIDE;
        switch(e.triggerstate)
        {
            case TRIGGERING:
                if(checktriggertype(e.attr[2], TRIG_COLLIDE) && lastmillis-e.lasttrigger >= 500) e.flags |= extentity::F_NOCOLLIDE;
                break;
            case TRIGGERED:
                if(checktriggertype(e.attr[2], TRIG_COLLIDE)) e.flags |= extentity::F_NOCOLLIDE;
                break;
            case TRIGGER_DISAPPEARED:
                e.flags |= extentity::F_NOVIS | extentity::F_NOCOLLIDE;
                break;
        }
    }

    void resettriggers()
    {
        loopv(ents)
        {
            fpsentity &e = *(fpsentity *)ents[i];
            if(e.type != ET_MAPMODEL || !validtrigger(e.attr[2])) continue;
            e.triggerstate = TRIGGER_RESET;
            e.lasttrigger = 0;
            setuptriggerflags(e);
        }
    }

    void unlocktriggers(int tag, int oldstate = TRIGGER_RESET, int newstate = TRIGGERING)
    {
        loopv(ents)
        {
            fpsentity &e = *(fpsentity *)ents[i];
            if(e.type != ET_MAPMODEL || !validtrigger(e.attr[2])) continue;
            if(e.attr[3] == tag && e.triggerstate == oldstate && checktriggertype(e.attr[2], TRIG_LOCKED))
            {
                if(newstate == TRIGGER_RESETTING && checktriggertype(e.attr[2], TRIG_COLLIDE) && overlapsdynent(e.o, e.attr[4] ? e.attr[4] : 20)) continue;
                e.triggerstate = newstate;
                e.lasttrigger = lastmillis;
                if(checktriggertype(e.attr[2], TRIG_RUMBLE)) playsound(S_RUMBLE, &e.o);
            }
        }
    }

    ICOMMAND(trigger, "ii", (int *tag, int *state),
    {
        if(*state) unlocktriggers(*tag);
        else unlocktriggers(*tag, TRIGGERED, TRIGGER_RESETTING);
    });

    VAR(triggerstate, -1, 0, 1);

    void doleveltrigger(int trigger, int state)
    {
        defformatstring(aliasname)("level_trigger_%d", trigger);
        if(identexists(aliasname))
        {
            triggerstate = state;
            execute(aliasname);
        }
    }

    void checktriggers()
    {
        if(player1->state != CS_ALIVE) return;
        vec o = player1->feetpos();
        loopv(ents)
        {
            fpsentity &e = *(fpsentity *)ents[i];
            if(e.type != ET_MAPMODEL || !validtrigger(e.attr[2])) continue;
            switch(e.triggerstate)
            {
                case TRIGGERING:
                case TRIGGER_RESETTING:
                    if(lastmillis-e.lasttrigger>=1000)
                    {
                        if(e.attr[3])
                        {
                            if(e.triggerstate == TRIGGERING) unlocktriggers(e.attr[3]);
                            else unlocktriggers(e.attr[3], TRIGGERED, TRIGGER_RESETTING);
                        }
                        if(checktriggertype(e.attr[2], TRIG_DISAPPEAR)) e.triggerstate = TRIGGER_DISAPPEARED;
                        else if(e.triggerstate==TRIGGERING && checktriggertype(e.attr[2], TRIG_TOGGLE)) e.triggerstate = TRIGGERED;
                        else e.triggerstate = TRIGGER_RESET;
                    }
                    setuptriggerflags(e);
                    break;
                case TRIGGER_RESET:
                    if(e.lasttrigger)
                    {
                        if(checktriggertype(e.attr[2], TRIG_AUTO_RESET|TRIG_MANY|TRIG_LOCKED) && e.o.dist(o)-player1->radius>= (e.attr[4] ? e.attr[4] : (checktriggertype(e.attr[2], TRIG_COLLIDE)  ? 20 : 12)))
                            e.lasttrigger = 0;
                        break;
                    }
                    else if(e.o.dist(o)-player1->radius>= (e.attr[4] ? e.attr[4] :(checktriggertype(e.attr[2], TRIG_COLLIDE) ? 20 : 12))) break;
                    else if(checktriggertype(e.attr[2], TRIG_LOCKED))
                    {
                        if(!e.attr[3]) break;
                        doleveltrigger(e.attr[3], -1);
                        e.lasttrigger = lastmillis;
                        break;
                    }
                    e.triggerstate = TRIGGERING;
                    e.lasttrigger = lastmillis;
                    setuptriggerflags(e);
                    if(checktriggertype(e.attr[2], TRIG_RUMBLE)) playsound(S_RUMBLE, &e.o);
                    if(checktriggertype(e.attr[2], TRIG_ENDSP)) endsp(false);
                    if(e.attr[3]) doleveltrigger(e.attr[3], 1);
                    break;
                case TRIGGERED:
                    if(e.o.dist(o)-player1->radius< (e.attr[4] ? e.attr[4] : (checktriggertype(e.attr[2], TRIG_COLLIDE) ? 20 : 12)))
                    {
                        if(e.lasttrigger) break;
                    }
                    else if(checktriggertype(e.attr[2], TRIG_AUTO_RESET))
                    {
                        if(lastmillis-e.lasttrigger<6000) break;
                    }
                    else if(checktriggertype(e.attr[2], TRIG_MANY))
                    {
                        e.lasttrigger = 0;
                        break;
                    }
                    else break;
                    if(checktriggertype(e.attr[2], TRIG_COLLIDE) && overlapsdynent(e.o, e.attr[4] ? e.attr[4] : 20)) break;
                    e.triggerstate = TRIGGER_RESETTING;
                    e.lasttrigger = lastmillis;
                    setuptriggerflags(e);
                    if(checktriggertype(e.attr[2], TRIG_RUMBLE)) playsound(S_RUMBLE, &e.o);
                    if(checktriggertype(e.attr[2], TRIG_ENDSP)) endsp(false);
                    if(e.attr[3]) doleveltrigger(e.attr[3], 0);
                    break;
            }
        }
    }

    void animatemapmodel(const extentity &e, int &anim, int &basetime)
    {
        const fpsentity &f = (const fpsentity &)e;
        if(validtrigger(f.attr[2])) switch(f.triggerstate)
        {
            case TRIGGER_RESET: anim = ANIM_TRIGGER|ANIM_START; break;
            case TRIGGERING: anim = ANIM_TRIGGER; basetime = f.lasttrigger; break;
            case TRIGGERED: anim = ANIM_TRIGGER|ANIM_END; break;
            case TRIGGER_RESETTING: anim = ANIM_TRIGGER|ANIM_REVERSE; basetime = f.lasttrigger; break;
        }
    }

    void fixentity(extentity &e)
    {
        switch(e.type)
        {
            case FLAG:
            case BOX:
            case BARREL:
            case PLATFORM:
            case ELEVATOR:
            case MONSTER:
            case TELEDEST:
            case RESPAWNPOINT:
                e.attr.pop();
                e.attr.insert(0, player1->yaw);
                break;
        }
    }

    void entradius(extentity &e, bool &color)
    {
        switch(e.type)
        {
            case DYNLIGHT:
                if(color) glColor3f(e.attr[1]/255.0f, e.attr[2]/255.0f, e.attr[3]/255.0f);
                renderentsphere(e, e.attr[0]);
                break;
            case TELEPORT:
                loopv(ents) if(ents[i]->type == TELEDEST && e.attr[0]==ents[i]->attr[1])
                {
                    renderentarrow(e, vec(ents[i]->o).sub(e.o).normalize(), e.o.dist(ents[i]->o));
                    break;
                }
                break;

            case JUMPPAD:
                renderentarrow(e, vec((int)(char)e.attr[2]*10.0f, (int)(char)e.attr[1]*10.0f, e.attr[0]*12.5f).normalize(), 4);
                break;

            case FLAG:
            case MONSTER:
            case TELEDEST:
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
            case MAPMODEL:
                if(validtrigger(e.attr[2])) renderentring(e, e.attr[4] ? e.attr[4] : checktriggertype(e.attr[2], TRIG_COLLIDE) ? 20 : 12);
                break;
        }
    }

    bool radiusent(extentity &e)
    {
        switch(e.type)
        {
            case LIGHT:
            case ENVMAP:
            case MAPSOUND:
            case DYNLIGHT:
                return true;
                break;
            default:
                return false;
                break;
        }
    }

    bool dirent(extentity &e)
    {
        switch(e.type)
        {
            case MAPMODEL:
            case PLAYERSTART:
            case SPOTLIGHT:
            case TELEDEST:
            case TELEPORT:
            case MONSTER:
            case JUMPPAD:
            case BOX:
            case BARREL:
            case PLATFORM:
            case ELEVATOR:
            case RESPAWNPOINT:
            case FLAG:
                return true;
                break;
            default:
                return false;
                break;
        }
    }

    bool printent(extentity &e, char *buf)
    {
        return false;
    }

    const char *entnameinfo(entity &e) { return ""; }
    const char *entname(int i)
    {
        static const char *entnames[] =
        {
            "none?", "light", "mapmodel", "playerstart", "envmap", "particles", "sound", "spotlight",
            "shells", "bullets", "rockets", "riflerounds", "grenades", "cartridges",
            "health", "healthboost", "greenarmour", "yellowarmour", "quaddamage",
            "teleport", "teledest",
            "creature", "carrot", "jumppad",
            "base", "respawnpoint",
            "box", "barrel",
            "platform", "elevator",
            "flag",
            "", "", "", "",
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
            3, //teledest
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
            0, //null x 4
            0,
            0,
            0
        };

        type -= ET_GAMESPECIFIC;
        return type >= 0 && size_t(type) < sizeof(num)/sizeof(num[0]) ? num[type] : 0;
    }

    void renderhelpertext(extentity &e, int &colour, vec &pos, string &tmp)
    {
        switch(e.type)
        {
            case TELEPORT:
                pos.z += 3.0;
                formatstring(tmp)("Teledest Tag: %i\nModel: %s (%i)\nTag: %i",
                    e.attr[0],
                    mapmodelname(e.attr[1]),
                    e.attr[1],
                    e.attr[2]
                );
                return;
            case TELEDEST:
                pos.z += 3;
                formatstring(tmp)("Yaw: %i\nTeleport Tag: %i\n",
                    e.attr[0],
                    e.attr[1]
                );
                return;
            case MONSTER:
                pos.z += 4.5;
                formatstring(tmp)("Yaw: %i\nMonster: %s (%i)\nTag: monster_dead_%i",
                    e.attr[0],
                    ((e.attr[1] >= 0 && e.attr[1] < game::NUMMONSTERTYPES) ? game::monstertypes[e.attr[1]].name : "(null)"), e.attr[1],
                    e.attr[2]
                );
                return;
            case JUMPPAD:
                pos.z += 4.5;
                formatstring(tmp)("Z: %i\nY: %i\nX: %i",
                    e.attr[0],
                    e.attr[1],
                    e.attr[2]
                );
                return;
            case BASE:
                pos.z += 3.0f;
                formatstring(tmp)("Ammo: %i\nTag: base_%i",
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
                formatstring(tmp)("Yaw: %i\nModel: %s (%i)\nWeight: %i\nHealth: %i",
                    e.attr[0],
                    mapmodelname(e.attr[1]), e.attr[1],
                    e.attr[2],
                    e.attr[3]
                );
                return;
            case PLATFORM:
            case ELEVATOR:
                pos.z += 6.0;
                formatstring(tmp)("Yaw: %i\nModel: %s (%i)\nTag: %i\nSpeed: %i",
                    e.attr[0],
                    mapmodelname(e.attr[1]), e.attr[1],
                    e.attr[2],
                    e.attr[3]
                );
                return;
            case FLAG:
                pos.z += 3.0f;
                formatstring(tmp)("Yaw: %i\nTeam: %i",
                    e.attr[0],
                    e.attr[1]
                );
                return;
            case DYNLIGHT:
                pos.z += 7.5;
                formatstring(tmp)("Radius %i\n\fs\fRRed: %i\n\fJGreen: %i\n\fDBlue: %i\fr\nTag: %i",
                    e.attr[0],
                    e.attr[1],
                    e.attr[2],
                    e.attr[3],
                    e.attr[4]
                );
                return;
        }
    }

    void editent(int i, bool local)
    {
        extentity &e = *ents[i];
        if(e.type == ET_MAPMODEL && validtrigger(e.attr[2]))
        {
            fpsentity &f = (fpsentity &)e;
            f.triggerstate = TRIGGER_RESET;
            f.lasttrigger = 0;
            setuptriggerflags(f);
        }
        else e.flags = 0;
        if(local) addmsg(N_EDITENT, "rii3i2v", i, (int)(e.o.x*DMF), (int)(e.o.y*DMF), (int)(e.o.z*DMF), e.type, e.attr.length(), e.attr.length(), e.attr.getbuf());
      }

    float dropheight(entity &e)
    {
        if(e.type==BASE || e.type==FLAG) return 0.0f;
        return 4.0f;
    }

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
#endif
}

