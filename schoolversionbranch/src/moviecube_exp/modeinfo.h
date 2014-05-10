/*
 * modeinfo.h
 *
 *  Created on: 18.12.2009
 *      Author: tha
 */

//TODO:
//	*add trigger flag
//	*add new slowmo mode

#ifndef __MODEINFO_H__
#define __MODEINFO_H__

//Mode Flags - features used that came with engine or moviecube
enum
{
    M_BOTS		 = 1<<0,
    M_CAMERA	 = 1<<1,
    M_TRIGGER	 = 1<<2,
	M_OVERTIME   = 1<<3,
    M_EDIT       = 1<<4,
    M_LOCAL      = 1<<5,
    M_LOBBY      = 1<<6,
    M_DEMO       = 1<<7,
    M_TEAM       = 1<<8,
    M_NOITEMS    = 1<<9,
    M_NOAMMO     = 1<<10,
    M_DMSP       = 1<<11,
    M_CLASSICSP  = 1<<12,
    M_SLOWMO     = 1<<13
};

static struct gamemodeinfo
{
    const char *name;
    int flags;
    const char *info;
} gamemodes[] =
{
    { "lobby", M_LOBBY, "Run around freely, just visit the map"},
    { "movie", M_BOTS | M_CAMERA, "Spawn Characters and use Cameras to shot your movie" },
    { "coop edit", M_EDIT | M_BOTS | M_CAMERA, "Edit cooperatively with friends over the LAN or internet" },
    { "demo", M_DEMO | M_LOCAL, "playback recorded demos" },
};

#define STARTGAMEMODE (1)
#define NUMGAMEMODES ((int)(sizeof(gamemodes)/sizeof(gamemodes[0])))

#define m_valid(mode)          ((mode) >= STARTGAMEMODE && (mode) < STARTGAMEMODE + NUMGAMEMODES)
#define m_check(mode, flag)    (m_valid(mode) && gamemodes[(mode) - STARTGAMEMODE].flags&(flag))
#define m_checknot(mode, flag) (m_valid(mode) && !(gamemodes[(mode) - STARTGAMEMODE].flags&(flag)))
#define m_checkall(mode, flag) (m_valid(mode) && (gamemodes[(mode) - STARTGAMEMODE].flags&(flag)) == (flag))

#define m_noitems      (m_check(gamemode, M_NOITEMS))
#define m_noammo       (m_check(gamemode, M_NOAMMO|M_NOITEMS))
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

#endif /* __MODEINFO_H__ */
