#ifndef __GAME_H__
#define __GAME_H__

#include "cube.h"

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
    CON_FRAG_OTHER = 1<<12,
    CON_TEAMKILL   = 1<<13
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
    TELEPORT,                   // attr1 = idx, attr2 = model, attr3 = tag
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
    DYNLIGHT,
    MAXENTTYPES
};

enum
{
    TRIGGER_RESET = 0,
    TRIGGERING,
    TRIGGERED,
    TRIGGER_RESETTING,
    TRIGGER_DISAPPEARED
};

struct fpsentity : extentity
{
    int triggerstate, lasttrigger;

    fpsentity() : triggerstate(TRIGGER_RESET), lasttrigger(0) {}
};

enum { GUN_FIST = 0, GUN_SG, GUN_CG, GUN_RL, GUN_RIFLE, GUN_GL, GUN_PISTOL, GUN_FIREBALL, GUN_ICEBALL, GUN_SLIMEBALL, GUN_BITE, GUN_BARREL, NUMGUNS };
enum { A_BLUE, A_GREEN, A_YELLOW };     // armour types... take 20/40/60 % off
enum { M_NONE = 0, M_SEARCH, M_HOME, M_ATTACKING, M_PAIN, M_SLEEP, M_AIMING };  // monster states

enum
{
    M_TEAM       = 1<<0,
    M_NOITEMS    = 1<<1,
    M_NOAMMO     = 1<<2,
    M_INSTA      = 1<<3,
    M_EFFICIENCY = 1<<4,
    M_TACTICS    = 1<<5,
    M_CAPTURE    = 1<<6,
    M_REGEN      = 1<<7,
    M_CTF        = 1<<8,
    M_PROTECT    = 1<<9,
    M_HOLD       = 1<<10,
    M_OVERTIME   = 1<<11,
    M_EDIT       = 1<<12,
    M_DEMO       = 1<<13,
    M_LOCAL      = 1<<14,
    M_LOBBY      = 1<<15,
    M_DMSP       = 1<<16,
    M_CLASSICSP  = 1<<17,
    M_SLOWMO     = 1<<18,
    M_COLLECT    = 1<<19
};

static struct gamemodeinfo
{
    const char *name;
    int flags;
    const char *info;
} gamemodes[] =
{
    { "DMSP", M_LOCAL | M_DMSP, NULL },
	{ "SP", M_LOCAL | M_CLASSICSP, "enable single player options" },
    { "demo", M_DEMO | M_LOCAL, "playback recorded demos" },
    { "default", M_LOBBY, "Run around freely without a care in the world" },
    { "coop edit", M_EDIT, "Edit cooperatively with friends over the LAN or internet" },
    { "banana relay", M_NOAMMO | M_TACTICS | M_CAPTURE | M_TEAM, "Capture bananas, robochimp is hungry" },
    { "capture the banana (ctf)", M_CTF | M_TEAM, "Capture the other team's banana, and add it to your own"},
	{ "team drenchmatch", M_TEAM | M_OVERTIME, "Teamplay: Collect items for ammo. Drench \fs\f3the enemy team\fr to score points for \fs\f1your team\fr." },
	{ "instadrench", M_NOITEMS | M_INSTA, "Instadrench: You spawn with full rifle ammo and drench instantly from one shot. There are no items. Drench everyone to score points." },
    { "instadrench team", M_NOITEMS | M_INSTA | M_TEAM | M_OVERTIME, "Instadrench Team: You spawn with full rifle ammo and drench instantly from one shot. There are no items. Drench \fs\f3the enemy team\fr to score points for \fs\f1your team\fr." }
};

#define STARTGAMEMODE (-3)
#define NUMGAMEMODES ((int)(sizeof(gamemodes)/sizeof(gamemodes[0])))

#define m_valid(mode)          ((mode) >= STARTGAMEMODE && (mode) < STARTGAMEMODE + NUMGAMEMODES)
#define m_check(mode, flag)    (m_valid(mode) && gamemodes[(mode) - STARTGAMEMODE].flags&(flag))
#define m_checknot(mode, flag) (m_valid(mode) && !(gamemodes[(mode) - STARTGAMEMODE].flags&(flag)))
#define m_checkall(mode, flag) (m_valid(mode) && (gamemodes[(mode) - STARTGAMEMODE].flags&(flag)) == (flag))

