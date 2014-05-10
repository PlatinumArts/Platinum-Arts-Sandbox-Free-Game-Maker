#include "pzlgame.h"
using namespace entities;

namespace game
{
	string clientmap;
	const char *getclientmap() { return clientmap; }

	bool connected    = false;
	bool intermission = false;

	void gamedisconnect(bool cleanup) {connected = false;}
	void gameconnect   (bool remote)  {connected = true; }
	
	VARP(debug, 0, 0, 1);
	
	SVARFP
	(
		name,
		"unnamed",
		player1->name = newstring(name)
	);
	
	VARFP
	(
		playermodel,
		0, 0, 1,
		player1->playerindex = playermodel
	);

	pzlchar  *player1 = new pzlchar  (); // our player

    pzlportal portals[2];

	pzlchar *spawnstate(pzlchar *d)
	{
		d->respawn();
		return d;
	}

    // PORTALS
    ICOMMAND(portal_y, "", (), {
        portals[PORTAL_YELLOW].calc(player1);
        adddecal(2, portals[PORTAL_YELLOW].pos, vec(1, 1, 1), portals[PORTAL_YELLOW].size, bvec(255, 255, 000));
    });
    ICOMMAND(portal_b, "", (), {
        portals[PORTAL_BLUE].calc(player1);
        adddecal(2, portals[PORTAL_BLUE].pos, vec(1, 1, 1), portals[PORTAL_BLUE].size, bvec(000, 000, 255));
    });

	void spawnplayer(pzlchar *d)
	{
		findplayerspawn(d, -1, 0);
		spawnstate(d);
		d->state = d==player1 && editmode ? CS_EDITING : CS_ALIVE;
	}

	void respawnself()
	{
		if(paused || ispaused()) return;
		
		spawnplayer(player1);
	}

	bool canjump()
	{
		if(player1->state == CS_ALIVE)
			return true;
		return false;
	}
	
	void updateplayer()
	{
		checkitems(player1);
		moveplayer(player1, 10, true);
	}

	void updateworld() //main game update loop
	{
		if(!curtime) return;

		physicsframe();
		updateplayer();

		if(editmode)
			intermission = false;
		else
			disablezoom();
	}

	bool mousemove(int &dx, int &dy, float &cursens)
	{
		return false;
	}

	bool recomputecamera(physent *&camera1, physent &tempcamera, bool &detachedcamera, float &thirdpersondistance)
	{
		if(player1->state == CS_DEAD || editmode || !thirdperson)
			return false;
		return true;
	}

	///engine specific
	void setwindowcaption()
	{
		defformatstring(capt)("SandBox %s: Puzzle Games - %s", version, getclientmap()[0] ? getclientmap() : "[new map]");
		SDL_WM_SetCaption(capt, NULL);
	}

	void startmap(const char *name)
	{
		lastpickupmillis = -30000;
		copystring(clientmap, name ? name : "");
		intermission = false;
		findplayerspawn(player1, -1);

		if(identexists("mapstart"))
			execute("mapstart");
	}

	void edittoggled(bool on)
	{
	}

	void physicstrigger(physent *d, bool local, int floorlevel, int waterlevel, int material)
	{
		if(d->state!=CS_ALIVE||d->type==ENT_INANIMATE) return;
		switch(material)
		{
			case MAT_LAVA:
				if (waterlevel==0) break;
				//playsound(S_BURN, d==player1 ? NULL : &d->o);
				loopi(60)
				{
					vec o = d->o;
					o.z -= d->eyeheight *i/60.0f;
					regular_particle_flame(PART_FLAME, o, 6, 2, 0x903020, 3, 2.0f);
					regular_particle_flame(PART_SMOKE, vec(o.x, o.y, o.z + 8.0f), 6, 2, 0x303020, 1, 4.0f, 100.0f, 2000.0f, -20);
				}
				if(d==player1)
					respawnself();
				break;
			case MAT_WATER:
				if (waterlevel==0) break;
				//playsound(waterlevel > 0 ? S_SPLASH_IN : S_SPLASH_OUT , d==player1 ? NULL : &d->o);
				particle_splash(PART_WATER, 200, 200, d->o, (watercolor.x<<16) | (watercolor.y<<8) | watercolor.z, 0.5);
				break;
			default:
				if (floorlevel==0) break;
				//playsound(floorlevel > 0 ? S_JUMP : S_LAND, local ? NULL : &d->o);
				break;
		}
	}

	void openworld(const char *name, bool fall = false)
	{
		if(!connected)
			localconnect();
		if(name && *name && load_world(name))
			return;
		else
			emptymap(10, true, (name && *name) ? name : NULL);
	}
	ICOMMAND(map, "s", (char *s), openworld(s))

	void initclient()
	{
		if(!player1->name)
			player1->name = name;

		clientmap[0] = 0;
	}

	int numdynents(){ return 1; }

	dynent *iterdynents(int i)
	{
		if(!i)
			return player1;
		i--;
		return NULL;
	}

	void changemap(const char *name) { openworld(name, true); }
	void forceedit(const char *name) { openworld(name); }

	const char *gameident()    { return "pzl"; }
	const char *autoexec()     { return "autoexec.cfg"; }
	const char *savedservers() { return NULL; }
}
