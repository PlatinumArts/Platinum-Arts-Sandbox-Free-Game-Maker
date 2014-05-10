#include "sspgame.h"

namespace entities
{
	vector<extentity *> ents;

	vector<extentity *> &getents() { return ents; }
	extentity *newentity() { return new sspentity(); }
	void deleteentity(extentity *e) { delete (sspentity *)e; }

	vector<sspitem *> items;
	vector<platform *> platforms;

	const char *entmodel(const entity &e)
	{
		switch(e.type)
		{
			case TELEPORT:
				return mapmodelname(e.attr[2]);
				break;
			default: return NULL;
		}
	}

	void animatemapmodel(const extentity &e, int &anim, int &basetime)
	{
		anim = ANIM_MAPMODEL|ANIM_LOOP;
	}

	int testdist(extentity &e)
	{
		switch(e.type)
		{
			case TELEPORT:
			case CHECKPOINT:
				return e.attr[1] != 0 ? abs(e.attr[1]) : 16;
			case JUMPPAD:
				return e.attr[3] != 0 ? abs(e.attr[3]) : 12;
			case AXIS:
				return e.attr[3] != 0 ? abs(e.attr[3]) : 16;
			default:
				return 24;
		}
	}

	int pickupdist()
	{
		if(game::pickups.inrange(game::player1->armourvec) && game::player1->armour==ARM_ATTRACT)
			return 48;
		else
			return 12; //quadruple distance for attractive armour
	}

	void fixentity(extentity &e)
	{
		switch(e.type)
		{
			case BOX:
			case ENEMY:
			case TELEDEST:
			case CHECKPOINT:
			case PLATFORM:
			{
				e.attr.pop();
				e.attr.insert(0, game::player1->yaw);
				break;
			}
		}
	}

	bool teleport(sspent *d, int dest)
	{
		loopv(ents)
		{
			extentity &e = *ents[i];
			if(e.type==TELEDEST && e.attr[1] == dest)
			{
				d->o = d->newpos = vec(e.o).add(vec(0, 0, d->eyeheight));
				d->vel = d->falling = vec(0,0,0);
				d->yaw = abs(e.attr[0]) % 360;
				particle_splash(PART_EDIT, 1250, 750, e.o, d==game::player1 ? 0x0000FF : 0xFF0000, 1.0f, 150, 0x7FFFFF);
				playsound(S_TELEPORT, &e.o);
				return true;

			}
		}
		conoutf("no such teledest: %i", dest);
		return false;
	}

	bool pickuppowerup(sspchar *d, sspitem *i)
	{
		if(d != game::player1 || !game::pickups.inrange(i->index)) return false;  //we currently only want the player to be able to pickup these powerups

		vec emit = i->o;
		emit.z += 5;

		string ds;
		bool pickedup = false;
		///WARNING only use the valid mutations, or there WILL be SERIOUS consequences!
		pickup *p = game::pickups[i->index];

		pickup_generic *gp = (pickup_generic *) p;
		pickup_armour *ap = (pickup_armour *) p;
		//pickup_weapon *wp = (pickup_weapon *) p; ///UNUSED

		switch(p->type)
		{
			case PICKUP_COIN:
				pickedup = true;
				d->coins += gp->amount;

				while(d->coins >= 100 && d->lives < 99)
				{
					d->coins -= 100;
					d->lives += 1;
				}

				formatstring(ds)("%i coin%s", gp->amount, gp->amount==1 ? "" : "s");
				particle_textcopy(emit, ds, PART_TEXT, 2000, 0xffd700, 8.0f, -8);
				break;

			case PICKUP_HEALTH:
				if(gp->amount > 0 && d->maxhealth <= d->health) break; //keeps useful pickups from dissapearing, if you've no need of them yet

				pickedup = true;
				d->health = min(d->maxhealth, d->health + gp->amount); //fully heals players or hurts
				if(gp->amount<0) d->takedamage(-1 * gp->amount, false); //my tummy hurts :(

				formatstring(ds)("%i HP", gp->amount);
				particle_textcopy(emit, ds, PART_TEXT, 2000, gp->amount >= 0 ? 0x00FF00 : 0xFF0000, 8.0f, -8);
				break;

			case PICKUP_TIME:
				pickedup = true;
				game::secsallowed += gp->amount;
				formatstring(ds)("%i second%s", gp->amount, gp->amount==1 ? "" : "s");
				particle_textcopy(emit, ds, PART_TEXT, 2000, gp->amount >= 0 ? 0xAFAFAF : 0xFF0000, 8.0f, -8);
				break;

			case PICKUP_LIVES:
				if(gp->amount <= 0 || game::player1->lives != 99)
				{
					pickedup = true;
					d->lives = min(99, d->lives + gp->amount);
					formatstring(ds)("%i %s", gp->amount, gp->amount>1 ? "lives" : "life");
					particle_textcopy(emit, ds, PART_TEXT, 2000, 0xffd700, 8.0f, -8);
				}
				break;

			case PICKUP_WEAPON:
				pickedup = true;
				d->gunselect = i->index; //set the player equipped weapon to the valid part of the vector for fire sound and projectile information
				break;

			case PICKUP_ARMOUR:
			{
				pickedup = true;
				d->armourvec = i->index; //just for the appearance
				d->armour = ap->armour;
				const char *type[ARM_MAX] = {"", "Plain", "Attractive", "Winged", "Spiked", "Double Jump"};
				formatstring(ds)("%s Armour", type[ap->armour]);
				particle_textcopy(emit, ds, PART_TEXT, 2000, 0x007FFF, 8.0f, -8);
			}
				break;

			default:
				break;
		}
		if (pickedup)
		{
			particle_splash(PART_STEAM, 200, 200, emit, 0xCFCFCF, 1, 150, -10);

			defformatstring(script)("pickup_%i", i->script);
			if(identexists(script)) execute(script);
		}
		return pickedup;
	}

