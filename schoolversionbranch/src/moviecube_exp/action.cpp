#include "action.h"

ActionMove::ActionMove(extentity& e) : Action(NULL), instance(gettype()), ent(e)
{
	conoutf("ActionMove::ActionMove %p", &instance);
	dst = vec(ent.o);
}

ActionMove::~ActionMove() {}

Control::ControlNPC& ActionMove::gettype()
{
	return dynamic_cast<Control::ControlNPC&>(*QueuedCommandMgr::getcurrent());
}

bool ActionMove::checktype() { return dynamic_cast<Control::ControlNPC*>( &instance ); }

bool ActionMove::process()
{
	float dist = dst.dist(vec(instance.dynent->o));
	if (dist < ent.attr[1]) //waypoint reached: TODO: make depending from speed
	{
		instance.dynent->move = instance.dynent->strafe = 0;
        return true;
	}

	vec dir(dst);
	dir.sub(instance.dynent->o);
	dir.normalize();
	instance.dynent->move = 1;
//	if (d->flying) { vectoyawpitch(dir, d->yaw, d->pitch); }
//	else { float dummy; vectoyawpitch(dir, d->yaw, dummy); }
	float dummy; vectoyawpitch(dir, instance.dynent->yaw, dummy);
    if(game::smoothmove && instance.dynent->smoothmillis>0) game::predictplayer(instance.dynent, true);
    else moveplayer(instance.dynent, 1, false);
    return false;
}

ICOMMAND(cq_move, "i", (int *wp),
    {
		new ActionMove(*entities::getents()[*wp]);
    }
);
