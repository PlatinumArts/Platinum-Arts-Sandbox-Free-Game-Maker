#ifndef SB_ENTITYSCRIPTMANAGER_H
#define SB_ENTITYSCRIPTMANAGER_H

#include "cube.h"
#include "objectfactory.h"
#include "entityscript/script.h"
#include "entityscript/instance.h"

namespace EntityScript
{
	class ScriptManager : public ObjectFactory< Instance (Script&), const char* >
	{
		hashtable<const char*, Script*> scripts;

	public:
		ScriptManager();
		~ScriptManager();

		Instance* Create(const char* type, const char* script);

		void newscript(const char* type, const char* name, const char* body);
		Script* findscript(const char* s);
	};
};

#endif // SB_ENTITYSCRIPTMANAGER_H
