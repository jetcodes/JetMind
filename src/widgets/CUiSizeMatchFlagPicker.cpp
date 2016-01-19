/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#include "CUiSizeMatchFlagPicker.h"

CUiSizeMatchFlagPicker::CUiSizeMatchFlagPicker(QWidget *parent)
	: QMenu(parent),
	  m_default(Global::MatchMaximumWidth)
{
	setupFlags();
	setupMenu();

	connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onActionTriggered(QAction*)));
}

CUiSizeMatchFlagPicker::~CUiSizeMatchFlagPicker()
{

}

QIcon CUiSizeMatchFlagPicker::getIcon(Global::MatchSizeFlag value)
{
	int		index = 0;
	QIcon	icon;

	index = m_flags.indexOf(value);
	if (index >= 0 && index < m_icons.length())
		icon = m_icons[index];
	return icon;
}

void CUiSizeMatchFlagPicker::setupFlags()
{
	m_names.push_back( QString::fromUtf8("Match Maximum Height"));
	m_icons.push_back( QIcon(QString::fromUtf8(":/res/icons/equalsize_maxheight_16x16.png")) );
	m_flags.push_back( Global::MatchMaximumHeight );

	m_names.push_back( QString::fromUtf8("Match Maximum Width"));
	m_icons.push_back( QIcon(QString::fromUtf8(":/res/icons/equalsize_maxwidth_16x16.png")) );
	m_flags.push_back( Global::MatchMaximumWidth );

	m_names.push_back( QString::fromUtf8("Match Minimum Height"));
	m_icons.push_back( QIcon(QString::fromUtf8(":/res/icons/equalsize_minheight_16x16.png")) );
	m_flags.push_back( Global::MatchMinimumHeight );

	m_names.push_back( QString::fromUtf8("Match Minimum Width"));
	m_icons.push_back( QIcon(QString::fromUtf8(":/res/icons/equalsize_minwidth_16x16.png")) );
	m_flags.push_back( Global::MatchMinimumWidth );

	m_default = Global::MatchMaximumHeight;
}

void CUiSizeMatchFlagPicker::setupMenu()
{
	int				i = 0;
	QAction			*act = NULL;

	for (i = 0; i < m_names.length(); ++i)
	{
		act = addAction(m_icons[i], m_names[i]);
		m_actions.push_back(act);
	}
}

void CUiSizeMatchFlagPicker::onActionTriggered(QAction* act)
{
	int				i = 0;
	for (i = 0; i < m_actions.length(); ++i)
	{
		if (act == m_actions[i])
			break;
	}
	emit equalFlagSelected( m_flags[i], m_icons[i] );
}
