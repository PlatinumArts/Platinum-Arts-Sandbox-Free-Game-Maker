#ifndef __PLAYERMODELINFO_H__
#define __PLAYERMODELINFO_H__

//#include "cube.h"
#include "engine.h" //if you get a fdopen error, comment this and uncomment #include "cube.h"

/////////////////////////////////////////////////////////////
//            Playermodel Definition
/////////////////////////////////////////////////////////////

namespace PlayerModel
{
    /////////////////////////////////////////////////////////
    // __offtools__: attachments (items, clothes ...)
    // following tags should appear in the model config file
    /////////////////////////////////////////////////////////

    enum { ATTACH_HEAD = 0,
        ATTACH_NECK,
        ATTACH_LEFTSHOULDER,
        ATTACH_RIGHTSHOULDER,
        ATTACH_LEFTARM,
        ATTACH_RIGHTARM,
        ATTACH_LEFTFOREARM,
        ATTACH_RIGHTFOREARM,
        ATTACH_LEFTHAND,
        ATTACH_RIGHTHAND,
        ATTACH_BELLY,
        ATTACH_HIP,
        ATTACH_LEFTTHIGH,
        ATTACH_RIGHTTHIGH,
        ATTACH_LEFTSHIN,
        ATTACH_RIGHTSHIN,
        ATTACH_LEFTFOOT,
        ATTACH_RIGHTFOOT,
        ATTACH_ARMOUR,
        ATTACH_WEAPON,
        ATTACH_POWERUP,
        ATTACH_MUZZLE,
        ATTACH_NUMPARTS
    };

    //////////////////////////////////////////////////////////
    //__offtools__:
    //tag_name, body part name translations used in gui
    //////////////////////////////////////////////////////////

    static const struct attachinfo { const char* part; const char* tag; } taginfo[ATTACH_NUMPARTS] =
    {
            { "Head", "tag_head" },
            { "Neck", "tag_neck" },
            { "LeftShoulder", "tag_lshoulder" },
            { "RightShoulder", "tag_rshoulder" },
            { "LeftArm", "tag_larm" },
            { "RightArm", "tag_rarm" },
            { "LeftForearm", "tag_lforearm" },
            { "RightForearm", "tag_rforearm" },
            { "LeftHand", "tag_lhand" },
            { "RightHand", "tag_rhand" },
            { "Belly", "tag_belly" },
            { "Hip", "tag_hip" },
            { "LeftThigh", "tag_lthigh" },
            { "RightThigh", "tag_rthigh" },
            { "LeftShin", "tag_lshin" },
            { "RightShin", "tag_rshin" },
            { "LeftFoot", "tag_lfoot" },
            { "RightFoot", "tag_rfoot" },
            { "Armour", "tag_armour" },
            { "Weapon", "tag_weapon" },
            { "Powerup", "tag_powerup" },
            { "Muzzle", "tag_muzzle" }
    };

    ///////////////////////////////////////////////////////////
    // offtools:
    // animation handling of the attachments (not implemented)
    //
    // handle animation:
    // ATTACH_ANIM_NONE - static, no animation
    // ATTACH_ANIM_IDLE - only use idle animation
    // ATTACH_ANIM_FULL - use current animation of model
    //
    // ao (animation overwrite, e.g. holding a flag)
    // not implemeted
    ///////////////////////////////////////////////////////////

    enum { ATTACH_ANIM_NONE = 0, ATTACH_ANIM_IDLE, ATTACH_ANIM_FULL, ATTACH_ANIM_NUMRULES };

    //////////////////////////////////////////////////////////
    //__offtools__:
    //Playermodel Definition
    //////////////////////////////////////////////////////////
    struct PlayerModelInfo
    {
        struct animationinfo
        {
            int index;
            string descr;

            animationinfo(char* s, int num) : index(num)
            {
                strcpy(descr, s);
            }
        };

        struct attachmentinfo
        {
            int bodypart;
            int rule;
            string model;

            attachmentinfo(int _part, char* _model, int _rule) : bodypart(_part), rule(_rule)
            {
                strcpy(model, _model);
            }

            const char* gettag(int bpart)
            {
                if(bpart < ATTACH_NUMPARTS || bpart >= 0) { return taginfo[bpart].tag; }
                else { return NULL; }
            }

            const char* getbodypart(int bpart)
            {
                if(bpart < ATTACH_NUMPARTS || bpart >= 0) { return taginfo[bpart].part; }
                else { return NULL; }
            }
        };

        char* name;
        bool ragdoll, selectable;
        vector<animationinfo *> animinfo;
        vector<attachmentinfo *> iteminfo;

        PlayerModelInfo(const char* _name);

        void registeranimation(char *anim, int num);

        void registeritem( int part, char* model, int rule);

        bool hasanimation(int anim);

        char* getname();
    };

    int RegisterModel(const char *s);

	void RegisterItem(int playermodel, int part, char* model, int rule);

    void RegisterAnimation(char *loadname, char* anim, int num);

    void BuildModelAttach(PlayerModelInfo& pm, int itemnum, modelattach& a);

    bool HasAnimation(PlayerModelInfo& pm, int anim);

    const char* GetName(PlayerModelInfo& pm);

    vector<PlayerModelInfo*> PlayerModels();

    PlayerModelInfo& getPlayerModelInfo(int i);
}
#endif // __PLAYERMODELINFO_H__
