#include "sspgame.h"

namespace game
{
	struct bouncer : physent
	{
		int prj, lifetime;
		float lastyaw, roll;
		sspent *owner;

		bouncer() : roll(0), owner(NULL)
		{
			type = ENT_BOUNCE;
			collidetype = COLLIDE_AABB;
		}
		~bouncer() {}
	};

	struct mine
	{
		vec o, delta;
		int prj, lifetime;
		float roll;
		sspent *owner, *homing;

		mine() : delta(vec(0, 0, 0)), roll(0), homing(NULL) {}
		~mine() {}
	};

	vector<proj *> projs;
	vector<projectile *> projectiles;
	vector<bouncer *> bouncers;
	vector<mine *> mines;

	ICOMMAND(addproj, "siiiii", (const char *s, int *u, int *w, int *x, int *y, int *z), projs.add(new proj(s, *u, *w, *x, *y, *z)))
	ICOMMAND(resetprojs, "", (), projs.deletecontents();)

	void clearprojs(sspent *d)
	{
		loopv(projectiles) if(projectiles[i]->owner == d) projectiles[i]->owner = NULL;
		loopv(bouncers) if(bouncers[i]->owner == d) bouncers[i]->owner = NULL;
		loopv(mines)
		{
			if(mines[i]->owner == d) mines[i]->owner = NULL;
			if(mines[i]->homing == d) mines[i]->homing = NULL;
		}
	}

	bool intersect(dynent *d, const vec &from, const vec &to, float &dist)   // if lineseg hits entity bounding box
	{
		vec bottom(d->o), top(d->o);
		bottom.z -= d->eyeheight;
		top.z += d->aboveeye;
		return linecylinderintersect(from, to, bottom, top, d->radius, dist);
	}

	dynent *intersectclosest(const vec &from, const vec &to, sspent *at, float &bestdist, float maxdist)
	{
		dynent *best = NULL;
		bestdist = maxdist;
		loopi(numdynents())
		{
			dynent *o = iterdynents(i);
			if(o==at || o->state != CS_ALIVE) continue;
			float dist;
			if(!intersect(o, from, to, dist)) continue;
			if(dist < bestdist)
			{
				best = o;
				bestdist = dist;
			}
		}
		return best;
	}

	void setprojdecal(int *idx, int *rad, int *col)
	{
		if(!projs.length()) {conoutf("Error: no projectiles defined, define some with /addproj"); return;}

		proj &p = *projs[projs.length()-1];
		p.didx = *idx;
		p.drad = *rad;
		p.dcol = bvec((*col >> 16) & 0xFF, (*col >> 8) & 0xFF, *col & 0xFF);
	}
	COMMAND(setprojdecal, "iii");

	void addproj(int p, sspent *d)
	{
		if(!projs.inrange(p)) return;

		projectiles.add(new projectile(p, vec(d->o.x, d->o.y, d->o.z - d->eyeheight/2), d->yaw, d->pitch, d));
	}

	void explode(const vec &o, int radius, int damage, const sspent *owner)
	{
		particle_fireball(o, radius * 1.25, PART_EXPLOSION, 500, 0xDFAF5F);

		vec col(1.5, 1, .75);
		adddynlight(o, radius * 3, col, 150, 400, DL_EXPAND);

		loopv(sspobjs)
		{
			sspent *d = sspobjs[i];
			if(d == owner) continue;

			vec min = vec(d->o).sub(vec(d->radius, d->radius, d->eyeheight));
			vec max = vec(d->o).add(vec(d->radius, d->radius, d->aboveeye));

			if(o.dist_to_bb(min, max) <= radius)
				d->takedamage(damage);
		}
	}


	void clearprojs()
	{
		projectiles.deletecontents();
		bouncers.deletecontents();
		mines.deletecontents();
	}

