#include "game.h"

namespace game
{
    VARP(ragdoll, 0, 1, 1);
    VARP(ragdollmillis, 0, 10000, 300000);
    VARP(ragdollfade, 0, 1000, 300000);
    VARP(forceplayermodels, 0, 0, 1);
    VARP(defaultplayermodel, 0, 0, 8);

    vector<DynamicEntity *> ragdolls;

    void saveragdoll(DynamicEntity *d)
    {
        if(!d->ragdoll || !ragdollmillis || !ragdollfade) return;
        DynamicEntity *r = new DynamicEntity(*d);
        r->lastupdate = ragdollfade;
        r->edit = NULL;
//        r->ai = NULL;
//        r->attackchan = r->idlechan = -1;
//        if(d==player1) r->playermodel = playermodel;
        ragdolls.add(r);
        d->ragdoll = NULL;
    }

    void clearragdolls()
    {
        ragdolls.deletecontents();
    }

    void moveragdolls()
    {
        loopv(ragdolls)
        {
            DynamicEntity *d = ragdolls[i];
            if(lastmillis > d->lastupdate + ragdollmillis)
            {
                delete ragdolls.remove(i--);
                continue;
            }
            moveragdoll(d);
        }
    }

    const PlayerModelInfo *getplayermodelinfo(int i)
    {
		if(playermodels.inrange(i))
		{
			return playermodels[i];
		}
		else return NULL;
    }

    void preloadplayermodel(const char *s)
    {
		loopv(playermodels)
		{
			if(strcmp(playermodels[i]->name, s) == 0)
			{
				return;
			}
		}
        conoutf("game: loading playermodel %s", s);
		playermodels.add(new PlayerModelInfo(s));
		preloadmodel(s);
    }
	ICOMMAND(preloadplayermodel, "s", (char *s), preloadplayermodel(s));

    const PlayerModelInfo &getplayermodelinfo(DynamicEntity *d)
    {
        CharacterInfo& info = d->getcharinfo();
        if(!playermodels.inrange(info.playermodel))
        {
			if(!playermodels.inrange(0))
			{
				preloadplayermodel("uh/chars/man");
			}
			conoutf("DEBUG: no playermodel - loading std");
			return *playermodels[0];
        }
        return *playermodels[info.playermodel];
    }

