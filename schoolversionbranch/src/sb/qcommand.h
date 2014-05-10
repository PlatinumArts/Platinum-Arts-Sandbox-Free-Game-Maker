#ifndef QCOMMAND_H_INCLUDED
#define QCOMMAND_H_INCLUDED

#include "cube.h"

///TODO:
///*remove Action from pending if checktype fails in constructor of Action

class Instance;

//Interface to action classes
class Action
{
public:
    //constructor, one or more actions are contructed by an child object at the same time
    //and executed later, if you need a timer initialze it with init
    Action(const char *s);
    virtual ~Action();

    //check if the action matches the instance type
    virtual bool checktype();
    //init stuff here, called with first update
    virtual void init();
    //wraps process and calls a new child, when finished
    //return true if done
    virtual bool update();

protected:
    //body of an child action, executed inside update, if process
    //returns true
    const char* body;

    //reference to the calling instance
    Instance& instance;

    //all the main stuff happpens here, return true if finished
    virtual bool process() = 0;

	///TODO: need to call init, move call into Child::update
	bool isinit;
};

//states can be changed by receiving signals or by scripts
//instances can run
class QueuedState
{
    friend class QueuedCommand;

public:
    //_body: script of the QueuedCommand
    QueuedState(const char* _body);
    ~QueuedState();

private:
    const char* body;
};

class QueuedCommand
{
public:
    //id: id provided by hashtable in QueuedCommandMgr
    //body: body of the script (forwared to state 0)
    QueuedCommand(const char *_id, const char *_body);
    ~QueuedCommand();

    //adds a new state (script)
    void addstate(const char* body);

    //returns the script of the current state
    const char* getbody(int i);

private:
    //QueuedCommand id is untestet, managed by hashtable in QueuedCommandMgr
    const char* id;
    //States holding the script
    vector<QueuedState*> states;
};

class Child
{
friend class Instance;

public:
    Child();
    ~Child();

protected:
    vector<Action*>		pending;

    void				pushback(Action* a);
    void				popfront();
    Action*				front();
    virtual bool 		update();
};

class ChildAtOnce : public Child
{
public:
    ChildAtOnce();
    ~ChildAtOnce();

    bool update();
};

class Handler
{
public:
    Handler();
    ~Handler();

    void 		sethandler(const char* s);
    const char* gethandler();
	const char* gethandlerdata();

private:
    char* body;
};

enum { INSTANCE_STATE_NONE = 0, INSTANCE_STATE_WAITING, INSTANCE_STATE_RUNNING };

//base class of all kind of instances, used for character controls, entity controls ...
class Instance
{
public:
    QueuedCommand& queued;

    Instance(QueuedCommand& _queued);
    virtual ~Instance();

    void init();
    void process();
    void pushaction(Action* action);
    void pushchild(Child* child, const char* body);
    void reqchangestate(int i);

    //emmits a signal
    void emmitsignal(const char* name);
    //register a callback for the signal
    void sighandler(const char* sig, const char* action);

protected:
    hashtable<const char*, Handler*> signals;
    vector<Child*> stack;
    vector<Child*> handler;
    int qcstate;
    int state;

    //registers a new signal name (use in constructor of derived instance)
    void registersignal(const char* id);
    //checks if the state was changed, calls the new state
    void changestate();
    //queue is empty - finished
    void finished();
    //cleanup and delete all stack elements
    void emptystack();
};

class QueuedCommandMgr
{
friend void Instance::pushchild(Child* child, const char* body);

public:
    static void				newqueue(const char* s, const char* a);
    static QueuedCommand*	findqueued(const char* s);

    static Instance*		getcurrent();

private:
    QueuedCommandMgr() {};
    ~QueuedCommandMgr();

    static hashtable<const char*, QueuedCommand*>	queues;
    static Instance*								current;
};

#endif // QCOMMAND_H_INCLUDED
