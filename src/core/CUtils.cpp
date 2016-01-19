/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CUtils>
#include <QtXml/QtXml>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramNode>
#include <core/CDiagramEdge>
#include <core/CDiagramWidget>
#include <core/CDiagramEdgeStyle>
#include <core/CDiagramItemTypeCategory>
#include <core/CDiagramItemTypeDatabase>

void Utils::drawEdgeStyleHelper(QPainter *painter, CDiagramEdgeStyle *style, const QRectF &target, qreal padding)
{
	int				s1 = Global::East, s2 = Global::West;
	QRectF			r;
	QPointF			p1, p2;
	QPainterPath	path;
	QList<QPointF>	ps;

	r = target;
	r.adjust(padding, padding, -padding, -padding);
	if (style && painter)
	{
		p1 = r.topLeft();
		p2 = r.bottomRight();
		style->route(p1, s1, p2, s2, ps);
		style->draw(painter, ps, NULL, NULL, path);
	}
}

QSizeF Utils::getPageSize(QPrinter::PageSize type, bool landscape)
{
	qreal				w, h, t;
	qreal				pixelPerXInch = 0;
	qreal				pixelPerYInch = 0;
	QDesktopWidget		*widget = NULL;

	widget = QApplication::desktop();
	pixelPerXInch = widget->logicalDpiX();
	pixelPerYInch = widget->logicalDpiY();

	switch(type)
	{
	default:
	case QPrinter::A3:
		w = 11.69;
		h = 16.54;
		break;

	case QPrinter::A4:
		w = 8.26;
		h = 11.69;
		break;

	case QPrinter::B5:
		w = 6.93;
		h = 9.84;
		break;

	case QPrinter::Letter:
		w = 8.5;
		h = 11;
		break;
	};

	if (landscape)
	{
		t = w;
		w = h;
		h = t;
	}
	return QSizeF( w * pixelPerXInch, h * pixelPerYInch );
}

QDomElement Utils::genXmlElement(QDomDocument &doc, const QString &name, const QString &text)
{
	QDomElement		e;

	e = doc.createElement( name );
	e.appendChild( doc.createTextNode( text ) );
	return e;
}

bool Utils::stringToBool(const QString &value)
{
	bool		r;
	QString		v;

	r = true;
	v = value.trimmed();
	if (v == QString::fromUtf8("false") || v.isEmpty() || v == QString::fromUtf8("0")) 
		r = false;
	return r;
}

QString Utils::boolToString(bool value)
{
	if (value)
		return QString::fromUtf8("true");
	return QString::fromUtf8("false");
}

bool Utils::isValidWebUrl(const QString &value)
{
	QUrl		url;
	QString		scheme;
	QStringList	protocols;

	protocols << "http" << "https" << "ftp";
	url.setUrl(value);
	scheme = url.scheme().toLower();
	return (url.isValid() && protocols.contains(scheme) );
}

bool Utils::isValidLocalPath(const QString &value)
{
	QUrl		url;
	
	url.setUrl(value);
	return (url.isValid() && (url.isRelative() || url.scheme().toLower() == "file") );
}

QPoint Utils::getPopupPoint(QGraphicsScene *scene, QWidget *hostedWidget, QToolButton *button, QWidget *widget)
{
    qreal                   x, y;
    QRectF                  sc, r0;
    QRect                   menuRect, buttonRect;
    QPoint                  p;
    CDiagram                *diagram = NULL;
    QGraphicsView           *v = NULL;
    QWidget                 *parent = NULL;
    CDiagramHostedWidget    *hosted = NULL;
    QGraphicsItem           *item = NULL;

    diagram = dynamic_cast<CDiagram*>(scene);
    hosted = dynamic_cast<CDiagramHostedWidget*>( hostedWidget );
    parent = button->parentWidget();
    if (!diagram || !widget || !parent || !hosted)
        return QPoint();

    v = diagram->getMainView();
    if (!v)
        return QPoint();

    item = hosted->diagramWidget();
    menuRect = widget->geometry();
    buttonRect = button->geometry();
    p = parent->mapTo(hostedWidget, button->pos());
    sc = v->mapToScene(v->viewport()->geometry()).boundingRect();
    r0 = item->mapToScene( item->boundingRect() ).boundingRect();
    r0 = QRectF(r0.topLeft() + QPointF(p.x(), p.y()), QSizeF(buttonRect.width(), buttonRect.height()));

    // determine x
    if (sc.right() - menuRect.width() > r0.left())
        x = r0.left();
    else
        x = sc.right() - menuRect.width();

    // determine y
    if (sc.bottom() - menuRect.height() > r0.bottom() )
        y = r0.bottom();
    else
        y = sc.bottom() - menuRect.height();

    return v->mapToGlobal( v->mapFromScene( QPointF(x, y) ) );
}


