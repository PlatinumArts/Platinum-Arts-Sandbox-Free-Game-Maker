#include "signal.h"

namespace EntityScript {

	Signal::Signal(const char* _fmt) :  fmt(newstring(_fmt && *_fmt ? _fmt : ""))
	{
		while(_fmt && *_fmt) switch(*_fmt++)
		{
			case 'i':
			{
				ident* id_var = new ident(ID_VAR, "param", 0, 0, 0, new int);
				params.add(id_var);
				break;
			}
			case 'f':
			{
				ident* id_float = new ident(ID_FVAR, "param", 0, 0, 0, new float);
				params.add(id_float);
				break;
			}
			case 's':
				ident* id_string = new ident(ID_SVAR, "param", newstring(""), new char*);
				*id_string->storage.s = NULL;
				params.add(id_string);
				break;
		}
	}

	const char* Signal::getfmt()
	{
		return fmt;
	}

	void Signal::getparam(int i)
	{
		if ( params.inrange (i) ) {
			switch(params[i]->type)
			{
				case ID_VAR:
					intret(*params[i]->storage.i);
					break;
				case ID_FVAR:
					floatret(*params[i]->storage.f);
					break;
				case ID_SVAR:
					result(*params[i]->storage.s);
			}
		}
		else {
			result("");
		}
	}

	void Signal::parseparams(char* args)
	{
		const char* f = fmt;
		if(f)
        {
            int i = 0;
            while(*f) switch(*f++)
            {
                case 'i':
                {
                	int val = va_arg(args, int);
                	conoutf("Signal::parseparam: found %d", val);
					*params[i]->storage.i = val;
					break;
                }
                case 's':
                {
					char* val = va_arg(args, char*);
					conoutf("Signal::parseparam: found %s", val);
					if(*params[i]->storage.s)
						delete[] *params[i]->storage.s;
					*params[i]->storage.s = newstring(val);
                	break;
                }
                case 'f':
                {
                	float val = (float)va_arg(args, double);
                	conoutf("Signal::parseparam: found %f", val);
                	*params[i]->storage.f = val;
                	break;
                }
            }
        }
	}

	void Signal::parseparams(char** args, int num)
	{
		const char *f = fmt;
		loopi(num) if(*f)
		{
			switch(*f)
			{
                case 'i':
                {
                	conoutf("Signal::parseparam: found arg[%d] int %s", i+1, args[i+1]);
                	ident *id = params[i];
                	*id->storage.i = atoi(args[i+1]);
                	break;
                }
                case 's':
                {
					conoutf("Signal::parseparam: found char* %s", args[i+1]);
					if(*params[i]->storage.s)
						delete[] *params[i]->storage.s;
					*params[i]->storage.s = newstring(args[i+1]);
                	break;
                }
                case 'f':
                {
                	conoutf("Signal::parseparam: found float: %s", args[i+1]);
                	*params[i]->storage.f = atof(args[i+1]);
                	break;
                }
			}
			f++;
		}
	}

	Signal::~Signal()
	{
		delete[] fmt;
		params.deletecontents();
	}
}
