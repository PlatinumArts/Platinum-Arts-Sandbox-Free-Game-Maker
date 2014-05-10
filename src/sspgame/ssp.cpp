#include "sspgame.h"

namespace game
{
	int  maptime = 0, secsremain = 0, secsallowed, cameraent;
	// time started / time left / time allowed / camera / last camera / camera assignment time
	bool intermission = false;
	string clientmap;

	bool connected = false;
	void gamedisconnect(bool cleanup) {connected = false;}
	void gameconnect(bool _remote) {connected = true;}

	VARP(debug, 0, 0, 1);

	vector<sspent *> sspobjs;

	sspchar *player1 = new sspchar();

	//items
	vector<pickup *> pickups;
	vector<box_def *> boxdefs;

	vector<eventimage *> eventimages;

	void addcoin(char *s, int *x)
	{
		pickups.add(new pickup_generic(s, PICKUP_COIN));
		pickup_generic *p = (pickup_generic *) pickups[pickups.length() - 1];
		p->amount = *x;
	}
	COMMAND(addcoin, "si");

	void addhealth(char *s, int *x)
	{
		pickups.add(new pickup_generic(s, PICKUP_HEALTH));
		pickup_generic *p = (pickup_generic *) pickups[pickups.length() - 1];
		p->amount = *x;
	}
	COMMAND(addhealth, "si");

	void addtime(char *s, int *x)
	{
		pickups.add(new pickup_generic(s, PICKUP_TIME));
		pickup_generic *p = (pickup_generic *) pickups[pickups.length() - 1];
		p->amount = *x;
	}
	COMMAND(addtime, "si");

	void addlife(char *s, int *x)
	{
		pickups.add(new pickup_generic(s, PICKUP_LIVES));
		pickup_generic *p = (pickup_generic *) pickups[pickups.length() - 1];
		p->amount = *x;
	}
	COMMAND(addlife, "si");

	void addweapon(char *s, char *t, int *x, int *y, int *z)
	{
		pickups.add(new pickup_weapon(s));
		pickup_weapon *p = (pickup_weapon *) pickups[pickups.length() - 1];
		p->projectile = *x;
		p->sound = *y;
		p->cooldown = *z;
		p->attachmdl = newstring(t);
	}
	COMMAND(addweapon, "ssiii");

	void addarmour(char *s, char *t, int *x, char *tex, int *col)
	{
		pickups.add(new pickup_armour(s));
		pickup_armour *p = (pickup_armour *) pickups[pickups.length() - 1];
		p->armour = clamp<int, int>(*x, ARM_PLAIN, ARM_MAX);
		p->attachmdl = newstring(t);
		if(*tex)
		{
			delete[] p->tex;
			p->tex = newstring(tex);
			p->icon = textureload(tex, 3);
		}
		if(*col) p->iconcolour = *col;
	}
	COMMAND(addarmour, "ssisi");

	ICOMMAND(resetpickups, "", (), pickups.deletecontents(); boxdefs.deletecontents();)

	ICOMMAND(eventimage, "si", (char *s, int *x), eventimages.add(new eventimage(s, *x)))

	ICOMMAND(addbox, "i", (int *i), boxdefs.add(new box_def()); boxdefs[boxdefs.length() - 1]->inv.add(*i);)
	ICOMMAND(addboxitem, "i", (int *i), boxdefs[boxdefs.length() - 1]->inv.add(*i);)

	ICOMMAND(getyaw, "", (), defformatstring(s)("%f", player1->yaw); result(s);)

	//movement
	void switchaxis() //flip the rotation if a player is close to an axis entity, try to make it automatic eventually
	{
		loopv(entities::ents)
		{
			extentity &e = *entities::ents[i];
			if(e.type !=AXIS) continue;
			vec pos1 = e.o;
			vec pos2 = player1->o;
			pos1.z = pos2.z = 0;

			if(pos1.dist(pos2) <= (e.attr[3] ? abs(e.attr[3]) : 16) && abs(player1->o.z-e.o.z) <= (e.attr[4] ? abs(e.attr[4]) : 64))
			{
				vec pos = player1->o;
				int oldyaw = player1->yaw;

				if((int) player1->yaw % 180 == e.attr[0] % 180)
					player1->yaw = e.attr[1];
				else
					player1->yaw = e.attr[0];

				player1->newpos = e.o;
				player1->vel.rotate_around_z((oldyaw - player1->yaw) * RAD);
				player1->newpos.z = pos.z;

				defformatstring(id)("axis_script_%i", e.attr[2]); //ie, to change cameras
				if(identexists(id)) execute(id);
				return;
			}
		}
	}
	ICOMMAND(switchaxis, "", (), switchaxis())