	void trypickup(int n, sspchar *d)
	{
		switch(ents[n]->type)
		{
			case TELEPORT:
				if(lastmillis < d->lastpickupmillis + 200) return;
				if(teleport(d, ents[n]->attr[0]))
					playsound(S_TELEPORT, &ents[n]->o);
				d->lastpickupmillis  = lastmillis;
				return;
			case JUMPPAD:
				if(lastmillis < d->lastpickupmillis + 200) return;
				d->falling = vec(0, 0, 0);
				d->vel.z = ents[n]->attr[0] * 10;
				d->vel.y += ents[n]->attr[1] * 10;
				d->vel.x += ents[n]->attr[2] * 10;
				playsound(S_JUMPPAD, &ents[n]->o);
				d->lastpickupmillis = lastmillis;
				return;
			case CHECKPOINT:
			{
				string ds;
				vec emit = ents[n]->o;
				emit.z += 5;
				switch(ents[n]->attr[3])
				{
					case CP_END:
						game::intermission = true;
						formatstring(ds)("Level Completed!");
						break;
					case CP_SAVE:
					default:
						if(n == d->checkpoint)
							return;
						formatstring(ds)("Checkpoint!");
						d->checkpoint = n;
						break;
				}
				particle_textcopy(emit, ds, PART_TEXT, 2000, 0x7FFF7F, 8.0f, -8);
				game::player1->lastpickupmillis  = lastmillis;

				formatstring(ds)("checkpoint_%i", ents[n]->attr[4]);
				if(identexists(ds)) execute(ds);

				return;
			}
		}
	}

	void checkitems(sspchar *d)
	{
		if(d != game::player1 || editmode) return;
		vec o = d->o;
		o.z -= d->eyeheight;
		loopv(ents)
		{
			extentity &e = *ents[i];
			if(e.type==NOTUSED) continue;
			if(!e.spawned && e.type!=TELEPORT && e.type!=JUMPPAD && e.type!=CHECKPOINT) continue;
			float dist = e.o.dist(o);
			if(dist<testdist(e)) trypickup(i, d);
		}
	}

	void renderent(extentity &e, const char *mdlname, float z, float yaw, int anim = ANIM_MAPMODEL)
	{
		if(!mdlname) return;
		rendermodel(&e.light, mdlname, anim|ANIM_LOOP, vec(e.o).add(vec(0, 0, z)), yaw, 0, 0, MDL_SHADOW | MDL_CULL_VFC | MDL_CULL_DIST | MDL_CULL_OCCLUDED);
	}

