#include "launcher.h"

aboutwindow *About;

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	IO::reset();
	IO::readDat();
	IO::readCfg();

	mainwindow window;
	window.createWidgets();
	window.setWindowTitle("Platinum Arts Sandbox - Launcher");
	window.show();

	About = new aboutwindow();

	int ret = app.exec();

	IO::writeCfg();
	IO::writeDat();

	return ret;
}