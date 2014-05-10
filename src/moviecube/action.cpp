/*
 * action.cpp
 *
 *  Created on: 04.12.2009
 *      Author: offtools
 */

#include "game.h"
#include "action.h"
#include "clientmode.h"

/// TODO (offtools#1#): in editmode: change player pos to spawnpoint if spawned
/// TODO (offtools#1#): prevent removing of used waypoints or notify
/// TODO:(offtools#1#): check deletecontents

//-------------------------------------------------------
GenericAction::GenericAction() {}
GenericAction::~GenericAction() {}
void GenericAction::finished(ControlCharacter& target)
{
//	conoutf("DEBUG: GenericAction::finished");
    target.event(ACTIONINSTANCE_EVENT_STEP_FINISH);
}

ActionSpawn::ActionSpawn(int uid)
{
    loopv(MapEntities::ents) if(MapEntities::ents[i]->uid == uid)
    {
        ent = i;
        break;
    }
}

ActionSpawn::~ActionSpawn() {}

void ActionSpawn::process(ControlCharacter& target)
{
    using namespace MapEntities;

    StaticEntity* e = ents[ent];
    vec spawn = vec(e->o);
    droptofloor(spawn, 0, 0);
    target.dynent->o = spawn;
    setbbfrommodel(target.dynent, game::playermodels[target.dynent->charinfo.playermodel]->name); // Protagoras: bounding box support
    target.dynent->o.z += target.dynent->eyeheight;
    target.dynent->yaw = e->attr[0];

    if(!editmode)
    {
        game::addmsg(SV_TRYSPAWN, "rc", target.dynent);
    }
    target.event(ACTIONINSTANCE_EVENT_SPAWN);
    finished(target);
}

ActionWait::ActionWait(int _millis) : millis(_millis) {}

ActionWait::~ActionWait() {}

void ActionWait::process(ControlCharacter& target)
{
    if ( (lastmillis - target.dynent->lastaction ) > millis )
    {
        finished(target);
        return;
    }
    moveplayer(target.dynent, 1, false); // Protagoras, FIX: no one has cancelled the physics yet!
}

ActionMove::ActionMove(int uid)
{
    loopv(MapEntities::ents) if(MapEntities::ents[i]->uid == uid)
    {
        ent = i;
        break;
    }
}

ActionMove::~ActionMove() {}

void ActionMove::process(ControlCharacter& target)
{
    using namespace MapEntities;

	StaticEntity* e = ents[ent];
	vec wp = e->o;
	float dist = wp.dist(vec(target.dynent->o));
	if (dist < e->attr[1]) //waypoint reached: TODO: make depending from speed
	{
		target.dynent->move = target.dynent->strafe = 0;
		finished(target);
		return;
	}
	vec dir(wp);
	dir.sub(target.dynent->o);
	dir.normalize();
	target.dynent->move = 1;
//	if (d->flying) { vectoyawpitch(dir, d->yaw, d->pitch); }
//	else { float dummy; vectoyawpitch(dir, d->yaw, dummy); }
	float dy, dp, dc; //Protagoras: smooth character turns
	vectoyawpitch(dir, dy, dp);
	if (!(target.dynent->inwater)) dp = 0.0f;
	dy -= target.dynent->yaw;
	while (dy <= -180.0f) dy += 360.0f; // Normalizing
	while (dy > 180.0f) dy -= 360.0f;
	dp -= target.dynent->pitch;
	dc = (lastmillis - target.lastprocess) * target.dynent->maxspeed / (5000.0f * RAD);
	dy = (dy < -dc) ? -dc : dy; // Clamping
	dy = (dy > dc) ? dc : dy;
	dp = (dp < -dc) ? -dc : dp;
	dp = (dp > dc) ? dc : dp;
	target.dynent->yaw += dy;
	target.dynent->pitch += dp;
	if (game::smoothmove && target.dynent->smoothmillis>0) game::predictplayer(target.dynent, true);
	else moveplayer(target.dynent, 1, false);
}

