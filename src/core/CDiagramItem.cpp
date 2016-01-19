/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>

#include <core/CDiagram>
#include <core/CDiagramText>
#include <core/CDiagramItem>
#include <core/CDiagramItemTypeCategory>
#include <core/CDiagramItemTypeDatabase>
#include <core/CDiagramLayer>
#include <core/CDiagramPlugin>

CDiagramItem::CDiagramItem(CDiagram *parent)
	: QGraphicsWidget(NULL),
	  m_p(parent),
	  m_stroke( QPen(QColor(0, 0, 0, 48), 8) ),
#if defined(__APPLE__) && defined(__MACH__)
      m_font( QFont("Arial", 18, QFont::Normal, false) ),
#else
      m_font( QFont("Arial", 12, QFont::Normal, false) ),
#endif //
	  m_textColor( Qt::black ),
	  m_textAlign( (int)Qt::AlignHCenter ),
	  m_resizable(true),
	  m_rotatable(true),
	  m_coordMode(CDiagramItem::CenterAsOrigin),
	  m_keepSizeRatio(false),
	  m_sizeRatio(1.0),
	  m_contextMenu(NULL),
	  m_editor(NULL),
	  m_label(NULL),
	  m_id(-1)
{
	QLinearGradient				g(0,0,1,1);
	
	g.setCoordinateMode(QGradient::ObjectBoundingMode);
	g.setColorAt(0.0, Qt::white);
	g.setColorAt(1.0, QColor(204, 204, 204, 128) );

	setFlag(ItemIsSelectable);
	setFlag(ItemIsMovable);
	setFlag(ItemIsFocusable);
	setFlag(ItemSendsGeometryChanges);
	resize(100,100);

	m_label = new CDiagramText(this);
	m_label->setPlainText( QString("") );

	m_brush = QBrush( g );
	m_pen = QPen(QColor("#999999"), 1);

	setCursor( Qt::SizeAllCursor );
	DS_PROPERTY(m_label);
}

CDiagramItem::~CDiagramItem()
{
	SAFE_DELETE(m_label);
}


void CDiagramItem::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
	if (!m_p)
		return;

	if (event->button() == Qt::LeftButton && (flags() & QGraphicsItem::ItemIsSelectable))
	{
		m_p->m_pdata->m_pressedItem = this;
	}

	// Press event handle in QGraphicsItem will help
	// with the multiple selection, so it should be called.
	QGraphicsItem::mousePressEvent(event);
}

void CDiagramItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (!m_p)
		return;

	m_p->m_pdata->m_pressedItem = NULL;
	m_p->m_pdata->m_draggedItems.clear();

	// mouse release handler in QGraphicsItem should be called
	// to handle multiple selection release.
	QGraphicsItem::mouseReleaseEvent(event);
}

void CDiagramItem::init(const QVariant &value)
{
	// TO-BE IMPLEMENTED IN SUB-CLASS
}

void CDiagramItem::setResizable(bool value)
{
	m_resizable = value;
	emit propertyChanged(QString("resizable"), qVariantFromValue(value) );
}

bool CDiagramItem::resizable()
{
	return m_resizable;
}

bool CDiagramItem::rotatable()
{
	return m_rotatable;
}

void CDiagramItem::setRotatable(bool value)
{
	m_rotatable = value;

	emit propertyChanged(QString("rotatable"), qVariantFromValue(value) );
}

qreal CDiagramItem::sizeRatio()
{
	qreal	w = width();
	qreal	h = height();
	qreal	r = 1.0;

	r = h == 0 ? r : w / h;
	r = r == 0 ? 1.0 : r;

	return r;
}

void CDiagramItem::setKeepSizeRatio(bool value)
{
	m_keepSizeRatio = value;

	emit propertyChanged(QString("keepSizeRatio"), qVariantFromValue(value) );
}

void CDiagramItem::setLockPosition(bool value)
{
	setFlag(QGraphicsItem::ItemIsMovable, !value);
	emit propertyChanged(QString("lockPosition"), qVariantFromValue(value) );
}

bool CDiagramItem::lockPosition()
{
	return !(flags() & QGraphicsItem::ItemIsMovable); 
}

void CDiagramItem::setId(int value)
{
	m_id = value;
}

