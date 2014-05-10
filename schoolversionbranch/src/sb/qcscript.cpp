#include "qcscript.h"

ActionDbg::ActionDbg(const char *e) : Action(NULL), str(newstring(e)) {}

ActionDbg::~ActionDbg()
{
    delete[] str;
}

bool ActionDbg::checktype()
{
    return true;
}

bool ActionDbg::process()
{
    conoutf("From Instance(%p): %s", &instance, str);
    return true;
}

ICOMMAND(cq_dbg, "s", (const char* e),
    new ActionDbg(e);
);

ActionPack::ActionPack(const char* c) : Action(c) {}

ActionPack::~ActionPack() {}

bool ActionPack::process()
{
    return true;
}

ICOMMAND(cq_pack, "s", (const char* s),
    new ActionPack(s);
);

ActionDelay::ActionDelay(float sec, const char *c) : Action(c), millis(int(sec)*1000), start(0) {}

ActionDelay::~ActionDelay() {}

void ActionDelay::init()
{
    start = lastmillis;
    isinit = true;
}

bool ActionDelay::process()
{
    if(lastmillis-start < millis)
    {
        return false;
    }
    return true;
}

ICOMMAND(cq_delay, "fs", (float *sec, const char* s),
    new ActionDelay(*sec, s);
);

ActionWatch::ActionWatch(const char* _alias, int _val, const char* c ) : Action(c), val(_val), alias(newstring(_alias))
{
    if(!identexists(alias))
    {
        delete[] alias;
        alias = NULL;
        conoutf("[DEBUG] ActionWatch::ActionWatch - alias %s not found", _alias);
    }
}

ActionWatch::~ActionWatch() { delete[] alias; }

bool ActionWatch::process()
{
    if(!alias) return true;
    ident *i = getident(alias);
    return ( i->type == ID_ALIAS && val == atoi(getalias(alias)) ) ? true : false;
}


ICOMMAND(cq_watch, "sis", (const char* s, int* i, const char* c),
    new ActionWatch(s, *i, c);
);

ActionSigHandler::ActionSigHandler(const char* _signal, const char* c ) : Action(c), signal(newstring(_signal)){}

ActionSigHandler::~ActionSigHandler() { delete[] signal; }

bool ActionSigHandler::update()
{
    if(!checktype()) return true;

    if(!isinit) init();

    if (process() && body)
    {
        instance.sighandler(signal, body);
    }
    return true;
}

bool ActionSigHandler::process()
{
    return true;
}

ICOMMAND(cq_sighandler, "ss", (const char* s, const char* c),
    new ActionSigHandler(s, c);
);

ActionChangeState::ActionChangeState(int i) : Action(NULL), state(i) {}

ActionChangeState::~ActionChangeState() {}

bool ActionChangeState::process()
{
    instance.reqchangestate(state);
    return true;
}

ICOMMAND(cq_changestate, "i", (int *i),
    new ActionChangeState(*i);
);

ActionTest::ActionTest(const char* _alias, int _val, const char* c ) : Action(c), val(_val), alias(newstring(_alias))
{
    if(!identexists(alias))
    {
        delete[] alias;
        alias = NULL;
        conoutf("[DEBUG] ActionTest::ActionTest - alias %s not found", _alias);
    }
}

ActionTest::~ActionTest() { delete[] alias; }

bool ActionTest::update()
{
    if(!checktype()) return true;

    if(!isinit) init();

    if(process() && body)
    {
    	instance.pushchild(new Child, body);
    }
    return true;
}

bool ActionTest::process()
{
    if(!alias) return true;
    ident *i = getident(alias);
    return ( i->type == ID_ALIAS && val == atoi(getalias(alias)) ) ? true : false;
}

ICOMMAND(cq_test, "sis", (const char* s, int* i, const char* c),
    new ActionTest(s, *i, c);
);

ActionAtOnce::ActionAtOnce(const char* body) : Action(body) {}
ActionAtOnce::~ActionAtOnce() {}

bool ActionAtOnce::update()
{
    if(!checktype()) return true;

    if(!isinit) init();

    if (body)
    {
    	instance.pushchild(new ChildAtOnce, body);
    }
    return true;
}

bool ActionAtOnce::process() { return true; }

ICOMMAND(cq_atonce, "s", (const char* body),
    new ActionAtOnce(body);
);
