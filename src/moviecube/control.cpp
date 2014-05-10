/*
 * control.cpp
 *
 *  Created on: 08.12.2009
 *      Author: offtools
 */

#include "game.h"

//--------------Control Player-----------------------

void ControlPlayer::init(DynamicEntity* de)
{
	dynent = de;
	dynent->controltype = CONTROL_PLAYER;
	isinit = true;
}

void ControlPlayer::spawn() {}

void ControlPlayer::update()
{
    if(isinit)
    {
        moveplayer(dynent, 10, true);
        game::swayhudgun(curtime);
        entities::checkitems(dynent);
    }
}

void ControlPlayer::disconnect() {}

Control* ControlPlayer::cleanup() { return NULL; }

//--------------Control Remote-----------------------
ControlRemote::~ControlRemote()
{
    cleanup();
}

void ControlRemote::init(DynamicEntity* de)
{
	dynent = de;
	dynent->controltype = CONTROL_REMOTE;
	isinit = true;
}

void ControlRemote::spawn() {}

void ControlRemote::update()
{
    if(isinit)
    {
        //game::otherplayers
        if(dynent->state==CS_DEAD && dynent->ragdoll) moveragdoll(dynent);

        const int lagtime = lastmillis-dynent->lastupdate;
        if(!lagtime) {
            return;
        }
        else if(lagtime>1000 && dynent->state==CS_ALIVE)
        {
            dynent->state = CS_LAGGED;
            return;
        }
        if(dynent->state==CS_ALIVE || dynent->state==CS_EDITING)
        {
            if(game::smoothmove && dynent->smoothmillis>0) game::predictplayer(dynent, true);
            else moveplayer(dynent, 1, false);
        }
        else if(dynent->state==CS_DEAD && !dynent->ragdoll) moveplayer(dynent, 1, true);
    }
}

void ControlRemote::disconnect() {}

Control* ControlRemote::cleanup()
{
    game::controls.removeobj(this);
    game::clientdisconnected(dynent->clientnum);
    return this;
}

//--------------Control Ai-----------------------

void ControlAi::init(DynamicEntity* de)
{
	dynent = de;
    dynent->controltype = CONTROL_AI;
	copystring(dynent->name, "blood-ai");

	isinit = true;
}
void ControlAi::spawn() {}
void ControlAi::update() {}
void ControlAi::disconnect() {}
Control* ControlAi::cleanup() { return NULL; }

//--------------Control Actor-----------------------
ControlCharacter::ControlCharacter(ActionLib& _lib) : Control(), ActionInstance(_lib) {}

ControlCharacter::~ControlCharacter() { }

void ControlCharacter::init(DynamicEntity* de)
{
	if(!de) fatal("Controler::Character : no valid Dynamic Entity. Exit!");
	dynent = de;

	copystring(dynent->name, lib.librarydescr);

// TODO (offtools#1#): set and send basic infos like name, model ...

//    filtertext(dynent->name, lib.librarydescr, false, MAXNAMELEN);
//    addmsg(SV_SWITCHNAME, "rs", dynent->name);

	dynent->controltype = CONTROL_CHARACTER;
	dynent->lastupdate = lastmillis;
	plastaction = &(dynent->lastaction); //added by Protagoras

	//first spawn, no actions defined in Actionlib
	if(editmode && !lib.states[0]->steps.length())
	{
// TODO (offtools#1#): fix if author under floor
		dynent->o = vec(game::player1->o);
		droptofloor(dynent->o, 0, 0);
        vec sp = vec(0,0,0);
        vecfromyawpitch(game::player1->yaw, 0, 50, 0, sp);
        dynent->o.add(sp);
        dynent->o.z += dynent->eyeheight;

	    game::addmsg(SV_TRYSPAWN, "rc", dynent);
	    event(ACTIONINSTANCE_EVENT_INIT);
	}
	else
	{
	    event(ACTIONINSTANCE_EVENT_INIT);
	}

	isinit = true;
}

void ControlCharacter::spawn() {}

void ControlCharacter::update()
{
    if(isinit)
    {
        process(*this);

        if(dynent->state == CS_LAGGED)
        {
            dynent->lastupdate = lastmillis; //TODO: add a real check
        }
    }
}

void ControlCharacter::disconnect()
{
    isinit = false;
    game::addmsg(SV_DELETEBOT, "ri", dynent->clientnum);
//    conoutf("DEBUG: ControlCharacter::disconnect %d", dynent->clientnum);
}

Control* ControlCharacter::cleanup()
{
    game::controls.removeobj(this);
    game::clientdisconnected(dynent->clientnum);
    return this;
}
