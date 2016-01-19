/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramLayout>

CDiagramLayout::CDiagramLayout(QObject *parent, const QString &name, const QIcon &icon)
	: QObject(parent),
	  m_name(name),
	  m_icon(icon)
{

}

CDiagramLayout::~CDiagramLayout()
{
	
}

void CDiagramLayout::layoutDiagram(CDiagram *diagram)
{
	// TO BE OVERRIEDED
}

void CDiagramLayout::layoutItem(CDiagramItem *item)
{
	// TO BE OVERRIEDED
}

void CDiagramLayout::onLayoutDiagram(CDiagram *diagram)
{
	layoutDiagram(diagram);
}

void CDiagramLayout::onLayoutItem(CDiagramItem *item)
{
	layoutItem(item);
}

void CDiagramLayout::setName(const QString &name)
{
	m_name = name;
}

QString CDiagramLayout::name()
{
	return m_name;
}

void CDiagramLayout::setIcon(const QIcon &icon)
{
	m_icon = icon;
}

QIcon CDiagramLayout::icon()
{
	return m_icon;
}