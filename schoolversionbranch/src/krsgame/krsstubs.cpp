#include "krsgame.h"

namespace game
{
	int clipconsole(int w, int h){ return 0;}

	float abovegameplayhud(int w, int h) {return 1;}

	const char *getmapinfo() {return NULL;}

	void g3d_gamemenus() {}
	void adddynlights() {}
	void resetgamestate() {}
	void newmap(int size){}
	void writemapdata(stream *f) {}
	void preload() {}
	void parsepacketclient(int chan, packetbuf &p) {}
	void writeclientinfo(stream *f) {}
	void toserver(char *text) {}
	void connectattempt(const char *name, const char *password, const ENetAddress &address) {}
	void connectfail() {}
	void parseoptions(vector<const char *> &args) {}
	void renderavatar() {}
	void setupcamera(){}
	void lighteffects(dynent *d, vec &color, vec &dir) {}
	void particletrack(physent *owner, vec &o, vec &d) {}
	void dynlighttrack(physent *owner, vec &o, vec &hud) {}
	void vartrigger(ident *id) {}
	void bounced(physent *d, const vec &surface) {}
	void registeranimation(char *dir, char* anim, int num) {}
	void writegamedata(vector<char> &extras) {}
	void readgamedata(vector<char> &extras)  {}
	void loadconfigs(){}
	void dynentcollide(physent *d, physent *o, const vec &dir) {}
	void edittrigger(const selinfo &sel, int op, int arg1, int arg2, int arg3) {}
	void texturefailed(char *name, int slot) {}
	void mmodelfailed(const char *name, int idx) {}
	void mapfailed(const char *name) {}
	void suicide(physent *d){}

	bool showenthelpers() { return editmode; }
	bool ispaused() { return false; }
	bool allowdoublejump(physent *d) {return false;}
	bool detachcamera() { return false; }
	bool needminimap() { return false; }
	bool allowedittoggle() { return true; }
	bool allowmove (physent *d) {return true;}
	bool collidecamera() {return !editmode;}
}

namespace entities
{
	int extraentinfosize() { return 0; }
	int *getmodelattr(extentity &e) { return NULL; }

	const char *entnameinfo(entity &e) { return ""; }

	void fixentity(extentity &e){}
	void editent(int i, bool local) {}
	void writeent(entity &e, char *buf) {}
	void renderhelpertext(extentity &e, int &colour, vec &pos, string &tmp){}
	void entradius(extentity &e, bool &color){}
	void readent(entity &e, char *buf, int ver){}

	bool dirent(extentity &e){return false;}
	bool radiusent(extentity &e){return false;}
	bool mayattach(extentity &e) { return false; }
	bool attachent(extentity &e, extentity &a) { return false; }
	bool printent(extentity &e, char *buf) { return false; }
	bool checkmodelusage(extentity &e, int i){ return false; }
}

namespace server
{
	int reserveclients() { return 0; }
	int clientconnect(int n, uint ip) { return DISC_NONE; }
	int numchannels() {return 0;}
	int laninfoport() {return 0;}
	int serverinfoport(int servport) {return 0;}
	int serverport(int infoport) {return 0;}
	int masterport() {return 0;}

	const char *defaultmaster() {return "";}


	void *newclientinfo() {return NULL;}
	void deleteclientinfo(void *ci) {}
	void serverinit() {}
	void clientdisconnect(int n) {}
	void localdisconnect(int n) {}
	void localconnect(int n) {}
	void recordpacket(int chan, void *data, int len) {}
	void parsepacket(int sender, int chan, packetbuf &p) {}
	void sendservmsg(const char *s) {}
	void serverinforeply(ucharbuf &q, ucharbuf &p) {}
	void serverupdate() {}
	void processmasterinput(const char *cmd, int cmdlen, const char *args) {}

	bool allowbroadcast(int n) { return false; }
	bool sendpackets(bool force) { return false; }
	bool servercompatible(char *name, char *sdec, char *map, int ping, const vector<int> &attr, int np) { return true; }
	bool ispaused() {return game::ispaused();}
}
