/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramCore>
#include <core/CDiagramItem>
#include <core/CDiagramCommands>
#include <core/CDiagramCommandBuilder>
#include <core/CDiagramItemTypeCategory>
#include <core/CDiagramItemTypeDatabase>
#include <core/CDiagramArrowCategory>
#include <core/CDiagramArrowDatabase>
#include <core/CDiagramEdgeStyleCategory>
#include <core/CDiagramEdgeStyleDatabase>
#include <core/CDiagramNodeHandler>
#include <core/CDiagramEdgeHandler>
#include <core/CDiagramGroupHandler>
#include <core/CDiagramTextNode>
#include <core/CDiagramLine>
#include <core/CDiagramGroup>
#include <core/CDiagramEdge>
#include <core/CDiagramSvgNode>
#include <core/CDiagramPixmapNode>
#include <core/CDiagramIconProvider>

static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

Rect::Rect(CDiagram *parent)
	: CDiagramPathNode(parent)
{
	m_pen = QPen(Qt::black, 1, Qt::SolidLine);
	m_brush = QBrush(Qt::white);
	m_name = QString("Rect");
	m_category = QString("Core");
	m_keepSizeRatio = false;
	m_rotatable = true;
	resize(48, 24);
}

QPainterPath Rect::computePath()
{
	qreal			w = width() / 2.0;
	qreal			h = height() / 2.0;
	QPainterPath	path;

	path.addRect(-w, -h, width(), height() );
	return path;
}

RoundedRect::RoundedRect(CDiagram *parent)
	: CDiagramPathNode(parent)
{
	m_pen = QPen(Qt::black, 1, Qt::SolidLine);
	m_brush = QBrush(Qt::white);
	m_name = QString("Rounded Rect");
	m_category = QString("Core");
	m_keepSizeRatio = false;
	m_rotatable = true;
	resize(48, 24);
}

QPainterPath RoundedRect::computePath()
{
	qreal				d = 20.0;
	qreal				r = d/2.0;
	qreal				w = width() / 2.0;
	qreal				h = height() / 2.0;
	QPainterPath		path;
	
	path.moveTo(w, -h + r);
    path.arcTo(w - d, -h, d, d, 0.0, 90.0);
    path.lineTo(-w + r, -h);
    path.arcTo(-w, -h, d, d, 90.0, 90.0);
    path.lineTo(-w, h - r);
    path.arcTo(-w, h - d, d, d, 180.0, 90.0);
    path.lineTo(w - r, h);
    path.arcTo(w - d, h - d, d, d, 270.0, 90.0);
    path.closeSubpath();
	
	return path;
}

Square::Square(CDiagram *parent)
	: Rect(parent)
{
	m_name = QString("Square");
	m_keepSizeRatio = true;
	m_rotatable = true;
	resize(48, 48);
}

RoundedSquare::RoundedSquare(CDiagram *parent)
	: RoundedRect(parent) 
{
	m_name = QString("Rounded Square");
	m_keepSizeRatio = true;
	m_rotatable = true;
	resize(48, 48);
}

Circle::Circle(CDiagram *parent)
	: CDiagramPathNode(parent)
{
	m_pen = QPen(Qt::black, 1, Qt::SolidLine);
	m_brush = QBrush(Qt::white);
	m_name = QString("Circle");
	m_category = QString("Core");
	m_keepSizeRatio = true;
	m_rotatable = true;
	resize(48, 48);
}

QPainterPath Circle::computePath()
{
	qreal				w = width() / 2.0;
	qreal				h = height() / 2.0;
	QPainterPath		path;

	path.addEllipse(-width() / 2.0, -height() / 2.0, width(), height() );	
	return path;
}

Ellipse::Ellipse(CDiagram *parent)
	: Circle(parent)
{
	m_pen = QPen(Qt::black, 1, Qt::SolidLine);
	m_brush = QBrush(Qt::white);
	m_name = QString("Ellipse");
	m_keepSizeRatio = false;
	m_rotatable = true;
	resize(48, 24);
}

CCoreNodes::CCoreNodes()
	: CDiagramItemTypeCategory("Core")
{
	 m_published = false;
	 m_checked = false;
	 load();
}

