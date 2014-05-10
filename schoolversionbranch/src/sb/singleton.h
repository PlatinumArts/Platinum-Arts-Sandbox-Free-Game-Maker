#ifndef SB_SINGELTON_H_INCLUDED
#define SB_SINGELTON_H_INCLUDED

template <class T>
class Singleton
{
public:
	static T& Get ()
	{
		static T inst;
		return inst;
	}

private:
	Singleton( const Singleton& ) {}

protected:
	Singleton () { }
};

#endif // SB_SINGELTON_H_INCLUDED
