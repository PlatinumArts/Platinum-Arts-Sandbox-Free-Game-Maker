#include "game.h"

//rpgcamera by Hirato adapted to moviecube

namespace CutScene
{
	struct scene
	{
		const char *actions; //stuff to execute after initialisation
		const char *post; //stuff to execute after the cutscenes

		scene(const char *a, const char *p) : actions(newstring(a)), post(p ? newstring(p) : NULL) {}
		~scene()
		{
			delete[] actions;
			delete[] post;
		}
	};

	struct action
	{
		int startmillis;
		int duration;
		const char *successors;

		action(int d = 0, const char *s = NULL) : startmillis(lastmillis), duration(d), successors(s && *s ? newstring(s) : NULL) {}
		virtual ~action() {delete[] successors;}

		float multiplier(bool total = false)
		{
			if(!duration)
				return 1;

			if(total)
				return min<float>(1, (lastmillis - startmillis) / (float)duration);
			else
			{
				int elapsed = curtime;
				if(startmillis + duration - lastmillis < curtime)
					elapsed = startmillis + duration - lastmillis;
				return (float) elapsed / duration;

			}
		}
		virtual bool update()
		{
			if(startmillis + duration <= lastmillis)
			{
				if(successors)
					execute(successors);
				return false;
			}
			return true;
		}
		virtual void render(int w, int h) {}
		virtual void debug(int w, int h, int &hoffset, bool type = false)
		{
			if(type)
			{
				draw_text("Type: action/delay", 100, 0 + hoffset);
				hoffset += 64;
			}

			draw_textf("Duration: %i (%f%%)", 100, 0 + hoffset, duration, (lastmillis - startmillis) / (float)duration);
			draw_textf("Children: %p", 100, 64 + hoffset, successors);

			hoffset += 64 * 3;
		}
		virtual void getsignal(const char *signal) {}
	};

	vector<scene *> scenes;
	scene *curscene;
	vector <action *> pending;
	int suboffset = 0;
	vector <action *> subtitles; //subtitles are tracked independantly, and rendered in reverse, always on top
	physent camera;
	physent *attached = NULL;
	bool cutscene = false;
	bool cancelled = false;

	void cleanup(bool clear)
	{
		cutscene = false;
		curscene = NULL;
		pending.deletecontents();
		subtitles.deletecontents();

		if(clear)
			scenes.deletecontents();
	}

	ICOMMAND(r_new_cutscene, "ss", (const char *a, const char *p),
		if(!a[0])
		{
			conoutf(CON_ERROR, "ERROR: Can not create actionless cutscenes");
			return;
		}
		scenes.add(new scene(a, *p ? p : NULL));
	)

	ICOMMAND(r_start_cutscene, "i", (int *ind),
		if(!scenes.inrange(*ind))
		{
			conoutf(CON_ERROR, "ERROR: cannot start cutscene %i, out of range", *ind);
			return;
		}
		camera = *game::player1;
		pending.deletecontents();
		if(curscene && curscene->post)
			execute(curscene->post);
		curscene = scenes[*ind];
		execute(curscene->actions);
	)

	ICOMMAND(r_interrupt_cutscene, "", (),
		cancelled = curscene != NULL;
		intret(cancelled ? 1 : 0);
	)

	void update()
	{
		cutscene = curscene && (pending.length() || subtitles.length()) && !cancelled;
		attached = NULL;
		if(!cutscene)
		{
			if(curscene && curscene->post)
				execute(curscene->post);
			curscene = NULL;
			pending.deletecontents();
			subtitles.deletecontents();
			if(cancelled)
				cancelled = false;
		}
		else
		{
			loopv(pending)
			{
				if(!pending[i]->update())
				{
					delete pending[i];
					pending.remove(i);
					i--;
				}
			}
			loopv(subtitles)
			{
				if(!subtitles[i]->update())
				{
					delete subtitles[i];
					subtitles.remove(i);
					i--;
				}
			}
			if(!pending.length() && !subtitles.length())
				cutscene = false;
		}
	}

