#ifndef SB_ENTITYSCRIPT_H_INCLUDED
#define SB_ENTITYSCRIPT_H_INCLUDED

#include "cube.h"

namespace EntityScript {

	class Script
	{
		const char* type;

		bool script_lock();
		bool script_unlock();

		hashtable<const char*, const char*> states;
		hashtable<const char*, const char*> signals;

	public:
		Script(const char* type, const char* body);
		~Script();

		void addstate(const char* signal, const char* body);
		void addsignal(const char* signal, const char* fmt);
		hashtable<const char*, const char*> &getsignals();
		const char* getstate(const char* state);
	};
}

#endif // SB_ENTITYSCRIPT_H_INCLUDED
