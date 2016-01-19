/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMARROWCATEGORY_H
#define PRODRAW_CDIAGRAMARROWCATEGORY_H

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramArrow>

class CORE_EXPORT CDiagramArrowCategory
{
public:
	CDiagramArrowCategory(const QString &name);
	virtual ~CDiagramArrowCategory();

	virtual void load() {};

	int count();
	QString name() { return m_name; }
	CDiagramArrow *getArrow(const QString &name);
	CDiagramArrow *getDefaultArrow();
	QList<CDiagramArrow*> arrows();

protected:
	QString							m_name;
	QMap<QString, CDiagramArrow*>	m_arrows;
};

// Make it an interface for plugins
QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(CDiagramArrowCategory,"com.prodraw.CDiagramArrowCategory/1.0")
QT_END_NAMESPACE

#endif // PRODRAW_CDIAGRAMARROWCATEGORY_H
