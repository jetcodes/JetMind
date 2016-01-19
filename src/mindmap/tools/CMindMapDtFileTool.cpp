/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CDiagramItem>
#include <core/CDiagramWidget>
#include <core/CDiagramCommands>

#include "../CMindMapNode.h"
#include "../CMindMapUiItem.h"
#include "../CMindMapCommands.h"
#include "../CMindMapNodeSprite.h"
#include "CMindMapDtFileTool.h"
#include "ui_CMindMapDtFileTool.h"

extern QList<CMindMapNode*> getAllSelectedNodes(CDiagram *d);

CMindMapFileSpriteMenu::CMindMapFileSpriteMenu(QWidget *parent)
    : QMenu(parent),
      m_actAttachFile(NULL),
      m_actDeleteAttachedFile(NULL),
      m_actRemoveAttach(NULL)
{
    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onFileClicked(QAction*)));
}

CMindMapFileSpriteMenu::~CMindMapFileSpriteMenu()
{
}

void CMindMapFileSpriteMenu::reset(CMindMapNodeSprite *sprite)
{
    QAction                 *action = NULL;
    QList<QString>          suffixKeys;
    QMap<QString, QIcon>    suffixMaps;
    QStringList             files;
    CMindMapFileSprite      *fileSprite = NULL;

    fileSprite = dynamic_cast<CMindMapFileSprite*>(sprite);
    if (!fileSprite)
        return;
    files = fileSprite->files();

    suffixMaps["ppt pptx"]  = QIcon(":/mindstorm/icons/ui-document-powerpoint.png");
    suffixMaps["doc docx"] = QIcon(":/mindstorm/icons/ui-document-word.png");
    suffixMaps["flv fl as"]  = QIcon(":/mindstorm/icons/ui-document-flash.png");
    suffixMaps["tex"]  = QIcon(":/mindstorm/icons/ui-document-tex.png");
    suffixMaps["pdf"]  = QIcon(":/mindstorm/icons/ui-document-pdf.png");
    suffixMaps["png jpg jpeg bmp ps"]  = QIcon(":/mindstorm/icons/ui-document-picture.png");
    suffixMaps["xls xlsx"] = QIcon(":/mindstorm/icons/ui-document-excel.png");
    suffixMaps["ps"] = QIcon(":/mindstorm/icons/ui-document-photoshop.png");
    suffixMaps["avi mpeg mp4 wmv rm rmvb"] = QIcon(":/mindstorm/icons/ui-document-film.png");
    suffixMaps["mp3"] = QIcon(":/mindstorm/icons/ui-document-music.png");

    clear();
    suffixKeys = suffixMaps.keys();
    foreach (QString file, files)
    {
        QIcon           icon(":/mindstorm/icons/ui-document-stack.png");
        QString         suffix;
        QFileInfo       info(file);

        suffix = info.suffix().toLower();
        foreach (QString key, suffixKeys)
        {
            if (key.contains(suffix))
            {
                icon = suffixMaps[key];
            }
        }

        action = addAction( icon, info.fileName() );
        action->setCheckable(true);
        action->setChecked(false);
        action->setData( QVariant(file) );
    }

    addSeparator();

    if (!m_actAttachFile)
        m_actAttachFile = new QAction(QIcon(":/mindstorm/icons/ui-attach.png"), tr("Attach File..."), NULL);
    addAction(m_actAttachFile);

    if (!m_actDeleteAttachedFile)
        m_actDeleteAttachedFile = new QAction(QIcon(":/mindstorm/icons/ui-document-delete.png"), tr("Remove Attached File"), NULL);
    addAction(m_actDeleteAttachedFile);

    if (!m_actRemoveAttach)
        m_actRemoveAttach = new QAction(QIcon(":/mindstorm/icons/ui-delete.png"), tr("Drop Attach"), NULL);
    addAction(m_actRemoveAttach);

    CMindMapNodeSpriteMenu::reset(sprite);
}

void CMindMapFileSpriteMenu::onFileClicked(QAction *act)
{
    QVariant            data;
    QString             filePath;

    if (act == m_actAttachFile)
    {
        emit attachFile();
    }
    else if (act == m_actDeleteAttachedFile)
    {
        emit deleteAttachedFile();
    }
    else if (act == m_actRemoveAttach)
    {
        emit removeAttach();
    }
    else
    {
        data = act->data();
        filePath = data.value<QString>();
        emit viewFile(filePath);
    }
}

