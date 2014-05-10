#ifndef SB_OBJECT_FACTORY_H_INCLUDED
#define SB_OBJECT_FACTORY_H_INCLUDED

#include "cube.h"


//factory templates for registering types by an id (e.g. int or c-strings)
//this is useful to create objects given by strings from cube script or for
//writing handlers classes for entities which could be registered by using entity::type
//
////usage:
//	ObjectFactory<BaseClass, const char*> factory;
//
//	factory.Register<DerivedClass>("identifier"));
//
//	DerivedClass *dc = factory.Create("identifier");
//
//	factory.Unregister("identifier");

template <class TypeParam, class TypeID> class ObjectFactory;

template<class BaseType, class Type>
inline BaseType *CreateObject()
{
   return new Type();
}

template<class BaseType, class TypeParam, class Type>
inline BaseType *CreateObject(TypeParam param)
{
   return new Type(param);
}

template<class BaseType, class TypeID>
class ObjectFactory<BaseType (), TypeID>
{
protected:
	typedef BaseType *(*CreateObjectFunc)();
	hashtable<TypeID, CreateObjectFunc> creator;

public:
	template<class Type>
	bool Register(TypeID id)
	{
		if(creator.access(id)) {
			conoutf("ObjectFactory<BaseType, Type>::Register: error - key found");
			return false;
		}
		else {
			creator[id] = &CreateObject<BaseType(), Type>;
		}

		return true;
	}

	bool Unregister(TypeID id)
	{
		if(!creator.access(id)) {
			conoutf("ObjectFactory<BaseType, Type>::Register: error - key not found");
			return false;
		}
		return creator.remove(id);
	}

	BaseType* Create(TypeID id)
	{
		if(!*creator.access(id)) {
			conoutf("ObjectFactory<BaseType, Type>::Create: not found");
			return NULL;
		}
		else {
			return creator[id]();
		}
	}
};


template<class BaseType, class TypeParam, class TypeID>
class ObjectFactory<BaseType (TypeParam), TypeID>
{
protected:
	typedef BaseType *(*CreateObjectFunc)(TypeParam);
	hashtable<TypeID, CreateObjectFunc> creator;

public:
	template<class Type>
	bool Register(TypeID id)
	{
		if(creator.access(id)) {
			conoutf("ObjectFactory<BaseType(TypeParam), Type>::Register: error - key found");
			return false;
		}
		else {
			creator[id] = &CreateObject< BaseType, TypeParam, Type >;
		}
		return true;
	}

	bool Unregister(TypeID id)
	{
		if(!creator.access(id)) {
			conoutf("ObjectFactory<BaseType(TypeParam), Type>::Register: error - key not found");
			return false;
		}

		return creator.remove(id);
	}

	BaseType* Create(TypeID id, TypeParam param)
	{
		if(!creator.access(id)) {
			conoutf("ObjectFactory<BaseType(TypeParam), Type>::Create: not found");
			return NULL;
		}
		else {
			return creator[id](param);
		}
	}
};

template<class BaseType>
class ObjectFactory<BaseType(), const char*>
{
protected:
	typedef BaseType *(*CreateObjectFunc)();
	hashtable<const char*, CreateObjectFunc> creator;

public:
	template<class Type>
	bool Register(const char* id)
	{
		const char* key = newstring(id);
		if(creator.access(key)) {
			conoutf("ObjectFactory<Type, const char*>::Register: error - key found");
			return false;
		}
		else {
			creator[id] = &CreateObject<BaseType, Type>;
		}

		return true;
	}

	bool Unregister(const char* id)
	{
		const char* key = newstring(id);
		if(!creator.access(key)) {
			conoutf("ObjectFactory<Type, const char*>::Unregister error - key not found");
			return false;
		}

		return creator.remove(key);
	}

	BaseType* Create(const char* id)
	{
		const char* key = newstring(id);
		if(!creator.access(key)) {
			conoutf("ObjectFactory<Type, const char*>::Create: not found");
			return NULL;
		}
		else {
			return creator[key]();
		}
	}
};

template<class BaseType>
class ObjectFactory<BaseType (), int>
{
protected:
	typedef BaseType *(*CreateObjectFunc)();
	vector<CreateObjectFunc> creator;

public:
	template<class Type>
	bool Register(int id)
	{
		if( creator.inrange(id) ) {
			if ( creator[id] ) {
				conoutf("ObjectFactory<BaseType, int>::Register: error - key found");
				return false;
			}
			else {
				creator[id] = &CreateObject<BaseType, Type>;
				return true;
			}
		}
		else {
			while( creator.length() <= id ) creator.add(NULL);
			creator[id] = &CreateObject<BaseType, Type>;
		}
		return true;
	}

	bool Unregister(int id)
	{
		if(!creator[id]) {
			conoutf("ObjectFactory<BaseType, int>::Register: error - key not found");
			return false;
		}
		creator[id] = NULL;
		return true;
	}

	BaseType* Create(int id)
	{
		conoutf("ObjectFactory<BaseType, int>::Create(int id)");
		if(!creator.inrange(id) || !creator[id]) {
			conoutf("ObjectFactory<BaseType, int>::Create: not found");
			return NULL;
		}
		else {
			conoutf("ObjectFactory<BaseType, int>::Create: found");
			return creator[id]();
		}
	}
};

#endif // SB_OBJECT_FACTORY_H_INCLUDED
