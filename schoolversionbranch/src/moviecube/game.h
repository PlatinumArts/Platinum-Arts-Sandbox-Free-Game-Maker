#ifndef __GAME_H__
#define __GAME_H__

///TODO: preload playermodel in menu
///TODO: dont register animations twice


#include "cube.h"

#define DEBUG_CAMERA  (true)

#ifndef STANDALONE
extern int dedicated;
#endif

// console message types
enum
{
    CON_CHAT       = 1<<8,
    CON_TEAMCHAT   = 1<<9,
    CON_GAMEINFO   = 1<<10,
    CON_FRAG_SELF  = 1<<11,
    CON_FRAG_OTHER = 1<<12
};

enum
{
	CHAT_TALK = 1<<0,
	CHAT_EMOTE = 1<<1,
	CHAT_TEAM = 1<<2,
};

// network quantization scale
#define DMF 16.0f                // for world locations
#define DNF 100.0f              // for normalized vectors
#define DVELF 1.0f              // for playerspeed based velocity vectors

#define MAXNAMELEN 15
#define MAXTEAMLEN 4

#define SGRAYS 20
#define SGSPREAD 4
#define RL_DAMRAD 40
#define RL_SELFDAMDIV 2
#define RL_DISTSCALE 1.5f

#define MAXBOTS 32

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
    I_SHELLS, I_BULLETS, I_ROCKETS, I_ROUNDS, I_GRENADES, I_CARTRIDGES,
    I_HEALTH, I_BOOST,
    I_GREENARMOUR, I_YELLOWARMOUR,
    I_QUAD,
    TELEPORT,                   // attr1 = idx
    TELEDEST,                   // attr1 = angle, attr2 = idx
    MONSTER,                    // attr1 = angle, attr2 = monstertype
    CARROT,                     // attr1 = tag, attr2 = type
    JUMPPAD,                    // attr1 = zpush, attr2 = ypush, attr3 = xpush
    BASE,
    RESPAWNPOINT,
    BOX,                        // attr1 = angle, attr2 = idx, attr3 = weight
    BARREL,                     // attr1 = angle, attr2 = idx, attr3 = weight, attr4 = health
    PLATFORM,                   // attr1 = angle, attr2 = idx, attr3 = tag, attr4 = speed
    ELEVATOR,                   // attr1 = angle, attr2 = idx, attr3 = tag, attr4 = speed
    FLAG,                       // attr1 = angle, attr2 = team
    WAYPOINT,
    DYNLIGHT,
    CAMERA,
    MAXENTTYPES
};

enum { MM_AUTH = -1, MM_OPEN = 0, MM_VETO, MM_LOCKED, MM_PRIVATE, MM_PASSWORD, MM_START = MM_AUTH };

static const char * const mastermodenames[] = { "auth", "open", "veto", "locked", "private", "password" };

// hardcoded sounds, defined in sounds.cfg
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

// network messages codes, c2s, c2c, s2c

enum { PRIV_NONE = 0, PRIV_MASTER, PRIV_ADMIN };

enum
{
    SV_CONNECT = 0, SV_SERVINFO, SV_WELCOME, SV_INITCLIENT, SV_POS, SV_TEXT, SV_SOUND, SV_CDIS,
    SV_SHOOT, SV_EXPLODE, SV_SUICIDE,
    SV_TRYSPAWN, SV_SPAWNSTATE, SV_SPAWN, SV_FORCEDEATH,
    SV_MAPCHANGE, SV_MAPVOTE, SV_ITEMSPAWN, SV_ITEMPICKUP, SV_ITEMACC,
    SV_PING, SV_PONG, SV_CLIENTPING,
    SV_TIMEUP, SV_MAPRELOAD, SV_FORCEINTERMISSION,
    SV_SERVMSG, SV_ITEMLIST, SV_RESUME,
    SV_EDITMODE, SV_EDITENT, SV_EDITF, SV_EDITT, SV_EDITM, SV_FLIP, SV_COPY, SV_PASTE, SV_ROTATE, SV_REPLACE, SV_DELCUBE, SV_REMIP, SV_NEWMAP, SV_GETMAP, SV_SENDMAP, SV_SENDCFG, SV_UPLOADMAP, SV_UPLOADCFG, SV_SENDTEXTURE, SV_TEXTUREREQUEST, SV_EDITVAR,
    SV_MASTERMODE, SV_KICK, SV_CLEARBANS, SV_CURRENTMASTER, SV_SPECTATOR, SV_SETMASTER, SV_SETTEAM,
    SV_BASES, SV_BASEINFO, SV_BASESCORE, SV_REPAMMO, SV_BASEREGEN, SV_ANNOUNCE,
    SV_LISTDEMOS, SV_SENDDEMOLIST, SV_GETDEMO, SV_SENDDEMO,
    SV_DEMOPLAYBACK, SV_RECORDDEMO, SV_STOPDEMO, SV_CLEARDEMOS,
    SV_TAKEFLAG, SV_RETURNFLAG, SV_RESETFLAG, SV_INVISFLAG, SV_TRYDROPFLAG, SV_DROPFLAG, SV_SCOREFLAG, SV_INITFLAGS,
    SV_CLIENT,
    SV_AUTHTRY, SV_AUTHCHAL, SV_AUTHANS, SV_REQAUTH,
    SV_PAUSEGAME,
    SV_REQUESTBOT, SV_DELETEBOT, SV_INITBOT, SV_FROMBOT, SV_BOTLIMIT, SV_BOTBALANCE,
    SV_MAPCRC, SV_CHECKMAPS,
    SV_SWITCHNAME, SV_SWITCHMODEL, SV_SWITCHTEAM,
    SV_ATTACH, SV_DETACH, SV_MAPENTITIES, SV_MAPUID,
    NUMSV
};

