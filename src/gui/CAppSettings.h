/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_SETTINGS_H
#define PRODRAW_SETTINGS_H

#include <QtGui>
#include <mindmap/CMindMapSettings>

namespace Settings
{
    const QString	LibraryRoot = QString("app/path/library-root");
    const QString   TemplateRoot = QString("app/path/template-root");
    const QString	PluginRoot = QString("app/path/plugin-root");
    const QString	RecentFiles	 = QString("app/history/recent-files");
    const QString   LastLocation = QString("app/history/last-location");
    const QString	PurchaseUrl	= QString("app/url/purchase");
    const QString	SupportUrl = QString("app/url/support");
    const QString	DownloadUrl = QString("app/url/download");
    const QString	ShowToolBar = QString("app/ui/show-toolbar");
    const QString	ProductActivated = QString("app/activated");

} // End of namespace

namespace  AppConfig
{
    const QString JetMindFileSuffix = QString("*.rdx");
    const QString JetMindFileString = QString("JetMind File(*.rdx)");
} // End of namespace

#endif // PRODRAW_SETTINGS_H
