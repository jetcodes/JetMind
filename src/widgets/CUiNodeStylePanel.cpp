/**
* Copyright (c) 2012 ProDraw. All rights reserved.
*/
#include <core/CDiagramItem>
#include <core/CDiagramNode>

#include "CUiNodeStylePicker.h"
#include "CUiNodeStylePanel.h"
#include "ui_CUiNodeStylePanel.h"

CUiNodeStylePanel::CUiNodeStylePanel(QWidget *parent)
	: QMenu(parent),
	  m_systemGrid(NULL),
	  m_recentGrid(NULL),
	  m_styleReset(false),
	  m_cols(7)
{
	QHBoxLayout		*sysLayout = NULL;
	QHBoxLayout		*recLayout = NULL;

	//setMouseTracking(true);
    //setFrameShape(QFrame::StyledPanel);
    //setFrameShadow(QFrame::Plain);

	ui = new Ui::CUiNodeStylePanel();
	ui->setupUi(this);
	
	setupSystemStyles();

	m_systemGrid = new CUiNodeStylePicker(this);
	m_systemGrid->setSelfClose(false);
	sysLayout = new QHBoxLayout(NULL);
	sysLayout->setContentsMargins(0, 0, 0, 0);
	sysLayout->setSpacing(0);
	sysLayout->addWidget(m_systemGrid);

	recLayout = new QHBoxLayout(NULL);
	recLayout->setContentsMargins(0, 0, 0, 0);
	recLayout->setSpacing(0);

	m_recentGrid = new CUiNodeStylePicker(this);
	m_recentGrid->setRows(1);
	m_recentGrid->setStyles(QList<CDiagramNodeStyle>());
	m_recentGrid->setSelfClose(false);
	recLayout->addWidget(m_recentGrid);

	ui->m_systemGridHolder->setLayout(sysLayout);
	ui->m_recentGridHolder->setLayout(recLayout);

	// For the current version, we only need a simple version of 
	// node style grid
	ui->m_btnCustom->setVisible(false);
	ui->m_recentGridHolder->setVisible(false);
	ui->m_historyInfo->setVisible(false);
	// ui->m_styleFilterHolder->setVisible(false);

	ui->m_styleCatetorySelector->addItem(tr("Flat"), QVariant(CUiNodeStylePanel::Flat));
	ui->m_styleCatetorySelector->addItem(tr("Classic"), QVariant(CUiNodeStylePanel::Classic));

	connect(m_systemGrid, SIGNAL(styleHovered(const CDiagramNodeStyle&)), this, SLOT(onStyleHovered(const CDiagramNodeStyle&)));
	connect(m_systemGrid, SIGNAL(styleSelected(const CDiagramNodeStyle&)), this, SLOT(onStyleSelected(const CDiagramNodeStyle&)));
	connect(m_recentGrid, SIGNAL(styleHovered(const CDiagramNodeStyle&)), this, SLOT(onStyleHovered(const CDiagramNodeStyle&)));
	connect(m_recentGrid, SIGNAL(styleSelected(const CDiagramNodeStyle&)), this, SLOT(onRecentStyleSelected(const CDiagramNodeStyle&)));
	connect(ui->m_styleCatetorySelector, SIGNAL(activated(int)), this, SLOT(onStyleCategoryChanged(int)));

	if (m_systemGrid->styles().length() > 0)
		m_defaultStyle = m_systemGrid->styles().first();
}

CUiNodeStylePanel::~CUiNodeStylePanel()
{
	delete ui;
}