	void renderentities()
	{
		loopv(ents)
		{
			extentity &e = *ents[i];
			switch(e.type)
			{
				case TELEPORT:
					renderent(e, mapmodelname(e.attr[2]), (float)(1+sin(lastmillis/100.0+e.o.x+e.o.y)/20), lastmillis/10.0f);
					continue;
				case CHECKPOINT:
					renderent(e, mapmodelname(e.attr[2]), 0, e.attr[0], ANIM_IDLE);
					continue;
				case AXIS:
				{
					int radius = e.attr[3] ? abs(e.attr[3]) : 16;
					vec dir = vec(0, 0, 0);
					vecfromyawpitch(e.attr[0], 0, 1, 0, dir);
					dir.mul(radius);
					dir.add(e.o);
					particle_flare(e.o, dir, 1, PART_STREAK, 0x007FFF, 0.4);

					vecfromyawpitch(e.attr[1], 0, 1, 0, dir);
					dir.mul(radius);
					dir.add(e.o);
					particle_flare(e.o, dir, 1, PART_STREAK, 0x007FFF, 0.4);
					continue;
				}
				case PICKUP:
					if(editmode && game::pickups.inrange(e.attr[0]))
					{
						pickup *p = game::pickups[e.attr[0]];

						rendermodel(
							&e.light,
							p->mdl,
							ANIM_MAPMODEL|ANIM_LOOP,
							vec(e.o).add(vec(0, 0, (float)(1 + sin(lastmillis/300.0)))),
							lastmillis / 10.0f,
							0,
							0,
							MDL_SHADOW | MDL_CULL_VFC | MDL_CULL_DIST | MDL_CULL_OCCLUDED,
							NULL,
							NULL,
							0,
							0,
							0.5
						); //render a transparent model as a preview
					}
					continue;
				case BOX:
					if(editmode)
					{
						rendermodel(
							&e.light,
							mapmodelname(e.attr[1]),
							ANIM_MAPMODEL|ANIM_LOOP,
							e.o,
							0,
							0,
							0,
							MDL_SHADOW | MDL_CULL_VFC | MDL_CULL_DIST | MDL_CULL_OCCLUDED,
							NULL,
							NULL,
							0,
							0,
							0.5
						); //render a transparent model as a preview
					}
					continue;
				default:
					continue;
			}
		}
	}

	void entradius(extentity &e, bool &color)
	{
		switch(e.type)
		{
			case AXIS:
			{
				vec dir;

				vecfromyawpitch(e.attr[0], 0, 1, 0, dir);
				renderentarrow(e, dir, testdist(e));

				vecfromyawpitch(e.attr[1], 0, 1, 0, dir);
				renderentarrow(e, dir, testdist(e));

				break;
			}
			case CHECKPOINT:
				renderentsphere(e, testdist(e));
				break;
			case ENEMY:
			case TELEDEST:
			{
				vec dir;
				vecfromyawpitch(e.attr[0], 0, 1, 0, dir);
				renderentarrow(e, dir, 4);
				break;
			}
			case PLATFORM:
			{
				renderentattachment(e);
				vec dir;
				vecfromyawpitch(e.attr[0], 0, 1, 0, dir);
				renderentarrow(e, dir, 4);
				break;
			}
			case PLATFORMROUTE:
			loopv(ents)
			{
				extentity &o = *ents[i];
				if(o.type == PLATFORMROUTE && e.attr[1] == o.attr[0])
				{
					vec delta = vec(o.o).sub(e.o);
					float d = delta.magnitude();

					delta.normalize();
					renderentarrow(e, delta, d);
					break;
				}
			}
			break;

			case CAMERA:
			{
				vec dir;
				vecfromyawpitch(e.attr[1], e.attr[2], 1, 0, dir);
				renderentarrow(e, dir, 4);
				break;
			}
			case WAYPOINT:
				/* loopv(ents)
				{
					if(ents[i]->type == WAYPOINT && e.attr[1]==ents[i]->attr[1])
					{
						renderentarrow(e, vec(ents[i]->o).sub(e.o).normalize(), e.o.dist(ents[i]->o));
						break;
					}
				} */
				break;
			case TELEPORT:
				loopv(ents)
				{
					if(ents[i]->type == TELEDEST && e.attr[0]==ents[i]->attr[1])
					{
						renderentarrow(e, vec(ents[i]->o).sub(e.o).normalize(), e.o.dist(ents[i]->o));
						renderentsphere(e, testdist(e) );
						break;
					}
				}
                		break;
			case JUMPPAD:
				renderentarrow(e, vec((int)(char)e.attr[2]*10.0f, (int)(char)e.attr[1]*10.0f, e.attr[0]*12.5f).normalize(), testdist(e));
				renderentsphere(e, testdist(e));
				break;
		}
	}

	bool radiusent(extentity &e)
	{
		switch(e.type)
		{
			case LIGHT:
			case ENVMAP:
			case MAPSOUND:
			case JUMPPAD:
			case TELEPORT:
				return true;
				break;
			default:
				return false;
				break;
		}
	}

	bool dirent(extentity &e)
	{
		switch(e.type)
		{
			case AXIS:
			case MAPMODEL:
			case PLAYERSTART:
			case SPOTLIGHT:
			case ENEMY:
			case WAYPOINT:
			case TELEPORT:
			case TELEDEST:
			case CHECKPOINT:
			case JUMPPAD:
			case PLATFORM:
			case CAMERA:
				return true;
				break;
			default:
				return false;
				break;
		}
	}

