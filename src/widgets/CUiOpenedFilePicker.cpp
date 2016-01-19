/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include <QtGui>
#include <core/CGlobal>
#include <core/CUtils>

#include "CUiOpenedFilePicker.h"
#include "ui_CUiOpenedFilePicker.h"

CUiFileItemDelegate::CUiFileItemDelegate(QObject *parent)
    : QItemDelegate(parent),
	  m_padding(8),
      m_spacing(8),
      m_fixedWidth(180)
{
	m_titleFont = QFont("Arial", 12, QFont::Bold, false);
	m_tagFont = QFont("Arial", 9, QFont::Normal, true);
}

QWidget* CUiFileItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QLineEdit			*editor = NULL;

	editor = new QLineEdit(NULL);
    return editor;
}

void CUiFileItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool			saved = false;
    int             revision = 0;
    QDateTime		lastUpdatedDate;
    QString			updatedDate, fileName, info;
    QString			tpl("Revision:%1 Updated: %2");
	QRect			r, textRect;

	fileName = index.data( Qt::DisplayRole ).value<QString>();
    saved = !(index.data( Qt::UserRole + 4 ).value<bool>());
    lastUpdatedDate = index.data( Qt::UserRole + 2).value<QDateTime>();
    revision = index.data( Qt::UserRole + 5).value<int>();

    fileName = QFontMetrics(m_titleFont).elidedText(
                saved? fileName : QString("[*]%1").arg(fileName),
                Qt::ElideRight,
                m_fixedWidth);

    updatedDate = lastUpdatedDate.toString("yyyy-MM-dd hh:mm ap");
    info = QFontMetrics(m_tagFont).elidedText(
                tpl.arg( QString::number(revision) ).arg(updatedDate),
                Qt::ElideRight,
                m_fixedWidth);

    if (index.row() % 2 == 1)
    {
        painter->fillRect( option.rect, QColor(238, 238, 238) );
    }

    if (option.state & QStyle::State_Selected)
	{
        painter->fillRect( option.rect, QColor(22, 110, 200, 48) );
	}

    if (option.state & QStyle::State_MouseOver)
	{
        painter->fillRect( option.rect, QColor(22, 110, 200, 48) );
	}

	
	painter->save();

	// draw the content
	r = option.rect;
	textRect = QRect( r.topLeft(), QSize(r.width(), QFontMetrics(m_titleFont).height()) );
	textRect.adjust(m_padding, m_padding, -2 * m_padding, m_padding);

	painter->setPen( Qt::black );
	painter->setFont( m_titleFont );
    painter->drawText( textRect, Qt::AlignLeft | Qt::AlignVCenter, fileName );

	textRect = QRect( textRect.bottomLeft(), QSize(r.width(), QFontMetrics(m_tagFont).height()));
    textRect.adjust(0, m_spacing, 0, m_spacing);

	painter->setPen( Qt::gray );
	painter->setFont( m_tagFont );
	painter->drawText( textRect, Qt::AlignLeft | Qt::AlignVCenter, info );

//    if (index.model()->rowCount() - 1 != index.row())
//        painter->drawLine( r.bottomLeft(), r.bottomRight());

	painter->restore();
}


QSize CUiFileItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    int     h = 0;

	h = QFontMetrics(m_titleFont).height() + QFontMetrics(m_tagFont).height() + m_spacing + 2 * m_padding;
    return QSize(m_fixedWidth, h);
}

void CUiFileItemDelegate::setEditorData( QWidget * editor, const QModelIndex & index ) const
{
	QString			fileName;
	QLineEdit		*lineEdit = NULL;

	lineEdit = dynamic_cast<QLineEdit*>(editor);
	fileName = index.data( Qt::DisplayRole ).value<QString>();
	if (lineEdit)
	{
		lineEdit->setText( fileName );
	}
}

