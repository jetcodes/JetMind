/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramFile>
#include <core/CDiagramFileManager>
#include <core/CDiagramIconProvider>
#include <core/CDiagramContextMenuHandler>

#include "CMindMapNode.h"
#include "CMindMapFile.h"
#include "CMindMapUiItem.h"
#include "CMindMapNodeSprite.h"
#include "CMindMapIconProvider.h"
#include "CMindMapCategory.h"
#include "ui_CMindMapUiIconBrowser.h"
#include "ui_CMindMapUiLinkEditor.h"
#include "ui_CMindMapUiNoteEditor.h"
#include "ui_CMindMapUiTaskEditor.h"
#include "ui_CMindMapUiDateEditor.h"
#include "ui_CMindMapUiFileDropper.h"

extern QList<CMindMapNode*> getAllSelectedNodes(CDiagram *d);

CMindMapUiShapeList::CMindMapUiShapeList(QWidget *parent)
	: QMenu(parent)
{
	setupMenus();
	connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onActionTriggered(QAction*)));
}

CMindMapUiShapeList::~CMindMapUiShapeList()
{
}

void CMindMapUiShapeList::setupMenus()
{
	QAction					*action = NULL;
	QList<QIcon>			icons;
    QList<int>              filtered;
	QStringList				labels;
	QPainterPath			path;
	CDiagramIconProvider	*provider = NULL;
	
	labels	<< tr("Rect") << tr("Rounded Rect")
            << tr("Ellipse") << tr("Diamond") << tr("Underline")
            << tr("Left Fish Head") << tr("Right Fish Head")
            << tr("None");
	
	provider = CDiagramIconProvider::instance();

	// rect
	path = QPainterPath();
	path.addRect( -6, -4, 12, 8 );
	icons << provider->genIconForPath(path);

	// rounded rect
	path = QPainterPath();
	path.addRoundedRect(-6, -4, 12, 8, 3, 3);
	icons << provider->genIconForPath(path);

	// ellipse
	path = QPainterPath();
	path.addEllipse( -6, -4, 12, 8 );
	icons << provider->genIconForPath(path);

	// diamond
	path = QPainterPath();
	path.moveTo( 0, -6 );
	path.lineTo( 6, 0 );
	path.lineTo( 0, 6 );
	path.lineTo( -6, 0 );
	path.lineTo( 0, -6 );
	icons << provider->genIconForPath(path);

	// underline
	path = QPainterPath();
	path.moveTo( -6, 6 );
	path.lineTo( 6, 6 );
	icons << provider->genIconForPath(path);

	// fish head left
	path = QPainterPath();
	path.moveTo(-6, 0);
	path.lineTo(6, -6);
	path.lineTo(6, 6);
	path.lineTo(-6, 0);
	icons << provider->genIconForPath(path);

	// fish head right
	path = QPainterPath();
	path.moveTo(6, 0);
	path.lineTo(-6, 6);
	path.lineTo(-6, -6);
	path.lineTo(6, 0);
	icons << provider->genIconForPath(path);

	// borderless
	icons << QIcon();

    // only some of the shape types are added to the
    // menu
    filtered << 0 << 1 << 2 << 3 << 4 << 7;
    for (int i = 0; i < filtered.length(); ++i)
    {
        int index = filtered.at(i);
        action = addAction(icons.at(index), labels.at(index));
        action->setData( QVariant(index) );
        m_actions.push_back(action);
    }
}

void CMindMapUiShapeList::onActionTriggered(QAction *act)
{
	int				i = 0;

	for (i = 0; i < m_actions.length(); ++i)
	{
		if (act == m_actions[i])
			break;
	}

    emit shapeSelected( act->data().toInt() );
}

CMindMapUiLayoutList::CMindMapUiLayoutList(QWidget *parent)
	: QMenu(parent)
{
	connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onActionTriggered(QAction*)));
}

CMindMapUiLayoutList::~CMindMapUiLayoutList()
{
}

void CMindMapUiLayoutList::reset(bool root)
{
	m_layoutTypes.clear();
	if (root)
	{
		m_layoutTypes << CMindMapNode::MindMap
					  << CMindMapNode::ClockwiseMindMap
					  << CMindMapNode::AntiClockwiseMindMap;
	}

	m_layoutTypes << CMindMapNode::DownsideOrganization
				  << CMindMapNode::UpsideOrganization
				  << CMindMapNode::LeftTree
				  << CMindMapNode::RightTree
				  << CMindMapNode::LeftLogic
				  << CMindMapNode::RightLogic;

	setupMenus();
}

void CMindMapUiLayoutList::setupMenus()
{
	QAction					*action = NULL;
	QMap<int, QIcon>		icons;
	QMap<int, QString>		labels;
	
	labels[CMindMapNode::MindMap] = tr("MindMap");
	labels[CMindMapNode::ClockwiseMindMap] = tr("MindMap (Clockwise)");
	labels[CMindMapNode::AntiClockwiseMindMap] = tr("MindMap (AntiClockwise)");
	labels[CMindMapNode::DownsideOrganization] = tr("Organization (Downside)");
	labels[CMindMapNode::UpsideOrganization] = tr("Organization (Upside)");
	labels[CMindMapNode::LeftTree] = tr("Tree (Left)");
	labels[CMindMapNode::RightTree] = tr("Tree (Right)");
	labels[CMindMapNode::LeftLogic] = tr("Logic (Left)");
	labels[CMindMapNode::RightLogic] = tr("Logic (Right)");
	
    icons[CMindMapNode::MindMap] = QIcon(QString::fromUtf8(":/res/icons/node-layout-mindmap-doubleflow.png"));
    icons[CMindMapNode::ClockwiseMindMap] = QIcon(QString::fromUtf8(":/res/icons/node-layout-mindmap-clockwise.png"));
    icons[CMindMapNode::AntiClockwiseMindMap] = QIcon(QString::fromUtf8(":/res/icons/node-layout-mindmap-anticlockwise.png"));
    icons[CMindMapNode::DownsideOrganization] = QIcon(QString::fromUtf8(":/res/icons/node-layout-organization-down.png"));
    icons[CMindMapNode::UpsideOrganization] = QIcon(QString::fromUtf8(":/res/icons/node-layout-organization-up.png"));
    icons[CMindMapNode::LeftTree] = QIcon(QString::fromUtf8(":/res/icons/node-layout-tree-left.png"));
    icons[CMindMapNode::RightTree] = QIcon(QString::fromUtf8(":/res/icons/node-layout-tree-right.png"));
    icons[CMindMapNode::LeftLogic] = QIcon(QString::fromUtf8(":/res/icons/node-layout-logic-left.png"));
    icons[CMindMapNode::RightLogic] = QIcon(QString::fromUtf8(":/res/icons/node-layout-logic-right.png"));

	clear();
	m_actions.clear();

	// loop to create actions
	foreach (int i, m_layoutTypes)
	{
		action = addAction(icons[i], labels[i]);
		m_actions.push_back(action);
	}
}

