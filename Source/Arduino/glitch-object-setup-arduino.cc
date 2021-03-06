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

#include "glitch-object-edit-window.h"
#include "glitch-object-setup-arduino.h"
#include "glitch-object-view.h"

glitch_object_setup_arduino::glitch_object_setup_arduino
(QWidget *parent):glitch_object_setup_arduino(1, parent)
{
}

glitch_object_setup_arduino::glitch_object_setup_arduino
(const quint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_editView = new glitch_object_view
    (glitch_common::ProjectTypes::ArduinoProject, m_id, m_undoStack, this);
  m_editWindow = new glitch_object_edit_window(parent);
  m_editWindow->setCentralWidget(m_editView);
  m_editWindow->setWindowIcon(QIcon(":Logo/glitch-logo.png"));
  m_editWindow->setWindowTitle(tr("Glitch: setup()"));
  m_editWindow->resize(600, 600);
  m_properties[POSITION_LOCKED] = true;
  m_type = "arduino-setup";
  m_ui.setupUi(this);
  m_ui.label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
  m_ui.label->setAutoFillBackground(true);
  connect(m_editView,
	  SIGNAL(changed(void)),
	  this,
	  SIGNAL(changed(void)));
  prepareContextMenu();
  setName(m_ui.label->text());
}

glitch_object_setup_arduino::~glitch_object_setup_arduino()
{
  if(m_editWindow)
    m_editWindow->deleteLater();
}

bool glitch_object_setup_arduino::hasView(void) const
{
  return true;
}

bool glitch_object_setup_arduino::isMandatory(void) const
{
  return true;
}

glitch_object_setup_arduino *glitch_object_setup_arduino::clone
(QWidget *parent) const
{
  return new glitch_object_setup_arduino(parent);
}

void glitch_object_setup_arduino::addActions(QMenu &menu)
{
  if(!m_actions.contains(DefaultMenuActions::EDIT))
    {
      auto *action = new QAction(tr("&Edit..."), this);

      connect(action,
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotEdit(void)));
      m_actions[DefaultMenuActions::EDIT] = action;
      menu.addAction(action);
    }
  else
    menu.addAction(m_actions.value(DefaultMenuActions::EDIT));

  addDefaultActions(menu);
}

void glitch_object_setup_arduino::mouseDoubleClickEvent(QMouseEvent *event)
{
  slotEdit();
  QWidget::mouseDoubleClickEvent(event);
}

void glitch_object_setup_arduino::save(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  if(m_editView)
    m_editView->save(db, error);
}

void glitch_object_setup_arduino::slotEdit(void)
{
  m_editWindow->showNormal();
  m_editWindow->raise();
}
