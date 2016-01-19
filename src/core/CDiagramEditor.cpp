/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <cmath>
#include <core/CDiagram>
#include <core/CDiagramEditor>
#include <core/CDiagramContext>

CDiagramView::CDiagramView(QWidget *parent)
	: QGraphicsView(parent),
	  m_d(NULL),
	  m_pressed(false)
{
	setRenderHint(QPainter::Antialiasing, true);
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	setInteractive(false);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

CDiagramView::~CDiagramView()
{
}

void CDiagramView::setDiagram(CDiagram *d)
{
	setScene(d);
	m_d = d;

	updateScale();
}

CDiagram* CDiagramView::diagram()
{
	return m_d;
}

void CDiagramView::updateScale()
{
	QRect		r;
	QRectF		sc;
	qreal		scale = 1.0;
	QMatrix		matrix;

	if (!m_d)
		return;

	r = geometry();
	sc = m_d->sceneRect();

	scale = qMin(r.width() / sc.width(), r.height() / sc.height() );
    matrix.scale(scale, scale);
    setMatrix(matrix);

	m_scale = scale;
}

void CDiagramView::drawForeground(QPainter *painter, const QRectF &rect)
{
	QGraphicsView	*v = NULL;

	if (!m_d)
		return;

	if (m_sc.isNull() || m_sc.isEmpty())
	{
		v = m_d->getMainView();
		if (!v) 
			return;
		m_sc = v->mapToScene(v->viewport()->geometry()).boundingRect();
	}

	painter->save();
	painter->setPen( QPen(Qt::red, 16, Qt::SolidLine) );
	painter->setBrush( Qt::NoBrush );
	painter->drawRect( m_sc );
	painter->restore();
}

void CDiagramView::resizeEvent(QResizeEvent *event)
{
	QGraphicsView::resizeEvent(event);

	updateScale();
}

void CDiagramView::mousePressEvent(QMouseEvent *event)
{
	QPointF			p0, p1;

	if (event->button() & Qt::LeftButton)
	{
		p0 = event->posF();
		p1 = mapToScene(p0.toPoint());
		if (m_sc.contains(p1))
		{
			m_buttonDownPos = p0;
			m_viewCenterPos = m_sc.center();
			m_pressed = true;
		}
	}
}

void CDiagramView::mouseMoveEvent(QMouseEvent *event)
{
	QPointF			delta, p0, p1;
	QRectF			sc;
	qreal			w, h;
	QGraphicsView	*v = NULL;

	if (!m_d)
		return;

	sc = m_d->sceneRect();
	w = m_sc.width() / 2.0;
	h = m_sc.height() / 2.0;
	if (m_pressed && (event->buttons() & Qt::LeftButton))
	{
		p0 = event->posF();
		delta = p0 - m_buttonDownPos;
		p1 = m_viewCenterPos + delta / m_scale;

		if (p1.x() - w < sc.left())
			p1.setX(sc.left() + w);
		if (p1.x() + w > sc.right())
			p1.setX(sc.right() - w);
		if (p1.y() - h < sc.top())
			p1.setY(sc.top() + h);
		if (p1.y() + h > sc.bottom())
			p1.setY(sc.bottom() - h);

		v = m_d->getMainView();
		m_sc.moveCenter(p1);
		if (v)
			v->centerOn(p1);
		update();
	}
}

void CDiagramView::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_pressed && (event->button() & Qt::LeftButton) )
	{
		m_pressed = false;
	}
}

void CDiagramView::setViewRect(const QRectF &value)
{
	if (!m_pressed)
	{
		m_sc = value;
		update();
	}
}

QRectF CDiagramView::viewRect()
{
	return m_sc;
}

CDiagramEditor::CDiagramEditor(QWidget* parent)
	: QGraphicsView(parent),
	  m_d(NULL),
	  m_miniView(NULL),
	  m_context(NULL),
	  m_rubberBandSelection(false),
	  m_rubberBandSelectionStarted(false)
{	
	setRenderHint(QPainter::Antialiasing, true );
	setAlignment(Qt::AlignLeft | Qt::AlignTop);
	setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);
	setDragMode(QGraphicsView::NoDrag);
	
	// for performance optimization
	setViewportUpdateMode(BoundingRectViewportUpdate);
	setFrameStyle(0);
	setAttribute(Qt::WA_TranslucentBackground, false);
	setOptimizationFlags(DontSavePainterState);

	setWindowTitle(tr("Diagram Editor"));

	m_miniView = new CDiagramView(NULL);
	connect(horizontalScrollBar(), SIGNAL(valueChanged (int)), this, SLOT(onScrollBarChanged(int)));
	connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onScrollBarChanged(int)));
}

