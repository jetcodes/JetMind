/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagram>
#include <core/CDiagramWidget>
#include <core/CDiagramWidgetManager>

CDiagramWidgetManager::CDiagramWidgetManager()
	: m_d(NULL)
{
}

CDiagramWidgetManager::~CDiagramWidgetManager()
{
}

void CDiagramWidgetManager::addWidget(CDiagramWidget *widget)
{
	if (!widget) return;

	if (!m_widgets.contains(widget))
	{
		m_widgets.push_back(widget);
		if (m_d)
			widget->init(m_d);
	}
}

void CDiagramWidgetManager::removeWidget(CDiagramWidget *widget)
{
	if (!widget)
		return;

	m_widgets.removeAll(widget);
	if (m_d)
		widget->uninit(m_d);
}

CDiagramWidget* CDiagramWidgetManager::getWidget(int widgetType)
{
	CDiagramWidget	*widget = NULL;

	foreach (CDiagramWidget *w, m_widgets)
	{
		if (w->widgetType() == widgetType)
		{
			widget = w;
			break;
		}
	}

	return widget;
}

QList<CDiagramWidget*>	CDiagramWidgetManager::widgets()
{
	return m_widgets;
}

void CDiagramWidgetManager::init(CDiagram *d)
{
	CDiagramWidget		*widget = NULL;

	if (!d || d == m_d)
		return;
	
	foreach (widget, m_widgets)
	{
		widget->init(d);
	}
	m_d = d;
}

void CDiagramWidgetManager::uninit(CDiagram *d)
{
	CDiagramWidget		*widget = NULL;

	if (!d || d != m_d)
		return;

	foreach (widget, m_widgets)
	{
		widget->uninit(d);
	}
	m_d = NULL;
}