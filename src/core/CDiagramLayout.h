/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMLAYOUT_H
#define PRODRAW_CDIAGRAMLAYOUT_H

#include <QtGui>
#include <core/CGlobal>

class CDiagram;
class CDiagramItem;

class CORE_EXPORT CDiagramLayout : public QObject
{
	Q_OBJECT

public:
	CDiagramLayout(QObject *parent,const QString &name = QString(""), const QIcon &icon = QIcon());
	virtual ~CDiagramLayout();

	// Used to layout the whole diagram's nodes
	virtual void layoutDiagram(CDiagram *diagram);

	// Used to layout specified node, use it as the
	// root element
	virtual void layoutItem(CDiagramItem *item);
	
	void setName(const QString &name);
	QString name();

	void setIcon(const QIcon &icon);
	QIcon icon();

public Q_SLOTS:
	// Slots are used to perform auto layout
	// The nodes will be layouted automatically
	// when their geometries are changed.
	void onLayoutDiagram(CDiagram *diagram);
	void onLayoutItem(CDiagramItem *item);

protected:
	QString					m_name;
	QIcon					m_icon;

}; // End of class CDiagramLayout

#endif // CDIAGRAMLAYOUT_H
