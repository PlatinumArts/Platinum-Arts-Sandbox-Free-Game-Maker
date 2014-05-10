#include "game.h"
#include "extentity.h"

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

    void renderplayer(DynamicEntity *d, float fade, bool mainpass)
    {
        using namespace Character;

        CharacterInfo& info = d->getcharinfo();

        int playermodel = info.playermodel;
        PlayerModel::PlayerModelInfo& mdl = PlayerModel:: getPlayerModelInfo(playermodel);
        const char* name = PlayerModel::GetName(mdl);

        int lastaction = d->lastaction;
        int lastpain = -1;
        bool hold = false;
        int actiondelay = 300;
        bool ragdoll = false;

        //__offtools__: add attachments (clothes)
        int ai = 0;

		int num = info.attacheditems.length() + 1;
		modelattach* attached = new modelattach[num];
        loopv(info.attacheditems)
        {
            PlayerModel::BuildModelAttach(mdl, info.attacheditems[i], attached[i]);
        }

//        if(mainpass)
//        {
//            d->muzzle = vec(-1, -1, -1);
//            a[ai++] = modelattach("tag_muzzle", &d->muzzle);
//        }

	    int anim = hold ? info.ao[AO_HOLD]|ANIM_LOOP : info.ao[AO_IDLE]|ANIM_LOOP; //if !hold set hold to ANIM_IDLE
	    float yaw = d->yaw+90;
	    float pitch = d->pitch;
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
		        else if(lastpain < lastaction && (action < 0 || (d->type != ENT_AI && lastmillis-lastaction < actiondelay)))
		        {
		            anim = action < 0 ? -action : action;
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
	    int flags = MDL_LIGHT;

	    if(d!=player1 && !(anim&ANIM_RAGDOLL)) flags |= MDL_CULL_VFC | MDL_CULL_OCCLUDED | MDL_CULL_QUERY;

	    if(d->type==ENT_PLAYER) flags |= MDL_FULLBRIGHT;
	    else flags |= MDL_CULL_DIST;

	    if(d->state==CS_LAGGED) fade = min(fade, 0.3f);
	    else flags |= MDL_DYNSHADOW;

	    rendermodel(NULL, name, anim, d->feetpos(), yaw, pitch, 0, flags, d, attached[0].tag ? attached : NULL, basetime, 0, fade);
	    DELETEA(attached);
    }

    void renderentities()
    {
    	const vector<extentity*> &ents = entities::getents();
        loopv(ents)
        {
            extentity &e = *ents[i];
            int revs = 10;
            switch(e.type)
            {
                case TELEPORT:
                    if(e.attr[1] < 0) continue;
                    break;
                case GENERIC:
                {
                    float dist = e.o.dist(vec(game::player1->o));
                    if (dist < e.attr[0])
                    {
                        particle_splash(PART_STEAM, 80, 200, e.o, 0xBFBFBF, 2, 150, 10);
                    }
                    break;
                }
                default:
                    if(!e.spawned) continue;
            }
            const char *mdlname = entities::entmodel(e);
            if(mdlname)
            {
                vec p = e.o;
                p.z += 1+sinf(lastmillis/100.0+e.o.x+e.o.y)/20;
                rendermodel(&e.light, mdlname, ANIM_MAPMODEL|ANIM_LOOP, p, lastmillis/(float)revs, 0, 0, MDL_SHADOW | MDL_CULL_VFC | MDL_CULL_DIST | MDL_CULL_OCCLUDED);
            }
        }
    }

    void rendergame(bool mainpass)
    {
        startmodelbatches();

        DynamicEntity *exclude = isthirdperson() ? NULL : followingplayer();
        loopv(players)
        {
            DynamicEntity *d = players[i];
            if(d == player1 || d->state==CS_SPECTATOR || d->state==CS_SPAWNING || d == exclude ) continue;
            renderplayer(d, 1, mainpass);
            copystring(d->info, colorname(d));
        }
        loopv(ragdolls)
        {
            DynamicEntity *d = ragdolls[i];
            float fade = 1.0f;
            if(ragdollmillis && ragdollfade)
            fade -= clamp(float(lastmillis - (d->lastupdate + max(ragdollmillis - ragdollfade, 0)))/min(ragdollmillis, ragdollfade), 0.0f, 1.0f);
            renderplayer(d, fade, mainpass);
        }
        //if(isthirdperson() && !followingplayer()) renderplayer(player1, getplayermodelinfo(player1), 1, mainpass);
        if(isthirdperson() && !followingplayer()) renderplayer(player1, 1, mainpass);
        //if(!followingplayer()) renderplayer(player1, 1, mainpass);

        rendermovables();
        renderentities();
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
    	PlayerModel::RegisterModel("uh/chars/man");
//        entities::preloadentities();
    }
}

