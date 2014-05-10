#ifndef STATICENTITYHANDLER_H_INCLUDED
#define STATICENTITYHANDLER_H_INCLUDED

#include "cube.h"

//Basic Handler Class for extentities
//implements the entities namespcace from igame.h

///TODO:
///*do not query types in handlers, use caps
///*query caps in Teleport (for CAP_TELEDEST)
///*remove second text under entities

//capilities of an entity
enum EntityCap
{
	CAP_NONE        =  0,
	CAP_TELEPORT    =  1 << 0,
	CAP_TELEDEST    =  1 << 1,
	CAP_TRIGGER     =  1 << 2,
	CAP_JUPPAD      =  1 << 3,
	CAP_ATTACH      =  1 << 4,
	CAP_PICKUP      =  1 << 5
};

class EntityHandler
{
public:
	EntityHandler();
	virtual ~EntityHandler();

	virtual const char *getname();
	virtual const char *getmodel();
	virtual int *getmodelattr(extentity &e);

	virtual void fix(extentity &e);

	virtual bool hasradius();
	virtual bool hasdir(extentity &e);
	virtual bool hascap(EntityCap cap);
	virtual bool info(extentity &e, char *buf); //printent
	virtual const char *info(entity &e); //entnameinfo
	virtual const int numattrs();

	virtual void renderhelper(extentity &e, bool &color);
	virtual void renderhelpertext(const extentity &e, int &colour, vec &pos, string &tmp);

	virtual float getdropheight(entity &e);
	virtual bool checkmodelusage(extentity &e, int i);

	virtual void writeent(entity &e, char *buf);   // write any additional data to disk (except for ET_ ents)
	virtual void readent(entity &e, char *buf);     // read from disk, and init

protected:
	char* typestring;
	char* model;
	bool radius;
	bool dir;
	int numattr;
	float dropheight;
	EntityCap cap;
};


class EntityHandlerLight : public EntityHandler
{
public:
	EntityHandlerLight();
};

class EntityHandlerMapmodel : public EntityHandler
{
public:
	EntityHandlerMapmodel();
	void renderhelper(extentity &e, bool &color);
};

class EntityHandlerPlayerstart : public EntityHandler
{
public:
	EntityHandlerPlayerstart();
};

class EntityHandlerEnvmap : public EntityHandler
{
public:
	EntityHandlerEnvmap();
};

class EntityHandlerParticles : public EntityHandler
{
public:
	EntityHandlerParticles();
};

class EntityHandlerMapsound : public EntityHandler
{
public:
	EntityHandlerMapsound();
};

class EntityHandlerSpotlight : public EntityHandler
{
public:
	EntityHandlerSpotlight();
};

class EntityHandlerTeleport : public EntityHandler
{
public:
	EntityHandlerTeleport();
	void renderhelpertext(const extentity &e, int &colour, vec &pos, string &tmp);
	int *getmodelattr(extentity &e);
	bool checkmodelusage(extentity &e, int i);
	void renderhelper(extentity &e, bool &color);
};

class EntityHandlerTeledest : public EntityHandler
{
public:
	EntityHandlerTeledest();
	void fix(extentity &e);
	void renderhelper(extentity &e, bool &color);
	void renderhelpertext(const extentity &e, int &colour, vec &pos, string &tmp);
};

class EntityHandlerMonster : public EntityHandler
{
public:
	EntityHandlerMonster();
};

class EntityHandlerJumppad : public EntityHandler
{
public:
	EntityHandlerJumppad();
	void renderhelper(extentity &e, bool &color);
	void renderhelpertext(const extentity &e, int &colour, vec &pos, string &tmp);
};

///TODO: Put all Movable Ents under a base class
class EntityHandlerBox : public EntityHandler
{
public:
	EntityHandlerBox();
	void fix(extentity& e);
	void renderhelper(extentity &e, bool &color);
	void renderhelpertext(const extentity &e, int &colour, vec &pos, string &tmp);
	int *getmodelattr(extentity &e);
	bool checkmodelusage(extentity &e, int i);
};

class EntityHandlerBarrel : public EntityHandler
{
public:
	EntityHandlerBarrel();
};

class EntityHandlerPlatform : public EntityHandler
{
public:
	EntityHandlerPlatform();
	void fix(extentity& e);
	void renderhelper(extentity &e, bool &color);
	void renderhelpertext(const extentity &e, int &colour, vec &pos, string &tmp);
	int *getmodelattr(extentity &e);
	bool checkmodelusage(extentity &e, int i);
};

class EntityHandlerElevator : public EntityHandler
{
public:
	EntityHandlerElevator();
};

class EntityHandlerWaypoint : public EntityHandler
{
public:
	EntityHandlerWaypoint();
	void fix(extentity& e);
	void renderhelper(extentity &e, bool &color);
	void renderhelpertext(const extentity &e, int &colour, vec &pos, string &tmp);
};

class EntityHandlerCamera : public EntityHandler
{
public:
	EntityHandlerCamera();
	void renderhelper(extentity &e, bool &color);
	void renderhelpertext(const extentity &e, int &colour, vec &pos, string &tmp);
};

class EntityHandlerDynlight : public EntityHandler
{
public:
	EntityHandlerDynlight();
	void renderhelper(extentity &e, bool &color);
	void renderhelpertext(const extentity &e, int &colour, vec &pos, string &tmp);
};

class EntityHandlerGeneric : public EntityHandler
{
public:
	EntityHandlerGeneric();
	void fix(extentity& e);
	void renderhelper(extentity &e, bool &color);
};

#endif // STATICENTITYHANDLER_H_INCLUDED