//Protagoras: implemented jump
ActionJump::ActionJump() {}

ActionJump::~ActionJump() {}

void ActionJump::process(ControlCharacter& target)
{
	target.dynent->jumping = true;
	moveplayer(target.dynent, 1, false);
	finished(target);
}

//ActionFace, added by Protagoras
ActionFace::ActionFace(int _cuid) : cuid(_cuid) {}

ActionFace::~ActionFace() {}

void ActionFace::process(ControlCharacter& target)
{
	if (game::controls[cuid] == &target || cuid >= game::controls.length())
	{
		finished(target);
		return;
	}
	vec dir(game::controls[cuid]->dynent->o);
	dir.sub(target.dynent->o);
	dir.normalize();
	float dy, dp, dc;
	vectoyawpitch(dir, dy, dp);
	dy -= target.dynent->yaw;
	while (dy <= -180.0f) dy += 360.0f;
	while (dy > 180.0f) dy -= 360.0f;
	dp -= target.dynent->pitch;
	if (abs(dy) < 1.0f && abs(dp) < 1.0f)
	{
		finished(target);
		return;
	}
	dc = (lastmillis - target.lastprocess) * target.dynent->maxspeed / (5000.0f * RAD);
	dy = (dy < -dc) ? -dc : dy;
	dy = (dy > dc) ? dc : dy;
	dp = (dp < -dc) ? -dc : dp;
	dp = (dp > dc) ? dc : dp;
	target.dynent->yaw += dy;
	target.dynent->pitch += dp;
	moveplayer(target.dynent, 1, false);
}

//ActionExpect, added by Protagoras
ActionExpect::ActionExpect(int _cuid, int _radius) : cuid(_cuid), radius(_radius) {}

ActionExpect::~ActionExpect() {}

void ActionExpect::process(ControlCharacter& target)
{
	if (game::controls[cuid] == &target || cuid >= game::controls.length())
	{
		finished(target);
		return;
	}
	vec tgt = game::controls[cuid]->dynent->o;
	float dist = tgt.dist(vec(target.dynent->o));
	if (dist < radius)
	{
		finished(target);
		return;
	}
	moveplayer(target.dynent, 1, false);
}

//ActionFollow, added by Protagoras
ActionFollow::ActionFollow(int _cuid, int uid) : cuid(_cuid)
{
    loopv(MapEntities::ents) if(MapEntities::ents[i]->uid == uid)
    {
        ent = i;
        break;
    }
}

ActionFollow::~ActionFollow() {}

void ActionFollow::process(ControlCharacter& target)
{
	using namespace MapEntities;

	StaticEntity* e = ents[ent];
	vec wp = e->o;
	float dist = wp.dist(vec(target.dynent->o));
	if (dist < e->attr[1]) // TODO: make depending from speed
	{
		target.dynent->move = target.dynent->strafe = 0;
		finished(target);
		return;
	}
	vec dir;
	if (game::controls[cuid] == &target || cuid >= game::controls.length())
	{
		dir = vec(wp);
		target.dynent->move = 1;
	}
	else
	{
		dir = vec(game::controls[cuid]->dynent->o);
		if (dir.dist(vec(target.dynent->o)) > 30.0f) target.dynent->move = 1; // hehehe...
		if (dir.dist(vec(target.dynent->o)) < 20.0f) target.dynent->move = 0;
	}
	dir.sub(target.dynent->o);
	dir.normalize();
	float dy, dp, dc;
	vectoyawpitch(dir, dy, dp);
//	if (!(target.dynent->inwater)) dp = 0.0f; // Protagoras: hopefully not needed
	dy -= target.dynent->yaw;
	while (dy <= -180.0f) dy += 360.0f;
	while (dy > 180.0f) dy -= 360.0f;
	dp -= target.dynent->pitch;
	dc = (lastmillis - target.lastprocess) * target.dynent->maxspeed / (5000.0f * RAD);
	dy = (dy < -dc) ? -dc : dy;
	dy = (dy > dc) ? dc : dy;
	dp = (dp < -dc) ? -dc : dp;
	dp = (dp > dc) ? dc : dp;
	target.dynent->yaw += dy;
	target.dynent->pitch += dp;
	if (game::smoothmove && target.dynent->smoothmillis>0) game::predictplayer(target.dynent, true);
	else moveplayer(target.dynent, 1, false);
}

