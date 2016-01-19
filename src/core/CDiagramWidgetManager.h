/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMWIDGETMANAGER_H
#define PRODRAW_CDIAGRAMWIDGETMANAGER_H

#include <QtGui>
#include <core/CGlobal>

class CDiagram;
class CDiagramWidget;
class CDiagramWidgetManager;

class CORE_EXPORT CDiagramWidgetManager
{
public:
	CDiagramWidgetManager();
	virtual ~CDiagramWidgetManager();
	
	virtual void init(CDiagram *d);
	virtual void uninit(CDiagram *d);

	void addWidget(CDiagramWidget *widget);
	void removeWidget(CDiagramWidget *widget);
	CDiagramWidget* getWidget(int widgetType);
	QList<CDiagramWidget*> widgets();

protected:
	QList<CDiagramWidget*>			m_widgets;
	CDiagram						*m_d;

private:
	Q_DISABLE_COPY(CDiagramWidgetManager)

}; // End of CDiagramWidgetManager

#endif // PRODRAW_CDIAGRAMWIDGETMANAGER_H
