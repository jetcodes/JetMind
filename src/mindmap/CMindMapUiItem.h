/**
* Copyright (c) 2013 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CMINDMAPUIITEMS_H
#define PRODRAW_CMINDMAPUIITEMS_H

#include <QtGui>
#include <core/CDiagramContextMenuHandler>
#include <widgets/CUiAbstractDialog>

class CDiagram;

namespace Ui {
	class CMindMapUiIconBrowser;
	class CMindMapUiLinkEditor;
	class CMindMapUiNoteEditor;
    class CMindMapUiTaskEditor;
    class CMindMapUiDateEditor;
    class CMindMapUiFileDropper;
}

class CMindMapUiShapeList: public QMenu
{
	Q_OBJECT

public:
	CMindMapUiShapeList(QWidget *parent);
	virtual ~CMindMapUiShapeList();

Q_SIGNALS:
	void shapeSelected(int);

public Q_SLOTS:
	void onActionTriggered(QAction*);

protected:
	void setupMenus();

	QList<QAction*>		m_actions;


}; // End of class CMindMapUiShapeList

class CMindMapUiIconList: public QMenu
{
	Q_OBJECT

public:
    CMindMapUiIconList(QWidget *parent, int cols = 12, int rows=10, int w = 24, int h = 24);
	virtual ~CMindMapUiIconList();

	QSize sizeHint() const;
	virtual QSize minimumSizeHint() const;

    void setIconText(const QString &value);
    QString iconText();
    bool hasIconText();

    void setExtraActionText(const QString &value);
    QString extraActionText();

Q_SIGNALS:
    void browseIconLibrary();
    void endIconList();
    void iconSelected(const QIcon &, int index);

protected:
	void loadIcons();
	void setupMenus();

	virtual void paintEvent( QPaintEvent *event );
	virtual void mousePressEvent( QMouseEvent *event );
	virtual void mouseMoveEvent( QMouseEvent *event );
	virtual void mouseReleaseEvent( QMouseEvent *event);

protected:
	int					m_gridWidth;
	int					m_gridHeight;
	int					m_columns;
	int					m_rows;
	int					m_space;
	int					m_currIconIndex;
	int					m_radius;
    int                 m_minWidth;
    int                 m_iconTextGrids;
    int                 m_defaultColumns;
	QPointF				m_mousePos;
	QIcon				m_defaultIcon;
    QList<QIcon>		m_icons;
	QString				m_iconEndActionText;
    QString				m_extraActionText;

}; // End of class CMindMapUiIconList

class CMindMapUiIconAlterList: public CMindMapUiIconList
{
	Q_OBJECT

public:
    CMindMapUiIconAlterList(QWidget *parent, int cols = 12, int rows=10, int w = 24, int h = 24);
	virtual ~CMindMapUiIconAlterList();

	void setAlterIconIndex(int index);
	int alterIconIndex() const;

	void setIcons(QList<QIcon> &icons, int startIndex);
	QList<QIcon> icons() const;
	int startIndex() const;

Q_SIGNALS:
    void deleteIcon(int);

protected:
	virtual void mousePressEvent( QMouseEvent *event );

protected:
	int			m_startIndex;
	int			m_alterIconIndex;

}; // End of class CMindMapUiIconAlterList
 
class CMindMapUiPixmapList: public QMenu
{
	Q_OBJECT

public:
    CMindMapUiPixmapList(QWidget *parent, int cols = 12, int rows=10, int w = 32, int h = 32);
	virtual ~CMindMapUiPixmapList();

	QSize sizeHint() const;
	virtual QSize minimumSizeHint() const;

Q_SIGNALS:
    void clearPixmap();
    void browsePixmap();
    void pixmapSelected(const QPixmap &pixmap);

protected:
	void setupMenus();

	virtual void paintEvent( QPaintEvent *event );
	virtual void mousePressEvent( QMouseEvent *event );
	virtual void mouseMoveEvent( QMouseEvent *event );
	virtual void mouseReleaseEvent( QMouseEvent *event);

protected:
	int					m_gridWidth;
	int					m_gridHeight;
	int					m_columns;
	int					m_rows;
	int					m_space;
	int					m_currPixmapIndex;
	int					m_radius;
    int                 m_defaultColumns;
	QPointF				m_mousePos;
    QList<QString>		m_pixmaps;

}; // End of class CMindMapUiIconList

class CMindMapUiLayoutList: public QMenu
{
	Q_OBJECT

public:
	CMindMapUiLayoutList(QWidget *parent);
	virtual ~CMindMapUiLayoutList();

	void reset(bool root = true);

Q_SIGNALS:
	void layoutSelected(int);
	
public Q_SLOTS:
	void onActionTriggered(QAction*);

protected:
	void setupMenus();

	QList<QAction*>		m_actions;
	QList<int>			m_layoutTypes;

}; // End of class CMindMapUiLayoutList

class CMindMapUiPathList: public QMenu
{
	Q_OBJECT

public:
	CMindMapUiPathList(QWidget *parent);
	virtual ~CMindMapUiPathList();

Q_SIGNALS:
	void pathSelected(int);

public Q_SLOTS:
	void onActionTriggered(QAction*);

protected:
	void setupMenus();

	QList<QAction*>		m_actions; 

}; // End of class CMindMapUiPathList

class CMindMapUiNewItemList: public QMenu
{
	Q_OBJECT

public:
	CMindMapUiNewItemList(QWidget *parent);
	virtual ~CMindMapUiNewItemList();

	void reset(bool isRoot);

Q_SIGNALS:
	void insertPositionSelected(int, QIcon);

public Q_SLOTS:
	void onActionTriggered(QAction*);

protected:
	void setupMenus();
	QList<QAction*>		m_actions;
	QList<QIcon>		m_icons;

}; // End of classCMindMapUiNewItemList

class CMindMapUiIconBrowser: public CUiAbstractDialog
{
	Q_OBJECT

public:
	CMindMapUiIconBrowser(QWidget *parent = NULL);
	virtual ~CMindMapUiIconBrowser();

Q_SIGNALS:
    void iconSelected(const QIcon &, int index);

protected Q_SLOTS:
    void onOk();
    void onCancel();
    void onItemSelected(QListWidgetItem *item);

protected:
    void loadIcons();

	Ui::CMindMapUiIconBrowser	*m_ui;

}; // End of class CMindMapUiIconBrowser

class CMindMapUiLinkEditor: public CUiAbstractDialog
{
	Q_OBJECT

public:
	CMindMapUiLinkEditor(QWidget *parent = NULL);
	virtual ~CMindMapUiLinkEditor();

    int linkType();
    QUrl linkAddress();

    bool reset(CDiagram *d);
	
Q_SIGNALS:
    void deleteLink();
    void resetLink(int, QString);

public Q_SLOTS:
	void onLinkTypeChanged(int type);
	void onCancel();
	void onOk();
	void onDelete();

protected:
	Ui::CMindMapUiLinkEditor	*m_ui;
	
    int         m_status;
	int			m_linkType;
	QString		m_linkAddress;
    CDiagram    *m_d;

}; // End of class CMindMapUiLinkEditor

class CMindMapUiNoteEditor: public CUiAbstractDialog
{
	Q_OBJECT

public:
	CMindMapUiNoteEditor(QWidget *parent = NULL);
	virtual ~CMindMapUiNoteEditor();

    void setText(const QString &value);
    QString text();

    bool reset(CDiagram *d);

Q_SIGNALS:
    void deleteNote();
    void resetNote(const QString&);

public Q_SLOTS:
    void onCancel();
    void onOk();
    void onDelete();

protected:
	Ui::CMindMapUiNoteEditor	*m_ui;

    QString                     m_text;

}; // End of class CMindMapUiNoteEditor


class CMindMapUiDateEditor: public CUiAbstractDialog
{
    Q_OBJECT

public:
    CMindMapUiDateEditor(QWidget *parent = NULL);
    virtual ~CMindMapUiDateEditor();

    bool reset(CDiagram *d);
    bool enableTime();

Q_SIGNALS:
    void dateSelected(QString);

public Q_SLOTS:
    void onOk();
    void onCancel();
    void onReset();
    void onEnabledTime(bool);
    void onTimeChanged(QTime);
    void onDateChanged(QDate);

protected:
    Ui::CMindMapUiDateEditor    *m_ui;
	void updateDateString();

    bool                        m_enableTime;
	QString						m_dateString;

}; // End of class CMindMapUiDateEditor

class CMindMapUiTaskEditor: public CUiAbstractDialog
{
    Q_OBJECT

public:
    CMindMapUiTaskEditor(QWidget *parent = NULL);
    virtual ~CMindMapUiTaskEditor();

    bool reset(CDiagram *d);

Q_SIGNALS:
    void deleteTask();
    void resetTask(int priority,
                   int progress,
                   const QString &assignee,
                   const QString &dueDate,
                   bool enableTime);

public Q_SLOTS:
    void onCancel();
    void onOk();
    void onDelete();
	void onShowDateEditor();
	void onDateSelected(QString);
	void onAssignedTo(QString);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

    Ui::CMindMapUiTaskEditor    *m_ui;

	CMindMapUiDateEditor		*m_dateEditor;
	CDiagram					*m_d;

}; // End of class CMindMapUiTaskEditor

class CMindMapUiGraphMenu : public CDiagramContextMenu
{
    Q_OBJECT

public:
    CMindMapUiGraphMenu();
    virtual ~CMindMapUiGraphMenu();

    bool support(CDiagram *d);
	void reset(CDiagram *d);
    void apply(QMenu *menu);

Q_SIGNALS:
    void insertTopicBefore();
    void insertTopicAfter();
    void insertTopicParent();
    void insertTopicChild();
    void addBoundary();
    void shapeSelected(int);
    void pathSelected(int);
    void layoutSelected(int);

protected:
	Q_DISABLE_COPY(CMindMapUiGraphMenu)
	
	void setupMenu();

    CDiagram                *m_d;

    CMindMapUiLayoutList    *m_layoutMenu;
    CMindMapUiShapeList     *m_shapeMenu;
    CMindMapUiPathList      *m_pathMenu;

}; // End of class CMindMapUiMenu

class CMindMapUiFileDropper: public CUiAbstractDialog
{
    Q_OBJECT

public:
    CMindMapUiFileDropper();
    virtual ~CMindMapUiFileDropper();

    bool reset(CDiagram *d);

Q_SIGNALS:
    void dropFiles(const QStringList &files);
    void removeAttach();

public Q_SLOTS:
    void onOk();
    void onCancel();

protected:
    Q_DISABLE_COPY(CMindMapUiFileDropper)

    Ui::CMindMapUiFileDropper    *m_ui;
    CDiagram                     *m_d;

}; // End of class CMindMapUiFileDropper

#endif // PRODRAW_CMINDMAPUIITEMS_H