ActionSpeed::ActionSpeed(int _speed)
{
	clamp(_speed, 1, 1000);
	speed = _speed;
}

ActionSpeed::~ActionSpeed() {}

void ActionSpeed::process(ControlCharacter& target)
{
	target.dynent->maxspeed = speed;
	finished(target);
}

ActionDestroy::ActionDestroy() {}

ActionDestroy::~ActionDestroy() {}

void ActionDestroy::process(ControlCharacter& target)
{
    target.disconnect();
	finished(target);
}

ActionModel::ActionModel(int _model) : model(_model) {}

ActionModel::~ActionModel() {}

void ActionModel::process(ControlCharacter& target)
{
	CharacterInfo& info = target.dynent->getcharinfo();
	info.setplayermodel(model);
	// Protagoras: bounding box support
	target.dynent->o.z -= target.dynent->eyeheight;
	setbbfrommodel(target.dynent, game::playermodels[info.playermodel]->name);
	target.dynent->o.z += target.dynent->eyeheight;
	target.dynent->resetinterp();
	game::addmsg(SV_SWITCHMODEL, "rci", target.dynent, info.getplayermodel());
	finished(target);
}

ActionSetAO::ActionSetAO(int _ao, int _anim) : ao(_ao), anim(_anim) {}

ActionSetAO::~ActionSetAO() {}

void ActionSetAO::process(ControlCharacter& target)
{
	CharacterInfo& info = target.dynent->getcharinfo();
    info.setao(ao, anim);
	finished(target);
}

ActionSetGesture::ActionSetGesture(int _anim) : anim(_anim) {}

ActionSetGesture::~ActionSetGesture() {}

void ActionSetGesture::process(ControlCharacter& target)
{
	CharacterInfo& info = target.dynent->getcharinfo();
    info.setactivegesture(anim);
	finished(target);
}

ActionDoGesture::ActionDoGesture(int _millis) : millis(_millis) {}

ActionDoGesture::~ActionDoGesture() {}

void ActionDoGesture::process(ControlCharacter& target)
{
	CharacterInfo& info = target.dynent->getcharinfo();
	if ( (lastmillis - target.dynent->lastaction ) > millis ) //Protagoras: removed dynamic cast
    {
        info.dogesture = false;
        finished(target);
    }
    else
    {
        info.dogesture = true;
        moveplayer(target.dynent, 1, false);
    }
}

ActionAttach::ActionAttach(int _part, char* _model, int _rule) : part(_part), rule(_rule)
{
	copystring(model, _model);
}

ActionAttach::~ActionAttach()
{
	delete[] model;
}

void ActionAttach::process(ControlCharacter& target)
{
   	CharacterInfo& info = target.dynent->getcharinfo();
  	info.attached.add(part,model,rule);
   	game::addmsg(SV_ATTACH, "rcisi", target.dynent, part, model, rule);
    finished(target);
}

ActionDetach::ActionDetach(int _part) : part(_part) {}

ActionDetach::~ActionDetach() {}

void ActionDetach::process(ControlCharacter& target)
{
    CharacterInfo& info = target.dynent->getcharinfo();
    info.attached.remove(part);
    game::addmsg(SV_DETACH, "rci", target.dynent, part);
    finished(target);
}

ActionCommand::ActionCommand(const char* cmd) : command(cmd && *cmd ? newstring(cmd) : NULL) {}

