/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CDIAGRAMITEM_H
#define PRODRAW_CDIAGRAMITEM_H

#include <QtGui>
#include <QtXml/QtXml>
#include <core/CGlobal>
#include <core/CDiagramSerializable>

class QDomNode;
class CDiagram;
class CDiagramText;

class CORE_EXPORT CDiagramItem : public QGraphicsWidget, public CDiagramSerializable
{
	Q_OBJECT

	Q_PROPERTY(bool resizable READ resizable WRITE setResizable)
	Q_PROPERTY(bool rotatable READ rotatable WRITE setRotatable)
	Q_PROPERTY(bool keepSizeRatio READ keepSizeRatio WRITE setKeepSizeRatio)
	Q_PROPERTY(bool lockPosition READ lockPosition WRITE setLockPosition)
	Q_PROPERTY(QPen pen READ pen WRITE setPen)
	Q_PROPERTY(QPen stroke READ stroke WRITE setStroke)
	Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
	Q_PROPERTY(QFont font READ font WRITE setFont)
	Q_PROPERTY(bool effectEnabled READ effectEnabled WRITE setEffectEnabled)
	Q_PROPERTY(QString text READ text WRITE setText)
	Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)
	Q_PROPERTY(int textAlign READ textAlign WRITE setTextAlign)
	Q_PROPERTY(int stackOrder READ stackOrder WRITE setStackOrder)
	Q_PROPERTY(int coordinationMode READ coordinationMode WRITE setCoordinationMode)

public:
	CDiagramItem(CDiagram *parent = NULL);
	virtual ~CDiagramItem();

	enum { Type = UserType + Global::DiagramItem };
	int type() const { return Type; }
	virtual int interType() const { return Global::Undefined; }

	enum CoordinationMode {
		TopLeftAsOrigin,
		CenterAsOrigin
	}; // CoordinationMode

	void setId(int id);
	int id() const;

	void setCategory(const QString &value);
	QString category();

	void setName(const QString &value);
	QString name();

	void setLibraryCategory(const QString &value);
	QString libraryCategory();

	void setLibraryName(const QString &value);
	QString libraryName();

	qreal width() const;
	qreal height() const;

	void setPos(const QPointF &p);
	void setPos(qreal x, qreal y);

	void setResizable(bool value);
	bool resizable();

	void setRotatable(bool value);
	bool rotatable();

	qreal sizeRatio();
	void setKeepSizeRatio(bool value);
	bool keepSizeRatio() { return m_keepSizeRatio; }

	void setLockPosition(bool value);
	bool lockPosition();

	void setPen( const QPen &value);
	QPen& pen() { return m_pen; }

	void setBrush( const QBrush &value);
	QBrush& brush() { return m_brush; }

	void setStroke( const QPen &value);
	QPen& stroke() { return m_stroke; }

	void setFont( const QFont &value);
	QFont& font() { return m_font; }

	void setEffectEnabled(bool value);
	bool effectEnabled();

	void setTextColor(const QColor &c);
	QColor textColor() { return m_textColor; }

	void setTextAlign(int align);
	int textAlign() { return m_textAlign; }

	virtual void setText(const QString &text);
	virtual QString text();

	void changeStackOrder(int step, Qt::SortOrder = Qt::AscendingOrder);
	void setStackOrder(int order);
	int stackOrder();

	virtual QMenu *contextMenu();
	void setContextMenu(QMenu *menu);

	virtual QWidget *editor();
	virtual bool isSupportedEditor(QWidget *widget);
	void setEditor(QWidget *widget);

	virtual void anchorPoints(QList<qreal> &left, QList<qreal> &right, QList<qreal> &top, QList<qreal> &bottom, qreal &cx, qreal &cy);

	// The three member methods are useful when adding a new item
	// to the diagram
	virtual void startGrowing(const QPointF &p);
	virtual bool onGrowing(const QPointF &p);
	virtual bool stopGrowing(const QPointF &p);

	virtual QPainterPath dragArea();

	virtual void defaultInit();
	virtual void init(const QVariant &value);
	virtual void getContentsMargins(qreal *left, qreal *top, qreal *right, qreal *bottom) const;
	virtual QRectF contentsRect() const;
	virtual QRectF printArea();

	void setCoordinationMode(int mode);
	int coordinationMode();

	virtual bool isAlignable() const;
	virtual bool isAnchorable() const;

	virtual void fromXml(const QDomElement &n);
	virtual void toXml(QDomElement &n);

Q_SIGNALS:
	void addedToDiagram();
	void propertyChanged(const QString&, const QVariant&);

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;
	void showLabel(bool value);
	void updateChildrenProperty(const QString &name, const QVariant &value);

	virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

	// To be overrided to implement custom serialization
	// actions.
	virtual void extFromXml(const QDomElement &n) {}
	virtual void extToXml(QDomElement &n) {}

    virtual QStringList filterProperties();

	int					m_id;
	int					m_index;
	QString				m_category;
	QString				m_name;
	QString				m_libCategory;
	QString				m_libName;

	qreal				m_sizeRatio; // m_sizeRatio = w / h;
	bool				m_resizable;
	bool				m_rotatable;
	bool				m_keepSizeRatio;

	QPen				m_pen;
	QPen				m_stroke;
	QBrush				m_brush;
	QFont				m_font;
	QColor				m_textColor;
	
	QPointF				m_growStartPos;
	int					m_textAlign;

	int					m_coordMode;

	CDiagramText		*m_label;

	QMenu				*m_contextMenu;
	QWidget				*m_editor;

	CDiagram			*m_p;
	
	friend class CDiagram;
	friend class CDiagramItemLayer;

}; // End of class CDiagramItem

#endif // PRODRAW_CDIAGRAMITEM_H
