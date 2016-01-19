/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#ifndef PRODRAW_CUIOPENEDFILEPICKER_H
#define PRODRAW_CUIOPENEDFILEPICKER_H

#include <QtGui>
#include <widgets/CUiGlobal>
#include <core/CDiagramFile>

namespace Ui {
    class CUiOpenedFilePicker;
}

class WIDGETS_EXPORT CUiFileItemDelegate: public QItemDelegate
{
    Q_OBJECT

public:
    CUiFileItemDelegate(QObject *parent);

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData( QWidget * editor, const QModelIndex & index ) const;
    void setModelData( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const;
    void updateEditorGeometry ( QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index ) const;

protected:
	int				m_padding;
	int				m_spacing;
    int             m_fixedWidth;
	QFont			m_titleFont;
	QFont			m_tagFont;

}; // End of class FileItemDelegate

class WIDGETS_EXPORT CUiOpenedFilePicker : public QWidget
{
	Q_OBJECT

public:
	CUiOpenedFilePicker(QWidget *parent = 0);
	virtual ~CUiOpenedFilePicker();

	void paintEvent(QPaintEvent *event);

	void setModel(QAbstractItemModel *model);
	QAbstractItemModel* model();

Q_SIGNALS:
	void switchToFile(int key);

public Q_SLOTS:
	void onFilterFiles(const QString&);
	void onClick(const QModelIndex &);
	void onDoubleClick(const QModelIndex&);
	void onNavigateUp();
	void onNavigateDown();

protected:
	bool eventFilter(QObject *obj, QEvent *ev);

private:
	Ui::CUiOpenedFilePicker		*ui;

	QSortFilterProxyModel	*m_proxyModel;

}; // End of class CUiOpenedFilePicker

#endif // PRODRAW_CUIOPENEDFILEPICKER_H