ActionCommand::~ActionCommand()
{
	delete[] command;
}

void ActionCommand::process(ControlCharacter& target)
{
	if(command && !editmode) //#offtools: only execute if not in editmode to prevent recursion
	{
		execute(command);
	}
	finished(target);
}

ActionCond::ActionCond(const char* t, const char* b) : test(t && *t ? newstring(t) : NULL), body(b && *b ? newstring(b) : NULL) {}

ActionCond::~ActionCond()
{
	delete[] test;
	delete[] body;
}

void ActionCond::process(ControlCharacter& target)
{
	if( (!test || execute(test)) && !editmode )
	{
		if(body) execute(body);
	}
	finished(target);
}

ActionCondWait::ActionCondWait(const char* t) : test(t && *t ? newstring(t) : NULL) {}

ActionCondWait::~ActionCondWait()
{
	delete[] test;
}

void ActionCondWait::process(ControlCharacter& target)
{
	if( (!test || execute(test)) && !editmode )
	{
		finished(target);
	}
	moveplayer(target.dynent, 1, false);
}

//-------ActionState----------------------------------
ActionState::ActionState(const char*  descr)
{
//    conoutf("DEBUG: ActionState::ActionState %s", descr);
    copystring(statedescr, descr, MAXSTRLEN);
}

ActionState::~ActionState()
{
    steps.deletecontents();
}

bool ActionState::hasstep(int i)
{
	return steps.inrange(i);
}

void ActionState::append(GenericAction *a)
{
	steps.add(a);
}

void ActionState::insert(int pos, GenericAction *a)
{
    if(steps.inrange(pos)) { steps.insert(pos, a); }
}

void ActionState::remove(int pos)
{
    if(steps.inrange(pos))
    {
        GenericAction* a = steps.remove(pos);
        DELETEP(a);
    }
}

GenericAction* ActionState::getaction(int i)
{
	if(steps.inrange(i)) return steps[i];
	else return NULL;
}

//---------ActionLib-----------------------------------
ActionLib::ActionLib(const char* descr) : usage(0)
{
	copystring(librarydescr, descr, MAXSTRLEN);
	if (! addstate("init")) fatal("add action state: could not add action state");
}
ActionLib::~ActionLib()
{
    //cleanup characters
    if(usage)
    {
//        conoutf("DEBUG: ActionLib::~ActionLib - usage: %d > 0", usage);
        notifydelete();
    }
    states.deletecontents();
}

bool ActionLib::hasstate(int state)
{
	return states.inrange(state);
}

bool ActionLib::hasstep(int state, int step)
{
	return hasstate(state) && states[state]->hasstep(step);
}

bool ActionLib::hasnextstep(int state, int step)
{
	return hasstep(state, step + 1);
}

GenericAction* ActionLib::getaction(int state, int step)
{
	ActionState* st = getstate(state);
	if(!st) { return NULL; }
	return st->getaction(step);
}

ActionState* ActionLib::getstate(int state)
{
	if(states.inrange(state)) { return states[state]; }
	else { return NULL; }
}

int ActionLib::addstate(const char *descr)
{
	states.add(new ActionState(descr));
    notify(ACTIONINSTANCE_EVENT_STATE_APPEND);
	return states.length();
}

void ActionLib::appendstep(int state, GenericAction* action)
{
    if(hasstate(state))
    {
        states[state]->append(action);
        notify(ACTIONINSTANCE_EVENT_STEP_APPEND);
    }
}

void ActionLib::notify(int ev)
{
    loopv(game::controls)
    {
        ControlCharacter* ca = (ControlCharacter*)game::controls[i];
        if(ca->type() == CONTROL_CHARACTER && this == &ca->lib)
        {
//            conoutf("DEBUG: ActionLib::notify lib: %s, cn: %d event %d", librarydescr, ca->dynent->clientnum, ev);
            ca->event(ev);
        }
    }
}

