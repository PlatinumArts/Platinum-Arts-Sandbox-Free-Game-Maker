#include "krsgame.h"

using namespace game;

namespace krsscript
{
	ICOMMAND(registervehicle, "siiiii",
	(
		const char *mdl,
		int *flags,
		int *jumpvel,
		int *maxspeed,
		int *traction,
		int *price
	),{
		vehicle *vehiclelookup = new vehicle;

		vehiclelookup->mdl = newstring(mdl);
		vehiclelookup->flags = *flags;
		vehiclelookup->jumpvel = *jumpvel;
		vehiclelookup->maxspeed = *maxspeed;
		vehiclelookup->traction = *traction;
		vehiclelookup->price = *price;

		vehicles.add(vehiclelookup);
		player1->hasvehicle.add(false);
	});

	ICOMMAND(registerbot, "s", (const char *name),
	{
		game::botnames.add(newstring(name));

		if(debug)
			conoutf("DEBUG: Registered Bot Name: %s", name);
	});
	
	ICOMMAND(buyvehicle, "i", (int *index),
	{
		vehicle *vehiclelookup = vehicles[*index];
		
		if(vehiclelookup && !player1->hasvehicle[*index])
		{
			if(player1->money >= vehiclelookup->price)
			{
				player1->money -= vehiclelookup->price;
				player1->hasvehicle[*index] = true;
			}
		}
	});
	
	ICOMMAND(sellvehicle, "i", (int *index),
	{
		vehicle *vehiclelookup = vehicles[*index];
		
		if(vehiclelookup && player1->hasvehicle[*index])
		{
			player1->money += vehiclelookup->price;
			player1->hasvehicle[*index] = false;
			player1->isinvehicle = false;
		}
	});
	
	ICOMMAND(ineedacar, "", (),
	{
		bool deserves = true;
		vehicle *vehiclelookup = vehicles[0];
		
		loopv(player1->hasvehicle)
		{
			if(player1->hasvehicle[i]) {
				deserves = false;
			}
		}
		
		if(deserves)
		{
			player1->hasvehicle[0] = true;
			player1->isinvehicle = true;
		}
	});		
}