	int closestyaw(int yaw)
	{
		while(yaw >= 360)
			yaw %= 360;
		while(yaw < 0)
			yaw += 360;

		if(yaw>= 45 && yaw <= 134)
			return 90;

		else if(yaw >= 135 && yaw <= 224)
			return 180;

		else if(yaw>=225 && yaw<=314)
			return 270;

		else
			return 0;
	}

	void moveright(int down) //if cam.yaw == 0, then right = 0 && 90
	{
		player1->k_right = down!=0;
	}

	void moveleft(int down)
	{
		player1->k_left = down!=0;
	}
	ICOMMAND(moveright, "D", (int *down), { moveright(*down); })
	ICOMMAND(moveleft, "D", (int *down), { moveleft(*down); })
	ICOMMAND(jump,   "D", (int *down), { if(!*down || canjump()) player1->jumping = *down!=0; })

	const char *getclientmap() { return clientmap; }
	void resetgamestate() {}

	void suicide(physent *d)
	{
		((sspent *) d)->takedamage(10, true);
	}

	sspent *spawnstate(sspent *d)              // reset player state not persistent accross spawns
	{
		d->respawn();
		return d;
	}

	void spawnplayer(sspent *d)   // place at random spawn. also used by monsters!
	{
		sspchar *en = (sspchar *) d;
		if (d==player1)
		{
			if (entities::ents.inrange(en->checkpoint)) d->yaw = entities::ents[en->checkpoint]->attr[0];
			findplayerspawn(d, en->checkpoint>=0 ? en->checkpoint : -1, 0);
		}
		else
			findplayerspawn(d, -1, 0);
		spawnstate(d);
		d->state = d==player1 && editmode ? CS_EDITING : CS_ALIVE;
		en->lastpain = lastmillis;
	}

	void respawnself()
	{
		spawnplayer(player1);
	}

	void fixplayeryaw()
	{
		//constrains player's yaw to 90 degree angles, round up
		player1->yaw = closestyaw(player1->yaw);
	}

	void getcamera()
	{
		defformatstring(s)("%i", cameraent<0 ? -1 : entities::ents[cameraent]->attr[0]);
		result(s);
	}
	ICOMMAND(getcamera, "", (), getcamera();)

	//set cameraent to the valid ent if it exists, otherwise, just ignore the request
	void setcamera(int attr)
	{
		loopv(entities::ents)
		{
			extentity &e = *entities::ents[i];
			if(e.type == CAMERA && e.attr[0] == attr)
			{
				cameraent = i;
				return;
			}
		}
	}
	ICOMMAND(setcamera, "i", (int *x), { setcamera(*x);})

	void setwindowcaption()
	{
		defformatstring(capt)("Sandbox %s: Side Scrolling Platformer - %s", version, getclientmap()[0] ? getclientmap() : "[new map]");
		SDL_WM_SetCaption(capt, NULL);
	}

	void startmap(const char *name)   // called just after a map load
	{
		sspobjs.removeobj(player1); //remove the player
		sspobjs.deletecontents();
		sspobjs.add(player1);

		player1->~sspchar();
		new(player1) sspchar();

		secsallowed = 300;
		secsremain = 300;

		player1->checkpoint = -1;
		cameraent = -1;

		setcamera(0); //set the camera if it exists

		copystring(clientmap, name ? name : "");
		intermission = false;
		maptime = lastmillis;
		entities::prepareents();
		initialisemonsters();
		findplayerspawn(player1, -1);
		player1->lastpain = lastmillis; //for spawning invulnerability
		conoutf(CON_GAMEINFO, "\f2D to move right, A to move left, W to jump, and click to attack, have fun!");

		if(identexists("mapstart")) execute("mapstart");
	}

	void edittoggled(bool on)
	{
		if(!on)
		{
			sspobjs.remove(0);
			sspobjs.deletecontents();
			sspobjs.add(player1);

			entities::prepareents();
			initialisemonsters();
		}
	}

	void respawn()
	{
		if(player1->state==CS_DEAD)
		{
			player1->attacking = false;
			if(player1->lives > 0)
			{
				respawnself();
				player1->lives--;
			}
			else
			{
				player1->lives = 6; //i-1
				changemap(getclientmap());
			}
		}
	}

	void doattack(bool on)
	{
		if(intermission) return;
		if((player1->attacking = on)) respawn();
	}
	ICOMMAND(attack, "D", (int *down), { doattack(*down!=0); })

	bool canjump()
	{
		if(!intermission) respawn();
		return player1->state!=CS_DEAD && !intermission;
	}