void ActionLib::notifydelete()
{
    if(usage)
    {
        loopv(game::controls)
        {
            ControlCharacter* ca = (ControlCharacter*)game::controls[i];
            if(ca->type() == CONTROL_CHARACTER && &(ca->lib) == this)
            {
//                conoutf("DEBUG: ActionLib::notifydelete Control cn: %d", ca->dynent->clientnum);
                ca->disconnect();
            }
        }
    }
}

//---------ActionInstance-------------------------------
ActionInstance::ActionInstance(ActionLib& _lib) : lib(_lib), curstate(0), curstep(0), actionstate(ACTION_STATE_IDLE), actionpolicy(ACTION_POLICY_ON_STATE_END_WAIT), localuid(0), lastprocess(0), plastaction(NULL)
{
    lib.usage++;
}

ActionInstance::~ActionInstance() { lib.usage--; }

int ActionInstance::getcurstate() { return curstate; }

int ActionInstance::getcurstep() { return curstep; }

void ActionInstance::setcurstate(int state)
{
	if(!lib.hasstate(state)) return;
	curstate = state;
}

void ActionInstance::next()
{
	if(lib.hasstep(curstate, curstep + 1))
	{
//		conoutf("DEBUG: ActionInstance::next");
		curstep++;
	}
	else
	{
        event(ACTIONINSTANCE_EVENT_STATE_FINISH);
	}
	*plastaction = lastmillis;
}

//TODO: cleanup, move most stuff into handlers
void ActionInstance::event(int ev)
{
    switch(ev)
    {
        case ACTIONINSTANCE_EVENT_SPAWN:
        {
            actionstate = ACTION_STATE_PROCESS;
            break;
        }
        case ACTIONINSTANCE_EVENT_INIT:
        {
            actionstate = ACTION_STATE_INIT;
            *plastaction = lastmillis;
            break;
        }
        case ACTIONINSTANCE_EVENT_STEP_FINISH:
        {
            next();
            break;
        }
        case ACTIONINSTANCE_EVENT_STEP_APPEND:
        {
            if(actionstate == ACTION_STATE_END || ACTION_STATE_INIT)
            {
                actionstate = ACTION_STATE_PROCESS;
                *plastaction = lastmillis;
            }
            break;
        }
        case ACTIONINSTANCE_EVENT_STATE_FINISH:
        {
            if(!editmode) finished();
            break;
        }
        case ACTIONINSTANCE_EVENT_STATE_APPEND:
        {
            break;
        }
        case ACTIONINSTANCE_EVENT_STATE_DELETE:
        case ACTIONINSTANCE_EVENT_STEP_DELETE:
        default:
            conoutf("DEBUG: ActionInstance::event - handling event: %d not implemented", ev);
            break;
    }
}

void ActionInstance::finished()
{
    switch(actionpolicy)
    {
        case ACTION_POLICY_ON_STATE_END_WAIT:
        {
            actionstate = ACTION_STATE_END;
            break;
        }
        case ACTION_POLICY_ON_STATE_END_LOOP:
        {
            curstep = 0;
            actionstate = ACTION_STATE_PROCESS;
            break;
        }
        case ACTION_POLICY_ON_STATE_END_NEXT:
        default:
        {
            actionstate = ACTION_STATE_IDLE;
            break;
        }
    }
}

//BUG: set correct states in editmode - endless loop through last step
void ActionInstance::process(ControlCharacter& target)
{
    if(actionstate == ACTION_STATE_IDLE || actionstate == ACTION_STATE_END) return;

    if(actionstate == ACTION_STATE_INIT && editmode) { target.dynent->yaw += 0.25f; return; }

    if(GenericAction *a = lib.getaction(curstate, curstep))
    {
        a->process(target);
    }
    else
    {
        event(ACTIONINSTANCE_EVENT_STATE_FINISH);
    }

    lastprocess = lastmillis;
}

