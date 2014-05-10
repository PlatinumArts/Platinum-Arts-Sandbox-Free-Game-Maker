#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "cube.h"
#include "singleton.h"

//Singleton Template class, used for MapToc and Manager Classes (e.g. CameraMgr, QueuedCommandMgr)
//template <typename C>
//class Singleton
//{
//public:
//	static C& instance ()
//	{
//		static C inst;
//		return inst;
//	}
//
//private:
//	Singleton( const Singleton& ) {}
//
//protected:
//	Singleton () { }
//};


class MapTocEntry;
class MapObject;

class MapToc : public Singleton<MapToc>
{
	//vector is just a placeholder until a better solution is found (hash)
	vector<MapTocEntry*>	toc;
	int						lastid;

public:
	MapToc();
	~MapToc();

	MapTocEntry&	registerentry( MapObject& mapobject );
	void			removeentry( int id );
	MapTocEntry*	lookupentry( int id );
	void			writetoc(stream *f);
	void			readtoc(int idx, int id);
	void			lock(bool lk);
	void			clear();
};

//Interface to all Entities, we need to save into toc
class MapObject
{
public:
	MapTocEntry& entry;


	MapObject();
	~MapObject();

	int getid();
};

class MapTocEntry
{
	int			id;
	MapObject&  object;

public:
	MapTocEntry( MapObject& mapobject, int id );
	~MapTocEntry();

	int getid();
	void setid(int i);
};

#endif // MAP_H_INCLUDED