	bool allowdoublejump(physent *d)
	{
		sspchar *sc = (sspchar *) d;
		if(sc->etype != ENT_CHAR && sc->etype != ENT_ENEMY)
			return false;

		if(sc->armour==ARM_FLY && lastmillis - sc->lastjump > 600)
			return true;
		if(sc->armour == ARM_DJUMP && lastmillis - sc->lastjump > sc->timeinair)
			return true;

		return false;
	}

	bool detachcamera()
	{
		return false;
	}

	FVARP(camfocusrate, 0.01, 1.5, 10);
	FVARP(cammovemul, 0, 1, 1);
	VARP(mousetarget, 0, 1, 1);

	float mousex = 0, mousey = 0;

	bool recomputecamera(physent *&camera1, physent &tempcamera, bool &detachedcamera, float &thirdpersondistance)
	{
		if(editmode)
			return false;

		camera1 = &tempcamera;
		int cameratype = 0;
		float yaw = 0, pitch = 0, dist = 100,
		      multiplier = min<float>(1, camfocusrate * curtime / 200.0f);

		if(!detachedcamera)
			camera1->o = player1->o;

		if(entities::ents.inrange(cameraent))
		{
			extentity *e = entities::ents[cameraent];
			yaw = e->attr[1];
			pitch = e->attr[2];
			dist = e->attr[3];
			cameratype = e->attr[4];
		}

		vec offset = vec(0, 0, 0);

		if(mousetarget)
		{
			offset = vec(player1->yaw * RAD, 0);
			//if(offset.x < -1e-5f || offset.y < -1e-5f)
			if(closestyaw(player1->yaw - yaw) == 180 || closestyaw(player1->yaw - yaw) == 270)
				offset.mul(vec(-1, -1, 0));
			offset.mul(mousex);
			offset.z = mousey;
			offset.mul(dist);
		}

		offset.z -= player1->eyeheight / 2.0f;

		vec dest;

		switch(cameratype)
		{
			case 1:
				//NOTE: can only be 1 when cameraent is inrange
				dest = entities::ents[cameraent]->o;
				break;
			case 0:
			default:
				dest = vec(yaw * RAD, pitch * RAD).mul(-dist).add(vec(offset).mul(cammovemul)).add(player1->o);
				break;
		}
		//positions are interpolated...
		tempcamera.o.mul(1 - multiplier).add(dest.mul(multiplier));

		//... so are yaws

		vec olddir = vec(tempcamera.yaw * RAD, tempcamera.pitch * RAD).normalize().mul(1 - multiplier);
		offset.add(player1->o).sub(tempcamera.o).normalize().mul(multiplier).add(olddir);

		vectoyawpitch(offset, tempcamera.yaw, tempcamera.pitch);

		detachedcamera = true;
		return true;
	}

	bool mousemove(int &dx, int &dy, float &cursens)
	{
		if(editmode)
			return false;
		if(!mousetarget)
			return true;

		mousex += dx * cursens / 100.0f;
		mousey -= dy * cursens / 100.0f;

		mousex = clamp(.75f, -.75f, mousex);
		mousey = clamp(.75f, -.75f, mousey);

		return true;
	}

	void updateworld()        // main game update loop
	{
		if(!curtime || !connected) return;

		if(editmode)
		{
			secsremain = 300;
			intermission = false;
			maptime = lastmillis;

			physicsframe();
			moveplayer(player1, 10, true);
			return;
		}

		disablezoom();
		fixplayeryaw();

		if(mousetarget)
		{
			vec delta = vec(player1->yaw * RAD, 0);
			//if(delta.x < -1e-5f || delta.y < -1e-5f)
				//delta.mul(-1);

			float yaw = 0;
			if(entities::ents.inrange(cameraent) && entities::ents[cameraent]->type == CAMERA)
				yaw = entities::ents[cameraent]->attr[1];
			if(closestyaw(player1->yaw - yaw) == 180 || closestyaw(player1->yaw - yaw) == 270)
				delta.mul(vec(-1, -1, 0));

			delta.mul(mousex);
			delta.z = mousey;

			if(!delta.iszero())
				vectoyawpitch(delta, player1->yaw, player1->pitch);
		}
		else
			player1->pitch = 0;

		if (secsremain <= 0) intermission = true;

		if(!intermission)
		{
			physicsframe();
			updateprojs();
			secsremain = secsallowed - (lastmillis - maptime)/1000;
			loopvrev(sspobjs)
			{
				if(!sspobjs[i]->update())
				{
					sspent *s = sspobjs.remove(i);
					clearprojs(s);
					delete s;
				}
				else if(sspobjs[i]->stool())
				{
					sspobjs[i]->takedamage(1);
				}
			}
			loopvrev(entities::platforms)
			{
				entities::platforms[i]->update();
			}
			loopvrev(entities::items)
			{
				if(!entities::items[i]->update())
				{
					delete entities::items[i];
					entities::items.remove(i);
				}
			}
		}
		else
		{
			clearprojs();
		}
	}