// TODO (offtools#1#): implement nextstep - step through commands
void ActionInstance::nextstep(ControlCharacter& target) {}

// TODO (offtools#1#): implement laststep - step to last, process steps between if necessary
void ActionInstance::laststep(ControlCharacter& target) {}
//-----------------------------------------------------

vector<ActionLib*> actionlibrary;

ActionLib* findcharacter(const char* s)
{
	loopv(actionlibrary) if(strcmp(actionlibrary[i]->librarydescr, s) == 0)
	{
		return actionlibrary[i];
	}
	return NULL;
}

//--------------Character Commands----------------------------
//adds new actionlibrary and spawns character if in editmode
bool newcharacter(const char* s)
{
    if(!findcharacter(s))
    {
        ActionLib* library = actionlibrary.add(new ActionLib(s));
        if(editmode)
        {
            ControlCharacter* controler = new ControlCharacter(*library);
            game::controls.add(controler);
            game::addmsg(SV_REQUESTBOT, "r");
			return true;
        }
    }
    else
    {
        conoutf("game::newcharacter: actionlib %s already exits", s);
    }
	return false;
}
//
int lastuid = 0;

//reset on mapchange
void clearactions() { lastuid = 0; }

//TODO: join stuff from addchar and showchar into one function

//only works in nonedit mode
//adds an character based on character from actionlib [s]
int addcharacter(const char *s)
{
    if(editmode) return 0;
    ActionLib* lib = findcharacter(s);
    if(lib)
    {
        ControlCharacter* controler = new ControlCharacter(*lib);
        controler->localuid = ++lastuid;
        game::controls.add(controler);
        game::addmsg(SV_REQUESTBOT, "r");
    	return controler->localuid;
    }
    else
    {
        conoutf("game::addcharacter: could not add character, actionlib %s not found", s);
        return 0;
    }
}

void showcharacter(char *s)
{
	if(!editmode) return;
	ActionLib* lib = findcharacter(s);
    if(lib)
    {
        ControlCharacter* controler = new ControlCharacter(*lib);
        game::controls.add(controler);
        game::addmsg(SV_REQUESTBOT, "r");
    }
    else
    {
        conoutf("game::addcharacter: could not add character, actionlib %s not found", s);
    }
}

void listcharacters()
{
    vector<char> buf;
    string lst;
    loopv(actionlibrary)
    {
        formatstring(lst)("%s", actionlibrary[i]->librarydescr);
        buf.put(lst, strlen(lst));
        if(i < (actionlibrary.length()-1)) buf.add(' ');
    }
    buf.add('\0');
    result(buf.getbuf());
}

//TODO: deletes character from actionlib and removes all controls using it
void deletecharacter(const char* s, bool force = false)
{
    ActionLib* lib = findcharacter(s);
    if(!lib)
    {
        conoutf("deletecharacter: actionlib not found");
        return;
    }

    if(editmode || !lib->usage || force)
    {
        conoutf("ERROR: deletecharacter: delete actionlib");
        DELETEP(lib);
    }
}

//--------------Character Definition Commands----------------------------
//following commands manipulate character definitions
void actionappend(char *s, int state, GenericAction* a)
{
    ActionLib* lib = findcharacter(s);
    if(lib && lib->hasstate(state))
    {
        lib->appendstep(state, a);
    }
    else
    {
        conoutf("ERROR: actionappend - ActionLib or State does not exist");
        DELETEP(a);
    }
}

void actionappendspawn(char *s, int state, int uid)
{
    if(MapEntities::findmapuid(uid, WAYPOINT))
        actionappend(s, state, new ActionSpawn(uid));
    else
        conoutf("actionappendspawn: waypoint not found");
}

void actionappenddestroy(char *s, int state)
{
	actionappend(s, state, new ActionDestroy());
}

void actionappendwait(char *s, int state, int millis)
{
    actionappend(s, state, new ActionWait(millis));
}

