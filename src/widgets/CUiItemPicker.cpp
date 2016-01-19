/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <core/CDiagramLibraryModel>
#include <core/CDiagramItemTypeDatabase>
#include <core/CDiagramItemTypeCategory>

#include "CUiItemPicker.h"

CUiItemPicker::CUiItemPicker(QWidget *parent)
	: QMenu(parent)
{
	setMouseTracking(true);
	m_defaultCategory = "Core";
}

CUiItemPicker::~CUiItemPicker()
{
}

void CUiItemPicker::setupMenus()
{
	CCategoryQuickView						*ctgView = NULL;
	CDiagramLibraryModel					*libModel = NULL;
	CDiagramLibraryCategoryModel			*ctgModel = NULL;
	QList<CDiagramLibraryCategoryModel*>	ctgs;

	libModel = CDiagramLibraryModel::instance();
	if (!libModel || m_views.count() > 0)
		return;

	ctgs = libModel->categories();
	for (int i = 0; i < ctgs.length() - 1; ++i)
	{
		ctgModel = ctgs.at(i);
		ctgView = new CCategoryQuickView(this->parentWidget());
		ctgView->setLibraryCategoryModel(ctgModel);
		ctgView->setTitle(ctgModel->name());
		connect(ctgView, SIGNAL(shapeSelected(CDiagramLibraryItem*)), this, SLOT(onShapeSelected(CDiagramLibraryItem*)));
		m_views.insert(ctgModel->name(), ctgView);
		addMenu(ctgView);
	}
}

CDiagramLibraryItem* CUiItemPicker::defaultShape()
{
	CCategoryQuickView		*view = NULL;
	CDiagramLibraryItem		*item = NULL;

	if (m_views.contains(m_defaultCategory))
	{
		view = m_views[m_defaultCategory];
		item = view->defaultShape();
	}
	return item;
}

void CUiItemPicker::onShapeSelected(CDiagramLibraryItem *item)
{
	m_defaultCategory = item->libraryCategory();
	hide();
	emit shapeSelected(item);
}

CCategoryQuickView::CCategoryQuickView(QWidget *parent)
	: QMenu(parent),
	  m_gridSize(32),
	  m_space(8),
	  m_columns(6),
	  m_rows(6),
	  m_currShape(-1)
{
	setupLibrary();
	setMouseTracking(true);
	resize(160, 120);
}

CCategoryQuickView::~CCategoryQuickView()
{

}

void CCategoryQuickView::setLibraryCategoryModel(CDiagramLibraryCategoryModel *model)
{
	qreal		step;

	if (model)
	{
		m_currCategory = model->name();
		m_shapes = model->items();
	}
	else
	{
		m_currCategory = "Core";
		m_shapes = m_cores;
	}

	if (!m_defaultShapes.contains(m_currCategory) && m_shapes.length() > 0)
	{
		m_defaultShapes.insert(m_currCategory, m_shapes.at(0));
	}
	m_rows = qMax( (m_shapes.length() + m_columns - 1) / m_columns, 1 );

	step = m_gridSize;
	
	// Resize the widget to reasonable size
	resize(m_columns * step + 2 * m_space, m_rows * step + 2 * m_space );
}

void CCategoryQuickView::setupLibrary()
{
	QString							libCtg;
	QList<CDiagramItemType*>		wrappers;
	CDiagramItemTypeCategory			*ctg = NULL;
	CDiagramItemType				*w = NULL;
	CDiagramLibraryItem				*item = NULL;
	QStringList						filters;

	filters << "Square" << "Rect" << "Rounded Rect" << "Circle" << "Ellipse";
	libCtg = "Core";
	m_cores.clear();

	ctg = CDiagramItemTypeDatabase::instance()->getCategory(libCtg);
	if (ctg)
	{
		wrappers = ctg->items();
		for (int i = 0; i < wrappers.length(); ++i)
		{
			w = wrappers.at(i);
			if (filters.contains(w->name()))
			{
				item = new CDiagramLibraryItem();
				item->setIcon(w->icon());
				item->setName(w->name());
				item->setDesc(w->desc());
				item->setMetaName(w->name());
				item->setMetaType(libCtg);
				item->setLibraryCategory(libCtg);
				item->setInitHeight(0);
				item->setInitWidth(0);
				item->setInitMethod(InitedByCode);
				m_cores.append(item);
			}
		}
	}
}