	void render(int w, int h)
	{
		loopv(pending)
			pending[i]->render(w, h);
		suboffset = 0;
		loopvrev(subtitles)
			subtitles[i]->render(w, h);

		glPushMatrix();
		glScalef(.3, .3, 1);

		int offset = 128;
		if(DEBUG_CAMERA)
		{
			loopv(pending)
				pending[i]->debug(w, h, offset, true);
			loopv(subtitles)
				subtitles[i]->debug(w, h, offset, true);
		}
		glPopMatrix();
	}

	void getsignal(const char *signal)
	{
		loopv(pending)
			pending[i]->getsignal(signal);
	}

	struct cond : action
	{
		const char *test;

		cond(const char *t, const char *s) : action(0, s), test(t && *t ? newstring(t) : NULL) {}
		~cond() { delete[] test;}

		bool update()
		{
			if(!test || execute(test))
			{
				if(successors) execute(successors);
				return false;
			}

			return true;
		}

		void debug(int w, int h, int &hoffset, bool type)
		{
			if(type)
			{
				draw_text("Type: Test", 100, 0 + hoffset);
				hoffset += 64;
			}

			draw_textf("Test: %p", 100, 0 + hoffset, test);
			draw_textf("Children: %p", 100, 64 + hoffset, successors);
			hoffset += 192;
		}
	};

	struct iterate : cond
	{
		int limit;
		int i;

		iterate(const char *t, int l, int d, const char *s) : cond(t, s), limit(l), i(0) { duration = d;}
		~iterate() {}

		bool update()
		{
			if(lastmillis - startmillis >= duration)
			{
				if(limit)
					i++;

				bool res = cond::update();

				if(res || (limit && i == limit))
					return false;

				startmillis += duration;
			}

			return true;
		}

		void debug(int w, int h, int &hoffset, bool type)
		{
			if(type)
			{
				draw_text("Type: Loop", 100, 0 + hoffset);
				hoffset += 64;
			}

			draw_textf("Test: %p", 100, 0 + hoffset);
			draw_textf("Loop: %i/%i", 100, 64 + hoffset, i, limit);
			hoffset += 128;

			action::debug(w, h, hoffset);
		}
	};

	struct signal : action
	{
		const char *name;
		bool received;

		signal(const char *n, const char *s) : action(0, s), name(n && *n ? newstring(n) : NULL), received(false) {}
		~signal() {delete[] name;}

		bool update()
		{
			if(!name || received)
			{
				if(successors) execute(successors);
				return false;
			}
			return true;
		}

		void getsignal(const char *signal)
		{
			if(!strcmp(signal, name))
				received = true;
		}

		void debug(int w, int h, int &hoffset, bool type)
		{
			if(type)
			{
				draw_text("Type: Signal", 100, 0 + hoffset);
				hoffset += 64;
			}

			draw_textf("Signal: %s", 100, 0 + hoffset, name);
			draw_textf("Children: %p", 100, 64 + hoffset, successors);
			hoffset += 192;
		}
	};

	struct subtitle : action
	{
		const char *text;
		vec color;

		subtitle(const char *t, float r, float g, float b, int d, const char *s) : action(d, s), text(t && *t ? newstring(t) : newstring("No Text")), color(vec(r, g, b)) {}
		~subtitle() {delete[] text;}

		void render(int w, int h)
		{
			float alpha = 1;
			if(duration > 500)
			{
				if(startmillis + 100 > lastmillis)
					alpha = 1 - (startmillis + 100 - lastmillis) / 100.0f;
				else if(startmillis - 100 + duration < lastmillis)
					alpha = (startmillis + duration - lastmillis) / 100.0f;
			}
			int th, tw;
			text_bounds(text, tw, th, w * 0.9);

			draw_text(text, w / 2 - tw / 2, h * 0.85 - th - suboffset, color.x * 255, color.y * 255, color.z * 255, alpha * 255, -1, w * 0.9);
			suboffset += th * alpha;
		}
		void debug(int w, int h, int &hoffset, bool type)
		{
			if(type)
			{
				draw_text("Type: subtitle", 100, 0 + hoffset);
				hoffset += 64;
			}

			draw_textf("Colour: (%f, %f, %f)", 100, 0 + hoffset, color.x, color.y, color.z);
			hoffset += 64;
			action::debug(w, h, hoffset);
		}
	};

