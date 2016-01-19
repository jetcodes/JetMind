/**
* Copyright (c) 2011 ProDraw. All rights reserved.
*/

#include "CUiTabBox.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)		  if ((p)) { delete (p); (p) = NULL; }
#endif //

void clearLayout(QWidget *widget, bool deleted = true)
{
	QLayout		*layout = NULL;

	if (widget)
	{
		layout = widget->layout();
		if (layout)
		{
			for (int i = 0; i < layout->count(); ++i)
			{
				layout->removeItem( layout->itemAt(i) );
			}
			delete layout;
		}
	}
}

CUiTab::CUiTab(QWidget *parent)
	: QPushButton(parent),
	  m_selected(false),
	  m_index(-1)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    setFocusPolicy(Qt::NoFocus);
	setCheckable(true);
	m_box = dynamic_cast<CUiTabBox*>(parent);
}

void CUiTab::setSelected(bool b)
{
	m_selected = b;
	setChecked(b);
	update();
}

void CUiTab::setIndex(int index)
{
	m_index = index;
}

QSize CUiTab::sizeHint() const
{
#if defined(__APPLE__) && defined(__MACH__)
    qreal delta = 8;
#else
    qreal delta = 0;
#endif

    QSize iconSize(8, 8);
    if (!icon().isNull()) 
	{
        int icone = style()->pixelMetric(QStyle::PM_SmallIconSize, 0, parentWidget());
        iconSize += QSize(icone + 2, icone);
    }
    QSize textSize = fontMetrics().size(Qt::TextShowMnemonic, text()) + QSize(0, 8);

    QSize total(iconSize.width() + textSize.width() + delta, qMax(iconSize.height(), textSize.height()));
    return total.expandedTo(QApplication::globalStrut());
}

QSize CUiTab::minimumSizeHint() const
{
    if (icon().isNull())
        return QSize();
    int icone = style()->pixelMetric(QStyle::PM_SmallIconSize, 0, parentWidget());
    return sizeHint();
}

CUiTabBox::CUiTabBox(QWidget *parent)
	: QFrame(parent),
	  m_currentItem(NULL),
	  m_layoutMode(CUiTabBox::Vertical),
	  m_ctrlLayout(NULL),
	  m_viewLayout(NULL),
	  m_splitter(NULL),
	  m_ctrlScroll(NULL),
	  m_viewScroll(NULL),
	  m_view(NULL),
	  m_ctrl(NULL),
	  m_ctrlHolder(NULL)
{
	m_view = new QWidget(this);
	m_ctrlHolder = new QWidget(this);
    m_ctrlHolder->setObjectName(tr("m_ctrlHolder"));
	m_ctrl = new QWidget(this);
    m_ctrl->setObjectName(tr("m_ctrl"));
	
	m_ctrlScroll = new QScrollArea(this);
	m_ctrlScroll->setWidgetResizable(true);
	m_ctrlScroll->setFrameStyle(QFrame::NoFrame);

	m_viewScroll = new QScrollArea(this);
	m_viewScroll->setWidgetResizable(true);
	m_viewScroll->setFrameStyle(QFrame::NoFrame);

	m_viewLayout = new QStackedLayout();
	m_viewLayout->setMargin(0);
	m_viewLayout->setSpacing(0);
	m_view->setLayout(m_viewLayout);

	m_splitter = new QSplitter(Qt::Vertical, this);
	m_splitter->addWidget(m_ctrlScroll);
	m_splitter->addWidget(m_viewScroll);

	relayout();
}

CUiTabBox::~CUiTabBox()
{
	
}

int CUiTabBox::addItem(QWidget *item, const QString &text)
{ 
	return insertItem(-1, item, QIcon(), text); 
}

int CUiTabBox::addItem(QWidget *item, const QIcon &iconSet, const QString &text)
{
	return insertItem(-1, item, iconSet, text); 
}

inline int CUiTabBox::insertItem(int index, QWidget *item, const QString &text)
{ 
	return insertItem(index, item, QIcon(), text); 
}

int CUiTabBox::insertItem(int index, QWidget *widget, const QIcon &icon, const QString &text)
{
	QWidget				*current = NULL;
	CUiTabBox::TabItem	t;
	int					oldIndex;

    if (!widget)
        return -1;

    t.widget = widget;
    t.tab = new CUiTab(this);
	connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(onWidgetDestroyed(QObject*)));
    connect(t.tab, SIGNAL(clicked()), this, SLOT(onTabClicked()));

    t.tab->setText(text);
    t.tab->setIcon(icon);

    if (index < 0 || index >= (int)m_items.count()) 
	{
        index = m_items.count();
        m_items.append(t);
		m_ctrlLayout->addWidget(t.tab);
        m_viewLayout->addWidget(t.widget);
        if (index == 0)
            setCurrentIndex(index);
    } 
	else
	{
        m_items.insert(index, t);
        reset();
        if (m_currentItem)
		{
            current = m_currentItem->widget;
            oldIndex = indexOf(current);
            if (index <= oldIndex) 
			{
                m_currentItem = 0;
                setCurrentIndex(oldIndex);
            }
        }
    }

    t.tab->show();

    updateTabs();
    return index;
}