void actionappendmove(char *s, int state, int uid)
{
    if(MapEntities::findmapuid(uid, WAYPOINT))
        actionappend(s, state, new ActionMove(uid));
    else
        conoutf("actionappendmove: waypoint not found");
}

//added by Protagoras
void actionappendjump(char *s, int state)
{
	actionappend(s, state, new ActionJump());
}

//added by Protagoras
void actionappendface(char *s, int state, int cuid)
{
	actionappend(s, state, new ActionFace(cuid));
}

//added by Protagoras
void actionappendexpect(char *s, int state, int cuid, int radius)
{
	actionappend(s, state, new ActionExpect(cuid, radius));
}

//added by Protagoras
void actionappendfollow(char *s, int state, int cuid, int uid)
{
    if(MapEntities::findmapuid(uid, WAYPOINT))
        actionappend(s, state, new ActionFollow(cuid, uid));
    else
        conoutf("actionappendfollow: waypoint not found");
}

void actionappendspeed(char *s, int state, int speed)
{
	actionappend(s, state, new ActionSpeed(speed));
}

void actionappendmodel(char *s, int state, int model)
{
	actionappend(s, state, new ActionModel(model));
}

void actionappendsetao(char *s, int state, int ao, int anim)
{
	actionappend(s, state, new ActionSetAO(ao, anim));
}

void actionappendsethold(char *s, int state, int anim)
{
	actionappend(s, state, new ActionSetAO(AO_HOLD, anim));
}

void actionappendsetgesture(char *s, int state, int anim)
{
	actionappend(s, state, new ActionSetGesture(anim));
}

void actionappendgesture(char *s, int state, int millis)
{
	actionappend(s, state, new ActionDoGesture(millis));
}

void actionappendattach(char *s, int state, int part, char* model, int rule)
{
	actionappend(s, state, new ActionAttach(part, model, rule));
}

void actionappenddetach(char *s, int state, int part)
{
	actionappend(s, state, new ActionDetach(part));
}

void actionappendcommand(char* s, int state, char* cmd)
{
	actionappend(s, state, new ActionCommand(cmd));
}

void actionappendcond(char* s, int state, char* test, char* body)
{
	actionappend(s, state, new ActionCond(test, body));
}

void actionappendcondwait(char* s, int state, char* test)
{
	actionappend(s, state, new ActionCondWait(test));
}

//--------------Character Commands----------------------------
//following commands manipulate an spawned instance of an character,
//instances are referenced by their uid as return value on executing
//the addcharacter command

int clientnumfromuid(int requid)
{
	loopv(game::controls)
	{
		ControlCharacter* ca = (ControlCharacter*)game::controls[i];
		if(ca->type() == CONTROL_CHARACTER && ca->localuid == requid) { return ca->dynent->clientnum; }
	}
	return -1;
}

void loopcharacter(int id, bool loop = true)
{
	loopv(game::controls)
	{
		ControlCharacter* ca = (ControlCharacter*)game::controls[i];
		if(ca->type() == CONTROL_CHARACTER && ca->localuid == id)
		{
		    loop ? ca->actionpolicy = ACTION_POLICY_ON_STATE_END_LOOP : ca->actionstate = ACTION_POLICY_ON_STATE_END_WAIT;
        }
	}
}

ICOMMAND(newcharacter, "s", (char *s), intret(newcharacter(s)));
ICOMMAND(showcharacter, "s", (char *s), showcharacter(s));
ICOMMAND(listcharacters, "", (), listcharacters());
ICOMMAND(addcharacter, "s", (char *s), intret(addcharacter(s)));
ICOMMAND(deletecharacter, "si", (char *s, int *i), deletecharacter(s, *i); );

