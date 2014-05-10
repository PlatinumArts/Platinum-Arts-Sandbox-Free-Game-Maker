#include "pzlgame.h"
#include "engine.h"

namespace game
{
	void quad(int x, int y, int xs, int ys)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0, 0); glVertex2i(x,    y);
		glTexCoord2f(1, 0); glVertex2i(x+xs, y);
		glTexCoord2f(0, 1); glVertex2i(x,    y+ys);
		glTexCoord2f(1, 1); glVertex2i(x+xs, y+ys);
		glEnd();
	}

	void gameplayhud(int w, int h)
	{
		float scale = min(w / 1600.0f, h / 1200.0f);
		float right = w / scale, bottom = h / scale;

		glPushMatrix();

		glScalef(scale, scale, 1);
		
		//draw
		
		glPopMatrix();
	}

	const char *defaultcrosshair(int index)
	{
		return editmode ? "data/edit" : "data/items";
	}

	int selectcrosshair(float &r, float &g, float &b)
	{
		r = b = 0.5;
		g = 1.0f;
		return editmode;
	}
}
