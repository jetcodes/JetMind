/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#include <core/CGlobal>
#include "CUiPageSizePicker.h"

CUiPageSizePicker::CUiPageSizePicker(QWidget *parent)
	: QMenu(parent)
{
	setupSizes();
	setupMenu();

	connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onActionTriggered(QAction*)));
}

CUiPageSizePicker::~CUiPageSizePicker()
{

}

void CUiPageSizePicker::setupSizes()
{
	/*
	m_names.push_back( QString::fromUtf8("A3"));
	m_icons.push_back( QIcon() );
	m_sizes.push_back( Global::getPageSize( Global::A3 ) );

	m_names.push_back( QString::fromUtf8("A4"));
	m_icons.push_back( QIcon() );
	m_sizes.push_back( Global::getPageSize( Global::A4 ) );

	m_names.push_back( QString::fromUtf8("B5"));
	m_icons.push_back( QIcon() );
	m_sizes.push_back( Global::getPageSize( Global::B5 ) );

	m_names.push_back( QString::fromUtf8("Full Page"));
	m_icons.push_back( QIcon() );
	m_sizes.push_back( QSizeF(0,0) );

	m_names.push_back( QString::fromUtf8("Customized"));
	m_icons.push_back( QIcon() );
	m_sizes.push_back( QSizeF(-1,-1) );
	*/
}

void CUiPageSizePicker::setupMenu()
{
	int				i = 0;
	QAction			*act = NULL;

	for (i = 0; i < m_names.length(); ++i)
	{
		act = addAction(m_icons[i], m_names[i]);
		m_actions.push_back(act);
	}
}

void CUiPageSizePicker::onActionTriggered(QAction* act)
{
	int				i = 0;
	for (i = 0; i < m_actions.length(); ++i)
	{
		if (act == m_actions[i])
			break;
	}
	emit pageSizeChanged( m_sizes[i] );
}
