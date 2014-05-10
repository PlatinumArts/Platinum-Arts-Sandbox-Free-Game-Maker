#include "sspgame.h"

namespace game
{
	void parsepacketclient(int chan, packetbuf &p) {}
	int sendpacketclient(ucharbuf &p, bool &reliable, dynent *d) { return -1; }
	bool allowedittoggle() { return true; }
	void writeclientinfo(stream *f) {}
	void toserver(char *text) {}
	void connectattempt(const char *name, const char *password, const ENetAddress &address) {}
	void connectfail() {}
	void parseoptions(vector<const char *> &args) {}
	bool allowmove (physent *d) {return true;}
	void renderavatar() {}
	bool collidecamera() {return !editmode;}
	void setupcamera(){}
	void lighteffects(dynent *d, vec &color, vec &dir) {}
	void particletrack(physent *owner, vec &o, vec &d) {}
	void dynlighttrack(physent *owner, vec &o, vec &hud) {}
	void preload() {}
	void g3d_gamemenus() {}
	bool ispaused() {return false;}
	void vartrigger(ident *id) {}
	void bounced(physent *d, const vec &surface) {}

	void registeranimation(char *dir, char* anim, int num) {}
}

namespace server
{
	void *newinfo() { return NULL; }
	void deleteinfo(void *ci) {}
	void serverinit() {}
	void clientdisconnect(int n) {}
	int clientconnect(int n, uint ip) { return DISC_NONE; }
	void localdisconnect(int n) {}
	void localconnect(int n) {}
	int numchannels() {return 0;}
	void parsepacket(int sender, int chan, packetbuf &p) {}
	void sendservmsg(const char *s) {}
	bool sendpackets(bool force) { return false; }
	int welcomepacket(ucharbuf &p, int n, ENetPacket *packet) { return -1; }
	void serverinforeply(ucharbuf &q, ucharbuf &p) {}
	void serverupdate(int lastmillis, int totalmillis) {}
	bool servercompatible(char *name, char *sdec, char *map, int ping, const vector<int> &attr, int np) { return true; }
	void serverinfostr(char *buf, const char *name, const char *desc, const char *map, int ping, const vector<int> &attr, int np) {}

	int laninfoport() {return 0;}
	int serverinfoport(int servport) {return 0;}
	int serverport(int infoport) {return 0;}
	const char *defaultmaster() {return "";}
	int masterport() {return 0;}
	void processmasterinput(const char *cmd, int cmdlen, const char *args) {}

	bool allowbroadcast(int n) { return false; }
	int reserveclients() { return 0; }
	bool ispaused() {return game::ispaused();}

	void *newclientinfo() {return NULL;}
	void deleteclientinfo(void *ci) {}
	void recordpacket(int chan, void *data, int len) {}
	void serverupdate() {}
}
