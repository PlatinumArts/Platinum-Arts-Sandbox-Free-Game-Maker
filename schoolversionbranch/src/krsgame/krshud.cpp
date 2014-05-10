#include "krsgame.h"
#include "engine.h"

namespace game
{
	VARP(metertype, 0, 0, 1);

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

		if(thirdperson)
		{
			/*apply meter based on meter type selection*/
			if(metertype == 0)
				settexture("data/krs/hud/meter_km.png", 3);
			else if(metertype == 1)
				settexture("data/krs/hud/meter_mph.png", 3);

			quad(right-512, bottom-512, 512, 512);


			/*apply needel to meter*/
			settexture("data/krs/hud/needle.png", 3);

			const static int coords[4][2] = {{0, 1}, {1, 1}, {0, 0}, {1, 0}};
			vec points[4] = { vec(-32, -40, 0), vec(32, -40, 0), vec(-32, 216, 0), vec(32, 216, 0)};
			float angle = (player1->vel.magnitude() * 170.0f / 250.0f ) + 60;

			loopi(4) points[i].rotate_around_z(angle * RAD);

			glBegin(GL_TRIANGLE_STRIP);
			loopi(4)
			{
				glTexCoord2f(coords[i][0], coords[i][1]); glVertex2i(right - 256 + points[i].x, bottom - 256 + points[i].y);
			}
			glEnd();
		}

		glPopMatrix();
	}

	const char *defaultcrosshair(int index)
	{
		return editmode ? "packages/crosshairs/edit" : "data/items";
	}

	int selectcrosshair(float &r, float &g, float &b)
	{
		r = b = 0.5;
		g = 1.0f;
		return editmode;
	}
}
