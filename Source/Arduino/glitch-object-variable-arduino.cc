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

#include "glitch-object-compound-operator-arduino.h"
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
  m_ui.array->setChecked(string.contains("array"));
  m_ui.type->setCurrentIndex(m_ui.type->findText(string.remove(" (array)")));
}

glitch_object_variable_arduino::glitch_object_variable_arduino
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_type = "arduino-variable";
  m_ui.setupUi(this);

  auto list(glitch_structures_arduino::variableTypes());

  list.prepend("");
  list.removeAll("array");
  m_ui.type->addItems(list);
  prepareContextMenu();
  setName(m_type);
  connectSignals(true);
}

glitch_object_variable_arduino::~glitch_object_variable_arduino()
{
}

QString glitch_object_variable_arduino::code(void) const
{
  QString assignment("=");
  auto array(m_ui.array->isChecked() ? QString("[]") : QString(""));
  auto inputs(this->inputs());
  auto name(m_ui.name->text().trimmed());
  auto pointerAccess(m_ui.pointer_access->currentText());
  auto progmem(m_ui.progmem->isChecked() ? QString("PROGMEM") : QString(""));
  auto qualifier(m_ui.qualifier->currentText());
  auto type(m_ui.type->currentText().trimmed());

  if(glitch_object_compound_operator_arduino::isOperator(inputs.value(0)) ||
     glitch_object_compound_operator_arduino::isOperator(inputs.value(1)))
    assignment.clear();

  if(array.isEmpty())
    {
      // Non-array.

      if(type.isEmpty())
	{
	  /*
	  ** The variable is not being defined.
	  */

	  if(inputs.isEmpty())
	    return (pointerAccess + name + ";").trimmed();
	  else
	    return (pointerAccess +
		    name +
		    " " +
		    assignment +
		    " " +
		    inputs.value(0) +
		    ";").trimmed();
	}
      else
	{
	  /*
	  ** The variable is being defined.
	  */

	  if(inputs.isEmpty())
	    return (qualifier +
		    " " +
		    type +
		    " " +
		    pointerAccess +
		    name +
		    " " +
		    progmem +
		    ";").trimmed();
	  else
	    return (qualifier +
		    " " +
		    type +
		    " " +
		    pointerAccess +
		    name +
		    " " +
		    progmem +
		    " " +
		    assignment +
		    " " +
		    inputs.value(0) +
		    ";").trimmed();
	}
    }
  else
    {
      // Array.

      if(type.isEmpty())
	{
	  /*
	  ** The array is not being defined.
	  */

	  if(inputs.size() >= 2)
	    return QString("%1[%2] = %3;").
	      arg(name).
	      arg(inputs.value(0)).
	      arg(inputs.value(1));
	  else
	    return QString("%1[%2];").arg(name).arg(inputs.value(0));
	}
      else
	{
	  /*
	  ** The array is being defined.
	  */

	  if(inputs.size() >= 2)
	    return (qualifier +
		    " " +
		    type +
		    " " +
		    pointerAccess +
		    name +
		    "[" +
		    inputs.value(0) +
		    "] " +
		    progmem +
		    ";").trimmed();
	  else
	    {
	      if(inputs.value(0).startsWith("{"))
		return (qualifier +
			" " +
			type +
			" " +
			pointerAccess +
			name +
			"[] " +
			progmem +
			" " +
			assignment +
			" " +
			inputs.value(0) +
			";").trimmed();
	      else
		return (qualifier +
			" " +
			type +
			" " +
			pointerAccess +
			name +
			"[" +
			inputs.value(0) +
			"] " +
			progmem +
			";").trimmed();
	    }
	}
    }
}

bool glitch_object_variable_arduino::hasInput(void) const
{
  return true;
}

bool glitch_object_variable_arduino::hasOutput(void) const
{
  return true;
}

bool glitch_object_variable_arduino::isFullyWired(void) const
{
  if(m_ui.array->isChecked())
    return inputs().size() > 1;
  else
    return inputs().size() > 0;
}

bool glitch_object_variable_arduino::shouldPrint(void) const
{
  if(outputs().size() > 0)
    return false;
  else if(m_ui.array->isChecked())
    return inputs().size() == 2 ||
      m_ui.type->currentText().trimmed().length() > 0;
  else
    return inputs().size() > 0 ||
      m_ui.type->currentText().trimmed().length() > 0;
}