void CCoreNodes::load()
{
	QString					name;
	QMetaObject				*meta = NULL;
	CDiagramIconProvider	*iconProvider = NULL;

	iconProvider = CDiagramIconProvider::instance();

	meta = const_cast<QMetaObject*>(&Square::staticMetaObject);
	name = QString::fromUtf8("Square");
	m_items.insert(name, new CDiagramItemType(name, name, iconProvider->genIconForItem(meta, QSizeF(128, 128)), meta));

	meta = const_cast<QMetaObject*>(&Rect::staticMetaObject);
	name = QString::fromUtf8("Rect");
	m_items.insert(name, new CDiagramItemType(name, name, iconProvider->genIconForItem(meta, QSizeF(128, 128)), meta));

	meta = const_cast<QMetaObject*>(&RoundedRect::staticMetaObject);
	name = QString::fromUtf8("Rounded Rect");
	m_items.insert(name, new CDiagramItemType(name, name, iconProvider->genIconForItem(meta, QSizeF(128, 128)), meta));

	meta = const_cast<QMetaObject*>(&Circle::staticMetaObject);
	name = QString::fromUtf8("Circle");
	m_items.insert(name, new CDiagramItemType(name, name, iconProvider->genIconForItem(meta, QSizeF(128, 128)), meta));

	meta = const_cast<QMetaObject*>(&Ellipse::staticMetaObject);
	name = QString::fromUtf8("Ellipse");
	m_items.insert(name, new CDiagramItemType(name, name, iconProvider->genIconForItem(meta, QSizeF(128, 128)), meta));

	meta = const_cast<QMetaObject*>(&CDiagramEdge::staticMetaObject);
	name = QString::fromUtf8("Edge");
	m_items.insert(name, new CDiagramItemType(name, name, QIcon(), meta));

	meta = const_cast<QMetaObject*>(&CDiagramLine::staticMetaObject);
	name = QString::fromUtf8("Line");
	m_items.insert(name, new CDiagramItemType(name, name, QIcon(), meta));

	meta = const_cast<QMetaObject*>(&CDiagramTextNode::staticMetaObject);
	name = QString::fromUtf8("Text");
	m_items.insert(name, new CDiagramItemType(name, name, QIcon(), meta));

	meta = const_cast<QMetaObject*>(&CDiagramSvgNode::staticMetaObject);
	name = QString::fromUtf8("SVG");
	m_items.insert(name, new CDiagramItemType(name, name, QIcon(), meta));

	meta = const_cast<QMetaObject*>(&CDiagramPixmapNode::staticMetaObject);
	name = QString::fromUtf8("Pixmap");
	m_items.insert(name, new CDiagramItemType(name, name, QIcon(), meta));

	meta = const_cast<QMetaObject*>(&CDiagramGroup::staticMetaObject);
	name = QString::fromUtf8("Group");
	m_items.insert(name, new CDiagramItemType(name, name, QIcon(), meta));
}

QPointF NoneArrow::draw(QPainter *painter, const QPointF &p1, const QPointF &p2, QPainterPath &path) const
{
	return p2;
}

QPointF DiamondArrow::draw(QPainter *painter, const QPointF &p1, const QPointF &p2, QPainterPath &path) const
{
	QPen			pen;
	double			length;
	double			angle;
	double			rlength;
	QPointF			t, a1, a2;
	QLineF			line(p1, p2);
	QVector2D		v;
	QPainterPath	localPath;

	pen = painter ? painter->pen() : QPen( Qt::black );
	pen.setStyle(Qt::SolidLine);

	length = m_length + pen.widthF() * 2;
	v = QVector2D( (p2 - p1) ).normalized() * length;
	t = p2 - v.toPointF();

	angle = ::acos(line.dx() / line.length());
	if (line.dy() >= 0)
		angle = TwoPi - angle;

	rlength = length * sqrt(2.0) / 2.0;
	a1 = t + QPointF(sin(angle + Pi / 4.0) * rlength, cos(angle + Pi / 4.0) * rlength);
	a2 = t + QPointF(sin(angle + Pi - Pi / 4.0) * rlength, cos(angle + Pi - Pi / 4.0) * rlength);

	localPath.addPolygon(QPolygonF() << t << a1 << p2 << a2 << t);

	if (painter)
	{
		painter->save();
		painter->setRenderHint(QPainter::Antialiasing, true);
		painter->strokePath(localPath, pen);
		painter->fillPath(localPath, QBrush(Qt::white)); 
		painter->restore();
	}

	path.addPath(localPath);

	if (line.length() >= length) return t;
	else return p1;
}

