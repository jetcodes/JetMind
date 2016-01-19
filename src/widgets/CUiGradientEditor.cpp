/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CGlobal>
#include "CUiGradientEditor.h"

#define STOPDRAGGER_WIDTH		9
#define STOPDRAGGER_HEIGHT		15
#define STOPAREA_HEIGHT			16
#define STOPSTAT_HEIGHT			16
#define GRADIENTEDITOR_HEIGHT	32
#define GRADIENTSTAT_WIDTH		32
#define GRADIENTSTAT_HEIGHT		16

bool lessThan(StopDragger *first, StopDragger *second)
{
	if (first && !second) return false;
	else if (!first && second) return true;
	else if (!first && !second) return false;
	else return first->x() < second->y();
}

StopDragger::StopDragger(StopArea *parent)
	: QWidget(parent),
	  m_selected(false),
	  m_dragged(false),
	  m_stopArea(parent)
{
    setToolTip(tr("Right click to Remove Stop"));
    setFixedSize(STOPDRAGGER_WIDTH, STOPDRAGGER_HEIGHT);
    setContextMenuPolicy(Qt::DefaultContextMenu);
}

void StopDragger::paintEvent( QPaintEvent * event )
{
	QRect			r;
	QPainter		painter(this);
	QPainterPath	angle;
	int				w, h, stop = 5;

	r = rect();
	w = r.width();
	h = r.height();

	angle.moveTo(w / 2.0, 0);
	angle.lineTo(0, stop);
	angle.lineTo(w, stop);
	angle.lineTo(w / 2.0, 0);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.fillRect( QRect(0, stop, w, h - stop), m_color );
	// painter.fillRect( QRect(2, stop + 2, w - 4, h - stop - 4), m_color);
	if (m_selected)
	{
		painter.fillPath(angle, QBrush(Qt::black));
	}
	else
	{
		painter.fillPath(angle, QBrush(Qt::white));
	}
	painter.setPen( QPen(Qt::black, 1) );
	//painter.drawLine(w / 2.0, 0, 0, stop);
	//painter.drawLine(w / 2.0, 0, w, stop);
	painter.drawRect(QRect(0, stop, w, h - stop));
}

void StopDragger::setColor(const QColor &value)
{
	m_color = value;
	emit m_stopArea->stopChanged();
	update();
}

QColor StopDragger::color()
{
	return m_color;
}

void StopDragger::setSelected(bool value)
{
	m_selected = value;
	update();
}

bool StopDragger::selected()
{
	return m_selected;
}

qreal StopDragger::stopPosition()
{
	qreal	d, length;
	QRect	r;

	r = m_stopArea->rect();
	length = r.width() - rect().width();
	
	d = x() / length;
	if (d > 1.0) d = 1.0;
	return d;
}

void StopDragger::mousePressEvent(QMouseEvent *event)
{
	if (event->button() & Qt::LeftButton)
	{
		m_stopArea->clearSelection();
		setSelected(true);
	}
    else if (event->button() & Qt::RightButton)
    {
        m_stopArea->removeStop(this);
        deleteLater();
    }
}

void StopDragger::mouseMoveEvent(QMouseEvent *event)
{
	QRect	pr, cr;
	QPoint	p;

	cr = rect();
	pr = parentWidget()->rect();
	if (m_selected && event->buttons() & Qt::LeftButton)
	{
		p = mapToParent( event->pos() );
        p = QPoint( qMax(cr.width() / 2.0, qMin(p.x() + 0.0, pr.width() - cr.width() / 2.0) ), 1 );
        move(p.x() - cr.width() / 2.0, 1);
        if (!isVisible())
            show();

		emit m_stopArea->stopChanged();
		m_dragged = true;
	}
}

void StopDragger::mouseReleaseEvent(QMouseEvent *event)
{
	QRect	pr, cr;
	QPoint	p;
	qreal	hcr;

	cr = rect();
	pr = parentWidget()->rect();
	hcr = cr.width() / 2.0;
	if (m_dragged && event->button() & Qt::LeftButton)
    {
        p = mapToParent( event->pos() );
        p = QPoint( qMax(cr.width() / 2.0, qMin(p.x() + 0.0, pr.width() - cr.width() / 2.0) ), 1 );
        move(p.x() - cr.width() / 2.0, 1);
	}
	m_dragged = false;
}

//void StopDragger::contextMenuEvent(QContextMenuEvent *event)
//{
//    QAction     *act = NULL;
//    QMenu       *menu = NULL;