CDiagramLibraryItem* CCategoryQuickView::defaultShape()
{
	CDiagramLibraryItem	*shape = NULL;

	if (m_defaultShapes.contains(m_currCategory))
	{
		shape = m_defaultShapes[m_currCategory];
	}
	return shape;
}

QSize CCategoryQuickView::sizeHint() const
{
	return QSize(geometry().size());
}

void CCategoryQuickView::paintEvent( QPaintEvent *event )
{
	int			i, row, col;
	qreal		step, radius = 5, div = m_space / 2.0;
	QRectF		r;	
	QPointF		p;
	QPainter	painter(this);

	step = m_gridSize;
	
	// Resize the widget to reasonable size
	resize(m_columns * step + 2 * m_space, m_rows * step + 2 * m_space );

	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setPen( QPen(Qt::gray, 1, Qt::SolidLine) );
	painter.setBrush( Qt::white );
	painter.drawRect( rect().adjusted(0,0,-1,-1) );
	painter.setRenderHint(QPainter::Antialiasing, true);

	for (i = 0; i < m_shapes.length(); ++i)
	{
		QSize					actualSize;
		QPixmap					pixmap;
		CDiagramLibraryItem		*shape = NULL;
		
		row = i / m_columns;
		col = i % m_columns;
		r.setTopLeft( QPointF(col * step + m_space, row * step + m_space) );
		r.setSize( QSizeF(m_gridSize, m_gridSize) );
		r.adjust(div, div,-div,-div);
		
		shape = m_shapes.at(i);
		actualSize = shape->icon().actualSize(QSize(m_gridSize,m_gridSize));
		pixmap = shape->icon().pixmap(actualSize);
		r = QRectF(r.topLeft() + QPointF((r.width() - actualSize.width()) / 2.0, (r.height() - actualSize.height()) / 2.0), QSizeF(actualSize.width(), actualSize.height()));
		painter.drawPixmap( r, pixmap, QRectF() );
	}

	// locate the hovered color
	p = m_mousePos;
	if (rect().contains(p.toPoint()))
	{
		col = qFloor((p.x() - m_space) / step);
		row = qFloor((p.y() - m_space) / step);
		m_currShape = row * m_columns + col;

		if (m_currShape >= 0 && m_currShape < m_shapes.length())
		{
			painter.setRenderHint(QPainter::Antialiasing, true);
			painter.setPen( Qt::NoPen );
			painter.setBrush( QColor(0,0,255, 48) );
			painter.drawRoundedRect(m_space + col * step, m_space + row * step, m_gridSize, m_gridSize, radius, radius );
		}
	}
	else
		m_currShape = -1;
}

void CCategoryQuickView::mousePressEvent( QMouseEvent *event )
{
	CDiagramLibraryItem	*shape = NULL;

	QWidget::mousePressEvent(event);
	if (m_currShape >= 0 && m_currShape < m_shapes.length())
	{
		hide();
		shape = m_shapes[m_currShape];
		if (m_defaultShapes.contains(m_currCategory))
			m_defaultShapes[m_currCategory] = shape;
		else
			m_defaultShapes.insert(m_currCategory, shape);
		emit shapeSelected( shape );
	}
}

void CCategoryQuickView::mouseMoveEvent( QMouseEvent *event )
{
	m_mousePos = event->pos();
	repaint();
	QMenu::mouseMoveEvent(event);
}

void CCategoryQuickView::mouseReleaseEvent( QMouseEvent *event)
{
	QWidget::mouseReleaseEvent(event);
}