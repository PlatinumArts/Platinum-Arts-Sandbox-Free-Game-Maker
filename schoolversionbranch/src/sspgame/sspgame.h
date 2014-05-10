#ifndef __GAME_H__
#define __GAME_H__

#include "cube.h"
// console message types
enum
{
	CON_GAMEINFO = 1<<8, //ie, A to blah, D to blah
	CON_COLLECT = 1<<9, //ie, collecting an item
	CON_KILL = 1<<10 //just when an enemy is defeated :P
};

struct eventimage;
struct box_def;
struct enemy;
struct pickup;
struct platform;
struct sspchar;
struct sspent;
struct sspitem;
struct sspmonster;

namespace entities
{
	extern vector<extentity *> ents;
	extern vector<sspitem *> items;
	extern vector<platform *> platforms;
	extern void prepareents();
	extern void trypickup(int n, sspchar *d);
	extern void renderentities();
	extern void checkitems(sspchar *d);
	extern bool pickuppowerup(sspchar *d, sspitem *i);
}

namespace game
{
	extern sspchar *player1;
	extern vector<sspent *> sspobjs;
	extern vector<eventimage *> eventimages;
	extern int secsallowed, secsremain;
	extern bool intermission;
	extern vector<pickup *> pickups;
	extern vector<box_def *> boxdefs;
	extern int closestyaw(int yaw);
	extern void clearprojs();
	extern void explode(const vec &o, int radius, int damage = 1, const sspent *owner = NULL);
	extern void clearprojs(sspent *d);
	extern void updateprojs();
	extern void renderprojs();
	extern void initialisemonsters();
	extern void checkmonsteritems(sspmonster *d);

	extern int debug;
	extern int mousetarget;

	extern vector<enemy *> monstertypes;

	extern void writemonsters(stream *f);
	extern void writeprojectiles(stream *f);
}

enum                            // static entity types
{
	NOTUSED = ET_EMPTY,         // entity slot not in use in map
	LIGHT = ET_LIGHT,           // lightsource, attr1 = radius, attr2 = intensity
	MAPMODEL = ET_MAPMODEL,     // attr1 = angle, attr2 = idx
	PLAYERSTART,                // attr1 = angle, attr2 = team
	ENVMAP = ET_ENVMAP,         // attr1 = radius
	PARTICLES = ET_PARTICLES,
	MAPSOUND = ET_SOUND,
	SPOTLIGHT = ET_SPOTLIGHT,
	BOX, //attr1 = yaw, attr2 = mdl, attr3 = index, attr4 = flags
	PICKUP, //attr1 = item, attr2 = script  as defined in script
	ENEMY, //attr1 = yaw, attr2 = enemy, as defined is a script, attr3 = first waypoint
	WAYPOINT, //attr1 = number, attr2 and attr3 is linked waypoints, as long as they're above 0, an enemy will head to that specific waypoint
	TELEPORT, //attr1 = teledest, attr2 = radius (0 = 16), attr3 = model
	TELEDEST, //attr1 = yaw, attr2 = from
	CHECKPOINT, //attr1 = yaw, attr2 = radius, attr3 = model, attr4 = type
	JUMPPAD, //attr1 = Z, attr2 = Y, attr3 = X, attr4 = radius
	PLATFORM, //attr1 = yaw, attr2 = model, attr3 = route, attr4 = speed
	PLATFORMROUTE, //attr1 = route, attr2 = linkedent1, attr3 = linkedent2
	CAMERA, //attr1 = number, attr2 = yaw, attr3 = pitch, attr4 = distance
	AXIS, //attr1 == 0 == manual, else automatic oneway switch, attr2 = axis script, ie camera change //switches the player's yaw
	MAXENTTYPES
};

enum
{
	PICKUP_COIN = 0,
	PICKUP_HEALTH,
	PICKUP_TIME,
	PICKUP_LIVES,
	PICKUP_WEAPON,
	PICKUP_ARMOUR
};

enum //armour : if armour or a weapon is equipped, lose it instead of health
{
	ARM_NONE = 0,
	ARM_PLAIN, //no special abbilities;
	ARM_ATTRACT, //quadruples pickup range of well.. pickups
	ARM_FLY, //the player can jump repeatedly in the air for extra height
	ARM_SPIKE, //kills whatever lands on the player, at the cost of the armour
	ARM_DJUMP,
	ARM_MAX
};

enum
{
	CP_SAVE = 0,
	CP_END = 1,
	CP_MAX
};

//object structs, mdl is representation model

struct pickup
{
	const char *mdl;
	const int type;

	pickup(const char *m, int t) : mdl(newstring(m)), type (t) {}
	virtual ~pickup()
	{
		delete[] mdl;
	}
};

struct pickup_generic : pickup //hp, time, coins and lives
{
	int amount;

