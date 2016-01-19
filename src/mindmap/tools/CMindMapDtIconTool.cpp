/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CGlobal>
#include <core/CUtils>
#include <core/CDiagram>
#include <core/CDiagramWidget>

#include "../CMindMapNode.h"
#include "../CMindMapUiItem.h"
#include "../CMindMapCommands.h"
#include "../CMindMapNodeSprite.h"
#include "../CMindMapIconProvider.h"
#include "../CMindMapMenuManager.h"
#include "CMindMapDtIconTool.h"
#include "ui_CMindMapDtIconTool.h"

extern QList<CMindMapNode*> getAllSelectedNodes(CDiagram *d);

CMindMapIconSpriteMenu::CMindMapIconSpriteMenu(QWidget *parent)
	: CMindMapUiIconAlterList(parent)
{
	
}

CMindMapIconSpriteMenu::~CMindMapIconSpriteMenu()
{
}

void CMindMapIconSpriteMenu::reset(CMindMapNodeSprite *sprite)
{
	int						index = -1;
	QList<QIcon>			icons;
	CMindMapIconSprite		*iconSprite = NULL;

	iconSprite = dynamic_cast<CMindMapIconSprite*>(sprite);
	if (iconSprite)
	{
		clear();
		index = iconSprite->iconIndex();
		setAlterIconIndex(index);

		icons = CMindMapIconProvider::instance()->getAllAlterIcons(index);
        setIcons(icons, index);
	}

    CMindMapNodeSpriteMenu::reset(sprite);
}

CMindMapPixmapSpriteMenu::CMindMapPixmapSpriteMenu(QWidget *parent)
    : QMenu(parent)
{

}

CMindMapPixmapSpriteMenu::~CMindMapPixmapSpriteMenu()
{

}

void CMindMapPixmapSpriteMenu::reset(CMindMapNodeSprite *sprite)
{
    QAction                     *act = NULL;
    CMindMapPixmapSprite		*pixmapSprite = NULL;

    pixmapSprite = dynamic_cast<CMindMapPixmapSprite*>( sprite );
    if (!pixmapSprite)
        return;

    clear();

    act = addAction(tr("Restore Size"));
    connect(act, SIGNAL(triggered()), this, SIGNAL(restorePixmapSize()));

    act = addAction(tr("Replace with..."));
    connect(act, SIGNAL(triggered()), this, SIGNAL(alterPixmap()));

    addSeparator();

    act = addAction(QIcon(":/mindstorm/icons/ui-delete.png"), tr("Delete Pixmap"));
    connect(act, SIGNAL(triggered()), this, SIGNAL(deletePixmap()));

    CMindMapNodeSpriteMenu::reset(sprite);
}

