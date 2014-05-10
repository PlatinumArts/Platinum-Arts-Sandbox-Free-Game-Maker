#ifndef SB_EXTENTITY_H_INCLUDED
#define SB_EXTENTITY_H_INCLUDED

#include "cube.h"
#include "handlerfactory.h"
#include "objectgenerator.h"
#include "extentityhandler.h"

///TODO:
///*add something like an adapter for Extentity, result should be something like:
///Exentities::Init<MyExtentityType>();
///which can be used to work with derived Extentity Types (newentity, delentity should know these types)
///
///*use HandlerFactory<...>::Register(int typeid, const char unique_name, bool check_unique_name = true)


enum				          	// static entity types
{
	NOTUSED = ET_EMPTY,         // entity slot not in use in map
	LIGHT = ET_LIGHT,           // lightsource, attr1 = radius, attr2 = intensity
	MAPMODEL = ET_MAPMODEL,     // attr1 = angle, attr2 = idx
	PLAYERSTART = ET_PLAYERSTART,// attr1 = angle, attr2 = team
	ENVMAP = ET_ENVMAP,         // attr1 = radius
	PARTICLES = ET_PARTICLES,
	MAPSOUND = ET_SOUND,
	SPOTLIGHT = ET_SPOTLIGHT,
	TELEPORT = ET_GAMESPECIFIC,  // attr1 = idx
	TELEDEST,                   // attr1 = angle, attr2 = idx
	MONSTER,                    // attr1 = angle, attr2 = monstertype
	JUMPPAD,                    // attr1 = zpush, attr2 = ypush, attr3 = xpush
	BOX,                        // attr1 = angle, attr2 = idx, attr3 = weight
	BARREL,                     // attr1 = angle, attr2 = idx, attr3 = weight, attr4 = health
	PLATFORM,                   // attr1 = angle, attr2 = idx, attr3 = tag, attr4 = speed
	ELEVATOR,                   // attr1 = angle, attr2 = idx, attr3 = tag, attr4 = speed
	WAYPOINT,
	CAMERA,                     //attr1 = pitch, attr2 = yaw, attr3 = focus
	DYNLIGHT,
	GENERIC,
	MAXBASICENTTYPES
};

namespace Extentities
{
	//just a placeholder for the moment, will store a reference to an object listed
	//in a map object registry or toc
	class Extentity : public extentity
	{
		public:
			virtual ~Extentity() {}
	};

	extentity *NewEntity();
	void RegisterStdHandler();
	void DeleteEntity(extentity *e);
	void Destroy();

	HandlerFactory<EntityHandler, int>& Handler();
	ObjectGenerator<Extentity>& Generator();

	template<class T> void Init()
	{
		Generator().Init<T>();
		RegisterStdHandler();
	}
}

#endif // SB_EXTENTITY_H_INCLUDED
