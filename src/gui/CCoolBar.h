/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CCOOLBAR_H
#define PRODRAW_CCOOLBAR_H

#include <QtGui>
#include <core/CDiagram>
#include <core/CDiagramItem>
#include "ui_CCoolBar.h"

class CWorkbench;
class CDiagramItemType;
class CCoolBar;
class CDiagramEdgeStyle;
class CUiCoreNodePicker;
class CUiPathStyleList;
class CUiPathStylePicker;
class CUiColorPanel;
class CUiPenStylePicker;
class CUiPenWeightPicker;
class CUiNodeStylePicker;
class CUiOpenedFilePicker;

class CCoolBar : public QWidget
{
	Q_OBJECT

public:
	CCoolBar(QWidget *parent = 0);
	virtual ~CCoolBar();

	void paintEvent(QPaintEvent *event);
	QMetaObject* currItemMeta() { return m_currMeta; }

	void setOpenedFilesModel(QAbstractItemModel	*model);
	QAbstractItemModel* openedFilesModel();

	void toggleFashionMode(bool value);
	bool fashionMode();

	void lockFocus(bool value);

Q_SIGNALS:
	void insertTextMode(bool);
	void insertLinkMode(bool);
	void cursorMode();
    void insertFreeTopic();

public Q_SLOTS:
    void onFilterNodes();
    void onFilterNodes(QString keywords);
    void onFilterTypeChanged(QAction *action);

    void onSwitchFile(int);
	void onOpenedFileList();
	void onViewNextFile();
	void onViewPrevFile();

	void onInsertLink();
	void onInsertText();
    void onInsertFreeTopic();

	void onRouterChanged(CDiagramEdgeStyle *r);
	void onNewItemAdded(CDiagramItem *item);
	void onDiagramModeChanged(CDiagram::DiagramMode mode);
	void onDiagramChanged(CDiagram *curr, CDiagram *prev);

protected:
	bool eventFilter(QObject *obj, QEvent *event);
	void relocate();

	Ui::CCoolBar			*m_ui;

    CUiOpenedFilePicker		*m_openedFileList;
    QButtonGroup			*m_tools;
    QMetaObject				*m_currMeta;
    CDiagramItemType		*m_wrapper;
    CWorkbench				*m_bench;
	QCompleter				*m_completer;
    QMenu                   *m_filterTypes;

#if defined(__APPLE__) && defined(__MACH__)
    CUiPathStyleList        *m_edgeStyles;
#else
	CUiPathStylePicker		*m_edgeStyles;
#endif

	friend class CWorkbench;

}; // End of class CCoolBar

#endif // PRODRAW_CCOOLBAR_H