QPointF TriangleArrow::draw(QPainter *painter, const QPointF &p1, const QPointF &p2, QPainterPath &path) const
{
	QPen			pen;
	QPointF			t1;
	QVector2D		v1, v2( p2 - p1 );
	QPainterPath	localPath;

	pen = painter ? painter->pen() : QPen( Qt::black );
	pen.setStyle(Qt::SolidLine);

	v1 = QVector2D( (p2 - p1) ).normalized() * (m_length + pen.widthF());
	t1 = p2 - v1.toPointF();

	v1 = QVector2D( p2.y() - t1.y(), t1.x() - p2.x() ).normalized() * ((m_width + pen.widthF()) / 2.0);

	localPath.moveTo(t1);
	localPath.lineTo(t1 + v1.toPointF());
	localPath.lineTo(p2);
	localPath.lineTo(t1 - v1.toPointF());
	localPath.lineTo(t1);
	localPath.setFillRule(Qt::WindingFill);

	if (painter)
	{
		painter->save();
		painter->setRenderHint(QPainter::Antialiasing, true);
		painter->strokePath(localPath, pen);
		painter->fillPath(localPath, QBrush(Qt::white));
		painter->restore();
	}
	
	path.addPath(localPath);

	if (v2.length() >= (m_length + pen.widthF())) return t1;
	else return p1;
}


QPointF FilledDiamondArrow::draw(QPainter *painter, const QPointF &p1, const QPointF &p2, QPainterPath &path) const
{
	QPen			pen;
	double			length;
	double			angle;
	double			rlength;
	QPointF			t, a1, a2;
	QLineF			line(p1, p2);
	QVector2D		v;
	QPainterPath	localPath;

	pen = painter ? painter->pen() : QPen( Qt::black );
	length = m_length + pen.widthF() * 2;
	v = QVector2D( (p2 - p1) ).normalized() * length;
	t = p2 - v.toPointF();

	angle = ::acos(line.dx() / line.length());
	if (line.dy() >= 0)
		angle = TwoPi - angle;

	rlength = length * sqrt(2.0) / 2.0;
	a1 = t + QPointF(sin(angle + Pi / 4.0) * rlength, cos(angle + Pi / 4.0) * rlength);
	a2 = t + QPointF(sin(angle + Pi - Pi / 4.0) * rlength, cos(angle + Pi - Pi / 4.0) * rlength);

	localPath.addPolygon(QPolygonF() << t << a1 << p2 << a2 << t);
	
	if (painter)
	{
		painter->save();
		painter->setPen(Qt::NoPen);
		painter->fillPath(localPath, pen.color() );
		painter->restore();
	}

	path.addPath(localPath);

	if (line.length() >= length) return t;
	else return p1;
}

QPointF FilledTriangleArrow::draw(QPainter *painter, const QPointF &p1, const QPointF &p2, QPainterPath &path) const
{
	QPen			pen;
	QPointF			t1;
	QVector2D		v1, v2( p2 - p1 );
	QPainterPath	localPath;

	pen = painter ? painter->pen() : QPen( Qt::black );

	v1 = QVector2D( (p2 - p1) ).normalized() * (m_length + pen.widthF());
	t1 = p2 - v1.toPointF();

	v1 = QVector2D( p2.y() - t1.y(), t1.x() - p2.x() ).normalized() * ((m_width + pen.widthF()) / 2.0);

	localPath.moveTo(t1);
	localPath.lineTo(t1 + v1.toPointF());
	localPath.lineTo(p2);
	localPath.lineTo(t1 - v1.toPointF());
	localPath.lineTo(t1);
	localPath.setFillRule(Qt::WindingFill);

	if (painter)
	{
		painter->save();
		painter->setPen(Qt::NoPen);
		painter->setRenderHint(QPainter::Antialiasing, true);
		painter->fillPath(localPath, pen.color() );
		painter->restore();
	}

	path.addPath(localPath);

	if (v2.length() >= (m_length + pen.widthF())) return t1;
	else return p1;
}

