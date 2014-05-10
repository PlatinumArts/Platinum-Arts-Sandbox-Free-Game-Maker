#include "entityscript/script.h"

namespace EntityScript {

	Script* current_script;

	Script::Script(const char* _type, const char* body) : type(newstring(_type))
	{
		if ( script_lock() ) {
			execute(body);
			script_unlock();
		}
	}

	Script::~Script()
	{
		delete[] type;
	}

	void Script::addstate(const char* signal, const char* body)
	{
		if(!states.access(signal))
		{
			states.access( newstring(signal), newstring(body));
		}
		else
		{
			conoutf("[DEBUG] Script::addstate: state already exists");
		}
	}

	void Script::addsignal(const char* signal, const char* fmt)
	{
		if(!signals.access(signal))
		{
			conoutf("Script::addsignal - adding custom signal: %s, %s", signal, fmt);
			signals.access( newstring(signal), newstring(fmt));
		}
		else
		{
			conoutf("[DEBUG] Script::addsignal: signal already exists");
		}
	}

	hashtable<const char*, const char*> &Script::getsignals()
	{
		return signals;
	}

	const char* Script::getstate(const char* state)
	{
		return states.access(state) ? states[state] : NULL;
	}

	bool Script::script_lock()
	{
		if ( !current_script ) {
			current_script = this;
			return true;
		}
		return false;
	}

	bool Script::script_unlock()
	{
		if ( current_script == this ) {
			current_script = NULL;
			return true;
		}
		return false;
	}

	ICOMMAND(connect, "ss", (char* sig, char* body),
		{
			if(!current_script) {
				return;
			}
			current_script->addstate(sig, body);
		}
	);

	ICOMMAND(callback, "ss", (char* sig, char* fmt),
		{
			if(!current_script) {
				return;
			}
			current_script->addsignal(sig, fmt);
		}
	);
}
