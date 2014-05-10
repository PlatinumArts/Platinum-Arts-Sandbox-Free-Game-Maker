/*
 * clientmode.cpp
 *
 *  Created on: 18.12.2009
 *      Author: offtools
 */
#include "game.h"

namespace game
{
	void defaultclientmode::pickspawn(DynamicEntity *d)
	{
		switch(d->controltype)
		{
			case CONTROL_CHARACTER:
			{
				//check ActionLib empty
//                if(editmode)
//                {
//// TODO (offtools#1#): fix if author under floor
//					d->o = vec(player1->o);
//					droptofloor(d->o, 0, 0);
//                    vec sp = vec(0,0,0);
//                    vecfromyawpitch(game::player1->yaw, 0, 50, 0, sp);
//                    d->o.add(sp);
//                    d->o.z += d->eyeheight;
//                }
                break;
			}
			case CONTROL_PLAYER:
			case CONTROL_AI:
				findplayerspawn(d);
				break;
			case CONTROL_REMOTE:
			default:
				break;
		}
	}
}
