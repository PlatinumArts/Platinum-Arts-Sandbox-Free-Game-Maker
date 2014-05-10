#include "game.h"

///TODO: stackmovable (486)
///TODO: physicstrigger
namespace game
{
    int maptime = 0, maprealtime = 0, minremain = 0;
    int respawnent = -1;
    int lastspawnattempt = 0;

    int following = -1, followdir = 0;

    DynamicEntity *player1 = NULL;			// our client
    vector<DynamicEntity *> players;		// other clients
    vector<Control*> controls;			//our controls
    vector<PlayerModelInfo*> playermodels;

    bool clientoption(const char *arg) { return false; }

    void adddynlights() { MapEntities::adddynlights(); }

	void follow(char *arg)
    {
        if(arg[0] ? player1->state==CS_SPECTATOR : following>=0)
        {
	    following = arg[0] ? parseplayer(arg) : -1;
            if(following==player1->clientnum) following = -1;
            followdir = 0;
            conoutf("follow %s", following>=0 ? "on" : "off");
        }
	}
	COMMAND(follow, "s");

    void nextfollow(int dir)
    {
        if(player1->state!=CS_SPECTATOR || clients.empty())
        {
            stopfollowing();
            return;
        }
        int cur = following >= 0 ? following : (dir < 0 ? clients.length() - 1 : 0);
        loopv(clients)
        {
            cur = (cur + dir + clients.length()) % clients.length();
            if(clients[cur] && clients[cur]->state!=CS_SPECTATOR)
            {
                if(following<0) conoutf("follow on");
                following = cur;
                followdir = dir;
                return;
            }
        }
        stopfollowing();
    }
    ICOMMAND(nextfollow, "i", (int *dir), nextfollow(*dir < 0 ? -1 : 1));

    const char *getclientmap() { return clientmap; }

    void resetgamestate()
    {
//        if(m_classicsp)
//        {
		clearmovables();
//            resettriggers();
//        }
    }

    DynamicEntity *spawnstate(DynamicEntity *d)              // reset player state not persistent accross spawns
    {
        d->respawn();
        d->spawnstate(gamemode);
        return d;
    }

    void respawnself()
    {
        if(paused || ispaused()) return;
        if(m_mp(gamemode))
        {
//            if(player1->respawned!=player1->lifesequence)
//              {
//                addmsg(SV_TRYSPAWN, "rc", player1);
//                player1->respawned = player1->lifesequence;
//            }
        }
        else
        {
            spawnplayer(player1);
//	    if (deathscore) showscores(false);
//            lasthit = 0;
	    if(cmode) cmode->respawned(player1);
        }
    }

    bool intersect(dynent *d, const vec &from, const vec &to)   // if lineseg hits entity bounding box
    {
        float dist;
        vec bottom(d->o), top(d->o);
        bottom.z -= d->eyeheight;
        top.z += d->aboveeye;
        return linecylinderintersect(from, to, bottom, top, d->radius, dist);
    }

    dynent *intersectclosest(const vec &from, const vec &to, DynamicEntity *at)
    {
        dynent *best = NULL;
        float bestdist = 1e16f;
        loopi(numdynents())
        {
            dynent *o = iterdynents(i);
            if(o==at || o->state!=CS_ALIVE) continue;
            if(!intersect(o, from, to)) continue;
            float dist = at->o.dist(o->o);
            if(dist<bestdist)
            {
                best = o;
                bestdist = dist;
            }
        }
        return best;
    }

    DynamicEntity *pointatplayer()
    {
        loopv(players) if(players[i] != player1 && intersect(players[i], player1->o, worldpos)) return players[i];
        return NULL;
    }

    void stopfollowing()
    {
        if(following<0) return;
        following = -1;
        followdir = 0;
        conoutf("follow off");
    }

    DynamicEntity *followingplayer()
    {
        if(player1->state!=CS_SPECTATOR || following<0) return NULL;
        DynamicEntity *target = getclient(following);
        if(target && target->state!=CS_SPECTATOR) return target;
        return NULL;
    }

    DynamicEntity *hudplayer()
    {
        if(thirdperson) return player1;
        DynamicEntity *target = followingplayer();
        return target ? target : player1;
    }