#define m_noitems      (m_check(gamemode, M_NOITEMS))
#define m_noammo       (m_check(gamemode, M_NOAMMO|M_NOITEMS))
#define m_insta        (m_check(gamemode, M_INSTA))
#define m_tactics      (m_check(gamemode, M_TACTICS))
#define m_efficiency   (m_check(gamemode, M_EFFICIENCY))
#define m_capture      (m_check(gamemode, M_CAPTURE))
#define m_regencapture (m_checkall(gamemode, M_CAPTURE | M_REGEN))
#define m_ctf          (m_check(gamemode, M_CTF))
#define m_protect      (m_checkall(gamemode, M_CTF | M_PROTECT))
#define m_hold         (m_checkall(gamemode, M_CTF | M_HOLD))
#define m_collect      (m_check(gamemode, M_COLLECT))
#define m_teammode     (m_check(gamemode, M_TEAM))
#define m_overtime     (m_check(gamemode, M_OVERTIME))
#define isteam(a,b)    (m_teammode && strcmp(a, b)==0)

#define m_demo         (m_check(gamemode, M_DEMO))
#define m_edit         (m_check(gamemode, M_EDIT))
#define m_lobby        (m_check(gamemode, M_LOBBY))
#define m_timed        (m_checknot(gamemode, M_DEMO|M_EDIT|M_LOCAL|M_LOBBY))
#define m_botmode      (m_checknot(gamemode, M_DEMO|M_LOCAL))
#define m_mp(mode)     (m_checknot(mode, M_LOCAL))

#define m_sp           (m_check(gamemode, M_DMSP | M_CLASSICSP))
#define m_dmsp         (m_check(gamemode, M_DMSP))
#define m_classicsp    (m_check(gamemode, M_CLASSICSP))

enum { MM_AUTH = -1, MM_OPEN = 0, MM_VETO, MM_LOCKED, MM_PRIVATE, MM_PASSWORD, MM_START = MM_AUTH };

static const char * const mastermodenames[] =  { "auth",   "open",   "veto",       "locked",     "private",    "password" };
static const char * const mastermodecolors[] = { "",       "\f0",    "\f2",        "\f2",        "\f3",        "\f3" };
static const char * const mastermodeicons[] =  { "server", "server", "serverlock", "serverlock", "serverpriv", "serverpriv" };

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
    N_CONNECT = 0, N_SERVINFO, N_WELCOME, N_INITCLIENT, N_POS, N_TEXT, N_SOUND, N_CDIS,
    N_SHOOT, N_EXPLODE, N_SUICIDE,
    N_DIED, N_DAMAGE, N_HITPUSH, N_SHOTFX, N_EXPLODEFX,
    N_TRYSPAWN, N_SPAWNSTATE, N_SPAWN, N_FORCEDEATH,
    N_GUNSELECT, N_TAUNT,
    N_MAPCHANGE, N_MAPVOTE, N_ITEMSPAWN, N_ITEMPICKUP, N_ITEMACC, N_TELEPORT, N_JUMPPAD,
    N_PING, N_PONG, N_CLIENTPING,
    N_TIMEUP, N_MAPRELOAD, N_FORCEINTERMISSION,
    N_SERVMSG, N_ITEMLIST, N_RESUME,
    N_EDITMODE, N_EDITENT, N_EDITF, N_EDITT, N_EDITM, N_FLIP, N_COPY, N_PASTE, N_ROTATE, N_REPLACE,
    N_DELCUBE, N_REMIP, N_NEWMAP, N_GETMAP, N_SENDMAP, N_SENDCFG, N_UPLOADMAP, N_UPLOADCFG,
    N_SENDTEXTURE, N_TEXTUREREQUEST, N_CLIPBOARD, N_EDITVAR,
    N_MASTERMODE, N_KICK, N_CLEARBANS, N_CURRENTMASTER, N_SPECTATOR, N_SETMASTER, N_SETTEAM,
    N_BASES, N_BASEINFO, N_BASESCORE, N_REPAMMO, N_BASEREGEN, N_ANNOUNCE,
    N_LISTDEMOS, N_SENDDEMOLIST, N_GETDEMO, N_SENDDEMO,
    N_DEMOPLAYBACK, N_RECORDDEMO, N_STOPDEMO, N_CLEARDEMOS,
    N_TAKEFLAG, N_RETURNFLAG, N_RESETFLAG, N_INVISFLAG, N_TRYDROPFLAG, N_DROPFLAG, N_SCOREFLAG, N_INITFLAGS,
    N_CLIENT,
    N_AUTHTRY, N_AUTHCHAL, N_AUTHANS, N_REQAUTH,
    N_PAUSEGAME,
    N_ADDBOT, N_DELBOT, N_INITAI, N_FROMAI, N_BOTLIMIT, N_BOTBALANCE,
    N_MAPCRC, N_CHECKMAPS,
    N_SWITCHNAME, N_SWITCHMODEL, N_SWITCHTEAM,
    N_INITTOKENS, N_TAKETOKEN, N_EXPIRETOKENS, N_DROPTOKENS, N_DEPOSITTOKENS,
	N_SERVCMD,
    N_ATTACH, N_DETACH,
    NUMSV
};

