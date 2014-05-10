#ifndef __KRSGAME_H__
#define __KRSGAME_H__

/*vehicle simulation By: Dale Weiler (graphitemaster)
*for use with sandbox and sandbox related projects or mods
*/

#include "cube.h"

/*all time definitions
  are represented in
  millisconds */
#define NOSTIME   5000  // half a second
#define FIRETIME  5000  // half a second
#define ACCELTIME 5000  // half a second
#define ENTTIME   30000 // thirty seconds


/// static entity types
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


	PLAYERFINISH, // finish for race mode
	MONEY,        // money pickup ent
	NOS,          // nitrous oxide
	SPIKESTRIP,   // spike strip
	VEHICLE,      // vehicle get in entity
	BOT,          // bot entity
	MAXENTTYPES
};

/// hardcodded sounds
enum
{
	S_JUMP = 0,     S_LAND,
	S_SPLASH_IN,    S_SPLASH_OUT,
	S_KART_RUNNING, S_KART_IDLE,
	S_SMASH1,       S_SMASH2,
	S_BURN,         S_TIREPOP,
	S_RECICVEMONEY, S_USEMONEY,
	S_GAMESTART,    S_GAMEFINISHED,
	S_USENOS,       S_PICKUP
};

/// gear types
enum
{
	GEAR_REVERSE = 0,
	GEAR_FIRST,
	GEAR_SECOND,
	GEAR_THIRD,
	GEAR_FOURTH,
	GEAR_FIFTH
};

///bot types
enum
{
	BOT_VEHICLE = 0,
	BOT_OPPONENT,
	BOT_POLICE,
	BOT_CIVILIAN
};

/// transmission types
enum
{
	TRANS_STANDARD = 0,
	TRANS_AUTOMATIC
};

//gamemodes
enum
{
	MODE_RACE = 0,
	MODE_BUMPER,
	MODE_DRAG,
	MODE_JUMPPING
};

#if 0
static struct gamemodeinfo
{
	const char *name;
	int mode;
	const char *info;
} gamemodes[] =
{
	{ "Track Racing:",     MODE_RACE,     "You and your opponet races around a track, first one to the finish line wins"},
	{ "Bumper Cars:",      MODE_BUMPER,   "Drive around bumping players off the track, last man wins"},
	{ "Drag Racing:",      MODE_DRAG,     "You are givin a straight track, make it to the finish line as quick as possible, the quicker the better" },
	{ "Vehcile Jumpping:", MODE_JUMPPING, "You are givin a track full of jumps, drive around and jump the higher you get the more money you get" }
};
#endif



struct krsentity : extentity{};

//this is compatible with the FPSGAME waypoints
struct waypoint
{
	waypoint *parent;
	vec o;
	vector<ushort> links;
	int score;

	waypoint() {}
	waypoint(const vec &o) : parent(NULL), o(o), score(-1) {}
};

struct vehicle
{
	const char *name, *mdl;
	int maxspeed, price, traction, flags, jumpvel;

	vehicle() : name(NULL), mdl(NULL), maxspeed(0), price(0), traction(0), flags(0), jumpvel(0) {}

	~vehicle()
	{
		if(name) delete[] name;
		if(mdl) delete[] mdl;
	}
};

struct krschar : dynent
{
	const char *name;

	bool attacking;
	bool vehiclechanged;
	bool isinvehicle;

	bool onnos,onfire;
	bool hasflat, hasnos;

	int gear, gears[6];
	int vehicleindex, playerindex;
	int money, traction, damage;

	vector<bool> hasvehicle;

	int lastnosmillis, lastfiremillis, lastaccelmillis;

	editinfo *edit;

	krschar() : attacking(false), vehiclechanged(true), isinvehicle(true),
				onnos(false), onfire(false), hasflat(false), hasnos(false),
				gear(0), vehicleindex(0), playerindex(0), money(200), traction(50), damage(0)
	{
		respawn();
	}

	void respawn()
	{
		dynent::reset();
		collidetype = COLLIDE_AABB;

		attacking = false;
		vehiclechanged = false;

		onnos = false;
		onfire = false;
		hasflat = false;
		hasnos = false;

		gear = GEAR_FIRST;
		traction = 50;
		damage = 0;

		lastnosmillis = 0;
		lastfiremillis = 0;
		lastaccelmillis = 0;
	}
};

struct krsai : krschar
{
	vector<ushort> route;
	int entindex, rank;
	bool hasname;
};

enum
{
	ANIM_BRAKE = ANIM_GAMESPECIFIC,
	NUMANIMS
};

static const char * const animnames[] =
{
	ANIMNAMES,
	"brake"
};

namespace entities
{
	extern vector<extentity *> ents;

	extern void renderentities();
	extern void checkitems(krschar *d);
	extern void trypickup(int n, krschar *d);

	extern int lastpickupmillis;
}

namespace bots
{
	void setupbots();
	void updatebots();
}

namespace ai
{
	extern vector<waypoint> waypoints;
	extern waypoint *closestwaypoint(const vec &o);

	extern void loadwaypoints(const char *name, bool msg = false);
	extern void savewaypoints(const char *name);
	extern void findroute(int from, int to, vector<ushort> &route);
	extern void renderwaypoints();
	extern void trydrop();
	extern void clearwaypoints();
}

namespace game
{
	extern krschar *player1;

	extern vector<krsai *> bots;
	extern vector<vehicle *> vehicles;
	extern vector<const char *> botnames;

	extern void changeplayerstate(krschar *d);
	extern void spawnplayer(krschar *d);
	extern void assignvehicle(krschar *d, vehicle &car);
	extern void updatespeed(krschar *d);
	extern void updategears(krschar *d);
	extern void updateorientation(krschar *d);
	extern void setupbots();

	extern bool intermission;

	extern int debug;
	extern int dirtmap;
	extern int gamemode;

	//krsrender
	extern void rendersmoke(krschar *d);
	extern void renderflame(krschar *d);
	extern void renderplayer(krschar *d);
}

#endif