glitch_object_variable_arduino *glitch_object_variable_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_variable_arduino(parent);

  clone->cloneWires(m_wires);
  clone->m_properties = m_properties;
  clone->m_ui.array->setChecked(m_ui.array->isChecked());
  clone->m_ui.name->setText(m_ui.name->text().trimmed());
  clone->m_ui.name->selectAll();
  clone->m_ui.pointer_access->setCurrentIndex
    (m_ui.pointer_access->currentIndex());
  clone->m_ui.progmem->setChecked(m_ui.progmem->isChecked());
  clone->m_ui.qualifier->setCurrentIndex(m_ui.qualifier->currentIndex());
  clone->m_ui.type->setCurrentIndex(m_ui.type->currentIndex());
  clone->connectSignals(true);
  clone->resize(size());
  clone->setCanvasSettings(m_canvasSettings);
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
    (values.value("myoid").toLongLong(), parent);

  object->setProperties
    (values.value("properties").toString().split(s_splitRegularExpression));
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
      connect(m_ui.array,
	      QOverload<bool>::of(&QToolButton::toggled),
	      this,
	      &glitch_object_variable_arduino::slotToolButtonChecked,
	      Qt::UniqueConnection);
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
      connect(m_ui.progmem,
	      QOverload<bool>::of(&QToolButton::toggled),
	      this,
	      &glitch_object_variable_arduino::slotToolButtonChecked,
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
    }
  else
    {
      disconnect(m_ui.array,
		 QOverload<bool>::of(&QToolButton::toggled),
		 this,
		 &glitch_object_variable_arduino::slotToolButtonChecked);
      disconnect(m_ui.name,
		 &QLineEdit::returnPressed,
		 this,
		 &glitch_object_variable_arduino::slotLineEditSet);
      disconnect(m_ui.pointer_access,
		 QOverload<int>::of(&QComboBox::currentIndexChanged),
		 this,
		 &glitch_object_variable_arduino::slotComboBoxChanged);
      disconnect(m_ui.progmem,
		 QOverload<bool>::of(&QToolButton::toggled),
		 this,
		 &glitch_object_variable_arduino::slotToolButtonChecked);
      disconnect(m_ui.qualifier,
		 QOverload<int>::of(&QComboBox::currentIndexChanged),
		 this,
		 &glitch_object_variable_arduino::slotComboBoxChanged);
      disconnect(m_ui.type,
		 QOverload<int>::of(&QComboBox::currentIndexChanged),
		 this,
		 &glitch_object_variable_arduino::slotComboBoxChanged);
    }
}

void glitch_object_variable_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["variable_array"] = m_ui.array->isChecked();
  properties["variable_name"] = m_ui.name->text().trimmed();
  properties["variable_pointer_access"] = m_ui.pointer_access->currentText();
  properties["variable_progmem"] = m_ui.progmem->isChecked();
  properties["variable_qualifier"] = m_ui.qualifier->currentText();
  properties["variable_type"] = m_ui.type->currentText().trimmed();
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

      if(string.simplified().startsWith("variable_array = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::VARIABLE_ARRAY] =
	    QVariant(string.trimmed()).toBool();
	  m_ui.array->setChecked(QVariant(string.trimmed()).toBool());
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
      if(string.simplified().startsWith("variable_progmem = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::VARIABLE_PROGMEM] =
	    QVariant(string.trimmed()).toBool();
	  m_ui.progmem->setChecked(QVariant(string.trimmed()).toBool());
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
    }

  connectSignals(true);
}

void glitch_object_variable_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::VARIABLE_ARRAY:
      {
	m_ui.array->blockSignals(true);
	m_ui.array->setChecked(value.toBool());
	m_ui.array->blockSignals(false);
	break;
      }
    case Properties::VARIABLE_NAME:
      {
	if(value.toString().trimmed().isEmpty())
	  m_ui.name->setText(tr("arduino-variable"));
	else
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
    case Properties::VARIABLE_PROGMEM:
      {
	m_ui.progmem->blockSignals(true);
	m_ui.progmem->setChecked(value.toBool());
	m_ui.progmem->blockSignals(false);
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
	m_ui.type->setCurrentIndex
	  (m_ui.type->findText(value.toString().trimmed()));

	if(m_ui.type->currentIndex() < 0)
	  m_ui.type->setCurrentIndex(0);

	m_ui.type->blockSignals(false);
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
  if(!m_undoStack)
    return;

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
  undoCommand->setText
    (tr("variable property changed (%1, %2)").
     arg(scenePos().x()).arg(scenePos().y()));
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

  if(!m_undoStack)
    return;

  auto property = glitch_object::Properties::VARIABLE_NAME;

  if(lineEdit->text() == m_properties.value(property).toString())
    return;

  auto undoCommand = new glitch_undo_command
    (lineEdit->text(),
     m_properties.value(property),
     glitch_undo_command::PROPERTY_CHANGED,
     property,
     this);

  m_properties[property] = lineEdit->text();
  undoCommand->setText
    (tr("variable property changed (%1, %2)").
     arg(scenePos().x()).arg(scenePos().y()));
  m_undoStack->push(undoCommand);
  emit changed();
}

void glitch_object_variable_arduino::slotToolButtonChecked(void)
{
  if(!m_undoStack)
    return;

  auto toolButton = qobject_cast<QToolButton *> (sender());

  if(!toolButton)
    return;

  auto property = glitch_object::Properties::VARIABLE_ARRAY;

  if(m_ui.progmem == toolButton)
    property = glitch_object::Properties::VARIABLE_PROGMEM;

  auto undoCommand = new glitch_undo_command
    (toolButton->isChecked(),
     m_properties.value(property),
     glitch_undo_command::PROPERTY_CHANGED,
     property,
     this);

  m_properties[property] = toolButton->isChecked();
  undoCommand->setText
    (tr("variable property changed (%1, %2)").
     arg(scenePos().x()).arg(scenePos().y()));
  m_undoStack->push(undoCommand);
  emit changed();
}
