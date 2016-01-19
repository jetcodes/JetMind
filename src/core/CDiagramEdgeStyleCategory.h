/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CDIAGRAMEDGESTYLECATEGORY_H
#define PRODRAW_CDIAGRAMEDGESTYLECATEGORY_H

#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramEdgeStyle>

class CORE_EXPORT CDiagramEdgeStyleCategory
{

public:
	CDiagramEdgeStyleCategory(const QString &name);
	virtual ~CDiagramEdgeStyleCategory();

	virtual void load() {};

	int count();
	QString name() { return m_name; }
	CDiagramEdgeStyle *getStyle(const QString &name);
	CDiagramEdgeStyle *getDefaultStyle();
	QList<CDiagramEdgeStyle*> styles();

protected:
	QString								m_name;
	QMap<QString, CDiagramEdgeStyle*>	m_styles;
};

// Make it an interface for plugins
QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(CDiagramEdgeStyleCategory,"com.prodraw.CDiagramEdgeStyleCategory/1.0")
QT_END_NAMESPACE

#endif // PRODRAW_CDIAGRAMEDGESTYLECATEGORY_H
