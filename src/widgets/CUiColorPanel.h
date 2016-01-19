/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CQCOLORPANEL_H
#define PRODRAW_CQCOLORPANEL_H

#include <QtGui>
#include <widgets/CUiGlobal>

class CUiColorGrid;
class CUiGradientEditor;

namespace Ui {
class CUiColorPanel;
}

class WIDGETS_EXPORT CUiColorPanel : public QFrame
{
	Q_OBJECT

public:
	CUiColorPanel(QWidget *parent = 0);
	virtual ~CUiColorPanel();

	enum Mode {
		SolidFill,
		GradientFill,
		SolidOnly
	};

	void setMode(int mode);
	int mode();

	void setSelectedColor(const QColor &c);
	QColor selectedColor();

	void setGradient(const QGradient &g);
	QGradient gradient();

	void setTransparency(int value);
	qreal transparency();

	void paintEvent(QPaintEvent *event);

Q_SIGNALS:
	void colorHovered(QColor c);
	void colorSelected(QColor c);
	void gradientSelected(QGradient g);

public Q_SLOTS:
	void onClose();
	void onMoreColor();
	void onColorHovered(QColor c);
	void onColorSelected(QColor c);
	void onColorSelectionDone();
	void onColorValueChanged();
	void onColorValueEditing(const QString & text);
	void onNoColor();
	void onFillTypeChanged(int index);
	void onTransparencyChanged(int value);
	void onTransparencyChanged(const QString &value);

protected:
	void closeEvent( QCloseEvent * event );

private:
	Ui::CUiColorPanel *ui;

    int                     m_mode;
    bool                    m_moreColor;
    QColor                  m_color;
    qreal                   m_transparency;
    QGradient               m_gradient;
	CUiColorGrid			*m_grid;
	CUiGradientEditor		*m_gradientEditor;
};

#endif // PRODRAW_CQCOLORPANEL_H
