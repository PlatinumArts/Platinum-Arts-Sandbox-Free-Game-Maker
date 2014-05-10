/*
 * control.h
 *
 *  Created on: 13.12.2009
 *      Author: offtools
 */

#ifndef __CONTROL_H__
#define __CONTROL_H__

struct DynamicEntity;

namespace Control
{
    class Control
    {
        //base class of an Control
        //Controlers provide functions for actors, ai, players ...
        //Control manipulate, control DynamicEntities (clients) on clientside
    public:
        int 			uid;
        DynamicEntity* 	dynent;
        bool 			isinit; //if false - waiting for a client

        Control();
        virtual ~Control();

        //init client, has dynent (client) now
        virtual void connect(DynamicEntity* de) = 0;

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

    ///TODO: should be a Singleton
    class ControlPlayer : public Control
    {
    public:
        bool k_left, k_right, k_up, k_down;
        bool k_rise, k_des;
        bool k_yawleft, k_yawright;
        bool k_action1, k_action2;

        ControlPlayer();
        ~ControlPlayer();

        void connect(DynamicEntity* de);
        void spawn();
        void update();
        void disconnect();
        Control* cleanup();

        //movement & action controls
        void forward(int down);
        void backward(int down);
        void left(int down);
        void right(int down);
        void turnleft(int down);
        void turnright(int down);
    };

    //All remote clients // Bots and Players from other clients
    class ControlRemote : public Control
    {
    public:
        ControlRemote();
        ControlRemote(DynamicEntity* de) : Control() { connect(de); }
        ~ControlRemote();

        void connect(DynamicEntity* de);
        void spawn();
        void update();
        void disconnect();
        Control* cleanup();
    };

    class ControlAi : public Control
    {
    public:
        ControlAi();

        ~ControlAi() {}

        void connect(DynamicEntity* de);
        void spawn();
        void update();
        void disconnect();
        Control* cleanup();
    };

    class ControlNPC : public Control, public Instance
    {
    public:
        ControlNPC(QueuedCommand& qu);

        ~ControlNPC();

        void connect(DynamicEntity* de);
        void spawn();
        void update();
        void disconnect();
        Control* cleanup();
    };
}
#endif /* __CONTROL_H__ */
