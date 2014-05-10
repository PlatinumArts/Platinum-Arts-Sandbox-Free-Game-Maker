#include "qcommand.h"

hashtable<const char*, QueuedCommand*>  QueuedCommandMgr::queues;
Instance*                               QueuedCommandMgr::current = NULL;

QueuedState::QueuedState(const char* _body) : body(newstring(_body)) {}

QueuedState::~QueuedState()
{
    delete[] body;
}

QueuedCommand::QueuedCommand(const char *_id, const char *_body) : id(newstring(_id))
{
    states.add(new QueuedState(_body));
}

void QueuedCommand::addstate(const char* _body)
{
    states.add(new QueuedState(_body));
}

const char* QueuedCommand::getbody(int i)
{
    return states.inrange(i) ? states[i]->body : NULL;
}

QueuedCommand::~QueuedCommand()
{
    delete[] id;
}

Child::Child() {}
Child::~Child()
{
    pending.deletecontents();
}

void Child::pushback(Action* a)
{
    if(a)
    {
        pending.add(a);
    }
}

void Child::popfront()
{
    if(!pending.empty())
        delete pending.remove(0);
}

Action* Child::front()
{
    if(!pending.empty())
        return pending[0];
    else
        return NULL;
}

bool Child::update()
{
    if(pending.empty())
    {
        return true;
    }
    if(front()->update())
    {
        popfront();
    }
    return false;
}

ChildAtOnce::ChildAtOnce() {}

ChildAtOnce::~ChildAtOnce() {}

bool ChildAtOnce::update()
{
    if(pending.empty())
    {
        return true;
    }

    loopv(pending)
    {
        if(pending[i]->update())
        {
            delete pending.remove(i);
        }
    }
    return false;
}

Instance::Instance(QueuedCommand& _queued) : queued(_queued), qcstate(0), state(INSTANCE_STATE_NONE)
{
    registersignal("statechanged");
    registersignal("finished");
}

void Instance::init()
{
    pushchild(new Child(), queued.getbody(qcstate) );
    state = INSTANCE_STATE_RUNNING;
}

void Instance::pushchild(Child* child, const char* body)
{
    if(child)
    {
        QueuedCommandMgr::current = this;
        stack.add(child);
        execute(body);
        QueuedCommandMgr::current = NULL;
    }
}

void Instance::process()
{
    if ( stack.empty() )
    {
        return;
    }

    changestate();

    if( stack.last()->update() )
    {
        delete stack.pop();

        if( stack.empty () && state == INSTANCE_STATE_RUNNING)
            finished();
    }
}

void Instance::reqchangestate(int i)
{
    if(queued.getbody(i))
    {
        qcstate = i;
    }
}

void Instance::pushaction(Action* action)
{
    stack.last()->pushback(action);
}

void Instance::sighandler(const char* sig, const char* body)
{
    if(!body || !sig) return;
    char* key = newstring(sig);
    Handler** hh = signals.access(key);
    if(hh)
    {
        signals[key]->sethandler(body);
    }
}

void Instance::registersignal(const char* id)
{
    char* key = newstring(id);
    Handler** hh = signals.access(key);
    if(!hh)
    {
    	conoutf("Instance::registersignal %p %s", this, id);
        signals.access(newstring(id), new Handler);
    }
    else
    {
        conoutf("Instance::registersignal: signal %s already registered", key);
    }
}

void Instance::changestate()
{
    static int curstate = 0;

    if ( qcstate != curstate )
    {
        emptystack();
        pushchild(new Child, queued.getbody(qcstate));
        curstate = qcstate;
    }
}

void Instance::finished()
{
    state = INSTANCE_STATE_WAITING;
    emmitsignal("finished");
}

void Instance::emmitsignal(const char* sig)
{
    char* key = newstring(sig);
    if(signals[key]->gethandler())
    {
		//conoutf("Instance::emmitsignal: %s", signals[key]->gethandlerdata() );
		const char* body = signals[key]->gethandler();
		defformatstring(s)("format [ %s ] %s", body, signals[key]->gethandlerdata());
    	const char* tmp = executeret(s);
        pushchild(new Child, tmp );
//        pushchild(new Child, signals[key]->gethandler() );
    }
    else
    {
        conoutf("Instance::emmitsignal: signal %s not registered", sig);
    }
}

void Instance::emptystack()
{
    stack.deletecontents();
}

Instance::~Instance()
{
    emptystack();
}

void QueuedCommandMgr::newqueue(const char* id, const char* a)
{
    char* key = newstring(id);
    if(!queues.access(key))
    {
        queues.access(key, new QueuedCommand(key,a));
    }
    else
    {
        conoutf("[DEBUG] QueuedCommand already exists - overwriting not implemented");
    }
}

QueuedCommand* QueuedCommandMgr::findqueued(const char* s)
{
    char* key = newstring(s);
    if(QueuedCommand** cc = queues.access(key))
    {
        return *cc;
    }
    return NULL;
}

Instance* QueuedCommandMgr::getcurrent()
{
    return current ? current : NULL;
}

Handler::Handler() : body(NULL) {}
Handler::~Handler()
{
    delete[] body;
}
void Handler::sethandler(const char* s)
{
    body = newstring(s);
}

const char* Handler::gethandler()
{
    return body;
}

const char* Handler::gethandlerdata()
{
	return "Handler::gethandlerdata";
}

Action::Action(const char *s = NULL) : body(s && *s ? newstring(s): NULL), instance(*QueuedCommandMgr::getcurrent()), isinit(false)
{
    instance.pushaction(this);
}

Action::~Action()
{
    if(body)
    {
        delete[] body;
    }
}

bool Action::checktype()
{
    return true;
}

void Action::init() { isinit = true; }

bool Action::update()
{
    if(!checktype()) return true;

    if(!isinit) init();

    if(process())
    {
        if (body)
        {
            instance.pushchild(new Child, body);
        }
        return true;
    }
    return false;
}


ICOMMAND(cq_new, "ss", (const char *id, char* body),
         QueuedCommandMgr::newqueue(id, body)
);

ICOMMAND(cq_state, "ss", (const char *id, char* body),
{
    QueuedCommand* c = QueuedCommandMgr::findqueued(id);
    if(c)
    {
        c->addstate(body);
    }
}
);

ICOMMAND(cq_self, "", (),
	{
		Instance* instance = QueuedCommandMgr::getcurrent();
		defformatstring(s)("%p", instance);
		result(s);
	}
);

ICOMMAND(cq_data, "", (),
	{
		Instance* instance = QueuedCommandMgr::getcurrent();
		defformatstring(s)("%p", instance);
		result(s);
	}
);