	pickup_generic(const char *m, int type) : pickup(m, type), amount(1) {}
	~pickup_generic() {}
};

struct pickup_armour : pickup
{
	const char *attachmdl;
	int armour; //the armour type
	const char *tex;
	Texture *icon;
	int iconcolour;

	pickup_armour(const char *m) : pickup(m, PICKUP_ARMOUR), attachmdl(NULL), armour(ARM_PLAIN), tex(newstring("data/ssp/hud/shield")), icon(textureload(tex, 3)), iconcolour(0xFFFFFF) {}
	~pickup_armour()
	{
		delete[] attachmdl;
		delete[] tex;
	}
};

struct pickup_weapon : pickup
{
	const char *attachmdl;
	int projectile;
	int sound;
	int cooldown;

	pickup_weapon(const char *m) : pickup(m, PICKUP_WEAPON), attachmdl(NULL), projectile(0), sound(0), cooldown(0) {}
	~pickup_weapon()
	{
		delete[] attachmdl;
	}
};

struct sspentity : extentity {}; //extend with additional properties if needed

enum
{
	S_JUMP = 0, S_LAND, S_RIFLE, S_TELEPORT, S_SPLASH1, S_SPLASH2, S_CG,
	S_RLFIRE, S_RUMBLE, S_JUMPPAD, S_WEAPLOAD, S_ITEMAMMO, S_ITEMHEALTH,
	S_ITEMARMOUR, S_ITEMPUP, S_ITEMSPAWN,  S_NOAMMO, S_PUPOUT,
	S_PAIN,
	S_DIE,
	S_FLAUNCH, S_FEXPLODE,
	S_SG, S_PUNCH1,
	S_GRUNT1, S_GRUNT2, S_RLHIT,
	S_PAINO,
	S_PAINR, S_DEATHR,
	S_PAINE, S_DEATHE,
	S_PAINS, S_DEATHS,
	S_PAINB, S_DEATHB,
	S_PAINP, S_PIGGR2,
	S_PAINH, S_DEATHH,
	S_PAIND, S_DEATHD,
	S_PIGR1, S_ICEBALL, S_SLIMEBALL, S_PISTOL,

	S_V_BASECAP, S_V_BASELOST,
	S_V_FIGHT,
	S_V_BOOST, S_V_BOOST10,
	S_V_QUAD, S_V_QUAD10,
	S_V_RESPAWNPOINT,

	S_FLAGPICKUP,
	S_FLAGDROP,
	S_FLAGRETURN,
	S_FLAGSCORE,
	S_FLAGRESET,

	S_BURN,
	S_CHAINSAW_ATTACK,
	S_CHAINSAW_IDLE,

	S_HIT
};

enum
{
	ENT_CHAR = 0,
	ENT_PICKUP,
	ENT_BOX,
	ENT_ENEMY
};

struct sspent : dynent
{
	const int etype;
	sspent(int t) : etype(t) {}
	virtual ~sspent() {}

	virtual void respawn()
	{
		dynent::reset();
	}

	virtual bool update() {return true;} //only return false when it's okay to delete
	virtual void takedamage(int amount, bool immunity = true) {}
	virtual void render() {}
	virtual bool stool() {return false;} // aka, is someone standing/sitting on the entity? :D
};

struct sspchar : sspent
{
	int health, maxhealth;
	int armour, armourvec;
	int powerupmillis;
	int gunselect, shootmillis; //gunselect, weapon vector(i);

	int weight;                         // affects the effectiveness of hitpush
	int lastpain;
	int lastaction;
	bool attacking;
	int lastpickupmillis;
	int powerup;
	editinfo *edit;
	int lives, coins, checkpoint;

	sspchar(int t = ENT_CHAR) : sspent(t), maxhealth(3), weight(100), lastpain(0), edit(NULL), lives(5), coins(0) { respawn(); }

	void respawn()
	{
		dynent::reset();
		maxspeed = 80;
		collidetype = COLLIDE_AABB;

		lastaction = 0;
		attacking = false;
		lastpickupmillis = 0;
		powerup = -1;
		health = 1;
		armour = 0;
		armourvec = -1;
		powerupmillis = 0;
		gunselect = -1;
		shootmillis = 0;
	}
	bool update();
	void takedamage(int amount, bool immunity = true);
	void render();
	bool stool();
};

struct enemy //enemy declaration
{
	const char *mdl;
	int health, speed, painsound, diesound;

	enemy(const char *_mdl, int _hlt, int _spd, int _ps, int _ds) {
		mdl = newstring(_mdl[0] ? _mdl : "rc/red");
		health = _hlt ? _hlt : 3;
		speed = _spd ? _spd : 80;
		painsound = _ps ? _ps : S_PAIN;
		diesound = _ds ? _ds : S_DIE;
	}
	~enemy() {DELETEA(mdl);}
};

