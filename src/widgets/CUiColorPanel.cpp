/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/
#include "ui_CUiColorPanel.h"
#include "CUiColorGrid.h"
#include "CUiColorPanel.h"
#include "CUiGradientEditor.h"

CUiColorPanel::CUiColorPanel(QWidget *parent)
	: QFrame(parent, Qt::Popup),
	  m_mode(CUiColorPanel::SolidFill),
	  m_moreColor(false),
	  m_transparency(0)
{
	QStringList			types;
	QStringListModel	*model = new QStringListModel();
	QAbstractItemView	*view = new QListView(NULL);

    types << tr("Solid") << tr("Linear Gradient");
	model->setStringList(types);
	view->setModel(model);

	ui = new Ui::CUiColorPanel();
	ui->setupUi(this);

	ui->m_cbFillType->setModel(model);
	ui->m_cbFillType->setView(view);

	QHBoxLayout		*layout = new QHBoxLayout();
	layout->setContentsMargins(3, 3, 3, 3);
	layout->setSpacing(0);
	m_grid = new CUiColorGrid(this);
	layout->addWidget(m_grid);
	ui->m_gridHolder->setLayout( layout );

	QHBoxLayout		*layout2 = new QHBoxLayout();
	layout2->setContentsMargins(3, 3, 3, 3);
	layout2->setSpacing(0);
	m_gradientEditor = new CUiGradientEditor(this);
	layout2->addWidget(m_gradientEditor);
	ui->m_gradientEditor->setLayout(layout2);

	ui->m_colorIndicator->setAutoFillBackground(true);
	
	connect(m_grid, SIGNAL(colorHovered(QColor)), this, SLOT(onColorHovered(QColor)));
	connect(m_grid, SIGNAL(colorSelected(QColor)), this, SLOT(onColorSelected(QColor)));
	connect(m_grid, SIGNAL(colorSelectionDone()), this, SLOT(onColorSelectionDone()));
	connect(ui->m_btnClose, SIGNAL(clicked()), this, SLOT(onClose()));
	connect(ui->m_cbFillType, SIGNAL(currentIndexChanged(int)), this, SLOT(onFillTypeChanged(int)));
	connect(ui->m_btnNoFill, SIGNAL(clicked()), this, SLOT(onNoColor()));
	connect(ui->m_colorValue, SIGNAL(editingFinished()), this, SLOT(onColorValueChanged()));
	connect(ui->m_colorValue, SIGNAL(textEdited(const QString&)), this, SLOT(onColorValueEditing(const QString&)));
	connect(ui->m_transEdit, SIGNAL(textEdited(const QString&)), this, SLOT(onTransparencyChanged(const QString&)));
	connect(ui->m_transSlider, SIGNAL(sliderMoved(int)), this, SLOT(onTransparencyChanged(int))); 
	connect(ui->m_btnColorMore,SIGNAL(clicked()), this, SLOT(onMoreColor()));

	setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Plain);

	setMode(SolidFill);
}

CUiColorPanel::~CUiColorPanel()
{

}

void CUiColorPanel::setMode(int mode)
{
	m_mode = mode;
	if (ui->m_cbFillType->currentIndex() != mode)
		ui->m_cbFillType->setCurrentIndex(mode);

	switch(mode)
	{
	default:
	case SolidFill:
		m_gradientEditor->hide();
		resize(230, 248);
		layout()->activate();
		break;

	case GradientFill:
        m_gradientEditor->show();
		resize(230, 288);
		layout()->activate();
        //m_gradientEditor->setGradient(m_gradient);
		break;

	case SolidOnly:
		m_gradientEditor->hide();
		ui->m_cbFillType->setEnabled(false);
		layout()->activate();
		break;

	}; // end of switch
}

int CUiColorPanel::mode()
{
	return m_mode;
}

