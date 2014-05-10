#include "sspgame.h"

namespace game
{
	void rendergame(bool mainpass)
	{
		startmodelbatches();

		loopv(sspobjs)
			sspobjs[i]->render();

		loopv(entities::platforms)
			entities::platforms[i]->render();

		loopv(entities::items)
			entities::items[i]->render();

		entities::renderentities();

		renderprojs();
		endmodelbatches();

		if(mainpass && debug)
		{
			loopv(sspobjs)
			{
				sspent *o = sspobjs[i];
				defformatstring(ds)("%p", o);
				particle_textcopy(o->abovehead(), ds, PART_TEXT, 5, 0xFFFFFF, 4);
			}
			loopv(entities::platforms)
			{
				platform *o = entities::platforms[i];
				defformatstring(ds)("%p", o);
				particle_textcopy(o->abovehead(), ds, PART_TEXT, 5, 0xFFFFFF, 4);
			}
			loopv(entities::items)
			{
				sspitem *o = entities::items[i];
				defformatstring(ds)("%p", o);
				particle_textcopy(vec(o->o).add(vec(0, 0, 4)), ds, PART_TEXT, 5, 0xFFFFFF, 4);
			}
		}
	}

	const char *animname(int i)
	{
		i &= 0x7F;
		if(i >= NUMANIMS) return "";
		return animnames[i];
	}

	const int numanims() {return NUMANIMS;}
}