static const int msgsizes[] =               // size inclusive message token, 0 for variable or not-checked sizes
{
    N_CONNECT, 0, N_SERVINFO, 0, N_WELCOME, 2, N_INITCLIENT, 0, N_POS, 0, N_TEXT, 0, N_SOUND, 2, N_CDIS, 2,
    N_SHOOT, 0, N_EXPLODE, 0, N_SUICIDE, 1,
    N_DIED, 4, N_DAMAGE, 6, N_HITPUSH, 7, N_SHOTFX, 10, N_EXPLODEFX, 4,
    N_TRYSPAWN, 1, N_SPAWNSTATE, 14, N_SPAWN, 3, N_FORCEDEATH, 2,
    N_GUNSELECT, 2, N_TAUNT, 1,
    N_MAPCHANGE, 0, N_MAPVOTE, 0, N_ITEMSPAWN, 2, N_ITEMPICKUP, 2, N_ITEMACC, 3,
    N_PING, 2, N_PONG, 2, N_CLIENTPING, 2,
    N_TIMEUP, 2, N_MAPRELOAD, 1, N_FORCEINTERMISSION, 1,
    N_SERVMSG, 0, N_ITEMLIST, 0, N_RESUME, 0,
    N_EDITMODE, 2, N_EDITENT, 0, N_EDITF, 16, N_EDITT, 16, N_EDITM, 16, N_FLIP, 14, N_COPY, 14, N_PASTE, 14, N_ROTATE, 15, N_REPLACE, 17,
    N_DELCUBE, 14, N_REMIP, 1, N_NEWMAP, 2, N_GETMAP, 1, N_SENDMAP, 0, N_SENDCFG, 0, N_UPLOADMAP, 0, N_UPLOADCFG, 0,
    N_SENDTEXTURE, 0, N_TEXTUREREQUEST, 0, N_EDITVAR, 0,
    N_MASTERMODE, 2, N_KICK, 2, N_CLEARBANS, 1, N_CURRENTMASTER, 4, N_SPECTATOR, 3, N_SETMASTER, 0, N_SETTEAM, 0,
    N_BASES, 0, N_BASEINFO, 0, N_BASESCORE, 0, N_REPAMMO, 1, N_BASEREGEN, 6, N_ANNOUNCE, 2,
    N_LISTDEMOS, 1, N_SENDDEMOLIST, 0, N_GETDEMO, 2, N_SENDDEMO, 0,
    N_DEMOPLAYBACK, 3, N_RECORDDEMO, 2, N_STOPDEMO, 1, N_CLEARDEMOS, 2,
    N_TAKEFLAG, 3, N_RETURNFLAG, 4, N_RESETFLAG, 6, N_INVISFLAG, 3, N_TRYDROPFLAG, 1, N_DROPFLAG, 7, N_SCOREFLAG, 10, N_INITFLAGS, 0,
    N_CLIENT, 0,
    N_AUTHTRY, 0, N_AUTHCHAL, 0, N_AUTHANS, 0, N_REQAUTH, 0,
    N_PAUSEGAME, 2,
    N_ADDBOT, 2, N_DELBOT, 1, N_INITAI, 0, N_FROMAI, 2, N_BOTLIMIT, 2, N_BOTBALANCE, 2,
    N_MAPCRC, 0, N_CHECKMAPS, 1,
    N_SWITCHNAME, 0, N_SWITCHMODEL, 2, N_SWITCHTEAM, 0,
    N_INITTOKENS, 0, N_TAKETOKEN, 2, N_EXPIRETOKENS, 0, N_DROPTOKENS, 0, N_DEPOSITTOKENS, 2,
	N_SERVCMD, 0,
    N_ATTACH, 0, N_DETACH, 0,
    -1
};

#define SANDBOX_LANINFO_PORT 28784
#define SANDBOX_SERVER_PORT 28785
#define SANDBOX_SERVINFO_PORT 28786
#define SANDBOX_MASTER_PORT 28787
#define PROTOCOL_VERSION 9              // bump when protocol changes
#define DEMO_VERSION 1                  // bump when demo format changes
#define DEMO_MAGIC "SAUERBRATEN_DEMO"

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


#define MAXNAMELEN 15
#define MAXTEAMLEN 4

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
    HICON_NEUTRAL_FLAG,

    HICON_TOKEN,

    HICON_X       = 20,
    HICON_Y       = 1650,
    HICON_TEXTY   = 1644,
    HICON_STEP    = 490,
    HICON_SIZE    = 120,
    HICON_SPACE   = 40
};

static struct itemstat { int add, max, sound; const char *name; int icon, info; } itemstats[] =