void CUiColorPanel::paintEvent(QPaintEvent *event)
{
	QStyleOption		opt;
	QPainter			p(this);
	
	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CUiColorPanel::onColorHovered(QColor c)
{	
	QPalette		palette;

	palette.setColor( QPalette::Background, c);
	ui->m_colorIndicator->setPalette(palette);
	ui->m_colorIndicator->update();
	ui->m_colorValue->setText( c.name() );

	if (m_mode != GradientFill)
	{
		c.setAlpha(255 * (100 - m_transparency) / 100);
		emit colorHovered(c);
	}
}

void CUiColorPanel::onColorSelected(QColor c)
{
	if (m_mode == GradientFill)
	{
		c.setAlpha(255 * (100 - m_transparency) / 100);
		m_gradientEditor->setStopColor(c);
		emit gradientSelected(m_gradientEditor->gradient()); 
	}
	else
	{
		c.setAlpha(255 * (100 - m_transparency) / 100);
		m_color = c;
		emit colorSelected(c);
	}
}

void CUiColorPanel::onColorSelectionDone()
{
	onClose();
}

void CUiColorPanel::onColorValueEditing(const QString &text)
{
	QColor			c(ui->m_colorValue->text() );
	QPalette		palette;

	palette.setColor( QPalette::Background, c);
	ui->m_colorIndicator->setPalette(palette);
	ui->m_colorIndicator->update();

	if (m_mode != GradientFill)
	{
		c.setAlpha(255 * (100 - m_transparency) / 100);
		m_color = c;
		emit colorHovered( c );
	}
}

void CUiColorPanel::onColorValueChanged()
{
	QColor	c;

	c = QColor( ui->m_colorValue->text());
	if (m_mode == GradientFill)
	{
		c.setAlpha(255 * (100 - m_transparency) / 100);
		m_gradientEditor->setStopColor(c);
		emit gradientSelected(m_gradientEditor->gradient()); 
	}
	else
	{
		c.setAlpha(255 * (100 - m_transparency) / 100);
		m_color = c;
		emit colorSelected(c);
	}
}

void CUiColorPanel::onNoColor()
{
	QColor	c;
	
	c = QColor(0,0,0,0);
	m_color = c;
	emit colorSelected( c );
	close();
}

void CUiColorPanel::onFillTypeChanged(int type)
{
	setMode(type);
}

void CUiColorPanel::onTransparencyChanged(int value)
{
	QColor		c;

	ui->m_transEdit->setText( QString::number(value) );
	m_transparency = value;

	if (m_mode == GradientFill)
	{
		m_gradientEditor->setTransparency(255 * (100 - m_transparency) / 100);
		emit gradientSelected(m_gradientEditor->gradient());
	}
	
	else
	{
		c = m_color;
		c.setAlpha(255 * (100 - m_transparency) / 100);
		emit colorSelected(c);
	}
}

void CUiColorPanel::onTransparencyChanged(const QString &value)
{
	QColor		c;
	qreal		v;

	v = value.toInt();
	ui->m_transSlider->setValue( v );
	m_transparency = v;
	
	if (m_mode == GradientFill)
	{
		m_gradientEditor->setTransparency(255 * (100 - m_transparency) / 100);
		emit gradientSelected(m_gradientEditor->gradient());
	}
	else
	{
		c = m_color;
		c.setAlpha(255 * (100 - m_transparency) / 100);
		emit colorSelected(c);
	}
}

void CUiColorPanel::onMoreColor()
{
	QPalette	palette;
	QColor		c;

	m_moreColor = true;
	c = QColorDialog::getColor(Qt::white, NULL);
	m_moreColor = false;
	show();

	palette.setColor( QPalette::Background, c);
	ui->m_colorIndicator->setPalette(palette);
	ui->m_colorIndicator->update();
	ui->m_colorValue->setText( c.name() );

	if (m_mode == GradientFill)
	{
		c.setAlpha(255 * (100 - m_transparency) / 100);
		m_gradientEditor->setStopColor(c);
		emit gradientSelected(m_gradientEditor->gradient());
	}
	else
	{
		m_color = c;
		c.setAlpha(255 * (100 - m_transparency) / 100);
		emit colorSelected(c);
	}
	update();
}

void CUiColorPanel::closeEvent( QCloseEvent * event ) 
{
	if (m_mode == GradientFill && !m_moreColor)
	{
		emit gradientSelected(m_gradientEditor->gradient());
	}
	QWidget::closeEvent(event); 
}

void CUiColorPanel::setSelectedColor(const QColor &c)
{
	QPalette	palette;
	palette.setColor( QPalette::Background, c);

	ui->m_colorIndicator->setPalette(palette);
	ui->m_colorIndicator->update();
	ui->m_colorValue->setText( c.name() );

	setTransparency( ((255 - c.alpha()) / 255.0) * 100 );
	m_color = c;
}

QColor CUiColorPanel::selectedColor()
{
	return m_color;
}

void CUiColorPanel::setGradient(const QGradient &g)
{
    m_gradient = g;
	m_gradientEditor->setGradient(g);
}

QGradient CUiColorPanel::gradient()
{
	return m_gradient;
}

void CUiColorPanel::setTransparency(int value)
{
	ui->m_transEdit->setText( QString::number( value ) );
	ui->m_transSlider->setValue( value );
	m_transparency = value;
}

qreal CUiColorPanel::transparency()
{
	return m_transparency;
}

void CUiColorPanel::onClose()
{
	close();
}