CDiagramEditor::~CDiagramEditor()
{
}

void CDiagramEditor::setDiagram(CDiagram *d, const QPointF &center)
{
	QRectF		r;
	CDiagram	*prev = NULL;

	if (m_d)
	{
		disconnect(m_d, SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(unitedSceneRect(const QRectF&)));
		if (m_context)
			m_context->uninitContext(m_d);
	}
	prev = m_d;

	QGraphicsView::setScene( dynamic_cast<QGraphicsScene*>(d) );
	if (d)
	{
		r = d->sceneRect();

		unitedSceneRect(r);
		connect(d, SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(unitedSceneRect(const QRectF&)));

		if (!center.isNull())
			centerOn(center);
		else
			centerOn(QPointF(0, 0));

		if (m_context)
			m_context->initContext(d);
	}

	m_d = d;
	
	// set it to the miniview
	if (m_miniView)
		m_miniView->setDiagram(d);

	emit diagramChanged(prev, d);
}

void CDiagramEditor::unitedSceneRect(const QRectF& rect)
{
	setSceneRect(rect);
    if (m_miniView)
        m_miniView->updateScale();
}

void CDiagramEditor::scaleView(qreal scaleFactor)
{
    qreal       w = 0, h = 0, scale = 0;
    QPointF     p;
    QRectF      sc, r;

    r = mapToScene(viewport()->geometry()).boundingRect();
    p = r.center();

	if (m_d) 
    {
        sc = m_d->sceneRect();
        scale = m_d->getScale();
        w = sc.width() / scale * scaleFactor;
        h = sc.height() / scale * scaleFactor;
        m_d->setSceneRect( QRectF(-w / 2.0, -h / 2.0, w, h) );
		m_d->setScale(scaleFactor);
    }

    r = mapToScene(viewport()->geometry()).boundingRect();
    centerOn(p);
}

QPointF CDiagramEditor::getAnchorPoint(QRectF rect)
{
	QPointF		p;
	QRectF		sc;

	p = rect.topLeft();
	sc = mapToScene(viewport()->geometry()).boundingRect();
	p = QPointF(qMin(p.x(), sc.right() - rect.width()), qMin(p.y(), sc.bottom() - rect.height()));
	return p;
}

QPoint CDiagramEditor::getAnchorPoint(QRect rect)
{
	QPoint		p;
	QRectF		sc;

	p = rect.topLeft();
	sc = mapToScene(viewport()->geometry()).boundingRect();
	p = QPoint(qMin(p.x(), qRound(sc.right() - rect.width())), qMin(p.y(), qRound(sc.bottom() - rect.height())));
	return p;
}

CDiagram* CDiagramEditor::diagram()
{
	CDiagram*	d = NULL;

	d = dynamic_cast<CDiagram*>(scene());
	return d;
}

bool CDiagramEditor::event(QEvent *evt)
{
	QKeyEvent	*keyEvent = NULL;

	if (evt->type() == QEvent::KeyPress)
	{
		keyEvent = dynamic_cast<QKeyEvent*>(evt);
		if (keyEvent && keyEvent->key() == Qt::Key_Tab)
		{
			if (m_d)
			{
				QApplication::sendEvent(m_d, evt);
				evt->accept();
				return true;
			}
		}
	}

	return QGraphicsView::event(evt);
}

void CDiagramEditor::mousePressEvent(QMouseEvent *event)
{
    // Make sure we do not use the rubber band selection
	// function provided by qgraphicsview, we are going to
	// implements ourselfs.
	if (dragMode() == QGraphicsView::RubberBandDrag)
	{
		setDragMode(QGraphicsView::NoDrag);
	}
	QGraphicsView::mousePressEvent(event);
	
	if (!event->isAccepted() && event->buttons() && Qt::LeftButton && m_d)
	{
		m_rubberBandSelection = true;
		m_rubberBandSelectionStarted = false;
		m_rubberBandStartPosition = mapToScene( mapFromGlobal(event->globalPos()) );
	}
}