{
    {10,    30,    S_ITEMAMMO,   "SG", HICON_SG, GUN_SG},
    {40,    100,    S_ITEMAMMO,   "CG", HICON_CG, GUN_CG},
    {5,     15,    S_ITEMAMMO,   "RL", HICON_RL, GUN_RL},
    {5,     15,    S_ITEMAMMO,   "RI", HICON_RIFLE, GUN_RIFLE},
    {4,    8,    S_ITEMAMMO,   "GL", HICON_GL, GUN_GL},
    {30,    120,   S_ITEMAMMO,   "PI", HICON_PISTOL, GUN_PISTOL},
    {25,    100,   S_ITEMHEALTH, "H", HICON_HEALTH},
    {10,    1000,  S_ITEMHEALTH, "MH", HICON_HEALTH},
    {100,   100,   S_ITEMARMOUR, "GA", HICON_GREEN_ARMOUR, A_GREEN},
    {200,   200,   S_ITEMARMOUR, "YA", HICON_YELLOW_ARMOUR, A_YELLOW},
    {20000, 30000, S_ITEMPUP,    "Q", HICON_QUAD},
};

/* original values cal edit
{
    {10,    30,    S_ITEMAMMO,   "SG", HICON_SG, GUN_SG},
    {20,    60,    S_ITEMAMMO,   "CG", HICON_CG, GUN_CG},
    {5,     15,    S_ITEMAMMO,   "RL", HICON_RL, GUN_RL},
    {5,     15,    S_ITEMAMMO,   "RI", HICON_RIFLE, GUN_RIFLE},
    {10,    30,    S_ITEMAMMO,   "GL", HICON_GL, GUN_GL},
    {30,    120,   S_ITEMAMMO,   "PI", HICON_PISTOL, GUN_PISTOL},
    {25,    100,   S_ITEMHEALTH, "H", HICON_HEALTH},
    {10,    1000,  S_ITEMHEALTH, "MH", HICON_HEALTH},
    {100,   100,   S_ITEMARMOUR, "GA", HICON_GREEN_ARMOUR, A_GREEN},
    {200,   200,   S_ITEMARMOUR, "YA", HICON_YELLOW_ARMOUR, A_YELLOW},
    {20000, 30000, S_ITEMPUP,    "Q", HICON_QUAD},
};
*/


#define SGRAYS 20
#define RL_DAMRAD 40
#define RL_SELFDAMDIV 2
#define RL_DISTSCALE 1.5f

static const struct guninfo { short sound, attackdelay, damage, spread, projspeed, part, kickamount, range; const char *name, *file; } guns[NUMGUNS] =

{
    { S_PUNCH1,    250,  50, 0,   0,   0, 0,   14,  "fist",            "fist"  },
    { S_SG,       1400,  15, 400, 0,   0, 20, 1024, "shotgun",         "shotg" },  // *SGRAYS
    { S_CG,        100,  20, 100, 0,   0, 7, 1024,  "chaingun",        "chaing"},
    { S_RLFIRE,    1800, 120, 0,   180,  0, 10, 1024, "rocketlauncher",  "rocket"},
    { S_RIFLE,    1500, 75, 0,   0,   0, 30, 2048, "rifle",           "rifle" },
    { S_FLAUNCH,  2000,  175, 0,   300, 0, 10, 25, "grenadelauncher", "gl" },
    { S_PISTOL,    500,  20, 50,  0,   0,  7, 1024, "pistol",          "pistol" },
    { S_FLAUNCH,   200,  20, 0,   50,  PART_FIREBALL1,  1, 1024, "fireball",  NULL },
    { S_ICEBALL,   200,  40, 0,   30,  PART_FIREBALL2,  1, 1024, "iceball",   NULL },
    { S_SLIMEBALL, 200,  30, 0,   160, PART_FIREBALL3,  1, 1024, "slimeball", NULL },
    { S_PIGR1,     250,  50, 0,   0,   0,  1,   12, "bite",            NULL },
    { -1,            0, 120, 0,   0,   0,  0,    0, "barrel",          NULL }
};

/* Original weapon values cal edit
{
    { S_PUNCH1,    250,  50, 0,   0,   0, 0,   14,  "fist",            "fist"  },
    { S_SG,       1400,  10, 400, 0,   0, 20, 1024, "shotgun",         "shotg" },  // *SGRAYS
    { S_CG,        100,  30, 100, 0,   0, 7, 1024,  "chaingun",        "chaing"},
    { S_RLFIRE,    800, 120, 0,   80,  0, 10, 1024, "rocketlauncher",  "rocket"},
    { S_RIFLE,    1500, 100, 0,   0,   0, 30, 2048, "rifle",           "rifle" },
    { S_FLAUNCH,   500,  75, 0,   270, 0, 10, 1024, "grenadelauncher", "gl" },
    { S_PISTOL,    500,  35, 50,  0,   0,  7, 1024, "pistol",          "pistol" },
    { S_FLAUNCH,   200,  20, 0,   50,  PART_FIREBALL1,  1, 1024, "fireball",  NULL },
    { S_ICEBALL,   200,  40, 0,   30,  PART_FIREBALL2,  1, 1024, "iceball",   NULL },
    { S_SLIMEBALL, 200,  30, 0,   160, PART_FIREBALL3,  1, 1024, "slimeball", NULL },
    { S_PIGR1,     250,  50, 0,   0,   0,  1,   12, "bite",            NULL },
    { -1,            0, 120, 0,   0,   0,  0,    0, "barrel",          NULL }
};
*/