	void prepareents()
	{
		items.deletecontents();
		platforms.deletecontents();
		loopv(ents)
		{
			extentity &e = *ents[i];
			switch(e.type)
			{
				case PICKUP:
				{
					sspitem *item = new sspitem();
					item->init(e.o, e.attr[0], e.attr[1], true, 0);
					items.add(item);
					continue;
				}
				case BOX:
				{
					if(!game::boxdefs.inrange(e.attr[2]))
						continue;

					sspbox *box = new sspbox();
					box->init(*game::boxdefs[e.attr[2]], e);

					game::sspobjs.add(box);
					continue;
				}
				case PLATFORM:
				{
					if(!e.attached) continue;
					platform *p = platforms.add(new platform(e.attr[1]));

					p->yaw = e.attr[0];
					p->speed = e.attr[2];
					p->o = e.attached->o;
					p->dest = e.attached;

					p->resetinterp();

					continue;
				}
			}
		}
	}

	bool printent(extentity &e, char *buf)
	{
		return false;
	}

	const char *entnameinfo(entity &e) { return ""; }
	int extraentinfosize() {return 0;}

	const char *entname(int i)
	{
		static const char *entnames[] =
		{
			"none?", "light", "mapmodel", "playerstart", "envmap", "particles", "sound", "spotlight", "box", "pickup",
			"enemy", "waypoint", "teleport", "teledest", "checkpoint", "jumppad", "platform", "platformroute", "camera", "axis", "", "", ""
		};
		return i>=0 && size_t(i)<sizeof(entnames)/sizeof(entnames[0]) ? entnames[i] : "";
	}

	const int numattrs(int type)
	{
		static const int num[] =
		{
			5, //box
			2, //pickup
			2, //enemy
			0, //waypoint
			3, //teleport
			2, //teledest
			5, //checkpoint
			4, //jumppad
			3, //platform
			2, //platformroute
			5, //camera
			5  //axis
		};

		type -= ET_GAMESPECIFIC;
		return type >= 0 && size_t(type) < sizeof(num)/sizeof(num[0]) ? num[type] : 0;
	}

