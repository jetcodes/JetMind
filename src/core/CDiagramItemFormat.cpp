/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CGlobal>
#include <core/CDiagramEdge>
#include <core/CDiagramItem>
#include <core/CDiagramNode>
#include <core/CDiagramGroup>
#include <core/CDiagramCommands>
#include <core/CDiagramArrowDatabase>
#include <core/CDiagramArrowCategory>
#include <core/CDiagramItemFormat>
#include <core/CDiagramIconProvider>

#define FORMAT_TYPE_ID 1

class CDiagramItemFormatPrivate
{
public:
	virtual int type() = 0;
	virtual QIcon icon() = 0;
	virtual QString desc() = 0;

	virtual void toStream(QDataStream &stream) = 0;
	virtual void fromStream(QDataStream &stream) = 0;

	virtual bool initFrom(CDiagramItem* item) = 0;
	virtual bool support(CDiagramItem *item) = 0;
	virtual void apply(CDiagramItem *item) = 0;
}; // End of class CDiagramItemFormatPrivate

class CDiagramItemPen: public CDiagramItemFormatPrivate
{
public:

	int type()
	{
		return FORMAT_TYPE_ID + 1;
	}

	QIcon icon() 
	{ 
		return CDiagramIconProvider::instance()->getIconForPen(m_pen); 
	}
	
	QString desc() 
	{ 
		return QString("Pen"); 
	}

	void toStream(QDataStream &stream) 
	{ 
		stream << m_pen; 
	}
	
	void fromStream(QDataStream &stream) 
	{
		stream >> m_pen; 
	}
	
	bool initFrom(CDiagramItem* item) 
	{ 
		m_pen = item->pen(); 
		return true; 
	}
	
	bool support(CDiagramItem *item) 
	{ 
		return true;
	}
	
	void apply(CDiagramItem *item) 
	{ 
		CDiagram				*d = NULL;
		QList<CDiagramItem*>	items;

		d = dynamic_cast<CDiagram*>( item->scene() );
		if (d)
		{
			items << item;
			d->addUndoCommand(new CChangePropertyCommand(d, items, "pen", QVariant(m_pen)));
		}
	}

private:
	QPen		m_pen;
};

class CDiagramItemBrush: public CDiagramItemFormatPrivate
{
public:
	int type()
	{
		return FORMAT_TYPE_ID + 2;
	}

	QIcon icon() 
	{
		return CDiagramIconProvider::instance()->getIconForBrush(m_brush); 
	}
	
	QString desc() 
	{ 
		return QString("Brush"); 
	}

	void toStream(QDataStream &stream) 
	{
		stream << m_brush; 
	}

	void fromStream(QDataStream &stream)
	{
		stream >> m_brush; 
	}
	
	bool initFrom(CDiagramItem* item) 
	{
		m_brush = item->brush(); 
		return true; 
	}
	
	bool support(CDiagramItem *item) 
	{
		return true; 
	}
	
	void apply(CDiagramItem *item) 
	{ 
		CDiagram				*d = NULL;
		QList<CDiagramItem*>	items;

		d = dynamic_cast<CDiagram*>( item->scene() );
		if (d)
		{
			items << item;
			d->addUndoCommand(new CChangePropertyCommand(d, items, "brush", QVariant(m_brush)));
		}
	}

private:
	QBrush		m_brush;
};

class CDiagramItemFont: public CDiagramItemFormatPrivate
{
public:
	int type()
	{
		return FORMAT_TYPE_ID + 3;
	}

	QIcon icon() 
	{
		return CDiagramIconProvider::instance()->getIconForFont(m_font); 
	}
	
	QString desc() 
	{ 
		return QString("Font"); 
	}

	void toStream(QDataStream &stream)
	{ 
		stream << m_font; 
	}
	
	void fromStream(QDataStream &stream) 
	{ 
		stream >> m_font; 
	}
	
	bool initFrom(CDiagramItem* item) 
	{ 
		m_font = item->font(); 
		return true; 
	}
	
	bool support(CDiagramItem *item) 
	{
		return true; 
	}
	
	void apply(CDiagramItem *item) 
	{ 
		CDiagram				*d = NULL;
		QList<CDiagramItem*>	items;

		d = dynamic_cast<CDiagram*>( item->scene() );
		if (d)
		{
			items << item;
			d->addUndoCommand(new CChangePropertyCommand(d, items, "font", QVariant(m_font)));
		}
	}

private:
	QFont		m_font;
};

class CDiagramItemStartArrow: public CDiagramItemFormatPrivate
{
public:
	int type()
	{
		return FORMAT_TYPE_ID + 4;
	}

	CDiagramItemStartArrow()
		: m_arrow(NULL)
	{
	}

	QIcon icon() 
	{ 
		return CDiagramIconProvider::instance()->getIconForStartArrow(m_arrow); 
	}
	
	QString desc() 
	{ 
		return QString("Start Arrow"); 
	}

	void toStream(QDataStream &stream)
	{
		if (m_arrow)
			stream << m_arrow->getCategory() << m_arrow->getName();
	}

	void fromStream(QDataStream &stream)
	{ 
		
		QString					category, name;
		CDiagramArrowCategory	*ctg = NULL;

		stream >> category >> name;
		ctg = CDiagramArrowDatabase::instance()->getCategory(category);
		if (ctg)
			m_arrow = ctg->getArrow(name);
	}
	
	bool initFrom(CDiagramItem* item)
	{
		CDiagramEdge	*edge = NULL;

		edge = dynamic_cast<CDiagramEdge*>(item);
		if (edge)
			m_arrow = edge->startArrow();
		return true;
	}

	bool support(CDiagramItem *item)
	{
		bool			r = false;
		CDiagramEdge	*edge = NULL;

		edge = dynamic_cast<CDiagramEdge*>(item);
		r = (edge != NULL);
		return r;
	}