//    menu = new QMenu(NULL);
//    menu->addAction(tr("Delete Stop"));
//    act = menu->exec(mapToGlobal(event->pos()));
//    if (act)
//    {
//        m_stopArea->removeStop(this);
//        deleteLater();
//    }
//    menu->deleteLater();
//}

StopArea::StopArea(QWidget *parent)
	:QWidget(parent)
{
    setToolTip(tr("Left Click to Add a Stop"));
	setMinimumHeight(STOPAREA_HEIGHT);
	initDefault();
}

StopArea::~StopArea()
{
	for (int i = 0; i < m_stops.length(); ++i)
	{
		delete m_stops.at(i);
	}
	m_stops.clear();
}

void StopArea::reset()
{
	for (int i = 0; i < m_stops.length(); ++i)
	{
		delete m_stops.at(i);
	}
	m_stops.clear();
	initDefault();
}

void StopArea::initDefault()
{
	QRect		r;
	StopDragger	*d = NULL;

	r = rect();
	d = new StopDragger(this);
	d->setColor( QColor(Qt::white) );
	d->move(0, 1);
	d->show();
	m_stops.push_back(d);

	d = new StopDragger(this);
	d->setColor( QColor(Qt::black) );
	d->move(r.width() - 1.5 * d->width(), 1);
	d->setSelected(true);
	d->show();
	m_stops.push_back(d);
}

QGradientStops StopArea::stops()
{
	StopDragger		*d = NULL;
	QGradientStops	stops;

	qStableSort(m_stops.begin(), m_stops.end(), lessThan );
	for (int i = 0; i < m_stops.length(); ++i)
	{
		QPair<qreal, QColor>	pair;

		d = m_stops.at(i);
		pair.first = d->stopPosition();
		pair.second = d->color();
		stops.push_back( pair );
	}

	return stops;
}

void StopArea::setStops(const QGradientStops &stops)
{
	qreal		length;
    QRect		r = geometry();
	StopDragger	*d = NULL;

	for (int i = 0; i < m_stops.length(); ++i)
	{
		delete m_stops.at(i);
	}
	m_stops.clear();

	// Kind of tricky here, we use 93 the magic number
	// we should change it later.
    length = r.width() - STOPDRAGGER_WIDTH;

	for (int i = 0; i < stops.size(); ++i)
	{
		QPair<qreal, QColor> p;
		p = stops.at(i);
		
		d = new StopDragger(this);
		d->setColor(p.second);
		d->move(p.first * length, 1);
        d->setVisible(true);
        m_stops.push_back(d);
    }
	update();
}

void StopArea::clearSelection()
{
	for (int i = 0; i < m_stops.length(); ++i)
	{
		m_stops.at(i)->setSelected(false);
	}
	update();
}

void StopArea::removeStop(StopDragger *dragger)
{
	m_stops.removeAll(dragger);
	emit stopChanged();
}

void StopArea::mousePressEvent(QMouseEvent *event)
{
	qreal			stopPosition;
	StopDragger		*d = NULL;

    if (event->button() & Qt::LeftButton)
    {
        stopPosition = event->x();
        if (stopPosition >= STOPDRAGGER_WIDTH / 2.0 && stopPosition <= rect().width() - STOPDRAGGER_WIDTH / 2.0)
        {
            d = new StopDragger(this);
            d->move(stopPosition - d->width() / 2.0, 1);

            clearSelection();
            d->setSelected(true);
            d->show();
            m_stops.push_back(d);
            emit stopChanged();
        }
    }
}

void StopArea::paintEvent(QPaintEvent *event)
{
	QWidget::paintEvent(event);
}

StopDragger* StopArea::selectedStop()
{
	StopDragger	*d = NULL;

	for (int i = 0; i < m_stops.length(); ++i)
	{
		d = m_stops.at(i);
		if (d && d->selected())
			break;
		else
			d = NULL;
	}
	return d;
}

int StopArea::stopCount()
{
	return m_stops.count();
}

void StopArea::setStopColor(const QColor &color)
{
	StopDragger	*d = NULL;

	d = selectedStop();
	if (d)
	{
		d->setColor(color);
		update();
	}
}

void StopArea::setStopColor(int index, const QColor &color)
{
	if (index >= 0 && index < m_stops.count())
	{
		m_stops.at(index)->setColor(color);
	}
}

QColor StopArea::stopColor()
{
	QColor		c;
	StopDragger	*d = NULL;

	d = selectedStop();
	if (d)
	{
		c = d->color();
	}
	return c;
}