    void setupcamera()
    {
        DynamicEntity *target = followingplayer();
        if(target)
        {
            player1->yaw = target->yaw;
            player1->pitch = target->state==CS_DEAD ? 0 : target->pitch;
            player1->o = target->o;
            player1->resetinterp();
        }
    }

    bool detachcamera()
    {
        DynamicEntity *d = hudplayer();
        return d->state==CS_DEAD;
    }

    bool collidecamera()
    {
        switch(player1->state)
        {
            case CS_EDITING: return false;
            case CS_SPECTATOR: return followingplayer()!=NULL;
        }
        return true;
    }

    VARP(smoothmove, 0, 75, 100);
    VARP(smoothdist, 0, 32, 64);

    void predictplayer(DynamicEntity *d, bool move)
    {
        d->o = d->newpos;
        d->yaw = d->newyaw;
        d->pitch = d->newpitch;
        if(move)
        {
            moveplayer(d, 1, false);
            d->newpos = d->o;
        }
        float k = 1.0f - float(lastmillis - d->smoothmillis)/smoothmove;
        if(k>0)
        {
            d->o.add(vec(d->deltapos).mul(k));
            d->yaw += d->deltayaw*k;
            if(d->yaw<0) d->yaw += 360;
            else if(d->yaw>=360) d->yaw -= 360;
            d->pitch += d->deltapitch*k;
        }
    }

    //TODO: check if local or not - remove slowmosp, add new mode
    VARFP(slowmosp, 0, 0, 1,
    {
        if(m_sp && !slowmosp) setvar("gamespeed", 100);
    });

//    void checkslowmo()
//    {
//        static int lastslowmohealth = 0;
//        setvar("gamespeed", intermission ? 100 : clamp(player1->health, 25, 200), true, false);
//        if(player1->health<player1->maxhealth && lastmillis-max(maptime, lastslowmohealth)>player1->health*player1->health/2)
//        {
//            lastslowmohealth = lastmillis;
//            player1->health++;
//        }
//    }

    void updatecontrols()
    {
    	//check events, update ...
    	loopv(controls)
    	{
    		controls[i]->update();
    	}
    }

    void updateworld()        // main game update loop
    {
        if(!maptime) { maptime = lastmillis; maprealtime = totalmillis; return; }
        if(!curtime) { gets2c(); if(player1->clientnum>=0) c2sinfo(); return; }

        physicsframe();
//	    ai::trydropwaypoints();
//      otherplayers(curtime);
//	    ai::update();
        moveragdolls();

        gets2c();
        updatemovables(curtime);
        updatecontrols();
		CutScene::update();
        if(player1->clientnum>=0) c2sinfo();   // do this last, to reduce the effective frame lag
    }

    void spawnplayer(DynamicEntity *d)   // place at random spawn
    {
        if(cmode) cmode->pickspawn(d);
        else findplayerspawn(d, d==player1 && respawnent>=0 ? respawnent : -1);
        setbbfrommodel(d, getplayermodelinfo(d).name); // Protagoras: bounding box support
        spawnstate(d);
        if(d==player1)
        {
            if(editmode) d->state = CS_EDITING;
            else if(d->state != CS_SPECTATOR) d->state = CS_ALIVE;
        }
        else d->state = CS_ALIVE;
    }

    VARP(spawnwait, 0, 0, 1000);

    void respawn()
    {
        if(player1->state==CS_DEAD)
        {
            int wait = cmode ? cmode->respawnwait(player1) : 0;
            if(wait>0)
            {
                lastspawnattempt = lastmillis;
                //conoutf(CON_GAMEINFO, "\f2you must wait %d second%s before respawn!", wait, wait!=1 ? "s" : "");
                return;
            }
//            if(lastmillis < player1->lastpain + spawnwait) return;
            if(m_dmsp) { changemap(clientmap, gamemode); return; }    // if we die in SP we try the same map again
	    respawnself();
        }
    }

    bool canjump()
    {
//        if(!intermission) respawn();
        respawn();
//        return player1->state!=CS_DEAD && !intermission;
        return player1->state!=CS_DEAD;
    }

