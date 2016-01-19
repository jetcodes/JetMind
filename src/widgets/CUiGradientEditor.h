/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CUIGRADIENTEDITOR_H
#define PRODRAW_CUIGRADIENTEDITOR_H

#include <QtGui>
#include <widgets/CUiGlobal>

class StopDragger;
class StopArea;
class StopStat;
class GradientStat;

class StopDragger: public QWidget
{
public:
	StopDragger(StopArea *parent);
	virtual ~StopDragger() {}

	void setColor(const QColor &color);
	QColor color();

	void setSelected(bool value);
	bool selected();

	qreal stopPosition();

protected:
	void paintEvent( QPaintEvent * event );
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    // void contextMenuEvent(QContextMenuEvent *event);

private:
	bool		m_selected;
	bool		m_dragged;
	QColor		m_color;
	StopArea	*m_stopArea;

}; // End of class StopDragger

class StopArea: public QWidget
{
	Q_OBJECT

public:
	StopArea(QWidget *parent = NULL);
	virtual ~StopArea();

	QGradientStops stops();
	void setStops(const QGradientStops &stops);

	int stopCount();
	void setStopColor(const QColor &color);
	void setStopColor(int index, const QColor &color);
	QColor stopColor();
	QColor stopColor(int index);

	void reset();

Q_SIGNALS:
	void stopChanged();
	
protected:
	void mousePressEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);

	StopDragger* selectedStop();
	void initDefault();
	void clearSelection();
	void removeStop(StopDragger *dragger);

private:
	QList<StopDragger*>		m_stops;
	
	friend class StopDragger;
}; // End of StopArea

class StopStat: public QWidget
{
public:
	StopStat(QWidget *parent = NULL);

	void setStops( const QGradientStops &stops);

protected:
	void paintEvent(QPaintEvent *event);
	
	QLinearGradient		m_gradient;

}; // End of class StopStat

class GradientStat: public QWidget
{
public:
	GradientStat(QWidget *parent = NULL);

	void setStops( const QGradientStops &stops);
	void setAngle( qreal angle);
	QGradient gradient();

protected:
	void paintEvent(QPaintEvent *event);
	
	QLinearGradient		m_gradient;
}; // End of class GradientStat

class WIDGETS_EXPORT CUiGradientEditor : public QWidget
{
	Q_OBJECT

public:
	CUiGradientEditor(QWidget *parent = NULL);
	virtual ~CUiGradientEditor();

	void setStopColor(const QColor &color);
	QColor stopColor();

	void setGradient(const QGradient &value);
	QGradient gradient();

	void setTransparency(qreal value);
	qreal transparency();

public Q_SLOTS:
	void onStopChanged();
	void onAngleChanged(const QString &value);

protected:

	StopArea		*m_stopArea;
	StopStat		*m_stopStat;
	GradientStat	*m_gradientStat;
	QLabel			*m_angleMark;
	QComboBox		*m_angleEdit;
	
}; // End of class CUiGradientEditor

#endif // PRODRAW_CUIGRADIENTEDITOR_H
