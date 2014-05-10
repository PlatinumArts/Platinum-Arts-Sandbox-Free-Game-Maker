#include "krsgame.h"

namespace bots
{
	using namespace ai;

	void followroute(krsai *d)
	{
		if(!d->route.length())
		{
			waypoint *closest = ai::closestwaypoint(d->feetpos());
			ai::findroute(closest - ai::waypoints.getbuf(), rnd(ai::waypoints.length()), d->route);
			return;
		}

		int ind;
		waypoint *closest = ai::closestwaypoint(d->feetpos());
		if(!closest) // this should not happen
		{
			d->route.setsize(0);
			return;
		}

		ind = closest - ai::waypoints.getbuf();

		if(ind == d->route[0] && closest->o.dist(d->feetpos()) < 8)
			d->route.remove(0);

		d->move = 1;

		vec dir = vec(ai::waypoints[d->route[0]].o).sub(d->feetpos());
		vectoyawpitch(dir, d->yaw, d->pitch);

		game::updateorientation(d);
	}

	void updatemove(krsai *d)
	{
		if(d->timeinair)
			d->route.setsize(0);

		if(!d->onnos && d->hasnos && d->isinvehicle)
		{
			d->onnos = true;
			d->hasnos = false;
			playsound(S_USENOS, NULL);
			d->lastnosmillis = lastmillis;
		}

		waypoint *closest = ai::closestwaypoint(d->feetpos());

		if(closest && closest->o.dist(d->feetpos()) < 20 && !d->route.length())
			ai::findroute(closest - ai::waypoints.getbuf(), rnd(ai::waypoints.length()), d->route);
		else
			followroute(d);

		game::updategears(d);
		game::updatespeed(d);
		moveplayer(d, 10, false);
	}

	void setupbots()
	{
		loopv(entities::ents)
		{
			extentity &e = *entities::ents[i];

			if(e.type == BOT)
			{
				if(!ai::waypoints.length())
				{
					conoutf("cannot load bots, map contains no waypoints..");
					break;
				}
				krsai *newbot = new krsai;

				newbot->type = ENT_PLAYER;
				newbot->name = newstring(game::botnames[rnd(game::botnames.length())]);
				newbot->rank = e.attr[0];

				switch(e.attr[0])
				{
					case BOT_VEHICLE:
						game::assignvehicle(newbot, *game::vehicles[rnd(game::vehicles.length())]);
						newbot->hasname = false;
						break;
					case BOT_POLICE:
						game::assignvehicle(newbot, *game::vehicles[2]);
						newbot->hasname = false;
						break;
					case BOT_OPPONENT:
						game::assignvehicle(newbot, *game::vehicles[rnd(game::vehicles.length())]);
						newbot->hasname = true;
						break;
					case BOT_CIVILIAN:
					default:
						newbot->vehiclechanged = false;
						newbot->isinvehicle = false;
						newbot->playerindex = rnd(1);
						newbot->hasname = false;
						newbot->rank = BOT_CIVILIAN;
						break;
				}

				e.spawned = true;
				game::bots.add(newbot);
				newbot->o = newbot->newpos = vec(e.o).add(vec(0, 0, newbot->eyeheight));
			}
		}
	}

	void updatebots()
	{
		loopv(game::bots)
		{
			krsai *bot = game::bots[i];

			entities::checkitems(bot);
			updatemove(bot);
		}
	}
}