    bool allowmove(physent *d)
    {
        if(d->type!=ENT_PLAYER) return true;
//        return !((DynamicEntity *)d)->lasttaunt || lastmillis-((DynamicEntity *)d)->lasttaunt>=1000;
    	return true;
    }

    void timeupdate(int timeremain)
    {
        minremain = timeremain;
        if(!timeremain)
        {
//            intermission = true;
//            player1->attacking = false;
	    if(cmode) cmode->gameover();
            conoutf(CON_GAMEINFO, "\f2intermission:");
            conoutf(CON_GAMEINFO, "\f2game has ended!");
//            showscores(true);
            disablezoom();
        }
        else if(timeremain > 0)
        {
            conoutf(CON_GAMEINFO, "\f2time remaining: %d %s", timeremain, timeremain==1 ? "minute" : "minutes");
        }
    }

    vector<DynamicEntity *> clients;

    DynamicEntity *newclient(int cn)   // ensure valid entity
    {
//    	conoutf("DEBUG: game::newclient %d", cn);
        if(cn < 0 || cn > max(0xFF, MAXCLIENTS + MAXBOTS))
        {
            neterr("clientnum", false);
            return NULL;
        }

        if(cn == player1->clientnum) return player1;

        while(cn >= clients.length()) clients.add(NULL);
        if(!clients[cn])
        {
            DynamicEntity *d = new DynamicEntity;
//          conoutf("DEBUG: newclient DynamicEntity %d", cn);
            d->clientnum = cn;
            clients[cn] = d;
            players.add(d);
        }
        return clients[cn];
    }

    DynamicEntity *getclient(int cn)   // ensure valid entity
    {
        if(cn == player1->clientnum) return player1;
        return clients.inrange(cn) ? clients[cn] : NULL;
    }

    void clientdisconnected(int cn, bool notify)
    {
//      conoutf("DEBUG: game::clientdisconnected cn: %d", cn);
        if(!clients.inrange(cn)) return;
        if(following==cn)
        {
            if(followdir) nextfollow(followdir);
            else stopfollowing();
        }
        DynamicEntity *d = clients[cn];
        if(!d) return;
        if(notify && d->name[0]) conoutf("player %s disconnected", colorname(d));
        removetrackedparticles(d);
        removetrackeddynlights(d);
        if(cmode) cmode->removeplayer(d);
        players.removeobj(d);
        DELETEP(clients[cn]);
        cleardynentcache();
    }

    void clearclients(bool notify)
    {
        loopv(clients) if(clients[i]) clientdisconnected(i, notify);
    }

    void initclient()
    {
        player1 = spawnstate(new DynamicEntity);
        players.add(player1);

        Control* cp = controls.add(new ControlPlayer);
        cp->init(player1);

    }

    VARP(showmodeinfo, 0, 1, 1);

    void setwindowcaption()
    {
        defformatstring(capt)("Sandbox %s: %s - %s", version, server::modename(gamemode, "unknown"), getclientmap()[0] ? getclientmap() : "[new map]");
        SDL_WM_SetCaption(capt, NULL);
    }

    void startgame()
    {
//    	conoutf("DEBUG: cl startgame modename %d", gamemode);

        clearmovables();
        clearragdolls();
        setclientmode();
		clearactions();

//        intermission = false;
        maptime = 0;

        if(cmode)
        {
            cmode->preload();
            cmode->setup();
        }

        conoutf(CON_GAMEINFO, "\f2game mode is %s", server::modename(gamemode));

        if(m_sp)
        {
            defformatstring(scorename)("bestscore_%s", getclientmap());
            const char *best = getalias(scorename);
            if(*best) conoutf(CON_GAMEINFO, "\f2try to beat your best score so far: %s", best);
        }
        else
        {
            const char *info = m_valid(gamemode) ? gamemodes[gamemode - STARTGAMEMODE].info : NULL;
            if(showmodeinfo && info) conoutf(CON_GAMEINFO, "\f0%s", info);
        }

//        if(player1->playermodel != getclientmodel(player)) switchplayermodel(getplayermodel());

//        showscores(false);
        disablezoom();
//        lasthit = 0;

        if(identexists("mapstart")) execute("mapstart");
    }

