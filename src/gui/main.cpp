/**
* Copyright (c) 2011 JetMind. All rights reserved.
*/

#include <QtGui>

#include <core/CGlobal>
#include <core/CDiagramCore>
#include <core/CDiagramPlugin>
#include <core/CDiagramTemplateManager>
#include <core/CDiagramWidget>
#include <core/CDiagramWidgetManager>
#include <core/CDiagramLibraryModel>
#include <core/CDiagramContext>
#include <core/CDiagramItemToolBar>
#include <core/CDiagramTextToolBar>
#include <core/CDiagramDemoToolBar>
#include <widgets/CUiSplashScreen>

#include <mindmap/CMindMapLoader>

#include "CWorkbench.h"
#include "CApplication.h"
#include "CAppSettings.h"
#include "CPresentationToolbar.h"

static void initEnvironment()
{
    QDir					templateRoot;
	CDiagramContext			*env = NULL;
	CDiagramWidget			*widget = NULL;
	CDiagramWidgetManager	*widgetManager = NULL;
	CDiagramTemplateManager	*templateManager = NULL;
    CDiagramPluginEngine    *engine = NULL;
	
	// Core environment initialization
	env = CDiagramContext::env();
	if (!env)
		return;

	// core initialization
	env->initCoreEnvironment();

	// Custom widgets for diagram manipulation is registered here
	// Widget registerations
    widgetManager = dynamic_cast<CDiagramWidgetManager*>( env->widgetManager() );
	widget = widgetManager->getWidget( CDiagramDemoToolBar::WidgetType );
    widget->addHostedWidget( new CPresentationToolbar() );

	templateManager = CDiagramTemplateManager::instance();
	if (templateManager)
	{
		templateRoot = QDir(qApp->applicationDirPath());
        qDebug() << templateRoot.absolutePath();
		templateManager->setTemplateRoot( templateRoot.absolutePath() );
	}

    engine = CDiagramPluginEngine::instance();
    if (engine)
        CMindMapLoader::load(engine);
}

static void loadTemplates()
{
    QString                 filePath, prefix(":/res/templates/%1.tpl");
    QFileInfo               fileInfo;
	CDiagramTemplateManager	*templateManager = NULL;

	templateManager = CDiagramTemplateManager::instance();
	if (templateManager)
	{
        // load core templates
        for (int i = 1; ; ++i)
        {
            filePath = prefix.arg( QString::number(i).rightJustified(3, QChar('0')) );
            fileInfo.setFile(filePath);
            if ( fileInfo.exists() && fileInfo.isFile() )
            {
                templateManager->loadAsTemplate(filePath);
            }
            else
                break;
        }

        // load custom templates
        // templateManager->load();
	}
}

int main(int argc, char *argv[])
{
    CApplication	app(argc, argv);
    QPixmap			pixmap(":/res/icons/app-start.png");
	QSplashScreen	*splash = NULL;
	QString			tpl = "\n          %1";
    QColor			c(0, 0, 0, 168);
    CWorkbench		*bench = NULL;

    QCoreApplication::setOrganizationName("JetMind");
    QCoreApplication::setOrganizationDomain("jetmind.com");
    QCoreApplication::setApplicationName("JetMind");

    app.setWindowIcon(QIcon(QString::fromUtf8(":/res/icons/app-logo.png")));

#if defined(__APPLE__) && defined(__MACH__)
	app.setFont( QFont("Arial", 12) );
#else
	app.setFont( QFont("Arial", 9) );
#endif //

	splash = new QSplashScreen(pixmap);
	splash->setMask(pixmap.mask());
	splash->show();
	app.processEvents();

	splash->showMessage(tpl.arg( QObject::tr("Initing Environment...") ), Qt::AlignBottom | Qt::AlignRight , c);
	app.processEvents();
	initEnvironment();

	splash->showMessage(tpl.arg( QObject::tr("Loading Templates...") ), Qt::AlignBottom | Qt::AlignRight , c);
	app.processEvents();
    loadTemplates();

	splash->showMessage(tpl.arg( QObject::tr("Starting Workbench...") ), Qt::AlignBottom | Qt::AlignRight , c);
	app.processEvents();

	bench = new CWorkbench();
    bench->setWindowTitle("JetMind");
	bench->resize(800, 600);
	bench->show();
    bench->loadStyleSheet();
	app.setWorkbench(bench);
	CDiagramContext::env()->setMainUI(bench);

	splash->finish(bench);

	// set url handler
	QDesktopServices::setUrlHandler("topic", (QObject*)bench, "onOpenUrl");
	QDesktopServices::setUrlHandler("sheet", (QObject*)bench, "onOpenUrl");

#if defined(__APPLE__) && defined(__MACH__)
    bench->createFile();
#else
	QStringList args;
	args = app.arguments();
	if (args.length() > 1)
	{
		bench->loadFile(args.at(1));
	}
	else
	{
		bench->createFile();
	}
#endif //

	return app.exec();
}