QColor StopArea::stopColor(int index)
{
	QColor		c;

	if (index >= 0 && index < m_stops.count())
	{
		c = m_stops.at(index)->color();
	}
	return c;
}

StopStat::StopStat(QWidget *parent)
	:QWidget(parent)
{
	m_gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
	m_gradient.setStart(0,0);
	m_gradient.setFinalStop(1, 0);
	m_gradient.setColorAt(0, Qt::white);
	m_gradient.setColorAt(1, Qt::black);
}

void StopStat::paintEvent(QPaintEvent *event)
{
	QRect		r;
	QPainter	painter(this);
	QBrush		brush = QBrush(m_gradient);

	r = rect();
	painter.setPen( Qt::black );
	painter.setBrush( brush );
	painter.drawRect( QRect(r.x() + STOPDRAGGER_WIDTH / 2.0, 0, r.width() - STOPDRAGGER_WIDTH, r.height() ) );
}

void StopStat::setStops(const QGradientStops &stops)
{
	m_gradient.setStops(stops);
	update();
}

GradientStat::GradientStat(QWidget *parent)
	: QWidget(parent)
{
	setFixedWidth(GRADIENTSTAT_WIDTH);
	setMinimumHeight(GRADIENTSTAT_HEIGHT);

	m_gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
	m_gradient.setStart(0,0);
	m_gradient.setFinalStop(1, 0);
	m_gradient.setColorAt(0, Qt::white);
	m_gradient.setColorAt(1, Qt::black);
}

void GradientStat::paintEvent(QPaintEvent *event)
{
	QRect		r;
	QPainter	painter(this);
	QBrush		brush(m_gradient);

	r = rect();
	painter.setPen( Qt::black );
	painter.setBrush( brush );
	painter.drawRect(r);
}

QGradient GradientStat::gradient()
{
	return m_gradient;
}

void GradientStat::setStops(const QGradientStops &stops)
{
	m_gradient.setStops(stops);
	update();
}

void GradientStat::setAngle(qreal angle)
{
	while (angle < 0) angle += 360;
	while (angle >= 360) angle -= 360;

	switch(qRound(angle))
	{
	case 180:
		m_gradient.setStart(0, 0.5);
		m_gradient.setFinalStop(1, 0.5);
		break;

	case 135:
		m_gradient.setStart(0, 0);
		m_gradient.setFinalStop(1, 1);
		break;

	case 90:
		m_gradient.setStart(0.5, 0);
		m_gradient.setFinalStop(0.5, 1);
		break;

	default:
		break;
	};
	
	update();

	/*
	qreal	l;
	QPointF	p;
	QRect	r = rect();
	
	l = QLineF(0, 0, r.width(), r.height()).length();
	if (angle <= 90)
	{
		QLineF	line(0, 0, 1, 0);
		line.setAngle(-angle);
		line.setLength(l);
		if (QLineF::BoundedIntersection != line.intersect( QLineF(0, r.height(),  r.width(), r.height()), &p))
			line.intersect( QLineF(r.width(), 0,  r.width(), r.height()), &p);
		m_gradient.setStart(0, 0);
		m_gradient.setFinalStop(p.x() / r.width(), p.y() / r.height() );
	}
	else if (angle <= 180)
	{
		QLineF	line(r.width(), 0, 0, 0);
		line.setAngle(-angle);
		line.setLength(l);
		if (QLineF::BoundedIntersection != line.intersect( QLineF(0, r.height(),  r.width(), r.height()), &p))
			line.intersect( QLineF(0, 0,  0, r.height()), &p);
		m_gradient.setStart(1, 0);
		m_gradient.setFinalStop(p.x() / r.width(), p.y() / r.height() );
	}
	else if (angle <= 270)
	{
		QLineF	line(r.width(), r.height(), 0, 1);
		line.setAngle(-angle);
		line.setLength(l);
		if (QLineF::BoundedIntersection != line.intersect( QLineF(0, 0,  r.width(), 0), &p))
			line.intersect( QLineF(0, 0,  0, r.height()), &p);
		m_gradient.setStart(1, 1);
		m_gradient.setFinalStop(p.x() / r.width(), p.y() / r.height() );
	}
	else
	{
		QLineF	line(0, r.height(), 0, 0);
		line.setAngle(-angle);
		line.setLength(l);
		if (QLineF::BoundedIntersection != line.intersect( QLineF(0, 0,  r.width(), 0), &p))
			line.intersect( QLineF(r.width(), 0,  r.width(), r.height()), &p);
		m_gradient.setStart(0, 1);
		m_gradient.setFinalStop(p.x() / r.width(), p.y() / r.height() );
	}
	update();
	*/
}