CMindMapDtIconTool::CMindMapDtIconTool(QWidget *parent)
	: CDiagramTool(parent),
	  m_iconList(NULL),
      m_iconBrowser(NULL),
	  m_pixmapList(NULL),
      m_iconSpriteMenu(NULL),
      m_pixmapSpriteMenu(NULL)
{
	ui = new Ui::CMindMapDtIconTool();
	ui->setupUi(this);

    m_iconList = new CMindMapUiIconList(NULL);
	m_pixmapList = new CMindMapUiPixmapList(NULL);
	m_iconSpriteMenu = new CMindMapIconSpriteMenu(NULL);
    m_pixmapSpriteMenu = new CMindMapPixmapSpriteMenu(NULL);

    m_iconList->setIconText("");

	// register the menu to the manager
	CMindMapMenuManager::instance()->setNodeSpriteMenu(CMindMapNode::Icon, m_iconSpriteMenu);
    CMindMapMenuManager::instance()->setNodeSpriteMenu(CMindMapNode::Pixmap, m_pixmapSpriteMenu);

    connect(ui->m_btnSelectIcon, SIGNAL(clicked()), this, SLOT(onShowIconList()));
    connect(m_iconList, SIGNAL(iconSelected(const QIcon&, int)), this, SLOT(onIconSelected(const QIcon&, int)));
    connect(m_iconList, SIGNAL(browseIconLibrary()), this, SLOT(onBrowseIconLibrary()));
    connect(ui->m_btnSelectPixmap, SIGNAL(clicked()), this, SLOT(onBrowsePixmap()));
    connect(ui->m_btnDeleteAllIcons, SIGNAL(clicked()), this, SLOT(onDeleteAllIcons()));
    connect(ui->m_btnDeletePixmap, SIGNAL(clicked()), this, SLOT(onDeletePixmap()));
	//connect(m_pixmapList, SIGNAL(pixmapSelected(const QPixmap&)), this, SLOT(onPixmapSelected(const QPixmap&)));
    //connect(m_pixmapList, SIGNAL(browsePixmap()), this, SLOT(onBrowsePixmap()));
    connect(m_iconSpriteMenu, SIGNAL(iconSelected(const QIcon&, int)), this, SLOT(onIconSelected(const QIcon&, int)));
	connect(m_iconSpriteMenu, SIGNAL(deleteIcon(int)), this, SLOT(onDeleteIcon(int)));
    connect(m_pixmapSpriteMenu, SIGNAL(restorePixmapSize()), this, SLOT(onRestorePixmapSize()));
    connect(m_pixmapSpriteMenu, SIGNAL(alterPixmap()), this, SLOT(onBrowsePixmap()));
    connect(m_pixmapSpriteMenu, SIGNAL(deletePixmap()), this, SLOT(onDeletePixmap()));
}

CMindMapDtIconTool::~CMindMapDtIconTool()
{
	SAFE_DELETE(ui);
	SAFE_DELETE(m_pixmapList);
	SAFE_DELETE(m_iconList);
	SAFE_DELETE(m_iconSpriteMenu);
    SAFE_DELETE(m_pixmapSpriteMenu);
}

bool CMindMapDtIconTool::support(const QList<CDiagramItem *> &items)
{
    bool                r = false;
    CDiagramItem        *item = NULL;
    CMindMapNode        *node = NULL;

    if (!m_d)
        return r;

    foreach (item, items)
    {
        node = dynamic_cast<CMindMapNode*>(item);
        if (node)
        {
            r = true;
            break;
        }
    }

    return r;
}

void CMindMapDtIconTool::reset()
{
    bool                            deleteIconEnabled = false;
    bool                            deletePixmapEnabled = false;
    CMindMapNode                    *node = NULL;
    QList<CMindMapNode*>            nodes;

    if (!m_d)
        return;

    nodes = getAllSelectedNodes(m_d);
    foreach (node, nodes)
    {
        if (!deleteIconEnabled && node->sprites( CMindMapNode::Icon ).length() > 0)
            deleteIconEnabled = true;
        if (!deletePixmapEnabled && node->sprites( CMindMapNode::Pixmap).length() > 0)
            deletePixmapEnabled = true;

        if (deletePixmapEnabled && deleteIconEnabled)
            break;
    }

    ui->m_btnDeleteAllIcons->setVisible(deleteIconEnabled);
    ui->m_btnDeletePixmap->setVisible(deletePixmapEnabled);
}

void CMindMapDtIconTool::onShowIconList()
{
    QPoint      p;

    if (!m_d || !m_p)
        return;

    p = Utils::getPopupPoint(m_d, m_p, ui->m_btnSelectIcon, (QWidget*)m_iconList);
    if (!p.isNull())
        m_iconList->exec( p );
}

void CMindMapDtIconTool::onBrowseIconLibrary()
{
    if (!m_iconBrowser)
    {
        m_iconBrowser = new CMindMapUiIconBrowser(NULL);
        connect(m_iconBrowser, SIGNAL(iconSelected(QIcon,int)), this, SLOT(onIconSelected(QIcon,int)));
    }

    m_iconBrowser->show();
}

