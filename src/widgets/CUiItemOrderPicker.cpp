/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include "CUiItemOrderPicker.h"

CUiItemOrderPicker::CUiItemOrderPicker(QWidget *parent)
	: QMenu(parent),
	  m_defaultOrder(0)
{
	setupMoves();
	setupMenu();

	connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onActionTriggered(QAction*)));
}

CUiItemOrderPicker::~CUiItemOrderPicker()
{

}

QIcon CUiItemOrderPicker::getDefaultIcon()
{
	int		order;
	QIcon	icon;

	order = m_defaultOrder;
	if (order >= 0 && order < m_icons.length())
		icon = m_icons.at(order);
	return icon;
}

void CUiItemOrderPicker::setupMoves()
{
	m_names.push_back( QString::fromUtf8("Move Forwards") );
	m_icons.push_back( QIcon( QString::fromUtf8(":/res/icons/forward_16x16.png") ) );

	m_names.push_back( QString::fromUtf8("Move Backwards") );
	m_icons.push_back( QIcon( QString::fromUtf8(":/res/icons/backward_16x16.png") ) );

	m_names.push_back( QString::fromUtf8("Move to Front") );
	m_icons.push_back( QIcon( QString::fromUtf8(":/res/icons/movetofront_16x16.png") ) );

	m_names.push_back( QString::fromUtf8("Move to Back") );
	m_icons.push_back( QIcon( QString::fromUtf8(":/res/icons/movetoback_16x16.png") ) );
}

void CUiItemOrderPicker::setupMenu()
{
	int				i = 0;
	QAction			*act = NULL;

	for (i = 0; i < m_names.length(); ++i)
	{
		act = addAction(m_icons[i], m_names[i]);
		m_actions.push_back(act);
	}
}

void CUiItemOrderPicker::onActionTriggered(QAction* act)
{
	int				i = 0;
	for (i = 0; i < m_actions.length(); ++i)
	{
		if (act == m_actions[i])
			break;
	}

	switch(i)
	{
	case 0: emit bringForwards();  break;
	case 1: emit bringBackwards(); break;
	case 2: emit sendToFront();	   break;
	case 3: emit sendToBack();	   break;
	default: break;
	};

	m_defaultOrder = i;
}

void CUiItemOrderPicker::emitDefaultOrder()
{
	switch(m_defaultOrder)
	{
	case 0: emit bringForwards();  break;
	case 1: emit bringBackwards(); break;
	case 2: emit sendToFront();	   break;
	case 3: emit sendToBack();	   break;
	default: break;
	};
}