int CUiTabBox::count() const
{
	return m_items.count();
}

void CUiTabBox::removeItem(int index)
{
	CUiTabBox::TabItem	*c = NULL;
	QWidget				*w = NULL;

	c = item(index);
	w = widget(index);
	if (w) 
	{
        disconnect(w, SIGNAL(destroyed(QObject*)), this, SLOT(onWidgetDestroyed(QObject*)));
        w->setParent(this);
        // TODO: destroy internal data
		if (index == currentIndex())
			m_currentItem = NULL;

		m_viewLayout->removeWidget(w);
		m_ctrlLayout->removeWidget(c->tab);
		delete c->tab;

		m_items.removeAll(*c);
		if (!m_items.isEmpty())
		{
			setCurrentIndex(0);
		}
    }
	updateTabs();
}

void CUiTabBox::setCurrentIndex(int index)
{
    CUiTabBox::TabItem	*t = NULL;

	if (index < 0 || index >= m_items.length())
		return;

	t = item(index);
    if (!t || m_currentItem == t)
	{
		if (m_currentItem)
			m_currentItem->tab->setSelected(true);
        return;
	}

	if (m_currentItem) 
	{
        m_currentItem->tab->setSelected(false);
    }
    t->tab->setSelected(true);
    m_currentItem = t;
    m_viewLayout->setCurrentWidget ( t->widget );
    updateTabs();
    emit currentChanged(index);
}

void CUiTabBox::setCurrentWidget(QWidget *widget)
{
	int	 index = -1;

	if (!widget)
		return;

	index = indexOf(widget);
	if (index >= 0 && index < m_items.length())
	{
		setCurrentIndex(index);
	}
}

void CUiTabBox::onWidgetDestroyed(QObject *object)
{
    QWidget				*p = NULL;
	CUiTabBox::TabItem	*c = NULL;
	bool				removeCurrent = false;

	p = (QWidget*)object;
	c = item(p);

    if (!p || !c)
        return;

	m_viewLayout->removeWidget(c->widget);
    m_ctrlLayout->removeWidget(c->tab);
    delete c->tab;

    removeCurrent = (c == m_currentItem);
    m_items.removeAll(*c);

    if (!m_items.count()) 
	{
        m_currentItem = 0;
        emit currentChanged(-1);
    } 
	else if (removeCurrent) 
	{
        m_currentItem = 0;
        setCurrentIndex(0);
    }
}

void CUiTabBox::onTabClicked()
{
	CUiTab		*t = NULL;
    QWidget*	item = 0;

	t = qobject_cast<CUiTab*>( sender() );
    for (int i = 0; i < m_items.length(); ++i)
	{
        if ( m_items[i].tab == t) 
		{
            item = m_items[i].widget;
            break;
        }
	}
    setCurrentIndex(indexOf(item));
}

int CUiTabBox::currentIndex() const
{
	return m_currentItem ? indexOf(m_currentItem->widget) : -1;
}

QWidget* CUiTabBox::currentWidget() const 
{
	return m_currentItem ? m_currentItem->widget : 0; 
}

QWidget* CUiTabBox::widget(int index) const
{
	if (index < 0 || index >= m_items.count())
		return 0;

	return m_items[index].widget;
}

CUiTabBox::TabItem* CUiTabBox::item(int index)
{
	if (index < 0 || index >= m_items.length())
		return 0;

	return &(m_items[index]);
}

CUiTabBox::TabItem* CUiTabBox::item(QWidget *widget)
{
	TabItem		*c = NULL;

	for (int i = 0; i < m_items.length(); ++i)
	{
		if (m_items[i].widget == widget)
		{
			c = &m_items[i];
			break;
		}
	}
	return c;
}

const CUiTabBox::TabItem* CUiTabBox::item(int index) const
{
	if (index < 0 || index >= m_items.length())
		return 0;

    return &m_items.at(index);
}

const CUiTabBox::TabItem* CUiTabBox::item(QWidget *widget) const
{
	if (!widget)
		return 0;

	for (QList<CUiTabBox::TabItem>::ConstIterator i = m_items.constBegin(); i != m_items.constEnd(); ++i)
        if ((*i).widget == widget)
            return (TabItem*) &(*i);
    return 0;
}

int CUiTabBox::indexOf(QWidget *widget) const
{
	int		index = -1;

	for (int i = 0; i < m_items.length(); ++i)
	{
		if (m_items[i].widget == widget)
		{
			index = i;
			break;
		}
	}
	return index;
}

void CUiTabBox::changeEvent(QEvent *e)
{
	if (e->type() == QEvent::StyleChange)
		updateTabs();
	QFrame::changeEvent(e);
}

void CUiTabBox::setItemText(int index, const QString &text)
{
	TabItem		*c = NULL;

	c = item(index);
	if (!c)
		return;

	c->tab->setText(text);
}

