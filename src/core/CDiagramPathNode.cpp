/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramPathNode>

CDiagramPathNode::CDiagramPathNode(CDiagram *parent)
	: CDiagramNode(parent)
{
	QGraphicsDropShadowEffect	*effect = NULL;

	effect = new QGraphicsDropShadowEffect();
	effect->setColor( QColor(0, 0, 0, 32) );
	effect->setOffset(2, 3);

	setGraphicsEffect(effect);
	setEffectEnabled(false);

	if (labelPosition() == Center)
		m_label->setAutoWidth(false);
}

CDiagramPathNode::~CDiagramPathNode()
{
}

QPainterPath CDiagramPathNode::shape() const
{
	QPainterPath	r;
	QPainterPath	path, tmp;

	if (m_label && (const_cast<CDiagramPathNode*>(this))->labelPosition() != CDiagramNode::Center)
		r = mapFromScene( m_label->mapToScene(m_label->shape()) );

	path.setFillRule(Qt::WindingFill);
	if (m_path.isEmpty())
	{
		tmp = const_cast<CDiagramPathNode*>(this)->computePath();
		path.addPath(tmp);
	}	
	else
	{
		path.addPath(m_path);
	}
	path.addPath(r);

	return path;
}

QPainterPath CDiagramPathNode::computePath()
{
	return QPainterPath();
}

void CDiagramPathNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPen	 s;

	m_path = computePath();
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);
	/*
	if (option && (option->state & QStyle::State_Selected)) 
	{
		s = m_stroke;
		s.setJoinStyle(Qt::MiterJoin);
		s.setMiterLimit(1);
		s.setWidthF(m_pen.widthF() + m_stroke.widthF());
		//painter->setPen( s );
		painter->strokePath(m_path, s);
	}
	*/

	m_pen.setJoinStyle(Qt::MiterJoin);
	m_pen.setMiterLimit(1);
	painter->setBrush( m_brush );
	painter->setPen( m_pen );
	
	// draw the shadow
	/*
	painter->translate(3,3);
	painter->fillPath(m_path, QBrush(QColor(0,0,0,32)));
	
	painter->translate(-2.5, -2.5);
	*/

	painter->translate(0.5, 0.5);
	painter->drawPath( m_path );

	//painter->setPen(Qt::red);
	//painter->setBrush(Qt::NoBrush);
	//painter->drawRect( boundingRect() );
	painter->restore();
}

void CDiagramPathNode::setClosed(bool value)
{
	m_closed = value;
}