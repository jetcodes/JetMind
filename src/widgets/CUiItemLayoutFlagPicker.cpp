/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <QtGui>
#include "CUiItemLayoutFlagPicker.h"


CUiItemLayoutFlagPicker::CUiItemLayoutFlagPicker(QWidget *parent)
	: QMenu(parent),
	  m_default(Qt::Horizontal)
{
	setupFlags();
	setupMenu();

	connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onActionTriggered(QAction*)));
}

CUiItemLayoutFlagPicker::~CUiItemLayoutFlagPicker()
{

}

QIcon CUiItemLayoutFlagPicker::getIcon(Qt::Orientation value)
{
	int		index = 0;
	QIcon	icon;

	index = m_flags.indexOf(value);
	if (index >= 0 && index < m_icons.length())
		icon = m_icons[index];
	return icon;
}

void CUiItemLayoutFlagPicker::setupFlags()
{
	m_names.push_back( QString::fromUtf8("Distribute Horizontally"));
	m_icons.push_back( QIcon(QString::fromUtf8(":/res/icons/layouthorizontal_16x16.png")) );
	m_flags.push_back( Qt::Horizontal );

	m_names.push_back( QString::fromUtf8("Distribute Vertically"));
	m_icons.push_back( QIcon(QString::fromUtf8(":/res/icons/layoutvertical_16x16.png")) );
	m_flags.push_back( Qt::Vertical );

	m_default = Qt::Horizontal;
}

void CUiItemLayoutFlagPicker::setupMenu()
{
	int				i = 0;
	QAction			*act = NULL;

	for (i = 0; i < m_names.length(); ++i)
	{
		act = addAction(m_icons[i], m_names[i]);
		m_actions.push_back(act);
	}
}

void CUiItemLayoutFlagPicker::onActionTriggered(QAction* act)
{
	int				i = 0;
	for (i = 0; i < m_actions.length(); ++i)
	{
		if (act == m_actions[i])
			break;
	}

	if (i >= 0 && i < m_actions.length())
	{
		m_default = m_flags[i];
		emit layoutFlagSelected( m_flags[i], m_icons[i] );
	}
}
