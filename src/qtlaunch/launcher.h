#include <QtGui>

#define loop(v, c) for(int v = 0; v < (c); v++)
#define looprev(v, c) for(int v = (c) - 1; v >= 0; v--)

#define loopv(v, c) for(int v = 0; v < (c).count(); v++)
#define loopvrev(v, c) for(int v = (c).count() - 1; v >= 0; v--)

class aboutwindow : public QWidget
{
	Q_OBJECT

	public:
		aboutwindow();
		~aboutwindow() {}

	private:
		//Logo?
		//Info

	private slots:
		//close

};

extern aboutwindow *About;

class mainwindow : public QWidget
{
	Q_OBJECT

	public:
		mainwindow() {}
		~mainwindow() {}

		void createWidgets();
		void refreshWidgets();

	private:
		//The Toolbar
		QMenuBar *Menu;

		QMenu *FileMenu;
		QAction *Reset, *Quit;

		QMenu *AboutMenu;
		QAction *AboutQt, *AboutUs;

		//Widgets for selecting and launching the game

		QComboBox *Games;
		QProcess *Executable;
		QPushButton *Launch;

		//Widgets for modifying init.cfg

		//Widgets for playing around with packagedirs

	private slots:
		void reset();
		void quit();
		void aboutqt();
		void aboutus();
		void run();
		void closed();
		//commitpackagedir
		//deletepackagedir
		//run
		//quit
};

enum
{
	VAR_FULLSCREEN = 0,
	VAR_SCRW,
	VAR_SCRH,
	VAR_COLOURB,
	VAR_DEPTHB,
	VAR_STENCILB,
	VAR_FSAA,
	VAR_VSYNC,
	VAR_SHADERS,
	VAR_SPRECISION,
	VAR_GLSL,
	VAR_SCHAN,
	VAR_SFREQ,
	VAR_SBUFLEN,
	VAR_MAX
};

extern QDir homedir;
extern QStringList packagedirs;
extern QByteArray variables[];

namespace IO
{
	extern void reset();

	//reads and writes binary dat file which saves the current homedir, lastmap and loaded packagedirs
	extern void readDat();
	extern void writeDat();

	//reads and writes init.cfg; this depends on it being valid
	extern void readCfg();
	extern void writeCfg();
}