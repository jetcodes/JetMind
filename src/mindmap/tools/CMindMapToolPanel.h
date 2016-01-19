/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CMINDMAPTOOLPANEL_H
#define PRODRAW_CMINDMAPTOOLPANEL_H

#include <QtGui>
#include <core/CDiagramWidget>
#include "ui_CMindMapToolPanel.h"

namespace Ui {
class CMindMapToolPanel;
}

class CDiagram;
class CMindMapToolPanel;

class CMindMapToolPanel : public CDiagramHostedWidget
{
	Q_OBJECT

public:
	CMindMapToolPanel(QWidget *parent = 0);
	virtual ~CMindMapToolPanel();

    bool support(CDiagram *d);
	
	// implements the parent
	void init(CDiagramWidget *widget, CDiagram *d);
	void uninit(CDiagramWidget *widget, CDiagram *d);
	void reset();

	// custom methods
	void setTitle(const QString &value);

	void setCollapsed(bool value);
	bool isCollapsed();

    void setCollapsible(bool value);
	bool isCollapsible();

protected:
    virtual void initTools();

Q_SIGNALS:
	void anchorTypeSelected(CDiagramWidget::AnchorType);
	void autoHide(bool);
	void collapsible(bool);

public Q_SLOTS:
	void onShowAnchorMenu();
    void onAnchorTriggered(QAction*);
    void onAutoHideTriggered(bool value);
	void onCollapsibleTriggered(bool);
	void onCollapse();
	void onSettingChanged(const QString &key);

protected:
    Ui::CMindMapToolPanel   *m_ui;

	QVBoxLayout				*m_quickLayout;
	QFormLayout				*m_toolsLayout;
	
	QActionGroup			*m_anchorGroup;
    QMenu					*m_settingMenu;

    QAction                 *m_actAutoHide;
    QAction                 *m_actCollapsible;

	bool					m_collapsed;
	bool					m_collapsible;

}; // End of class CMindMapToolPanel

#endif // PRODRAW_CMINDMAPTOOLPANEL_H
