#include "krsgame.h"

namespace entities
{
	int lastpickupmillis;

	vector<extentity *> ents;
	vector<extentity *> &getents() { return ents; }

	extentity *newentity() { return new krsentity();}

	void deleteentity(extentity *e) { delete (krsentity *)e;}
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
			NULL, //SPOTLIGHT   - ET_SPOTLIGHT = 7

			NULL,                 //PLAYERFINISH, = 8
			"rpg/objects/coin",   //MONEY         = 9
			"hirato/box/standard",//NOS           = 10
			NULL,                 //SPIKESTRIP    = 11
			NULL,                 //VEHICLE       = 12
			NULL                  //BOT           = 13
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
				case MONEY:
				{
					if(e.spawned)
						renderent(e, entmodel(e), (float)(1+sin(lastmillis/100.0+e.o.x+e.o.y)/20), lastmillis/10.0f);
					continue;
				}
				case NOS:
				{
					if(e.spawned)
						renderent(e, entmodel(e), 0, e.attr[0], ANIM_IDLE);
					continue;
				}
				case SPIKESTRIP:
				{
					renderent(e, entmodel(e), 0, e.attr[0], ANIM_IDLE);
					continue;
				}
				case VEHICLE:
				{
					if(e.spawned)
					{
						if(game::vehicles.inrange(e.attr[0]))
						{
							defformatstring(mdl)("vehicles/%s", game::vehicles[e.attr[0]]->mdl);
							renderent(e, mdl, 0, 0, ANIM_IDLE);
						}
					}
					continue;
				}
				case BOT:
				{
					if(e.spawned)
					{
						loopv(game::bots)
						{
							krsai *bot = game::bots[i];
							game::renderplayer(bot);
						}
					}
				}
				default:
					continue;
			}
		}
	}

	void animatemapmodel(const extentity &e, int &anim, int &basetime)
	{
		anim = ANIM_MAPMODEL|ANIM_LOOP;
	}

	void trypickup(int n, krschar *d)
	{
		extentity &e = *ents[n];
		switch(e.type)
		{
			case PLAYERFINISH:
			{
				conoutf("game finished");
				playsound(S_GAMEFINISHED);
				game::spawnplayer(d);
				break;
			}

			case MONEY:
			{
				d->money += e.attr[0];
				playsound(S_RECICVEMONEY, &ents[n]->o);
				lastpickupmillis = lastmillis;
				e.spawned = false;
				break;
			}

			case NOS:
			{
				if((!d->hasnos || !d->onnos) && d->isinvehicle)
				{
					d->hasnos = true;
					lastpickupmillis = lastmillis;
					e.spawned = false;
				}
				break;
			}

			case SPIKESTRIP:
			{
				if(!d->hasflat && d->isinvehicle)
				{
					playsound(S_TIREPOP, &ents[n]->o);
					d->hasflat = true;
				}
				break;
			}

			case VEHICLE:
			{
				if(!d->isinvehicle && game::vehicles.inrange(e.attr[0]))
				{
					d->vehiclechanged = true;
					d->vehicleindex = e.attr[0];
					d->isinvehicle = true;
					e.spawned = false;
				}
				break;
			}
		}
	}

	const char *entname(int i)
	{
		static const char *entnames[] =
		{
			"none?", "light", "mapmodel", "playerstart", "envmap", "particles", "sound", "spotlight",
			"playerfinish", "money", "nos", "spikestrip", "vehicle", "bot"
		};
		return i>=0 && size_t(i)<sizeof(entnames)/sizeof(entnames[0]) ? entnames[i] : "";
	}

	void checkitems(krschar *d)
	{
		vec o = d->o;
		o.z -= d->eyeheight;

		loopv(ents)
		{
			extentity &e = *ents[i];
			if(e.type==NOTUSED) continue;
			float dist = e.o.dist(o);

			if(lastmillis > lastpickupmillis + ENTTIME && !e.spawned)
				e.spawned = true;

			if((e.spawned || e.type == PLAYERFINISH || e.type == SPIKESTRIP) && dist<=15)
				trypickup(i, d);
		}
	}

	const int numattrs(int type)
	{
		static const int num[] =
		{
			0, //playerfinish 8
			1, //money        9
			1, //nos          10
			1, //spikestrip   11
			1, //vehicle      13
			1  //bot          14
		};

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
};
