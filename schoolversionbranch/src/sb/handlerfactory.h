#ifndef SB_HANDLERFACTORY_H_INCLUDED
#define SB_HANDLERFACTORY_H_INCLUDED
#include "cube.h"

//factory templates for handler classes
//usefull for writing handler classes that implements entities or network messages
//
//
//HandlerFactory<EntHandler, int> factory;
//factory.Register<EntHandlerLight>(LIGHT);
//factory.Register<EntHandlerMapmodel>(MAPMODEL);
//       ...
//factory[e->type]->fixentity();

template<class BaseType, class TypeID>
class HandlerFactory
{
protected:
	hashtable<TypeID, BaseType*> handler;

public:
	template<class Type>
	bool Register(TypeID id)
	{
		if(handler.access(id)) {
			return false;
		}
		else {
			handler.access(id, new Type);
		}
		return true;
	}

	bool Unregister(TypeID id)
	{
		if(!handler.access(id)) {
			return false;
		}
		return handler.remove(id);
	}

	BaseType* operator[](TypeID id)
	{
		return handler.access(id) ? handler[id] : NULL;
	}

	const BaseType* operator[](TypeID id) const
	{
		return handler.access(id) ? handler[id] : NULL;
	}
};

template<class BaseType>
class HandlerFactory<BaseType, int>
{
protected:
	vector<BaseType*> handler;

public:
	template<class Type>
	bool Register(int id)
	{
		if( handler.inrange(id) ) {
			if ( handler[id] ) {
				return false;
			}
			else {
				handler[id] = new Type();
				return true;
			}
		}
		else {
			while( handler.length() < id ) {
				handler.add();
			}
			handler.add(new Type);
		}
		return true;
	}

	bool Unregister(int id)
	{
		if(!handler[id]) {
			return false;
		}
		delete handler[id];
		handler[id] = NULL;
		return true;
	}


	BaseType* operator[](int id)
	{
		return handler.inrange(id) ? handler[id] : NULL;
	}

	const BaseType* operator[](int id) const
	{
		return handler.inrange(id) ? handler[id] : NULL;
	}
};

template<class BaseType>
class HandlerFactory<BaseType, const char*>
{
protected:
	hashtable<const char*, BaseType*> handler;

public:
	template<class Type>
	bool Register(const char* id)
	{
		const char* key = newstring(id);
		if(handler.access(key)) {
			return false;
		}
		else {
			handler.access(key, new Type);
		}
		return true;
	}

	bool Unregister(const char* id)
	{
		const char* key = newstring(id);
		if(!handler.access(key)) {
			return false;
		}
		return handler.remove(key);
	}

	BaseType* operator[](const char* id)
	{
		const char* key = newstring(id);
		return handler.access(key) ? handler[key] : NULL;
	}

	const BaseType* operator[](const char* id) const
	{
		const char* key = newstring(id);
		return handler.access(key) ? handler[key] : NULL;
	}
};

#endif // SB_HANDLERFACTORY_H_INCLUDED
