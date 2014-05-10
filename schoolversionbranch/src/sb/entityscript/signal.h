#ifndef SB_ENTITYSCRIPT_SIGNAL_H
#define SB_ENTITYSCRIPT_SIGNAL_H

#include "cube.h"

//should be also an action
namespace EntityScript {

	class Signal
	{
		const char* fmt;
		vector<ident*> params;

		public:
			Signal(const char* _fmt = NULL);
			~Signal();

			const char* getfmt();
			void getparam(int i);
			void parseparams(char*);
			void parseparams(char**, int num);
	};
}
#endif // SB_ENTITYSCRIPT_SIGNAL_H
