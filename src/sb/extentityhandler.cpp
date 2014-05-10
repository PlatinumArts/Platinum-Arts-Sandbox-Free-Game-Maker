#include "extentityhandler.h"
#include "extentity.h"

EntityHandler::EntityHandler() : typestring(newstring("none?")), model(newstring("")), radius(false), dir(false), numattr(0), dropheight(0), cap(CAP_NONE) {}

EntityHandler::~EntityHandler()
{
	delete[] typestring;
	delete[] model;
}

const char* EntityHandler::getname() { return typestring; }
const char* EntityHandler::getmodel() { return model; }
int* EntityHandler::getmodelattr(extentity &e) { return NULL; }

void EntityHandler::fix(extentity &e) {}

bool EntityHandler::hasradius() { return radius; }
bool EntityHandler::hasdir(extentity &e) { return dir; }
bool EntityHandler::hascap(EntityCap flag) { return cap&flag; }
bool EntityHandler::info(extentity &e, char *buf) { return false; }
const char* EntityHandler::info(entity &e) { return typestring; }
const int EntityHandler::numattrs() { return numattr; }

void EntityHandler::renderhelper(extentity &e, bool &color) {}
void EntityHandler::renderhelpertext(const extentity &e, int &colour, vec &pos, string &tmp) {}

float EntityHandler::getdropheight(entity &e) { return dropheight; }
bool EntityHandler::checkmodelusage(extentity &e, int i) { return false; }

void EntityHandler::writeent(entity &e, char *buf) {}   // write any additional data to disk (except for ET_ ents)
void EntityHandler::readent(entity &e, char *buf) {}    // read from disk, and init

EntityHandlerLight::EntityHandlerLight()
{
	delete[] typestring; typestring = newstring("light");
	radius = true;
}

EntityHandlerMapmodel::EntityHandlerMapmodel()
{
	delete[] typestring; typestring = newstring("mapmodel");
	dir = true;
}

void EntityHandlerMapmodel::renderhelper(extentity &e, bool &color)
{
	vec direction;
	vecfromyawpitch(e.attr[0], 0, 1, 0, direction);
	renderentarrow(e, direction, 4);
}

EntityHandlerPlayerstart::EntityHandlerPlayerstart()
{
	delete[] typestring; typestring = newstring("playerstart");
	dir = true;
}

EntityHandlerMapsound::EntityHandlerMapsound()
{
	delete[] typestring; typestring = newstring("mapsound");
	radius = true;
}

EntityHandlerSpotlight::EntityHandlerSpotlight()
{
	delete[] typestring; typestring = newstring("spotlight");
	dir = true;
}

EntityHandlerEnvmap::EntityHandlerEnvmap()
{
	delete[] typestring; typestring = newstring("envmap");
	radius = true;
}

EntityHandlerParticles::EntityHandlerParticles()
{
	delete[] typestring; typestring = newstring("particles");
}

EntityHandlerTeleport::EntityHandlerTeleport()
{
	numattr = 3;
	delete[] typestring; typestring = newstring("teleport");
	dir = true;
}

void EntityHandlerTeleport::renderhelpertext(const extentity &e, int &colour, vec &pos, string &tmp)
{
	pos.z += 3.0;
	formatstring(tmp)("Teledest Tag: %i\nModel: %s (%i)\nTag: %i",
		e.attr[0],
		mapmodelname(e.attr[1]),
		e.attr[1]
	);
}

int *EntityHandlerTeleport::getmodelattr(extentity &e)
{
	return &e.attr[1];
}

bool EntityHandlerTeleport::checkmodelusage(extentity &e, int i)
{
	if(e.attr[1] == 0)
		return false;
	return true;
}

///TODO: query caps not type
void EntityHandlerTeleport::renderhelper(extentity &e, bool &color)
{
	vector<extentity*> &ents = entities::getents();
	loopv(ents) if(ents[i]->type == TELEDEST && e.attr[0]==ents[i]->attr[1])
	{
		renderentarrow(e, vec(ents[i]->o).sub(e.o).normalize(), e.o.dist(ents[i]->o));
		break;
	}
}

EntityHandlerTeledest::EntityHandlerTeledest()
{
	numattr = 2;
	cap = CAP_TELEDEST;
	delete[] typestring; typestring = newstring("teledest");
	dir = true;
}

///TODO: check namespace player1
void EntityHandlerTeledest::fix(extentity &e)
{
	e.attr.pop();
	e.attr.insert(0, game::iterdynents(0)->yaw);
}

void EntityHandlerTeledest::renderhelper(extentity &e, bool &color)
{
	vec direction;
	vecfromyawpitch(e.attr[0], 0, 1, 0, direction);
	renderentarrow(e, direction, 4);
}

void EntityHandlerTeledest::renderhelpertext(const extentity &e, int &colour, vec &pos, string &tmp)
{
	pos.z += 3;
	formatstring(tmp)("Yaw: %i\nTeleport Tag: %i\n",
			e.attr[0],
			e.attr[1]
	);
}

EntityHandlerMonster::EntityHandlerMonster()
{
	delete[] typestring; typestring = newstring("monster");
}

EntityHandlerJumppad::EntityHandlerJumppad()
{
	numattr = 3;
	cap = CAP_JUPPAD;
	delete[] typestring; typestring = newstring("jumppad");
	dir = true;
}

void EntityHandlerJumppad::renderhelper(extentity &e, bool &color)
{
	renderentarrow(e, vec((int)(char)e.attr[2]*10.0f, (int)(char)e.attr[1]*10.0f, e.attr[0]*12.5f).normalize(), 4);
}