	void quad(int x, int y, int xs, int ys)
	{
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2i(x,    y);
		glTexCoord2f(1, 0); glVertex2i(x+xs, y);
		glTexCoord2f(1, 1); glVertex2i(x+xs, y+ys);
		glTexCoord2f(0, 1); glVertex2i(x,    y+ys);
		glEnd();
	}

	float abovegameplayhud(int w, int h) {return 1;}

	void gameplayhud(int w, int h)
	{
		static Texture *character = NULL, *overlay = NULL, *hp = NULL, *time = NULL, *coin = NULL, *shield = NULL;
		if(!character) character = textureload("data/ssp/hud/character", 3);
		if(!overlay) overlay = textureload("data/ssp/hud/overlay", 3);
		if(!hp) hp = textureload("data/ssp/hud/hp", 3);
		if(!time) time = textureload("data/ssp/hud/time", 3);
		if(!coin) coin = textureload("data/ssp/hud/coin", 3);
		if(!shield) shield = textureload("data/ssp/hud/shield", 3);

		glPushMatrix();
		float scale = min (w / 1600.0f, h / 1200.0f);
		glScalef(scale, scale, 1);

		float right = w / scale, bottom = h / scale; // top and left are ALWAYS 0

		loopvrev(eventimages)
		{
			if(eventimages[i]->deltime < lastmillis)
			{
				delete eventimages[i];
				eventimages.remove(i);
			}
		}

		static int numw = 1;

		if(!eventimages.length())
			numw = 1;

		while(eventimages.length() > numw * numw)
			numw++;

		float size = 768.0f / numw;

		loopi(numw)
		{
			loopj(numw)
			{
				int index = i * numw + j;
				if(!eventimages.inrange(index))
					break;

				eventimage *img = eventimages[index];
				settexture(img->tex);

				if(img->deltime - lastmillis <= 500)
					glColor4f(1, 1, 1, (img->deltime - lastmillis) / 500.0);
				else if(lastmillis - img->starttime <= 200)
					glColor4f(1, 1, 1, (lastmillis - img->starttime) / 200.0);
				else
					glColor4f(1, 1, 1, 1);

				quad(right / 2 - 384 + size * j, bottom / 2 - 384 + size * i, size, size);

				settexture(overlay);
				quad(right / 2 - 384 + size * j, bottom / 2 - 384 + size * i, size, size);
			}
		}

		settexture(character);

		if(lastmillis < player1->lastpain + 2000)
			glColor4f(1, .5, .5, 1); //set colours, he obviously had a recent owie
		else
			glColor4f(1, 1, 1, 1);

		quad(32, bottom - 224, 192, 192);

		settexture(hp); //HP, try a cube or something

		switch(player1->health)
		{
			case 0:
			case 1:
				glColor3f(1, 0, 0);
				break;
			case 2:
				glColor3f(.8, 8, 0);
				break;
			default:
			case 3:
				glColor3f(0, 1, 0);
				break;
		}

		loopi(player1->health)
			quad(256 + 80 * i, bottom - 96, 64, 64);

		settexture(time); //clock
		glColor4f(1, 1, 1, 1);
		quad(right / 2 - 224, bottom - 160, 128, 128);

		settexture(coin); //bling bling!
		glColor4f(1, 1, 1, 1);
		quad(right - 448, bottom - 160, 128, 128);


		if(player1->armour && pickups.inrange(player1->armourvec))
		{
			pickup_armour *arm = (pickup_armour *) pickups[player1->armourvec];
			settexture(arm->icon);

			uchar col[3] = {
				uchar((arm->iconcolour >> 16) & 255),
				uchar((arm->iconcolour >> 8 ) & 255),
				uchar((arm->iconcolour      ) & 255)
			};
			glColor3ubv(col);
		}
		else
		{
			settexture(shield); //are you protected? :P
			glColor4f(1, 1, 1, .25);
		}

		quad(32, bottom - 384, 128, 128);

		draw_textf("%d", 256, bottom - 160, player1->lives);
		draw_textf("%d", right / 2 - 96, bottom - 96, secsremain);
		draw_textf("%d", right - 288, bottom - 96, player1->coins);

		glPopMatrix();
	}