CMindMapDtFileTool::CMindMapDtFileTool(QWidget *parent)
    : CDiagramTool(parent),
      m_fileSpriteMenu(NULL),
      m_fileDropper(NULL)
{
    ui = new Ui::CMindMapDtFileTool();
    ui->setupUi(this);

    m_fileSpriteMenu = new CMindMapFileSpriteMenu(NULL);
    m_fileDropper = new CMindMapUiFileDropper();

    // register the menu to the manager
    CMindMapMenuManager::instance()->setNodeSpriteMenu(CMindMapNode::File, m_fileSpriteMenu);

    connect(m_fileSpriteMenu, SIGNAL(attachFile()), this, SLOT(onAttachFile()));
    connect(m_fileSpriteMenu, SIGNAL(deleteAttachedFile()), this, SLOT(onShowFileDropper()));
    connect(m_fileSpriteMenu, SIGNAL(removeAttach()), this, SLOT(onRemoveAttach()));
    connect(m_fileSpriteMenu, SIGNAL(viewFile(QString)), this, SLOT(onViewFile(QString)));
    connect(m_fileDropper, SIGNAL(dropFiles(QStringList)), this, SLOT(onDeleteAttachedFiles(QStringList)));
    connect(m_fileDropper, SIGNAL(removeAttach()), this, SLOT(onRemoveAttach()));
    connect(ui->m_btnAttachFile, SIGNAL(clicked()), this, SLOT(onAttachFile()));
    connect(ui->m_btnDeleteAttachedFile, SIGNAL(clicked()), this, SLOT(onShowFileDropper()));
    connect(ui->m_btnRemoveAttach, SIGNAL(clicked()), this, SLOT(onRemoveAttach()));
}

CMindMapDtFileTool::~CMindMapDtFileTool()
{
    delete ui;
}

bool CMindMapDtFileTool::support(const QList<CDiagramItem *> &items)
{
    int             count = 0;
    bool            r = false;
    CDiagramItem    *item = NULL;

    if (!m_d)
        return r;

    foreach (item, items)
    {
        if (dynamic_cast<CMindMapNode*>(item))
        {
            ++count;
            if (count > 1)
            {
                break;
            }
        }
    }

    r = (count == 1);

    return r;
}

void CMindMapDtFileTool::reset()
{
    bool                            deleteEnabled = false, dropEnabled = false;
    CMindMapNode                    *node = NULL;
    CMindMapFileSprite              *fileSprite = NULL;
    QList<CMindMapNode*>            nodes;
    QList<CMindMapNodeSprite*>      sprites;

    if (!m_d)
        return;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() == 1)
    {
        node = nodes.first();
        if (node)
        {
            sprites = node->sprites( CMindMapNode::File );
            if (sprites.length() == 1)
            {
                fileSprite = dynamic_cast<CMindMapFileSprite*>(sprites.first());
                if (fileSprite)
                    dropEnabled = (fileSprite->files().length() > 0);
                deleteEnabled = true;
            }
        }
    }

    ui->m_btnDeleteAttachedFile->setVisible(deleteEnabled);
    ui->m_btnRemoveAttach->setVisible(dropEnabled);
}

void CMindMapDtFileTool::onDeleteAttachedFiles(const QStringList &files)
{
    QList<CMindMapNode*>    nodes;

    if (!m_d)
        return;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() == 1)
    {
        m_d->addUndoCommand( new CMindMapRemoveSpriteCommand(m_d,
                                                        nodes,
                                                        CMindMapNode::File,
                                                        false,
                                                        QVariant(files)) );
    }
}

void CMindMapDtFileTool::onAttachFile()
{
    QString                 filePath;
    QList<CMindMapNode*>    nodes;

    if (!m_d)
        return;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() == 1)
    {
        filePath = QFileDialog::getOpenFileName(NULL,
                                                tr("Choose file"),
                                                QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation),
                                                tr("All Files (*.*)") );
        if (!filePath.isNull() && !filePath.isEmpty())
        {
            m_d->addUndoCommand( new CMindMapAddSpriteCommand(m_d,
                                                            nodes,
                                                            CMindMapNode::File,
                                                            QVariant(filePath)) );
			ui->m_btnDeleteAttachedFile->setVisible(true);
			ui->m_btnRemoveAttach->setVisible(true);
        }
    }
}

void CMindMapDtFileTool::onShowFileDropper()
{
    if (m_d && m_fileDropper && m_fileDropper->reset(m_d))
    {
        m_fileDropper->show();
    }
}

void CMindMapDtFileTool::onRemoveAttach()
{
    QList<CMindMapNode*>    nodes;

    if (!m_d)
        return;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() == 1)
    {
        m_d->addUndoCommand( new CMindMapRemoveSpriteCommand(m_d,
                                                             nodes,
                                                             CMindMapNode::File,
                                                             true));
        ui->m_btnDeleteAttachedFile->setVisible(false);
        ui->m_btnRemoveAttach->setVisible(false);
    }
}

void CMindMapDtFileTool::onViewFile(QString filePath)
{
    QUrl        url;

    url = QUrl::fromLocalFile( filePath );
    QDesktopServices::openUrl( url );
}