void EntityHandlerJumppad::renderhelpertext(const extentity &e, int &colour, vec &pos, string &tmp)
{
	pos.z += 4.5;
	formatstring(tmp)("Z: %i\nY: %i\nX: %i",
			e.attr[0],
			e.attr[1],
			e.attr[2]
	);
}

EntityHandlerBox::EntityHandlerBox()
{
	numattr = 4;
	delete[] typestring; typestring = newstring("box");
	dir = true;
}

void EntityHandlerBox::fix(extentity& e)
{
	e.attr.pop();
	e.attr.insert(0, game::iterdynents(0)->yaw);
}

void EntityHandlerBox::renderhelper(extentity &e, bool &color)
{
	vec direction;
	vecfromyawpitch(e.attr[0], 0, 1, 0, direction);
	renderentarrow(e, direction, 4);
}

void EntityHandlerBox::renderhelpertext(const extentity &e, int &colour, vec &pos, string &tmp)
{
	pos.z += 6.0;
	formatstring(tmp)("Yaw: %i\nModel: %s (%i)\nWeight: %i\nHealth: %i",
		e.attr[0],
		mapmodelname(e.attr[1]), e.attr[1],
		e.attr[2],
		e.attr[3]
	);
}

int *EntityHandlerBox::getmodelattr(extentity &e)
{
	return &e.attr[1];
}

bool EntityHandlerBox::checkmodelusage(extentity &e, int i)
{
	return (e.attr[1] == i);
}

EntityHandlerBarrel::EntityHandlerBarrel()
{
	delete[] typestring; typestring = newstring("barrel");
}

EntityHandlerPlatform::EntityHandlerPlatform()
{
	numattr = 4;
	delete[] typestring; typestring = newstring("platform");
	dir = true;
}

void EntityHandlerPlatform::fix(extentity& e)
{
	e.attr.pop();
	e.attr.insert(0, game::iterdynents(0)->yaw);
}

void EntityHandlerPlatform::renderhelper(extentity &e, bool &color)
{
	vec direction;
	vecfromyawpitch(e.attr[0], 0, 1, 0, direction);
	renderentarrow(e, direction, 4);
}

void EntityHandlerPlatform::renderhelpertext(const extentity &e, int &colour, vec &pos, string &tmp)
{
	pos.z += 6.0;
	formatstring(tmp)("Yaw: %i\nModel: %s (%i)\nTag: %i\nSpeed: %i",
			e.attr[0],
			mapmodelname(e.attr[1]), e.attr[1],
			e.attr[2],
			e.attr[3]
	);
}

int *EntityHandlerPlatform::getmodelattr(extentity &e)
{
	return &e.attr[1];
}

bool EntityHandlerPlatform::checkmodelusage(extentity &e, int i)
{
	return (e.attr[1] == i);
}

EntityHandlerElevator::EntityHandlerElevator()
{
	delete[] typestring; typestring = newstring("elevator");
}

EntityHandlerWaypoint::EntityHandlerWaypoint()
{
	numattr = 3;
	delete[] typestring; typestring = newstring("waypoint");
	radius = true;
	dir = true;
}

void EntityHandlerWaypoint::fix(extentity& e)
{
	e.attr[0] = (int)game::iterdynents(0)->yaw;
	if(!e.attr[1]) e.attr[1] = 25.0f;
}

void EntityHandlerWaypoint::renderhelper(extentity &e, bool &color)
{
	renderentsphere(e, e.attr[1]);
	vec direction;
	vecfromyawpitch(e.attr[0], 0, 1, 0, direction);
	renderentarrow(e, direction, e.attr[1]);
}

void EntityHandlerWaypoint::renderhelpertext(const extentity &e, int &colour, vec &pos, string &tmp)
{
	pos.z += 4.5f;
	formatstring(tmp)("Direction %i\nRadius %i",
			e.attr[0],
			e.attr[1]
	);
}

EntityHandlerCamera::EntityHandlerCamera()
{
	numattr = 3;
	delete[] typestring; typestring = newstring("camera");
}

void EntityHandlerCamera::renderhelper(extentity &e, bool &color)
{
	vec direction;
	vecfromyawpitch(e.attr[0], e.attr[1], 1, 0, direction);
	renderentarrow(e, direction, 4);
}

void EntityHandlerCamera::renderhelpertext(const extentity &e, int &colour, vec &pos, string &tmp)
{
	pos.z += 7.5;
	formatstring(tmp)("Yaw: %i\nPitch: %i\nDistance: %i",
		e.attr[0],
		e.attr[1],
		e.attr[2]
	);
}

EntityHandlerDynlight::EntityHandlerDynlight()
{
	numattr = 5;
	delete[] typestring; typestring = newstring("dynlight");
	radius = true;
}

void EntityHandlerDynlight::renderhelper(extentity &e, bool &color)
{
	if(color) glColor3f(e.attr[1]/255.0f, e.attr[2]/255.0f, e.attr[3]/255.0f);
	renderentsphere(e, e.attr[0]);
}

void EntityHandlerDynlight::renderhelpertext(const extentity &e, int &colour, vec &pos, string &tmp)
{
	pos.z += 9.0;
	formatstring(tmp)("Radius %i\n\fs\fRRed: %i\n\fJGreen: %i\n\fDBlue: %i\fr\nTag: %i",
			e.attr[0],
			e.attr[1],
			e.attr[2],
			e.attr[3],
			e.attr[4]
	);
}

EntityHandlerGeneric::EntityHandlerGeneric()
{
	numattr = 8;
	delete[] typestring; typestring = newstring("generic");
}

void EntityHandlerGeneric::fix(extentity& e)
{
	if(!e.attr[0]) e.attr[0] = 25.0f;
}

void EntityHandlerGeneric::renderhelper(extentity &e, bool &color)
{
	renderentsphere(e, e.attr[0]);
}