void CUiNodeStylePanel::paintEvent( QPaintEvent *event )
{
	QStyleOption		opt;
	QPainter			p(this);
	
	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CUiNodeStylePanel::hideEvent( QHideEvent *event )
{
	QMenu::hideEvent(event);
	if (m_styleReset)
		emit styleReset();
}

void CUiNodeStylePanel::closeEvent( QCloseEvent *event )
{
	QMenu::closeEvent(event);
	if (m_styleReset)
		emit styleReset();
}

void CUiNodeStylePanel::showEvent( QShowEvent * event )
{
	layout()->invalidate();
	resize(layout()->sizeHint());

	QMenu::showEvent(event);
}

void CUiNodeStylePanel::addRecentStyle(const CDiagramNodeStyle &style)
{
	if (!m_recentStyles.contains(style))
	{
		m_recentStyles.push_front(style);

		if (m_recentStyles.length() > m_cols)
			m_recentStyles.removeLast();
	
		if (m_recentGrid)
			m_recentGrid->setStyles(m_recentStyles);
		update();
	}
}

void CUiNodeStylePanel::setRecentStyles(const QList<CDiagramNodeStyle> &styles)
{
	m_recentStyles = styles;
	if (m_recentGrid)
		m_recentGrid->setStyles(styles);
	update();
}

QList<CDiagramNodeStyle> CUiNodeStylePanel::recentStyles()
{
	return m_recentStyles;
}

const CDiagramNodeStyle CUiNodeStylePanel::defaultStyle() const
{
	return m_defaultStyle;
}

void CUiNodeStylePanel::setColumns(int num)
{
	if (num > 0)
		m_cols = num;
}

int CUiNodeStylePanel::columns()
{
	return m_cols;
}

void CUiNodeStylePanel::onStyleHovered(const CDiagramNodeStyle &s)
{
	emit styleHovered(s);
	m_styleReset = true;
}

void CUiNodeStylePanel::onStyleSelected(const CDiagramNodeStyle &s)
{
	emit styleSelected(s);
	addRecentStyle(s);
	m_styleReset = false;
	m_defaultStyle = s;
	close();
}

void CUiNodeStylePanel::onRecentStyleSelected(const CDiagramNodeStyle &s)
{
	emit styleSelected(s);
	m_styleReset = false;
	m_defaultStyle = s;
	close();
}

void CUiNodeStylePanel::onStyleCategoryChanged(int value)
{
	int							type;
	QList<CDiagramNodeStyle>	styles;

	type = ui->m_styleCatetorySelector->itemData( value ).toInt();
	if (m_systemStyles.contains(type))
	{
		styles = m_systemStyles[type];
		m_systemGrid->setStyles( styles );
	}
}

void CUiNodeStylePanel::setPenWidth(qreal value)
{
	if (m_systemGrid)
		m_systemGrid->setPenWidth(value);
}

qreal CUiNodeStylePanel::penWidth()
{
	qreal	r = 1.0;

	if (m_systemGrid)
		r = m_systemGrid->penWidth();

	return r;
}

void CUiNodeStylePanel::setupSystemStyles()
{
	int							alpha = 192;
	qreal						pw = penWidth();
	QList<QColor>				colors;
	QList<Qt::PenStyle>			penStyles;
	QColor						c, basicColor(255,255,255,alpha);
	QList<CDiagramNodeStyle>	styles;

	// classic
	colors << QColor("#ff00ff") << QColor("#4E80BC") << QColor("#cc00cc") << QColor("#9ABA58") \
		   << QColor("#7F63A1") << QColor("#4AABC5") << QColor("#F69545") << QColor("#c0c0c0");
	penStyles << Qt::DashLine << Qt::DotLine << Qt::SolidLine;

	foreach (Qt::PenStyle ps, penStyles)
	{
		foreach (c, colors)
		{
			CDiagramNodeStyle	s(QPen(c, pw, ps), QBrush(Qt::white));
			styles.push_back(s);
		}
	}
	
	// colors
	// first line
	foreach (c, colors)
	{
		CDiagramNodeStyle	 s(QPen(c, pw, Qt::SolidLine), QBrush(c));
		styles.push_back(s);
	}

	// second line
	colors.clear();
	colors  << QColor("#FF99CC") << QColor("#993399") << QColor("#FF6666") << QColor("#339999")
			<< QColor("#CCFF00") << QColor("#66CC99") << QColor("#FFCC99") << QColor("#CC3399");
	foreach (c, colors)
	{
		CDiagramNodeStyle		s(  QPen(c, pw, Qt::SolidLine), QBrush(c) );
		styles.push_back(s);
	}
	
	// third line
	colors.clear();
	colors  << QColor("#CCCCFF") << QColor("#FFCCCC") << QColor("#99CCCC") << QColor("#99CC99")
			<< QColor("#CCFF99") << QColor("#FF9966") << QColor("#99CC99") << QColor("#FFFFCC");
	foreach (c, colors)
	{
		CDiagramNodeStyle		s(QPen(c, pw, Qt::SolidLine), QBrush(c));
		styles.push_back(s);
	}

	m_systemStyles.insert(CUiNodeStylePanel::Flat, styles);

	///////////////////////////////////////////////////////////////
	styles.clear();
	colors.clear();
	penStyles.clear();

	colors  << QColor("#ff00ff") << QColor("#4E80BC") << QColor("#cc00cc") << QColor("#9ABA58") \
			<< QColor("#7F63A1") << QColor("#4AABC5") << QColor("#F69545") << QColor("#c0c0c0");
	penStyles << Qt::DashLine << Qt::DotLine << Qt::SolidLine;

	foreach (Qt::PenStyle ps, penStyles)
	{
		foreach (c, colors)
		{
			CDiagramNodeStyle	s(QPen(c, pw, ps), QBrush(Qt::white));
			styles.push_back(s);
		}
	}
	
	foreach (c, colors)
	{
		QLinearGradient			g(0.5,0,0.5,1);
		c.setAlpha(alpha);
		g.setCoordinateMode(QGradient::ObjectBoundingMode);
		g.setColorAt(0.0, basicColor);
		g.setColorAt(1.0, c);

		CDiagramNodeStyle	 s(QPen(c, pw, Qt::SolidLine), QBrush(g));
		styles.push_back(s);
	}

	foreach (c, colors)
	{
		QLinearGradient			g(0.5,0,0.5,1);
		c.setAlpha(alpha);
		g.setCoordinateMode(QGradient::ObjectBoundingMode);
		g.setColorAt(0.0, basicColor);
		g.setColorAt(0.5, c);
		g.setColorAt(1.0, basicColor);

		CDiagramNodeStyle		s(  QPen(c, pw, Qt::SolidLine), QBrush( g) );
		styles.push_back(s);
	}

	foreach (c, colors)
	{
		QLinearGradient			g(0,0,1,1);
		c.setAlpha(alpha);
		g.setCoordinateMode(QGradient::ObjectBoundingMode);
		g.setColorAt(0.0, basicColor);
		g.setColorAt(1.0, c);

		CDiagramNodeStyle		s(QPen(c, pw, Qt::SolidLine), QBrush(g));
		styles.push_back(s);
	}

	m_systemStyles.insert(CUiNodeStylePanel::Classic, styles);
}
