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

#include "glowbot-object-edit-window.h"
#include "glowbot-object-setup-arduino.h"
#include "glowbot-object-view.h"

glowbot_object_setup_arduino::glowbot_object_setup_arduino
(QWidget *parent):glowbot_object(parent)
{
  initialize(parent);
}

glowbot_object_setup_arduino::glowbot_object_setup_arduino
(const quint64 id, QWidget *parent):glowbot_object(id, parent)
{
  initialize(parent);
}

glowbot_object_setup_arduino::~glowbot_object_setup_arduino()
{
  if(m_editWindow)
    m_editWindow->deleteLater();
}

bool glowbot_object_setup_arduino::hasView(void) const
{
  return true;
}

bool glowbot_object_setup_arduino::isMandatory(void) const
{
  return true;
}

glowbot_object_setup_arduino *glowbot_object_setup_arduino::clone
(QWidget *parent) const
{
  glowbot_object_setup_arduino *object =
    new glowbot_object_setup_arduino(parent);

  object->initialize(parent);
  return object;
}

void glowbot_object_setup_arduino::addActions(QMenu &menu) const
{
  menu.addAction(tr("&Edit..."),
		 this,
		 SLOT(slotEdit(void)));
  addDefaultActions(menu);
}

void glowbot_object_setup_arduino::initialize(QWidget *parent)
{
  if(m_initialized)
    return;
  else
    m_initialized = true;

  m_editView = new glowbot_object_view
    (glowbot_common::ArduinoProject, m_id, this);
  m_editWindow = new glowbot_object_edit_window(parent);
  m_editWindow->setCentralWidget(m_editView);
  m_editWindow->setWindowIcon(QIcon(":Logo/glowbot-logo.png"));
  m_editWindow->setWindowTitle(tr("GlowBot: setup()"));
  m_editWindow->resize(600, 600);
  m_ui.setupUi(this);
  m_ui.label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
  m_ui.label->setAutoFillBackground(true);
  m_type = "arduino-setup";
  connect(m_editView,
	  SIGNAL(changed(void)),
	  this,
	  SIGNAL(changed(void)));
}

void glowbot_object_setup_arduino::mouseDoubleClickEvent(QMouseEvent *event)
{
  slotEdit();
  QWidget::mouseDoubleClickEvent(event);
}

void glowbot_object_setup_arduino::save(const QSqlDatabase &db, QString &error)
{
  glowbot_object::save(db, error);

  if(!error.isEmpty())
    return;

  m_editView->save(db, error);
}

void glowbot_object_setup_arduino::slotEdit(void)
{
  m_editWindow->showNormal();
  m_editWindow->raise();
}