int CDiagramItem::id() const
{
	return m_id;
}

void CDiagramItem::setCategory(const QString &value)
{
	m_category = value;
}

QString CDiagramItem::category()
{
	return m_category;
}

void CDiagramItem::setName(const QString &value)
{
	m_name = value;
}

QString CDiagramItem::name()
{
	return m_name;
}

void CDiagramItem::setLibraryCategory(const QString &value)
{
	m_libCategory = value;
}

QString CDiagramItem::libraryCategory()
{
	return m_libCategory;
}

void CDiagramItem::setLibraryName(const QString &value)
{
	m_libName = value;
}

QString CDiagramItem::libraryName()
{
	return m_libName;
}

qreal CDiagramItem::width() const
{
	return size().width();
}

qreal CDiagramItem::height() const
{
	return size().height();
}

void CDiagramItem::setPos(const QPointF &p)
{
	setGeometry( QRectF(p, size()) );
}

void CDiagramItem::setPos(qreal x, qreal y)
{
	setGeometry( QRectF(QPointF(x,y), size()) );
}

void CDiagramItem::setPen(const QPen &value)
{
	m_pen = value;
	update();
	emit propertyChanged(QString("pen"), qVariantFromValue(value) );
}

void CDiagramItem::setBrush(const QBrush &value)
{
	m_brush = value;
	update();
	emit propertyChanged(QString("brush"), qVariantFromValue(value) );
}

void CDiagramItem::setStroke(const QPen &value)
{
	m_stroke = value;
	update();
	emit propertyChanged(QString("stroke"), qVariantFromValue(value) );
}

void CDiagramItem::setFont(const QFont &value)
{
	m_font = value;
	if (m_label)
		m_label->setFont( value );
	updateChildrenProperty(QString("font"), qVariantFromValue(value) );
	update();
	emit propertyChanged(QString("font"), qVariantFromValue(value) );
}

void CDiagramItem::setEffectEnabled(bool value)
{
	QGraphicsEffect		*effect = NULL;

	effect = graphicsEffect();
	if (effect)
	{
		effect->setEnabled(value);
		update();
		emit propertyChanged(QString("effectEnabled"), qVariantFromValue(value) );
	}
}

bool CDiagramItem::effectEnabled()
{
	bool				r = false;
	QGraphicsEffect		*effect = NULL;

	effect = graphicsEffect();
	if (effect)
	{
		r = effect->isEnabled();
	}
	return r;
}

void CDiagramItem::setTextColor(const QColor &value)
{
	m_textColor = value;
	updateChildrenProperty(QString::fromUtf8("textColor"), qVariantFromValue(value) );
	update();
	emit propertyChanged(QString("textColor"), qVariantFromValue(value) );
}

void CDiagramItem::setTextAlign(int value)
{
	m_textAlign = value;
	updateChildrenProperty(QString("textAlign"), qVariantFromValue(value) );
	update();
	emit propertyChanged(QString("textAlign"), qVariantFromValue(value) );
}

void CDiagramItem::setText(const QString &text)
{
	// TO BE IMPLEMENTED FOR SPECIAL USAGE
}

QString CDiagramItem::text()
{
	// TO BE IMPLEMENTED FOR SPECIAL USAGE
	return QString("");
}

void CDiagramItem::updateChildrenProperty(const QString &name, const QVariant &value)
{
	QObject					*obj = NULL;
	QMetaObject				*meta = NULL;
	QList<QGraphicsItem*>	items;

	items = childItems();
	for (int i = 0; i < items.length(); ++i)
	{
		obj = dynamic_cast<QObject*>(items.at(i));
		if (obj)
		{
			meta = const_cast<QMetaObject*>( obj->metaObject() );
			if (meta && meta->indexOfProperty(name.toAscii().constData()) != -1 )
			{
				obj->setProperty(name.toAscii().constData(), value);
			}
		}
	}
}

void CDiagramItem::startGrowing(const QPointF &p)
{
	m_growStartPos = p;
}