	void renderhelpertext(extentity &e, int &colour, vec &pos, string &tmp)
	{
		switch(e.type)
		{
			case BOX:
			{
				pos.z += 7.5f;
				formatstring(tmp)("Yaw: %i\nModel: %s (%i)\nINV IDX: %i\nScript: box_%i\nFlags:",
					e.attr[0],
					mapmodelname(e.attr[1]), e.attr[1],
					e.attr[2],
					e.attr[4]
				);
				if(!(e.attr[3] & BOX_ALL))
					concatstring(tmp, " persist");
				else
				{
					if(e.attr[3] & BOX_DESTROY)
						concatstring(tmp, " destroy");
					if(e.attr[3] & BOX_PINJATA)
						concatstring(tmp, " pinjata");
					if(e.attr[3] & BOX_EXPLODE)
						concatstring(tmp, " explode");
				}
				return;
			}
			case PICKUP:
				if(game::pickups.inrange(e.attr[0]))
				{
					static const char *types[] = {"Coins", "Health", "Time", "Lives"};
					switch(game::pickups[e.attr[0]]->type)
					{
						case PICKUP_COIN:
						case PICKUP_HEALTH:
						case PICKUP_LIVES:
						case PICKUP_TIME:
						{
							pickup_generic *p = (pickup_generic *) game::pickups[e.attr[0]];
							pos.z += 6.0;
							formatstring(tmp)("Index: %i\nScript: pickup_%i\nType: %s\nStrength: %i", e.attr[0], e.attr[1], types[p->type], p->amount);
							break;
						}
						case PICKUP_WEAPON:
							pos.z += 4.5;
							formatstring(tmp)("Index: %i\nScript: pickup_%i\nType: Weapon", e.attr[0], e.attr[1]);
							break;
						case PICKUP_ARMOUR:
						{
							pickup_armour *ap = (pickup_armour *) game::pickups[e.attr[0]];
							static const char *armour[ARM_MAX] = {"", "Plain", "Attractive", "Winged", "Spike", "Double Jump"};
							pos.z += 4.5;
							formatstring(tmp)("Index: %i\nScript: pickup_%i\nType: %s Armour", e.attr[0], e.attr[1], armour[ap->armour]);
							break;
						}
					}
				}
				else
				{
					pos.z += 1.5;
					formatstring(tmp)("\fs\f3Invalid index %i\fr", e.attr[0]);
				}
				return;
			case ENEMY:
				pos.z += 3.0;
				formatstring(tmp)("Yaw: %i\nIndex: %i",
					e.attr[0],
					e.attr[1]
				);
				return;
			case WAYPOINT:

				return;
			case TELEPORT:
				pos.z += 4.5;
				formatstring(tmp)("Teleport Tag: %i\nRadius: %i\nModel: %s (%i)",
					e.attr[0],
					e.attr[1],
					mapmodelname(e.attr[2]), e.attr[2]
				);
				return;
			case TELEDEST:
				pos.z += 3.0;
				formatstring(tmp)("Yaw: %i\nTeleport Tag: %i",
					e.attr[0],
					e.attr[1]
				);
				return;
			case CHECKPOINT:
			{
				pos.z += 7.5;
				formatstring(tmp)("Yaw: %i\nRadius: %i\nModel: %s (%i)\nType: %s (%i)\nScript: checkpoint_%i",
					e.attr[0],
					e.attr[1],
					mapmodelname(e.attr[2]), e.attr[2],
					e.attr[3] ? "End" : "Save", e.attr[3],
					e.attr[4]
				);
				return;
			}
			case JUMPPAD:
				pos.z += 6.0;
				formatstring(tmp)("Z: %i\nY: %i\nX: %i\nRadius: %i",
					e.attr[0],
					e.attr[1],
					e.attr[2],
					e.attr[3]
				);
				return;
			case PLATFORM:
				pos.z += 4.5;
				formatstring(tmp)("Yaw: %i\nModel: %s (%i)\nSpeed: %i",
					e.attr[0],
					mapmodelname(e.attr[1]), e.attr[1],
					e.attr[2]
				);

				return;
			case PLATFORMROUTE:
				pos.z += 3.0;
				formatstring(tmp)("Tag: %i\nNext: %i",
					e.attr[0],
					e.attr[1]
				);

				return;
			case CAMERA:
				pos.z += 7.5;
				formatstring(tmp)("Tag: %i\nYaw: %i\nPitch: %i\nDistance: %i\nType: %s (%i)",
					e.attr[0],
					e.attr[1],
					e.attr[2],
					e.attr[3],
					e.attr[4] == 1 ? "Fixed" : "Follow", e.attr[4]
				);
				return;
			case AXIS:
				pos.z += 7.5;
				formatstring(tmp)("Yaw1: %i\nYaw2: %i\nTag: axis_script_%i\nRadius: %i\nVert Radius: %i",
					e.attr[0],
					e.attr[1],
					e.attr[2],
					e.attr[3],
					e.attr[4]
				);
				return;
		}
	}

	void writeent(entity &e, char *buf) {}  // write any additional data to disk (except for ET_ ents)

	void readent(entity &e, char *buf, int ver)     // read from disk, and init
	{
		if(ver <= 30)
		{
			switch(e.type)
			{
				case CAMERA:
				case AXIS:
					e.attr[1] = (e.attr[1] + 180) % 360;
					if(e.type == CAMERA)
						break;

				case BOX:
				case ENEMY:
				case TELEDEST:
				case CHECKPOINT:
				case PLATFORM:
					e.attr[0] = (e.attr[0] + 180) % 360;
					break;
			}
		}
	}

	float dropheight(entity &e)
	{
		if (e.type==MAPMODEL) return 0.0f;
		return 4.0f;
	}

	void clearents()
	{
		while(ents.length()) deleteentity(ents.pop());
	}

	//stubs
	void editent(int i, bool local) {}
	void rumble(const extentity &e) {}
	void trigger(extentity &e){}

	bool mayattach(extentity &e)
	{
		switch(e.type)
		{
			case PLATFORM: return true;
			default: return false;
		}
	}

	bool attachent(extentity &e, extentity &a)
	{
		switch(e.type)
		{
			case PLATFORM: if(a.type == PLATFORMROUTE) return true;;
			default: return false;
		}
	}

	int *getmodelattr(extentity &e)
	{
		switch(e.type)
		{
			case CHECKPOINT:
			case TELEPORT:
				return &e.attr[2];
			case BOX:
			case PLATFORM:
				return &e.attr[1];
			default:
				return NULL;
		}
	}

	bool checkmodelusage(extentity &e, int i)
	{
		switch(e.type)
		{
			case TELEPORT:
			case CHECKPOINT:
				return e.attr[2] == i;
			case PLATFORM:
			case BOX:
				return e.attr[1] == i;
			default:
				return false;
		}
	}
}