	struct move : action
	{
		float dx, dy, dz;
		vec start;

		move(float x, float y, float z, int d, const char *s) : action(d, s), dx(x), dy(y), dz(z), start(camera.o) {}
		move(vec dest, int d, const char *s) : action(d, s), start(camera.o)
		{
			dest.sub(start);
			dx = dest.x;
			dy = dest.y;
			dz = dest.z;
		}
		~move() {}

		bool update()
		{
			camera.o = vec(dx, dy, dz).mul(multiplier(true)).add(start);

			return action::update();
		}
		void debug(int w, int h, int &hoffset, bool type)
		{
			if(type)
			{
				draw_text("Type: Move", 100, 0 + hoffset);
				hoffset += 64;
			}

			draw_textf("Start: (%f, %f, %f)", 100, 0 + hoffset, start.x, start.y, start.z);
			draw_textf("Delta: (%f, %f, %f)", 100, 64 + hoffset, dx, dy, dz);
			hoffset += 64 * 2;
			action::debug(w, h, hoffset);
		}
	};

	struct moveaccel : move
	{
		moveaccel(float x, float y, float z, int d, const char *s) : move(x, y, z, d, s) {}
		moveaccel(vec dest, int d, const char *s) : move(dest, d, s) {}
		~moveaccel() {}

		bool update()
		{
			if(!duration)
				return move::update();

			int elapsed = min(duration, lastmillis - startmillis);
			float mult = 0.5f + sinf((elapsed - duration/2.0f) * M_PI / duration) / 2.0f;

			camera.o = vec(dx, dy, dz).mul(mult).add(start);

			return action::update();
		}
		void debug(int w, int h, int &hoffset, bool type)
		{
			if(type)
			{
				draw_text("Type: Move Accel", 100, 0 + hoffset);
				hoffset += 64;
			}

			draw_textf("Progress: %f", 100, 0 + hoffset, 0.5f + sinf((min(duration, lastmillis - startmillis) - duration / 2.0f) * M_PI / duration));
			hoffset += 64;
			move::debug(w, h, hoffset, false);
		}
	};

	struct view : action
	{
		float yaw, pitch, roll;
		float iyaw, ipitch, iroll;

		view(float y, float p, float r, int d, const char *s) : action(d, s), yaw(y), pitch(p), roll(r), iyaw(camera.yaw), ipitch(camera.pitch), iroll(camera.roll)
		{
			if(duration && (yaw - iyaw == 180 || yaw - iyaw == -180))
			{
				conoutf("Warning: viewspecific/viewcamera was called from two points with a 180 degree delta. iyaw was adjusted slightly to avoid funkiness and possible division by 0");
				iyaw += 1;
			}
		}
		~view() {}

		bool update()
		{
			vec old = vec(iyaw * RAD, ipitch * RAD).mul(1-multiplier(true));
			vec next = vec(yaw * RAD, pitch * RAD).mul(multiplier(true)).add(old);

			vectoyawpitch(next, camera.yaw, camera.pitch);
			camera.roll = iroll * (1 - multiplier(true)) + roll * multiplier(true);

			return action::update();
		}
		void debug(int w, int h, int &hoffset, bool type)
		{
			if(type)
			{
				draw_text("Type: View", 100, 0 + hoffset);
				hoffset += 64;
			}

			draw_textf("Initial: %f %f %f", 100, 0 + hoffset, iyaw, ipitch, iroll);
			draw_textf("Post: %f %f %f", 100, 64 + hoffset, yaw, pitch, roll);
			hoffset += 64 * 2;
			action::debug(w, h, hoffset);
		}
	};

	struct viewaccel : view
	{
		viewaccel(float y, float p, float r, int d, const char *s) : view(y, p, r, d, s) {}
		~viewaccel() {}

