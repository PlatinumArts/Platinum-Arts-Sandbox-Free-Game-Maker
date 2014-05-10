#include "camera.h"

namespace Cutscenes
{
//    vector<Cutscene*> cameras;
//    int current = -1;
//
	void Init()
	{
		ScriptManagerRegister<Cutscene>("cutscene");
	}

//    void Update()
//    {
//        if ( current >= 0 )
//        {
//            cameras[current]->process();
//            camera1 = cameras[current];
//        }
//    }
}

Cutscene::Cutscene(EntityScript::Script& s) : EntityScript::Instance(s)
{
	registersignal("reachedwaypoint", SIGNAL_BUILTIN, "i");
}

Cutscene::~Cutscene() {}

void Cutscene::debug()
{
	conoutf("Cutscene::debug");
}

ActionScene::ActionScene(int d) : Action(NULL), startmillis(-1), duration(d) {}
ActionScene::~ActionScene() {}
float ActionScene::multiplier(bool total = false)
{
    if(!duration)
        return 1;

    if(total)
        return min<float>(1, (lastmillis - startmillis) / (float)duration);
    else
    {
        int elapsed = curtime;
        if(lastmillis >= startmillis + duration)
            elapsed -= (lastmillis - startmillis - duration);
        return (float) elapsed / duration;

    }
}


ActionCameraMove::ActionCameraMove(extentity& e, int d) : ActionScene(d), ent(e), dx(0), dy(0), dz(0)
{
	start = dest = vec(0,0,0);
	startmillis = lastmillis;
    vec dest = ent.o;
    start = vec(camera1->o);
    dest.sub(start);
    dx = dest.x;
    dy = dest.y;
    dz = dest.z;
}

ActionCameraMove::~ActionCameraMove() {}

bool ActionCameraMove::process()
{
//    if(startmillis + duration >= lastmillis)
//    {
//        EntityScript::GetInstance()->o = vec(dx, dy, dz).mul(multiplier(true)).add(start);
//        return false;
//    }
//
//    .emmitsignal("reachedwaypoint", 0);
//	conoutf("TEST: ActionCameraMove::process: %p",)
    return true;
}

//ICOMMAND(move, "ii", (int *id, int *d),
//    {
//		new ActionMove<currentinstance>(*entities::getents()[*id], *d);
//    }
//);

ICOMMAND(test_action, "", (),
{
	Action* action = new CommandMove();
}
);