#include "ai.h"

// inherited by fpsent and server clients
struct fpsstate
{
    int health, maxhealth;
    int armour, armourtype;
    int quadmillis;
    int gunselect, gunwait;
    int ammo[NUMGUNS];
    int aitype, skill;

    fpsstate() : maxhealth(100), aitype(AI_NONE), skill(0) {}

    void baseammo(int gun, int k = 2, int scale = 1)
    {
        ammo[gun] = (itemstats[gun-GUN_SG].add*k)/scale;
    }

    void addammo(int gun, int k = 1, int scale = 1)
    {
        itemstat &is = itemstats[gun-GUN_SG];
        ammo[gun] = min(ammo[gun] + (is.add*k)/scale, is.max);
    }

    bool hasmaxammo(int type)
    {
       const itemstat &is = itemstats[type-I_SHELLS];
       return ammo[type-I_SHELLS+GUN_SG]>=is.max;
    }

    bool canpickup(int type)
    {
        if(type<I_SHELLS || type>I_QUAD) return false;
        itemstat &is = itemstats[type-I_SHELLS];
        switch(type)
        {
            case I_BOOST: return maxhealth<is.max;
            case I_HEALTH: return health<maxhealth;
            case I_GREENARMOUR:
                // (100h/100g only absorbs 200 damage)
                if(armourtype==A_YELLOW && armour>=100) return false;
            case I_YELLOWARMOUR: return !armourtype || armour<is.max;
            case I_QUAD: return quadmillis<is.max;
            default: return ammo[is.info]<is.max;
        }
    }

    void pickup(int type)
    {
        if(type<I_SHELLS || type>I_QUAD) return;
        itemstat &is = itemstats[type-I_SHELLS];
        switch(type)
        {
            case I_BOOST:
                maxhealth = min(maxhealth+is.add, is.max);
            case I_HEALTH: // boost also adds to health
                health = min(health+is.add, maxhealth);
                break;
            case I_GREENARMOUR:
            case I_YELLOWARMOUR:
                armour = min(armour+is.add, is.max);
                armourtype = is.info;
                break;
            case I_QUAD:
                quadmillis = min(quadmillis+is.add, is.max);
                break;
            default:
                ammo[is.info] = min(ammo[is.info]+is.add, is.max);
                break;
        }
    }

    void respawn()
    {
        health = maxhealth;
        armour = 0;
        armourtype = A_BLUE;
        quadmillis = 0;
        gunselect = GUN_PISTOL;
        gunwait = 0;
        loopi(NUMGUNS) ammo[i] = 0;
        ammo[GUN_FIST] = 1;
    }

    void spawnstate(int gamemode)
    {
        if(m_demo)
        {
            gunselect = GUN_FIST;
        }
        else if(m_insta)
        {
            armour = 0;
            health = 1;
            gunselect = GUN_RIFLE;
            ammo[GUN_RIFLE] = 100;
        }
        else if(m_regencapture)
        {
            armourtype = A_GREEN;
            armour = 0;
            gunselect = GUN_PISTOL;
            ammo[GUN_PISTOL] = 40;
            ammo[GUN_GL] = 1;
        }
        else if(m_tactics)
        {
            armourtype = A_GREEN;
            armour = 100;
            ammo[GUN_PISTOL] = 40;
            int spawngun1 = rnd(5)+1, spawngun2;
            gunselect = spawngun1;
            baseammo(spawngun1, m_noitems ? 2 : 1);
            do spawngun2 = rnd(5)+1; while(spawngun1==spawngun2);
            baseammo(spawngun2, m_noitems ? 2 : 1);
            if(m_noitems) ammo[GUN_GL] += 1;
        }
        else if(m_efficiency)
        {
            armourtype = A_GREEN;
            armour = 100;
            loopi(5) baseammo(i+1);
            gunselect = GUN_CG;
            ammo[GUN_CG] /= 2;
        }
        else if(m_ctf || m_collect)
        {
            armourtype = A_BLUE;
            armour = 50;
            ammo[GUN_PISTOL] = 40;
            ammo[GUN_GL] = 1;
        }
        else
        {
            ammo[GUN_PISTOL] = m_sp ? 80 : 40;
            ammo[GUN_GL] = 1;
        }
    }

