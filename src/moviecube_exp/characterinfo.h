#ifndef __CHARACTERINFO_H__
#define __CHARACTERINFO_H__

#include "playermodelinfo.h"

namespace Character
{
    enum
    {
        AO_DEAD = 0,
        AO_DYING,   //1
        AO_IDLE,    //2
        AO_FORWARD, //3
        AO_BACKWARD,//4
        AO_LEFT,    //5
        AO_RIGHT,   //6
        AO_HOLD,    //7
        AO_ACTION,  //8
        AO_GESTURE, //9
        AO_PAIN,    //10
        AO_JUMP,    //11
        AO_SINK,    //12
        AO_SWIM,    //13
        AO_EDIT,    //14
        AO_LAG,     //15
        AO_NUM      //16
    };

    struct CharacterInfo
    {
        int                     playermodel;
        vector<int>             attacheditems;
        int                     ao[AO_NUM];
        bool                    dogesture;

        CharacterInfo();
        ~CharacterInfo();

        void setplayermodel(int i);

        int getplayermodel();

        bool setgesture(int i);

        bool setao(int ao, int i);

        void attach(int i);

        void detach(int i);

        void resetao();
    };
}
#endif // __CHARACTERINFO_H__
