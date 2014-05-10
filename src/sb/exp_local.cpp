#include "cube.h"

typedef hashtable<const char *, ident> identtable;
extern identtable *idents;

class localident;

vector<identtable *> identtablestack;
vector<localident *> locals;

void pushidentstack(identtable *table)
{
	if(identtablestack.empty())
		identtablestack.add(idents);
	identtablestack.add(table);
	idents = identtablestack.last();
}

void popidentstack()
{
	identtablestack.pop();
	idents = identtablestack.last();
}

class localident
{
	public:
		localident(const char* _name) : name(newstring(_name))
		{
			local = new identtable;
			pushidentstack(local);
			CCOMMAND(getname, "", (localident *self), result(self->getname()));
			CCOMMAND(setname, "s", (localident *self, char* s), self->setname(s));
			popidentstack();
		}

		identtable *local;
		const char* getname() { return name; }

	private:
		const char* name;
		float fval;
		float ival;

		void setname(const char* _name) { delete[] name; name = newstring(_name); }
};

ICOMMAND(addlocal, "s", (char* name),
	{
		locals.add(new localident(name));
		intret(locals.length() - 1);
	}
);

ICOMMAND(local, "is", (int *i, char *s),
	{
		if ( ! locals.inrange(*i) )
			return;
		pushidentstack(locals[*i]->local);
		const char* ret = executeret(s);
		popidentstack();
		conoutf("TEST %s", ret);
	}
);
