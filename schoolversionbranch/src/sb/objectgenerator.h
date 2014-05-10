#ifndef SB_OBJECTGENERATOR_H_INCLUDED
#define SB_OBJECTGENERATOR_H_INCLUDED

template<class Base, class Derived>
inline Base *NewObject()
{
   return new Derived();
}

template<class Base>
class ObjectGenerator
{
protected:
	typedef Base *(*ObjectPtr)();
	ObjectPtr constructor;

public:
	template<class Derived>
	void Init()
	{
		constructor = &NewObject<Base, Derived>;
	}

	Base* New()
	{
		return constructor();
	}
};

#endif // SB_OBJECTGENERATOR_H_INCLUDED
