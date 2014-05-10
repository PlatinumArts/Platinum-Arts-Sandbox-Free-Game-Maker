#ifndef __KRSGAME_H__
#define __KRSGAME_H__

/*
 * Puzzle Games By: Dale Weiler (graphitemaster)
 * for use with sandbox and sandbox related projects or mods
*/

#include "cube.h"

#define ENTSPAWNTIME 30000 // 30 seconds

enum
{
	NOTUSED     = ET_EMPTY,      // entity slot not in use in map
	LIGHT       = ET_LIGHT,      // lightsource, attr1 = radius, attr2 = intensity
	MAPMODEL    = ET_MAPMODEL,   // attr1 = angle, attr2 = idx
	PLAYERSTART,                 // attr1 = angle
	ENVMAP      = ET_ENVMAP,     // attr1 = radius
	PARTICLES   = ET_PARTICLES,
	MAPSOUND    = ET_SOUND,
	SPOTLIGHT   = ET_SPOTLIGHT,

	MAXENTTYPES
};

struct pzlentity : extentity{};
struct pzlchar   : dynent
{
	const char *name;
	int playerindex;

	bool attacking;
	editinfo *edit;

	pzlchar() : playerindex(0),  attacking(false)
	{
		respawn();
	}

	void respawn()
	{
		dynent::reset();
		collidetype = COLLIDE_AABB;

		attacking = false;
	}
};

enum pzlportaltype
{
    PORTAL_BLUE,   // in
    PORTAL_YELLOW  // out
};

struct pzlportal
{
    pzlportal() :
        dist(1024),
        size(0030),
        dir (0, 0, 0),
        pos (0, 0, 0),
        o   (0, 0, 0)
    {}

    void calc(pzlchar *d)
    {
        // calculate placement for decal
        dir = vec(d->yaw * RAD, d->pitch * RAD);
        o   = d->o;
        raycubepos(o, vec(dir).normalize(), pos, dist, RAY_CLIPMAT|RAY_ALPHAPOLY);
    }

    float dist; // max distance for portal
    float size; // portal size
    vec   dir;  // direction
    vec   pos;  // position
    vec   o;    // start point
};

namespace entities
{
	extern vector<extentity *> ents;

	extern void renderentities();
	extern void checkitems(pzlchar *d);
	extern void trypickup(int n, pzlchar *d);

	extern int lastpickupmillis;
}

namespace game
{
	extern pzlchar *player1;

	extern void spawnplayer(pzlchar *d);

	extern bool intermission;

	extern int debug;
}

#endif
