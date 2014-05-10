/*
 * control.h
 *
 *  Created on: 13.12.2009
 *      Author: offtools
 */

#ifndef __CONTROL_H__
#define __CONTROL_H__

/// TODO (tha#1#): dynent cleanup or notify on deleting of Controler

struct DynamicEntity;

struct Control
{
	//base class of an Control
	//Controlers provide functions for actors, ai, players ...
	//Control manipulate, control DynamicEntities (clients) on clientside

	int 			uid;
	DynamicEntity* 	dynent;
	bool 			isinit; //if false - waiting for a client

	Control() : uid(-1), dynent(NULL), isinit(false) {}

	virtual ~Control() {}

    //check type of an control
    virtual int type()=0;

    //init client, has dynent (client) now
	virtual void init(DynamicEntity* de) = 0;

    //spawn client
    virtual void spawn() = 0;

	//update client
	virtual void update() = 0;

    //connect client
    //virtual void connect() = 0;

    //disconnect client
    virtual void disconnect() = 0;

	//called after client was disconnected from server or by Destructor
	virtual Control* cleanup() = 0;

    //check state, only init at the moment
	bool getstate() { return isinit; }
};

//TODO: should be a singleton
struct ControlPlayer : Control
{
	ControlPlayer() : Control() {}

	~ControlPlayer() {}

	int type() {return CONTROL_PLAYER;}
	void init(DynamicEntity* de);
	void spawn();
	void update();
	void disconnect();
	Control* cleanup();
};

//All remote clients // Bots and Players from other clients
struct ControlRemote : Control
{
	ControlRemote() : Control() {}
	ControlRemote(DynamicEntity* de) : Control() { init(de); }

	~ControlRemote();

	int type() {return CONTROL_REMOTE;}
	void init(DynamicEntity* de);
	void spawn();
	void update();
	void disconnect();
	Control* cleanup();
};

struct ControlAi : Control
{
//    ai::aiinfo *ai;

	ControlAi() : Control() {}

	~ControlAi() {}

	int type() {return CONTROL_AI;}
	void init(DynamicEntity* de);
	void spawn();
	void update();
	void disconnect();
	Control* cleanup();
};

#include "action.h"

struct ControlCharacter : Control, ActionInstance
{
	//new actor using an exisiting actionlib
	ControlCharacter(ActionLib& _lib);

	~ControlCharacter();

	int type() {return CONTROL_CHARACTER;}
	void init(DynamicEntity* de);
	void spawn();
	void update();
	void disconnect();
	Control* cleanup();
};

#endif /* __CONTROL_H__ */
