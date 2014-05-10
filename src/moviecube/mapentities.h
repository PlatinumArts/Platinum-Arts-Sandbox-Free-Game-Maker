#ifndef __MAP_ENTITIES_H__
#define __MAP_ENTITIES_H__

namespace MapEntities
{
    struct StaticEntity : extentity
    {
        int uid;

        StaticEntity() : uid(-1) {}

        //extentity& getextentity() { return (extentity&)this; }

        void setuid(int i) { (uid < 0) ? -1 : uid = i; }
        int  getuid() { return uid; }
    };

    extern vector<StaticEntity *> ents;

    int  getmapuid(extentity &e);
    StaticEntity* findmapuid(int uid);
    StaticEntity* findmapuid(int uid, int type);
    void setmapuid(int i, int uid);
    void sendmapuids(bool send);        //set flag
    bool sendmapuids();                 //query flag
    void putentities(packetbuf &p);     //send map entities with uid to server

    void initdynlights();
    void adddynlights();
    void renderentities();
    void renderentlight(StaticEntity &e);

    //wrapper functions - see entities.h
    void delentity(StaticEntity *e);
    void delentity(extentity *e);
    void clearents();
};

//namespace Map
//{
//	struct Map
//	{
//		int localuidcounter;
//		
//		Map() {}
//	} 	
	
//}
#endif // __MAP_ENTITIES_H__
