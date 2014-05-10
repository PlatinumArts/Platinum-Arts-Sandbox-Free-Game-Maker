#include "entityscript/action.h"

namespace EntityScript
{
	Action* current_action = NULL;

	Action::Action(const char *b) : parent(current_action), instance(current_action->getInstance()), proc_ptr(&Action::process_self), body(b)
	{
//		conoutf("Action::Action: parent: %p, instance");
	}

	Action::Action(Instance& inst, const char *b) : parent(current_action), instance(inst), proc_ptr(&Action::process_self), body(b)
	{
//		conoutf("Action::Action: parent: %p, instance");
	}

	Action::~Action() {}

	bool Action::process()
	{
		return (this->*proc_ptr)();
	}

	bool Action::process_self()
	{
		if( update() ) {
			if ( body && action_lock() ) {
				execute(body);
				proc_ptr = &Action::process_children;
				action_unlock();
			}
			else {
				proc_ptr = &Action::process_post;
			}
		}
		return false;
	}

	inline Instance& Action::getinstance()
	{
		return instance;
	}

	bool Action::process_children()
	{
		if ( pending.empty() )
		{
			proc_ptr = &Action::process_post;
			return false;
		}

		if ( pending[0]->process() )
		{
			delete pending.remove(0);
		}
		return false;
	}

	bool Action::process_post()
	{
		return true;
	}

	inline Instance& Action::getInstance()
	{
		return instance;
	}

	bool Action::update()
	{
		return true;
	}

	bool Action::action_lock()
	{
		if ( !current_action ) {
			current_action = this;
			return true;
		}
		return false;
	}

	bool Action::action_unlock()
	{
		if ( current_action == this ) {
			current_action = NULL;
			return true;
		}
		return false;
	}

	ActionCallback::ActionCallback(Instance& inst, Signal& sig, const char *b) : Action(inst, b), signal(sig) {}

	void ActionCallback::getparam(int i)
	{
		signal.getparam(i);
	}

	ICOMMAND(emmit, "sV", (char* sig, char** arg, int* num),
		{
			if (! current_action) {
				puts("no current action");
				return;
			}
			current_action->getinstance().emmitsignal(sig, arg, *num);
		}
	);

	ICOMMAND(param, "i", (int* i),
		{
			if (! current_action) {
				puts("no current action");
				return;
			}
			current_action->getinstance().getcallbackparam(*i);
		}
	);
}
