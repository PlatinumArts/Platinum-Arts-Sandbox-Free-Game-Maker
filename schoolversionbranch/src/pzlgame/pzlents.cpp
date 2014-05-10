#include "pzlgame.h"

namespace entities
{
	int lastpickupmillis;

	vector<extentity *> ents;
	vector<extentity *> &getents() { return ents; }

	extentity *newentity() { return new pzlentity();}

	void deleteentity(extentity *e) { delete (pzlentity *)e;}
	void fixentity(entity &e){}

	const char *entmodel(const entity &e)
	{
		const char *mdl[] =
		{
			NULL, //NOTUSED     - ET_EMPTY     = 0
			NULL, //LIGHT       - ET_LIGHT     = 1
			NULL, //MAPMODEL    - ET_MAPMODEL  = 2
			NULL, //PLAYERSTART - ET_NULL      = 3
			NULL, //ENVMAP      - ET_ENVMAP    = 4
			NULL, //PARTICLES   - ET_PARTICLES = 5
			NULL, //MAPSOUND    - ET_SOUND     = 6
			NULL  //SPOTLIGHT   - ET_SPOTLIGHT = 7
		};
		return mdl[e.type];
	}

	void renderent(extentity &e, const char *mdlname, float z, float yaw, int anim = ANIM_MAPMODEL)
	{
		if(!mdlname) return;

		rendermodel(&e.light, mdlname, anim|ANIM_LOOP, vec(e.o).add(vec(0, 0, z)), yaw, 0,
			MDL_SHADOW|MDL_CULL_VFC|MDL_CULL_DIST|MDL_CULL_OCCLUDED
		);
	}

	void renderentities()
	{
		loopv(ents)
		{
			extentity &e = *ents[i];
			switch(e.type)
			{
				default:
					continue;
			}
		}
	}

	void animatemapmodel(const extentity &e, int &anim, int &basetime)
	{
		anim = ANIM_MAPMODEL|ANIM_LOOP;
	}

	void trypickup(int n, pzlchar *d)
	{
		extentity &e = *ents[n];
		switch(e.type) {}
	}

	const char *entname(int i)
	{
		static const char *entnames[] =
		{
			"none?", "light", "mapmodel", "playerstart", "envmap", "particles", "sound", "spotlight"
		};
		return i>=0 && size_t(i)<sizeof(entnames)/sizeof(entnames[0]) ? entnames[i] : "";
	}

	void checkitems(pzlchar *d)
	{
		vec o = d->o;
		o.z -= d->eyeheight;

		loopv(ents)
		{
			extentity &e = *ents[i];
			if(e.type==NOTUSED)
                continue;
			if(lastmillis > lastpickupmillis + ENTSPAWNTIME && !e.spawned)
				e.spawned = true;
		}
	}

	const int numattrs(int type)
	{
		static const int num[] = {};

		type -= ET_GAMESPECIFIC;
		return type >= 0 && size_t(type) < sizeof(num)/sizeof(num[0]) ? num[type] : 0;
	}

	float dropheight(entity &e)
	{
		if(e.type==MAPMODEL) return 0.0f;
		return 4.0f;
	}

	void clearents()
	{
		while(ents.length()) deleteentity(ents.pop());
	}
}