bool Utils::getXmlFromDiagram(const CDiagram *d, QDomElement &n)
{
    QRectF					r;
    CDiagramItem			*item = NULL;
    QList<CDiagramItem*>	diagramItems;
    QList<QDomElement>		edges;
    QDomDocument            doc;
    QDomElement				items, e;

    doc = n.ownerDocument();

    r = d->sceneRect();
    e = doc.createElement( "width" );
    e.appendChild( doc.createTextNode( QString::number(r.width()) ) );
    n.appendChild( e );

    doc.createElement( "height" );
    e.appendChild( doc.createTextNode( QString::number(r.height()) ) );
    n.appendChild( e );

    items = doc.createElement( "items" );
    n.appendChild(items);

    diagramItems = const_cast<CDiagram*>(d)->topLevelItems();
    for (int i = 0; i < diagramItems.length(); ++i)
    {
        item = diagramItems.at(i);
        e = doc.createElement( "item" );
        item->toXml( e );

        if (item->interType() != Global::Edge)
            items.appendChild(e);
        else
            edges.push_back(e);
    }

    // save edges at the last
    for (int i = 0;i < edges.length(); ++i)
    {
        items.appendChild( edges.at(i) );
    }

    return true;
}

bool Utils::getDiagramFromXml(const QDomElement &n, CDiagram *d)
{
    bool			r = false;
    qreal			w, h;
    QDomElement		element;

    if (!d)
        return r;

    // load all the items
    w = n.firstChildElement( "width" ).text().toFloat();
    h = n.firstChildElement( "height" ).text().toFloat();
    //d->setSceneRect(-w/2.0, -h/2.0, w, h);

    element = n.firstChildElement( "items" );
    element = element.firstChildElement( "item" );
    while (!element.isNull())
    {
        QString						type, id, ctg, name;
        CDiagramItem				*item = NULL;
        CDiagramItemTypeCategory	*factory = NULL;

        type = element.attribute("type");
        id = element.attribute("id");
        ctg = element.attribute("category");
        name = element.attribute("name");

        factory = CDiagramItemTypeDatabase::instance()->getCategory(ctg);
        if (factory && d)
        {
            item = factory->createItem(name, Q_ARG(CDiagram*, d));
            if (item)
            {
                item->setId( id.toInt() );
                d->insertItem(item, item->pos(), false, true);
                item->fromXml(element);
            }
            else
            {
                qDebug() << "NULL item for " << ctg << " " << name;
            }
        }
        element = element.nextSiblingElement( QString::fromUtf8("item") );
    }

    d->refreshStackOrder();
    d->emitItemsLoadFinished();

    return r;
}

void Utils::setFontIcon(QToolButton *btn, const QChar &code, int ps, const QString &family)
{
	if (!btn)
		return;

	btn->setToolButtonStyle(Qt::ToolButtonTextOnly);
	btn->setFont( QFont(family, ps) );
	btn->setText( code );
}

void Utils::setFontIcon(QPushButton *btn, const QChar &code, int ps, const QString &family)
{
	if (!btn)
		return;

	btn->setFont( QFont(family, ps) );
	btn->setText( code );
	btn->setIcon( QIcon() );
}