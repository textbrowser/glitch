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

#include "glitch-object-variable-arduino.h"
#include "glitch-structures-arduino.h"
#include "glitch-undo-command.h"

glitch_object_variable_arduino::glitch_object_variable_arduino
(QWidget *parent):glitch_object_variable_arduino(1, parent)
{
}

glitch_object_variable_arduino::glitch_object_variable_arduino
(const quint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_type = "arduino-variable";
  m_ui.setupUi(this);
  m_ui.array_index->setVisible(false);
  m_ui.array_size->setVisible(false);
  m_ui.variable_type->addItems(glitch_structures_arduino::types());
  connect(m_ui.variable_type,
	  QOverload<int>::of(&QComboBox::currentIndexChanged),
	  this,
	  &glitch_object_variable_arduino::slotComboBoxChanged);
  prepareContextMenu();
  setName(m_type);
}

glitch_object_variable_arduino::
~glitch_object_variable_arduino()
{
}

QString glitch_object_variable_arduino::code(void) const
{
  return "";
}

bool glitch_object_variable_arduino::hasInput(void) const
{
  return true;
}

bool glitch_object_variable_arduino::hasOutput(void) const
{
  return true;
}

glitch_object_variable_arduino *glitch_object_variable_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_variable_arduino(parent);

  clone->m_properties = m_properties;
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_variable_arduino *glitch_object_variable_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_variable_arduino
    (values.value("myoid").toULongLong(), parent);

  object->setProperties(values.value("properties").toString().split('&'));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_variable_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_variable_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["variable_array_index"] = m_ui.array_index->value();
  properties["variable_array_size"] = m_ui.array_size->value();
  properties["variable_name"] = m_ui.name->text().trimmed();
  properties["variable_pointer_access"] = m_ui.pointer_access->currentText();
  properties["variable_qualifier"] = m_ui.qualifier->currentText();
  properties["variable_type"] = m_ui.variable_type->currentText();
  properties["variable_value"] = m_ui.value->text().trimmed();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_variable_arduino::setProperties
(const QStringList &list)
{
  glitch_object::setProperties(list);
}

void glitch_object_variable_arduino::slotComboBoxChanged(void)
{
  auto comboBox = qobject_cast<QComboBox *> (sender());

  if(!comboBox)
    return;

  if(comboBox == m_ui.variable_type)
    {
      m_ui.array_index->setVisible(comboBox->currentText() == "array");
      m_ui.array_size->setVisible(comboBox->currentText() == "array");
    }
}
