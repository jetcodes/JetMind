/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CGlobal>
#include <core/CUtils>
#include <core/CDiagram>
#include <core/CDiagramText>
#include <core/CDiagramItem>
#include <core/CDiagramWidget>
#include <widgets/CUiColorPanel>

#include "../CMindMapNode.h"
#include "../CMindMapCommands.h"
#include "CMindMapDtTextTool.h"
#include "ui_CMindMapDtTextTool.h"

extern QList<CMindMapNode*> getAllSelectedNodes(CDiagram *d);

CMindMapDtTextTool::CMindMapDtTextTool(QWidget *parent)
	: CDiagramTool(parent),
	  m_textColors(NULL)
{
	int					size;
	QFontDatabase		db;
	QButtonGroup		*group = NULL;

	ui = new Ui::CMindMapDtTextTool();
	ui->setupUi(this);

	foreach (size,db.standardSizes())
	{
		ui->m_cbFontSize->addItem( QString::number(size) );
	}

    m_textColors = new CUiColorPanel(NULL);
	m_textColors->setVisible(false);

    ui->m_cbFontFamily->setEditable(true);

    ui->m_btnAlignLeft->hide();
	ui->m_btnAlignCenter->hide();
	ui->m_btnAlignRight->hide();
	ui->m_btnListStyle->hide();

	connect(m_textColors, SIGNAL(colorSelected(QColor)), this, SLOT(onTextColorSelected(QColor)));
	connect(ui->m_btnTextColor, SIGNAL(clicked()), this, SLOT(onTextColor()));
	connect(ui->m_cbFontFamily, SIGNAL(activated(int)), this, SLOT(onFontFamilySelected(int)));
	connect(ui->m_cbFontSize, SIGNAL(activated(const QString &)), this, SLOT(onFontSizeSelected(const QString&)));
	connect(ui->m_btnListStyle, SIGNAL(clicked()), this, SLOT(onListStyle()));
	connect(ui->m_btnFontBold, SIGNAL(clicked()), this, SLOT(onFontBold()));
	connect(ui->m_btnFontItalic, SIGNAL(clicked()), this, SLOT(onFontItalic()));
	connect(ui->m_btnFontUnderline, SIGNAL(clicked()), this, SLOT(onFontUnderline()));
	connect(ui->m_btnAlignLeft, SIGNAL(clicked()), this, SLOT(onTextAlignLeft()));
	connect(ui->m_btnAlignCenter, SIGNAL(clicked()), this, SLOT(onTextAlignCenter()));
	connect(ui->m_btnAlignRight, SIGNAL(clicked()), this, SLOT(onTextAlignRight()));
	
	group = new QButtonGroup(this);
	group->setExclusive(true);
	group->addButton(ui->m_btnAlignCenter);
	group->addButton(ui->m_btnAlignLeft);
	group->addButton(ui->m_btnAlignRight);
}

CMindMapDtTextTool::~CMindMapDtTextTool()
{
	delete ui;
}

bool CMindMapDtTextTool::support(const QList<CDiagramItem *> &items)
{
    bool                        r = false;
    QList<CMindMapNode*>        nodes;

    if (!m_d)
        return r;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() > 0)
        r = true;

    return r;
}

void CMindMapDtTextTool::reset()
{
	QFont					ft;
	CMindMapNode			*node = NULL;
	QList<CDiagramItem*>	items;

	if (!m_d)
		return;

	items = m_d->selectedTopLevelItems();
	if (items.length() != 1)
		return;

	node = dynamic_cast<CMindMapNode*>( items.first() );
	if (!node)
		return;

	ft = node->font();
    ui->m_cbFontFamily->setCurrentFont(ft);
	ui->m_cbFontSize->setCurrentIndex( ui->m_cbFontSize->findText( QString::number(ft.pointSize()) ) );
	ui->m_btnFontBold->setChecked(ft.bold());
	ui->m_btnFontItalic->setChecked(ft.italic());
	ui->m_btnFontUnderline->setChecked(ft.underline());
}

void CMindMapDtTextTool::onListStyle()
{
	
}