QPointF AngleArrow::draw(QPainter *painter, const QPointF &p1, const QPointF &p2, QPainterPath &path) const
{
	QPen			pen;
	QPointF			t1, t2;
	QVector2D		v1, v2;
	QPainterPath	localPath;

	pen = painter ? painter->pen() : QPen( Qt::black );
	pen.setStyle(Qt::SolidLine);

	v1 = QVector2D( (p2 - p1) ).normalized() * m_length;
	v2 = QVector2D( (p2 - p1) ).normalized() * m_length;
	t1 = p2 - v1.toPointF();
	t2 = t1 + v2.toPointF();

	v1 = QVector2D( t2.y() - t1.y(), t1.x() - t2.x() ).normalized() * ((m_width + pen.widthF()) / 2.0);

	localPath.moveTo( t1 + v1.toPointF() );
	localPath.lineTo(t2);
	localPath.lineTo( t1 - v1.toPointF() );

	if (painter)
	{
		painter->save();
		pen.setCapStyle(Qt::RoundCap);
		pen.setJoinStyle(Qt::RoundJoin);
		if (qRound(pen.widthF()) % 2 == 1)
			painter->translate(0.5, 0.5);
		painter->setRenderHint(QPainter::Antialiasing, true);
		painter->strokePath(localPath, pen);
		painter->restore();
	}

	path.addPath(localPath);
	return p2;
}

QPointF CurveArrow::draw(QPainter *painter, const QPointF &p1, const QPointF &p2, QPainterPath &path) const
{
	QPen			pen;
	QPointF			t1, m1, m2, c1;
	QVector2D		v1, v2( p2 - p1 );
	QPainterPath	localPath;

	pen = painter ? painter->pen() : QPen( Qt::black );
	pen.setStyle(Qt::SolidLine);

	v1 = QVector2D( (p2 - p1) ).normalized() * (m_length + pen.widthF());
	t1 = p2 - v1.toPointF();
	c1 = (p2 + t1) / 2.0;

	v1 = QVector2D( p2.y() - t1.y(), t1.x() - p2.x() ).normalized() * ((m_width + pen.widthF()) / 2.0);

	m1 = t1 + v1.toPointF();
	m2 = t1 - v1.toPointF();

	localPath.moveTo( m1 );
	localPath.quadTo( c1, p2);
	localPath.moveTo( m2 );
	localPath.quadTo( c1, p2 );

	if (painter)
	{
		painter->save();
		pen.setCapStyle(Qt::RoundCap);
		pen.setJoinStyle(Qt::RoundJoin);
		if (qRound(pen.widthF()) % 2 == 1)
			painter->translate(0.5, 0.5);
		painter->setRenderHint(QPainter::Antialiasing, true);
		painter->strokePath(localPath, pen);
		painter->restore();
	}

	path.addPath(localPath);
	return p2;
}

void CCoreArrows::load()
{
	CDiagramArrow		 *s = NULL;
	CDiagramIconProvider *iconProvider = NULL;

	iconProvider = CDiagramIconProvider::instance();

	s = new NoneArrow();
	s->setCategory(m_name);
	s->setName( QString::fromUtf8("None") );
	s->setIcon( iconProvider->getIconForEndArrow( NULL ) );
	m_arrows.insert( s->getName(), s );

	s = new DiamondArrow();
	s->setCategory( m_name );
	s->setName( QString::fromUtf8("Empty Diamond") );
	s->setIcon( iconProvider->getIconForEndArrow( s ) );
	m_arrows.insert( s->getName(), s );

	s = new TriangleArrow();
	s->setCategory( m_name );
	s->setName( QString::fromUtf8("Empty Triangle") );
	s->setIcon( iconProvider->getIconForEndArrow( s ) );
	m_arrows.insert( s->getName(), s );

	s = new FilledDiamondArrow();
	s->setCategory( m_name );
	s->setName( QString::fromUtf8("Filled Diamond") );
	s->setIcon( iconProvider->getIconForEndArrow( s ) );
	m_arrows.insert( s->getName(), s );

	s = new FilledTriangleArrow();
	s->setCategory( m_name );
	s->setName( QString::fromUtf8("Filled Triangle") );
	s->setIcon( iconProvider->getIconForEndArrow( s ) );
	m_arrows.insert( s->getName(), s );

	s = new AngleArrow();
	s->setCategory( m_name );
	s->setName( QString::fromUtf8("Angle") );
	s->setIcon( iconProvider->getIconForEndArrow( s ) );
	m_arrows.insert( s->getName(), s );

	s = new CurveArrow();
	s->setCategory( m_name );
	s->setName( QString::fromUtf8("Curve") );
	s->setIcon( iconProvider->getIconForEndArrow( s ) );
	m_arrows.insert( s->getName(), s );
}

