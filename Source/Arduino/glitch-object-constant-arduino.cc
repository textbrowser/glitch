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

#include "glitch-object-constant-arduino.h"
#include "glitch-undo-command.h"

glitch_object_constant_arduino::glitch_object_constant_arduino
(QWidget *parent):glitch_object_constant_arduino(1, parent)
{
}

glitch_object_constant_arduino::glitch_object_constant_arduino
(const QString &constantType,
 QWidget *parent):glitch_object_constant_arduino(1, parent)
{
  setConstantType(constantType);
}

glitch_object_constant_arduino::glitch_object_constant_arduino
(const quint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_constantType = HIGH;
  m_type = "arduino-constant";
  m_ui.setupUi(this);
  connect(m_ui.constant,
	  SIGNAL(currentIndexChanged(int)),
	  this,
	  SLOT(slotConstantChanged(void)));
  prepareContextMenu();
  setName(m_type);
}

glitch_object_constant_arduino::
~glitch_object_constant_arduino()
{
}

glitch_object_constant_arduino *glitch_object_constant_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_constant_arduino(parent);

  clone->m_constantType = m_constantType;
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_constant_arduino *glitch_object_constant_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_constant_arduino
    (values.value("myoid").toULongLong(), parent);

  object->setProperties(values.value("properties").toString().split('&'));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_constant_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_constant_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["constant"] = m_ui.constant->currentText();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_constant_arduino::setConstantType
(const QString &constantType)
{
  QString c(constantType.toLower().trimmed());

  if(c == "false")
    m_constantType = FALSE;
  else if(c == "input")
    m_constantType = INPUT;
  else if(c == "input_pullup")
    m_constantType = INPUT_PULLUP;
  else if(c == "led_builtin")
    m_constantType = LED_BUILTIN;
  else if(c == "true")
    m_constantType = TRUE;
  else
    m_constantType = HIGH;
}

void glitch_object_constant_arduino::slotConstantChanged(void)
{
  auto undoCommand = new glitch_undo_command
    (m_properties.value(Properties::CONSTANT_TYPE).toString(),
     glitch_undo_command::CONSTANT_TYPE_CHANGED,
     this);

  undoCommand->setText(tr("constant type changed"));
  m_undoStack->push(undoCommand);
  emit changed();
}
