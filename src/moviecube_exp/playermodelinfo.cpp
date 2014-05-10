#include "playermodelinfo.h"

namespace PlayerModel
{
    vector<PlayerModelInfo*> playermodels;

    PlayerModelInfo::PlayerModelInfo(const char* _name) : name(newstring(_name)), ragdoll(true), selectable(true) {}

    void PlayerModelInfo::registeranimation(char *anim, int num)
    {
        loopv(animinfo) if( strcmp(animinfo[i]->descr, anim ) == 0) { return; }
        animinfo.add(new animationinfo(anim, num));
        conoutf("PlayerModelInfo::registeranimation: %s", animinfo.last()->descr);
    }

    void PlayerModelInfo::registeritem( int part, char* model, int rule)
    {
        iteminfo.add(new attachmentinfo(part, model, rule));
    }

    bool PlayerModelInfo::hasanimation(int anim)
    {
        loopv(animinfo)
        {
            if(animinfo[i]->index == anim) return true;
        }
        return false;
    }

    char* PlayerModelInfo::getname()
    {
        return name;
    }

    vector<PlayerModelInfo*> PlayerModels()
    {
        return playermodels;
    }

    PlayerModelInfo& getPlayerModelInfo(int i)
    {
        if(playermodels.inrange(i)) return *playermodels[i];
        else return *playermodels[0];
    }

	void RegisterAnimation(char *loadname, char* anim, int num)
	{
		loopv(playermodels)
		{
			if(strcmp(loadname, playermodels[i]->name) == 0)
			{
				playermodels[i]->registeranimation(anim, num);
				return;
			}
		}
	}

    bool HasAnimation(PlayerModelInfo& pm, int anim)
    {
        return pm.hasanimation(anim);
    }

    const char* GetName(PlayerModelInfo& pm)
    {
        return pm.getname();
    }

    int RegisterModel(const char *s)
    {
		loopv(playermodels)
		{
			if(strcmp(playermodels[i]->name, s) == 0)
			{
                conoutf("DEBUG: PlayerModel::RegisterModel - model: %s already registered", playermodels[i]->name);
				return -1;
			}
		}
		playermodels.add(new PlayerModelInfo(s));
		preloadmodel(s);
		return playermodels.length() - 1;
    }
	ICOMMAND(registercharactermodel, "s", (char *s), intret(RegisterModel(s)));

	void RegisterItem(int playermodel, int part, char* model, int rule)
	{
        if(loadmodel(model) && playermodels.inrange(playermodel))
        {
            PlayerModelInfo* info = playermodels[playermodel];
            info->registeritem(part, model, rule);
            conoutf("DEBUG: PlayerModel::RegisterItem - register item model: %s", model);
        }
        else
        {
            conoutf("PlayerModel::RegisterItem - could not register model: %s", model);
            return;
        }
	}
    ICOMMAND(registercharacteritem, "iisi", (int *i, int *p, char *s, int *r), RegisterItem(*i, *p, s, *r));

    void BuildModelAttach(PlayerModelInfo& pm, int itemnum, modelattach& a)
    {
        if(!pm.iteminfo.inrange(itemnum)) return;

        int animrule = pm.iteminfo[itemnum]->rule;
        int bpart = pm.iteminfo[itemnum]->bodypart;
        const char* tag = pm.iteminfo[itemnum]->gettag(bpart);
        int animtime;

        switch (pm.iteminfo[itemnum]->rule)
        {
            case ATTACH_ANIM_IDLE:
            {
                animrule = ANIM_IDLE|ANIM_LOOP; animtime = 0; break;
            }
            case ATTACH_ANIM_FULL:
            {
                animrule = -1; animtime = 0; break;
            }
            case ATTACH_ANIM_NONE:
            default:
            {
                animrule = -1; animtime = 0; break;
            }
        }
        a = modelattach(tag, pm.iteminfo[itemnum]->model, animrule, animtime);
    }

    void listanimations(int idx)
    {
        vector<char> buf;
        string lst;
        if(playermodels.inrange(idx))
        {
            PlayerModelInfo* info = playermodels[idx];
            loopv(info->animinfo)
            {
                formatstring(lst)("%d:%s", info->animinfo[i]->index, info->animinfo[i]->descr);
                buf.put(lst, strlen(lst));
                if(i < (info->animinfo.length()-1)) buf.add(' ');
            }
            buf.add('\0');
            result(buf.getbuf());
        }
    }
    ICOMMAND(listmodelanimations, "i", (int *i), listanimations(*i));

    void listitems(int idx)
    {
        vector<char> buf;
        string lst;
        if(playermodels.inrange(idx))
        {
            PlayerModelInfo* info = playermodels[idx];
            loopv(info->iteminfo)
            {
                int bpart = info->iteminfo[i]->bodypart;
                formatstring(lst)("%s:%s", info->iteminfo[i]->getbodypart(bpart), info->iteminfo[i]->model);
                buf.put(lst, strlen(lst));
                if(i < (info->animinfo.length()-1)) buf.add(' ');
            }
            buf.add('\0');
            result(buf.getbuf());
        }
    }
    ICOMMAND(listmodelitems, "i", (int *i), listitems(*i));

    void listplayermodels()
    {
        vector<char> buf;
        string lst;
        loopv(playermodels)
        {
            formatstring(lst)("%s", GetName( getPlayerModelInfo(i)) );
            buf.put(lst, strlen(lst));
            if(i < (playermodels.length()-1)) buf.add(' ');
        }
        buf.add('\0');
        result(buf.getbuf());
    }
    ICOMMAND(listplayermodels, "i", (), listplayermodels());
}
