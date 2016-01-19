/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPSETTINGPANEL_H
#define PRODRAW_CMINDMAPSETTINGPANEL_H

#include <QtGui>
#include <widgets/CUiGlobal>
#include <widgets/CUiAbstractDialog>

namespace Ui {
    class CCanvasSettingPanel;
}

class CDiagram;
class CUiColorGrid;
class CCanvasSettingPanel;

class CCanvasSettingPanel : public CUiAbstractDialog
{
    Q_OBJECT
    
public:
    explicit CCanvasSettingPanel(QWidget *parent = 0);
    virtual ~CCanvasSettingPanel();
    
    void update(CDiagram *d);

    void addStyle(const QBrush &value);
    QBrush getStyle(int index) const;

    void setStyles( QList<QBrush> &brushes);
    QList<QBrush> styles() const;

Q_SIGNALS:
    void gridsVisibleChanged(bool);
    void styleSelected(QBrush);

public Q_SLOTS:
    void onSetDefaultStyle(bool value);
    void onStyleClicked(QModelIndex index);
    void onStyleDoubleClicked(QModelIndex index);
    void onSettingChanged(const QString &key);
	void onBackgroundColorSelected(QColor c);
    void onShowColorGrid();
    void onBackgroundColorDone();

protected:
    void hideEvent(QHideEvent *evt);
    void changeEvent(QEvent *event);

private:
    void setupStyles();

    Ui::CCanvasSettingPanel *ui;

    CDiagram		*m_d;
    QList<QBrush>   m_brushes;
	CUiColorGrid    *m_colorGrid;

}; // End of class CCanvasSettingPanel

#endif // PRODRAW_CMINDMAPSETTINGPANEL_H