static const int msgsizes[] =               // size inclusive message token, 0 for variable or not-checked sizes
{
    SV_CONNECT, 0, SV_SERVINFO, 5, SV_WELCOME, 2, SV_INITCLIENT, 0, SV_POS, 0, SV_TEXT, 0, SV_SOUND, 2, SV_CDIS, 2,
    SV_SHOOT, 0, SV_EXPLODE, 0, SV_SUICIDE, 1,
    SV_TRYSPAWN, 1, SV_SPAWNSTATE, 2, SV_SPAWN, 1, SV_FORCEDEATH, 2,
    SV_MAPCHANGE, 0, SV_MAPVOTE, 0, SV_ITEMSPAWN, 2, SV_ITEMPICKUP, 2, SV_ITEMACC, 3,
    SV_PING, 2, SV_PONG, 2, SV_CLIENTPING, 2,
    SV_TIMEUP, 2, SV_MAPRELOAD, 1, SV_FORCEINTERMISSION, 1,
    SV_SERVMSG, 0, SV_ITEMLIST, 0, SV_RESUME, 0,
    SV_EDITMODE, 2, SV_EDITENT, 15, SV_EDITF, 16, SV_EDITT, 16, SV_EDITM, 16, SV_FLIP, 14, SV_COPY, 14, SV_PASTE, 14, SV_ROTATE, 15, SV_REPLACE, 16, SV_DELCUBE, 14, SV_REMIP, 1, SV_NEWMAP, 2, SV_GETMAP, 1, SV_SENDMAP, 0,  SV_SENDCFG, 0, SV_UPLOADMAP, 0, SV_UPLOADCFG, 0, SV_SENDTEXTURE, 0, SV_TEXTUREREQUEST, 0, SV_EDITVAR, 0,
    SV_MASTERMODE, 2, SV_KICK, 2, SV_CLEARBANS, 1, SV_CURRENTMASTER, 3, SV_SPECTATOR, 3, SV_SETMASTER, 0, SV_SETTEAM, 0,
    SV_BASES, 0, SV_BASEINFO, 0, SV_BASESCORE, 0, SV_REPAMMO, 1, SV_BASEREGEN, 6, SV_ANNOUNCE, 2,
    SV_LISTDEMOS, 1, SV_SENDDEMOLIST, 0, SV_GETDEMO, 2, SV_SENDDEMO, 0,
    SV_DEMOPLAYBACK, 3, SV_RECORDDEMO, 2, SV_STOPDEMO, 1, SV_CLEARDEMOS, 2,
    SV_TAKEFLAG, 2, SV_RETURNFLAG, 3, SV_RESETFLAG, 4, SV_INVISFLAG, 3, SV_TRYDROPFLAG, 1, SV_DROPFLAG, 6, SV_SCOREFLAG, 6, SV_INITFLAGS, 6,
    SV_CLIENT, 0,
    SV_AUTHTRY, 0, SV_AUTHCHAL, 0, SV_AUTHANS, 0, SV_REQAUTH, 0,
    SV_PAUSEGAME, 2,
    SV_REQUESTBOT, 1, SV_DELETEBOT, 2, SV_INITBOT, 5, SV_FROMBOT, 2, SV_BOTLIMIT, 2, SV_BOTBALANCE, 2,
    SV_MAPCRC, 0, SV_CHECKMAPS, 1,
    SV_SWITCHNAME, 0, SV_SWITCHMODEL, 2, SV_SWITCHTEAM, 0,
    SV_ATTACH, 0, SV_DETACH, 0, SV_MAPENTITIES, 0, SV_MAPUID, 3,
    -1
};