QString CUiTabBox::itemText(int index) const
{
	TabItem		*c = NULL;

	c = const_cast<TabItem*>(item(index));
	if (!c)
		return QString("");

	return c->tab->text();
}

void CUiTabBox::setItemIcon(int index, const QIcon &icon)
{
	TabItem		*c = NULL;

	c = const_cast<TabItem*>(item(index));
	if (!c)
		return;

	c->tab->setIcon(icon);
}

QIcon CUiTabBox::itemIcon(int index) const
{
	TabItem		*c = NULL;

	c = const_cast<TabItem*>(item(index));
	if (!c)
		return QIcon();

	return c->tab->icon();
}

void CUiTabBox::paintEvent(QPaintEvent *event)
{
	QStyleOption		opt;
	QPainter			p(this);
	
	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CUiTabBox::setLayoutMode(int mode)
{
	m_layoutMode = mode;
	relayout();
}

void CUiTabBox::relayout()
{
	QBoxLayout		*layout = NULL;
	QBoxLayout		*ctrlHolderLayout = NULL;

	clearLayout(m_ctrl);
	clearLayout(this);
	
	ctrlHolderLayout = dynamic_cast<QBoxLayout*>( m_ctrlHolder->layout() );
	if (ctrlHolderLayout)
	{
		ctrlHolderLayout->removeWidget(m_ctrl);
		SAFE_DELETE(ctrlHolderLayout);
	}
	
	for (int i = 0; i < m_items.length(); ++i)
	{
		m_items.at(i).tab->setParent(NULL);
	}
	SAFE_DELETE(m_ctrl);

	if (m_layoutMode == CUiTabBox::Horizontal)
	{
		// m_ctrl just need to be create here, or it won't
		// show up, it is kind of tricky
		m_ctrl = new QWidget(this);
        m_ctrl->setObjectName(tr("m_ctrl"));

		m_ctrlLayout = new QHBoxLayout(m_ctrl);
        m_ctrlLayout->setContentsMargins(0, 5, 0, 0);
        m_ctrlLayout->setSpacing(4);

		ctrlHolderLayout = new QHBoxLayout(m_ctrlHolder);
		ctrlHolderLayout->setContentsMargins(0, 0, 0, 0);
		ctrlHolderLayout->setSpacing(0);
		ctrlHolderLayout->addWidget(new QWidget(m_ctrlHolder), 1);
		ctrlHolderLayout->addWidget(m_ctrl, 0);
		ctrlHolderLayout->addWidget(new QWidget(m_ctrlHolder), 1);

		// take widget out of the scroll area
		m_ctrlScroll->takeWidget();
		m_viewScroll->takeWidget();

		m_splitter->hide();
		if (m_ctrlHolder->isHidden())
			m_ctrlHolder->show();

		layout = new QVBoxLayout(this);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(3);
		layout->addWidget(m_ctrlHolder);
		layout->addWidget(m_view);
	}
	else
	{
		m_ctrl = new QWidget(this);

		m_ctrlLayout = new QVBoxLayout(this);
		m_ctrlLayout->setContentsMargins(0, 0, 0, 0);
		m_ctrlLayout->setSpacing(0);
		m_ctrlLayout->setSizeConstraint(QLayout::SetMinimumSize);
		m_ctrl->setLayout(m_ctrlLayout);

		// scroll area assigned.
		m_ctrlScroll->setWidget(m_ctrl);
		m_viewScroll->setWidget(m_view);

		m_viewScroll->setWidgetResizable(true);
		m_viewScroll->setFrameStyle(QFrame::NoFrame);

		if (m_splitter->isHidden())
			m_splitter->show();
		m_ctrlHolder->hide();

		layout = new QVBoxLayout(this);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->addWidget(m_splitter);
	}

	setBackgroundRole(QPalette::Button);

	reset();
	setCurrentIndex(0);
}

void CUiTabBox::reset()
{
	for (int i = 0; i < m_ctrlLayout->count(); ++i)
		m_ctrlLayout->removeItem(m_ctrlLayout->itemAt(i));
	for (int i = 0; i < m_viewLayout->count(); ++i)
		m_viewLayout->removeItem(m_viewLayout->itemAt(i));

	for (int i = 0;i < m_items.length(); ++i)
	{
		m_ctrlLayout->addWidget(m_items[i].tab);
		m_viewLayout->addWidget(m_items[i].widget);
	}
}

void CUiTabBox::updateTabs()
{
	CUiTab	*t = NULL;
    int		index = 0;
	qreal	height = 0;

    for (index = 0; index < m_items.count(); ++index) 
	{
        const TabItem &item = m_items.at(index);
        t = item.tab;

		height += t->sizeHint().height();
        // update indexes, since the updates are delayed, the indexes will be correct
        // when we actually paint.
        t->setIndex(index);
    }

	if (m_layoutMode == Vertical && m_ctrlScroll)
		m_ctrlScroll->setMaximumHeight( height );
}
