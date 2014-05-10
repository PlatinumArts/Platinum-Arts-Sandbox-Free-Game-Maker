#include "entityscript/scriptmanager.h"

namespace EntityScript
{
	ScriptManager::ScriptManager() {}
	ScriptManager::~ScriptManager() {}

	Instance* ScriptManager::Create(const char* type, const char* script_name)
	{
		Script* script = findscript(script_name);
		if (! script ) {
			return NULL;
		}
		return ObjectFactory::Create(type, *script);
	}

	void ScriptManager::newscript(const char* type, const char* name, const char* body)
	{
		char* key = newstring(name);
		if(!scripts.access(key))
		{
			scripts.access(key, new Script(type, body));
		}
		else
		{
			conoutf("[DEBUG] ScriptManager::newscript: script already exists - overwriting not implemented");
		}
	}

	Script* ScriptManager::findscript(const char* name)
	{
		char* key = newstring(name);
		if(Script** script = scripts.access(key))
		{
			return *script;
		}
		return NULL;
	}
}
