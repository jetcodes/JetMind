/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include "CUiPercentPicker.h"

CUiPercentPicker::CUiPercentPicker(QWidget *parent)
	: QMenu(parent)
{
	setupPercentages();
	setupMenu();

	connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onActionTriggered(QAction*)));
}

CUiPercentPicker::~CUiPercentPicker()
{

}


void CUiPercentPicker::setupPercentages()
{
	m_names.push_back( QString::fromUtf8("25%"));
	m_icons.push_back( QIcon() );
	m_percs.push_back(25);

	m_names.push_back( QString::fromUtf8("50%"));
	m_icons.push_back( QIcon() );
	m_percs.push_back(50);

	m_names.push_back( QString::fromUtf8("75%"));
	m_icons.push_back( QIcon() );
	m_percs.push_back(75);

	m_names.push_back( QString::fromUtf8("100%"));
	m_icons.push_back( QIcon() );
	m_percs.push_back(100);

	m_names.push_back( QString::fromUtf8("125%"));
	m_icons.push_back( QIcon() );
	m_percs.push_back(125);

	m_names.push_back( QString::fromUtf8("150%"));
	m_icons.push_back( QIcon() );
	m_percs.push_back(150);

	m_names.push_back( QString::fromUtf8("200%"));
	m_icons.push_back( QIcon() );
	m_percs.push_back(200);
	
	m_names.push_back( QString::fromUtf8("300%"));
	m_icons.push_back( QIcon() );
	m_percs.push_back(300);

	m_names.push_back( QString::fromUtf8("400%"));
	m_icons.push_back( QIcon() );
	m_percs.push_back(400);
}

void CUiPercentPicker::setupMenu()
{
	int				i = 0;
	QAction			*act = NULL;

	for (i = 0; i < m_names.length(); ++i)
	{
		act = addAction(m_icons[i], m_names[i]);
		m_actions.push_back(act);
	}
}

void CUiPercentPicker::onActionTriggered(QAction* act)
{
	int				i = 0;
	for (i = 0; i < m_actions.length(); ++i)
	{
		if (act == m_actions[i])
			break;
	}
	emit percentageChanged( m_percs[i] );
}

