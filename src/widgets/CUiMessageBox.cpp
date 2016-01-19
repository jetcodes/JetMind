/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <core/CGlobal>

#include "CUiMessageBox.h"
#include "ui_CUiMessageBox.h"

CUiMessageBox::CUiMessageBox(QWidget *parent)
	: CUiAbstractDialog(parent),
	  m_btnGroup(NULL),
	  m_result(Ok)
{
	ui = new Ui::CUiMessageBox();
	ui->setupUi(m_main);

	m_btnGroup = new QButtonGroup(this);
	m_btnGroup->addButton(ui->m_btnOk);
	m_btnGroup->addButton(ui->m_btnCancel);
	m_btnGroup->addButton(ui->m_btnYes);
	m_btnGroup->addButton(ui->m_btnNo);
	m_btnGroup->addButton(ui->m_btnSave);
	m_btnGroup->addButton(ui->m_btnDiscard);

	m_buttons.insert(ui->m_btnOk, Ok);
	m_buttons.insert(ui->m_btnCancel, Cancel);
	m_buttons.insert(ui->m_btnYes, Yes);
	m_buttons.insert(ui->m_btnNo, No);
	m_buttons.insert(ui->m_btnSave, Save);
	m_buttons.insert(ui->m_btnDiscard, Discard);

	connect(m_btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(onButtonClicked(int)));
}

CUiMessageBox::~CUiMessageBox()
{
	delete ui;
}

void CUiMessageBox::onButtonClicked(int id)
{
	m_result = m_buttons.value( m_btnGroup->button(id) );
	accept();
}

int CUiMessageBox::result()
{
	return m_result;
}

void CUiMessageBox::information(const QString &title, const QString &text, CUiMessageBox::Buttons buttons)
{
    int     w = 360, h = 0, delta = 32;

	setTitle(title);
	ui->m_info->setText(text);
	
	ui->m_btnOk->setVisible( buttons.testFlag(Ok) );
	ui->m_btnDiscard->setVisible( buttons.testFlag(Discard) );
	ui->m_btnCancel->setVisible( buttons.testFlag(Cancel) );
	ui->m_btnYes->setVisible( buttons.testFlag(Yes) );
	ui->m_btnNo->setVisible( buttons.testFlag(No) );
	ui->m_btnSave->setVisible( buttons.testFlag(Save) );

	if (ui->m_btnOk->isVisible())  ui->m_btnOk->setFocus(Qt::TabFocusReason);
	if (ui->m_btnYes->isVisible()) ui->m_btnYes->setFocus(Qt::TabFocusReason);

    h = ui->m_info->heightForWidth(w);
    resize(w, h + delta);
}

void CUiMessageBox::question(const QString &title, const QString &text, CUiMessageBox::Buttons buttons)
{
    int     w = 360, h = 0, delta = 32;

	setTitle(title);
	ui->m_info->setText(text);

	ui->m_btnOk->setVisible( buttons.testFlag(Ok) );
	ui->m_btnDiscard->setVisible( buttons.testFlag(Discard) );
	ui->m_btnCancel->setVisible( buttons.testFlag(Cancel) );
	ui->m_btnYes->setVisible( buttons.testFlag(Yes) );
	ui->m_btnNo->setVisible( buttons.testFlag(No) );
	ui->m_btnSave->setVisible( buttons.testFlag(Save) );

	if (ui->m_btnOk->isVisible())  ui->m_btnOk->setFocus(Qt::TabFocusReason);
	if (ui->m_btnYes->isVisible()) ui->m_btnYes->setFocus(Qt::TabFocusReason);

    h = ui->m_info->heightForWidth(w);
    resize(w, h + delta);
}

int CUiMessageBox::information(QWidget *parent, const QString &title, const QString &text, CUiMessageBox::Buttons buttons)
{
	int			r = 0;
	CUiMessageBox	*msg = NULL;
	
	msg = new CUiMessageBox(parent);
	msg->setWindowModality(Qt::WindowModal);
	msg->information(title, text, buttons);

	msg->exec();

	r = msg->result();
	SAFE_DELETE(msg);
	return r;
}

int CUiMessageBox::question(QWidget *parent, const QString &title, const QString &text, CUiMessageBox::Buttons buttons)
{
	int			r = 0;
	CUiMessageBox	*msg = NULL;
	
	msg = new CUiMessageBox(parent);
	msg->setWindowModality(Qt::WindowModal);
	msg->question(title, text, buttons);

	msg->exec();
	r = msg->result();
	SAFE_DELETE(msg);
	return r;
}

void CUiMessageBox::paintEvent(QPaintEvent *event)
{
	QStyleOption		opt;
	QPainter			p(this);
	QRect				r;
	//QPainterPath			path;
	//QRegion				region;
	//int					roundness = 3;

	r = rect();
	/*
	path.addRoundRect(0, 0, r.width(), r.height(), roundness, roundness);
	p.setClipPath(path);
	region = p.clipRegion();
	setMask(region);
	*/
	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
