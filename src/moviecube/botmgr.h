// server-side bot manager
//changelog:
//  20091219
//  *removed test on privilege in reqadd
//  20091220
//  *set ownership always to clients who requested the bot (TODO: make this more flexible for use in addition with ai)

//TODO: remove all game type dependent stuff and put it into servmodes

namespace botmgr
{
    bool dorefresh = false;
    VARN(serverbotlimit, botlimit, 0, 8, MAXBOTS);
    VARN(serverbotbalance, botbalance, 0, 1, 1);

    static inline bool validbotclient(clientinfo *ci)
    {
        return ci->clientnum >= 0 && ci->state.clienttype == CLIENT_PLAYER && (ci->state.state!=CS_SPECTATOR || ci->local || ci->privilege);
    }

	clientinfo *findbotclient(clientinfo *exclude = NULL)
	{
        clientinfo *least = NULL;
		loopv(clients)
		{
			clientinfo *ci = clients[i];
			if(!validbotclient(ci) || ci==exclude) continue;
            if(!least || ci->bots.length() < least->bots.length()) least = ci;
		}
        return least;
	}

	bool addbot(clientinfo* rc, int limit)
	{
	    //TODO: clean up function
		int numai = 0, cn = -1, maxai = limit >= 0 ? min(limit, MAXBOTS) : MAXBOTS;
		loopv(bots) //count number of initialized bot
        {
            clientinfo *ci = bots[i];
            if(!ci || ci->ownernum < 0) { if(cn < 0) cn = i; continue; }
			numai++;
		}
		if(numai >= maxai) return false; //test number of bots against maxbots, exit if limit reached
        if(bots.inrange(cn))
        {
            clientinfo *ci = bots[cn];
            if(ci)
            { // reuse a slot that was going to removed
            	//clientinfo *owner = findbotclient(); //owner should be client who requested bot
                //ci->ownernum = owner ? owner->clientnum : -1;
                ci->ownernum = rc->clientnum;
                ci->botreinit = 2;
                dorefresh = true;
                return true;
            }
        }
        else { cn = bots.length(); bots.add(NULL); }
        if(!bots[cn]) { bots[cn] = new clientinfo; }
        clientinfo *ci = bots[cn];
		ci->clientnum = MAXCLIENTS + cn;
		ci->state.clienttype = CLIENT_BOT;
        //clientinfo *owner = findbotclient();
		//ci->ownernum = owner ? owner->clientnum : -1;
		ci->ownernum = rc->clientnum;
        if(rc) rc->bots.add(ci);
        else fatal("non exisitend player requested bot");
	    clients.add(ci);
		ci->state.lasttimeplayed = lastmillis;
		ci->state.state = CS_ALIVE; //changed by Protagoras
        ci->playermodel = 0;
		ci->botreinit = 2;
		ci->connected = true;
        dorefresh = true;
		return true;
	}

	void deletebot(clientinfo *ci)
	{
        int cn = ci->clientnum - MAXCLIENTS;
        if(!bots.inrange(cn)) return;
        if(smode) smode->leavegame(ci, true);
        sendf(-1, 1, "ri2", SV_CDIS, ci->clientnum);
        clientinfo *owner = (clientinfo *)getclientinfo(ci->ownernum);
        if(owner) owner->bots.removeobj(ci);
        clients.removeobj(ci);
        DELETEP(bots[cn]);
		dorefresh = true;
	}

	bool deletebot()
	{
        loopvrev(bots) if(bots[i] && bots[i]->ownernum >= 0)
        {
			deletebot(bots[i]);
			return true;
		}
		return false;
	}

