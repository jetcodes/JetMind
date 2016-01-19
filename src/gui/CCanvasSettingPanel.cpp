/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#include <QtGui>
#include <widgets/CUiColorGrid>
#include <mindmap/CMindMapSettings>
#include "CCanvasSettingPanel.h"
#include "ui_CCanvasSettingPanel.h"

CCanvasSettingPanel::CCanvasSettingPanel(QWidget *parent)
    : CUiAbstractDialog(parent),
      m_d(NULL),
      m_colorGrid(NULL)
{
	QVBoxLayout			*layout = NULL;
    CMindMapSetting     *setting = NULL;

    ui = new Ui::CCanvasSettingPanel();
    ui->setupUi(m_main);

    m_colorGrid = new CUiColorGrid(this);

    setting = CMindMapSetting::instance();
    if (setting)
    {
        ui->m_showGrids->setChecked( setting->get(Settings::MindMapBackgroundShowGrid, QVariant(false)).toBool() );
        connect(setting, SIGNAL(configChanged(QString)), this, SLOT(onSettingChanged(QString)));
    }

    ui->m_setAsDefaultStyle->setChecked(false);
	layout = dynamic_cast<QVBoxLayout*>(ui->m_stylesHolder->layout());
	if (layout)
	{
        layout->insertWidget(2, m_colorGrid);
	}

	connect(m_colorGrid, SIGNAL(colorSelected(QColor)), this, SLOT(onBackgroundColorSelected(QColor)));
    connect(m_colorGrid, SIGNAL(colorSelectionDone()), this, SLOT(onBackgroundColorDone()));
    connect(ui->m_btnMoreStyle, SIGNAL(clicked()), this, SLOT(onShowColorGrid()));
    connect(ui->m_showGrids, SIGNAL(toggled(bool)), this, SIGNAL(gridsVisibleChanged(bool)));
    connect(ui->m_setAsDefaultStyle, SIGNAL(toggled(bool)), this, SLOT(onSetDefaultStyle(bool)));
    connect(ui->m_styleList, SIGNAL(clicked(QModelIndex)), this, SLOT(onStyleClicked(QModelIndex)));
    connect(ui->m_styleList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onStyleDoubleClicked(QModelIndex)));

    setTitle(tr("Settings"));
    resize(260, 260);

    m_colorGrid->hide();

    setModal(false);
}

CCanvasSettingPanel::~CCanvasSettingPanel()
{
    delete ui;
}

void CCanvasSettingPanel::update(CDiagram *d)
{
    m_d = d;
}

void CCanvasSettingPanel::setStyles(QList<QBrush> &brushes)
{
    m_brushes = brushes;
    setupStyles();
}

QList<QBrush> CCanvasSettingPanel::styles() const
{
    return m_brushes;
}

void CCanvasSettingPanel::addStyle(const QBrush &value)
{
    m_brushes.append(value);
    setupStyles();
}

QBrush CCanvasSettingPanel::getStyle(int index) const
{
    if (index < 0 || index > m_brushes.length() - 1)
        return QBrush(Qt::white);

    return m_brushes.at(index);
}


void CCanvasSettingPanel::setupStyles()
{
    int                     size = 64;
    QStandardItem           *item = NULL;
    QStandardItemModel      *model = NULL;

    model = new QStandardItemModel();
    if (!model)
        return;

    foreach (QBrush brush, m_brushes)
    {
        QIcon       icon;
        QString     text;
        QPixmap     pixmap(size, size);
        QPainter    painter;

        painter.begin(&pixmap);
        painter.setBrush( brush );
        painter.setPen( Qt::lightGray );
        painter.drawRect( QRectF(0, 0, size - 1, size - 1) );
        painter.end();

        icon = QIcon(pixmap);
        item = new QStandardItem(icon, text);
        item->setData( QVariant(brush), Qt::UserRole + 1);
        model->appendRow(item);
    }

    ui->m_styleList->setModel(model);
    ui->m_styleList->setIconSize( QSize(size, size) );
    ui->m_styleList->setViewMode( QListView::IconMode );
    ui->m_styleList->setResizeMode(QListView::Adjust);
    ui->m_styleList->setSelectionRectVisible(true);
    ui->m_styleList->setUniformItemSizes(true);

}

void CCanvasSettingPanel::onStyleClicked(QModelIndex index)
{
    QBrush              brush;
    QStandardItem       *item = NULL;
    QStandardItemModel  *model = NULL;

    model = dynamic_cast<QStandardItemModel*> ( ui->m_styleList->model() );
    if (model)
    {
        item = model->itemFromIndex(index);
        brush = item->data( Qt::UserRole + 1).value<QBrush>();
    }
    emit styleSelected(brush);
}

void CCanvasSettingPanel::onStyleDoubleClicked(QModelIndex index)
{
    close();
}

void CCanvasSettingPanel::onSetDefaultStyle(bool value)
{
    CMindMapSetting     *setting = NULL;

    if (value)
    {
        setting = CMindMapSetting::instance();
        if (setting)
        {
            QBrush              brush;
            QStandardItem       *item = NULL;
            QStandardItemModel  *model = NULL;

            model = dynamic_cast<QStandardItemModel*> ( ui->m_styleList->model() );
            if (model)
            {
                item = model->itemFromIndex( ui->m_styleList->currentIndex() );
                if (item)
                {
                    brush = item->data( Qt::UserRole + 1 ).value<QBrush>();
                    setting->set(Settings::MindMapBackgroundBrush, QVariant(brush));
                }
            }
        }
    }
}

void CCanvasSettingPanel::changeEvent (QEvent *event)
{
    if(event->type() == QEvent::ActivationChange)
    {
        if (!isActiveWindow())
            close();
    }
}

void CCanvasSettingPanel::onSettingChanged(const QString &key)
{
    bool                r = false;
    CMindMapSetting     *setting = NULL;

    if (key == Settings::MindMapBackgroundShowGrid)
    {
        setting = CMindMapSetting::instance();
        if (setting)
        {
            r = setting->get(key, QVariant(false)).toBool();
            if (ui->m_showGrids->isChecked() != r)
            {
                ui->m_showGrids->setChecked(r);
                emit gridsVisibleChanged(r);
            }
        }
    }
}

void CCanvasSettingPanel::onBackgroundColorSelected(QColor c)
{
    emit styleSelected(QBrush(c));
}

void CCanvasSettingPanel::onShowColorGrid()
{
    if (!m_colorGrid)
        return;

	if (m_colorGrid->isVisible())
		m_colorGrid->hide();
	else
		m_colorGrid->show();
	ui->m_stylesHolder->layout()->invalidate();
}

void CCanvasSettingPanel::onBackgroundColorDone()
{
    if (!m_colorGrid)
        return;

    m_colorGrid->hide();
	ui->m_stylesHolder->layout()->invalidate();
	close();
}

void CCanvasSettingPanel::hideEvent(QHideEvent *evt)
{
    CUiAbstractDialog::hideEvent(evt);

    if (m_colorGrid)
        m_colorGrid->hide();
}