void CMindMapUiLayoutList::onActionTriggered(QAction *act)
{
	int				i = 0;

	for (i = 0; i < m_actions.length(); ++i)
	{
		if (act == m_actions[i])
			break;
	}

	emit layoutSelected( m_layoutTypes[i] );
}

CMindMapUiIconList::CMindMapUiIconList(QWidget *parent, int cols, int rows, int w, int h)
	: QMenu(parent),
	  m_columns(cols),
	  m_rows(rows),
	  m_gridWidth(w),
	  m_gridHeight(h),
      m_iconTextGrids(4),
	  m_space(4),
	  m_currIconIndex(-1),
	  m_radius(4),
	  m_defaultIcon( QIcon() ),
      m_iconEndActionText(tr("From File...")),
      m_extraActionText(tr("Browse Icon Library")),
      m_minWidth(168),
      m_defaultColumns(6)
{
	loadIcons();
	setupMenus();
	setMouseTracking(true);
}

CMindMapUiIconList::~CMindMapUiIconList()
{
}

void CMindMapUiIconList::loadIcons()
{
	int             maxIcons = 48;
    QList<QIcon>    icons;

    // first, clear things up
    m_icons.clear();

    // add icons(maximum 48)
    icons = CMindMapIconProvider::instance()->getAllIcons();
    for (int i = 0; i < maxIcons; ++i)
        m_icons << icons.at(i);

    if (!m_iconEndActionText.trimmed().isEmpty())
        m_icons << QIcon();
}

void CMindMapUiIconList::setupMenus()
{
    m_columns = m_icons.length() < m_columns ? m_icons.length() : m_columns;
    if (m_columns < m_defaultColumns)
        m_columns = m_defaultColumns;
    m_rows = (m_icons.length() + m_columns - 1) / m_columns + 1;

    // Resize the widget to reasonable size
    resize(qMax(m_columns * m_gridWidth + 2 * m_space, m_minWidth), m_rows * m_gridHeight + 2 * m_space );
}

void CMindMapUiIconList::setIconText(const QString &value)
{
    m_iconEndActionText = value;
    if (value.isNull() || value.trimmed().isEmpty())
    {
        if (m_icons.length() > 0 && m_icons.last().isNull())
        {
            m_icons.removeLast();
            setupMenus();
        }
    }
	else
	{
		if (m_icons.length() == 0 || (m_icons.length() > 0 && !m_icons.last().isNull()))
		{
			m_icons << QIcon();
			setupMenus();
		}
	}

    update();
}

QString CMindMapUiIconList::iconText()
{
    return m_iconEndActionText;
}

bool CMindMapUiIconList::hasIconText()
{
    return !( m_iconEndActionText.trimmed().isEmpty() );
}

void CMindMapUiIconList::setExtraActionText(const QString &value)
{
    m_extraActionText = value;
    update();
}

QString CMindMapUiIconList::extraActionText()
{
    return m_extraActionText;
}

void CMindMapUiIconList::paintEvent( QPaintEvent *event )
{
    int			col = 0, row = 0, iconTextGrids = 0;
	QRectF		r;
    QPointF		p;
	QPainter	painter(this);
	
    painter.setFont(font());
	painter.setPen( QPen(Qt::gray, 1, Qt::SolidLine) );
	painter.setBrush( Qt::white );
	painter.drawRect( rect().adjusted(0,0,-1,-1) );

	painter.setPen( QPen(Qt::black, 1, Qt::SolidLine) );
	painter.setRenderHint(QPainter::Antialiasing, true);

    iconTextGrids = hasIconText() ? m_iconTextGrids : 1;

    // draw icons
	for (int i = 0; i < m_icons.length(); ++i)
	{
		row = i / m_columns;
		col = i % m_columns;
		
		r.setTopLeft( QPointF(col * m_gridWidth + m_space, row * m_gridHeight + m_space) );
		r.setSize( QSizeF(m_gridWidth, m_gridHeight) );
		r.adjust(m_space,m_space,-m_space,-m_space);
		if (i == m_icons.length() - 1 && m_icons[i].isNull() )
			painter.drawText(QRectF(r.topLeft(), QSizeF(m_gridWidth * m_iconTextGrids, r.height())), Qt::AlignLeft | Qt::AlignVCenter, m_iconEndActionText);
		else
			m_icons[i].paint(&painter, r.toRect(), Qt::AlignHCenter | Qt::AlignVCenter);
    }

    // if the icons are empty, we don't have to draw the separator line 
    if (m_icons.length() > 0 && !m_icons.first().isNull())
    {
        ++ row;
        r.setTopLeft( QPointF(m_space, row * m_gridHeight + m_space) );
        r.setSize( QSizeF(rect().width() - 2 * m_space, m_gridHeight) );
        painter.setPen( QPen(Qt::lightGray, 1, Qt::SolidLine) );
        painter.drawLine( r.topLeft(), r.topRight() );
    }
    else
    {
        r.setTopLeft( QPointF(m_space, m_space) );
        r.setSize( QSizeF(rect().width() - 2 * m_space, m_gridHeight) );
    }

    painter.setPen( QPen(Qt::black, 1, Qt::SolidLine) );
    painter.drawText(r, Qt::AlignHCenter | Qt::AlignVCenter, m_extraActionText);

    // locate the hovered color
    p = m_mousePos;
    if (rect().contains(p.toPoint()))
	{
        col = qFloor((p.x() - m_space) / m_gridWidth);
        row = qFloor((p.y() - m_space) / m_gridHeight);
		m_currIconIndex = row * m_columns + col;

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen( Qt::NoPen );
        painter.setBrush( QColor(0,0,255, 48) );
        if (m_currIconIndex >= 0 && m_currIconIndex < m_icons.length() + iconTextGrids - 1)
		{
            if (m_icons.last().isNull() && m_currIconIndex >= m_icons.length() - 1)
                painter.drawRoundedRect(m_space + (col - (m_currIconIndex - m_icons.length() + 1)) * m_gridWidth, m_space + row * m_gridHeight, m_iconTextGrids * m_gridWidth, m_gridHeight, m_radius, m_radius );
			else
				painter.drawRoundedRect(m_space + col * m_gridWidth, m_space + row * m_gridHeight, m_gridWidth, m_gridHeight, m_radius, m_radius );
		}
        else
        {
            if (row == m_rows - 1)
                painter.drawRoundedRect(m_space, m_space + row * m_gridHeight, rect().width() - 2 * m_space, m_gridHeight, m_radius, m_radius );
        }
	}
	else
    {
		m_currIconIndex = -1;
    }
}

