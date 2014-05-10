#ifndef SB_Action_H_INCLUDED
#define SB_Action_H_INCLUDED

#include "cube.h"
#include "entityscript/instance.h"
#include "entityscript/signal.h"

namespace EntityScript
{
	class Instance;

	//Action Interface
	class Action
	{
		Action*		parent;
		Instance&	instance;
		bool		(Action::*proc_ptr)();

		Instance&		getInstance();
		virtual bool 	process_self();
		bool 			action_lock();
		bool 			action_unlock();

	public:
		Action(const char *body);
		Action(Instance& instance, const char *body);

		virtual ~Action();

		bool process();
		Instance& getinstance();

	protected:
		const char* 	body;
		vector<Action*> pending;

		virtual bool process_children();
		virtual bool process_post();
		virtual bool update();
	};

	//Action stays on top of an Action Chain, called as Callback Action
	class ActionCallback : public Action
	{
		Signal& signal;

		public:
			ActionCallback(Instance& instance, Signal& signal, const char *body);
			void getparam(int i);
	};
}
#endif // SB_Action_H_INCLUDED
