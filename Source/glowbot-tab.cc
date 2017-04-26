/*
** Copyright (c) 2004 - 10^10^10, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from GlowBot without specific prior written permission.
**
** GLOWBOT IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** GLOWBOT, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "glowbot-tab.h"

glowbot_tab_tabbar::glowbot_tab_tabbar(QWidget *parent):QTabBar(parent)
{
  setDocumentMode(true);
  setElideMode(Qt::ElideRight);
  setExpanding(true);
  setUsesScrollButtons(true);
}

glowbot_tab_tabbar::~glowbot_tab_tabbar()
{
}

QSize glowbot_tab_tabbar::tabSizeHint(int index) const
{
  QSize size(QTabBar::tabSizeHint(index));
  QTabWidget::TabPosition tabPosition =
    qobject_cast<glowbot_tab *> (parentWidget()) ?
    qobject_cast<glowbot_tab *> (parentWidget())->tabPosition() :
    QTabWidget::North;

  if(tabPosition == QTabWidget::East || tabPosition == QTabWidget::West)
    {
      int preferredTabHeight = 175;

      if(parentWidget() &&
	 count() * rect().height() < parentWidget()->size().height())
	preferredTabHeight = 175;
      else
	preferredTabHeight = qBound
	  (125,
	   qMax(size.height(), rect().height() / qMax(1, count())),
	   175);

      size.setHeight(preferredTabHeight);
    }
  else
    {
#ifdef Q_OS_MAC
      int preferred = 200;
#else
      int preferred = 200;
#endif
      int preferredTabWidth = preferred;

      if(parentWidget() &&
	 count() * rect().width() < parentWidget()->size().width())
	preferredTabWidth = preferred;
      else
	preferredTabWidth = qBound
	  (125,
	   qMax(size.width(), rect().width() / qMax(1, count())),
	   preferred);

      size.setWidth(preferredTabWidth);
    }

  return size;
}

glowbot_tab::glowbot_tab(QWidget *parent):QTabWidget(parent)
{
  m_tabBar = new glowbot_tab_tabbar(this);
  m_tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
  m_tabBar->setStyleSheet
    ("QTabBar::tear {"
     "image: none;"
     "}"
     );
  setStyleSheet("QTabWidget::tab-bar {"
		"alignment: left;"
		"}");
  setTabBar(m_tabBar);
}

glowbot_tab::~glowbot_tab()
{
}

QTabBar *glowbot_tab::tabBar(void) const
{
  return m_tabBar;
}
