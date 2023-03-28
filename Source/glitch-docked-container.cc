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

#include <QShortcut>
#include <QStandardItemModel>

#include "glitch-docked-container.h"
#include "glitch-floating-context-menu.h"

glitch_docked_container::glitch_docked_container(QWidget *parent):
  QWidget(parent)
{
  m_model = new QStandardItemModel(this);
  m_model->setHorizontalHeaderLabels
    (QStringList() << tr("Widget Property Editors"));
  m_ui.setupUi(this);
  m_ui.view->setModel(m_model);
  connect(&m_timer,
	  &QTimer::timeout,
	  this,
	  &glitch_docked_container::slotTimerTimeout);
}

glitch_docked_container::~glitch_docked_container()
{
  m_timer.stop();
}

void glitch_docked_container::add(QWidget *widget)
{
  if(!widget)
    return;

  QStandardItem *item = nullptr;

  for(int i = 0; i < m_model->rowCount(); i++)
    if(m_model->item(i, 0))
      {
	auto index(m_model->item(i, 0)->index());

	if(index.isValid() && m_ui.view->indexWidget(index) == widget)
	  {
	    item = m_model->item(i, 0);
	    break;
	  }
      }

  if(item)
    {
      m_ui.view->scrollTo(item->index(), QAbstractItemView::PositionAtTop);
      m_ui.view->selectRow(item->row());
    }
  else
    {
      auto item = new QStandardItem();

      foreach(auto shortcut, widget->findChildren<QShortcut *> ())
	if(shortcut)
	  shortcut->setEnabled(false);

      m_model->appendRow(item);
      m_model->setVerticalHeaderItem
	(item->row(), new QStandardItem(QString(5, ' ')));
      m_ui.view->scrollTo(item->index(), QAbstractItemView::PositionAtTop);
      m_ui.view->selectRow(item->row());
      m_ui.view->setIndexWidget(item->index(), widget);
    }

  if(!m_timer.isActive())
    m_timer.start(1500);
}

void glitch_docked_container::detach(void)
{
  for(int i = m_model->rowCount() - 1; i >= 0; i--)
    {
      if(m_model->item(i, 0))
	{
	  auto index(m_model->item(i, 0)->index());

	  if(index.isValid())
	    {
	      auto widget = m_ui.view->indexWidget(index);

	      if(widget)
		{
		  foreach(auto shortcut, widget->findChildren<QShortcut *> ())
		    if(shortcut)
		      shortcut->setEnabled(true);

		  auto menu = qobject_cast<glitch_floating_context_menu *>
		    (widget);

		  if(menu && menu->object())
		    {
		      auto event = new QMouseEvent
			(QEvent::MouseButtonDblClick,
			 QPoint(),
			 Qt::NoButton,
			 Qt::NoButton,
			 Qt::NoModifier);

		      QApplication::postEvent(menu->object(), event);
		    }
		}
	    }
	}

      m_model->removeRow(i);
    }

  m_timer.stop();
}

void glitch_docked_container::slotTimerTimeout(void)
{
  for(int i = m_model->rowCount() - 1; i >= 0; i--)
    if(m_model->item(i, 0))
      {
	auto index(m_model->item(i, 0)->index());

	if(index.isValid())
	  {
	    auto widget = m_ui.view->indexWidget(index);

	    if(widget && widget->isVisible() == false)
	      m_model->removeRow(i);
	  }
      }

  if(m_model->rowCount() == 0)
    m_timer.stop();
}
