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

#include <QActionGroup>
#include <QMenu>
#include <QShortcut>

#include "glitch-tools.h"

glitch_tools::glitch_tools(QWidget *parent):QDialog(parent)
{
  m_ui.setupUi(this);
  connect(m_ui.close,
	  &QPushButton::clicked,
	  this,
	  &glitch_tools::close);
  m_ui.close->setIcon(QIcon::fromTheme("window-close"));

  foreach(auto widget, findChildren<QRadioButton *> ())
    if(widget)
      connect(widget,
	      &QRadioButton::clicked,
	      this,
	      &glitch_tools::slotOperationChanged);

  new QShortcut(tr("Ctrl+W"),
		this,
		SLOT(close(void)));
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

void glitch_tools::populateMenu(QMenu *menu, QObject *parent) const
{
  if(!menu)
    return;

  QStringList list;
  auto group = new QActionGroup(parent);
  auto m = menu->addMenu(tr("Connections"));

  list << tr("Intelligent")
       << tr("Select")
       << tr("Wire (Connect)")
       << tr("Wire (Disconnect)");

  for(int i = 0; i < list.size(); i++)
    {
      auto action = m->addAction(list.at(i));

      action->setCheckable(true);
      group->addAction(action);
    }
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