		bool update()
		{
			if(!duration)
				return view::update();

			int elapsed = min(duration, lastmillis - startmillis);
			float mult = 0.5f + sinf((elapsed - duration/2.0f) * M_PI / duration) / 2.0f;

			vec old = vec(iyaw * RAD, ipitch * RAD).mul(1-mult);
			vec next = vec(yaw * RAD, pitch * RAD).mul(mult).add(old);

			vectoyawpitch(next, camera.yaw, camera.pitch);
			camera.roll = iroll * (1 - mult) + roll * mult;

			return action::update();
		}
		void debug(int w, int h, int &hoffset, bool type)
		{
			if(type)
			{
				draw_text("Type: View Accel", 100, 0 + hoffset);
				hoffset += 64;
			}

			draw_textf("Progress: %f", 100, 0 + hoffset, 0.5f + sinf((min(duration, lastmillis - startmillis) - duration / 2.0f) * M_PI / duration));
			hoffset += 64;
			view::debug(w, h, hoffset, false);
		}
	};

	struct viewspin : action
	{
		float yaw, pitch, roll;

		viewspin(float y, float p, float r, int d, const char *s) : action(d, s), yaw(y), pitch(p), roll(r) {}
		~viewspin() {}

		bool update()
		{
			float mult = multiplier();
			camera.yaw += yaw * mult;
			camera.pitch += pitch * mult;
			camera.roll += roll * mult;

			return action::update();
		}
		void debug(int w, int h, int &hoffset, bool type)
		{
			if(type)
			{
				draw_text("Type: View Spin", 100, 0 + hoffset);
				hoffset += 64;
			}

			draw_textf("Delta: %f %f %f", 100, 0 + hoffset, yaw, pitch, roll);
			hoffset += 64;
			action::debug(w, h, hoffset);
		}
	};

	struct sound : action
	{
		int ind;

		sound(int i, int d, const char *s) : action(d, s), ind(i)
		{
			playsound(ind, NULL, NULL, 0, 0, -1, 0, d);
		}
		~sound() {}

		void debug(int w, int h, int &hoffset, bool type)
		{
			if(type)
			{
				draw_text("Type: Sound", 100, 0 + hoffset);
				hoffset += 64;
			}

			draw_textf("Index: %i", 100, 0 + hoffset, ind);
			hoffset += 64;
			action::debug(w, h, hoffset);
		}
	};

	struct soundfile : action
	{
		const char *path;

		soundfile(const char *p, int d, const char *s) : action(d, s), path(newstring(p))
		{
			playsoundname(path, NULL, 0, 0, 0, -1, 0, d);
		}
		~soundfile() {delete[] path;}

		void debug(int w, int h, int &hoffset, bool type)
		{
			if(type)
			{
				draw_text("Type: Sound File", 100, 0 + hoffset);
				hoffset += 64;
			}

			draw_textf("Path: %s", 100, 0 + hoffset, path);
			hoffset += 64;
			action::debug(w, h, hoffset);
		}
	};

	struct overlay : action
	{
		Texture *img;
		vec4 color;
		int fade;

		overlay(Texture *i, float r, float g, float b, float a, int f, int d, const char *s) : action(d, s), img(i), color(vec4(r, g, b, a)), fade(f) {}
		~overlay() {}

		void render(int w, int h)
		{
			float alpha = 1;
			if(fade && duration >= fade * 2)
			{
				if(startmillis + fade > lastmillis)
					alpha = 1 - (startmillis + 150 - lastmillis) / (float) fade;
				else if(startmillis -fade + duration < lastmillis)
					alpha = (startmillis + duration - lastmillis) / (float) fade;
			}

			glColor4f(color.x, color.y, color.z, color.w * alpha);
			settexture(img);

			glBegin(GL_TRIANGLE_STRIP);

			glTexCoord2f(0, 0); glVertex2i(0, 0);
			glTexCoord2f(1, 0); glVertex2i(w, 0);
			glTexCoord2f(0, 1); glVertex2i(0, h);
			glTexCoord2f(1, 1); glVertex2i(w, h);

			glEnd();

			glColor3f(1, 1, 1);
		}
		void debug(int w, int h, int &hoffset, bool type)
		{
			if(type)
			{
				draw_text("Type: Overlay", 100, 0 + hoffset);
				hoffset += 64;
			}

			draw_textf("Color: (%f, %f, %f, %f)", 100, 0 + hoffset, color.x, color.y, color.z, color.w);
			draw_textf("Fade: %i", 100, 64 + hoffset, fade);
			hoffset += 64 * 2;
			action::debug(w, h, hoffset);
		}
	};