	void apply(CDiagramItem *item) 
	{
		CDiagram				*d = NULL;
		CDiagramEdge			*edge = NULL;
		QList<CDiagramEdge*>	edges;

		d = dynamic_cast<CDiagram*>( item->scene() );
		if (d)
		{
			edge = dynamic_cast<CDiagramEdge*>(item);
			if (edge)
			{
				edges << edge;
				d->addUndoCommand(new CChangeStartArrowCommand(d, edges, m_arrow) );
			}
		}
	}

private:
	CDiagramArrow	*m_arrow;
};


class CDiagramItemEndArrow: public CDiagramItemFormatPrivate
{
public:
	int type()
	{
		return FORMAT_TYPE_ID + 5;
	}

	CDiagramItemEndArrow()
		: m_arrow(NULL)
	{
	}

	QIcon icon() 
	{ 
		return CDiagramIconProvider::instance()->getIconForEndArrow(m_arrow); 
	}
	
	QString desc() 
	{ 
		return QString("End Arrow"); 
	}

	void toStream(QDataStream &stream)
	{
		if (m_arrow)
			stream << m_arrow->getCategory() << m_arrow->getName();
	}

	void fromStream(QDataStream &stream)
	{ 
		
		QString					category, name;
		CDiagramArrowCategory	*ctg = NULL;

		stream >> category >> name;
		ctg = CDiagramArrowDatabase::instance()->getCategory(category);
		if (ctg)
			m_arrow = ctg->getArrow(name);
	}
	
	bool initFrom(CDiagramItem* item)
	{
		CDiagramEdge	*edge = NULL;

		edge = dynamic_cast<CDiagramEdge*>(item);
		if (edge)
			m_arrow = edge->endArrow();
		return true;
	}

	bool support(CDiagramItem *item)
	{
		bool			r = false;
		CDiagramEdge	*edge = NULL;

		edge = dynamic_cast<CDiagramEdge*>(item);
		r = (edge != NULL);
		return r;
	}

	void apply(CDiagramItem *item) 
	{
		CDiagram				*d = NULL;
		CDiagramEdge			*edge = NULL;
		QList<CDiagramEdge*>	edges;

		d = dynamic_cast<CDiagram*>( item->scene() );
		if (d)
		{
			edge = dynamic_cast<CDiagramEdge*>(item);
			if (edge)
			{
				edges << edge;
				d->addUndoCommand(new CChangeEndArrowCommand(d, edges, m_arrow) );
			}
		}
	}

private:
	CDiagramArrow	*m_arrow;
};

CDiagramItemFormat::CDiagramItemFormat()
	: m_p(NULL)
{
}

QIcon CDiagramItemFormat::icon()
{
	QIcon	c;

	if (m_p)
		c = m_p->icon();
	return c;
}

QString CDiagramItemFormat::desc()
{
	QString		s;

	if (m_p)
		s = m_p->desc();
	return s;
}

void CDiagramItemFormat::toStream(QDataStream &stream)
{
	if (m_p)
	{
		stream << m_p->type();
		m_p->toStream(stream);
	}
}

void CDiagramItemFormat::fromStream(QDataStream &stream)
{
	int type;

	stream >> type;
	if (!m_p)
	{
		switch(type)
		{
		case FORMAT_TYPE_ID + 1:
			m_p = new CDiagramItemPen();
			break;
		
		case FORMAT_TYPE_ID + 2:
			m_p = new CDiagramItemBrush();
			break;

		case FORMAT_TYPE_ID + 3:
			m_p = new CDiagramItemFont();
			break;

		case FORMAT_TYPE_ID + 4:
			m_p = new CDiagramItemStartArrow();
			break;

		case FORMAT_TYPE_ID + 5:
			m_p = new CDiagramItemEndArrow();
			break;

		default:
			break;
		};
	}

	if (m_p)
		m_p->fromStream(stream);
}

bool CDiagramItemFormat::initFrom(CDiagramItem* item)
{
	bool	r = false;

	if (m_p)
		r = m_p->initFrom(item);
	return r;
}

bool CDiagramItemFormat::support(CDiagramItem *item)
{
	bool	r = false;

	if (m_p)
		r = m_p->support(item);
	return r;
}

void CDiagramItemFormat::apply(CDiagramItem *item)
{
	if (m_p)
		m_p->apply(item);
}

QList<CDiagramItemFormat*> CDiagramItemFormat::formats(CDiagramItem *item)
{
	CDiagramItemFormat			*format = NULL;
	QList<CDiagramItemFormat*>	fs;

	if (item->interType() != Global::Edge && item->interType() != Global::Node)
	{
		return fs;
	}

	format = new CDiagramItemFormat();
	format->m_p = new CDiagramItemPen();
	format->initFrom(item);
	fs.push_back(format);

	/*
	format = new CDiagramItemFormat();
	format->m_p = new CDiagramItemFont();
	format->initFrom(item);
	fs.push_back(format);
	*/

	if (item->interType() == Global::Edge)
	{
		CDiagramEdge	*edge = NULL;
		
		edge = dynamic_cast<CDiagramEdge*>(item);
		if (edge)
		{
			format = new CDiagramItemFormat();
			format->m_p = new CDiagramItemStartArrow();
			format->initFrom(item);
			fs.push_back(format);

			format = new CDiagramItemFormat();
			format->m_p = new CDiagramItemEndArrow();
			format->initFrom(item);
			fs.push_back(format);
		}
	}
	else
	{
		format = new CDiagramItemFormat();
		format->m_p = new CDiagramItemBrush();
		format->initFrom(item);
		fs.push_back(format);
	}

	return fs;
}