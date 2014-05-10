#include "launcher.h"

void mainwindow::aboutqt()
{
	QApplication::aboutQt();
}

void mainwindow::aboutus()
{
	About->show();
}

void mainwindow::reset()
{
	IO::reset();
	refreshWidgets();
}

void mainwindow::quit()
{
	QApplication::closeAllWindows();
}

void mainwindow::run()
{
	//only allow one instance at a time
	if(Executable->state() == QProcess::NotRunning)
	{
		QString exe = "bin/sandbox_";

		#ifdef Q_OS_LINUX
		exe += "client_32_";
		//TODO detect 64bit unix/linux
		#endif

		exe += Games->currentText();

		#ifdef Q_OS_WIN
		exe += ".exe";
		#endif

		IO::writeCfg();

		QStringList args;
		if(homedir.path() != "")
			args << QString("-q" + QDir::toNativeSeparators(homedir.path()));
		args << "-r";
		loopv(i, packagedirs)
		{
			args << QString("-k" + QDir::toNativeSeparators(packagedirs[i]));
		}

		Executable->setStandardOutputFile("stdout.txt");
		Executable->setStandardErrorFile("stderror.txt");
		Executable->start(exe, args);
	}
}

//This is executed when sandbox quits/crashes/whatever
void mainwindow::closed()
{
	IO::readCfg();
	refreshWidgets();
}

void mainwindow::refreshWidgets()
{

}

void mainwindow::createWidgets()
{
	QGridLayout *window = new QGridLayout(this);
	window->setMargin(0);
	/**

		The Toolbar

	*/
	{
		FileMenu = new QMenu("File", this);

		Reset = FileMenu->addAction("Reset");
		Reset->setMenuRole(QAction::ApplicationSpecificRole);
		connect(Reset, SIGNAL(triggered()), this, SLOT(reset()));

		FileMenu->addSeparator();

		Quit = FileMenu->addAction("Exit");
		Quit->setMenuRole(QAction::QuitRole);
		connect(Quit, SIGNAL(triggered()), this, SLOT(quit()));

		AboutMenu = new QMenu("About", this);

		AboutQt = AboutMenu->addAction("About Qt");
		AboutQt->setMenuRole(QAction::AboutQtRole);
		connect(AboutQt, SIGNAL(triggered()), this, SLOT(aboutqt()));

		AboutUs = AboutMenu->addAction("About Sandbox");
		AboutUs->setMenuRole(QAction::AboutRole);
		connect(AboutUs, SIGNAL(triggered()), this, SLOT(aboutus()));

		Menu = new QMenuBar(this);
		Menu->addMenu(FileMenu);
		Menu->addMenu(AboutMenu);

		window->addWidget(Menu, 0, 0, 1, 2);
	}

	/**

		Stuff to select the game and launch it
		and select the map

	*/
	{
		QGridLayout *launch = new QGridLayout();
		window->addLayout(launch, 1, 0, 2, 1);

		QStringList GameList;
		GameList << "fps" << "krs" << "movie" << "rpg" << "ssp";

		Games = new QComboBox(this);
		Games->addItems(GameList);
		launch->addWidget(Games, 0, 0);

		Executable = new QProcess(this);
		connect(Executable, SIGNAL(finished(int)), this, SLOT(closed()));

		Launch = new QPushButton("Launch", this);
		connect(Launch, SIGNAL(released()), this, SLOT(run()));

		launch->addWidget(Launch, 1, 0);
	}

	/**



	*/

	/**



	*/

	//setLayout(window);
}