	const char *getmapinfo() { return "";}

	int clipconsole(int w, int h)
	{
		return 0;
	}
	const char *defaultcrosshair(int index)
	{
		//memo to self, make 1^2 px crosshair
		switch(index)
		{
			case 2: return "packages/crosshairs/default";
			case 1: return "packages/crosshairs/edit";
			default: return "data/items";
		}
	}

	int selectcrosshair(float &r, float &g, float &b)
	{
		if(editmode)
		{
			r = b = 0.5;
			g = 1.0f;
			return 1;
		}
		if(mousetarget)
			return 2;

		return 0;
	}

	void initclient()
	{
		clientmap[0] = 0;
		sspobjs.add(player1);
	}

	void newmap(int size) {}

	void physicstrigger(physent *d, bool local, int floorlevel, int waterlevel, int material)
	{
		if(d->state!=CS_ALIVE) return;
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
				break;
			case MAT_WATER:
				if (waterlevel==0) break;
				playsound(waterlevel > 0 ? S_SPLASH1 : S_SPLASH2 , d==player1 ? NULL : &d->o);
				particle_splash(PART_WATER, 200, 200, d->o, (watercolor.x<<16) | (watercolor.y<<8) | watercolor.z, 0.5);
			default:
				if (floorlevel==0) break;
				playsound(floorlevel > 0 ? S_JUMP : S_LAND, local ? NULL : &d->o);
				break;
		}
	}

	int numdynents() { return sspobjs.length() + entities::platforms.length(); }

	dynent *iterdynents(int i)
	{
		if(i < sspobjs.length())
			return sspobjs[i];
		i -= sspobjs.length();
		if(i < entities::platforms.length())
			return entities::platforms[i];
		// i -= entities::platforms.length();
		return NULL;
	}

	void dynentcollide(physent *d, physent *o, const vec &dir)
	{
		//rewrite boxes and monsters to use this
	}

	void edittrigger(const selinfo &sel, int op, int arg1, int arg2, int arg3) {}

	void writemapdata(stream *f)
	{
		f->printf("resetpickups\n\n");
		loopv(pickups)
		{
			pickup *p = pickups[i];
			switch(p->type)
			{
				case PICKUP_COIN:
				case PICKUP_TIME:
				case PICKUP_LIVES:
				case PICKUP_HEALTH:
				{
					pickup_generic *gp = (pickup_generic *) p;
					static const char *cmds[] = {"addcoin", "addhealth", "addtime", "addlife"};

					f->printf("%s %s %i //%i\n", cmds[gp->type], escapestring(gp->mdl), gp->amount, i);
					break;
				}
				case PICKUP_WEAPON:
				{
					pickup_weapon *wp = (pickup_weapon *) p;
					f->printf("addweapon %s %s %i %i %i //%i\n", escapestring(wp->mdl), escapestring(wp->attachmdl), wp->projectile, wp->sound, wp->cooldown, i);
					break;
				}
				case PICKUP_ARMOUR:
				{
					pickup_armour *ap = (pickup_armour *) p;
					f->printf("addarmour %s %s %i %s 0x%.6X //%i\n", escapestring(ap->mdl), escapestring(ap->attachmdl), ap->armour, escapestring(ap->tex), ap->iconcolour, i);
					break;
				}
			}
		}
		f->putchar('\n');
		loopv(boxdefs)
		{
			loopvj(boxdefs[i]->inv)
			{
				if(j) f->printf("addboxitem %i\n", boxdefs[i]->inv[j]);
				else f->printf("addbox %i //%i\n", boxdefs[i]->inv[j], i);
			}
		}
		f->putchar('\n');
		writemonsters(f);
		writeprojectiles(f);
	}


	void openworld(const char *name, bool fall = false)
	{
		if(!connected) localconnect();

		if(name && *name && load_world(name))
			return;
		else if(fall && load_world("ssptest"))
			return;
		else
			emptymap(10, true, "untitled", true);
	}
	ICOMMAND(map, "s", (char *s), openworld(s, false);)

	void changemap(const char *name) { openworld(name, true); }
	void forceedit(const char *name) { openworld(name); }

	bool needminimap() { return 0; }
	bool showenthelpers() { return editmode; }

	void writegamedata(vector<char> &extras) {}
	void readgamedata(vector<char> &extras) {}

	const char *gameident() { return "ssp"; }
	const char *autoexec() { return "autoexec.cfg"; }
	const char *savedservers() { return NULL; }
	void loadconfigs() {}
	void texturefailed(char *name, int slot) {}
	void mmodelfailed(const char *name, int idx) {}
	void mapfailed(const char *name) {}
}

