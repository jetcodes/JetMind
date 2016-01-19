/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#ifndef PRODRAW_CQTABBOX_H
#define PRODRAW_CQTABBOX_H

#include <QtGui>
#include <widgets/CUiGlobal>

class CUiTabBox;

class WIDGETS_EXPORT CUiTab: public QPushButton
{
	Q_OBJECT

public:
	CUiTab(QWidget *parent);

	inline void setSelected(bool b);
	inline void setIndex(int index);

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

private:
	bool	m_selected;
	int		m_index;
	CUiTabBox	*m_box;
};

class WIDGETS_EXPORT CUiTabBox : public QFrame
{
	Q_OBJECT

public:
	struct TabItem {
		CUiTab			*tab;
        QWidget			*widget;

		inline bool operator==(const TabItem& other) const
        {
            return widget == other.widget;
        }
	};

	enum LayoutMode {
		Horizontal,
		Vertical
	}; // End of enumeration

	CUiTabBox(QWidget *parent);
	virtual ~CUiTabBox();

	int addItem(QWidget *widget, const QString &text);
	int addItem(QWidget *widget, const QIcon &icon, const QString &text);
	int insertItem(int index, QWidget *widget, const QString &text);
	int insertItem(int index, QWidget *widget, const QIcon &icon, const QString &text);
	void removeItem(int index);

    void setItemText(int index, const QString &text);
    QString itemText(int index) const;

    void setItemIcon(int index, const QIcon &icon);
    QIcon itemIcon(int index) const;

	int currentIndex() const;
	QWidget *currentWidget() const;
	QWidget *widget(int index) const;
    int indexOf(QWidget *widget) const;
    int count() const;

	void setLayoutMode(int mode);
	int layoutMode();

	void paintEvent(QPaintEvent *event);

public Q_SLOTS:
    void setCurrentIndex(int index);
    void setCurrentWidget(QWidget *widget);

Q_SIGNALS:
    void currentChanged(int index);

protected:
	TabItem* item(QWidget *widget);
	TabItem* item(int index);
	const TabItem* item(QWidget *widget) const;
	const TabItem* item(int index) const;

	bool event(QEvent *e) { return QFrame::event(e); }
	void showEvent(QShowEvent *e) { QWidget::showEvent(e); }
    void changeEvent(QEvent *);

	void relayout();
	void updateTabs();
	void reset();

protected Q_SLOTS:
	void onWidgetDestroyed(QObject*);
	void onTabClicked();

private:
	QSplitter			*m_splitter;
	QWidget				*m_ctrl;
	QBoxLayout			*m_ctrlLayout;
	QScrollArea			*m_ctrlScroll;
	int					m_layoutMode;

	QWidget				*m_view;
	QStackedLayout		*m_viewLayout;
	QScrollArea			*m_viewScroll;;

	QWidget				*m_ctrlHolder;
	TabItem				*m_currentItem;

	QList<CUiTabBox::TabItem>		m_items;
};

#endif // PRODRAW_CQTABBOX_H