    void startmap(const char *name)   // called just after a map load
    {
//        conoutf("DEBUG: game::startmap");
//        ai::savewaypoints();
//        ai::clearwaypoints(true);

        MapEntities::initdynlights();

        respawnent = -1; // so we don't respawn at an old spot
        if(!m_mp(gamemode)) spawnplayer(player1);
        else findplayerspawn(player1, -1);
        entities::resetspawns();
        copystring(clientmap, name ? name : "");

        sendmapinfo();
    }

    const char *getmapinfo()
    {
        return showmodeinfo && m_valid(gamemode) ? gamemodes[gamemode - STARTGAMEMODE].info : NULL;
    }

    void physicstrigger(physent *d, bool local, int floorlevel, int waterlevel, int material)
    {
        if(d->state!=CS_ALIVE||d->type==ENT_INANIMATE) return;
        switch(material)
        {
            case MAT_LAVA:
                if (waterlevel==0) break;
                playsound(S_BURN, d==player1 ? NULL : &d->o);
                loopi(60)
		{
			vec o = d->o;
			o.z -= d->eyeheight *i/60.0f;
//			regularflame(PART_FLAME, o, 6, 2, 0x903020, 3, 2.0f);
//			regularflame(PART_SMOKE, vec(o.x, o.y, o.z + 8.0f), 6, 2, 0x303020, 1, 4.0f, 100.0f, 2000.0f, -20);
		}
                break;
            case MAT_WATER:
                if (waterlevel==0) break;
                playsound(waterlevel > 0 ? S_SPLASH1 : S_SPLASH2 , d==player1 ? NULL : &d->o);
                particle_splash(PART_WATER, 200, 200, d->o, (watercolor.x<<16) | (watercolor.y<<8) | watercolor.z, 0.5);
                break;
            default:
                if (floorlevel==0) break;
                playsound(floorlevel > 0 ? S_JUMP : S_LAND, local ? NULL : &d->o);
                break;
        }
    }

    void dynentcollide(physent *d, physent *o, const vec &dir)
    {
        switch(d->type)
        {
            //case ENT_AI: if(dir.z > 0) stackmonster((monster *)d, o); break;
            //case ENT_INANIMATE: if(dir.z > 0) stackmovable((movable *)d, o); break;
        }
    }

    void bounced(physent *d, const vec &surface) {}

    void msgsound(int n, physent *d)
    {
        if(!d || d==player1)
        {
            addmsg(SV_SOUND, "ci", d, n);
            playsound(n);
        }
        else
        {
//            if(d->type==ENT_PLAYER && ((DynamicEntity *)d)->ai)
        	if(d->type==ENT_PLAYER) //TODO: check bot
                addmsg(SV_SOUND, "ci", d, n);
            playsound(n, &d->o);
        }
    }

    int numdynents() { return players.length()+movables.length(); }

    dynent *iterdynents(int i)
    {
        if(i<players.length()) return players[i];
        i -= players.length();
        if(i<movables.length()) return (dynent *)movables[i];
        return NULL;
    }

    bool duplicatename(DynamicEntity *d, const char *name = NULL)
    {
        if(!name) name = d->name;
        loopv(players) if(d!=players[i] && !strcmp(name, players[i]->name)) return true;
        return false;
    }

    const char *colorname(DynamicEntity *d, const char *name, const char *prefix)
    {
        if(!name) name = d->name;
        if(name[0] && !duplicatename(d, name) && d->controltype == CONTROL_PLAYER) return name;
        static string cname[3];
        static int cidx = 0;
        cidx = (cidx+1)%3;
        formatstring(cname[cidx])(d->controltype == CONTROL_PLAYER ? "%s%s \fs\f5(%d)\fr" : "%s%s \fs\f5[%d]\fr", prefix, name, d->clientnum);
        return cname[cidx];
    }