void CDiagramEditor::mouseMoveEvent(QMouseEvent *event)
{
	QWidget		*viewPort = NULL;
	QPoint		p1, p2;
	QRect		r1, r2;
	int			dx, dy;
	QPointF		pos;
	qreal		xstep, ystep;
	qreal		sdx, sdy;
	QRectF		scr;
	QScrollBar	*bar = NULL;

	if (event->buttons() & Qt::LeftButton)
	{
		p1 = event->globalPos();
		viewPort = viewport();
		if (viewPort)
		{
			r1 = viewPort->geometry();
			p2 = viewPort->parentWidget()->mapToGlobal(r1.topLeft());
			r2 = QRect(p2, r1.size());
			if (!r2.contains(p1))
			{
				p2 = viewPort->parentWidget()->mapToGlobal(r1.center());
				dx = p1.x() - p2.x();
				dy = p1.y() - p2.y();
				xstep = (qAbs(dx) - r1.width() / 2.0);
				ystep = (qAbs(dy) - r1.height() / 2.0);
				if (xstep > 0)
				{
					xstep = dx < 0 ? -qRound(xstep) : qRound(xstep);
					bar = horizontalScrollBar();
					if (bar)
					{
						bar->setValue( bar->value() + xstep );
					}
				}
				if (ystep > 0)
				{
					ystep = dy < 0 ? -qRound(ystep) : qRound(ystep);
					bar = verticalScrollBar();
					if (bar)
						bar->setValue( bar->value() + ystep );
				}
			}
		}
	}

	if (m_rubberBandSelection)
	{
		pos = mapToScene( mapFromGlobal(event->globalPos()) );

		if ( !m_rubberBandSelectionStarted && (pos - m_rubberBandStartPosition).manhattanLength() >= QApplication::startDragDistance() )
		{
			m_rubberBandSelectionStarted = true;
			m_d->startRubberBandSelection( m_rubberBandStartPosition );
		}

		if (m_rubberBandSelectionStarted)
			m_d->updateRubberBandSelection(pos);

		if (m_rubberBandSelectionStarted && !(event->buttons() & Qt::LeftButton))
		{
			m_d->finishRubberBandSelection(pos);
			m_rubberBandSelection = false;
			m_rubberBandSelectionStarted = false;
		}
	}
	else
	{
		QGraphicsView::mouseMoveEvent(event);
	}
}

void CDiagramEditor::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_rubberBandSelection && m_rubberBandSelectionStarted)
	{
		m_d->finishRubberBandSelection( mapToScene( mapFromGlobal(event->globalPos()) ) );
		m_rubberBandSelection = false;
		m_rubberBandSelectionStarted = false;
	}
	QGraphicsView::mouseReleaseEvent(event);
}

void CDiagramEditor::contextMenuEvent(QContextMenuEvent *event)
{
    // we have to reset the context menu event to NON-accepted, so the
    // scene can work correctly on context menu event
    if (event->isAccepted())
        event->setAccepted(false);
    QGraphicsView::contextMenuEvent(event);
}


void CDiagramEditor::wheelEvent ( QWheelEvent * event )
{
	int		delta;
	qreal	factor;

	if (m_d && (event->modifiers() & Qt::ControlModifier))
	{
		delta = event->delta();
		if (delta < 0) // stretch
		{
			factor = qMin(4.0, (m_d->getScale() + 0.20) );
		}
		else // shrink
		{
			factor = qMax(0.25, (m_d->getScale() - 0.20) );
		}
		m_d->setScale(factor);
	}
	else if (m_d)
	{
		QGraphicsView::wheelEvent(event);
	}
}

void CDiagramEditor::resizeEvent(QResizeEvent *event)
{
	QRectF		rect;
	QSize		s;

	QGraphicsView::resizeEvent(event);
	if (m_d)
	{
		rect = mapToScene(viewport()->geometry()).boundingRect();
		centerOn(rect.center());
		m_d->emitVisibleRectChanged();

		onScrollBarChanged(0);
	}
}

void CDiagramEditor::scrollContentsBy(int dx, int dy)
{
	QGraphicsView::scrollContentsBy(dx, dy);

	if (m_d)
	{
		m_d->emitVisibleRectChanged();
	}
}

void CDiagramEditor::setDiagramContext(CDiagramContext *cxt)
{
	if (!cxt)
		return;

	if (m_d && m_context)
		m_context->uninitContext(m_d);
	
	m_context = cxt;
	if (m_context && m_d)
		m_context->initContext(m_d);
}

CDiagramContext* CDiagramEditor::diagramContext()
{
	return m_context;
}

CDiagramView* CDiagramEditor::diagramView()
{
	return m_miniView;
}

void CDiagramEditor::onScrollBarChanged(int value)
{
	QRectF			sc;

	if (!m_d)
		return;
	
	sc = mapToScene(viewport()->geometry()).boundingRect();

	if (m_miniView)
		m_miniView->setViewRect(sc);
}