#define MOVIECUBE_LANINFO_PORT 28784
#define MOVIECUBE_SERVER_PORT 28785
#define MOVIECUBE_SERVINFO_PORT 28786
#define MOVIECUBE_MASTER_PORT 28787
#define PROTOCOL_VERSION 5            // bump when protocol changes
#define DEMO_VERSION 1                  // bump when demo format changes
#define DEMO_MAGIC "MOVIECUBE_DEMO"

struct demoheader
{
    char magic[16];
    int version, protocol;
};

//offtools: texture/modeltransfer
//idea for a cachefile
//cache entry: checksum (something unique, uuid, md5 or crc)
//struct cache
//{
//	typedef string checksum;
//	typedef vector<checksum> entries;
//
//	hashtable<string, entries> lookuptable;
//
//	void init();
//	void read();
//	void write();
//
//	char* search(string filename, string checksum)
//	{
//		//pseudo code:
//		//entries = lookuptable.access(filename)
//		//loop entries do:
//		//	if getchecksum(cname) == checksum
//		//		found cacheentry return
//		//end: not found return error
//		return NULL;
//	}
//};

//offtools: server side map data, for uploading and receiving mapdata and cfg's
enum {
	MD_NONE       = 0,
	MD_GOTMAP    = 1<<0,
	MD_GOTCFG    = 1<<1
};
struct mapdata
{
	int cn; //uploader (server side only, use -1 on client)
	string prefix;
	string mname; //tmp map name
	int status;
	stream *map;
	stream *cfg;

	mapdata() : map(NULL), cfg(NULL)
	{
		cn = -1;
		status = MD_NONE;
		mname[0] = prefix[0] = '\0';
	}
	~mapdata() { reset(); }

	void init(int _cn, string _mname)
	{
		if( status != MD_NONE ) return;
		cn = _cn;
		copystring(mname, _mname);
	}

	void reset()
	{
		status = MD_NONE;
		if (map) {delete map; map = NULL;}
		if (cfg) {delete cfg; cfg = NULL;}
	}

	bool check(int _cn, string _mname) {
		if(cn != _cn) return false;
		if(strcmp(mname, _mname) != 0) return false;
		return true;
	}

	void clear()
	{
		reset();

		defformatstring(fname)("packages/base/getmap_%s.ogz", mname);
		remove(findfile(fname, "rb"));

		formatstring(fname)("packages/base/getmap_%s-art.cfg", mname);
		remove(findfile(fname, "rb"));
	}

	bool finished()
	{
		return status == (MD_GOTMAP | MD_GOTCFG);
	}
};

/////////////////////////////////////////////////////////////
//            various Character definitions
/////////////////////////////////////////////////////////////

//__offtools__: following animations could be overwritten
//TODO: attack and gesture are same at the moment, divide these two cases
enum
{
    AO_DEAD = 0,
    AO_DYING,   //1
    AO_IDLE,    //2
    AO_FORWARD, //3
    AO_BACKWARD,//4
    AO_LEFT,    //5
    AO_RIGHT,   //6
    AO_HOLD,    //7
    AO_ACTION,  //8
    AO_GESTURE, //9
    AO_PAIN,    //10
    AO_JUMP,    //11
    AO_SINK,    //12
    AO_SWIM,    //13
    AO_EDIT,    //14
    AO_LAG,     //15
    AO_NUM      //16
};

//__offtools__:
//information about animations send by loaded model
struct PlayerModelInfo
{
	struct animationinfo
	{
		int index;
		string descr;

		animationinfo(char* s, int num) : index(num)
		{
			strcpy(descr, s);
		}
	};

	char* name;
    bool ragdoll, selectable;
    vector<animationinfo *> animinfo;

    PlayerModelInfo(const char* _name) : name(newstring(_name)), ragdoll(true), selectable(true) {}

