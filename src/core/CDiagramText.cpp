/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include <core/CDiagramText>

CDiagramText::CDiagramText(CDiagramItem *parent)
	: QGraphicsTextItem(parent),
	  m_readOnly(false),
	  m_lockFocus(false),
	  m_autoWidth(true),
	  m_lineEditMode(false),
	  m_styleable(true),
	  m_modification(false),
	  m_background( Qt::transparent )
{
	setFlag(ItemSendsGeometryChanges, true);
	setFlag(QGraphicsItem::ItemIsFocusable, false);
	setFlag(QGraphicsItem::ItemIsSelectable, false);
	setTextInteractionFlags(Qt::NoTextInteraction);
	
    setFont( QFont("Arial", 18) );
	//setTextAlign( (int) Qt::AlignCenter );
	
	m_cursor = QTextCursor(document());
	setTabStopWidth(4);
	document()->setIndentWidth(20);

	connect(document(), SIGNAL(contentsChange(int, int, int)), this, SLOT(onContentsChange(int, int, int)));
	connect(document(), SIGNAL(cursorPositionChanged(const QTextCursor &)), this, SIGNAL(cursorPositionChanged(const QTextCursor &)));
}

CDiagramText::~CDiagramText()
{
	
}

QRectF CDiagramText::boundingRect() const
{
	return QGraphicsTextItem::boundingRect();
}

QPainterPath CDiagramText::shape() const
{
	return QGraphicsTextItem::shape();
}

void CDiagramText::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    /*
	QPen						pen1, pen2;
	QRectF						r;
	QPixmap						pixmap;
	QPainter					pixmapPainter;
	QStyleOptionGraphicsItem	opt;

	r = QGraphicsTextItem::boundingRect();
	opt.exposedRect = r;

	pixmap = QPixmap( r.size().toSize() );
	pixmap.fill( Qt::transparent );
	pixmapPainter.begin( &pixmap );
	pixmapPainter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing );

	if (hasFocus() || textInteractionFlags() & Qt::TextEditorInteraction )
	{
		pen1 = QPen(QColor("#5ABFFD"), 2, Qt::SolidLine);
		pen1.setJoinStyle( Qt::MiterJoin );
		
		pen2 =  QPen(QColor("#DDDDDD"), 1, Qt::SolidLine);
		pen2.setJoinStyle( Qt::MiterJoin );

		pixmapPainter.save();
		pixmapPainter.setPen( pen1 );
		pixmapPainter.setBrush( Qt::NoBrush );
		pixmapPainter.drawRect( r );
		pixmapPainter.setPen( pen2 );
		pixmapPainter.setBrush( Qt::white );
		pixmapPainter.drawRect( r.adjusted(1.5, 1.5, -1.5, -1.5) );
		pixmapPainter.restore();
	}

	if (toPlainText().trimmed() != "")
	{
		pixmapPainter.save();
		pixmapPainter.setPen(Qt::NoPen);
		pixmapPainter.setBrush(m_background);
		pixmapPainter.drawRect( r );
		pixmapPainter.restore();
	}

	QGraphicsTextItem::paint(&pixmapPainter, &opt, widget);
	pixmapPainter.end();

	painter->save();
	painter->setRenderHint(QPainter::SmoothPixmapTransform);
	painter->drawPixmap(r, pixmap, QRectF());
	painter->restore();
    */
    QPen						pen1, pen2;
    QRectF						r;
    QStyleOptionGraphicsItem	opt;

    r = QGraphicsTextItem::boundingRect();
    opt.exposedRect = r;

    if (hasFocus() || textInteractionFlags() & Qt::TextEditorInteraction )
    {
        pen1 = QPen(QColor("#5ABFFD"), 2, Qt::SolidLine);
        pen1.setJoinStyle( Qt::MiterJoin );

        pen2 =  QPen(QColor("#DDDDDD"), 1, Qt::SolidLine);
        pen2.setJoinStyle( Qt::MiterJoin );

        painter->save();
        painter->setPen( pen1 );
        painter->setBrush( Qt::NoBrush );
        painter->drawRect( r );
        painter->setPen( pen2 );
        painter->setBrush( Qt::white );
        painter->drawRect( r.adjusted(1.5, 1.5, -1.5, -1.5) );
        painter->restore();
    }

    if (toPlainText().trimmed() != "")
    {
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(m_background);
        painter->drawRect( r );
        painter->restore();
    }

    painter->save();

    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing );
    QGraphicsTextItem::paint(painter, &opt, widget);

    painter->restore();
}