void CMindMapDtTextTool::onTextColor()
{
    QPoint      p;

    p = Utils::getPopupPoint(m_d, m_p, ui->m_btnTextColor, m_textColors);
    if (!p.isNull())
    {
        m_textColors->raise();
        m_textColors->show();
        m_textColors->move( p );
    }
}

void CMindMapDtTextTool::onTextColorSelected(QColor c)
{
	// CMindMapNode			*node = NULL;
	QList<CDiagramItem*>	items;
	// QList<CMindMapNode*>	nodes;

	items = m_d->selectedTopLevelItems();
	//foreach( CDiagramItem *item, items)
	//{
	//	node = dynamic_cast<CMindMapNode*>(item);
	//	if (node)
	//		nodes.push_back(node);
	//}

	if (m_d)
		m_d->addUndoCommand(new CMindMapChangePropertyCommand(m_d, items, "textColor", QVariant(c)));
}


void CMindMapDtTextTool::onTextAlignCenter()
{
	// EMPTY
}

void CMindMapDtTextTool::onTextAlignLeft()
{
	// EMPTY
}

void CMindMapDtTextTool::onTextAlignRight()
{
	// EMPTY
}

void CMindMapDtTextTool::onFontFamilySelected(int index)
{
    QFont					font;
	// CMindMapNode			*node = NULL;
	// QList<CMindMapNode*>	nodes;
	QList<CDiagramItem*>	items;

	items = m_d->selectedTopLevelItems();
	/*foreach( CDiagramItem *item, items)
	{
		node = dynamic_cast<CMindMapNode*>(item);
		if (node)
			nodes.push_back(node);
	}*/
	
    font = ui->m_cbFontFamily->currentFont();
	if (m_d)
        m_d->addUndoCommand(new CMindMapChangePropertyCommand(m_d, items, "fontFamily", QVariant(font.family())));
}

void CMindMapDtTextTool::onFontSizeSelected(const QString &size)
{
	/*CMindMapNode			*node = NULL;
	QList<CMindMapNode*>	nodes;*/
	QList<CDiagramItem*>	items;

	items = m_d->selectedTopLevelItems();
	/*foreach( CDiagramItem *item, items)
	{
		node = dynamic_cast<CMindMapNode*>(item);
		if (node)
			nodes.push_back(node);
	}*/

	if (m_d)
		m_d->addUndoCommand(new CMindMapChangePropertyCommand(m_d, items, "fontSize", QVariant(size.toInt())));
}

void CMindMapDtTextTool::onFontBold()
{	
	/*CMindMapNode			*node = NULL;
	QList<CMindMapNode*>	nodes;*/
	QList<CDiagramItem*>	items;

	items = m_d->selectedTopLevelItems();
	/*foreach( CDiagramItem *item, items)
	{
		node = dynamic_cast<CMindMapNode*>(item);
		if (node)
			nodes.push_back(node);
	}*/

	if (m_d)
		m_d->addUndoCommand(new CMindMapChangePropertyCommand(m_d, items, "fontBold", QVariant(ui->m_btnFontBold->isChecked())));
}

void CMindMapDtTextTool::onFontItalic()
{
	//CMindMapNode			*node = NULL;
	//QList<CMindMapNode*>	nodes;
	QList<CDiagramItem*>	items;

	items = m_d->selectedTopLevelItems();
	/*foreach( CDiagramItem *item, items)
	{
		node = dynamic_cast<CMindMapNode*>(item);
		if (node)
			nodes.push_back(node);
	}*/

	if (m_d)
		m_d->addUndoCommand(new CMindMapChangePropertyCommand(m_d, items, "fontItalic", QVariant(ui->m_btnFontItalic->isChecked())));
}

void CMindMapDtTextTool::onFontUnderline()
{
	//CMindMapNode			*node = NULL;
	//QList<CMindMapNode*>	nodes;
	QList<CDiagramItem*>	items;

	items = m_d->selectedTopLevelItems();
	/*foreach( CDiagramItem *item, items)
	{
		node = dynamic_cast<CMindMapNode*>(item);
		if (node)
			nodes.push_back(node);
	}*/

	if (m_d)
		m_d->addUndoCommand(new CMindMapChangePropertyCommand(m_d, items, "fontUnderline", QVariant(ui->m_btnFontUnderline->isChecked())));
}