    void suicide(physent *d)
    {
//        if(d==player1 || (d->type==ENT_PLAYER && ((DynamicEntity *)d)->ai))
//    	if(d==player1)
//        {
//            if(d->state!=CS_ALIVE) return;
//            DynamicEntity *pl = (DynamicEntity *)d;
//            if(!m_mp(gamemode)) killed(pl, pl);
//            else if(pl->suicided!=pl->lifesequence)
//            {
//                addmsg(SV_SUICIDE, "rc", pl);
//                pl->suicided = pl->lifesequence;
//            }
//        }
//	else if(d->type==ENT_AI) suicidemonster((monster *)d);
//        if(d->type==ENT_INANIMATE) suicidemovable((movable *)d);
    }
//    ICOMMAND(nap, "", (), suicide(player1));

    void drawicon(int icon, float x, float y, float sz)
    {
        settexture("packages/hud/items.png");
        glBegin(GL_QUADS);
        float tsz = 0.25f, tx = tsz*(icon%4), ty = tsz*(icon/4);
        glTexCoord2f(tx,     ty);     glVertex2f(x,    y);
        glTexCoord2f(tx+tsz, ty);     glVertex2f(x+sz, y);
        glTexCoord2f(tx+tsz, ty+tsz); glVertex2f(x+sz, y+sz);
        glTexCoord2f(tx,     ty+tsz); glVertex2f(x,    y+sz);
        glEnd();
    }

    float abovegameplayhud(int w, int h)
    {
        switch(hudplayer()->state)
        {
            case CS_EDITING:
            case CS_SPECTATOR:
                return 1;
            default:
                return 1650.0f/1800.0f;
        }
    }

    void drawhudicons(DynamicEntity *d)
    {
    }

    void gameplayhud(int w, int h)
    {
        glPushMatrix();
        glScalef(h/1800.0f, h/1800.0f, 1);

        if(player1->state==CS_SPECTATOR)
        {
            int pw, ph, tw, th, fw, fh;
            text_bounds("  ", pw, ph);
            text_bounds("SPECTATOR", tw, th);
            th = max(th, ph);
            DynamicEntity *f = followingplayer();
            text_bounds(f ? colorname(f) : " ", fw, fh);
            fh = max(fh, ph);
            draw_text("SPECTATOR", w*1800/h - tw - pw, 1650 - th - fh);
            if(f) draw_text(colorname(f), w*1800/h - fw - pw, 1650 - fh);
        }

        /*DynamicEntity *d = hudplayer();
        if(d->state!=CS_EDITING)
        {
            if(d->state!=CS_SPECTATOR) drawhudicons(d);
            if(cmode) cmode->drawhud(d, w, h);
        }*/

        glPopMatrix();
    }

    int clipconsole(int w, int h)
    {
        if(cmode) return cmode->clipconsole(w, h);
        return 0;
    }

//    VARP(teamcrosshair, 0, 1, 1);
    VARP(hitcrosshair, 0, 425, 1000);

    const char *defaultcrosshair(int index)
    {
        switch(index)
        {
	    case 3: return "packages/crosshairs/edit.png";
	    case 2: return "packages/crosshairs/hit.png";
	    case 1: return "packages/crosshairs/teammate.png";
	    default: return "packages/crosshairs/default.png";
        }
    }

    int selectcrosshair(float &r, float &g, float &b)
    {
        DynamicEntity *d = hudplayer();
	if(editmode) { r = g = 0.5f; b = 1; return 3;
	}
        if(d->state==CS_SPECTATOR || d->state==CS_DEAD) return -1;

        if(d->state!=CS_ALIVE) return 0;

        int crosshair = 0;
//        if(lasthit && lastmillis - lasthit < hitcrosshair) crosshair = 2;
//
//        if(crosshair!=1 && !m_insta)
//        {
//            if(d->health<=25) { r = 1.0f; g = b = 0; }
//            else if(d->health<=50) { r = 1.0f; g = 0.5f; b = 0; }
//        }
//        if(d->gunwait) { r *= 0.5f; g *= 0.5f; b *= 0.5f; }

        return crosshair;
    }

    void lighteffects(dynent *e, vec &color, vec &dir)
    {
#if 0
        DynamicEntity *d = (DynamicEntity *)e;
        if(d->state!=CS_DEAD && d->quadmillis)
        {
            float t = 0.5f + 0.5f*sinf(2*M_PI*lastmillis/1000.0f);
            color.y = color.y*(1-t) + t;
        }
#endif
    }