    // just subtract damage here, can set death, etc. later in code calling this
    int dodamage(int damage)
    {
        int ad = damage*(armourtype+1)*25/100; // let armour absorb when possible
        if(ad>armour) ad = armour;
        armour -= ad;
        damage -= ad;
        health -= damage;
        return damage;
    }

    int hasammo(int gun, int exclude = -1)
    {
        return gun >= 0 && gun <= NUMGUNS && gun != exclude && ammo[gun] > 0;
    }
};

/*
 * offtools: attachments (clothes ...)
 * documentation:
 * following tags should appear in the model config file
 */

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

/*
 * offtools:
 * animation handling of the attachments (not implemented)
 *
 * handle animation:
 * ATTACH_ANIM_NONE - static, no animation
 * ATTACH_ANIM_IDLE - only use idle animation
 * ATTACH_ANIM_FULL - use current animation of model
 *
 * ao (animation overwrite, e.g. holding a flag)
 * not implemeted
 */

enum { ATTACH_ANIM_NONE = 0, ATTACH_ANIM_IDLE, ATTACH_ANIM_FULL, ATTACH_ANIM_NUMRULES };

/*
 * offtools:
 * attachlist - manages the player / entity attachments
 */
struct attachlist
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

    vector<attachment*> list;

    bool hasattachment(int part)
    {
        loopv(list)
        {
            if(list[i]->part == part)
                return true;
        }
        return false;
    }

    int numattachments()
    {
        return list.length();
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

        attachment* a = list.add(new attachment);
        if(animrule > ATTACH_ANIM_NONE || animrule <= ATTACH_ANIM_NUMRULES) a->animrule = animrule;
        copystring(a->model, model);
        a->part = part;
    }

    void remove(int part)
    {
        loopv(list)
        {
            if(list[i]->part == part) {
                list.remove(i);
                return;
            }
        }
    }
};

struct fpsent : dynent, fpsstate
{
    int weight;                         // affects the effectiveness of hitpush
    int clientnum, privilege, lastupdate, plag, ping;
    int lifesequence;                   // sequence id for each respawn, used in damage test
    int respawned, suicided;
    int lastpain;
    int lastaction, lastattackgun;
    bool attacking;
    int attacksound, attackchan, idlesound, idlechan;
    int lasttaunt;
    int lastpickup, lastpickupmillis, lastbase, lastrepammo, flagpickup, tokens;
    vec lastcollect;
    int frags, flags, deaths, totaldamage, totalshots;
    editinfo *edit;
    float deltayaw, deltapitch, deltaroll, newyaw, newpitch, newroll;
    int smoothmillis;

    string name, team, info;
    int playermodel;
    ai::aiinfo *ai;
    int ownernum, lastnode;

    attachlist attached;

    vec muzzle;

    fpsent() : weight(100), clientnum(-1), privilege(PRIV_NONE), lastupdate(0), plag(0), ping(0), lifesequence(0), respawned(-1), suicided(-1), lastpain(0), attacksound(-1), attackchan(-1), idlesound(-1), idlechan(-1), frags(0), flags(0), deaths(0), totaldamage(0), totalshots(0), edit(NULL), smoothmillis(-1), playermodel(-1), ai(NULL), ownernum(-1), muzzle(-1, -1, -1)
    {
        name[0] = team[0] = info[0] = 0;
        respawn();
    }
    ~fpsent()
    {
        freeeditinfo(edit);
        if(attackchan >= 0) stopsound(attacksound, attackchan);
        if(idlechan >= 0) stopsound(idlesound, idlechan);
        if(ai) delete ai;
    }

    void hitpush(int damage, const vec &dir, fpsent *actor, int gun)
    {
        vec push(dir);
        push.mul(80*damage/weight);
        if(gun==GUN_RL || gun==GUN_GL) push.mul(actor==this ? 5 : (type==ENT_AI ? 3 : 2));
        vel.add(push);
    }

    void stopattacksound()
    {
        if(attackchan >= 0) stopsound(attacksound, attackchan, 250);
        attacksound = attackchan = -1;
    }

    void stopidlesound()
    {
        if(idlechan >= 0) stopsound(idlesound, idlechan, 100);
        idlesound = idlechan = -1;
    }

    void respawn()
    {
        dynent::reset();
        fpsstate::respawn();
        respawned = suicided = -1;
        lastaction = 0;
        lastattackgun = gunselect;
        attacking = false;
        lasttaunt = 0;
        lastpickup = -1;
        lastpickupmillis = 0;
        lastbase = lastrepammo = -1;
        flagpickup = 0;
        tokens = 0;
        lastcollect = vec(-1e10f, -1e10f, -1e10f);
        stopattacksound();
        lastnode = -1;
    }
};