	struct solid : action
	{
		bool modulate;
		vec4 color;
		int fade;

		solid(bool m, float r, float g, float b, float a, int f, int d, const char *s) : action(d, s), modulate(m), color(vec4(r, g, b, a)), fade(f) {}

		void render(int w, int h)
		{
			float alpha = 1;
			if(fade && duration >= fade * 2)
			{
				if(startmillis + fade > lastmillis)
					alpha = 1 - (startmillis + fade - lastmillis) / (float) fade;
				else if(startmillis -fade + duration < lastmillis)
					alpha = (startmillis + duration - lastmillis) / (float) fade;
			}

			if(modulate)
			{
				glBlendFunc(GL_ZERO, GL_SRC_COLOR);
				vec col = vec(color).mul(alpha).add(vec(1, 1, 1).mul(1-alpha));
				glColor3fv(col.v);
			}
			else
				glColor4f(color.x, color.y, color.z, color.w * alpha);

			glDisable(GL_TEXTURE_2D);
			setnotextureshader();
			glBegin(GL_TRIANGLE_STRIP);

			glVertex2i(0, 0);
			glVertex2i(w, 0);
			glVertex2i(0, h);
			glVertex2i(w, h);

			glEnd();
			setdefaultshader();
			glEnable(GL_TEXTURE_2D);

			if(modulate)
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor3f(1, 1, 1);
		}

		void debug(int w, int h, int &hoffset, bool type)
		{
			if(type)
			{
				draw_text("Type: Solid", 100, 0 + hoffset);
				hoffset += 64;
			}

			draw_textf("Modulate: %s", 100, 0 + hoffset, modulate ? "Yes": "No");
			draw_textf("Color: (%f, %f, %f, %f)", 100, 64 + hoffset, color.x, color.y, color.z, color.w);
			draw_textf("Fade: %i", 100, 128 + hoffset, fade);
			hoffset += 64 * 3;
			action::debug(w, h, hoffset);
		}
	};

	struct viewport : action
	{
		physent *ent;
		float height;
		float tail;

		viewport(physent *e, float h, float t, int d, const char *s) : action(d, s), ent(e), height(h), tail(t) {}
		~viewport() {}

		bool update()
		{
			camera = *ent;
			camera.o.z += height;
			camera.o.add(vec(camera.yaw * RAD, camera.pitch * RAD).mul(tail));

			vec delta = vec(camera.o).sub(ent->o);
			if(delta.z > ent->aboveeye || delta.z + ent->eyeheight < 0 || (fabs(delta.x) < ent->radius && fabs(delta.y) < ent->radius))
				attached = ent;

			return action::update();
		}
		void debug(int w, int h, int &hoffset, bool type)
		{
			if(type)
			{
				draw_text("Type: Viewport", 100, 0 + hoffset);
				hoffset += 64;
			}

			draw_textf("Entity: %p", 100, 0 + hoffset, ent);
			draw_textf("Height + tail: %f %f", 100, 64 + hoffset, height, tail);
			draw_textf("View: %f %f %f", 100, 128 + hoffset, ent->yaw, ent->pitch, ent->roll);
			draw_textf("Pos: (cam) %f %f %f (ent) %f %f %f", 100, 192 + hoffset, camera.o.x, camera.o.y, camera.o.z, ent->o.x, ent->o.y, ent->o.z);

			hoffset += 256;
			action::debug(w, h, hoffset);
		}
	};

	struct focus : action
	{
		physent *ent;
		float interp;
		float lead;

		focus(physent *e, float i, float l, int d, const char *s) : action(d, s), ent(e), interp(i), lead(l) {}
		~focus() {}

