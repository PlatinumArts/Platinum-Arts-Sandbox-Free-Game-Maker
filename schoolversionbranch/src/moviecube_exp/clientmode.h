/*
 * clientmode.h
 *
 *  Created on: 17.12.2009
 *      Author: offtools
 */

#ifndef __CLIENTMODE_H__
#define __CLIENTMODE_H__

#include "modeinfo.h"
//#include "dynentities.h"

namespace game
{
	struct clientmode
    {
        virtual ~clientmode() {}

        virtual void preload() {}
        virtual int clipconsole(int w, int h) { return 0; }
        virtual void drawhud(DynamicEntity *d, int w, int h) {}
        virtual void rendergame() {}
        virtual void respawned(DynamicEntity *d) {}
        virtual void setup() {}
        virtual void checkitems(DynamicEntity *d) {}
        virtual int respawnwait(DynamicEntity *d) { return 0; }
        virtual void pickspawn(DynamicEntity *d) {}
        virtual void senditems(packetbuf &p) {}
        virtual const char *prefixnextmap() { return "movie"; }
        virtual void removeplayer(DynamicEntity *d) {}
        virtual void gameover() {}
    };

	struct defaultclientmode : clientmode
	{
		defaultclientmode() {}
		~defaultclientmode() {}

		//find a spawn points for serveral client types
		void pickspawn(DynamicEntity *d);
	};
}

#endif /* __CLIENTMODE_H__ */
