/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <core/CDiagram>
#include <core/CDiagramText>
#include <core/CDiagramNode>
#include <core/CDiagramTextNode>

CDiagramTextNode::CDiagramTextNode(CDiagram *parent)
	: CDiagramNode(parent),
	  m_editing(false)
{
	QRectF		r;
	
	setRotatable(false);
	setConnectable(false);
	setCategory( QString::fromUtf8("Core") );
	setName( QString::fromUtf8("Text") );

	m_label->setAutoWidth(false);

	m_brush = QBrush(QColor(0, 0, 0, 0));
	m_pen.setColor( QColor(0, 0, 0, 0) );

	connect(this, SIGNAL(addedToDiagram()), this, SLOT(onAddedToDiagram()));
	connect(m_label, SIGNAL(sizeAutoChanged()), this, SLOT(onTextSizeChanged()));
}

CDiagramTextNode::~CDiagramTextNode()
{
}

QRectF CDiagramTextNode::boundingRect() const
{
	QRectF	r;

	r = CDiagramNode::boundingRect();
	if (m_coordMode != CDiagramItem::CenterAsOrigin)
		r = r.translated( -r.topLeft() );
	return r;
}

QRectF CDiagramTextNode::contentsRect() const
{
	QRectF		r;

	r = CDiagramNode::contentsRect();
	if (m_coordMode != CDiagramItem::CenterAsOrigin)
		r = r.translated( -r.topLeft() );
	return r;
}

void CDiagramTextNode::setEditing(bool editing)
{
	if (editing && m_label)
		m_label->setEditing();
}

void CDiagramTextNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	qreal	w = width() / 2.0;
	qreal	h = height() / 2.0;

	painter->save();
	painter->setBrush(m_brush);
	painter->setPen(m_pen);
	if (coordinationMode() != CDiagramItem::CenterAsOrigin)
		painter->translate(width() / 2.0, height() / 2.0);
	painter->drawRect(-w, -h, width(), height());
	painter->restore();
}

void CDiagramTextNode::onAddedToDiagram()
{
	m_p->clearSelection();
	m_brush = QBrush(Qt::NoBrush);
	m_pen.setColor( QColor(0, 0, 0, 0) );
	setEditing(true);
}


void CDiagramTextNode::setHtml(const QString &value)
{
	if (m_label)
		m_label->setText( value );
	resize( m_label->boundingRect().size() );
}

QString CDiagramTextNode::html()
{
	QString		label;

	if (m_label) 
		label = m_label->text();
	return label;
}

void CDiagramTextNode::setPlainText(const QString &value)
{
	if (m_label)
		m_label->setPlainText(value);
	update();
}

QString CDiagramTextNode::plainText()
{
	QString		r;

	if (m_label)
		r = m_label->toPlainText();
	return r;
}

void CDiagramTextNode::layoutContent()
{
	QRectF	r1, r2;

	r1 = contentsRect();
	r2 = m_label->boundingRect();
	m_label->setGeometry( QRectF( QPointF(-r1.width() / 2.0, -r2.height() / 2.0), r1.size() ) );
}

void CDiagramTextNode::onTextSizeChanged()
{
	QRectF		r1, r2;

	r1 = geometry();
	r2 = m_label->boundingRect();
	resize( qMax(r1.width(), r2.width()), qMax(r1.height(), r2.height()) );
}

void CDiagramTextNode::startGrowing(const QPointF &p)
{
	setPos(p);
}

bool CDiagramTextNode::onGrowing(const QPointF &p)
{
	resize(120, 24);
	return true;
}

bool CDiagramTextNode::stopGrowing(const QPointF &p)
{
	setGeometry( QRectF(p, QSizeF(80, 24)) );
	return true;
}

void CDiagramTextNode::setTextStyleable(bool value)
{
	m_label->setStyleable(value);
}

bool CDiagramTextNode::textStyleable() const
{
	return m_label->styleable();
}