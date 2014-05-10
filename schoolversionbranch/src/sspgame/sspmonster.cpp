#include "sspgame.h"

namespace game
{
	vector<int> intents; //INTeractive ENTities

	vector<enemy *> monstertypes;

	ICOMMAND(addmonster, "siiii", (char *s, int *t, int *x, int *y, int *z), {monstertypes.add(new enemy(s, *t, *x, *y, *z));})
	ICOMMAND(resetmonsters, "", (), monstertypes.deletecontents(); )


	void initialisemonsters()
	{
		intents.setsize(0);

		loopv(entities::ents)
		{
			extentity &e = *entities::ents[i];
			if(e.type==ENEMY)
			{
				if(monstertypes.inrange(e.attr[1]))
				{
					sspmonster *mon = new sspmonster(e, *monstertypes[e.attr[1]]);
					sspobjs.add(mon);
				}
			}
			if(e.type==TELEPORT || e.type==JUMPPAD)
			{
				intents.add(i);
			}
		}
	}

	void checkmonsteritems(sspmonster *d)
	{
		loopv(intents)
		{
			if(d->lastpickupmillis+500 > lastmillis) break;
			entity &e = *entities::ents[intents[i]];

			float dist = e.o.dist(d->o);

			if(e.type==TELEPORT && dist <= (e.attr[1] ? abs(e.attr[1]) : 16))
				entities::trypickup(intents[i], d);
			if(e.type==JUMPPAD && dist <= (e.attr[3] ? abs(e.attr[3]) : 12))
				entities::trypickup(intents[i], d);
		}
	}

	void writemonsters(stream *f)
	{
		f->printf("resetmonsters\n\n");
		loopv(monstertypes)
		{
			enemy &m = *monstertypes[i];
			f->printf("addmonster %s %i %i %i %i //%i\n", escapestring(m.mdl), m.health, m.speed, m.painsound, m.diesound, i);
		}
	}
}
