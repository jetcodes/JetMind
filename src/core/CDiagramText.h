/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMTEXT_H
#define PRODRAW_CDIAGRAMTEXT_H

#include <QtGui>
#include <QtXml/QtXml>
#include <core/CGlobal>
#include <core/CDiagramSerializable>

class CDiagram;
class CDiagramItem;

class CORE_EXPORT CDiagramText : public QGraphicsTextItem, public QGraphicsLayoutItem, public CDiagramSerializable
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsLayoutItem)
	Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly)
	Q_PROPERTY(bool autoWidth READ autoWidth WRITE setAutoWidth)
	Q_PROPERTY(bool lineEditMode READ lineEditMode WRITE setLineEditMode)
	Q_PROPERTY(int tabStopWidth READ tabStopWidth WRITE setTabStopWidth)
	Q_PROPERTY(QBrush background READ background WRITE setBackground)
	Q_PROPERTY(QString text READ text WRITE setText)

public:
	CDiagramText(CDiagramItem *parent = NULL);
	virtual ~CDiagramText();

	enum { Type = UserType + Global::DiagramText };
	int type() const { return Type; }

	QRectF boundingRect() const;
	QPainterPath shape() const;

	// For debugging
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0);

	// Normal methods
	void setGeometry(const QRectF &r);
	void resize(qreal w, qreal h);

	void setEditing();
	bool underEditing() const;

	void setReadOnly(bool value);
	bool readOnly() const;

	void setAutoWidth(bool value);
	bool autoWidth() const;

	void setLineEditMode(bool value);
	bool lineEditMode() const;

	void setBackground( const QBrush &brush = QBrush(Qt::white) );
	QBrush background() const;

	void lockFocus(bool locked);
	bool focusLocked() const;

	void setText(const QString &value);
	QString text() const;

	//////////////////////////////////////////////////////////////////////
	// advance editing
	void setTabStopWidth(int width);
	int tabStopWidth();

	void setListStyle(const QTextListFormat::Style &style);
	QTextListFormat::Style listStyle();

	void setTextColor(const QColor &color);
	QColor textColor();

	void setFont(const QFont &font);
	QFont font() const;

	void setFontFamily(const QString &value);
	QString fontFamily() const;

	void setFontSize(int size);
	int fontSize() const;

	void setBold(bool value);
	bool bold() const;

	void setUnderline(bool value);
	bool underline() const;

	void setItalic(bool value);
	bool italic() const;

	void setHyperlink(bool anchor, const QString &value);
	bool isHyperlink() const;
	QString hyperlink() const;

	void setTextAlign(int flag);
	int textAlign();

	void setStyleable(bool value);
	bool styleable() const;

	bool hasModification() const;

	// Import and export
	void toXml(QDomElement&);
	void fromXml(const QDomElement &);

Q_SIGNALS:
	void resizeRequest(QGraphicsLayoutItem*);
	void sizeAutoChanged();
	void textChanged(CDiagramText*);
	void editRequest(CDiagramText*);
	void cursorPositionChanged(const QTextCursor &cursor);

protected Q_SLOTS:
	void onContentsChange(int, int, int);

protected:
	void updateGeometry();
	void interAlignment(int value);

protected:
	void focusInEvent(QFocusEvent *event);
	void focusOutEvent(QFocusEvent *event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;

	QTextCursor			m_cursor;
	QString				m_tmpText;
	QRectF				m_tmpRect;
	QBrush				m_background;
	bool				m_readOnly;
	bool				m_lockFocus;
	bool				m_autoWidth;
	bool				m_lineEditMode;
	QSizeF				m_idealSize;
	QSizeF				m_actualSize;
	bool				m_styleable;
	bool				m_modification;

private:
	Q_DISABLE_COPY(CDiagramText)

}; // End of class CDiagramText

#endif // PRODRAW_CINPUTBOX_H