QSizeF CDiagramText::sizeHint(Qt::SizeHint which, const QSizeF &contraint) const
{
	QSizeF		r;

	switch(which)
	{
	case Qt::MinimumSize:
		if (autoWidth())
			r = m_idealSize;
		else
			// The value should NOT be 0
			r = QSizeF(1, 1);
		break;

	case Qt::MaximumSize:
		r = QSizeF(QWIDGETSIZE_MAX, m_actualSize.height());
		break;

	default:
		if (autoWidth())
			r = m_idealSize;
		else
			r = m_actualSize;
		break;
	};
	return r;
}

void CDiagramText::focusInEvent(QFocusEvent *event)
{
	CDiagram		*d;
	CDiagramItem	*item = NULL;

	QGraphicsTextItem::focusInEvent(event);
	d = (CDiagram*)scene();
	if (d)
	{
		item = dynamic_cast<CDiagramItem*>( parentItem() );
		m_tmpText = toHtml();

		// The old rect should be the contents rect
		// but not the bounding rect.
		if (item)
			m_tmpRect = item->contentsRect();
	}
}

void CDiagramText::focusOutEvent(QFocusEvent *event)
{
	CDiagram	*d = NULL;
	
	d = (CDiagram*)scene();
	if (d && d->hasFocus())
	{
		if (!focusLocked())
		{
			QGraphicsTextItem::focusOutEvent(event);
			setFlag(QGraphicsItem::ItemIsFocusable, false);
			setTextInteractionFlags(Qt::NoTextInteraction);

			m_cursor.clearSelection();
			setTextCursor(m_cursor);

			d->finishEditingText(this, m_tmpText, m_tmpRect);
			m_tmpText = QString("");
			setCursor(Qt::ArrowCursor);
		}
	}
}

void CDiagramText::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (!underEditing() && (event->buttons() & Qt::LeftButton) )
	{
		setEditing();
	}
	else
	{
		QGraphicsTextItem::mouseDoubleClickEvent(event);
	}

	// Pay attention to the process of receiving event of doubleclick
	// 1. a press event
	// 2. a release event
	// 3. a doubleclick event
	// 4. a release event
	// But for qgraphicsitem, the doubleclick event handler calls mousePressEvent
	// to handle the event.
}

void CDiagramText::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	int					pos;
	QTextCursor			cursor;
	QTextBlock			block;
	QTextDocument		*doc = NULL;
	QAbstractTextDocumentLayout	*layout = NULL;

	QGraphicsTextItem::mousePressEvent(event);
	
	if (underEditing())
	{
		doc = document();
		layout = doc->documentLayout();
		pos = layout->hitTest( event->pos(), Qt::FuzzyHit);
		block = doc->findBlock(pos);
		
		cursor = textCursor();
		cursor.setPosition(pos);
		emit cursorPositionChanged(cursor); 
	}
}

void CDiagramText::keyPressEvent(QKeyEvent *event)
{
	if (underEditing())
	{
		if (lineEditMode() && (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) )
		{
			lockFocus(false);
			clearFocus();
			return;
		}
	}
	QGraphicsTextItem::keyPressEvent(event);
}

void CDiagramText::onContentsChange(int position, int charsRemoved, int charsAdded)
{
	updateGeometry();

	m_modification = true;

	emit textChanged(this);
	emit resizeRequest(this);
	emit sizeAutoChanged();
}

void CDiagramText::resize(qreal w, qreal h)
{
	setGeometry(QRectF(pos(), QSizeF(w, h) ) );
}

void CDiagramText::setGeometry( const QRectF &rect)
{
	if (!autoWidth()) // non-auto expand
	{
		qreal		w, h;

		prepareGeometryChange();
		w = rect.width();
		setTextWidth(w);
		h = QGraphicsTextItem::boundingRect().height();
		
		// NOTE: We need to set the position of QGraphicsTextItem and QGraphicsLayoutItem
		// individually, because the data is stored in different place.
		setPos(rect.topLeft());
		QGraphicsLayoutItem::setGeometry( QRectF(pos(), QSizeF(w, h)) );
	}
	else
	{
		// NOTE: We need to set the position of QGraphicsTextItem and QGraphicsLayoutItem
		// individually, because the data is stored in different place.
		setPos(rect.topLeft());
		QGraphicsLayoutItem::setGeometry( QRectF(pos(), boundingRect().size()) );
	}
	updateGeometry();
}