	void render(DynamicEntity *d, const char *mdlname, modelattach *attachments, bool hold, int actiondelay, int lastaction, int lastpain, float fade, bool ragdoll)
	{
        CharacterInfo& info = d->getcharinfo();
	    int anim = hold ? info.ao[AO_HOLD]|ANIM_LOOP : info.ao[AO_IDLE]|ANIM_LOOP; //if !hold set hold to ANIM_IDLE
	    float yaw = d->yaw+90;
	    float pitch = d->pitch;
//	    vec o = d->feetpos();
	    int basetime = 0;
        int action = info.dogesture ? -info.ao[AO_GESTURE] : 0;

		switch(d->state)
		{
			case CS_DEAD:
			{
		        anim = info.ao[AO_DYING];
		        basetime = lastpain;
		        if(ragdoll)
		        {
		            //if(!d->ragdoll || d->ragdoll->millis < basetime) anim |= ANIM_RAGDOLL;
		        }
		        else
		        {
		            pitch *= max(1.0f - (lastmillis-basetime)/500.0f, 0.0f);
		            if(lastmillis-basetime>1000) anim = info.ao[AO_DEAD]|ANIM_LOOP;
		        }
				break;
			}
			case CS_EDITING:
			case CS_SPECTATOR:
			{
				anim = info.ao[AO_EDIT]|ANIM_LOOP;
				break;
			}
			case CS_LAGGED:
			{
			    anim = ANIM_LAG|ANIM_LOOP;
			    break;
			}
			default:
			{
		     	if(lastmillis-lastpain < 300)
		        {
		            anim = info.ao[AO_PAIN];
		            basetime = lastpain;
		        }
		        else if(action && lastpain < lastaction && (action < 0 || (d->type != ENT_AI && lastmillis-lastaction < actiondelay)))
		        {
		            anim = (action < 0 ? -action : action) | ANIM_LOOP; // Protagoras: Static pose after all frames looks really bad. So we'll repeat them.
		            basetime = lastaction;
		        }

		        if(d->inwater && d->physstate<=PHYS_FALL) anim |= (((game::allowmove(d) && (d->move || d->strafe)) || d->vel.z+d->falling.z>0 ? info.ao[AO_SWIM] : info.ao[AO_SINK])|ANIM_LOOP)<<ANIM_SECONDARY;
		        else if(d->timeinair>100) anim |= (info.ao[AO_JUMP]|ANIM_END)<<ANIM_SECONDARY;
		        else if(game::allowmove(d) && (d->move || d->strafe))
		        {
		            if(d->move>0) anim |= (info.ao[AO_FORWARD]|ANIM_LOOP)<<ANIM_SECONDARY;
		            else if(d->strafe) anim |= ((d->strafe>0 ? info.ao[AO_LEFT] : info.ao[AO_RIGHT])|ANIM_LOOP)<<ANIM_SECONDARY;
		            else if(d->move<0) anim |= (info.ao[AO_BACKWARD]|ANIM_LOOP)<<ANIM_SECONDARY;
		        }

		        if((anim&ANIM_INDEX)==info.ao[AO_IDLE] && (anim>>ANIM_SECONDARY)&ANIM_INDEX) anim >>= ANIM_SECONDARY;
				break;
			}
		}

		//remove ragdoll if not needed
	    //if(d->ragdoll && (!ragdoll || anim!=ANIM_DYING)) DELETEP(d->ragdoll);

	    if(!((anim>>ANIM_SECONDARY)&ANIM_INDEX)) anim |= (ANIM_IDLE|ANIM_LOOP)<<ANIM_SECONDARY;
	    int flags = MDL_LIGHT | MDL_DYNLIGHT | MDL_SHADOW | MDL_DYNSHADOW;

	    if(d!=player1 && !(anim&ANIM_RAGDOLL)) flags |= MDL_CULL_VFC | MDL_CULL_OCCLUDED | MDL_CULL_QUERY;

//	    if(d->type==ENT_PLAYER) flags |= MDL_FULLBRIGHT;
//	    else flags |= MDL_CULL_DIST;
	    if(d->type!=ENT_PLAYER) flags |= MDL_CULL_DIST;

	    if(d->state==CS_LAGGED) fade = min(fade, 0.3f);
//	    else flags |= MDL_DYNSHADOW;

//	    rendermodel(NULL, name   , anim, d->feetpos(), yaw, pitch, 0, flags, d, attached[0].tag ? attached : NULL, basetime, 0, fade);
	    rendermodel(NULL, mdlname, anim, d->feetpos(), yaw, pitch, 0, flags, d, attachments, basetime, 0, fade); // Fix by Protagoras 20101119
	}


    void renderplayer(DynamicEntity *d, const PlayerModelInfo &mdl, float fade, bool mainpass)
    {
        CharacterInfo& charinfo = d->getcharinfo();
        int lastaction = d->lastaction;
        bool hold = false;
        int delay = 300;

        //offtools: add attachments (clothes)
        int ai = 0;

		int num = charinfo.attached.numattachments() + 5;
        modelattach *a = new modelattach[num];
        AttachedItems &attachments = charinfo.attached;
        loopv(attachments.items)
        {
            int animrule;
            int animtime;
        	const char* tag = attachtag(attachments.items[i]->part);
        	switch (attachments.items[i]->animrule)
        	{
				case ATTACH_ANIM_IDLE:
				{
					animrule = ANIM_IDLE|ANIM_LOOP; animtime = 0; break;
				}
				case ATTACH_ANIM_FULL:
				{
					animrule = -1; animtime = 0; break;
				}
				case ATTACH_ANIM_NONE:
				default:
				{
					animrule = -1; animtime = 0; break;
				}
        	}
        	a[ai++] = modelattach(tag, attachments.items[i]->model, animrule, animtime);
        }

        if(mainpass)
        {
            d->muzzle = vec(-1, -1, -1);
            a[ai++] = modelattach("tag_muzzle", &d->muzzle);
        }
        render(d, mdl.name, a[0].tag ? a : NULL, hold, delay, lastaction, -1, fade, false);
		DELETEA(a);
    }

