#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "game.h"
#include "entityscript/command.h"
//Cutscene Instance for cutscenes
//using namespace EntityScript;


class Cutscene : public EntityScript::Instance, public physent
{
public:
    Cutscene(EntityScript::Script &s);
    ~Cutscene();

	void debug();
};

//base class of scene actions
class ActionScene : public EntityScript::Action
{
public:
    ActionScene(int d = 0);
	virtual ~ActionScene();

    float multiplier(bool total);

protected:
    int startmillis;
    int duration;
};

class ActionCameraMove : public ActionScene
{
public:
    ActionCameraMove(extentity& e, int d);
    ~ActionCameraMove();

    bool process();

private:
    extentity& ent;
    float dx, dy, dz;
    vec start;
    vec dest;
};

class CommandMove : public Command<Cutscene, ActionCameraMove>
{
	public:

}

namespace Cutscenes
{
	void Init();
//    void Update();
}

#endif // CAMERA_H_INCLUDED