void CUiFileItemDelegate::setModelData( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
{
	QLineEdit		*lineEdit = NULL;

	lineEdit = dynamic_cast<QLineEdit*>(editor);
	if (!model || !lineEdit)
		return;

	model->setData(index, lineEdit->text(), Qt::DisplayRole);
}

void CUiFileItemDelegate::updateEditorGeometry ( QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	editor->setGeometry( option.rect );
}

CUiOpenedFilePicker::CUiOpenedFilePicker(QWidget *parent)
	: QWidget(parent),
      m_proxyModel(new QSortFilterProxyModel(this))
{
	ui = new Ui::CUiOpenedFilePicker();
	ui->setupUi(this);

	m_proxyModel->setFilterRole(Qt::DisplayRole);
	ui->m_fileListView->setModel(m_proxyModel);
    ui->m_fileListView->setModelColumn(1);
    ui->m_fileListView->setItemDelegate( new CUiFileItemDelegate(NULL) );
	ui->m_fileListView->setEditTriggers( QAbstractItemView::NoEditTriggers );

	connect(ui->m_fileListView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onClick(const QModelIndex&)));
	connect(ui->m_fileListView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(onDoubleClick(const QModelIndex&)));
	connect(ui->m_filter, SIGNAL(textEdited(const QString &)), this, SLOT(onFilterFiles(const QString&)));
	connect(ui->m_btnUp,SIGNAL(clicked()), this, SLOT(onNavigateUp()));
	connect(ui->m_btnDown, SIGNAL(clicked()), this, SLOT(onNavigateDown()));
}

CUiOpenedFilePicker::~CUiOpenedFilePicker()
{
	delete ui;
}

void CUiOpenedFilePicker::setModel(QAbstractItemModel *model)
{
	m_proxyModel->setSourceModel(model);
}

QAbstractItemModel* CUiOpenedFilePicker::model()
{
	return m_proxyModel->sourceModel();
}

void CUiOpenedFilePicker::paintEvent(QPaintEvent *event)
{
	QStyleOption		opt;
	QPainter			p(this);

	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CUiOpenedFilePicker::onDoubleClick(const QModelIndex &index)
{
	QStandardItem		*item = NULL;
	QStandardItemModel	*model = NULL;

	model = dynamic_cast<QStandardItemModel*>(m_proxyModel->sourceModel());
	if (!model)
		return;

	item = model->itemFromIndex(m_proxyModel->mapToSource(index));
	if (item)
	{
		emit switchToFile( item->data( Qt::UserRole + 1 ).toInt() );
	}
	hide();
}

void CUiOpenedFilePicker::onClick(const QModelIndex &index)
{

}

void CUiOpenedFilePicker::onFilterFiles(const QString &filter)
{
	if (!m_proxyModel)
		return;
	m_proxyModel->setDynamicSortFilter(true);
	m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setFilterRegExp(filter);
}

void CUiOpenedFilePicker::onNavigateUp()
{
	QScrollBar	*vBar = NULL;

	vBar = ui->m_fileListView->verticalScrollBar();
	if (vBar)
	{
		if (QApplication::keyboardModifiers() & Qt::ControlModifier)
			vBar->setValue( vBar->value() - vBar->singleStep() );
		else
			ui->m_fileListView->scrollToTop();
	}
}

void CUiOpenedFilePicker::onNavigateDown()
{
	QScrollBar	*vBar = NULL;

	vBar = ui->m_fileListView->verticalScrollBar();
	if (vBar)
	{
		if (QApplication::keyboardModifiers() & Qt::ControlModifier)
			vBar->setValue( vBar->value() + vBar->singleStep() );
		else
			ui->m_fileListView->scrollToBottom();
	}
}

bool CUiOpenedFilePicker::eventFilter(QObject *obj, QEvent *ev)
{
	QEvent::Type	t;

	t = ev->type();

	switch(t)
	{
	case QEvent::HoverEnter:
		if (obj == ui->m_btnUp)
		{
		}
		else if (obj == ui->m_btnDown)
		{
		}
		break;

	case QEvent::HoverLeave:
		break;

	default:
		break;
	};
	return QWidget::eventFilter(obj, ev);
}