		bool update()
		{
			int mult = interp ? min(1.0f, interp * curtime / 1000.0f) : 1;
			vec view = vec(camera.yaw * RAD, camera.pitch * RAD).mul(1 - mult).add(vec(camera.o).sub(ent->o).sub(vec(ent->vel).mul(lead)).normalize().mul(mult));

			vectoyawpitch(view, camera.yaw, camera.pitch);

			return action::update();
		}
		void debug(int w, int h, int &hoffset, bool type)
		{
			if(type)
			{
				draw_text("Type: Focus", 100, 0 + hoffset);
				hoffset += 64;
			}

			vec ideal = vec(camera.o).sub(ent->o).sub(vec(ent->vel).mul(lead));
			float iyaw, ipitch;
			vectoyawpitch(ideal, iyaw, ipitch);

			draw_textf("Interp + Lead: %f %f", 100, 0 + hoffset, interp, lead);
			draw_textf("Current: %f %f", 100, 64 + hoffset, camera.yaw, camera.pitch);
			draw_textf("Ideal: %f %f", 100, 128 + hoffset, iyaw, ipitch);
			hoffset += 192;

			return action::debug(w, h, hoffset);
		}
	};

	#define GETCAM \
		extentity *cam = NULL; \
		loopv(MapEntities::ents) \
		{ \
			if(MapEntities::ents[i]->type == CAMERA && MapEntities::ents[i]->attr[0] == *tag) \
				cam = MapEntities::ents[i]; \
		}

	ICOMMAND(r_cutscene_delay, "is", (int *d, const char *s),
		if(curscene)
			pending.add(new action(*d, s));
	)
	ICOMMAND(r_cutscene_cond, "ss", (const char *c, const char *s),
		if(curscene)
			pending.add(new cond(c, s));
	)
	ICOMMAND(r_cutscene_loop, "siis", (const char *t, int *l, int *d, const char *s),
		if(curscene)
			pending.add(new iterate(t, *l, *d, s));
	)
	ICOMMAND(r_cutscene_signal, "ss", (const char *n, const char *s),
		if(curscene)
			pending.add(new signal(n, s));
	)
	ICOMMAND(r_cutscene_sendsignal, "s", (const char *n),
		if(curscene) getsignal(n);
	)