void CDiagramText::updateGeometry()
{	
	if (!autoWidth()) // non-auto expand
	{
		qreal		w;

		// update idealSize;
		w = textWidth();
		setTextWidth(-1);
		m_idealSize = boundingRect().size();
		setTextWidth(w);
		m_actualSize = QGraphicsLayoutItem::geometry().size();
	}
	else
	{
		// Tricky here, first set textwidth to -1, then get the actual width
		// of the text in that mode.
		setTextWidth(-1);
		m_idealSize = boundingRect().size();
		setTextWidth(m_idealSize.width());
		QGraphicsLayoutItem::setGeometry( QRectF(pos(), m_idealSize ) );
		m_actualSize = m_idealSize;
	}

	// IMPORTANT: updateGeometry of QGraphicsLayoutItem must be called to update the
	// cached sizehint values, so the layout engine (if any) can notice the changes.
	QGraphicsLayoutItem::updateGeometry();
}

void CDiagramText::setBackground(const QBrush &brush)
{
	m_background = brush;
	update();
}

QBrush CDiagramText::background() const
{
	return m_background;
}

void CDiagramText::setReadOnly(bool value)
{
	m_readOnly = value;
	update();
}

bool CDiagramText::readOnly() const
{
	return m_readOnly;
}

void CDiagramText::setEditing()
{
	CDiagram	*d = NULL;

	if (readOnly()) return;

	d = (CDiagram*)scene();
	if (!d || !d->requestEditingText(this))
		return;

	m_modification = false;
	setFlag(QGraphicsItem::ItemIsFocusable, true);
	setTextInteractionFlags(Qt::TextEditorInteraction);
	QGraphicsTextItem::setFocus( Qt::MouseFocusReason );

	m_cursor = textCursor();
	if (!m_cursor.hasSelection())
	{
		m_cursor.setPosition(QTextCursor::Start);
		m_cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
		m_cursor.select(QTextCursor::Document);
		setTextCursor(m_cursor);
	}
	else
	{
		m_cursor.clearSelection();
		setTextCursor(m_cursor);
	}
	setCursor(Qt::IBeamCursor);
}

bool CDiagramText::underEditing() const
{
	return (flags() & QGraphicsItem::ItemIsFocusable) && (textInteractionFlags() & Qt::TextEditorInteraction);
}

void CDiagramText::lockFocus(bool locked)
{
	m_lockFocus = locked;
}

bool CDiagramText::focusLocked() const
{
	return m_lockFocus;
}

void CDiagramText::setAutoWidth(bool value)
{
	m_autoWidth = value;
	updateGeometry();
}

bool CDiagramText::autoWidth() const
{
	return m_autoWidth;
}

void CDiagramText::setText(const QString &value)
{
	setHtml(value);
}

QString CDiagramText::text() const
{
	QString		charset("utf-8");

	return document()->toHtml( charset.toAscii() );
}

void CDiagramText::setLineEditMode(bool value)
{
	m_lineEditMode = value;
}

bool CDiagramText::lineEditMode() const
{
	return m_lineEditMode;
}

void CDiagramText::interAlignment(int value)
{
	QTextOption			opt;

	opt = document()->defaultTextOption();	
	opt.setAlignment( (Qt::Alignment) value );
	document()->setDefaultTextOption( opt );
}

void CDiagramText::setTabStopWidth(int width)
{
	QTextOption	opt;

	opt = document()->defaultTextOption();
	if (opt.tabStop() == width || width < 0)
		return;
	opt.setTabStop(width);
	document()->setDefaultTextOption(opt);
}

int CDiagramText::tabStopWidth()
{
	return qRound(document()->defaultTextOption().tabStop());
}

void CDiagramText::setListStyle(const QTextListFormat::Style &style)
{
	QTextBlockFormat	blockFormat;
	QTextListFormat		listFormat;

	m_cursor = textCursor();
	m_cursor.beginEditBlock();
	blockFormat = m_cursor.blockFormat();
	if (m_cursor.currentList())
	{
		listFormat = m_cursor.currentList()->format();
	}
	else
	{
		listFormat.setIndent(blockFormat.indent() + 1);
		blockFormat.setIndent(0);
		m_cursor.setBlockFormat(blockFormat);
	}

	listFormat.setStyle(style);
	m_cursor.createList(listFormat);

	m_cursor.endEditBlock();
	setTextCursor(m_cursor);
}

