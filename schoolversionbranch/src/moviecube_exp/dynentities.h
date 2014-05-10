#ifndef __DYNAMICENTITIES_H__
#define __DYNAMICENTITIES_H__

//#include "characterinfo.h"
//#include "game.h"

//enum { CONTROL_PLAYER = 0, CONTROL_CHARACTER, CONTROL_REMOTE, CONTROL_AI, CONTROL_MAX };
//enum { CLIENT_PLAYER = 0, CLIENT_BOT };
//
//struct dynentstate
//{
//    int clienttype;
//
//    dynentstate() : clienttype(CLIENT_PLAYER) {} //TODO: change this to NONE on implementing ControlPlayer
//
//    bool canpickup(int type) { return false; }
//
//    void pickup(int type) {}
//
//    void respawn() {}
//
//    void spawnstate(int gamemode)
//    {
////        if(m_demo) {}
//    }
//};
//
//struct DynamicEntity : dynent, dynentstate
//{
//    int weight;  // affects the effectiveness of hitpush
//    int clientnum, privilege, lastupdate, plag, ping;
//    int lastaction;
//    int basetime;
//    int lastpickup, lastpickupmillis;
//    editinfo *edit;
//    float deltayaw, deltapitch, newyaw, newpitch;
//    int smoothmillis;
//    string name, info;
//    int ownernum;
//    int controltype;
//    vec muzzle;
//	Character::CharacterInfo charinfo;
//
//    DynamicEntity() : weight(100), clientnum(-1), privilege(PRIV_NONE), lastupdate(0), plag(0), ping(0), basetime(0), edit(NULL), smoothmillis(-1), ownernum(-1), controltype(CONTROL_PLAYER), muzzle(-1, -1, -1)
//    {
//        name[0] = info[0] = 0;
//        respawn();
//    }
//    ~DynamicEntity()
//    {
////        conoutf("DEBUG: DynamicEntity::~DynamicEntity cn: %d", clientnum);
//        freeeditinfo(edit);
//    }
//
//	Character::CharacterInfo& getcharinfo() { return charinfo; }
//
//    void respawn()
//    {
////        conoutf("DEBUG: DynamicEntity::respawn cn: %d", clientnum);
//        dynent::reset();
//        dynentstate::respawn();
//        lastaction = 0;
//        lastpickup = -1;
//        lastpickupmillis = 0;
//    ///TODO: check this
////        charinfo.resetao();
//    }
//};

#endif // __DYNAMICENTITIES_H__