    void particletrack(physent *owner, vec &o, vec &d)
    {
          if(owner->type!=ENT_PLAYER && owner->type!=ENT_AI) return;
         DynamicEntity *pl = (DynamicEntity *)owner;
//         if(pl->muzzle.x < 0 || pl->lastattackgun != pl->gunselect) return;
         if(pl->muzzle.x < 0) return;
          float dist = o.dist(d);
         o = pl->muzzle;
         if(dist <= 0) d = o;
         else
         {
             vecfromyawpitch(owner->yaw, owner->pitch, 1, 0, d);
             float newdist = raycube(owner->o, d, dist, RAY_CLIPMAT|RAY_ALPHAPOLY);
             d.mul(min(newdist, dist)).add(owner->o);
         }
    }

        void dynlighttrack(physent *owner, vec &o, vec &hud)
    {
        if(owner->type!=ENT_PLAYER && owner->type!=ENT_AI) return;
        DynamicEntity *pl = (DynamicEntity *)owner;
//        if(pl->muzzle.x < 0 || pl->lastattackgun != pl->gunselect) return;
        o = pl->muzzle;
	hud = owner == hudplayer() ? vec(pl->o).add(vec(0, 0, 2)) : pl->muzzle;
    }

    bool showenthelpers()
    {
    	if(m_edit || editmode) return true;
    	return false;
    }

    bool allowdoublejump(physent *d) {return false;}

	bool mousemove(int &dx, int &dy, float &cursens) {return false;}

    void updatecamera() {}

        //NOTE FIXME TODO @offtools, get rid of the camera hack and use this to update the camera and do cutscenes and... stuff
//    bool recomputecamera(physent *&camera1, physent &tempcamera, bool &detachedcamera, float &thirdpersondistance)
//    {
//    	return false;
//    }

    bool needminimap()
    {
    	return false;
    }

    void writemapdata(stream *f) {}

    // any data written into this vector will get saved with the map data. Must take care to do own versioning, and endianess if applicable. Will not get called when loading maps from other games, so provide defaults.
    void writegamedata(vector<char> &extras) {}
    void readgamedata(vector<char> &extras) {}

    const char *defaultconfig() { return "data/defaults.cfg"; }
    const char *autoexec() { return "autoexec.cfg"; }
    const char *savedservers() { return "servers.cfg"; }
    const char *loadimage() { return "data/moviecube-logo"; }
    void loadconfigs()
    {
        execfile("auth.cfg", false);
    }

    void g3d_gamemenus() {}

    VARP(allowdownload, 0, 0, 1); //offtools: download stuff if server supports it

    //offtools: scriptprefix
    SVARP(scriptprefix, "data/script/");

    //__offtools__: fallback on missing texture
    void texturefailed(char *file, int slot)
    {
        if (multiplayer(false) && m_edit)// && allowdownload)
        {
            conoutf("debug: file requested by texture command doesnt exists: %s", file);
            addmsg(SV_TEXTUREREQUEST, "rsi", file, slot);
        }
    }

    //__offtools__: fallback on missing mapmodel
    void mmodelfailed(const char *name, int idx)
    {
        conoutf("mmodel failed %s : %d", name, idx);

        if (multiplayer(false) && m_edit)// && allowdownload)
        {
            conoutf("[debug]: dummy - download requested mmodel \"%s\" from server", name);
        }
    }
    //__offtools__: fallback on missing map
    void mapfailed(const char *name)
    {
        conoutf("map failed %s", name);

        if (multiplayer(false) && m_edit)// && allowdownload)
        {
            conoutf("[debug]: dummy - download requested map \"%s\" from server", name);
        }
    }

	void listplayermodels()
	{
		vector<char> buf;
		string lst;
		loopv(playermodels)
		{
			formatstring(lst)("%s", playermodels[i]->name);
			buf.put(lst, strlen(lst));
			if(i < (playermodels.length()-1)) buf.add(' ');
		}
		buf.add('\0');
		result(buf.getbuf());
	}
	ICOMMAND(listplayermodels, "", (), listplayermodels());
}