void CMindMapUiIconList::mousePressEvent( QMouseEvent *event )
{
    int     iconTextGrids = 0;

    iconTextGrids = hasIconText() ? m_iconTextGrids : 1;

	QWidget::mousePressEvent(event);

    if (m_currIconIndex < 0)
    {
        return;
    }
    else if (m_currIconIndex < m_icons.length())
	{
		hide();
		if (m_icons[m_currIconIndex].isNull())
		{
			emit endIconList();
		}
		else
		{
			m_defaultIcon = m_icons[m_currIconIndex];
			emit iconSelected( m_defaultIcon, m_currIconIndex );
		}
	}
    else if (m_currIconIndex < m_icons.length() + m_iconTextGrids - 1)
    {
		hide();
        emit endIconList();
    }
    else
    {
		hide();
        emit browseIconLibrary();
    }
}

void CMindMapUiIconList::mouseMoveEvent( QMouseEvent *event )
{
	m_mousePos = event->pos();
	repaint();
	QWidget::mouseMoveEvent(event);
}

void CMindMapUiIconList::mouseReleaseEvent( QMouseEvent *event)
{
	QWidget::mouseReleaseEvent(event);
}

QSize CMindMapUiIconList::sizeHint() const
{
	return QSize(geometry().size());
}

QSize CMindMapUiIconList::minimumSizeHint() const
{
	return QSize(m_columns * m_gridWidth + 2 * m_space, m_rows * m_gridHeight + 2 * m_space );
}

CMindMapUiIconAlterList::CMindMapUiIconAlterList(QWidget *parent, int cols, int rows, int w, int h)
	: CMindMapUiIconList(parent, cols, rows, w, h),
	  m_startIndex(0),
	  m_alterIconIndex(-1)
{
    m_minWidth = 128;
    m_iconEndActionText = tr("");
	m_extraActionText = tr("Delete Icon");
}

CMindMapUiIconAlterList::~CMindMapUiIconAlterList()
{
}

void CMindMapUiIconAlterList::setAlterIconIndex(int index)
{
	if (index > -1)
		m_alterIconIndex = index;
}

int CMindMapUiIconAlterList::alterIconIndex() const
{
	return m_alterIconIndex;
}

void CMindMapUiIconAlterList::setIcons(QList<QIcon> &icons, int index)
{
	m_icons = icons;
	m_startIndex = index;

	setupMenus();
}

QList<QIcon> CMindMapUiIconAlterList::icons() const
{
	return m_icons;
}

int CMindMapUiIconAlterList::startIndex() const
{
	return m_startIndex;
}

void CMindMapUiIconAlterList::mousePressEvent( QMouseEvent *event )
{
	QWidget::mousePressEvent(event);

    if (m_currIconIndex < 0)
    {
        return;
    }
    else if (m_currIconIndex < m_icons.length())
	{
		hide();
		m_defaultIcon = m_icons[m_currIconIndex];
        emit iconSelected( m_defaultIcon, m_startIndex + m_currIconIndex );
	}
    else
    {
		hide();
        emit deleteIcon(m_alterIconIndex);
    }
}

// Pixmap
CMindMapUiPixmapList::CMindMapUiPixmapList(QWidget *parent, int cols, int rows, int w, int h)
	: QMenu(parent),
	  m_columns(cols),
	  m_rows(rows),
	  m_gridWidth(w),
	  m_gridHeight(h),
	  m_space(4),
	  m_currPixmapIndex(-1),
      m_radius(4),
      m_defaultColumns(6)
{
	setupMenus();
	setMouseTracking(true);
}

CMindMapUiPixmapList::~CMindMapUiPixmapList()
{
}

void CMindMapUiPixmapList::setupMenus()
{
    m_pixmaps = CMindMapIconProvider::instance()->getAllPixmaps();
    m_pixmaps << tr("None");

	m_columns = m_pixmaps.length() < m_columns ? m_pixmaps.length() : m_columns;
    if (m_columns == m_defaultColumns)
        m_columns = m_defaultColumns;
    m_rows = (m_pixmaps.length() + m_columns - 1) / m_columns + 1;

    // Resize the widget to reasonable size
    resize(m_columns * m_gridWidth + 2 * m_space, m_rows * m_gridHeight + 2 * m_space );
}

void CMindMapUiPixmapList::paintEvent( QPaintEvent *event )
{
    int			col = 0, row = 0;
	QRectF		r;
    QPointF		p;
	QPainter	painter(this);
	
	painter.setPen( QPen(Qt::gray, 1, Qt::SolidLine) );
	painter.setBrush( Qt::white );
	painter.drawRect( rect().adjusted(0,0,-1,-1) );

	painter.setPen( QPen(Qt::black, 1, Qt::SolidLine) );
	painter.setRenderHint(QPainter::Antialiasing, true);

    // draw pixmaps
	for (int i = 0; i < m_pixmaps.length(); ++i)
	{
		row = i / m_columns;
		col = i % m_columns;
		
		r.setTopLeft( QPointF(col * m_gridWidth + m_space, row * m_gridHeight + m_space) );
		r.setSize( QSizeF(m_gridWidth, m_gridHeight) );
		r.adjust(m_space,m_space,-m_space,-m_space);
		if (!m_pixmaps[i].isNull())
			QIcon(m_pixmaps[i]).paint(&painter, r.toRect(), Qt::AlignHCenter | Qt::AlignVCenter);
		else
			painter.drawText(QRectF(r.topLeft(), QSizeF(r.width() * 2, r.height())), Qt::AlignHCenter | Qt::AlignVCenter, tr("None"));
	}

    // draw the browse button
    ++ row;
    r.setTopLeft( QPointF(m_space, row * m_gridHeight + m_space) );
    r.setSize( QSizeF(rect().width() - 2 * m_space, m_gridHeight) );
    painter.setPen( QPen(Qt::lightGray, 1, Qt::SolidLine) );
    painter.drawLine( r.topLeft(), r.topRight() );

    painter.setPen( QPen(Qt::black, 1, Qt::SolidLine) );
    painter.drawText(r, Qt::AlignHCenter | Qt::AlignVCenter, tr("Browse Local Pixmap"));

	// locate the hovered color
    p = m_mousePos;
    if (rect().contains(p.toPoint()))
	{
        col = qFloor((p.x() - m_space) / m_gridWidth);
        row = qFloor((p.y() - m_space) / m_gridHeight);
		m_currPixmapIndex = row * m_columns + col;

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen( Qt::NoPen );
        painter.setBrush( QColor(0,0,255, 48) );
		if (m_currPixmapIndex >= 0 && m_currPixmapIndex < m_pixmaps.length())
		{
			if (m_currPixmapIndex == m_pixmaps.length() - 1)
				painter.drawRoundedRect(m_space + col * m_gridWidth, m_space + row * m_gridHeight, 2 * m_gridWidth, m_gridHeight, m_radius, m_radius );
			else
				painter.drawRoundedRect(m_space + col * m_gridWidth, m_space + row * m_gridHeight, m_gridWidth, m_gridHeight, m_radius, m_radius );
		}
        else
        {
            if (row == m_rows - 1)
                painter.drawRoundedRect(m_space, m_space + row * m_gridHeight, rect().width() - 2 * m_space, m_gridHeight, m_radius, m_radius );
        }
	}
	else
    {
		m_currPixmapIndex = -1;
    }
}

