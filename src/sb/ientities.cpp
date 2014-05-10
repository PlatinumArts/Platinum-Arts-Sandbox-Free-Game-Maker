#include "cube.h"
#include "handlerfactory.h"
#include "extentity.h"
#include "extentityhandler.h"

vector<extentity*> ents;

namespace entities
{
	extentity *newentity()
	{
//		return Extentities::Get().newentity();
//		return new Extentities::Extentity();
		return Extentities::NewEntity();
	}

	void deleteentity(extentity *e)
	{
//		Extentities::Get().deleteentity(e);
//		delete (Extentities::Extentity *)e;
		Extentities::DeleteEntity(e);
	}

	void clearents()
	{
//		Extentities::Get().clearents();
//		loopvrev(ents) { delete (Extentities::Extentity *) ents[i]; }
		loopvrev(ents) { Extentities::DeleteEntity(ents[i]); }
		ents.shrink(0);
	}

	vector<extentity *> &getents()
	{
//		return Extentities::Get().getents();
		return ents;
	}

	///TODO: editent depends on network code, we have to move this somewhere else :(
	//void editent(int i, bool local)
	//{
	//
	//}

	const char *entnameinfo(entity &e)
	{
//		return Extentities::Handler()[e.type]->info(e);
		return Extentities::Handler()[e.type]->info(e);
	}

	const char *entname(int i)
	{
		return Extentities::Handler()[i]->getname();
	}

	const int numattrs(int type)
	{
		return Extentities::Handler()[type]->numattrs();
	}

	//we dont save additional entity data into the map,
	//all information will be stored in cfg files
	int extraentinfosize() { return 0; }
	void writeent(entity &e, char *buf) {}
	void readent(entity &e, char *buf) {}

	float dropheight(entity &e)
	{
		return Extentities::Handler()[e.type]->getdropheight(e);
	}

	void fixentity(extentity &e)
	{
		Extentities::Handler()[e.type]->fix(e);
	}

	void entradius(extentity &e, bool &color)
	{
		return Extentities::Handler()[e.type]->renderhelper(e, color);
	}

	///TODO: no use for this at the moment, move it into the handlers later
	bool mayattach(extentity &e) { return false; }
	bool attachent(extentity &e, extentity &a) { return false; }

	bool printent(extentity &e, char *buf)
	{
		return Extentities::Handler()[e.type]->info(e, buf);
	}

	bool dirent(extentity &e)
	{
		return Extentities::Handler()[e.type]->hasdir(e);
	}

	bool radiusent(extentity &e)
	{
		return Extentities::Handler()[e.type]->hasradius();
	}

	void renderhelpertext(extentity &e, int &colour, vec &pos, string &tmp)
	{
		return Extentities::Handler()[e.type]->renderhelpertext(e, colour, pos, tmp);
	}

	const char *entmodel(const entity &e)
	{
		return Extentities::Handler()[e.type]->getmodel();
	}

	///TODO: implement map model animation
	void animatemapmodel(const extentity &e, int &anim, int &basetime) {}

	int *getmodelattr(extentity &e)
	{
		return Extentities::Handler()[e.type]->getmodelattr(e);
	}

	bool checkmodelusage(extentity &e, int i)
	{
		return Extentities::Handler()[e.type]->checkmodelusage(e, i);
	}
}
