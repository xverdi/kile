/**************************************************************************************
    Copyright (C) 2004 by Jeroen Wijnhout (Jeroen.Wijnhout@kdemail.net)
              (C) 2006 by Thomas Braun (braun@physik.fu-berlin.de)
              (C) 2006, 2007 by Michel Ludwig (michel.ludwig@kdemail.net)
 **************************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kilesidebar.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLayout>

#include "kiledebug.h"

KileSideBar::KileSideBar(QWidget *parent, Qt::Orientation orientation /*= Vertical*/) :
	QWidget(parent),
	m_orientation(orientation),
	m_minimized(true),
	m_directionalSize(0)
{
	QBoxLayout *layout = NULL;
	KMultiTabBar::KMultiTabBarPosition tabbarpos = KMultiTabBar::Top;

	if (orientation == Qt::Horizontal) {
		layout = new QVBoxLayout(this);
		tabbarpos = KMultiTabBar::Top;
	}
	else if(orientation == Qt::Vertical) {
		layout = new QHBoxLayout(this);
		tabbarpos = KMultiTabBar::Left;
	}

	m_tabStack = new QStackedWidget(this);
	m_tabStack->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	m_tabStack->setVisible(false);

	m_tabBar = new KMultiTabBar(tabbarpos, this);
	m_tabBar->setStyle(KMultiTabBar::KDEV3ICON);

	if(orientation == Qt::Horizontal) {
		layout->add(m_tabBar);
		layout->add(m_tabStack);
		m_tabBar->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	}
	else if(orientation == Qt::Vertical) {
		layout->add(m_tabStack);
		layout->add(m_tabBar);
		m_tabBar->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
	}

	layout->setMargin(0);
	layout->setSpacing(0);

	setLayout(layout);
}

KileSideBar::~KileSideBar()
{
}

int KileSideBar::addPage(QWidget *widget, const QPixmap &pic, const QString &text /* = QString::null*/)
{
	int index = m_tabStack->addWidget(widget);
	m_tabBar->appendTab(pic, index, text);
	connect(m_tabBar->tab(index), SIGNAL(clicked(int)), this, SLOT(tabClicked(int)));

	switchToTab(index);

	return index;
}

void KileSideBar::removePage(QWidget *w) 
{
	int nTabs = m_tabStack->count();
	int index = m_tabStack->indexOf(w);
	int currentIndex = currentTab();
	m_tabStack->removeWidget(w);
	disconnect(m_tabBar->tab(index), SIGNAL(clicked(int)), this, SLOT(showTab(int)));
	m_tabBar->removeTab(index);
	if(index == currentIndex && nTabs >= 2) {
		switchToTab(findNextShownTab(index));
	}
}

QWidget* KileSideBar::currentPage()
{
	if(isMinimized()) {
		return NULL;
	}

	return m_tabStack->currentWidget();
}

int KileSideBar::currentTab()
{
	if(m_minimized) {
		return -1;
	}

	return m_tabStack->currentIndex();
}

bool KileSideBar::isMinimized()
{
	return m_minimized;
}

int KileSideBar::count()
{
	return m_tabStack->count();
}

void KileSideBar::shrink()
{
	if(isMinimized()) {
		return;
	}

	m_tabStack->setVisible(false);
	m_minimized = true;

	if(m_orientation == Qt::Horizontal) {
		m_directionalSize = height();
		setFixedHeight(m_tabBar->sizeHint().height());
	}
	else if(m_orientation == Qt::Vertical) {
		m_directionalSize = width();
		setFixedWidth(m_tabBar->sizeHint().width());
	}

	// deselect the currect tab
	int currentIndex = currentTab();
	m_tabBar->setTab(currentIndex, false);

	emit visibilityChanged(false);
}

void KileSideBar::expand()
{
	if(!isMinimized()) {
		return;
	}

	if(m_orientation == Qt::Horizontal) {
		setMinimumHeight(0);
		setMaximumHeight(QWIDGETSIZE_MAX);
	}
	else if(m_orientation == Qt::Vertical) {
		setMinimumWidth(0);
		setMaximumWidth(QWIDGETSIZE_MAX);
	}

	m_tabStack->setVisible(true);
	m_minimized = false;

	emit visibilityChanged(true);
}

void KileSideBar::tabClicked(int i)
{
	int currentIndex = currentTab();
	
	if(i == currentIndex && !isMinimized()) {
		shrink();
	}
	else {
		switchToTab(i);
	}
}

int KileSideBar::findNextShownTab(int i)
{
	int nTabs = m_tabStack->count();
	if(nTabs <= 0) {
		return -1;
	}
	for(int j = 1; j < nTabs; ++j) {
		int index = (i + j) % nTabs;

		if(m_tabBar->tab(index)->isShown()) {
			return index;
		}
	}
	return -1;
}

void KileSideBar::setPageVisible(QWidget *w, bool b)
{
	int nTabs = m_tabStack->count();
	int index = m_tabStack->indexOf(w);
	int currentIndex = currentTab();

	KMultiTabBarTab *tab = m_tabBar->tab(index);
	if(tab->isShown() == b) {
		return;
	}
	tab->setShown(b);
	if(!b && index == currentIndex && nTabs >= 2) {
		switchToTab(findNextShownTab(index));
	}
}

void KileSideBar::showPage(QWidget *widget)
{
	int i = m_tabStack->indexOf(widget);
	if(i >= 0) {
		switchToTab(i);
	}
}

int KileSideBar::directionalSize()
{
	if(m_minimized) {
		return m_directionalSize;
	}

	if(m_orientation == Qt::Horizontal) {
		return m_tabStack->height();
	}
	else if(m_orientation == Qt::Vertical) {
		return m_tabStack->width();
	}

	return 0;
}

void KileSideBar::setDirectionalSize(int i)
{
	if(m_orientation == Qt::Horizontal) {
		m_tabStack->resize(m_tabStack->width(), i);
	}
	else if(m_orientation == Qt::Vertical) {
		m_tabStack->resize(i, m_tabStack->height());
	}
}

void KileSideBar::switchToTab(int id)
{
	int nTabs = m_tabStack->count();
	int currentIndex = currentTab();

	if(id >= nTabs || id < 0 || !m_tabBar->tab(id)->isShown()) {
		shrink();
		return;
	}
	// currentIndex == id is allowed if we are expanding, for example
	if(currentIndex >= 0) {
		m_tabBar->setTab(currentIndex, false);
	}
	m_tabBar->setTab(id, true);

	m_tabStack->setCurrentIndex(id);
	expand();
}

KileBottomBar::KileBottomBar(QWidget *parent) : KileSideBar(parent, Qt::Horizontal)
{
}

#include "kilesidebar.moc"