struct teamscore
{
    const char *team;
    int score;
    teamscore() {}
    teamscore(const char *s, int n) : team(s), score(n) {}

    static bool compare(const teamscore &x, const teamscore &y)
    {
        if(x.score > y.score) return true;
        if(x.score < y.score) return false;
        return strcmp(x.team, y.team) < 0;
    }
};

enum
{
    ANIM_HOLD1 = ANIM_GAMESPECIFIC, ANIM_HOLD2, ANIM_HOLD3, ANIM_HOLD4, ANIM_HOLD5, ANIM_HOLD6, ANIM_HOLD7,
    ANIM_ATTACK1, ANIM_ATTACK2, ANIM_ATTACK3, ANIM_ATTACK4, ANIM_ATTACK5, ANIM_ATTACK6, ANIM_ATTACK7,
    ANIM_TAUNT, ANIM_WIN, ANIM_LOSE,
    ANIM_GUN_IDLE, ANIM_GUN_SHOOT,
    ANIM_VWEP_IDLE, ANIM_VWEP_SHOOT, ANIM_SHIELD, ANIM_POWERUP,
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
};

namespace entities
{
    extern vector<extentity *> ents;

    extern bool dynlightsactive[32767];
    extern void adddynlights();

    extern const char *entmdlname(int type);
    extern const char *itemname(int i);
    extern int itemicon(int i);

    extern void preloadentities();
    extern void renderentities();
    extern void resettriggers();
    extern void checktriggers();
    extern void checkitems(fpsent *d);
    extern void checkquad(int time, fpsent *d);
    extern void resetspawns();
    extern void spawnitems(bool force = false);
    extern void putitems(packetbuf &p);
    extern void setspawn(int i, bool on);
    extern void teleport(int n, fpsent *d);
    extern void pickupeffects(int n, fpsent *d);
    extern void teleporteffects(fpsent *d, int tp, int td, bool local = true);
    extern void jumppadeffects(fpsent *d, int jp, bool local = true);

    extern void repammo(fpsent *d, int type, bool local = true);
}

namespace game
{
    struct clientmode
    {
        virtual ~clientmode() {}

        virtual void preload() {}
        virtual int clipconsole(int w, int h) { return 0; }
        virtual void drawhud(fpsent *d, int w, int h) {}
        virtual void rendergame() {}
        virtual void respawned(fpsent *d) {}
        virtual void setup() {}
        virtual void checkitems(fpsent *d) {}
        virtual int respawnwait(fpsent *d) { return 0; }
        virtual void pickspawn(fpsent *d) { findplayerspawn(d); }
        virtual void senditems(packetbuf &p) {}
        virtual const char *prefixnextmap() { return ""; }
        virtual void removeplayer(fpsent *d) {}
        virtual void gameover() {}
        virtual bool hidefrags() { return false; }
        virtual int getteamscore(const char *team) { return 0; }
        virtual void getteamscores(vector<teamscore> &scores) {}
        virtual void aifind(fpsent *d, ai::aistate &b, vector<ai::interest> &interests) {}
        virtual bool aicheck(fpsent *d, ai::aistate &b) { return false; }
        virtual bool aidefend(fpsent *d, ai::aistate &b) { return false; }
        virtual bool aipursue(fpsent *d, ai::aistate &b) { return false; }
    };

    extern clientmode *cmode;
    extern void setclientmode();

    // fps
    extern int gamemode, nextmode;
    extern string clientmap;
    extern bool intermission;
    extern int maptime, maprealtime, maplimit;
    extern fpsent *player1;
    extern vector<fpsent *> players, clients;
    extern int lastspawnattempt;
    extern int lasthit;
    extern int respawnent;
    extern int following;
    extern int smoothmove, smoothdist;
    extern int deathscore;

    extern bool clientoption(const char *arg);
    extern fpsent *getclient(int cn);
    extern fpsent *newclient(int cn);
    extern const char *colorname(fpsent *d, const char *name = NULL, const char *prefix = "");
    extern fpsent *pointatplayer();
    extern fpsent *hudplayer();
    extern fpsent *followingplayer();
    extern void stopfollowing();
    extern void clientdisconnected(int cn, bool notify = true);
    extern void clearclients(bool notify = true);
    extern void startgame();
    extern void spawnplayer(fpsent *);
    extern void deathstate(fpsent *d, bool restore = false);
    extern void damaged(int damage, fpsent *d, fpsent *actor, bool local = true);
    extern void killed(fpsent *d, fpsent *actor);
    extern void timeupdate(int timeremain);
    extern void msgsound(int n, physent *d = NULL);
    struct monstertype      // see docs for how these values modify behaviour
    {
        short gun, speed, health, freq, lag, rate, pain, loyalty, bscale, weight;
        short painsound, diesound;
        const char *name, *mdlname, *vwepname;
    };

