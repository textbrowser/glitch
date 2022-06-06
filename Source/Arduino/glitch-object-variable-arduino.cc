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
(const QString &variableType,
 QWidget *parent):glitch_object_variable_arduino(1, parent)
{
  auto string(variableType);

  string.remove("glitch-arduino-variables-");
  m_ui.type->setCurrentIndex(m_ui.type->findText(string));
}

glitch_object_variable_arduino::glitch_object_variable_arduino
(const quint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_type = "arduino-variable";
  m_ui.setupUi(this);

  auto list(glitch_structures_arduino::variableTypes());

  list.removeAll("array");
  m_ui.type->addItems(list);
  prepareContextMenu();
  setName(m_type);
}

glitch_object_variable_arduino::
~glitch_object_variable_arduino()
{
}

QString glitch_object_variable_arduino::code(void) const
{
  auto name(m_ui.name->text().trimmed());
  auto pointerAccess(m_ui.pointer_access->currentText());
  auto qualifier(m_ui.qualifier->currentText());
  auto type(m_ui.type->currentText());
  auto value(m_ui.value->text().trimmed());

  if(value.isEmpty())
    return (qualifier +
	    " " +
	    type +
	    " " +
	    pointerAccess +
	    name +
	    ";").trimmed();
  else
    return (qualifier +
	    " " +
	    type +
	    " " +
	    pointerAccess +
	    name +
	    " = " +
	    value + ";").trimmed();
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
  clone->m_ui.name->setText(m_ui.name->text().trimmed());
  clone->m_ui.name->selectAll();
  clone->m_ui.pointer_access->setCurrentIndex
    (m_ui.pointer_access->currentIndex());
  clone->m_ui.qualifier->setCurrentIndex(m_ui.qualifier->currentIndex());
  clone->m_ui.type->setCurrentIndex(m_ui.type->currentIndex());
  clone->m_ui.value->setText(m_ui.value->text().trimmed());
  clone->m_ui.value->selectAll();
  clone->connectSignals(true);
  clone->setName(clone->name());
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

void glitch_object_variable_arduino::connectSignals(const bool state)
{
  if(state)
    {
      connect(m_ui.name,
	      &QLineEdit::returnPressed,
	      this,
	      &glitch_object_variable_arduino::slotLineEditSet,
	      Qt::UniqueConnection);
      connect(m_ui.pointer_access,
	      QOverload<int>::of(&QComboBox::currentIndexChanged),
	      this,
	      &glitch_object_variable_arduino::slotComboBoxChanged,
	      Qt::UniqueConnection);
      connect(m_ui.qualifier,
	      QOverload<int>::of(&QComboBox::currentIndexChanged),
	      this,
	      &glitch_object_variable_arduino::slotComboBoxChanged,
	      Qt::UniqueConnection);
      connect(m_ui.type,
	      QOverload<int>::of(&QComboBox::currentIndexChanged),
	      this,
	      &glitch_object_variable_arduino::slotComboBoxChanged,
	      Qt::UniqueConnection);
      connect(m_ui.value,
	      &QLineEdit::returnPressed,
	      this,
	      &glitch_object_variable_arduino::slotLineEditSet,
	      Qt::UniqueConnection);
    }
  else
    {
      disconnect(m_ui.name,
		 &QLineEdit::returnPressed,
		 this,
		 &glitch_object_variable_arduino::slotLineEditSet);
      disconnect(m_ui.pointer_access,
		 QOverload<int>::of(&QComboBox::currentIndexChanged),
		 this,
		 &glitch_object_variable_arduino::slotComboBoxChanged);
      disconnect(m_ui.qualifier,
		 QOverload<int>::of(&QComboBox::currentIndexChanged),
		 this,
		 &glitch_object_variable_arduino::slotComboBoxChanged);
      disconnect(m_ui.type,
		 QOverload<int>::of(&QComboBox::currentIndexChanged),
		 this,
		 &glitch_object_variable_arduino::slotComboBoxChanged);
      disconnect(m_ui.value,
		 &QLineEdit::returnPressed,
		 this,
		 &glitch_object_variable_arduino::slotLineEditSet);
    }
}

void glitch_object_variable_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["variable_name"] = m_ui.name->text().trimmed();
  properties["variable_pointer_access"] = m_ui.pointer_access->currentText();
  properties["variable_qualifier"] = m_ui.qualifier->currentText();
  properties["variable_type"] = m_ui.type->currentText();
  properties["variable_value"] = m_ui.value->text().trimmed();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_variable_arduino::setProperties
(const QStringList &list)
{
  connectSignals(false);
  glitch_object::setProperties(list);

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("variable_array_index = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::VARIABLE_ARRAY_INDEX] = string.trimmed();
	}
      else if(string.simplified().startsWith("variable_array_size = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::VARIABLE_ARRAY_SIZE] = string.trimmed();
	}
      else if(string.simplified().startsWith("variable_name = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::VARIABLE_NAME] = string.trimmed();
	  m_ui.name->setText(string.trimmed());
	  m_ui.name->selectAll();
	  setName(m_ui.name->text());
	}
      else if(string.simplified().startsWith("variable_pointer_access = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::VARIABLE_POINTER_ACCESS] = string.trimmed();
	  m_ui.pointer_access->setCurrentIndex
	    (m_ui.pointer_access->findText(string.trimmed()));

	  if(m_ui.pointer_access->currentIndex() < 0)
	    m_ui.pointer_access->setCurrentIndex
	      (m_ui.pointer_access->findText(""));
	}
      else if(string.simplified().startsWith("variable_qualifier = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::VARIABLE_QUALIFIER] = string.trimmed();
	  m_ui.qualifier->setCurrentIndex
	    (m_ui.qualifier->findText(string.trimmed()));

	  if(m_ui.qualifier->currentIndex() < 0)
	    m_ui.qualifier->setCurrentIndex(m_ui.qualifier->findText(""));
	}
      else if(string.simplified().startsWith("variable_type = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::VARIABLE_TYPE] = string.trimmed();
	  m_ui.type->setCurrentIndex(m_ui.type->findText(string.trimmed()));

	  if(m_ui.type->currentIndex() < 0)
	    m_ui.type->setCurrentIndex(0);
	}
      else if(string.simplified().startsWith("variable_value = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::VARIABLE_VALUE] = string.trimmed();
	  m_ui.value->setText(string.trimmed());
	  m_ui.value->selectAll();
	}
    }

  connectSignals(true);
}

void glitch_object_variable_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::VARIABLE_NAME:
      {
	m_ui.name->setText(value.toString().trimmed());
	m_ui.name->selectAll();
	setName(m_ui.name->text());
	break;
      }
    case Properties::VARIABLE_POINTER_ACCESS:
      {
	m_ui.pointer_access->blockSignals(true);
	m_ui.pointer_access->setCurrentIndex
	  (m_ui.pointer_access->findText(value.toString()));
	m_ui.pointer_access->blockSignals(false);
	break;
      }
    case Properties::VARIABLE_QUALIFIER:
      {
	m_ui.qualifier->blockSignals(true);
	m_ui.qualifier->setCurrentIndex
	  (m_ui.qualifier->findText(value.toString()));
	m_ui.qualifier->blockSignals(false);
	break;
      }
    case Properties::VARIABLE_TYPE:
      {
	m_ui.type->blockSignals(true);
	m_ui.type->setCurrentIndex(m_ui.type->findText(value.toString()));
	m_ui.type->blockSignals(false);
	break;
      }
    case Properties::VARIABLE_VALUE:
      {
	m_ui.value->setText(value.toString().trimmed());
	m_ui.value->selectAll();
	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_object_variable_arduino::slotComboBoxChanged(void)
{
  auto comboBox = qobject_cast<QComboBox *> (sender());

  if(!comboBox)
    return;

  auto property = glitch_object::Properties::XYZ_PROPERTY;

  if(comboBox == m_ui.pointer_access)
    property = glitch_object::Properties::VARIABLE_POINTER_ACCESS;
  else if(comboBox == m_ui.qualifier)
    property = glitch_object::Properties::VARIABLE_QUALIFIER;
  else
    property = glitch_object::Properties::VARIABLE_TYPE;

  auto undoCommand = new glitch_undo_command
    (comboBox->currentText(),
     m_properties.value(property),
     glitch_undo_command::PROPERTY_CHANGED,
     property,
     this);

  m_properties[property] = comboBox->currentText();
  undoCommand->setText(tr("variable property changed"));
  m_undoStack->push(undoCommand);
  emit changed();
}

void glitch_object_variable_arduino::slotLineEditSet(void)
{
  auto lineEdit = qobject_cast<QLineEdit *> (sender());

  if(!lineEdit)
    return;

  lineEdit->setText(lineEdit->text().trimmed());
  lineEdit->selectAll();

  auto property = glitch_object::Properties::XYZ_PROPERTY;

  if(lineEdit == m_ui.name)
    property = glitch_object::Properties::VARIABLE_NAME;
  else
    property = glitch_object::Properties::VARIABLE_VALUE;

  if(lineEdit->text() == m_properties.value(property).toString())
    return;

  auto undoCommand = new glitch_undo_command
    (lineEdit->text(),
     m_properties.value(property),
     glitch_undo_command::PROPERTY_CHANGED,
     property,
     this);

  m_properties[property] = lineEdit->text();
  undoCommand->setText(tr("variable property changed"));
  m_undoStack->push(undoCommand);
  emit changed();
}
