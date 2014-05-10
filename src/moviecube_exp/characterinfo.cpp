#include "game.h"
//#include "characterinfo.h"

namespace Character
{
    CharacterInfo::CharacterInfo() : playermodel(0), dogesture(false)
    {
        resetao();
    }

    CharacterInfo::~CharacterInfo() {}

    void CharacterInfo::setplayermodel(int i)
    {
        if(PlayerModel::PlayerModels().inrange(i))
        {
            playermodel = i;
        }
    }

    int CharacterInfo::getplayermodel()
    {
        return playermodel;
    }

    bool CharacterInfo::setgesture(int i)
    {
        PlayerModel::PlayerModelInfo& pm = PlayerModel::getPlayerModelInfo(playermodel);
        if(PlayerModel::HasAnimation(pm, i))
        {
            ao[AO_GESTURE] = i;
            return true;
        }
        else return false;
    }

    bool CharacterInfo::setao(int idxao, int idxanim)
    {
        if(idxao >= 0 && idxao < AO_NUM)
        {
            PlayerModel::PlayerModelInfo& pm = PlayerModel::getPlayerModelInfo(playermodel);
            if(PlayerModel::HasAnimation(pm, idxanim))
            {
                ao[idxao] = idxanim;
                return true;
            }
        }
        return false;
    }

    void CharacterInfo::attach(int num)
    {
        attacheditems.add(num);
    }

    void CharacterInfo::detach(int num)
    {
        attacheditems.remove(num);
    }

    void CharacterInfo::resetao()
    {
        ao[AO_DEAD] = ANIM_DEAD;
        ao[AO_DYING] = ANIM_DYING;
        ao[AO_IDLE] = ANIM_IDLE;
        ao[AO_FORWARD] = ANIM_FORWARD;
        ao[AO_BACKWARD] = ANIM_BACKWARD;
        ao[AO_LEFT] = ANIM_LEFT;
        ao[AO_RIGHT] = ANIM_RIGHT;
        ao[AO_HOLD] = ANIM_IDLE;
        ao[AO_ACTION] = ANIM_ATTACK1;
        ao[AO_PAIN] = ANIM_PAIN;
        ao[AO_GESTURE] = ANIM_TAUNT;
        ao[AO_JUMP] = ANIM_JUMP;
        ao[AO_SINK] = ANIM_SINK;
        ao[AO_EDIT] = ANIM_EDIT;
        ao[AO_LAG] = ANIM_LAG;
    }

    void playerattach(int i)
    {
    	CharacterInfo& info = game::player1->getcharinfo();
        info.attach(i);
        game::addmsg(SV_ATTACH, "rci", game::player1, i);
    }
    ICOMMAND(playerattach, "i", (int *i), playerattach(*i));

    void playerdetach(int i)
    {
    	CharacterInfo& info = game::player1->getcharinfo();
		info.detach(i);
    	game::addmsg(SV_DETACH, "rci", game::player1, i);
    }
    ICOMMAND(playerdetach, "i", (int *i), playerdetach(*i));
}
