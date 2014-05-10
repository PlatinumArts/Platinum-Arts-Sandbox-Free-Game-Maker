#ifndef ACTION_H_INCLUDED
#define ACTION_H_INCLUDED

#include "game.h"
#include "control.h"

class ActionMove : Action
{
public:
    ActionMove(extentity& wp);
    ~ActionMove();

	bool checktype();
    bool process();

private:
    Control::ControlNPC& instance;
	extentity& ent;
	vec dst;

	Control::ControlNPC& gettype();
};

#endif // ACTION_H_INCLUDED
