#include "entityscript/instance.h"

namespace EntityScript
{
	Instance::Instance(Script& s) : script(s)
	{
		//will not work as intened in derived types
		registercallback("default", "i");
		registercallback("waiting");

		//additional signals - if already registered, ignore
		hashtable<const char*, const char*> &signals = script.getsignals();
		enumeratekt(signals, const char *, _signal, const char *, _fmt, registercallback(_signal, _fmt));
		int val = 10;
		emmitsignal("default", "i", val);
	}

	Instance::~Instance()
	{
		//dtor
	}

	void Instance::process()
	{
		if ( callbacks.last()->process() )
		{
			//test exist callback waiting ? emmit : remove from execution stack
			callbacks.drop();
			if ( callbacks.empty() )
				emmitsignal("waiting");
		}
	}

	//query key exist: signals.access(name)
	//query value: *signals.access(name)
	void Instance::registercallback(const char* name, const char* fmt)
	{
		//test if callback is given in script
		if ( script.getstate( name )) {
			if( ! signals.access( name )) {
				signals.access( name,  new Signal(fmt));
			}
			else {
				conoutf("Instance::registercallback: callback %s already registered", name);
			}
		}
	}

	void Instance::emmitsignal(const char* sig, const char *fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		Signal* signal = getsignal(sig);
		const char* body = script.getstate(sig);
		if ( ! signal || ! body )
			return;
		signal->parseparams(args);
		va_end(args);
		callbacks.add( new ActionCallback(*this, *signal, body) );
	}

	inline void Instance::emmitsignal(const char* sig)
	{
		emmitsignal(sig, NULL);
	}

	void Instance::emmitsignal(const char* sig, char** args, int num)
	{
		Signal* signal = getsignal(sig);
		const char* body = script.getstate(sig);
		if ( ! signal || ! body )
			return;
		signal->parseparams(args, num);
		callbacks.add( new ActionCallback(*this, *signal, body) );
	}

	const char* Instance::getsignalfmt(const char* signal)
	{
		return (*signals.access(signal))->getfmt();
	}

	Signal* Instance::getsignal(const char* signal)
	{
		if (signals.access(signal))
			return signals[signal];
		else
			return NULL;
	}

	void Instance::getcallbackparam(int i)
	{
		callbacks.last()->getparam(i);
	}

	void Instance::debug()
	{
		conoutf("Instance::debug");
	}
}