	void updateprojs()
	{
		if(player1->attacking && pickups.inrange(player1->gunselect))
		{
			pickup_weapon &p =* ((pickup_weapon *) pickups[player1->gunselect]);
			if(player1->shootmillis + p.cooldown < lastmillis)
			{
				player1->shootmillis = lastmillis;
				addproj(p.projectile, player1);
				playsound(p.sound, &player1->o);
			}
		}
		loopvrev(projectiles)
		{ //do simplistic flying first, add gravity effects, timers, and so forth, later
			projectile *p = projectiles[i];
			proj *prj = projs[p->prj];

			if(prj->speed > 10000) //assume bullet-ish for high speeds
			{
				vec oldpos = p->o;
				float dist = raycubepos(oldpos, p->d, p->o, 0, RAY_CLIPMAT|RAY_ALPHAPOLY);
				float vicdist;

				sspent *victim = (sspent *) intersectclosest(oldpos, p->o, p->owner, vicdist, 1);
				if(victim)
				{
					p->o.sub(vec(p->d).mul((1 - vicdist) * dist));

					if(debug)
					{
						defformatstring(ds)("%p", victim);
						particle_textcopy(p->o, ds, PART_TEXT, 2000, 0xFF0000, 4, -5);
					}
				}

				explode(p->o, prj->radius, prj->damage, p->owner);

				adddecal(prj->didx, p->o, p->d.neg(), prj->drad, prj->dcol);

				if(debug)
					particle_splash(PART_SPARK, 10, 1000, p->o, 0xFF0000, 1, 200);

				delete projectiles.remove(i);
			}
			else if(prj->speed>500) //lower speeds means it's rocketish
			{
				bool deleted = false;

				float dist = (prj->speed * curtime) / 1000.0f;
				vec pos;
				float newdist = raycubepos(p->o, p->d, pos, 0, RAY_CLIPMAT|RAY_ALPHAPOLY);

				if(newdist > dist)
				{
					pos = vec(p->o).add(vec(p->d).mul(dist));
				}
				else
				{
					dist = newdist;
					deleted = true;
				}

				sspent *victim = (sspent *) intersectclosest(p->o, pos, p->owner, newdist, 1);

				if(victim)
				{
					deleted = true;
					pos.sub(vec(p->d).mul((1 - newdist) * dist));

					if(debug)
					{
						defformatstring(ds)("%p", victim);
						particle_textcopy(pos, ds, PART_TEXT, 2000, 0xFF0000, 4, -5);
					}
				}

				p->o = pos;

				if(deleted)
				{
					explode(p->o, prj->radius, prj->damage, p->owner);
					if(debug)
						particle_splash(PART_SPARK, 10, 1000, p->o, 0xFF0000, 1, 200);

					adddecal(prj->didx, p->o, p->d.neg(), prj->drad, prj->dcol);
					delete projectiles.remove(i);
					continue;
				}

				if(debug)
					particle_splash(PART_SPARK, 10, 1000, p->o, 0xFFBF3A, 1, 200);
			}
			else if(prj->speed) //hand grenad -ish, bouncers, they are affected by jumppads
			{
				bouncer *b = bouncers.add(new bouncer());
				projectile *p = projectiles.remove(i);
				proj *prj = projs[p->prj];

				setbbfrommodel(b, prj->mdl);
				b->prj = p->prj;
				b->o = p->o;
				b->owner = p->owner;
				b->lifetime = 2500;
				b->vel = vec(p->d).mul(prj->speed);

				avoidcollision(b, p->d, b->owner, .1f);
				b->resetinterp();

				delete p;
			}
			else //mines or traps
			{
				mine *m = mines.add(new mine());
				projectile *p = projectiles.remove(i);
// 				proj *prj = projs[p->prj];

				m->owner = p->owner;
				m->o = p->o;
				m->prj = p->prj;
				m->lifetime = 10000;

				delete p;
			}
		}

		loopvrev(bouncers)
		{
			bouncer &bnc = *bouncers[i];

			vec oldo = bnc.o;
			if(bounce(&bnc, .6, .5) || (bnc.lifetime -= curtime) <= 0)
			{
				explode(bnc.o, projs[bnc.prj]->radius, projs[bnc.prj]->damage, bnc.owner);

				delete bouncers.remove(i);
				continue;
			}

			bnc.roll += (oldo.sub(bnc.o).magnitude()) / (4 * RAD);
		}

		loopvrev(mines)
		{
			mine &m = *mines[i];
			m.lifetime -= curtime;

			if(m.lifetime <= 0)
			{
				explode(m.o, projs[m.prj]->radius, projs[m.prj]->damage, m.owner);
				delete mines.remove(i);
				continue;
			}

			if(!m.homing) loopvj(sspobjs)
			{
				sspent *d = sspobjs[j];
				if(d == m.owner) continue;

				vec min = vec(d->o).sub(vec(d->radius, d->radius, d->eyeheight));
				vec max = vec(d->o).add(vec(d->radius, d->radius, d->aboveeye));

				if(m.o.dist_to_bb(min, max) <= (d->etype == ENT_ENEMY ? 40: 16))
				{
					m.homing = d;
					break;
				}
			}

			m.o.add(vec(m.delta).mul(curtime / 1000.f));
			m.roll += m.delta.magnitude() * curtime / RAD / 4000;

			if(m.homing)
			{
				vec d = vec(m.homing->o).sub(m.o).sub(vec(0, 0, m.homing->eyeheight/2)).rescale(curtime / 150.f);
				m.delta.add(d);

				if(m.o.dist(m.homing->o) < 8)
				{
					explode(m.o, 16, projs[m.prj]->damage, m.owner);
					delete mines.remove(i);
				}
			}
		}
	}

	void renderprojs()
	{
		loopv(projectiles)
		{
			projectile &p = *projectiles[i];
			float yaw, pitch;
			vectoyawpitch(p.d, yaw, pitch);

			rendermodel(NULL, projs[p.prj]->mdl, 0, p.o, yaw, pitch, 0);
		}

		loopv(bouncers)
		{
			bouncer &b = *bouncers[i];

			rendermodel(NULL, projs[b.prj]->mdl, ANIM_MAPMODEL|ANIM_LOOP, b.o, b.yaw, 0 - b.roll, 0);
		}

		loopv(mines)
		{
			mine &m = *mines[i];

			rendermodel(NULL, projs[m.prj]->mdl, ANIM_MAPMODEL|ANIM_LOOP, m.o, m.roll, m.roll, m.roll);
		}
	}

	void adddynlights()
	{

	}

	void writeprojectiles(stream *f)
	{
		f->printf("resetprojs\n\n");
		loopv(projs)
		{
			proj &p = *projs[i];
			f->printf("addproj %s %i %i %i %i %i //%i\n", escapestring(p.mdl), p.damage, p.radius, p.force, p.travelsound, p.speed, i);
			f->printf("setprojdecal %i %i 0x%.2X%.2X%.2X\n", p.didx, p.drad, p.dcol.x, p.dcol.y, p.dcol.z);
		}
	}
}