bool CDiagramItem::onGrowing(const QPointF &p)
{
	bool	r = false;
	qreal	w, h;

	w = qAbs(p.x() - m_growStartPos.x());
	h = qAbs(p.y() - m_growStartPos.y());
	if (w >= 20.0)
	{
		r = true;
		if (keepSizeRatio())
		{
			w = m_sizeRatio * h;
		}
		setGeometry( QRectF(m_growStartPos + QPointF(w/2.0, h/2.0), QSizeF(w, h)) );
		setSelected(true);
	}
	return r;
}

bool CDiagramItem::stopGrowing(const QPointF &p)
{
	return onGrowing(p);
}

void CDiagramItem::changeStackOrder(int step, Qt::SortOrder order)
{
	CDiagramItemLayer	*layer = NULL;

	if (!m_p) return;

	layer = dynamic_cast<CDiagramItemLayer*>( parentItem() );
	if (layer)
		layer->changeStackOrder(this, step, order);
}

int CDiagramItem::stackOrder()
{
	return m_index;
}

void CDiagramItem::setStackOrder(int order)
{
	m_index = order;
}

void CDiagramItem::setEditor(QWidget *widget)
{
	m_editor = widget;
}

QWidget* CDiagramItem::editor()
{
	return m_editor;
}

bool CDiagramItem::isSupportedEditor(QWidget *widget)
{
	bool	r = true;

	if (!widget)
		r = false;
	return r;
}

void CDiagramItem::setContextMenu(QMenu *menu)
{
	m_contextMenu = menu;
}

QMenu* CDiagramItem::contextMenu()
{
	return m_contextMenu;
}

QPainterPath CDiagramItem::dragArea()
{
	return shape();
}

QSizeF CDiagramItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
	QSizeF		sh;

	switch(which)
	{
    case Qt::MinimumSize:
		if (layout())
		{
			sh = layout()->effectiveSizeHint(Qt::MinimumSize);
		}
		else
		{
			sh = QSizeF(0, 0);
		}
		break;
    
	case Qt::PreferredSize:
		sh = QSizeF(50, 50);
		break;
	
	case Qt::MaximumSize:
		sh = QSizeF(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
		break;
            
	default:
		qWarning("QGraphicsWidget::sizeHint(): Don't know how to handle the value of 'which'");
		break;
	}
	return sh;
}

void CDiagramItem::getContentsMargins(qreal *left, qreal *top, qreal *right, qreal *bottom) const
{
	QRectF		r;

	r = boundingRect();

	if (left)
		*left = r.left();
	if (top)
		*top = r.top();
	if (right)
		*right = -r.left();
	if (bottom)
		*bottom = -r.top();
}

QRectF CDiagramItem::contentsRect() const
{
	return QRectF(-width() / 2.0, -height() / 2.0, width(), height());
}

QRectF CDiagramItem::printArea()
{
	return mapToScene( boundingRect() ).boundingRect();
}

void CDiagramItem::setCoordinationMode(int mode)
{
	m_coordMode = mode;
	update();
}

int CDiagramItem::coordinationMode()
{
	return m_coordMode;
}

bool CDiagramItem::isAnchorable() const
{
	return true;
}

bool CDiagramItem::isAlignable() const
{
	return true;
}

void CDiagramItem::showLabel(bool value)
{
	if (m_label)
		m_label->setVisible(value);
}

QVariant CDiagramItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged && scene())
	{
		if (isSelected())
			m_p->setFocusItem(this, Qt::MouseFocusReason);
    }
    return QGraphicsItem::itemChange(change, value);
}

void CDiagramItem::defaultInit()
{
	m_label->setTextAlign( (int) Qt::AlignCenter );
}

void CDiagramItem::anchorPoints(QList<qreal> &left, QList<qreal> &right, QList<qreal> &top, QList<qreal> &bottom, qreal &cx, qreal &cy)
{
	// 
}

