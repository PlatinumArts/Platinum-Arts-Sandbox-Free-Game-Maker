#include "krsgame.h"
using namespace entities;

namespace game
{
	string clientmap;
	const char *getclientmap() { return clientmap; }

	bool connected = false;
	bool intermission = false;

	int gamemode;

	void gamedisconnect(bool cleanup) {connected = false;}
	void gameconnect(bool remote)     {connected = true;}

	vector<const char *> botnames;
	vector<vehicle *> vehicles;
	vector<krsai *> bots;

	VARP(debug, 0, 1, 1);
	VARR(dirtmap, 0, 0, 1);
	SVARFP(name, "noname", player1->name = newstring(name));
	VARFP(playermodel, 0, 0, 1, player1->playerindex = playermodel);

	krschar *player1 = new krschar();// our player

	krschar *spawnstate(krschar *d)
	{
		d->respawn();
		return d;
	}

	void spawnplayer(krschar *d)
	{
		findplayerspawn(d, -1, 0);
		spawnstate(d);
		d->state = d==player1 && editmode ? CS_EDITING : CS_ALIVE;
	}

	void respawnself()
	{
		if(paused || ispaused()) return;
		if(gamemode == MODE_BUMPER) return;

		spawnplayer(player1);
	}

	bool canjump()
	{
		if(player1->state == CS_ALIVE && !player1->isinvehicle)
			return true;
		return false;
	}

	/*controls*/
	ICOMMAND(accelerate, "D", (int *down),
	{
		player1->k_up = *down!=0;
		if(player1->gear == GEAR_REVERSE)
			player1->gear = GEAR_FIRST;
	});
	ICOMMAND(reverse, "D", (int *down),
	{
		player1->k_down = *down!=0;
		player1->gear = GEAR_REVERSE;
	});
	ICOMMAND(jump, "D", (int *down),
	{
		if(!*down || canjump())
		player1->jumping = *down!=0;
	});
	ICOMMAND(usenos, "", (),
	{
		if(!player1->onnos && player1->hasnos && player1->isinvehicle)
		{
			player1->onnos = true;
			player1->hasnos = false;
			playsound(S_USENOS, NULL);
			player1->lastnosmillis = lastmillis;
		}
	});
	ICOMMAND(action, "", (),
	{
		if(player1->isinvehicle)
			player1->isinvehicle = false;
		else
			player1->isinvehicle = true;
	});

	ICOMMAND(givenos, "s", (), player1->hasnos = true);

	ICOMMAND(steerleft,  "D", (int *down),player1->k_left =  *down != 0);
	ICOMMAND(steerright, "D", (int *down),player1->k_right = *down != 0);
	ICOMMAND(restart, "", (), spawnplayer(player1));

	///updates
	void updategears(krschar *d)
	{
		if(d->vehiclechanged && d->isinvehicle)
		{
			vehicle *vehiclelookup = vehicles[d->vehicleindex];
			loopi(6)
			{
				d->gears[i] = vehiclelookup->maxspeed*(i+1)/6;
				if(debug) conoutf("Assigned Gears: %d", d->gears[i]);
			}
			d->maxspeed = d->gears[0];
			d->vehiclechanged = false;
		}
	}

	ICOMMAND(mydamage, "i", (int *i), player1->damage = *i);

	void updatedamage(krschar *d)
	{
		if(!d->isinvehicle) return;

		if(d->damage >= 50)
			rendersmoke(d);

		if(d->damage >= 80)
		{
			if(d->onfire)
				renderflame(d);
			else
			{
				d->onfire = true;
				d->lastfiremillis = lastmillis;
			}

			if(lastmillis > d->lastfiremillis + FIRETIME && d->onfire)
			{
				if(d==player1)
					respawnself();
				else
				{
					krsai *pl = (krsai *)d;
					pl->state = CS_DEAD;
				}
			}
		}
	}

	void updatespeed(krschar *d)
	{
		if(!d->isinvehicle)
			d->maxspeed = 100;
		else
		{
			if(totalmillis > d->lastaccelmillis + ACCELTIME && d->gear != GEAR_REVERSE)
			{
				d->gear++;
				d->gear = clamp((int)GEAR_FIFTH, (int)GEAR_FIRST, d->gear);
				d->lastaccelmillis = lastmillis;
			}

			if(!d->onnos && !d->hasflat)
				d->maxspeed = d->gears[d->gear];

			else
			{
				if(d->hasflat)
					d->maxspeed = d->gears[0] * 0.5;

				else
				{
					if(lastmillis > d->lastnosmillis+NOSTIME)
						d->onnos = false;
					else
						d->maxspeed = d->gears[GEAR_FIFTH]*1.5f;
				}
			}
		}
	}

	void updateorientation(krschar *d)
	{
		if(!d->isinvehicle) return;
		if(d->timeinair)
		{
			d->pitch -= curtime / 20.0f;
			d->pitch = max<float>(-90, d->pitch);
		}
		else if(!d->floor.iszero())
		{
			if(d==player1)
			{
				vec dir = vec(d->floor).rotate(PI / 2.0f, vec((d->yaw + 90) * RAD, 0));

				float yaw, newpitch;
				vectoyawpitch(dir, yaw, newpitch);

				yaw = min<float>(1, curtime / 2000.0f  + curtime * d->vel.magnitude() / 40000.0f);
				d->pitch = (d->pitch + 90) * (1 - yaw) + (newpitch + 90) * (yaw) - 90;
			}
		}
	}

