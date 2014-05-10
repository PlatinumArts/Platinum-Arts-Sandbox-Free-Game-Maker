#include "krsgame.h"

namespace game
{
	VARP(shownames, 0, 1, 1);

	const char *playermodels(int mdl)
	{
		const char *mdls[] =
		{
			"rc", "ogre"
		};

		return mdls[mdl];
	};

	const char *getvehiclemodel(krschar *d)
	{
		vehicle *vehiclelookup = vehicles[d->vehicleindex];
		if(vehiclelookup)
			return vehiclelookup->mdl;
		return NULL;
	}

	void renderplayer(krschar *d)
	{
		if(d->isinvehicle)
		{
			defformatstring(mdl)("vehicles/%s", getvehiclemodel(d));
			setbbfrommodel(d, mdl);
			renderclient(d, mdl, NULL, ANIM_IDLE|ANIM_LOOP, 0, 0, 0, 0, 1, false);
		}
		else
			renderclient(d, playermodels(d->playerindex), NULL, ANIM_IDLE|ANIM_LOOP, 0, 0, 0, 0, 1, false);
	}

	void rendersmoke(krschar *d)
	{
		vec o = d->o;
		o.z -= d->eyeheight / 2.5f;

		regular_particle_flame(PART_SMOKE, vec(o.x, o.y, o.z), 6, 2, 0x303020, 1, 4.0f, 100.0f, 2000.0f, -20);
	}

	void renderflame(krschar *d)
	{
		vec o = d->o;
		o.z -= d->eyeheight /2.5f;

		regular_particle_flame(PART_FLAME, vec(o.x, o.y, o.z), 6, 2, 0x903020, 3, 2.0f);
	}

	void rendergame(bool mainpass)
	{
		startmodelbatches();

		if(isthirdperson())
			renderplayer(game::player1);

		entities::renderentities();

		endmodelbatches();

		if(mainpass)
		{
			if(debug)
			{
				//draw player vecors
				if(!editmode && thirdperson)
				{
					if(player1->isinvehicle)
					{
						vec origin = player1->o; origin.z -= player1->eyeheight / 2.0f;
						particle_flare(origin, vec(player1->floor).mul(24).add(origin), 1, PART_STREAK, 0x0000FF);
						particle_flare(origin, vec(player1->yaw * RAD, 0).mul(24).add(origin), 1, PART_STREAK, 0xFF0000);
						particle_flare(origin, vec(player1->floor).rotate(PI / 2.0f, vec((player1->yaw + 90) * RAD, 0)).mul(24).add(origin), 1, PART_STREAK, 0x00FF00);
					}
				}
				loopv(game::bots)
				{
					krsai *d = bots[i];

					if(d->isinvehicle)
					{
						vec origin = d->o;
						origin.z -= d->eyeheight / 2.0f;

						particle_flare(origin, vec(d->floor).mul(24).add(origin), 1, PART_STREAK, 0x0000FF);
						particle_flare(origin, vec(d->yaw * RAD, 0).mul(24).add(origin), 1, PART_STREAK, 0xFF0000);
						particle_flare(origin, vec(d->floor).rotate(PI / 2.0f, vec((d->yaw + 90) * RAD, 0)).mul(24).add(origin), 1, PART_STREAK, 0x00FF00);
					}
				}
				ai::renderwaypoints();
			}

			if(shownames)
			{
				loopv(game::bots)
				{
					krsai *d = bots[i];
					if(d->hasname)
						particle_textcopy(d->abovehead(), d->name, PART_TEXT, 1, 0xFFFFFF, 4);
				}

				particle_textcopy(player1->abovehead(), player1->name, PART_TEXT, 1, 0xFFFFFF, 4);
			}

			loopv(game::bots)
			{
				krsai *d = bots[i];

				if(d->isinvehicle && d->onnos && d->move)
				{
					vec origin = d->o;
					origin.z -= d->eyeheight / 4.4f;

					particle_splash(PART_WATER, 500, 500, origin, 0xC0C0C0, 4.221);
				}
			}

			if(!editmode && player1->isinvehicle && player1->onnos && player1->move)
			{
				vec origin = player1->o;
				origin.z -= player1->eyeheight / 4.4f;

				particle_splash(PART_WATER, 500, 500, origin, 0xC0C0C0, 4.221);
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