void OrthogonalEdge::draw(QPainter *painter, const QList<QPointF> &points, const CDiagramArrow *start, const CDiagramArrow *end, QPainterPath &path) const
{
	int				r = points.length();
	qreal			penWidth = 0;
	QPointF			p1, p2;
	QPainterPath	stroke, arrow1, arrow2;

	if (painter)
	{
		penWidth = painter->pen().widthF();
		painter->setRenderHint(QPainter::Antialiasing, true);
	}

	p1 = (start != NULL) ? start->draw(painter, points[1], points[0], arrow1)   : points.first();
	p2 = (end   != NULL) ? end->draw(painter, points[r-2], points[r-1], arrow2) : points.last();

	stroke.moveTo( p1 );
	for (int i = 1; i < points.length() - 1; ++i)
	{
		stroke.lineTo( points[i] );
	}
	stroke.lineTo( p2 );

	if (painter)
	{
		painter->setRenderHint(QPainter::Antialiasing, false);
		painter->strokePath(stroke, painter->pen());
	}

	path.setFillRule(Qt::WindingFill);
	path.addPath(arrow1);
	path.addPath(stroke);
	path.addPath(arrow2);
}

void RoundedEdge::draw(QPainter *painter, const QList<QPointF> &points, const CDiagramArrow *start, const CDiagramArrow *end, QPainterPath &path) const
{
	qreal			radius = 6;
	qreal			deg, mark;
	int				r = points.length();
	QPointF			p1, p2, t1, t2, prev;
	QVector2D		v1, v2;
	QPainterPath	stroke, arrow1, arrow2;

	if (painter)
	{
		painter->setRenderHint(QPainter::Antialiasing, true);
		painter->translate(0.5, 0.5);
	}

	p1 = (start != NULL) ? start->draw(painter, points[1], points[0], arrow1)   : points.first();
	p2 = (end   != NULL) ? end->draw(painter, points[r-2], points[r-1], arrow2) : points.last();

	stroke.moveTo( p1 );
	prev = p1;
	mark = radius;
	for (int i = 1; i < points.length() - 1; ++i)
	{
		v1 = QVector2D(points[i-1] - points[i]);
		v2 = QVector2D(points[i+1] - points[i]);
		deg = qMin(radius, qMax(qAbs(points[i].x() - points[i+1].x()), qAbs(points[i].y() - points[i+1].y()) ) / 2.0 );
		v1.normalize();
		v2.normalize();
		if (deg <= mark)
		{
			t1 = points[i] + (v1 * deg).toPointF();
		}
		else
		{
			t1 = prev;
			deg = mark;
		}		
		t2 = points[i] + (v2 * deg).toPointF();
		stroke.lineTo(t1);
		stroke.quadTo(points[i], t2);
		prev = t2;
		mark = deg;
	}
	stroke.lineTo( p2 );

	if (painter)
	{
		painter->strokePath(stroke, painter->pen());
	}

	path.setFillRule(Qt::WindingFill);
	path.addPath(arrow1);
	path.addPath(stroke);
	path.addPath(arrow2);
}

void CurveEdge::draw(QPainter *painter, const QList<QPointF> &points, const CDiagramArrow *start, const CDiagramArrow *end, QPainterPath &path) const
{
	qreal			deg = 5;
	int				r = points.length();
	QPointF			p1, p2, t;
	QPainterPath	stroke, arrow1, arrow2;

	if (painter)
		painter->setRenderHint(QPainter::Antialiasing, true);
	
	p1 = (start != NULL) ? start->draw(painter, points[1], points[0], arrow1)   : points.first();
	p2 = (end   != NULL) ? end->draw(painter, points[r-2], points[r-1], arrow2) : points.last();

	stroke.moveTo( p1 );
	for (int i = 1; i < points.length() - 1; ++i)
	{
		if (i == points.length() - 2)
			t = p2;
		else
			t = (points[i] + points[i+1]) / 2.0;
		stroke.quadTo(points[i], t);
	}

	//path.lineTo( p2 );
	if (painter)
	{
		painter->strokePath(stroke, painter->pen());
	}

	path.setFillRule(Qt::WindingFill);
	path.addPath(arrow1);
	path.addPath(stroke);
	path.addPath(arrow2);
}

