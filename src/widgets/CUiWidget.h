/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CUIWIDGET_H
#define PRODRAW_CUIWIDGET_H

#include <QtGui>
#include <widgets/CUiGlobal>

class CUiBorder;
class CUiWidget;

class CUiBorder : public QWidget
{
	Q_OBJECT

public:
	CUiBorder(CUiWidget *parent, int index, const Qt::CursorShape &cursor, QString &image, QString &repeat);
	virtual ~CUiBorder();

	virtual void paintEvent(QPaintEvent *event);
	
	void setIndex(int value) { m_index = value; }
	int index() { return m_index; }

	bool isDragging() { return m_dragging; }

protected:
	virtual void mouseMoveEvent( QMouseEvent *event );
	virtual void mousePressEvent( QMouseEvent *event );
	virtual void mouseReleaseEvent( QMouseEvent *event );

private:
	int			m_index;
	bool		m_dragging;
	CUiWidget	*m_parent;
};

class WIDGETS_EXPORT CUiWidget : public QFrame
{
	Q_OBJECT

public:
    CUiWidget(QWidget *parent = 0);
	virtual ~CUiWidget();

	// Borders
	enum Borders {
		Top = 0,
		LeftTop,
		Left,
		LeftBottom,
		Bottom,
		RightBottom,
		Right,
		RightTop
	};

	void setBorder(QWidget *widget, int pos);
	QWidget *border(int pos);

	void setTitleBar(QWidget *widget);
	QWidget *titleBar() const;

	Qt::WindowState widgetState();

	void paintEvent(QPaintEvent *event);

Q_SIGNALS:
	void geometryChanged(const QRect &);
	void fullscreen(bool);

public Q_SLOTS:
	void onMaximize();
	void onMinimize();
	void onRestore();
	void onMove(const QPoint &p);
	void onClose();
	void onFullScreen();

protected:
    virtual void initBorders();

	void toggleFullScreen(bool value);
	void doResize(CUiBorder *border, const QPoint &p);

    virtual bool beforeEnterFullScreen();
    virtual bool afterExitFullScreen();

	int						m_border;
	QList<QWidget*>			m_borders;
    QWidget					*m_titleBar;
    QWidget					*m_centralWidget;
	QRect					m_oldGeometry;
	Qt::WindowState			m_prevState;
	Qt::WindowState			m_currState;

private:
	QVBoxLayout				*m_mainLayout;
	QVBoxLayout				*m_navLayout;
	QHBoxLayout				*m_topLayout;
	QHBoxLayout				*m_centerLayout;
	QHBoxLayout				*m_bottomLayout;

	friend class CUiBorder;
};

#endif // PRODRAW_CUIWIDGET_H