void CDiagramItem::toXml(QDomElement &n)
{
	QMetaObject				*meta = NULL;
  QMetaProperty			prop;
	QDomDocument			doc;
	QDomElement				ext, childNode;
	QList<QByteArray>		props;
  QStringList				filtersOut, filtersIn;
	CDiagramSerializable	*inst = NULL;

    filtersIn = filterProperties();
    filtersOut  << "width"
                << "height"
                << "x"
                << "y"
                << "z"
                << "pos"
                << "size"
                << "parent"
                << "effect"
                << "children"
                << "layout"
                << "palette";

	doc = n.ownerDocument();
	ext = doc.createElement( QString("ext") );
	childNode = doc.createElement( QString("children") );

	n.setAttribute( QString("type"), interType());
	n.setAttribute( QString("category"), category() );
	n.setAttribute( QString("name"), name() );
	n.setAttribute( QString("libCategory"), libraryCategory() );
	n.setAttribute( QString("libName"), libraryName() );
	n.setAttribute( QString("id"), QString::number(id()) );
	
	meta = const_cast<QMetaObject*>(metaObject());
	for (int i = 0; i < meta->propertyCount(); ++i)
	{
		QString		propName;
		QByteArray	b;
		QDataStream s(&b, QIODevice::WriteOnly);

		prop = meta->property(i);
		propName = QString(prop.name());
        if (filtersIn.isEmpty())
        {
            if (filtersOut.contains(propName, Qt::CaseInsensitive) || prop.userType() == 0)
            {
                continue;
            }
        }
        else
        {
            if (!filtersIn.contains(propName, Qt::CaseInsensitive))
            {
                continue;
            }
        }
		
		if (prop.isValid() && prop.isReadable())
		{
			s <<  prop.read(this);
			QDomElement	e = doc.createElement(QString("property"));
			e.setAttribute( QString("name"), QString(prop.name()) );
			e.setAttribute( QString("type"), QString(prop.typeName()) );
			e.appendChild( doc.createTextNode( QString(b.toBase64() ) ) );
			n.appendChild(e);
			// qDebug() << "save->name:" << prop.name() << " value:" << prop.read(this);
		}
	}
	
	props = dynamicPropertyNames();
	for (int i = 0; i < props.length(); ++i)
	{
		inst = RS_PROPERTY(props.at(i).constData());
		// inst = property(props.at(i).constData()).value<CDiagramSerializable*>();
		if (inst)
		{
			QDomElement	 c = doc.createElement(QString("child"));
			c.setAttribute( QString("dynamicProperty"), QString(props.at(i).constData()) );
			inst->toXml(c);
			childNode.appendChild(c);
		}
	}
	n.appendChild(childNode);

	extToXml( ext );
	n.appendChild( ext );
}

void CDiagramItem::fromXml(const QDomElement &n)
{
	QDomElement				e;
	QString					type, name, prop;
	QObject					*obj = NULL;
	QMetaProperty			pro;
	QMetaObject				*meta = NULL;
	CDiagramSerializable	*serialItem = NULL;

	setLibraryCategory(n.attribute( QString("libCategory") ) );
	setLibraryName(n.attribute( QString("libName") ) );
	setCategory(n.attribute( QString("category") ) );
	setName(n.attribute( QString("name") ) );
	setId(n.attribute( QString("id") ).toInt() );

	obj = dynamic_cast<QObject*>(this);
	e = n.firstChildElement( QString("property") );
	while (!e.isNull())
	{
		QByteArray	b;

		name = e.attribute( QString("name") );
		type = e.attribute( QString("type") );

		b = QByteArray::fromBase64( e.text().toAscii() );
		QDataStream s(&b, QIODevice::ReadOnly);
		QVariant	value(s);
		if (value.convert( QVariant::nameToType( qPrintable(type) ) ) )
		{
			meta = const_cast<QMetaObject*>( obj->metaObject() );
			pro = meta->property( meta->indexOfProperty(qPrintable(name)) );
			if (pro.isWritable())
				obj->setProperty(qPrintable(name), value);
		}
		e = e.nextSiblingElement( QString("property") );
	}
	
	e = n.firstChildElement( QString("children") );
	if (!e.isNull())
	{
		e = e.firstChildElement( QString("child") );
		while (!e.isNull())
		{
			prop = e.attribute( QString("dynamicProperty") );
			serialItem = RS_PROPERTY(prop.toAscii().constData());
			if (serialItem)
				serialItem->fromXml(e);
			e = e.nextSiblingElement(QString("child"));
		}
	}
	
	e = n.firstChildElement( QString("ext" ) );
	extFromXml(e);
}

QStringList CDiagramItem::filterProperties()
{
    QStringList     filters;

    return filters;
}
