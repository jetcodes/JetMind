/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#include "CUiAlignmentFlagPicker.h"

CUiAlignmentFlagPicker::CUiAlignmentFlagPicker(QWidget *parent)
	: QMenu(parent),
	  m_default(Qt::AlignLeft)
{
	setupAligns();
	setupMenu();

	connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onActionTriggered(QAction*)));
}

CUiAlignmentFlagPicker::~CUiAlignmentFlagPicker()
{

}

QIcon CUiAlignmentFlagPicker::getIcon(Qt::AlignmentFlag flag)
{
	QIcon		icon;
	int			index = 0;

	index = m_aligns.indexOf(flag);
	if (index >= 0 && index < m_icons.length())
		icon = m_icons[index];
	return icon;
}

void CUiAlignmentFlagPicker::setupAligns()
{
	m_names.push_back( QString::fromUtf8("Align Left"));
	m_icons.push_back( QIcon(QString::fromUtf8(":/res/icons/objectalignleft_16x16.png")) );
	m_aligns.push_back( Qt::AlignLeft );

	m_names.push_back( QString::fromUtf8("Align Right"));
	m_icons.push_back( QIcon(QString::fromUtf8(":/res/icons/objectalignright_16x16.png")) );
	m_aligns.push_back( Qt::AlignRight );

	m_names.push_back( QString::fromUtf8("Align Horizontal Center"));
	m_icons.push_back( QIcon(QString::fromUtf8(":/res/icons/aligncenter_16x16.png")) );
	m_aligns.push_back( Qt::AlignHCenter );

	m_names.push_back( QString::fromUtf8("Align Top"));
	m_icons.push_back( QIcon(QString::fromUtf8(":/res/icons/alignup_16x16.png")) );
	m_aligns.push_back( Qt::AlignTop );

	m_names.push_back( QString::fromUtf8("Align Bottom"));
	m_icons.push_back( QIcon(QString::fromUtf8(":/res/icons/aligndown_16x16.png")) );
	m_aligns.push_back( Qt::AlignBottom );

	m_names.push_back( QString::fromUtf8("Align Vertical Middle"));
	m_icons.push_back( QIcon(QString::fromUtf8(":/res/icons/alignmiddle_16x16.png")) );
	m_aligns.push_back( Qt::AlignVCenter );

	m_default = Qt::AlignLeft;
}

void CUiAlignmentFlagPicker::setupMenu()
{
	int				i = 0;
	QAction			*act = NULL;

	for (i = 0; i < m_names.length(); ++i)
	{
		act = addAction(m_icons[i], m_names[i]);
		m_actions.push_back(act);
	}
}

void CUiAlignmentFlagPicker::onActionTriggered(QAction* act)
{
	int				i = 0;
	for (i = 0; i < m_actions.length(); ++i)
	{
		if (act == m_actions[i])
			break;
	}

	if (i >= 0 && i < m_actions.length())
	{
		m_default = m_aligns[i];
		emit alignFlagSelected( m_aligns[i], m_icons[i] );
	}
}