	void addanimation(char *anim, int num)
	{
		loopv(animinfo) if(num == animinfo[i]->index) //#offtools: just a workaround will fix this on lower level in a later version
		{
			return;
		}
		animinfo.add(new animationinfo(anim, num));
	}

	int getanimindex(char* descr)
	{
		loopv(animinfo) if(strcmp(animinfo[i]->descr, descr) == 0)
		{
			return animinfo[i]->index;
		}
		return -1;
	}

	char* getanimdescr(int index)
	{
		loopv(animinfo) if(animinfo[i]->index == index)
		{
			return animinfo[i]->descr;
		}
		return NULL;
	}
};

/////////////////////////////////////////////////////////
// __offtools__: attachments (clothes ...)
// documentation:
// following tags should appear in the model config file
/////////////////////////////////////////////////////////

enum { ATTACH_HEAD = 0, ATTACH_NECK,
	ATTACH_LEFTSHOULDER, ATTACH_RIGHTSHOULDER,
	ATTACH_LEFTARM, ATTACH_RIGHTARM,
	ATTACH_LEFTFOREARM, ATTACH_RIGHTFOREARM,
	ATTACH_LEFTHAND, ATTACH_RIGHTHAND,
	ATTACH_BELLY, ATTACH_HIP,
	ATTACH_LEFTTHIGH, ATTACH_RIGHTTHIGH,
	ATTACH_LEFTSHIN, ATTACH_RIGHTSHIN,
	ATTACH_LEFTFOOT, ATTACH_RIGHTFOOT,
	ATTACH_PARTS
};

static const struct attachinfo { const char* part; const char* tag; } body[ATTACH_PARTS] =
{
		{ "Head", "tag_head" },
		{ "Neck", "tag_neck" },
		{ "LeftShoulder", "tag_lshoulder" },
		{ "RightShoulder", "tag_rshoulder" },
		{ "LeftArm", "tag_larm" },
		{ "RightArm", "tag_rarm" },
		{ "LeftForearm", "tag_lforearm" },
		{ "RightForearm", "tag_rforearm" },
		{ "LeftHand", "tag_lhand" },
		{ "RightHand", "tag_rhand" },
		{ "Belly", "tag_belly" },
		{ "Hip", "tag_hip" },
		{ "LeftThigh", "tag_lthigh" },
		{ "RightThigh", "tag_rthigh" },
		{ "LeftShin", "tag_lshin" },
		{ "RightShin", "tag_rshin" },
		{ "LeftFoot", "tag_lfoot" },
		{ "RightFoot", "tag_rfoot" }
};

const char* attachtag(int i);
int attachnumbypart(char* part);

///////////////////////////////////////////////////////////
// offtools:
// animation handling of the attachments (not implemented)
//
// handle animation:
// ATTACH_ANIM_NONE - static, no animation
// ATTACH_ANIM_IDLE - only use idle animation
// ATTACH_ANIM_FULL - use current animation of model
//
// ao (animation overwrite, e.g. holding a flag)
// not implemeted
///////////////////////////////////////////////////////////

enum { ATTACH_ANIM_NONE = 0, ATTACH_ANIM_IDLE, ATTACH_ANIM_FULL, ATTACH_ANIM_NUMRULES };

///////////////////////////////////////////////////////////
// __offtools__:
// attachlist - manages the player and entity attachments
///////////////////////////////////////////////////////////
struct AttachedItems
{
	struct attachment
	{
		int part;
		int animrule;
		int ao;
		string model;

		attachment() : part(-1), animrule(ATTACH_ANIM_NONE), ao(-1)
		{
			model[0] = 0;
		}
	};

	vector<attachment*> items;

	bool hasattachment(int part)
    {
    	loopv(items)
    	{
    		if(items[i]->part == part)
    			return true;
    	}
    	return false;
    }

	int numattachments()
	{
		return items.length();
	}

    const char* gettag(int i) {
    	if(i < ATTACH_PARTS || i >= 0)
    		return body[i].tag;
    	else
    		return NULL;
    }

	void add(int part, char* model, int animrule)
	{
    	if (part < 0 || part > ATTACH_PARTS) return;

    	if (hasattachment(part)) remove(part);

    	attachment* a = items.add(new attachment);
    	if(animrule > ATTACH_ANIM_NONE || animrule <= ATTACH_ANIM_NUMRULES) a->animrule = animrule;
    	copystring(a->model, model);
    	a->part = part;
	}

