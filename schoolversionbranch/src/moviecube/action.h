/*
 * action.h
 *
 *  Created on: 04.12.2009
 *      Author: offtools
 */

#ifndef __ACTION_H__
#define __ACTION_H__

/// TODO (offtools#1#): validate new actions (test waypoints, spawns ...)
/// TODO (offtools#2#): add destructors, you ****** ** **** ******** !!!!!!!!

struct ActionLib;
struct ActionInstance;
struct Control;
struct ControlCharacter;

//----------------Action Types-----------------
struct GenericAction
{
    GenericAction();
    virtual ~GenericAction() = 0;
	virtual void process(ControlCharacter& target) = 0;
    void finished(ControlCharacter& target);
};

struct ActionSpawn : GenericAction
{
    int ent;

    ActionSpawn(int uid);
    ~ActionSpawn();
	void process(ControlCharacter& target);
};

struct ActionWait : GenericAction
{
	int millis;

	ActionWait(int _millis);
	~ActionWait();
	void process(ControlCharacter& target);
};

struct ActionMove : GenericAction
{
	int ent;

	ActionMove(int uid);
	~ActionMove();
	void process(ControlCharacter& target);
};

//added by Protagoras
struct ActionJump : GenericAction
{
	int ent;

	ActionJump();
	~ActionJump();
	void process(ControlCharacter& target);
};

//added by Protagoras
struct ActionFace : GenericAction
{
	int cuid;

	ActionFace(int _cuid);
	~ActionFace();
	void process(ControlCharacter& target);
};

//added by Protagoras
struct ActionExpect : GenericAction
{
	int cuid, radius;

	ActionExpect(int _cuid, int _radius);
	~ActionExpect();
	void process(ControlCharacter& target);
};

//added by Protagoras
struct ActionFollow : GenericAction
{
	int cuid, ent;

	ActionFollow(int _cuid, int uid);
	~ActionFollow();
	void process(ControlCharacter& target);
};

struct ActionSpeed : GenericAction
{
	int speed;

	ActionSpeed(int _speed);
	~ActionSpeed();
	void process(ControlCharacter& target);
};

struct ActionDestroy : GenericAction
{
	ActionDestroy();
	~ActionDestroy();
	void process(ControlCharacter& target);
};

struct ActionModel : GenericAction
{
	int model;

	ActionModel(int model);
	~ActionModel();
	void process(ControlCharacter& target);
};

struct ActionSetAO : GenericAction
{
	int ao;
	int anim;

	ActionSetAO(int _ao, int _anim);
	~ActionSetAO();
	void process(ControlCharacter& target);
};

struct ActionSetGesture : GenericAction
{
	int anim;

	ActionSetGesture(int anim);
	~ActionSetGesture();
	void process(ControlCharacter& target);
};

struct ActionDoGesture : GenericAction
{
	int millis;

	ActionDoGesture(int millis);
	~ActionDoGesture();
	void process(ControlCharacter& target);
};

struct ActionAttach : GenericAction
{
	int part;
    int rule;
    string model;

	ActionAttach(int part, char* model, int rule);
	~ActionAttach();
	void process(ControlCharacter& target);
};

struct ActionDetach : GenericAction
{
	int part;

	ActionDetach(int part);
	~ActionDetach();
	void process(ControlCharacter& target);
};

struct ActionCommand : GenericAction
{
	const char* command;

	ActionCommand(const char* cmd);
	~ActionCommand();
	void process(ControlCharacter& target);
};

struct ActionCond : GenericAction
{
	const char* test;
	const char* body;

	ActionCond(const char* test, const char* body);
	~ActionCond();
	void process(ControlCharacter& target);
};

struct ActionCondWait : GenericAction
{
	const char* test;

	ActionCondWait(const char* test);
	~ActionCondWait();
	void process(ControlCharacter& target);
};
//-------------Action Managment----------------
struct ActionState
{
	string statedescr;
	vector <GenericAction*> steps;

	ActionState(const char*  descr);

	~ActionState();

	const char* getstatedescr() { return statedescr; }

	bool hasstep(int i);

	void append(GenericAction *a);

	void insert(int pos, GenericAction *a);

	void remove(int pos);

	GenericAction* getaction(int i);
};

struct ActionLib
{
    int usage;
	string librarydescr;
	vector<ActionState*> states;

	ActionLib(const char* descr);

	~ActionLib();

	bool hasstep(int state, int step);

	bool hasnextstep(int state, int step);

	bool hasstate(int state);

	//returns the Action of the requested step in requested state
	GenericAction* getaction(int state, int step);

	//returns the requested state
	ActionState* getstate(int state);

	//preserves a new state - returns number of states
	int addstate(const char *descr);

    //append a new step on given state
	void appendstep(int state, GenericAction* action);

    //insert step on current position
	void insertstep(int state, int step, GenericAction* action);

    //notify running instances about changed
    void notify(int ev);

	//notify running instances about destroy
	void notifydelete();
};


//action states (refers to action instances)
enum {
    ACTION_STATE_IDLE = 0,
    ACTION_STATE_INIT,
    ACTION_STATE_PROCESS,
    ACTION_STATE_END
};

//following events can be handled by actioninstance:
//general: ActionLib has changed, own operation like reached last step
enum {
    ACTIONINSTANCE_EVENT_SPAWN = 0,
    ACTIONINSTANCE_EVENT_INIT,
    ACTIONINSTANCE_EVENT_STEP_APPEND,
    ACTIONINSTANCE_EVENT_STEP_DELETE,
    ACTIONINSTANCE_EVENT_STEP_FINISH,
    ACTIONINSTANCE_EVENT_STATE_APPEND,
    ACTIONINSTANCE_EVENT_STATE_DELETE,
    ACTIONINSTANCE_EVENT_STATE_FINISH
};

//policies, how to hanlde certain events
enum
{
    ACTION_POLICY_ON_STATE_END_WAIT = 1<<0,
    ACTION_POLICY_ON_STATE_END_LOOP = 1<<1,
    ACTION_POLICY_ON_STATE_END_NEXT = 1<<2,
    ACTION_POLICY_ON_LAST_STATE_END_DESTROY = 1<<3,
    ACTION_POLICY_ON_LAST_END_STATE_IDLE = 1<<4,
    ACTION_POLICY_ON_LAST_STATE_END_LOOP = 1<<5
};

//TODO: set localuid in contructor
class ActionInstance
{
public:
	ActionLib& lib;
	int curstate;	    //index of current state
	int curstep;	    //index of current step
	int actionstate;    //states, process, idle, end ...
	int actionpolicy;   //sets behaviour on finishing a state: stop, nextstate, or loop
	int localuid;       //return value for scripts on adding character in default mode (see addcharacter)
	int lastprocess;    //last time when processed an action (Protagoras)
	int *plastaction;   //pointer to corresponding DynamicEntity's lastaction, (Protagoras)

	ActionInstance(ActionLib& _lib);

	~ActionInstance();

	int getcurstate();

	int getcurstep();

	void setcurstate(int state);

    void event(int ev);

    void finished();

	void next();

	void process(ControlCharacter& target);

	void nextstep(ControlCharacter& target);

	void laststep(ControlCharacter& target);
};

void clearactions();

//on the fly for 2.6 - can used to save actions
class CommandProxy
{
	public:
		CommandProxy();
		~CommandProxy();

		void addcmd(char **args, int *numargs);
		void save();
		void clear();

	private:
		vector<char*> commands;
};
#endif /* __ACTION_H__ */