ICOMMAND(appendactionspawn, "sii", (char *s, int *state, int *uid), actionappendspawn(s, *state, *uid));
ICOMMAND(appendactiondestroy, "si", (char *s, int *state), actionappenddestroy(s, *state));
ICOMMAND(appendactionwait, "sii", (char *s, int *state, int *m), actionappendwait(s, *state, *m));
ICOMMAND(appendactionmove, "sii", (char *s, int *state,  int *uid), actionappendmove(s, *state, *uid));
ICOMMAND(appendactionspeed, "sii", (char *s, int *state,  int *speed), actionappendspeed(s, *state, *speed));
ICOMMAND(appendactionmodel, "sii", (char *s, int *state,  int *model), actionappendmodel(s, *state, *model));
ICOMMAND(appendactionsetao, "siii", (char *s, int *state,  int *ao, int *anim), actionappendsetao(s, *state, *ao, *anim));
ICOMMAND(appendactionsetgesture, "sii", (char *s, int *state,  int *anim), actionappendsetgesture(s, *state, *anim));
ICOMMAND(appendactionsethold, "sii", (char *s, int *state, int *anim), actionappendsethold(s, *state, *anim));
ICOMMAND(appendactiongesture, "sii", (char *s, int *state, int *time), actionappendgesture(s, *state, *time));
ICOMMAND(appendactionattach, "siisi", (char *s, int *state,  int *part, char *model, int *rule), actionappendattach(s, *state, *part, model, *rule));
ICOMMAND(appendactiondetach, "sii", (char *s, int *state, int *rule), actionappenddetach(s, *state, *rule));
ICOMMAND(appendactioncommand, "sis", (char *s, int *state, char* cmd), actionappendcommand(s, *state, cmd));
ICOMMAND(appendactioncond, "siss", (char *s, int *state, char* test, char* body), actionappendcond(s, *state, test, body));
ICOMMAND(appendactioncondwait, "siss", (char *s, int *state, char* test), actionappendcondwait(s, *state, test));
//jump, face, expect, follow by Protagoras
ICOMMAND(appendactionjump, "si", (char *s, int *state), actionappendjump(s, *state));
ICOMMAND(appendactionface, "sii", (char *s, int *state, int *cuid), actionappendface(s, *state, *cuid));
ICOMMAND(appendactionexpect, "siii", (char *s, int *state, int *cuid, int *radius), actionappendexpect(s, *state, *cuid, *radius));
ICOMMAND(appendactionfollow, "siii", (char *s, int *state, int *cuid, int *uid), actionappendfollow(s, *state, *cuid, *uid));

//-------------Character Instance Commands------------------------------------
ICOMMAND(getclientfromuid, "i", (int *requid), intret(clientnumfromuid(*requid)));
ICOMMAND(loopcharacter, "ii", (int *id, int *loop), loopcharacter(*id, *loop); );

//-------------Character Script Proxy (saving stuff)--------------------------
CommandProxy::CommandProxy() {}

CommandProxy::~CommandProxy()
{
	commands.deletecontents();
}

void CommandProxy::addcmd(char **args, int *numargs)
{

	vector<char> buf;
	string lst;
	char* ret;

	for (int i = 0; i < *numargs; i++)
	{
		formatstring(lst)("%s", args[i]);
		buf.put(lst, strlen(lst));
		if(i < ( *numargs-1 )) buf.add(' ');
	}
	buf.add('\0');
	commands.add(newstring(buf.getbuf()));
	ret = executestr(commands.last());
	if (ret) {
		result(ret);
	}
}

void CommandProxy::save()
{
	defformatstring(fname)("packages/base/movie/%s.cfg", game::getclientmap());
	stream* f = openrawfile(fname, "wb");
	loopv(commands)
	{
		f->putline(commands[i]);
	}
	f->close();
}

void CommandProxy::clear()
{
	commands.deletecontents();
}

CommandProxy proxy;

ICOMMAND(appendactionproxy, "V", (char** c, int *i), proxy.addcmd(c, i));
ICOMMAND(saveactions, "", (), proxy.save());
ICOMMAND(clearactions, "", (), proxy.clear());