	void reinitbot(clientinfo *ci)
	{
		if(ci->ownernum < 0) deletebot(ci);
		else if(ci->botreinit >= 1)
		{
//            defformatstring(dbg)("DEBUG: botmgr::reinitbot send SV_INITBOT cl: %d on: %d, clienttype: %d, ", ci->clientnum, ci->ownernum, ci->state.clienttype);
//            sendf(-1, 1, "ris", SV_SERVMSG, dbg);

			sendf(-1, 1, "ri5", SV_INITBOT, ci->clientnum, ci->ownernum, ci->state.clienttype, ci->playermodel);
			if(ci->botreinit == 2)
            {
                ci->reassign();
                if(ci->state.state==CS_ALIVE)
                {
//                    defformatstring(dbg)("DEBUG: botmgr::reinitbot sendresume cl: %d on: %d, clienttype: %d", ci->clientnum, ci->ownernum, ci->state.clienttype);
//                    sendf(-1, 1, "ris", SV_SERVMSG, dbg);
                    sendspawn(ci);
                }
                else
                {
//                    defformatstring(dbg)("DEBUG: botmgr::reinitbot sendresume cl: %d on: %d, clienttype: %d", ci->clientnum, ci->ownernum, ci->state.clienttype);
//                    sendf(-1, 1, "ris", SV_SERVMSG, dbg);
                    sendresume(ci);
                }
            }
			ci->botreinit = 0;
		}
	}

	void shiftbot(clientinfo *ci, clientinfo *owner)
	{
        clientinfo *prevowner = (clientinfo *)getclientinfo(ci->ownernum);
        if(prevowner) prevowner->bots.removeobj(ci);
		if(!owner) { ci->botreinit = 0; ci->ownernum = -1; }
		else { ci->botreinit = 2; ci->ownernum = owner->clientnum; owner->bots.add(ci); }
        dorefresh = true;
	}

	void removebot(clientinfo *ci)
	{ // either schedules a removal, or someone else to assign to
		loopvrev(ci->bots) shiftbot(ci->bots[i], findbotclient(ci));
	}

	bool reassignbot()
	{
        clientinfo *hi = NULL, *lo = NULL;
		loopv(clients)
		{
			clientinfo *ci = clients[i];
			if(!validbotclient(ci)) continue;
            if(!lo || ci->bots.length() < lo->bots.length()) lo = ci;
            if(!hi || ci->bots.length() > hi->bots.length()) hi = ci;
		}
		if(hi && lo && hi->bots.length() - lo->bots.length() > 1)
		{
			loopvrev(hi->bots)
			{
				shiftbot(hi->bots[i], lo);
				return true;
			}
		}
		return false;
	}


	void checksetup()
	{
//	    if(m_teammode && botbalance) balanceteams();
		loopvrev(bots) if(bots[i]) reinitbot(bots[i]);
	}

	void clearbot()
	{ // clear and remove all ai immediately
        loopvrev(bots) if(bots[i]) deletebot(bots[i]);
	}

	void checkbot()
	{
        if(!dorefresh) return;
        dorefresh = false;
        if(m_botmode && numclients(-1, false, true))
		{
			checksetup();
			while(reassignbot());
		}
		else clearbot();
	}

	void reqadd(clientinfo *ci)
	{
//        if(!ci->local && !ci->privilege) {
//        	return;
//        }
        if(!addbot(ci, !ci->local && ci->privilege < PRIV_ADMIN ? botlimit : -1)) sendf(ci->clientnum, 1, "ris", SV_SERVMSG, "failed to create or assign bot");
	}

	void reqdel(clientinfo *ci)
	{
        if(!ci->local && !ci->privilege) return;
        if(!deletebot()) sendf(ci->clientnum, 1, "ris", SV_SERVMSG, "failed to remove any bots");
	}

    void setbotlimit(clientinfo *ci, int limit)
    {
        if(ci && !ci->local && ci->privilege < PRIV_ADMIN) return;
        botlimit = clamp(limit, 0, MAXBOTS);
        dorefresh = true;
        defformatstring(msg)("bot limit is now %d", botlimit);
        sendservmsg(msg);
    }

    void setbotbalance(clientinfo *ci, bool balance)
    {
//        if(ci && !ci->local && !ci->privilege) return;
//        botbalance = balance ? 1 : 0;
//        dorefresh = true;
//        defformatstring(msg)("bot team balancing is now %s", botbalance ? "enabled" : "disabled");
//        sendservmsg(msg);
    }

    void changemap()
    {
        dorefresh = true;
        loopv(clients) if(clients[i]->local || clients[i]->privilege) return;
//        if(!botbalance) setbotbalance(NULL, true);
    }

    void addclient(clientinfo *ci)
    {
        if(ci->state.clienttype == CLIENT_PLAYER) dorefresh = true;
    }

    void changeteam(clientinfo *ci)
    {
        if(ci->state.clienttype == CLIENT_PLAYER) dorefresh = true;
    }
}
