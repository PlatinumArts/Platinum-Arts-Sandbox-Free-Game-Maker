#ifndef QCSCRIPT_H_INCLUDED
#define QCSCRIPT_H_INCLUDED

#include "cube.h"
#include "qcommand.h"

//ActionDbg: use for debug reasons, will echo a string and the pointer address of the instance
class ActionDbg : public Action
{
public:
    ActionDbg(const char *e);
    ~ActionDbg();

    bool checktype();
    bool process();
private:
    char* str;
};

//ActionPack: empty Action for wrapping general scripting command (e.g echo)
class ActionPack : public Action
{
public:
    ActionPack(const char* c);
    ~ActionPack();

    bool process();
};

//ActionDelay: delays the execution of the script in the body n seconds
class ActionDelay : public Action
{
public:
    ActionDelay(float sec, const char *c);
    ~ActionDelay();

    void init();
    bool process();

private:
    int millis;
    int start;
};

//ActionWatch: waits until an alias has a specified value
class ActionWatch : public Action
{
public:
    ActionWatch(const char* _alias, int _val, const char* c);
    ~ActionWatch();

    bool process();

private:
    int val;
    char* alias;
};

//ActionSigHandler: adds an signal handler to an signal, that is emmited by an Instance
class ActionSigHandler : public Action
{
public:
    ActionSigHandler(const char* signal, const char* c);
    ~ActionSigHandler();

    bool update();
    bool process();

private:
    char* signal;
};

class ActionChangeState : public Action
{
public:
    ActionChangeState(int i);
    ~ActionChangeState();

    bool process();

private:
    int state;
};

//ActionTest: test equal against an alias, if true body will be executed
class ActionTest : public Action
{
public:
    ActionTest(const char* _alias, int _val, const char* c);
    ~ActionTest();

    bool update();
    bool process();

private:
    int val;
    char* alias;
};

class ActionAtOnce : public Action
{
public:
    ActionAtOnce(const char* body);
    ~ActionAtOnce();

    bool update();
    bool process();
};
#endif // QCSCRIPT_H_INCLUDED