void CMindMapUiPixmapList::mousePressEvent( QMouseEvent *event )
{
	QWidget::mousePressEvent(event);

    if (m_currPixmapIndex < 0)
    {
        return;
    }
    else if (m_currPixmapIndex < m_pixmaps.length() - 1)
	{
		hide();
        emit pixmapSelected( QPixmap( m_pixmaps[m_currPixmapIndex] ) );
	}
    else if (m_currPixmapIndex == m_pixmaps.length() - 1)
    {
        emit clearPixmap();
    }
    else
    {
        emit browsePixmap();
    }
}

void CMindMapUiPixmapList::mouseMoveEvent( QMouseEvent *event )
{
	m_mousePos = event->pos();
	repaint();
	QWidget::mouseMoveEvent(event);
}

void CMindMapUiPixmapList::mouseReleaseEvent( QMouseEvent *event)
{
	QWidget::mouseReleaseEvent(event);
}

QSize CMindMapUiPixmapList::sizeHint() const
{
	return QSize(geometry().size());
}

QSize CMindMapUiPixmapList::minimumSizeHint() const
{
	return QSize(m_columns * m_gridWidth + 2 * m_space, m_rows * m_gridHeight + 2 * m_space );
}

// path list
CMindMapUiPathList::CMindMapUiPathList(QWidget *parent)
	: QMenu(parent)
{
	setupMenus();
	connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onActionTriggered(QAction*)));
}

CMindMapUiPathList::~CMindMapUiPathList()
{
}

void CMindMapUiPathList::setupMenus()
{
	QAction					*action = NULL;
	QList<QIcon>			icons;
	QStringList				labels;
	QPainterPath			path;
	QPainterPathStroker		stroker;
	CDiagramIconProvider	*provider = NULL;
	
	labels	<< tr("Straight Line") << tr("Curve Line")
			<< tr("Arrow Line") << tr("Polygon Line") << tr("Rounded Line") 
			<< tr("None");
	
	provider = CDiagramIconProvider::instance();

	// Straight Line
	path = QPainterPath();
	path.moveTo(-6, -6);
	path.lineTo(6, 6);
	icons << provider->genIconForPath( stroker.createStroke(path) );

	// Curve
	path = QPainterPath();
	path.moveTo(-6, -6);
	path.quadTo(QPointF(4, -6), QPointF(6, 6));
	icons << provider->genIconForPath( stroker.createStroke(path) );

	// Arrow Line
	path = QPainterPath();
	path.moveTo(6, 6);
	path.lineTo(-6, -6);
	path.lineTo(-6, -2);
	path.moveTo(-6, -6);
	path.lineTo(-2, -6);
	icons << provider->genIconForPath( stroker.createStroke(path) );

	// Polygon Line
	path = QPainterPath();
	path.moveTo( 6, 6  );
	path.lineTo( 0, 6  );
	path.lineTo( 0, -6 );
	path.lineTo( -6,-6 );
	icons << provider->genIconForPath( stroker.createStroke(path) );

	// Rounded Line
	path = QPainterPath();
	path.moveTo( 6, 6 );
	path.lineTo( 0, 6 );
	path.quadTo( QPointF(0, -6), QPointF(-6, -6) );
	icons << provider->genIconForPath( stroker.createStroke(path) );

	// None
	icons << QIcon();

	// loop to create icons
	for (int i = CMindMapNode::StraightLine; i <= CMindMapNode::NoneLine; ++i)
	{
		action = addAction(icons.at(i), labels.at(i));
		m_actions.push_back(action);
	}
}

void CMindMapUiPathList::onActionTriggered(QAction *act)
{
	int				i = 0;

	for (i = 0; i < m_actions.length(); ++i)
	{
		if (act == m_actions[i])
			break;
	}

	emit pathSelected( i );
}


CMindMapUiNewItemList::CMindMapUiNewItemList(QWidget *parent)
	: QMenu(parent)
{
	setupMenus();
	connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onActionTriggered(QAction*)));
}

CMindMapUiNewItemList::~CMindMapUiNewItemList()
{
}

void CMindMapUiNewItemList::setupMenus()
{
	QAction					*action = NULL;
	QStringList				labels;
	
	m_icons.clear();
	m_actions.clear();

	labels	<< tr("Insert Sibling After (Press Enter)") 
			<< tr("Insert Sibling Before")
			<< tr("Insert Parent") 
			<< tr("Insert Child (Press Tab)");
	
    m_icons << QIcon(":/res/icons/node-insert-previous.png")
            << QIcon(":/res/icons/node-insert-next.png")
            << QIcon(":/res/icons/node-insert-parent.png")
            << QIcon(":/res/icons/node-insert-child.png");

	for (int i = 0; i < CMindMapNode::NoneInsertPosition; ++i)
	{
		action = addAction(m_icons[i], labels[i]);
		if (action)
			m_actions.push_back(action);
	}
}

void CMindMapUiNewItemList::reset(bool isRoot)
{
	if (isRoot)
	{
		m_actions[0]->setVisible(false);
		m_actions[1]->setVisible(false);
		m_actions[2]->setVisible(false);
	}
	else
	{
		m_actions[0]->setVisible(true);
		m_actions[1]->setVisible(true);
		m_actions[2]->setVisible(true);
	}
}

void CMindMapUiNewItemList::onActionTriggered(QAction *act)
{
	int				i = 0;

	for (i = 0; i < m_actions.length(); ++i)
	{
		if (act == m_actions[i])
			break;
	}

	emit insertPositionSelected(i, m_icons[i]);
}