    void rendergame(bool mainpass)
    {
        startmodelbatches();

        DynamicEntity *exclude = isthirdperson() ? NULL : followingplayer();
        loopv(players)
        {
            DynamicEntity *d = players[i];
            if(d == player1 || d->state==CS_SPECTATOR || d->state==CS_SPAWNING || d == exclude ) continue;
            renderplayer(d, getplayermodelinfo(d), 1, mainpass);
            copystring(d->info, colorname(d));
        }
        loopv(ragdolls)
        {
            DynamicEntity *d = ragdolls[i];
            float fade = 1.0f;
            if(ragdollmillis && ragdollfade)
            fade -= clamp(float(lastmillis - (d->lastupdate + max(ragdollmillis - ragdollfade, 0)))/min(ragdollmillis, ragdollfade), 0.0f, 1.0f);
            renderplayer(d, getplayermodelinfo(d), fade, mainpass);
        }
        if(isthirdperson() && !followingplayer()) renderplayer(player1, getplayermodelinfo(player1), 1, mainpass);
        rendermovables();
        MapEntities::renderentities();
        if(cmode) cmode->rendergame();

        endmodelbatches();
    }

    VARP(hudgun, 0, 1, 1);
    VARP(hudgunsway, 0, 1, 1);
    VARP(chainsawhudgun, 0, 1, 1);
    VAR(testhudgun, 0, 0, 1);

    FVAR(swaystep, 1, 35.0f, 100);
    FVAR(swayside, 0, 0.04f, 1);
    FVAR(swayup, 0, 0.05f, 1);

    float swayfade = 0, swayspeed = 0, swaydist = 0;
    vec swaydir(0, 0, 0);

    void swayhudgun(int curtime)
    {
        DynamicEntity *d = hudplayer();
        if(d->state != CS_SPECTATOR)
        {
            if(d->physstate >= PHYS_SLOPE)
            {
                swayspeed = min(sqrtf(d->vel.x*d->vel.x + d->vel.y*d->vel.y), d->maxspeed);
                swaydist += swayspeed*curtime/1000.0f;
                swaydist = fmod(swaydist, 2*swaystep);
                swayfade = 1;
            }
            else if(swayfade > 0)
            {
                swaydist += swayspeed*swayfade*curtime/1000.0f;
                swaydist = fmod(swaydist, 2*swaystep);
                swayfade -= 0.5f*(curtime*d->maxspeed)/(swaystep*1000.0f);
            }

            float k = pow(0.7f, curtime/10.0f);
            swaydir.mul(k);
            vec vel(d->vel);
            vel.add(d->falling);
            swaydir.add(vec(vel).mul((1-k)/(15*max(vel.magnitude(), d->maxspeed))));
        }
    }

    dynent guninterp;

    SVARP(hudgunsdir, "");

    void drawhudmodel(DynamicEntity *d, int anim, float speed = 0, int base = 0) {}

    void drawhudgun() {}

    void renderavatar()
    {
        drawhudgun();
    }

    vec hudgunorigin(int gun, const vec &from, const vec &to, DynamicEntity *d)
    {
        if(d->muzzle.x >= 0) return d->muzzle;
        vec offset(from);
        if(d!=hudplayer() || isthirdperson())
        {
            vec front, right;
            vecfromyawpitch(d->yaw, d->pitch, 1, 0, front);
            offset.add(front.mul(d->radius));
            if(d->type!=ENT_AI)
            {
                offset.z += (d->aboveeye + d->eyeheight)*0.75f - d->eyeheight;
                vecfromyawpitch(d->yaw, 0, 0, -1, right);
                offset.add(right.mul(0.5f*d->radius));
                offset.add(front);
            }
            return offset;
        }
        offset.add(vec(to).sub(from).normalize().mul(2));
        if(hudgun)
        {
            offset.sub(vec(camup).mul(1.0f));
            offset.add(vec(camright).mul(0.8f));
        }
        else offset.sub(vec(camup).mul(0.8f));
        return offset;
    }

    void preload()
    {
    	preloadplayermodel("uh/chars/man");
        entities::preloadentities();
    }

	const char *animname(int i)
	{
		i &= 0x7F;
		if(i >= NUMANIMS) return "";
		return animnames[i];
	}

	const int numanims() {return NUMANIMS;}
}