    void remove(int part)
    {
    	loopv(items)
    	{
    		if(items[i]->part == part) {
    			items.remove(i);
    			return;
    		}
    	}
    }
};

struct CharacterInfo
{
    int playermodel;		//playermodel
    AttachedItems attached; //attached item models
	int ao[AO_NUM];			//animation overwrites
	bool dogesture;			//gesture Key
//    Clothes clothes;

	CharacterInfo();
	~CharacterInfo();

	void setplayermodel(int i);

	int getplayermodel();

	bool setactivegesture(int i);

    bool setao(int ao, int i);

	void resetao();
};

enum { CONTROL_PLAYER = 0, CONTROL_CHARACTER, CONTROL_REMOTE, CONTROL_AI, CONTROL_MAX };
enum { CLIENT_PLAYER = 0, CLIENT_BOT };

struct dynentstate
{
    int clienttype;

    dynentstate() : clienttype(CLIENT_PLAYER) {} //TODO: change this to NONE on implementing ControlPlayer

    bool canpickup(int type) { return false; }

    void pickup(int type) {}

    void respawn() {}

    void spawnstate(int gamemode)
    {
//        if(m_demo) {}
    }
};

struct DynamicEntity : dynent, dynentstate
{
    int weight;  // affects the effectiveness of hitpush
    int clientnum, privilege, lastupdate, plag, ping;
    int lastaction;
    int basetime;
    int lastpickup, lastpickupmillis;
    editinfo *edit;
    float deltayaw, deltapitch, newyaw, newpitch;
    int smoothmillis;
    string name, info;
    int ownernum;
    int controltype;
    vec muzzle;
	CharacterInfo charinfo;

    DynamicEntity() : weight(100), clientnum(-1), privilege(PRIV_NONE), lastupdate(0), plag(0), ping(0), basetime(0), edit(NULL), smoothmillis(-1), ownernum(-1), controltype(CONTROL_PLAYER), muzzle(-1, -1, -1)
    {
        name[0] = info[0] = 0;
        respawn();
    }
    ~DynamicEntity()
    {
//        conoutf("DEBUG: DynamicEntity::~DynamicEntity cn: %d", clientnum);
        freeeditinfo(edit);
    }

	CharacterInfo& getcharinfo() { return charinfo; }

    void respawn()
    {
//        conoutf("DEBUG: DynamicEntity::respawn cn: %d", clientnum);
        dynent::reset();
        dynentstate::respawn();
        lastaction = 0;
        lastpickup = -1;
        lastpickupmillis = 0;
//        charinfo.resetao();
    }
};

#include "mapentities.h"

namespace entities
{
    extern const char *entmdlname(int type);
    extern const char *itemname(int i);

    extern void preloadentities();
    extern void renderentities();
    extern void checkitems(DynamicEntity *d);
    extern void checkquad(int time, DynamicEntity *d);
    extern void resetspawns();
    extern void spawnitems();
    extern void putitems(packetbuf &p);
    extern void setspawn(int i, bool on);
    extern void teleport(int n, DynamicEntity *d);
    extern void pickupeffects(int n, DynamicEntity *d);

    extern void repammo(DynamicEntity *d, int type, bool local = true);

    extern void checkmapuids();
    extern void setmapuid(int i, int uid);
    extern int getlastmapuid();
}

namespace CutScene
{
	extern void update();
}

#include "clientmode.h"
#include "control.h"

enum
{
	ANIM_HOLD1 = ANIM_GAMESPECIFIC, ANIM_HOLD2, ANIM_HOLD3, ANIM_HOLD4, ANIM_HOLD5, ANIM_HOLD6, ANIM_HOLD7,
	ANIM_ATTACK1, ANIM_ATTACK2, ANIM_ATTACK3, ANIM_ATTACK4, ANIM_ATTACK5, ANIM_ATTACK6, ANIM_ATTACK7,
	ANIM_TAUNT, ANIM_WIN, ANIM_LOSE,
	ANIM_GUN_IDLE, ANIM_GUN_SHOOT,
	ANIM_VWEP_IDLE, ANIM_VWEP_SHOOT, ANIM_SHIELD, ANIM_POWERUP,
	ANIM_WAVE, ANIM_BOW,
	NUMANIMS
};

static const char * const animnames[] =
{
	ANIMNAMES,
	"hold 1", "hold 2", "hold 3", "hold 4", "hold 5", "hold 6", "hold 7",
	"attack 1", "attack 2", "attack 3", "attack 4", "attack 5", "attack 6", "attack 7",
	"taunt", "win", "lose",
	"gun idle", "gun shoot",
	"vwep idle", "vwep shoot", "shield", "powerup",
	"wave", "bow"
};

