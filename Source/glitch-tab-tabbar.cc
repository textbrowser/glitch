/*
** Copyright (c) 2015 - 10^10^10, Alexis Megas.
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
**    derived from Glitch without specific prior written permission.
**
** GLITCH IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** GLITCH, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QDrag>
#include <QMenu>
#include <QMimeData>
#include <QMouseEvent>
#include <QToolButton>

#include "glitch-tab-tabbar.h"
#include "glitch-view.h"

glitch_tab_tabbar::glitch_tab_tabbar(QWidget *parent):QTabBar(parent)
{
  m_disableSeparation = false;
  setContextMenuPolicy(Qt::CustomContextMenu);
  setDocumentMode(true);
  setElideMode(Qt::ElideRight);
  setExpanding(true);
  setStyleSheet("QTabBar::tear {border: none; image: none; width: 0px;}");
  setUsesScrollButtons(true);
  connect(this,
	  SIGNAL(customContextMenuRequested(const QPoint &)),
	  this,
	  SLOT(slotCustomContextMenuRequested(const QPoint &)));

  foreach(auto toolButton, findChildren <QToolButton *> ())
    if(toolButton)
      toolButton->setStyleSheet
	(QString("QToolButton {background-color: %1;"
		 "border: none;"
		 "margin-bottom: 0px;"
		 "margin-top: 0px;"
		 "}"
		 "QToolButton::menu-button {border: none;}").
	 arg(QWidget::palette().color(QWidget::backgroundRole()).name()));
}

glitch_tab_tabbar::~glitch_tab_tabbar()
{
}

QSize glitch_tab_tabbar::tabSizeHint(int index) const
{
  auto const tabPosition = qobject_cast<glitch_tab *> (parentWidget()) ?
    qobject_cast<glitch_tab *> (parentWidget())->tabPosition() :
    QTabWidget::North;
  auto size(QTabBar::tabSizeHint(index));

  if(tabPosition == QTabWidget::East || tabPosition == QTabWidget::West)
    {
      auto preferredTabHeight = 175;

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
#ifdef Q_OS_MACOS
      auto preferred = 250;
#else
      auto preferred = 250;
#endif
      auto preferredTabWidth = 0;

      if(parentWidget() &&
	 count() * rect().width() < parentWidget()->size().width())
	preferredTabWidth = preferred;
      else
	preferredTabWidth = qBound
	  (125,
	   qMax(size.width(), rect().width() / qMax(1, count())),
	   preferred);

      size.setHeight(5 + size.height());
      size.setWidth(preferredTabWidth);
    }

  return size;
}

QTabBar::ButtonPosition glitch_tab_tabbar::
preferredCloseButtonPositionOpposite(void) const
{
#ifdef Q_OS_MACOS
  if(!style())
    return QTabBar::RightSide;
#else
  if(!style())
    return QTabBar::RightSide;
#endif

  auto const buttonPosition = static_cast<QTabBar::ButtonPosition>
    (style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, this));

#ifdef Q_OS_MACOS
  return buttonPosition == QTabBar::LeftSide ?
    QTabBar::RightSide : QTabBar::LeftSide;
#else
  return buttonPosition == QTabBar::LeftSide ?
    QTabBar::RightSide : QTabBar::LeftSide;
#endif
}

void glitch_tab_tabbar::disableSeparation(void)
{
  m_disableSeparation = true;
}

void glitch_tab_tabbar::mouseMoveEvent(QMouseEvent *event)
{
  if(m_disableSeparation)
    {
      QTabBar::mouseMoveEvent(event);
      return;
    }

#ifdef Q_OS_ANDROID
  QTabBar::mouseMoveEvent(event);
  return;
#endif

  if(!(QGuiApplication::keyboardModifiers() & Qt::ControlModifier) ||
     !(event) ||
     !(event->buttons() & Qt::LeftButton))
    {
      QTabBar::mouseMoveEvent(event);
      return;
    }

  QImage image;

  if(qobject_cast<QTabWidget *> (parentWidget()))
    {
      auto widget = qobject_cast<glitch_view *>
	(qobject_cast<QTabWidget *> (parentWidget())->widget(currentIndex()));

      if(widget)
	image = widget->snap().scaled
	  (200, 250, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
      else
	{
	  QTabBar::mouseMoveEvent(event);
	  return;
	}
    }
  else
    {
      QTabBar::mouseMoveEvent(event);
      return;
    }

  QPixmap pixmap(1, 1);
  auto const index = currentIndex();
  auto drag = new QDrag(this);
  auto widget = qobject_cast<QTabWidget *> (parentWidget())->widget(index);

  pixmap.fill();
  drag->setDragCursor(pixmap, Qt::IgnoreAction);
  drag->setHotSpot(QPoint(25, 25));
  drag->setMimeData(new QMimeData());
  drag->setPixmap(QPixmap::fromImage(image));
  emit separate(widget); // Create an invisible separated window.
  drag->exec();
  emit separate(nullptr); // Show the separated window.
}

void glitch_tab_tabbar::slotCustomContextMenuRequested(const QPoint &point)
{
  auto tab = qobject_cast<glitch_tab *> (parentWidget());

  if(!tab)
    return;

  auto view = qobject_cast<glitch_view *> (tab->widget(tabAt(point)));

  if(!view)
    return;

  auto menu = view->defaultContextMenu();

  if(!menu)
    return;

  menu->exec(mapToGlobal(point));
}
