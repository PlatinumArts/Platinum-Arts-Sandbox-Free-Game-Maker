////////////////////////////////////////////////////////
// control.cpp
//
// Created on: 08.12.2009
// Author: offtools
////////////////////////////////////////////////////////

#include "game.h"

namespace Control
{
    int lastid  = -1;

//--------------Control Player-----------------------

    Control::Control() : uid(-1), dynent(NULL), isinit(false) {}
    Control::~Control() {}

//--------------Control Player-----------------------

    ControlPlayer::ControlPlayer() : Control(), k_left(false), k_right(false), k_up(false), k_down(false), k_rise(false), k_des(false), k_yawleft(false), k_yawright(false), k_action1(false), k_action2(false)
    {
        uid = -1;
        dynent = NULL;
        isinit = false;
    }

    ControlPlayer::~ControlPlayer() {}

    void ControlPlayer::connect(DynamicEntity* de)
    {
        dynent = de;
        dynent->controltype = CONTROL_PLAYER;
        isinit = true;
    }

    void ControlPlayer::spawn()
    {
        if(!editmode)
        {
            dynent->state = CS_ALIVE;
            game::addmsg(SV_TRYSPAWN, "rc", dynent);
        }
    }

    void ControlPlayer::update()
    {
        if(isinit)
        {
            ///TODO: check time, make turn depending on fps
            if(k_yawleft) dynent->yaw -= 1;
            else if(k_yawright) dynent->yaw += 1;

            moveplayer(dynent, 10, true);
            ///TODO: move to DynamicEntity
            //game::swayhudgun(curtime);
            //entities::checkitems(dynent);
        }
    }

    void ControlPlayer::disconnect() {}

    Control* ControlPlayer::cleanup() { return NULL; }

    void ControlPlayer::forward(int down)
    {
        k_up = down!=0;
        dynent->move = k_up ? 1 : (k_down ? -1 : 0);
    }

    void ControlPlayer::backward(int down)
    {
        k_down = down!=0;
        dynent->move = k_down ? -1 : (k_up ? 1 : 0);
    }

    void ControlPlayer::right(int down)
    {
        k_right = down!=0;
        dynent->strafe = k_right ? 1 : (k_left ? -1 : 0);
    }

    void ControlPlayer::left(int down)
    {
        k_left = down!=0;
        dynent->strafe = k_left ? -1 : (k_right ? 1 : 0);
    }

    void ControlPlayer::turnleft(int down)
    {
        k_yawleft = down!=0;
    }

    void ControlPlayer::turnright(int down)
    {
        k_yawright = down!=0;
    }

//--------------Control Remote-----------------------
    ControlRemote::ControlRemote() : Control()
    {
        uid = -1;
        dynent = NULL;
        isinit = false;
    }

    ControlRemote::~ControlRemote()
    {
        cleanup();
    }

    void ControlRemote::connect(DynamicEntity* de)
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
        controls.removeobj(this);
        game::clientdisconnected(dynent->clientnum);
        return this;
    }

//--------------Control Ai-----------------------
    ControlAi::ControlAi()
    {
        uid = ++lastid;
        dynent = NULL;
        isinit = false;
    }

    void ControlAi::connect(DynamicEntity* de)
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
    ControlNPC::ControlNPC(QueuedCommand& qu) : Control(), Instance(qu)
    {
        if(!editmode)
        {
            uid = ++lastid;
        }
        else
        {
            uid = -1;
        }
        dynent = NULL;
		registersignal("reachedwaypoint");
		registersignal("collidewall");
		registersignal("collideent");

        isinit = false;
    }

    ControlNPC::~ControlNPC() { }

    void ControlNPC::connect(DynamicEntity* de)
    {
        if(!de) fatal("Controler::Character : no valid Dynamic Entity. Exit!");
        dynent = de;

///TODO: set name ActionInstance
		defformatstring(s)("%p", this);
        copystring(dynent->name, s);

///TODO: set and send basic infos like name, model ...

//        filtertext(dynent->name, lib.librarydescr, false, MAXNAMELEN);
        game::addmsg(SV_SWITCHNAME, "rs", dynent->name);

        dynent->controltype = CONTROL_NPC;
        dynent->lastupdate = lastmillis;

///TODO: spawn behaviour in edit and normal mode
        //first spawn, no actions defined in Actionlib
//        if(editmode)
//        {
            dynent->o = vec(game::player1->o);
            droptofloor(dynent->o, 0, 0);
            vec sp = vec(0,0,0);
            vecfromyawpitch(game::player1->yaw, 0, 50, 0, sp);
            dynent->o.add(sp);
            dynent->o.z += dynent->eyeheight;

            game::addmsg(SV_TRYSPAWN, "rc", dynent);
//        }
		Instance::init();
        isinit = true;
    }

    void ControlNPC::spawn() {}

    void ControlNPC::update()
    {
        if(isinit)
        {
            process();
            if(dynent->state == CS_LAGGED)
            {
                dynent->lastupdate = lastmillis; //TODO: add a real check
            }
        }
    }

    void ControlNPC::disconnect()
    {
        isinit = false;
        game::addmsg(SV_DELETEBOT, "ri", dynent->clientnum);
    }

    Control* ControlNPC::cleanup()
    {
        controls.removeobj(this);
        game::clientdisconnected(dynent->clientnum);
        return this;
    }

    ICOMMAND(cq_character, "s", (char *s),
        {
            QueuedCommand* c = QueuedCommandMgr::findqueued(s);
            if(c)
            {

				ControlNPC* controler = new ControlNPC(*c);
				controls.add(controler);
				game::addmsg(SV_REQUESTBOT, "r");
				//return controler->uid;

//                new ControlNPC(*c);
//                character->init();
            }
        }
    );
//////////////////////////////////////////////////////////////////////
//Control - external Interface Functions
//////////////////////////////////////////////////////////////////////

    ControlPlayer& GetPlayer()
    {
        static ControlPlayer player;
        return player;
    }

    ICOMMAND(moveforward, "D", (int *down), { GetPlayer().forward(*down); });
    ICOMMAND(movebackward, "D", (int *down), { GetPlayer().backward(*down); });
    ICOMMAND(moveright, "D", (int *down), { GetPlayer().right(*down); });
    ICOMMAND(moveleft, "D", (int *down), { GetPlayer().left(*down); });
    ICOMMAND(turnleft, "D", (int *down), { GetPlayer().turnleft(*down); });
    ICOMMAND(turnright, "D", (int *down), { GetPlayer().turnright(*down); });

    vector<Control*> controls;
}