namespace game
{
    extern clientmode *cmode;
    extern void setclientmode();
    extern vector<PlayerModelInfo*> playermodels;


    // fps
    extern int gamemode, nextmode;
    extern string clientmap;
    extern int minremain;
    extern bool intermission;
    extern int maptime, maprealtime;
    extern DynamicEntity *player1;
    extern vector<DynamicEntity *> players, clients;
    extern int lastspawnattempt;
    extern int lasthit;
    extern int respawnent;
    extern int following;
    extern int smoothmove, smoothdist;
    extern int deathscore;

    extern bool clientoption(const char *arg);
    extern DynamicEntity *getclient(int cn);
    extern DynamicEntity *newclient(int cn);
    extern const char *colorname(DynamicEntity *d, const char *name = NULL, const char *prefix = "");
    extern DynamicEntity *pointatplayer();
    extern DynamicEntity *hudplayer();
    extern DynamicEntity *followingplayer();
    extern void stopfollowing();
    extern void clientdisconnected(int cn, bool notify = true);
    extern void clearclients(bool notify = true);
    extern void startgame();
    extern void spawnplayer(DynamicEntity *);
    extern void deathstate(DynamicEntity *d, bool restore = false);
    extern void damaged(int damage, DynamicEntity *d, DynamicEntity *actor, bool local = true);
    extern void killed(DynamicEntity *d, DynamicEntity *actor);
    extern void timeupdate(int timeremain);
    extern void msgsound(int n, physent *d = NULL);
    extern void predictplayer(DynamicEntity *d, bool move);

    extern vector<Control*> controls;

    enum
    {
        HICON_BLUE_ARMOUR = 0,
        HICON_GREEN_ARMOUR,
        HICON_YELLOW_ARMOUR,

        HICON_HEALTH,

        HICON_FIST,
        HICON_SG,
        HICON_CG,
        HICON_RL,
        HICON_RIFLE,
        HICON_GL,
        HICON_PISTOL,

        HICON_QUAD,

        HICON_RED_FLAG,
        HICON_BLUE_FLAG,

        HICON_X       = 20,
        HICON_Y       = 1650,
        HICON_TEXTY   = 1644,
        HICON_STEP    = 490,
        HICON_SIZE    = 120,
        HICON_SPACE   = 40
    };

    extern void drawicon(int icon, float x, float y, float sz = 120);

    // client
    extern bool connected, remote, demoplayback;

    extern int parseplayer(const char *arg);
    extern void addmsg(int type, const char *fmt = NULL, ...);
    extern void switchname(const char *name);
    extern void switchplayermodel(int playermodel);
    extern void sendmapinfo();
    extern void stopdemo();
    extern void changemap(const char *name, int mode);
    extern void c2sinfo();
    extern const char* attachtag(int i);
    extern int attachnumbypart(char* part);

    // movable
    struct movable;
    extern vector<movable *> movables;

    extern void clearmovables();
    extern void updatemovables(int curtime);
    extern void rendermovables();
    extern void suicidemovable(movable *m);
    extern void hitmovable(int damage, movable *m, DynamicEntity *at, const vec &vel, int gun);

    extern bool intersect(dynent *d, const vec &from, const vec &to);
    extern dynent *intersectclosest(const vec &from, const vec &to, DynamicEntity *at);

    extern void saveragdoll(DynamicEntity *d);
    extern void clearragdolls();
    extern void moveragdolls();
    extern const PlayerModelInfo &getplayermodelinfo(DynamicEntity *d);
    extern int chooserandomplayermodel(int seed);
    extern void swayhudgun(int curtime);
    extern vec hudgunorigin(int gun, const vec &from, const vec &to, DynamicEntity *d);
}

namespace server
{
	// extern bool allowupload; //offtools: server option
	extern const char *modename(int n, const char *unknown = "unknown");
    extern const char *mastermodename(int n, const char *unknown = "unknown");
    extern void startintermission();
    extern void stopdemo();
    extern void forcemap(const char *map, int mode);
    extern void hashpassword(int cn, int sessionid, const char *pwd, char *result, int maxlen = MAXSTRLEN);
    extern int msgsizelookup(int msg);
    extern bool serveroption(const char *arg);
}

#endif
