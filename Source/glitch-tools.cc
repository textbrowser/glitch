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

#include <QActionGroup>
#include <QMenu>

#include "glitch-tools.h"

glitch_tools::glitch_tools(QWidget *parent):QDialog(parent)
{
  m_ui.setupUi(this);
#ifdef Q_OS_ANDROID
  connect(m_ui.close,
	  &QPushButton::clicked,
	  this,
	  &glitch_tools::hide);
#else
  connect(m_ui.close,
	  &QPushButton::clicked,
	  this,
	  &glitch_tools::close);
#endif
  m_ui.close->setIcon(QIcon(":/close.png"));

  foreach(auto widget, findChildren<QRadioButton *> ())
    if(widget)
      connect(widget,
	      &QRadioButton::clicked,
	      this,
	      &glitch_tools::slotOperationChanged);

  setWindowModality(Qt::NonModal);
}

glitch_tools::~glitch_tools()
{
}

glitch_tools::Operations glitch_tools::operation(void) const
{
  if(m_ui.intelligent->isChecked())
    return Operations::INTELLIGENT;
  else if(m_ui.select->isChecked())
    return Operations::SELECT;
  else if(m_ui.wire_connect->isChecked())
    return Operations::WIRE_CONNECT;
  else
    return Operations::WIRE_DISCONNECT;
}

void glitch_tools::populateMenu(QMenu *menu, QObject *parent)
{
  if(!menu || !parent)
    return;

  QStringList list;
  auto group = new QActionGroup(parent);

  connect(menu,
	  &QMenu::aboutToShow,
	  this,
	  &glitch_tools::slotAboutToShowConnectionsMenu);
  list << tr("Intelligent (Connect / Disconnect / Select)")
       << tr("Select Widgets")
       << tr("Wire (Connect)")
       << tr("Wire (Disconnect)");
  menu->clear();

  for(int i = 0; i < list.size(); i++)
    {
      auto action = menu->addAction(list.at(i));

      action->setCheckable(true);
      group->addAction(action);

      if(i == 0)
	{
	  connect(action,
		  &QAction::triggered,
		  m_ui.intelligent,
		  &QRadioButton::click);
	  action->setChecked(m_ui.intelligent->isChecked());
	}
      else if(i == 1)
	{
	  connect(action,
		  &QAction::triggered,
		  m_ui.select,
		  &QRadioButton::click);
	  action->setChecked(m_ui.select->isChecked());
	}
      else if(i == 2)
	{
	  connect(action,
		  &QAction::triggered,
		  m_ui.wire_connect,
		  &QRadioButton::click);
	  action->setChecked(m_ui.wire_connect->isChecked());
	}
      else
	{
	  connect(action,
		  &QAction::triggered,
		  m_ui.wire_disconnect,
		  &QRadioButton::click);
	  action->setChecked(m_ui.wire_disconnect->isChecked());
	}
    }

  if(group->actions().isEmpty())
    group->deleteLater();
}

void glitch_tools::setOperation(const Operations operation)
{
  switch(operation)
    {
    case Operations::SELECT:
      {
	m_ui.select->setChecked(true);
	break;
      }
    case Operations::WIRE_CONNECT:
      {
	m_ui.wire_connect->setChecked(true);
	break;
      }
    case Operations::WIRE_DISCONNECT:
      {
	m_ui.wire_disconnect->setChecked(true);
	break;
      }
    default:
      {
	m_ui.intelligent->setChecked(true);
	break;
      }
    }
}

void glitch_tools::slotAboutToShowConnectionsMenu(void)
{
  auto menu = qobject_cast<QMenu *> (sender());

  if(!menu)
    return;

  foreach(auto action, menu->actions())
    if(action)
      {
	auto const string(action->text());

	action->blockSignals(true);

	if(string == tr("Intelligent (Connect / Disconnect / Select)"))
	  action->setChecked(m_ui.intelligent->isChecked());
	else if(string == tr("Select Widgets"))
	  action->setChecked(m_ui.select->isChecked());
	else if(string == tr("Wire (Connect)"))
	  action->setChecked(m_ui.wire_connect->isChecked());
	else
	  action->setChecked(m_ui.wire_disconnect->isChecked());

	action->blockSignals(false);
      }
}

void glitch_tools::slotOperationChanged(void)
{
  if(m_ui.intelligent == sender())
    emit operation(Operations::INTELLIGENT);
  else if(m_ui.select == sender())
    emit operation(Operations::SELECT);
  else if(m_ui.wire_connect == sender())
    emit operation(Operations::WIRE_CONNECT);
  else
    emit operation(Operations::WIRE_DISCONNECT);
}
