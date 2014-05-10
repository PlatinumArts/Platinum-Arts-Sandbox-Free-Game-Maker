#include "map.h"
#include "mapentities.h"

VARR(toclocked, 0, 1, 1);

MapToc::MapToc() : lastid(-1) {}
MapToc::~MapToc() {}

MapTocEntry& MapToc::registerentry( MapObject& mapobject )
{
	int id = ++lastid;
	conoutf("MapToc::registerentry id %d", id);
	return *toc.add(new MapTocEntry(mapobject, id));
}

void MapToc::removeentry( int id ) {}

MapTocEntry* MapToc::lookupentry(int id)
{
	loopv(toc) if(toc[i]->getid() == id)
	{
		return toc[i];
	}
	return NULL;
}

///TODO: implementation only works on MapEntities now, this is just for testing and acts
///as replacment for mapuid. Later it will used as general Lookup for all kind of entities
///and instances
void MapToc::writetoc(stream *f)
{
//	int idx = 0;
//	f->printf("tocreset\n");
//	loopv(MapEntities::ents)
//	{
//		MapEntities::StaticEntity* ent = MapEntities::ents[i];
//		if(ent->type != ET_EMPTY)
//		{
//			conoutf("MapToc::writetoc - not implementet, ent idx: %d, num: %d, id: %d", i, idx, ent->getid());
//			f->printf("toc %d %d\n", idx, ent->getid());
//			idx++;
//		}
//	}
}

void MapToc::readtoc(int idx, int id)
{
//	if( toclocked )
//	{
//		conoutf("toc command is locked");
//	}
//
//	if( MapEntities::ents.inrange(idx) )
//	{
//		MapEntities::StaticEntity* ent = MapEntities::ents[idx];
//		ent->entry.setid(id);
//		if(lastid < id)
//		{
//			lastid = id;
//		}
//	}
}

void MapToc::lock(bool lk)
{
	toclocked = lk;
}

void MapToc::clear()
{
	lastid = -1;
}

MapObject::MapObject() : entry(MapToc::Get ().registerentry(*this))
{
	conoutf("MapObject::MapObject");
}

MapObject::~MapObject() {}

int MapObject::getid() { return entry.getid(); }

MapTocEntry::MapTocEntry( MapObject& mapobject, int id ) : id(id), object(mapobject) {}

MapTocEntry::~MapTocEntry() {}

int MapTocEntry::getid()
{
	return id;
}

void MapTocEntry::setid(int i)
{
	id = i;
}

ICOMMAND(toc, "ii", (int *idx, int *id), MapToc::Get().readtoc(*idx, *id));
ICOMMAND(tocreset, "", (),
		MapToc::Get ().clear();
//		toclocked = 0;
		MapToc::Get ().lock(false);
);