CMindMapUiIconBrowser::CMindMapUiIconBrowser(QWidget *parent)
	: CUiAbstractDialog(parent),
	  m_ui(NULL)
{
	m_ui = new Ui::CMindMapUiIconBrowser();
	m_ui->setupUi(m_main);

    m_ui->m_icons->setViewMode( QListView::IconMode );
    m_ui->m_icons->setIconSize( QSize(20, 20) );
    m_ui->m_icons->setUniformItemSizes(true);
    m_ui->m_icons->setSpacing(0);
    m_ui->m_icons->setGridSize( QSize(24, 24));

	setTitle(tr("Icon Browser"));

    loadIcons();

    connect(m_ui->m_btnOk, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(m_ui->m_btnCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
    connect(m_ui->m_icons, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onItemSelected(QListWidgetItem*)));

    resize(320, 280);
}

CMindMapUiIconBrowser::~CMindMapUiIconBrowser()
{
}

void CMindMapUiIconBrowser::loadIcons()
{
    CMindMapIconProvider    *iconProvider = NULL;
    QList<QIcon>            icons;

    iconProvider = CMindMapIconProvider::instance();
    if (iconProvider)
    {
        m_ui->m_icons->clear();

        icons = iconProvider->getAllIcons();
        for (int i = 0; i < icons.length(); ++i)
        {
            QListWidgetItem     *item = NULL;
            item = new QListWidgetItem(icons.at(i), "");
            item->setTextAlignment( Qt::AlignCenter );
            item->setToolTip(tr("Icon No.%1").arg(i));
            item->setData(Qt::UserRole + 1, QVariant(i));
            m_ui->m_icons->addItem(item);
        }
    }
}

void CMindMapUiIconBrowser::onOk()
{
    int                 index = 0;
    QIcon               icon;
    QListWidgetItem     *item = NULL;

    item = m_ui->m_icons->currentItem();
    if (item)
    {
        icon = item->icon();
        index = item->data( Qt::UserRole + 1).toInt();
        close();
        emit iconSelected(icon, index);
    }
}

void CMindMapUiIconBrowser::onCancel()
{
    close();
}

void CMindMapUiIconBrowser::onItemSelected(QListWidgetItem *item)
{
    int         index = 0;
    QIcon       icon;

    if (item)
    {
        icon = item->icon();
        index = item->data( Qt::UserRole + 1).toInt();
        close();
        emit iconSelected(icon, index);
    }
}

CMindMapUiLinkEditor::CMindMapUiLinkEditor(QWidget *parent)
	: CUiAbstractDialog(parent),
	  m_linkType(-1)
{
	m_ui = new Ui::CMindMapUiLinkEditor();
	m_ui->setupUi(m_main);

    setTitle(tr("Links"));
    setWindowFlags( windowFlags() | Qt::FramelessWindowHint);

    m_ui->m_topicTree->header()->hide();
    m_ui->m_topicTree->setHeaderHidden(true);
    m_ui->m_topicTree->expandAll();

	connect(m_ui->m_cblinkTypes, SIGNAL(activated(int)), this, SLOT(onLinkTypeChanged(int)));
    connect(m_ui->m_btnCancel, SIGNAL(clicked()), this, SLOT(onClose()));
    connect(m_ui->m_btnOk, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(m_ui->m_btnDelete, SIGNAL(clicked()), this, SLOT(onDelete()));
    connect(m_ui->m_linkAddress, SIGNAL(returnPressed()), this, SLOT(onOk()));
}

CMindMapUiLinkEditor::~CMindMapUiLinkEditor()
{
}

int CMindMapUiLinkEditor::linkType()
{
	return m_linkType;
}

QUrl CMindMapUiLinkEditor::linkAddress()
{
    return QUrl(m_linkAddress);
}

bool CMindMapUiLinkEditor::reset(CDiagram *d)
{
    int                             type = 0;
    QUrl                            address;
    CDiagramFileManager             *fileManager = NULL;
    CMindMapFile                    *file = NULL;
    CMindMapGraph                   *graph = NULL;
    CMindMapLinkSprite              *linkSprite = NULL;
    QStandardItem                   *item = NULL;
    QStandardItemModel              *dataModel = NULL, *treeModel = NULL;
    QList<CMindMapNode*>            nodes;
    QList<CMindMapNodeSprite*>      sprites;
    QList<CMindMapGraph*>           graphes;

    if (!d)
        return false;

    treeModel = new QStandardItemModel(this);
    fileManager = CDiagramFileManager::instance();
    if (fileManager && treeModel)
    {
        file = dynamic_cast<CMindMapFile*>( fileManager->getCurrentFile() );
        if (file)
        {
            graphes = file->graphes();
            foreach (graph, graphes)
            {
                dataModel = dynamic_cast<QStandardItemModel*>( graph->dataModel(true) );
                if (dataModel)
                {
                    item = dataModel->invisibleRootItem();
                    item->setData(QVariant(graph->title()), Qt::DisplayRole);
                    treeModel->appendRow( item );
                }
            }
        }

        m_ui->m_topicTree->setModel(treeModel);
    }

    m_ui->m_btnDelete->setVisible(false);
    nodes = getAllSelectedNodes(d);
    if (nodes.length() == 1)
    {
        sprites = nodes.first()->sprites( CMindMapNode::Link );
        if (sprites.length() > 0)
        {
            linkSprite = dynamic_cast<CMindMapLinkSprite*>( sprites.first() );
            type = linkSprite->linkType();
            address = linkSprite->link();
            m_ui->m_btnDelete->setVisible(true);
        }
    }

    m_ui->m_cblinkTypes->setCurrentIndex(type);
    switch(type)
    {
    case CMindMapLinkSprite::Web:
    case CMindMapLinkSprite::Email:
        m_ui->m_linkAddress->setText( address.toString() );
        break;

    case CMindMapLinkSprite::Topic:
        m_ui->m_linkAddress->setVisible(false);
    case CMindMapLinkSprite::SubGraph:

        break;

    default:
        break;

    }; // End of switch

    onLinkTypeChanged(type);
	return true;
}

void CMindMapUiLinkEditor::onCancel()
{
	close();
}

void CMindMapUiLinkEditor::onOk()
{
    bool        passValidation = true;
    QUrl        url;

    m_linkType = m_ui->m_cblinkTypes->currentIndex();
    m_linkAddress = m_ui->m_linkAddress->text();

    switch(m_linkType)
    {
    case CMindMapLinkSprite::Web:
    {
		url.setUrl(m_linkAddress);
		if (url.scheme().trimmed().isEmpty())
			url.setScheme("http");
        break;
    }

    case CMindMapLinkSprite::Email:
    {
        QRegExp     exp("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
        exp.setCaseSensitivity(Qt::CaseInsensitive);
        if (!exp.exactMatch(m_linkAddress))
            passValidation = false;
        break;
    }

    case CMindMapLinkSprite::Topic:
    {
        int                     nodeId, sheetId;
        QString                 linkAddress("topic:%1:%2");
        QModelIndex             index;
        QStandardItem           *item = NULL;
        QStandardItemModel      *model = NULL;

        index = m_ui->m_topicTree->currentIndex();
        model = dynamic_cast<QStandardItemModel*>( m_ui->m_topicTree->model() );
        if (model)
        {
            item = model->itemFromIndex(index);
            if (item)
            {
                sheetId = item->data( Qt::UserRole + 1).value<int>();
                nodeId = item->data( Qt::UserRole + 2).value<int>();
                m_linkAddress = linkAddress.arg( QString::number(sheetId) )
                                           .arg( QString::number(nodeId) );
            }
        }
        break;
    }

    case CMindMapLinkSprite::SubGraph:
    default:
        break;
    }; // End of switch

    if (passValidation)
    {
        url.setUrl(m_linkAddress);
        if (url.isValid())
        {
            close();
            emit resetLink(m_linkType, m_linkAddress);
        }
    }
}

void CMindMapUiLinkEditor::onDelete()
{
    close();
    emit deleteLink();
}

void CMindMapUiLinkEditor::onLinkTypeChanged(int type)
{
	switch(type)
	{
	default:
	case CMindMapLinkSprite::Web:
		m_ui->m_topicTree->hide();
        m_ui->m_linkAddress->show();
        m_ui->m_linkAddress->setPlaceholderText(tr("http://"));
		layout()->invalidate();
		resize(320, 80);
		break;

	case CMindMapLinkSprite::Email:
		m_ui->m_topicTree->hide();
        m_ui->m_linkAddress->show();
        m_ui->m_linkAddress->setPlaceholderText(tr("mailto://"));
		layout()->invalidate();
		resize(320, 80);
		break;

	case CMindMapLinkSprite::Topic:
		m_ui->m_topicTree->show();
        m_ui->m_linkAddress->hide();
        m_ui->m_linkAddress->setPlaceholderText(tr("topic://"));
		resize(240, 240);
		layout()->invalidate();
		break;

	case CMindMapLinkSprite::SubGraph:
		m_ui->m_topicTree->show();
        m_ui->m_linkAddress->hide();
		resize(240, 240);
		layout()->invalidate();
		break;

	}; // End of switch
	
    m_linkType = type;
}

CMindMapUiNoteEditor::CMindMapUiNoteEditor(QWidget *parent)
	: CUiAbstractDialog(parent)
{
	m_ui = new Ui::CMindMapUiNoteEditor();
	m_ui->setupUi(m_main);

	setTitle(tr("Note Editor"));

    setWindowFlags( windowFlags() | Qt::FramelessWindowHint);

    connect(m_ui->m_btnCancel, SIGNAL(clicked()), this, SLOT(onClose()));
    connect(m_ui->m_btnOk, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(m_ui->m_btnDelete, SIGNAL(clicked()), this, SLOT(onDelete()));
}

CMindMapUiNoteEditor::~CMindMapUiNoteEditor()
{
}

void CMindMapUiNoteEditor::setText(const QString &value)
{
    m_text = value;
    m_ui->m_noteEditor->setPlainText(m_text);
}

QString CMindMapUiNoteEditor::text()
{
    return m_text;
}

bool CMindMapUiNoteEditor::reset(CDiagram *d)
{
    QString                         note;
    CMindMapNoteSprite              *noteSprite = NULL;
    QList<CMindMapNode*>            nodes;
    QList<CMindMapNodeSprite*>      sprites;

    if (!d)
        return false;

    m_ui->m_btnDelete->setVisible(false);
    nodes = getAllSelectedNodes(d);
    if (nodes.length() == 1)
    {
        sprites = nodes.first()->sprites( CMindMapNode::Note );
        if (sprites.length() > 0)
        {
            noteSprite = dynamic_cast<CMindMapNoteSprite*>( sprites.first() );
            if (noteSprite)
            {
                note = noteSprite->note();
                m_ui->m_btnDelete->setVisible(true);
            }
        }
    }

    m_ui->m_noteEditor->setPlainText(note);
	return true;
}

void CMindMapUiNoteEditor::onCancel()
{
    close();
}

void CMindMapUiNoteEditor::onOk()
{
    close();
    m_text = m_ui->m_noteEditor->toPlainText();
    if (m_text.isEmpty())
        emit deleteNote();
    else
        emit resetNote(m_text);
}

void CMindMapUiNoteEditor::onDelete()
{
    close();
    emit deleteNote();
}


CMindMapUiDateEditor::CMindMapUiDateEditor(QWidget *parent)
    : CUiAbstractDialog(parent),
      m_ui(NULL),
	  m_enableTime(false)
{
    m_ui = new Ui::CMindMapUiDateEditor();
    m_ui->setupUi(m_main);

    setTitle(tr("Date Editor"));
    setWindowFlags( windowFlags() | Qt::FramelessWindowHint);

    m_ui->m_timeChecker->hide();
    m_ui->m_timeEditor->hide();

    resize(320, 240);

    connect(m_ui->m_btnOk, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(m_ui->m_btnCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
    connect(m_ui->m_btnReset, SIGNAL(clicked()), this, SLOT(onReset()));
    connect(m_ui->m_timeChecker, SIGNAL(toggled(bool)), this, SLOT(onEnabledTime(bool)));
    connect(m_ui->m_calendar, SIGNAL(clicked(QDate)), this, SLOT(onDateChanged(QDate)));
    connect(m_ui->m_timeEditor, SIGNAL(timeChanged(QTime)), this, SLOT(onTimeChanged(QTime)));
}

CMindMapUiDateEditor::~CMindMapUiDateEditor()
{

}

bool CMindMapUiDateEditor::reset(CDiagram *d)
{
    m_ui->m_timeChecker->hide();
    m_ui->m_timeEditor->hide();

    updateDateString();

    return true;
}

bool CMindMapUiDateEditor::enableTime()
{
    return m_enableTime;
}

void CMindMapUiDateEditor::onCancel()
{
    close();
}

void CMindMapUiDateEditor::onOk()
{
    close();
    emit dateSelected(m_dateString);
}

void CMindMapUiDateEditor::onReset()
{
	updateDateString();
    close();
    emit dateSelected("");
}

void CMindMapUiDateEditor::onEnabledTime(bool value)
{
    m_ui->m_timeEditor->setVisible(value);

    m_enableTime = value;
}

void CMindMapUiDateEditor::onDateChanged(QDate date)
{
	updateDateString();
}

void CMindMapUiDateEditor::onTimeChanged(QTime time)
{
    updateDateString();
}

void CMindMapUiDateEditor::updateDateString()
{
	QString     tpl(tr("Due Date:%1"));
    QString     dateString;

    if (m_enableTime)
        dateString = QString("%1 %2")
                .arg(m_ui->m_calendar->selectedDate().toString( Qt::ISODate ))
                .arg(m_ui->m_timeEditor->time().toString( "HH:mm a"  ));
    else
        dateString = m_ui->m_calendar->selectedDate().toString(Qt::ISODate);

	m_dateString = dateString;
    m_ui->m_dueDate->setText( tpl.arg(m_dateString) );
}

CMindMapUiTaskEditor::CMindMapUiTaskEditor(QWidget *parent)
    : CUiAbstractDialog(parent),
      m_dateEditor(NULL),
      m_d(NULL),
      m_ui(NULL)
{
    m_ui = new Ui::CMindMapUiTaskEditor();
    m_ui->setupUi(m_main);

    setTitle(tr("Task Editor"));

    setWindowFlags( windowFlags() | Qt::FramelessWindowHint);
    resize(350, 180);

	m_dateEditor = new CMindMapUiDateEditor(NULL);
	m_dateEditor->hide();

    m_ui->m_dueDate->installEventFilter(this);

    connect(m_ui->m_btnCancel, SIGNAL(clicked()), this, SLOT(onClose()));
    connect(m_ui->m_btnOk, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(m_ui->m_btnClear, SIGNAL(clicked()), this, SLOT(onDelete()));
	connect(m_ui->m_btnShowDateEditor, SIGNAL(clicked()), this, SLOT(onShowDateEditor()));
	connect(m_ui->m_cbAssignedTo, SIGNAL(activated(QString)), this, SLOT(onAssignedTo(QString)));
	connect(m_dateEditor, SIGNAL(dateSelected(QString)), this, SLOT(onDateSelected(QString)));
}

CMindMapUiTaskEditor::~CMindMapUiTaskEditor()
{

}

bool CMindMapUiTaskEditor::reset(CDiagram *d)
{
    bool                        taskExisted = false;
    CMindMapNode                *node = NULL;
    QList<CMindMapNode*>        nodes;
    QList<CMindMapNodeSprite*>  sprites;
    QString                     assignee("");
    QString                     dueDate;
    int                         priority = -1, progress = -1, index = -1;
    QString                     custom(tr("Customize..."));

    m_d = d;
    if (!m_d)
        return false;

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() > 0)
    {
        node = nodes.first();
        sprites = node->sprites(CMindMapNode::Task);
        if (sprites.length() > 0)
        {
            CMindMapTaskSprite          *taskSprite = NULL;

            taskSprite = dynamic_cast<CMindMapTaskSprite*>(sprites.first());
            if (taskSprite)
            {
                assignee = taskSprite->assignedTo();
                dueDate = taskSprite->dueDateString();
                taskExisted = true;
            }
        }

        sprites = node->sprites(CMindMapNode::TaskPriority);
        if (sprites.length() > 0)
        {
            CMindMapTaskPrioritySprite	*prioritySprite = NULL;

            prioritySprite = dynamic_cast<CMindMapTaskPrioritySprite*>(sprites.first());
            if (prioritySprite)
            {
                priority = prioritySprite->priority();
                taskExisted = true;
            }
        }

        sprites = node->sprites(CMindMapNode::TaskProgress);
        if (sprites.length() > 0)
        {
            CMindMapTaskProgressSprite	*progressSprite = NULL;

            progressSprite = dynamic_cast<CMindMapTaskProgressSprite*>(sprites.first());
            if (progressSprite)
            {
                progress = progressSprite->progress();
                taskExisted = true;
            }
        }

        // assignee reset
        if (assignee.isEmpty())
        {
            m_ui->m_cbAssignedTo->setCurrentIndex(0);
        }
        else
        {
            index = m_ui->m_cbAssignedTo->findText( assignee );
            if (index > -1 )
            {
                m_ui->m_cbAssignedTo->setCurrentIndex( index );
            }
            else
            {
                m_ui->m_cbAssignedTo->removeItem( m_ui->m_cbAssignedTo->findText(custom) );
                m_ui->m_cbAssignedTo->addItem( assignee );
                m_ui->m_cbAssignedTo->addItem( custom );
                m_ui->m_cbAssignedTo->setCurrentIndex( m_ui->m_cbAssignedTo->count() - 2 );
            }
        }

        // due date reset
        if (dueDate.isEmpty())
        {
            m_ui->m_dueDate->setText("--Not Set--");
        }
        else
        {
            m_ui->m_dueDate->setText(dueDate);
        }

        // priority reset
        if (priority == -1)
        {
            m_ui->m_cbPriority->setCurrentIndex(0);
        }
        else
        {
            m_ui->m_cbPriority->setCurrentIndex(priority + 1);
        }

        // progress reset
        if (progress == -1)
        {
            m_ui->m_cbProgress->setCurrentIndex(0);
        }
        else
        {
            m_ui->m_cbProgress->setCurrentIndex(progress + 1);
        }
    }

    m_ui->m_btnClear->setVisible( taskExisted );

    return true;
}

void CMindMapUiTaskEditor::onCancel()
{
    close();
}

void CMindMapUiTaskEditor::onOk()
{
    int         priority = -1;
    int         progress = -1;
    QString     dueDate;
    QString     assignee;

    close();

    priority = m_ui->m_cbPriority->currentIndex();
    progress = m_ui->m_cbProgress->currentIndex();

    // assigned to
    if (m_ui->m_cbAssignedTo->currentIndex() == 0)
        assignee = "";
    else
        assignee = m_ui->m_cbAssignedTo->currentText();

    // due date
    if ( m_ui->m_dueDate->text() != m_ui->m_dueDate->placeholderText())
        dueDate = m_ui->m_dueDate->text();
    else
        dueDate = "";

    emit resetTask(priority - 1, progress - 1, assignee, dueDate, m_dateEditor->enableTime());
}

void CMindMapUiTaskEditor::onDelete()
{
    close();
    emit deleteTask();
}

void CMindMapUiTaskEditor::onShowDateEditor()
{
	if (!m_d || !m_dateEditor)
        return;

    if (m_dateEditor->reset(m_d))
        m_dateEditor->show();
}

void CMindMapUiTaskEditor::onDateSelected(QString dueDate)
{
	m_ui->m_dueDate->setText( dueDate );
}

void CMindMapUiTaskEditor::onAssignedTo(QString user)
{
	int                     index = -1;
    bool                    ok;
    QString                 text, custom(tr("Customize..."));

    if (!m_d)
        return;

    index = m_ui->m_cbAssignedTo->findText(user);
    if (index == 0)
    {
        text = "";
    }
    else if (index == m_ui->m_cbAssignedTo->count() - 1)
    {
        user = QInputDialog::getText(NULL,
                                     tr("Assigned Task"),
                                     tr("Task assigned to:"),
                                     QLineEdit::Normal,
                                     tr("Anonymouse"),
                                     &ok,
                                     Qt::FramelessWindowHint);
        if (ok && !user.isEmpty())
        {
            text = user;
            index = m_ui->m_cbAssignedTo->findText(user);
            if (index < 0)
            {
                m_ui->m_cbAssignedTo->addItem(user);
                index = m_ui->m_cbAssignedTo->findText(custom);
                if (index >= 0)
                {
                    m_ui->m_cbAssignedTo->removeItem(index);
                    m_ui->m_cbAssignedTo->addItem(custom);
                }
            }
			m_ui->m_cbAssignedTo->setCurrentIndex( m_ui->m_cbAssignedTo->findText(user) );
        }
        else
        {
            return;
        }
    }
    else
    {
        text = user;
    }
}

bool CMindMapUiTaskEditor::eventFilter(QObject *obj, QEvent *event)
{
    bool        r = false;

    if (obj == m_ui->m_dueDate)
    {
        switch(event->type())
        {
        case QEvent::MouseButtonPress:
            onShowDateEditor();
            r = true;
            break;

        default:
            break;
        }; // End of switch
    }

    return r;
}

CMindMapUiGraphMenu::CMindMapUiGraphMenu()
    : CDiagramContextMenu(),
      m_layoutMenu(NULL),
      m_shapeMenu(NULL),
      m_pathMenu(NULL)
{
    setupMenu();
}

CMindMapUiGraphMenu::~CMindMapUiGraphMenu()
{
    SAFE_DELETE(m_layoutMenu);
    SAFE_DELETE(m_shapeMenu);
    SAFE_DELETE(m_pathMenu);
}

void CMindMapUiGraphMenu::setupMenu()
{
    m_layoutMenu = new CMindMapUiLayoutList(NULL);
    m_shapeMenu = new CMindMapUiShapeList(NULL);
    m_pathMenu = new CMindMapUiPathList(NULL);

    m_layoutMenu->setTitle(tr("Layout Types"));
	m_layoutMenu->setIcon( QIcon(":/res/icons/node-layout.png") );

    m_shapeMenu->setTitle(tr("Node Shape"));
	m_shapeMenu->setIcon( QIcon(":/res/icons/node-shape.png") );

    m_pathMenu->setTitle(tr("Edge Types"));
	m_pathMenu->setIcon( QIcon(":/res/icons/node-edge.png") );

    connect(m_layoutMenu, SIGNAL(layoutSelected(int)), this, SIGNAL(layoutSelected(int)));
    connect(m_shapeMenu, SIGNAL(shapeSelected(int)), this, SIGNAL(shapeSelected(int)));
    connect(m_pathMenu, SIGNAL(pathSelected(int)), this, SIGNAL(pathSelected(int)));
}

bool CMindMapUiGraphMenu::support(CDiagram *d)
{
    bool                    r = false;
    QList<CMindMapNode*>	nodes;

    if (d && d->mode() != CDiagram::TagMode)
    {
        nodes = getAllSelectedNodes(d);
        if (nodes.length() != 0)
        {
            r = true;
        }
    }

    return r;
}

void CMindMapUiGraphMenu::reset(CDiagram *d)
{
    m_d = d;
}

void CMindMapUiGraphMenu::apply(QMenu *menu)
{
    CMindMapNode            *node = NULL;
    QList<CMindMapNode*>	nodes;

    if (!menu || !m_d)
        return;

    if (!menu->isEmpty())
        menu->addSeparator();

    nodes = getAllSelectedNodes(m_d);
    if (nodes.length() == 1)
    {
        node = nodes.first();
        if (node->isRoot())
        {

            menu->addAction(QIcon(":/res/icons/node-insert-child.png"), tr("Insert Child Topic"), this, SIGNAL(insertTopicChild()));
        }
        else
        {
            menu->addAction(QIcon(":/res/icons/node-insert-child.png"), tr("Insert Child Topic"), this, SIGNAL(insertTopicChild()));
            menu->addAction(QIcon(":/res/icons/node-insert-parent.png"), tr("Insert Parent Topic"), this, SIGNAL(insertTopicParent()));
            menu->addAction(QIcon(":/res/icons/node-insert-previous.png"), tr("Insert Sibling Topic Before"), this, SIGNAL(insertTopicBefore()));
            menu->addAction(QIcon(":/res/icons/node-insert-next.png"), tr("Insert Sibling Topic After"), this, SIGNAL(insertTopicAfter()));
        }
        m_layoutMenu->reset( node->isRoot() );
    }
    else
    {
        m_layoutMenu->reset(false);
    }

    if (!node || !node->isRoot())
    {
        menu->addSeparator();
        menu->addAction(QIcon(":/res/icons/node-boundary.png"), tr("Add Boundary"), this, SIGNAL(addBoundary()));
    }

    if (!menu->isEmpty())
        menu->addSeparator();

    menu->addMenu(m_shapeMenu);
    menu->addMenu(m_layoutMenu);
    menu->addMenu(m_pathMenu);

}

CMindMapUiFileDropper::CMindMapUiFileDropper()
    : CUiAbstractDialog(NULL),
      m_ui(NULL),
      m_d(NULL)
{
    m_ui = new Ui::CMindMapUiFileDropper();
    m_ui->setupUi(m_main);

    connect(m_ui->m_btnOk, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(m_ui->m_btnCancel, SIGNAL(clicked()), this, SLOT(onCancel()));

    setWindowFlags( windowFlags() | Qt::FramelessWindowHint);

    resize(360, 180);
}

CMindMapUiFileDropper::~CMindMapUiFileDropper()
{

}

bool CMindMapUiFileDropper::reset(CDiagram *d)
{
    QStringList                 files;
    QListWidgetItem             *item = NULL;
    QList<CMindMapNode*>        nodes;
    QList<CMindMapNodeSprite*>  sprites;
    QList<QString>              suffixKeys;
    QMap<QString, QIcon>        suffixMaps;
    CMindMapNode                *node = NULL;
    CMindMapFileSprite          *fileSprite = NULL;

    if (!d)
        return false;
    m_d = d;
    m_ui->m_fileList->clear();

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
    suffixKeys = suffixMaps.keys();

    nodes = getAllSelectedNodes(d);
    if (nodes.length() > 0)
    {
        node = nodes.first();
        sprites = node->sprites(CMindMapNode::File);
        if (sprites.length() > 0)
        {
            fileSprite = dynamic_cast<CMindMapFileSprite*>(sprites.first());
            if (fileSprite)
            {
                files = fileSprite->files();
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

                    item = new QListWidgetItem(icon, file);
                    item->setCheckState(Qt::Unchecked);
                    m_ui->m_fileList->addItem(item);
                }
            }
        }
    }

    return true;
}

void CMindMapUiFileDropper::onOk()
{
    int                         count = 0;
    QStringList                 files;
    QListWidgetItem             *item = NULL;
    QList<QListWidgetItem*>     items;

    count = m_ui->m_fileList->count();
    for (int i = 0; i < count; ++i)
    {
        item = m_ui->m_fileList->item(i);
        if (item && item->checkState() == Qt::Checked)
            items.push_back( item );
    }

    if (items.length() == m_ui->m_fileList->count())
    {
        emit removeAttach();
    }
    else
    {
        foreach (QListWidgetItem* item, items)
        {
            files << item->text();
        }
        emit dropFiles(files);
    }
    close();
}

void CMindMapUiFileDropper::onCancel()
{
    close();
}