void StraightEdge::route(const QPointF &p1, int d1, const QPointF &p2, int d2, QList<QPointF> &points) const
{
	points.push_back(p1);
	points.push_back(p2);
}

void StraightEdge::draw(QPainter *painter, const QList<QPointF> &points, const CDiagramArrow *start, const CDiagramArrow *end, QPainterPath &path) const
{
	int				r = points.length();
	QPointF			p1, p2;
	QPainterPath	stroke, arrow1, arrow2;

	if (painter)
	{
		painter->setRenderHint(QPainter::Antialiasing, true);
		//painter->translate(0.5, 0.5);
	}

	p1 = (start != NULL) ? start->draw(painter, points[1], points[0], arrow1)   : points.first();
	p2 = (end   != NULL) ? end->draw(painter, points[r-2], points[r-1], arrow2) : points.last();

	stroke.moveTo( p1 );
	stroke.lineTo( p2 );

	if (painter)
	{
		painter->strokePath(stroke, painter->pen());
	}

	path.setFillRule(Qt::WindingFill);
	path.addPath(arrow1);
	path.addPath(stroke);
	path.addPath(arrow2);
}

void ArcEdge::route(const QPointF &p1, int d1, const QPointF &p2, int d2, QList<QPointF> &points) const
{
	points.push_back(p1);
	points.push_back(p2);
}

void ArcEdge::draw(QPainter *painter, const QList<QPointF> &points, const CDiagramArrow *start, const CDiagramArrow *end, QPainterPath &path) const
{
	int				r = points.length();
	QPointF			p1, p2;
	QPainterPath	stroke, arrow1, arrow2;

	if (painter)
	{
		painter->setRenderHint(QPainter::Antialiasing, true);
		//painter->translate(0.5, 0.5);
	}

	p1 = points[0];
	p2 = points[1];

	stroke.moveTo( p1 );
	stroke.quadTo(QPointF((p1.x() + p2.x()) / 2.0, p2.y()), p2);

	if (painter)
	{
		painter->strokePath(stroke, painter->pen());
	}

	p1 = (start != NULL) ? start->draw(painter, stroke.pointAtPercent(0.2), p1, arrow1)   : points.first();
	p2 = (end   != NULL) ? end->draw(painter, stroke.pointAtPercent(0.9), p2, arrow2) : points.last();

	path.setFillRule(Qt::WindingFill);
	path.addPath(arrow1);
	path.addPath(stroke);
	path.addPath(arrow2);
}

void CCoreEdges::load()
{
	int					 s1 = Global::East, s2 = Global::West;
	QPointF				 p1(-8, -8), p2(8, 8);
	QList<QPointF>		 ps;
	QPainterPath		 path;
	QPainterPathStroker  stroker;
	CDiagramEdgeStyle	 *r = NULL;
	CDiagramIconProvider *iconProvider = NULL;

	iconProvider = CDiagramIconProvider::instance();

	r = new StraightEdge( m_name, "Straight Line", QIcon() );
	r->setIcon( iconProvider->getIconForEdgeStyle(r) );
	m_styles.insert( r->getName(), r );

	ps.clear();
	path = QPainterPath();
	
    r = new OrthogonalEdge(m_name, "Orthogonal Lines", QIcon() );
	r->setIcon( iconProvider->getIconForEdgeStyle(r) );
	m_styles.insert( r->getName(), r );

	ps.clear();
	path = QPainterPath();
	
	r = new CurveEdge(m_name, "Curve", QIcon() );
	r->setIcon( iconProvider->getIconForEdgeStyle(r) );
	m_styles.insert( r->getName(), r );

	ps.clear();
	path = QPainterPath();

    r = new RoundedEdge(m_name, "Rounded Lines", QIcon());
	r->setIcon( iconProvider->getIconForEdgeStyle(r) );
	m_styles.insert( r->getName(), r );

	ps.clear();
	path = QPainterPath();

	r = new ArcEdge(m_name, "Arc", QIcon());
	r->setIcon( iconProvider->getIconForEdgeStyle(r) );
	m_styles.insert( r->getName(), r );
}