void CMindMapDtIconTool::onIconSelected(const QIcon &icon, int index)
{
	CMindMapNode			*node = NULL;
	QList<CDiagramItem*>	items;
	QList<CMindMapNode*>	nodes;

    if (!m_d)
		return;

	items = m_d->selectedTopLevelItems();
	foreach (CDiagramItem *item, items)
	{
		node = dynamic_cast<CMindMapNode*>(item);
		if (node)
			nodes.push_back(node);
	}

    if (nodes.length() > 0)
    {
        m_d->addUndoCommand( new CMindMapAddSpriteCommand(m_d,
                                                          nodes,
                                                          CMindMapNode::Icon,
                                                          QVariant(icon),
                                                          QVariant(index)) );
        ui->m_btnDeleteAllIcons->setVisible(true);
    }
}

void CMindMapDtIconTool::onDeleteAllIcons()
{
    QList<CMindMapNode*>        nodes;

    if (!m_d)
        return;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() > 0)
    {
        m_d->addUndoCommand( new CMindMapRemoveSpriteCommand(m_d,
                                                             nodes,
                                                             CMindMapNode::Icon,
                                                             true) );
        ui->m_btnDeleteAllIcons->setVisible(false);
    }
}

void CMindMapDtIconTool::onDeleteIcon(int value)
{
    QList<CMindMapNode*>        nodes;

    if (!m_d)
        return;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() == 1)
    {
        m_d->addUndoCommand( new CMindMapRemoveSpriteCommand(m_d,
                                                             nodes,
                                                             CMindMapNode::Icon,
                                                             false,
                                                             QVariant(value)) );
        ui->m_btnDeleteAllIcons->setVisible(false);
    }
}

void CMindMapDtIconTool::onShowPixmapList()
{
	QPoint      p;

    if (!m_d || !m_p)
        return;

	p = Utils::getPopupPoint(m_d, m_p, ui->m_btnSelectPixmap, (QWidget*)m_pixmapList);
    if (!p.isNull())
        m_pixmapList->exec( p );
}

void CMindMapDtIconTool::onPixmapSelected(const QString &filePath)
{
	CMindMapNode			*node = NULL;
	QList<CDiagramItem*>	items;
	QList<CMindMapNode*>	nodes;

	if (!m_d)
		return;

	items = m_d->selectedTopLevelItems();
	foreach (CDiagramItem *item, items)
	{
		node = dynamic_cast<CMindMapNode*>(item);
		if (node)
			nodes.push_back(node);
	}

    if (nodes.length() > 0)
    {
        m_d->addUndoCommand( new CMindMapAddSpriteCommand(m_d,
                                                          nodes,
                                                          CMindMapNode::Pixmap,
                                                          QVariant(filePath)) );
        ui->m_btnDeletePixmap->setVisible(true);
    }
}

void CMindMapDtIconTool::onBrowsePixmap()
{
	QString			filePath;
    QStringList		filters;
    QFileInfo       info;

    filters << tr("Image Files (*.png *.PNG *.jpg *.jpeg *.JPEG *.JPG *.BMP *.bmp)")
            << tr("All Files (*.*)");

    filePath = QFileDialog::getOpenFileName(NULL,
                                            tr("Choose image"),
                                            QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation),
                                            filters.join(tr(";;")) );

    if (!filePath.isNull() && !filePath.isEmpty())
    {
        info.setFile(filePath);
        if (info.exists())
            onPixmapSelected(filePath);
    }
}

void CMindMapDtIconTool::onDeletePixmap()
{
    QList<CMindMapNode*>        nodes;

    if (!m_d)
        return;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() > 0)
    {
        m_d->addUndoCommand( new CMindMapRemoveSpriteCommand(m_d,
                                                             nodes,
                                                             CMindMapNode::Pixmap) );
        ui->m_btnDeletePixmap->setVisible(false);
    }
}

void CMindMapDtIconTool::onRestorePixmapSize()
{

}