struct sspmonster : sspchar
{
	const int mtype;

	sspmonster(extentity &e, enemy &d) : sspchar(ENT_ENEMY), mtype(e.attr[1])
	{
		type = ENT_AI;
		collidetype = COLLIDE_AABB;

		health = maxhealth = d.health;
		maxspeed = d.speed;
		setbbfrommodel(this, d.mdl);

		o = newpos = e.o; yaw = e.attr[0];
		o.z += eyeheight;
		respawn();
	}
	~sspmonster() {}

	void respawn()
	{
		dynent::reset();
		sspent::respawn();
	}

	bool update();
	void takedamage(int amount, bool immunity = true);
	void render();
};

struct platform : dynent
{
	extentity *dest;
	int model;
	int speed;

	void update();
	void render();

	platform(int m) : dest(NULL), model(m)
	{
		physent::type = ENT_INANIMATE;
		setbbfrommodel(this, mapmodelname(model));
	}
	~platform() {}
};

enum
{
	BOX_PERSIST = 0, //stays after clearing
	BOX_DESTROY = 1, //explodes after being cleared
	BOX_PINJATA = 2, //whole inventory is ejected when hit
	BOX_EXPLODE = 4, //explode when BOX_DESTROY is true, damaging all nearby creatures after a small delay
	BOX_ALL     = 7
};

struct box_def
{
	vector<int> inv;

	box_def() {}
	~box_def() {}
};

struct sspbox : sspent
{
	vector<int> inventory;
	int mdl;
	int flags;
	int script;

	int explode, lasttouch; //explode is non zero when touched and last item is rejected

	sspbox() : sspent(ENT_BOX)
	{
		explode = lasttouch = 0;
	}
	~sspbox() {}

	void init(box_def &b, extentity &e)
	{
		loopv(b.inv)
			inventory.add(b.inv[i]);

		mdl = e.attr[1];
		flags = e.attr[3];
		script = e.attr[4];
		o = e.o;
		yaw = e.attr[0];


		collidetype = COLLIDE_OBB;
		setbbfrommodel(this, mapmodelname(e.attr[1]));
		if(eyeheight <= 0)
		{
			conoutf("WARNING: negative eyeheight for %s, expect odd behaviour", mapmodelname(e.attr[1]));
			eyeheight = xradius;
		}

		o.z += eyeheight;
	}
	void trigger();
	bool update();
	void takedamage(int amount, bool immunity = false);
	void render();
};

struct sspitem
{
	entitylight light;
	vec o, vel;
	int index;
	int script;
	bool fixed; //allow movement
	int deathtime; //if not 0, the pickup dissapears once set time is reached

	sspitem() {}
	~sspitem() {};

	void init(vec pos, int i, int s, bool f = true, int dt = 0)
	{
		o = pos;
		vel = vec(0, 0, 0);
		index = i;
		script = s;
		fixed = f;
		deathtime = dt;
	}
	bool update();
	void render();
};

struct proj //ectiles
{
	const char *mdl;
	int damage, radius, force, travelsound, speed;

	//decal properties, set via /setprojdecal
	int didx, drad;
	bvec dcol;

	proj(const char *_mdl, int _dmg, int _rad, int _frce, int _ts, int _spd) {
		mdl = newstring(_mdl[0] ? _mdl : "banana");
		damage = _dmg ? _dmg : 1;
		radius = _rad ? _rad : 1;
		force = _frce ? _frce : 1;
		travelsound = _ts ? _ts : 1;
		speed = _spd;

		didx = DECAL_BURN,dcol.x = dcol.y = dcol.z = 0xFF, drad = 50;
	}
	~proj() {DELETEA(mdl);}
};

struct projectile //the actual flying ones
{
	const int prj;
	vec o, d;
	sspent *owner;

	projectile(int _prj, vec _o, float _yaw, float _pitch, sspent *_owner = NULL) : prj(_prj), o(_o), owner(_owner)
	{
		vecfromyawpitch(_yaw, _pitch, 1, 0, d);
		vec p = d;
		p.mul(owner ? owner->radius * 2 : 4);

		o.add(p);
	}
	~projectile() {}
};

struct eventimage
{
	Texture *tex;
	int deltime;
	int starttime;

	eventimage(const char *_tex, int _dt) : starttime(lastmillis)
	{
		tex = textureload(*_tex ? _tex : "data/sandboxlogo", 3);
		deltime = lastmillis + (_dt ? _dt : 2000);
	}
	~eventimage() {}
};

enum
{
	ANIM_POWERUP = ANIM_GAMESPECIFIC,
	ANIM_ATTACK, ANIM_LOSE, ANIM_WIN,
	NUMANIMS
};

static const char * const animnames[] =
{
    ANIMNAMES,
	"powerup", "attack", "lose", "win"
};

#endif
