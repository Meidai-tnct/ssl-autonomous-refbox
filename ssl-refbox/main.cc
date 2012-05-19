/**
 * @mainpage ssl-autonomous-refbox
 *
 * The developed System uses the SSL Shared Vision to get the Knowledge about the positions of all objects.
 * On these information a Particle Filter is run for object tracking.
 * Some heuristical functions are implemented for getting information about ball collisions,
 * and at the end a Prolog Rule System analyses if some of the rules given for the SSL are broken.
 * The feedback is given on a QT and OpenGL-based GUI.
 *
 * @author Ulfert Nehmiz
 * @author Nicolai Ommer
 * @author Daniel Andres
 * @author Felix Pistorius
 */

/**
 * @file main.cc
 * @brief This file includes the main() function that is the entrance to the application
 */
#include <QApplication>
#include <QMainWindow>
#include <GL/glut.h>
#include "ui_GuiControls.h"
#include "guiactions.h"
#include "GuiPropertiesDlg.h"
#include <iostream>
#include <stdio.h>
#include <sys/stat.h>

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/rollingfileappender.h>

#include "global.h"

using namespace log4cxx;
using namespace log4cxx::helpers;
using namespace std;

LoggerPtr logger(Logger::getLogger("main"));

/**
 * @brief Returns if the given file exists
 * @param filename full path to the file
 * @return true, if file exists
 */
bool fexists(string filename) {
	std::ifstream ifile(filename.c_str());
	return ifile;
}

/**
 * @brief This is the main function of the application from where all start :)
 *
 * The main function will to the following actions:
 * initialize logger (log4cxx)
 * load config file
 * initialize QT application
 * start the QT application
 *
 * @param argc number of arguments
 * @param argv list of arguments
 * @return exit code (from QT application)
 */
int main(int argc, char* argv[]) {
	// if you want to start reading the code,
	// you might want to look in guiactions.cc
	// from there, actions start ;)

	// If the log4cxx config exist, load it, else create a default config
	if (fexists("log4j.conf")) {
		PropertyConfigurator::configure("log4j.conf");
	} else {
		// get home directory
		string home = getenv("HOME");
		if (home.empty()) {
			home = "/root";
		}
		// create directory, if it not exists
		mkdir(string(home + "/.ssl-autonomous-refbox").c_str(), 0775);

		// save all logs in a logfile
		string logfile = home + "/.ssl-autonomous-refbox/refbox.log";
		// define the log pattern
		LayoutPtr layout(new PatternLayout("\%6r \%-5p \%-16c{1} \%-50m \%n"));
		// create a file and console appender, so that all log events
		// will be sent to the logfile and to the std output
		RollingFileAppender * fileAppender = new RollingFileAppender(layout, logfile, true);
		fileAppender->fileLength = 500000; // size_t equal to num in malloc etc. => byte
		ConsoleAppender * consoleAppender = new ConsoleAppender(layout);

		// From log4cxx doc:
		// Sets and opens the file where the log output will go
		helpers::Pool p;
		fileAppender->activateOptions(p);

		// add the appenders to the logger
		BasicConfigurator::configure(AppenderPtr(fileAppender));
		BasicConfigurator::configure(AppenderPtr(consoleAppender));
		// set log level
		Logger::getRootLogger()->setLevel(Level::getInfo());
	}


	// handle arguments
	string custConfig = "";
	Global::logFile = NULL;
	for (int i = 1; i < argc; i++) {
		if(strcmp(argv[i],"--help") == 0 || strcmp(argv[i],"-h") == 0) {
			printf("Following options are available:\n");
			printf("%-20s %s\n", "-h (--help)","Print this help");
			printf("%-20s %s\n", "-c configfile","Use given config file");
			printf("%-20s %s\n", "logfile","Immediately start given log file");
			exit(0);
		} else if(strcmp(argv[i], "-c") == 0) {
			if(i + 1>=argc) {
				fprintf(stderr,"Missing parameter for option -c\n");
				exit(1);
			}
			custConfig = argv[i+1];
			i++;
		} else {
			// load log file
			Global::logFile = argv[i];
//			logFile = argv[i];
//			Global::setLogFile(argv[i]);
		}
	}

	LOG4CXX_INFO(logger, "");
	LOG4CXX_INFO(logger, "Entering application.");

	// load the config file. Settings are stored publicly
	Global::loadConfig(custConfig);

	// external variable in ssl_refbox_rules.h for initializing prolog
	argv_global = argv[0];

	// initialize qt app and window
	QApplication app(argc, argv);
	QMainWindow* refbox = new QMainWindow;
	Ui::GuiControls* gui = new Ui::GuiControls;
	GuiActions act(gui, refbox);
	gui->setupUi(refbox);

	// connect GUI components with actions
	act.connectActions();

	// initialize the GLUT library
	glutInit(&argc, argv);

	// display MainWindow
	refbox->show();

	//main stuff
	int res = app.exec();

	LOG4CXX_INFO(logger, "Exit application");

	return res;
}