	void updateplayer()
	{
		if(editmode)
		{
			player1->maxspeed = 100;
			moveplayer(player1, 10, true);
			return;
		}

		updatedamage(player1);
		updategears(player1);
		updatespeed(player1);
		checkitems(player1);
		moveplayer(player1, 10, true);

		if(!(player1->k_up ^ player1->k_down))
			player1->move = 0;
		else if(player1->k_up)
			player1->move = 1;
		else
			player1->move = -1;

		if(player1->k_left ^ player1->k_right)
		{
			player1->yaw += (player1->k_left ? -1 : 1) *
			curtime / 500.0f * min(60.0f, player1->vel.magnitude());
		}

		updateorientation(player1);
	}

	void updateworld() //main game update loop
	{
		if(!curtime) return;

		physicsframe();
		updateplayer();

		bots::updatebots();
		ai::trydrop();

		if(editmode)
			intermission = false;
		else
			disablezoom();
	}

	bool mousemove(int &dx, int &dy, float &cursens)
	{
		if(editmode || !player1->isinvehicle)
			return false;

		return true;
	}

	bool recomputecamera(physent *&camera1, physent &tempcamera, bool &detachedcamera, float &thirdpersondistance)
	{
		if(player1->state == CS_DEAD || editmode || !thirdperson || !player1->isinvehicle)
			return false;

		detachedcamera = true;
		camera1 = &tempcamera;

		float mult = min<float>(1, 2 * curtime / 200.0f);

		vec pos = vec(0, 0, thirdpersondistance * .4).add(player1->o).sub(vec(player1->yaw * RAD, -15 * RAD).mul(thirdpersondistance));
		tempcamera.o.mul(1 - mult).add(pos.mul(mult));

		tempcamera.yaw = player1->yaw;
		tempcamera.pitch = -15;

		return true;
	}

	///vehicle oriented stuff
	void assignvehicle(krschar *d, vehicle &car)
	{
		if(debug)
		{
			conoutf(
			"DEBUG: assigned vehicle: \"%s\", using vehicle model: \"packages/models/vehicles/%s\"",
			car.name, car.mdl
			);
		}

		d->vehiclechanged = true;
		d->jumpvel = car.jumpvel;

		updategears(d);
	}


	///engine specific
	void setwindowcaption()
	{
		defformatstring(capt)("SandBox %s: Vehicle Simulator - %s", version, getclientmap()[0] ? getclientmap() : "[new map]");
		SDL_WM_SetCaption(capt, NULL);
	}

	void startmap(const char *name)
	{
		lastpickupmillis = -30000;
		copystring(clientmap, name ? name : "");
		intermission = false;
		findplayerspawn(player1, -1);

		ai::clearwaypoints();
		ai::loadwaypoints(name);

		bots::setupbots();

		if(identexists("mapstart"))
			execute("mapstart");
	}

	void edittoggled(bool on)
	{
		if(!on)
		{
			bots.deletecontents();
			bots::setupbots();
		}
	}

	void physicstrigger(physent *d, bool local, int floorlevel, int waterlevel, int material)
	{
		if(d->state!=CS_ALIVE||d->type==ENT_INANIMATE) return;
		switch(material)
		{
			case MAT_LAVA:
				if (waterlevel==0) break;
				playsound(S_BURN, d==player1 ? NULL : &d->o);
				loopi(60)
				{
					vec o = d->o;
					o.z -= d->eyeheight *i/60.0f;
					regular_particle_flame(PART_FLAME, o, 6, 2, 0x903020, 3, 2.0f);
					regular_particle_flame(PART_SMOKE, vec(o.x, o.y, o.z + 8.0f), 6, 2, 0x303020, 1, 4.0f, 100.0f, 2000.0f, -20);
				}
				if(d==player1) respawnself();
				else
				{
					 krsai *pl = (krsai *)d;
					 pl->state = CS_DEAD;
				}
				break;
			case MAT_WATER:
				if (waterlevel==0) break;
				playsound(waterlevel > 0 ? S_SPLASH_IN : S_SPLASH_OUT , d==player1 ? NULL : &d->o);
				particle_splash(PART_WATER, 200, 200, d->o, (watercolor.x<<16) | (watercolor.y<<8) | watercolor.z, 0.5);
				d->maxspeed = 0;
				break;
			default:
				if (floorlevel==0) break;
				playsound(floorlevel > 0 ? S_JUMP : S_LAND, local ? NULL : &d->o);
				break;
		}
	}

	void openworld(const char *name, bool fall = false)
	{
		assignvehicle(player1, *vehicles[player1->vehicleindex]);
		if(!connected)
			localconnect();
		if(name && *name && load_world(name))
			return;
		//else if(fall && load_world(DEFAULTMAP))
		//	return;
		else
			emptymap(10, true, (name && *name) ? name : NULL);
	}
	ICOMMAND(map, "s", (char *s), openworld(s))
	
	ICOMMAND(changevehicle, "i", (int *index),
	{
		vehicle *vehiclelookup = vehicles[*index];
		
		if(vehiclelookup && player1->hasvehicle[*index])
		{
			assignvehicle(player1, *vehiclelookup);
			player1->vehicleindex = *index;
		}
	});

	void initclient()
	{
		if(!player1->name)
			player1->name = name;

		clientmap[0] = 0;
	}

	int numdynents(){ return 1+bots.length();}

	dynent *iterdynents(int i)
	{
		if(!i)
			return player1;
		i--;
		if(i<bots.length())
			return bots[i];
		return NULL;
	}

	void changemap(const char *name) { openworld(name, true); }
	void forceedit(const char *name) { openworld(name); }

	const char *gameident() { return "krs"; }
	const char *autoexec() { return "autoexec.cfg"; }
	const char *savedservers() { return NULL; }
}