	ICOMMAND(r_cutscene_subtitle, "sfffis", (const char *text, float *r, float *g, float *b, int *d, const char *s),
		if(curscene)
			subtitles.add(new subtitle(text, *r, *g, *b, *d, s));
	)
	ICOMMAND(r_cutscene_movecamera, "iis", (int *tag, int *d, const char *s),
		if(curscene)
		{
			GETCAM
			if(cam)
				pending.add(new move(cam->o, *d, s));
			else
			{
				conoutf("warning: unable to find a camera entity with tag %i", *tag);
				pending.add(new action(*d, s));
			}
		}
	)
	ICOMMAND(r_cutscene_movespecific, "fffis", (float *x, float *y, float *z, int *d, const char *s),
		if(curscene)
			pending.add(new move(*x, *y, *z, *d, s));
	)
	ICOMMAND(r_cutscene_moveaccelcamera, "iis", (int *tag, int *d, const char *s),
		if(curscene)
		{
			GETCAM
			if(cam)
				pending.add(new moveaccel(cam->o, *d, s));
			else
			{
				conoutf("warning: unable to find a camera entity with tag %i", *tag);
				pending.add(new action(*d, s));
			}
		}
	)
	ICOMMAND(r_cutscene_moveaccelspecific, "fffis", (float *x, float *y, float *z, int *d, const char *s),
		if(curscene)
			pending.add(new moveaccel(*x, *y, *z, *d, s));
	)
	ICOMMAND(r_cutscene_viewcamera, "iis", (int *tag, int *d, const char *s),
		if(curscene)
		{
			GETCAM
			if(cam)
				pending.add(new view(cam->attr[1], cam->attr[2], cam->attr[3], *d, s));
			else
			{
				conoutf("warning: unable to find a camera entity with tag %i", *tag);
				pending.add(new action(*d, s));
			}
		}
	)
	ICOMMAND(r_cutscene_viewspecific, "fffis", (float *y, float *p, float *r, int *d, const char *s),
		if(curscene)
			pending.add(new view(*y, *p, *r, *d, s));
	)
	ICOMMAND(r_cutscene_viewaccelcamera, "iis", (int *tag, int *d, const char *s),
		if(curscene)
		{
			GETCAM
			if(cam)
				pending.add(new viewaccel(cam->attr[1], cam->attr[2], cam->attr[3], *d, s));
			else
			{
				conoutf("warning: unable to find a camera entity with tag %i", *tag);
				pending.add(new action(*d, s));
			}
		}
	)
	ICOMMAND(r_cutscene_viewaccelspecific, "fffis", (float *y, float *p, float *r, int *d, const char *s),
		if(curscene)
			pending.add(new viewaccel(*y, *p, *r, *d, s));
	)
	ICOMMAND(r_cutscene_viewspin, "fffis", (float *y, float *p, float *r, int *d, const char *s),
		if(curscene)
			pending.add(new viewspin(*y, *p, *r, *d, s));
	)
	ICOMMAND(r_cutscene_sound, "iis", (int *ind, int *d, const char *s),
		if(curscene)
			pending.add(new sound(*ind, *d, s));
	)
	ICOMMAND(r_cutscene_soundfile, "sis", (const char *p, int *d, const char *s),
		 if(curscene)
			 pending.add(new soundfile(p, *d, s));
		 )
	ICOMMAND(r_cutscene_overlay, "sffffiis", (const char *p, float *r, float *g, float *b, float *a, int *f, int *d, const char *s),
		if(curscene)
			pending.add(new overlay(textureload(p, 3), *r, *g, *b, *a, *f, *d, s));
	)
	ICOMMAND(r_cutscene_solid, "iffffiis", (int *m, float *r, float *g, float *b, float *a, int *f, int *d, const char *s),
		if(curscene)
			pending.add(new solid(*m != 0, *r, *g, *b, *a, *f, *d, s));
	)
	ICOMMAND(r_cutscene_viewport, "iffis", (int *e, float *h, float *t, int *d, const char *s),
		if(curscene && game::iterdynents(*e))
			pending.add(new viewport(game::iterdynents(*e), *h, *t, *d, s));
	)
	ICOMMAND(r_cutscene_focus, "iffis", (int *e, float *i, float *l, int *d, const char *s),
		if(curscene && game::iterdynents(*e))
			pending.add(new focus(game::iterdynents(*e), *i, *l, *d, s));
	)
//}

//namespace rpgio
//{
	/**
		NOTE when loading, do not restore the cutscene, just pretend it was skipped
	*/
	extern const char *readstring(stream *f);
	extern void writestring(stream *f, const char *str);

	int idx = -1;

	const char *readstring(stream *f)
	{
		int len = f->getlil<int>();
		char *s = NULL;
		if(len)
		{
			s = new char[len + 1];
			f->read(s, len + 1);
			conoutf(CON_DEBUG, "DEBUG: Read \"%s\" from file (%i)", s, len);
		}

		return s;
	}

	void writestring(stream *f, const char *s)
	{
		int len = s ? strlen(s) : 0;
		f->putlil(len);

		if(!len) return;

		len++;
		f->write(s, len);
		conoutf(CON_DEBUG, "DEBUG: Wrote \"%s\" to file (%i)", s, len);
	}

	void readcutscene(stream *f)
	{
		scenes.add(new scene(readstring(f), readstring(f)));
		if(scenes.inrange(idx))
		{
			curscene = scenes[idx];
			idx = -1;
		}
	}

	void readcamera(stream *f)
	{
		idx = f->getlil<int>();
	}

	void writecutscenes(stream *f, int token)
	{
		loopv(scenes)
		{
			f->putchar(token);
			writestring(f, scenes[i]->actions);
			writestring(f, scenes[i]->post);
		}
	}

	void writecamera(stream *f)
	{
		f->putlil(scenes.find(curscene));
	}
}

namespace game
{
	bool recomputecamera(physent *&camera1, physent &tempcamera, bool &detachedcamera, float &thirdpersondistance)
	{
		if(CutScene::cutscene)
		{
			camera1 = &tempcamera;
			tempcamera = CutScene::camera;
			detachedcamera = true;

			return true;
		}
		return false;
	}
}
