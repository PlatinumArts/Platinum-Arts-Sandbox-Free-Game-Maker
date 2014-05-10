/*
 * server.h
 *
 *  Created on: 17.12.2009
 *      Author: offtools
 */

#ifndef __SERVER_H__
#define __SERVER_H__

#include "modeinfo.h"

namespace server
{
	struct clientinfo;

	struct servmode
    {
        virtual ~servmode() {}

        virtual void entergame(clientinfo *ci) {}
        virtual void leavegame(clientinfo *ci, bool disconnecting = false) {}

        virtual void moved(clientinfo *ci, const vec &oldpos, bool oldclip, const vec &newpos, bool newclip) {}
        virtual bool canspawn(clientinfo *ci, bool connecting = false) { return true; }
        virtual void spawned(clientinfo *ci) {}
        virtual int fragvalue(clientinfo *victim, clientinfo *actor) { return 0; }
        virtual void died(clientinfo *victim, clientinfo *actor) {}
        virtual bool canchangeteam(clientinfo *ci, const char *oldteam, const char *newteam) { return false; }
        virtual void changeteam(clientinfo *ci, const char *oldteam, const char *newteam) {}
        virtual void initclient(clientinfo *ci, packetbuf &p, bool connecting) {}
        virtual void update() {}
        virtual void reset(bool empty) {}
        virtual void intermission() {}
        virtual bool hidefrags() { return true; }
        virtual int getteamscore(const char *team) { return 0; }
        virtual void getteamscores() {}
        virtual bool extinfoteam(const char *team, ucharbuf &p) { return false; }
    };

	struct defaultservmode : servmode
	{
		defaultservmode() {}
		~defaultservmode() {}
	};
}
#endif /* __SERVER_H__ */
