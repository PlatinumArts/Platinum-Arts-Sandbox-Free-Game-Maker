#include "pzlgame.h"

namespace game
{
	const char *playermodels(int mdl)
	{
		const char *mdls[] =
		{
			"rc", "ogre"
		};

		return mdls[mdl];
	};

	void renderplayer(pzlchar *d)
	{
		renderclient(d, playermodels(d->playerindex), NULL, ANIM_HOLD1|ANIM_LOOP, ANIM_ATTACK1, 0, 0, 0, 1, false);
	}

	void rendergame(bool mainpass)
	{
		startmodelbatches();

		if(isthirdperson())
			renderplayer(game::player1);

		entities::renderentities();

		endmodelbatches();

		if(mainpass){}
	}
}
