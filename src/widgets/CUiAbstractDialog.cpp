/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/

#include <QtGui>
#include "CUiAbstractDialog.h"
#include "ui_CUiAbstractDialog.h"

CUiAbstractDialog::CUiAbstractDialog(QWidget *parent)
	: QDialog(parent),
	  m_dragging(false),
      m_main(NULL)
{
	ui = new Ui::CUiAbstractDialog();
	ui->setupUi(this);

	setModal(true);
    setWindowIcon(QIcon());

	m_main = ui->m_main;
	connect(ui->m_btnClose, SIGNAL(clicked()), this, SLOT(onClose()));

#if defined(__APPLE__) && defined(__MACH__)
    setStyleMode(CUiAbstractDialog::Normal);
#else
    setStyleMode(CUiAbstractDialog::Custom);
#endif //

}

CUiAbstractDialog::~CUiAbstractDialog()
{
	delete ui;
}

void CUiAbstractDialog::setStyleMode(CUiAbstractDialog::StyleMode mode)
{
    m_styleMode = mode;
    if (CUiAbstractDialog::Custom == m_styleMode)
    {
        setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        if (ui->m_titleBar->isHidden())
            ui->m_titleBar->show();
    }
    else
    {
        setWindowFlags(Qt::Dialog);
        ui->m_titleBar->hide();
    }
}

int CUiAbstractDialog::styleMode() const
{
    return m_styleMode;
}
void CUiAbstractDialog::mousePressEvent(QMouseEvent *ev)
{
	m_initPos = pos();
	m_clickPos = ev->globalPos();
	m_dragging = true;

	QDialog::mousePressEvent(ev);
}

void CUiAbstractDialog::mouseMoveEvent(QMouseEvent *ev)
{
	if (m_dragging)
		move(m_initPos + ev->globalPos() - m_clickPos);
	QDialog::mouseMoveEvent(ev);
}

void CUiAbstractDialog::mouseReleaseEvent(QMouseEvent *ev)
{
	if (m_dragging)
		m_dragging = false;
	QDialog::mouseReleaseEvent(ev);
}

void CUiAbstractDialog::onClose()
{
	close();
}

void CUiAbstractDialog::setTitle(const QString &value)
{
	ui->m_title->setText(value);
    setWindowTitle(value);
}

QString CUiAbstractDialog::title()
{
	return ui->m_title->text();
}

void CUiAbstractDialog::resizeEvent(QResizeEvent *event)
{
    //qreal		tl = 4, tr = 4, bl = 4, br = 4;
    //QRegion		region, round, corner;

	QWidget::resizeEvent(event);

    /*
	region = QRegion(0, 0, width(), height(), QRegion::Rectangle);
 
    // top left
    round = QRegion(0, 0, 2*tl, 2*tl, QRegion::Ellipse);
    corner = QRegion(0, 0, tl, tl, QRegion::Rectangle);
    region = region.subtracted(corner.subtracted(round));
 
    // top right
    round = QRegion(width() - 2 * tr, 0, 2 * tr, 2 * tr, QRegion::Ellipse);
    corner = QRegion(width() - tr, 0, tr, tr, QRegion::Rectangle);
    region = region.subtracted(corner.subtracted(round));
 
    // bottom right
    round = QRegion(width() - 2 * br, height() - 2 * br, 2 * br, 2 * br, QRegion::Ellipse);
    corner = QRegion(width() - br, height() - br, br, br, QRegion::Rectangle);
    region = region.subtracted(corner.subtracted(round));
 
    // bottom left
    round = QRegion(0, height() - 2 * bl, 2 * bl, 2 * bl, QRegion::Ellipse);
    corner = QRegion(0, height() - bl, bl, bl, QRegion::Rectangle);
    region = region.subtracted(corner.subtracted(round));
 
    setMask(region);
    */
}

void CUiAbstractDialog::paintEvent(QPaintEvent *event)
{
	QStyleOption		opt;
	QPainter			p(this);

	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