    extern void drawicon(int icon, float x, float y, float sz = 120);
    const char *mastermodecolor(int n, const char *unknown);
    const char *mastermodeicon(int n, const char *unknown);

    // client
    extern bool connected, remote, demoplayback;
    extern string servinfo;

    extern int parseplayer(const char *arg);
    extern void ignore(int cn);
    extern void unignore(int cn);
    extern bool isignored(int cn);
    extern void addmsg(int type, const char *fmt = NULL, ...);
    extern void switchname(const char *name);
    extern void switchteam(const char *name);
    extern void switchplayermodel(int playermodel);
    extern void sendmapinfo();
    extern void stopdemo();
    extern void changemap(const char *name, int mode);
    extern void c2sinfo(bool force = false);
    extern void sendposition(fpsent *d, bool reliable = false);
    extern const char* attachtag(int i);
    extern int attachnumbypart(char* part);

    // monster
    struct monster;
    extern vector<monster *> monsters;
    extern const int NUMMONSTERTYPES;
    extern const monstertype monstertypes[];

    extern void clearmonsters();
    extern void preloadmonsters();
    extern void stackmonster(monster *d, physent *o);
    extern void updatemonsters(int curtime);
    extern void rendermonsters();
    extern void suicidemonster(monster *m);
    extern void hitmonster(int damage, monster *m, fpsent *at, const vec &vel, int gun);
    extern void monsterkilled();
    extern void endsp(bool allkilled);
    extern void spsummary(int accuracy);

    // movable
    struct movable;
    extern vector<movable *> movables;

    extern void clearmovables();
    extern void stackmovable(movable *d, physent *o);
    extern void updatemovables(int curtime);
    extern void rendermovables();
    extern void suicidemovable(movable *m);
    extern void hitmovable(int damage, movable *m, fpsent *at, const vec &vel, int gun);

    // weapon
    extern int getweapon(const char *name);
    extern void shoot(fpsent *d, const vec &targ);
    extern void shoteffects(int gun, const vec &from, const vec &to, fpsent *d, bool local, int id, int prevaction);
    extern void explode(bool local, fpsent *owner, const vec &v, dynent *safe, int dam, int gun);
    extern void explodeeffects(int gun, fpsent *d, bool local, int id = 0);
    extern void damageeffect(int damage, fpsent *d, bool thirdperson = true);
    extern void gibeffect(int damage, const vec &vel, fpsent *d);
    extern float intersectdist;
    extern bool intersect(dynent *d, const vec &from, const vec &to, float &dist = intersectdist);
    extern dynent *intersectclosest(const vec &from, const vec &to, fpsent *at, float &dist = intersectdist);
    extern void clearbouncers();
    extern void updatebouncers(int curtime);
    extern void removebouncers(fpsent *owner);
    extern void renderbouncers();
    extern void clearprojectiles();
    extern void updateprojectiles(int curtime);
    extern void removeprojectiles(fpsent *owner);
    extern void renderprojectiles();
    extern void preloadbouncers();
    extern void removeweapons(fpsent *owner);
    extern void updateweapons(int curtime);
    extern void gunselect(int gun, fpsent *d);
    extern void weaponswitch(fpsent *d);
    extern void avoidweapons(ai::avoidset &obstacles, float radius);

    // scoreboard
    extern void showscores(bool on);
    extern void getbestplayers(vector<fpsent *> &best);
    extern void getbestteams(vector<const char *> &best);

    // render
    struct playermodelinfo
    {
        const char *ffa, *blueteam, *redteam, *hudguns,
                   *vwep, *quad, *armour[3],
                   *ffaicon, *blueicon, *redicon;
        bool ragdoll;
    };

    extern int playermodel, teamskins, testteam;

    extern void saveragdoll(fpsent *d);
    extern void clearragdolls();
    extern void moveragdolls();
    extern void changedplayermodel();
    extern const playermodelinfo &getplayermodelinfo(fpsent *d);
    extern int chooserandomplayermodel(int seed);
    extern void swayhudgun(int curtime);
    extern vec hudgunorigin(int gun, const vec &from, const vec &to, fpsent *d);
}

namespace server
{
    //~ extern bool allowupload; //offtools: server option
    extern const char *modename(int n, const char *unknown = "unknown");
    extern const char *mastermodename(int n, const char *unknown = "unknown");
    extern void startintermission();
    extern void stopdemo();
    extern void forcemap(const char *map, int mode);
    extern void hashpassword(int cn, int sessionid, const char *pwd, char *result, int maxlen = MAXSTRLEN);
    extern int msgsizelookup(int msg);
    extern bool serveroption(const char *arg);
    extern bool delayspawn(int type);
}

#endif

