#ifndef SB_ENTITYSCIPTINSTANCE_H
#define SB_ENTITYSCIPTINSTANCE_H

#include "cube.h"
#include "entityscript/action.h"
#include "entityscript/script.h"
#include "entityscript/signal.h"

///TODO:
//template instance
//Template Types based Instance Type and Callbacks
//create temporary new types based on definition

///TODO: move getcallbackparam into action

namespace EntityScript
{
	class ActionCallback;

	class Instance
	{
		Script& script;
		vector<ActionCallback*> callbacks;
		hashtable<const char*, Signal*> signals;

	protected:
		void registercallback(const char* name, const char* fmt = NULL);
		Signal* getsignal(const char*);
		const char* getcallback(const char*);

	public:
		Instance(Script& s);
		virtual ~Instance();

		void process();
		void debug();

		void emmitsignal(const char* sig, const char *fmt, ...);
		void emmitsignal(const char* sig);
		void emmitsignal(const char* sig, char**, int);

		const char* getsignalfmt(const char* sig);
		void getcallbackparam(int i);
	};
}

#endif // SB_ENTITYSCIPTINSTANCE_H
