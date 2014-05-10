#include "game.h"
#include "extentity.h"

namespace game
{
	VARP(showlighting, 0, 1, 1);
	VARP(showdynlights, 0, 1, 1);

	bool dynlightsactive[32767];

	void initdynlights()
	{
		for(int i = 0; i != 32767; i++)
		{
			dynlightsactive[i] = true;
		}
	}

	void setdynlightactivity(int *who, int *active)
	{
	   if (*who > 32767 || *who <= 0) return;
	   else if (!*active) dynlightsactive[*who] = false;
	   else dynlightsactive[*who] = true;
	}
	COMMAND(setdynlightactivity, "ii");

	#define renderfocus(i,f) { extentity &e = *entities::getents()[i]; f; }
	void renderentlight(extentity &e)
	{
		vec color(e.attr[1], e.attr[2], e.attr[3]);
		color.div(255.f);
		adddynlight(vec(e.o), float(e.attr[0] != 0 ? e.attr[0] > 0 ? e.attr[0] : 0 : getworldsize() ), color);
	}

	void adddynlights()
	{
		vector<extentity*> ents = entities::getents();
		loopv(ents)
		{
			if((ents[i]->attr[4] > -1 && ents[i]->type == DYNLIGHT && dynlightsactive[ents[i]->attr[4]] && (!editmode || showdynlights)) //dynlights
				|| ((entgroup.find(i) >= 0 || enthover == i) &&    (ents[i]->type == LIGHT) && (editmode && showlighting))) //normal lights
			{
				renderfocus(i, renderentlight(e));
			}
		}
	}
}