QTextListFormat::Style CDiagramText::listStyle()
{
	QTextListFormat		listFormat;

	m_cursor = textCursor();
	listFormat = m_cursor.currentList()->format();
	return listFormat.style();
}

void CDiagramText::setTextColor(const QColor &color)
{
	if (!underEditing())
	{
		setDefaultTextColor(color);
	}
	else
	{
		QTextCharFormat		format;

		if (!color.isValid()) return;

		m_cursor = textCursor();
		format.setForeground(color);
		if (!m_cursor.hasSelection())
			m_cursor.select(QTextCursor::LineUnderCursor);
		m_cursor.mergeCharFormat(format); 
		setTextCursor(m_cursor);
	}
}

QColor CDiagramText::textColor()
{
	QColor	c;

	if (!underEditing())
		c = defaultTextColor();
	else
		c = QColor();
	return c;
}

void CDiagramText::setFont(const QFont &font)
{
	if (!underEditing())
	{
		document()->setDefaultFont(font);	
	}
	else
	{
		 QTextCharFormat	format;

		 format.setFont(font);
		 m_cursor = textCursor();
		 if (!m_cursor.hasSelection())
			 m_cursor.select(QTextCursor::WordUnderCursor);
		 m_cursor.mergeCharFormat(format);
		 setTextCursor(m_cursor);
	}
	updateGeometry();
	emit sizeAutoChanged();
}

QFont CDiagramText::font() const
{
	QFont	f;

	if (!underEditing())
		f = document()->defaultFont();
	else
		f = m_cursor.charFormat().font();
	return f;
}

void CDiagramText::setFontFamily(const QString &value)
{
	if (!underEditing())
	{
		QFont	font;

		font = document()->defaultFont();
		font.setFamily(value);
		document()->setDefaultFont(font);
	}
	else
	{
		QTextCharFormat	format;

		format.setFontFamily(value);
		m_cursor = textCursor();
		if (!m_cursor.hasSelection())
			 m_cursor.select(QTextCursor::WordUnderCursor);
		m_cursor.mergeCharFormat(format);
		setTextCursor(m_cursor);
	}
	updateGeometry();
	emit sizeAutoChanged();
}

QString CDiagramText::fontFamily() const
{
	QString s;

	if (!underEditing())
		s = document()->defaultFont().family();
	else
		s = m_cursor.charFormat().fontFamily();
	return s;
}

void CDiagramText::setFontSize(int size)
{
	if (!underEditing())
	{
		QFont	font;

		font = document()->defaultFont();
		font.setPointSize(size);
		document()->setDefaultFont(font);
	}
	else
	{
		QTextCharFormat	format;

		format.setFontPointSize(size);
		m_cursor = textCursor();
		if (!m_cursor.hasSelection())
			 m_cursor.select(QTextCursor::WordUnderCursor);
		m_cursor.mergeCharFormat(format);
		setTextCursor(m_cursor);
	}
	updateGeometry();
	emit sizeAutoChanged();
}

int CDiagramText::fontSize() const
{
	int		r;

	if (!underEditing())
		r = document()->defaultFont().pointSize();
	else
		r = qRound(m_cursor.charFormat().fontPointSize());
	return r;
}

void CDiagramText::setBold(bool value)
{
	if (!underEditing())
	{
		QFont	font;

		font = document()->defaultFont();
		font.setBold(value);
		document()->setDefaultFont(font);
	}
	else
	{
		QTextCharFormat format;

		format.setFontWeight(value ? QFont::Bold : QFont::Normal);
		m_cursor = textCursor();
		if (!m_cursor.hasSelection())
			 m_cursor.select(QTextCursor::WordUnderCursor);
		m_cursor.mergeCharFormat(format);
		setTextCursor(m_cursor);
	}
	updateGeometry();
	emit sizeAutoChanged();
}

bool CDiagramText::bold() const
{
	bool	r;
	if (!underEditing())
		r = document()->defaultFont().bold();
	else
		r = m_cursor.charFormat().font().bold();
	return r;
}

