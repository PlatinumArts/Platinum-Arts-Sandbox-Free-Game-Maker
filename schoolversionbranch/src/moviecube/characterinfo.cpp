#include "game.h"

CharacterInfo::CharacterInfo() : playermodel(0), dogesture(false)
{
    resetao();
}

CharacterInfo::~CharacterInfo() {}

void CharacterInfo::setplayermodel(int i)
{
    if(game::playermodels.inrange(i))
    {
        playermodel = i;
    }
}

int CharacterInfo::getplayermodel()
{
    return playermodel;
}

bool CharacterInfo::setactivegesture(int i)
{
    if(game::playermodels[playermodel]->getanimdescr(i))
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
        if(game::playermodels[playermodel]->getanimdescr(idxanim))
        {
            ao[idxao] = idxanim;
            return true;
        }
    }
    return false;
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
    ao[AO_SWIM] = ANIM_SWIM;
    ao[AO_EDIT] = ANIM_EDIT;
    ao[AO_LAG] = ANIM_LAG;
}

void listanimations(int idx)
{
    vector<char> buf;
    string lst;
    if(game::playermodels.inrange(idx))
    {
        PlayerModelInfo* info = game::playermodels[idx];
        loopv(info->animinfo)
        {
            formatstring(lst)("%s", info->animinfo[i]->descr);
            buf.put(lst, strlen(lst));
            if(i < (info->animinfo.length()-1)) buf.add(' ');
        }
        buf.add('\0');
        result(buf.getbuf());
    }
}
ICOMMAND(listanimations, "i", (int *mdl), listanimations(*mdl));

int getanimation(int mdl, char* anim)
{
    if(game::playermodels.inrange(mdl))
    {
		PlayerModelInfo* info = game::playermodels[mdl];
		return info->getanimindex(anim);
    }
    conoutf("requested playermodel out of range");
    return -1;
}
ICOMMAND(getanimation, "is", (int *mdl, char* anim), intret(getanimation(*mdl, anim)));