CUiGradientEditor::CUiGradientEditor(QWidget *parent)
	: QWidget(parent),
	  m_stopArea(NULL),
	  m_stopStat(NULL),
	  m_gradientStat(NULL),
	  m_angleMark(NULL),
	  m_angleEdit(NULL)
{
	QStringList			types;
	QStringListModel	*model = new QStringListModel();
	QAbstractItemView	*view = new QListView(NULL);

	types << QString("90") << QString("135") << QString("180");
	model->setStringList(types);
	view->setModel(model);

	m_stopArea = new StopArea(this);
	m_stopStat = new StopStat(this);
	m_gradientStat = new GradientStat(this);
	m_angleMark = new QLabel(QString("Angle"), this);
	m_angleEdit = new QComboBox(this);
	m_angleEdit->setModel(model);
	m_angleEdit->setView(view);
	m_angleEdit->setMinimumWidth(48);

	QVBoxLayout		*layout1 = new QVBoxLayout();
	layout1->setContentsMargins(0, 0, 0, 0);
	layout1->setSpacing(0);
	layout1->addWidget(m_stopStat);
	layout1->addWidget(m_stopArea);

	QHBoxLayout		*hBox1 = new QHBoxLayout();
	hBox1->setContentsMargins(0, 0, 0, 0);
	hBox1->setSpacing(3);
	hBox1->addWidget(m_angleMark);
	hBox1->addWidget(m_angleEdit);
	hBox1->addWidget(m_gradientStat);

	QVBoxLayout		*layout2 = new QVBoxLayout();
	layout2->setContentsMargins(0, 0, 0, 0);
	layout2->addLayout(hBox1);
	layout2->addSpacing(16);

	QHBoxLayout		*mainLayout = new QHBoxLayout();
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(3);
	mainLayout->addLayout(layout1, 100);
	mainLayout->addLayout(layout2, 0);

	setLayout(mainLayout);

	connect(m_stopArea, SIGNAL(stopChanged()), this, SLOT(onStopChanged()));
	connect(m_angleEdit, SIGNAL(activated(const QString&)), this, SLOT(onAngleChanged(const QString &)));
}

CUiGradientEditor::~CUiGradientEditor()
{
	SAFE_DELETE(m_stopArea);
	SAFE_DELETE(m_stopStat);
	SAFE_DELETE(m_gradientStat);
	SAFE_DELETE(m_angleMark);
	SAFE_DELETE(m_angleEdit);
}

void CUiGradientEditor::setStopColor(const QColor &color)
{
	m_stopArea->setStopColor(color);
}

QColor CUiGradientEditor::stopColor()
{
	return m_stopArea->stopColor();
}

void CUiGradientEditor::setGradient(const QGradient &value)
{
	qreal					angle = 0;
	const QLinearGradient	*linearGradient = (const QLinearGradient*)(&value);
	QGradientStops			stops;

	stops = value.stops();
	m_stopArea->setStops(stops);
	m_stopStat->setStops(stops);
	m_gradientStat->setStops(stops);
	
	if (linearGradient)
	{
		QPointF	s1, s2;
		s1 = linearGradient->start();
		s2 = linearGradient->finalStop();
		angle = QLineF(s2, s1).angle();
		if (angle <= 0) angle = -angle;
		m_gradientStat->setAngle(angle);
		m_angleEdit->setCurrentIndex( m_angleEdit->findText(QString::number(angle)));
	}
	update();
}

QGradient CUiGradientEditor::gradient()
{
	return m_gradientStat->gradient();
}

void CUiGradientEditor::onStopChanged()
{
	m_stopStat->setStops( m_stopArea->stops() );
	m_gradientStat->setStops( m_stopArea->stops() );
}

void CUiGradientEditor::onAngleChanged(const QString &value)
{
	int		r;

	r = value.toInt();
	m_gradientStat->setAngle(r);
}

void CUiGradientEditor::setTransparency(qreal value)
{
	int		s;
	QColor	c;

	s = m_stopArea->stopCount();
	for (int i = 0; i < s; ++i)
	{
		c = m_stopArea->stopColor(i);
		c.setAlpha(value);
		m_stopArea->setStopColor(i, c);
	}
}

qreal CUiGradientEditor::transparency()
{
	return 0.0;
}