void CDiagramText::setUnderline(bool value)
{
	if (!underEditing())
	{
		QFont	font;

		font = document()->defaultFont();
		font.setUnderline(value);
		document()->setDefaultFont(font);
	}
	else
	{
		QTextCharFormat format;

		format.setFontUnderline(value);
		m_cursor = textCursor();
		if (!m_cursor.hasSelection())
			 m_cursor.select(QTextCursor::WordUnderCursor);
		m_cursor.mergeCharFormat(format);
		setTextCursor(m_cursor);
	}
}

bool CDiagramText::underline() const
{
	bool	r;

	if (!underEditing())
		r = document()->defaultFont().underline();
	else
		r = textCursor().charFormat().fontUnderline(); 
	return r; 
}

void CDiagramText::setItalic(bool value)
{
	if (!underEditing())
	{
		QFont	font;

		font = document()->defaultFont();
		font.setItalic(value);
		document()->setDefaultFont(font);
	}
	else
	{
		QTextCharFormat format;

		format.setFontItalic(value);
		m_cursor = textCursor();
		if (!m_cursor.hasSelection())
			 m_cursor.select(QTextCursor::WordUnderCursor);
		m_cursor.mergeCharFormat(format);
		setTextCursor(m_cursor);
	}
}

bool CDiagramText::italic() const
{
	bool	r;

	if (!underEditing())
		r = document()->defaultFont().italic();
	else
		r = textCursor().charFormat().fontItalic();
	return r; 
}

void CDiagramText::setHyperlink(bool anchor, const QString &value)
{
	QTextCharFormat format;

    format.setAnchor(anchor);
	format.setAnchorHref(value);
	m_cursor = textCursor();
    if (m_cursor.hasSelection())
	{
		m_cursor.mergeCharFormat(format);
		setTextCursor(m_cursor);
	}
}

bool CDiagramText::isHyperlink() const
{
	return textCursor().charFormat().isAnchor();
}

QString CDiagramText::hyperlink() const
{
	return textCursor().charFormat().anchorHref();
}

void CDiagramText::setTextAlign(int flag)
{
	if (!underEditing())
	{
		QTextBlockFormat format;
		
		format.setAlignment( (Qt::Alignment)(flag) );
		m_cursor = textCursor();
		m_cursor.select( QTextCursor::Document );
		m_cursor.mergeBlockFormat(format);
		setTextCursor(m_cursor);

		m_cursor = textCursor();
		m_cursor.clearSelection();
		setTextCursor(m_cursor);
	}
	else
	{
		QTextBlockFormat format;

		format.setAlignment( (Qt::Alignment)(flag) );
		m_cursor = textCursor();
		m_cursor.mergeBlockFormat(format);
		setTextCursor(m_cursor);
	}
}

int CDiagramText::textAlign()
{
	int		r;

	if (!underEditing())
		r = Qt::AlignLeft;
	else
		r = (int)(textCursor().blockFormat().alignment());
	return r;
}

void CDiagramText::setStyleable(bool value)
{
	m_styleable = value;
}

bool CDiagramText::styleable() const
{
	return m_styleable;
}

bool CDiagramText::hasModification() const
{
	return m_modification;
}

void CDiagramText::toXml(QDomElement &n)
{
	QMetaObject		*meta = NULL;
	QMetaProperty	prop;
	QVariant		value;
	QDomDocument	doc;

	doc = n.ownerDocument();

	n.setAttribute( QString("category"), "Core" );
	n.setAttribute( QString("name"), "Text" );
	
	meta = const_cast<QMetaObject*>(metaObject());
	for (int i = 0; i < meta->propertyCount(); ++i)
	{
		QString		verify;
		QByteArray	b;
		QDataStream s(&b, QIODevice::WriteOnly);

		prop = meta->property(i);
		verify = QString(prop.typeName());
		if (verify.contains(QChar('*')) || verify.contains(QChar('<'))) 
		{
			continue;
		}

		if (prop.isValid() && prop.isReadable())
		{
			s <<  prop.read(this);
			QDomElement	e = doc.createElement(QString("property"));
			e.setAttribute( QString("name"), QString(prop.name()) );
			e.setAttribute( QString("type"), QString(prop.typeName()) );
			e.appendChild( doc.createTextNode( QString(b.toBase64() ) ) );
			n.appendChild(e);
		}
	}
}

void CDiagramText::fromXml(const QDomElement &n)
{
	QObject			*obj = NULL;
	QString			name, type;
	QDomElement		e;

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
			obj->setProperty(qPrintable(name), value);
		}
		e = e.nextSiblingElement( QString("property") );
	}
}
