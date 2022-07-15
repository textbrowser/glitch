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

#include "glitch-object-arithmetic-operator-arduino.h"
#include "glitch-undo-command.h"

glitch_object_arithmetic_operator_arduino::
glitch_object_arithmetic_operator_arduino
(QWidget *parent):glitch_object_arithmetic_operator_arduino(1, parent)
{
}

glitch_object_arithmetic_operator_arduino::
glitch_object_arithmetic_operator_arduino
(const QString &operatorType, QWidget *parent):
  glitch_object_arithmetic_operator_arduino(1, parent)
{
  setOperatorType(operatorType);
}

glitch_object_arithmetic_operator_arduino::
glitch_object_arithmetic_operator_arduino
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_operatorType = OperatorTypes::ADDITION_OPERATOR;
  m_type = "arduino-arithmeticoperator";
  m_ui.setupUi(this);
  connect(m_ui.arithmetic_operator,
	  QOverload<int>::of(&QComboBox::currentIndexChanged),
	  this,
	  &glitch_object_arithmetic_operator_arduino::
	  slotArithmeticOperatorChanged);
  prepareContextMenu();
  setOperatorType(m_operatorType);
}

glitch_object_arithmetic_operator_arduino::
~glitch_object_arithmetic_operator_arduino()
{
}

QString glitch_object_arithmetic_operator_arduino::
arithmeticOperator(void) const
{
  return m_ui.arithmetic_operator->currentText();
}

QString glitch_object_arithmetic_operator_arduino::code(void) const
{
  switch(m_operatorType)
    {
    default:
      {
	QString str("(");
	auto list(inputs());

	for(int i = 0; i < list.size(); i++)
	  {
	    str.append(QString("(%1)").arg(list.at(i)));

	    if(i != list.size() - 1)
	      str.append
		(QString(" %1 ").arg(m_ui.arithmetic_operator->currentText()));
	  }

	str = str.trimmed();
	str.append(")");
	return str;
      }
    }
}

bool glitch_object_arithmetic_operator_arduino::hasInput(void) const
{
  return true;
}

bool glitch_object_arithmetic_operator_arduino::hasOutput(void) const
{
  return true;
}

bool glitch_object_arithmetic_operator_arduino::isFullyWired(void) const
{
  switch(m_operatorType)
    {
    default:
      {
	return false;
      }
    }
}

bool glitch_object_arithmetic_operator_arduino::shouldPrint(void) const
{
  return false;
}

glitch_object_arithmetic_operator_arduino *
glitch_object_arithmetic_operator_arduino::clone(QWidget *parent) const
{
  auto clone = new glitch_object_arithmetic_operator_arduino(parent);

  clone->m_properties = m_properties;
  clone->setOperatorType(m_operatorType);
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_arithmetic_operator_arduino *
glitch_object_arithmetic_operator_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_arithmetic_operator_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties(values.value("properties").toString().split('&'));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_arithmetic_operator_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_arithmetic_operator_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["arithmetic_operator"] =
    m_ui.arithmetic_operator->currentText().trimmed();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_arithmetic_operator_arduino::setOperatorType
(const QString &operatorType)
{
  if(operatorType.contains("%"))
    setOperatorType(OperatorTypes::REMAINDER_OPERATOR);
  else if(operatorType.contains("*"))
    setOperatorType(OperatorTypes::MULTIPLICATION_OPERATOR);
  else if(operatorType.contains("/"))
    setOperatorType(OperatorTypes::DIVISION_OPERATOR);
  else if(operatorType.contains("="))
    setOperatorType(OperatorTypes::ASSIGNMENT_OPERATOR);
  else if(operatorType.contains("+"))
    setOperatorType(OperatorTypes::ADDITION_OPERATOR);
  else if(operatorType.contains("-"))
    setOperatorType(OperatorTypes::SUBTRACTION_OPERATOR);
  else
    setOperatorType(OperatorTypes::ADDITION_OPERATOR);
}

void glitch_object_arithmetic_operator_arduino::setOperatorType
(const OperatorTypes operatorType)
{
  m_operatorType = operatorType;
  m_ui.arithmetic_operator->blockSignals(true);

  switch(m_operatorType)
    {
    case OperatorTypes::ADDITION_OPERATOR:
      {
	m_ui.arithmetic_operator->setCurrentIndex(5);
	break;
      }
    case OperatorTypes::ASSIGNMENT_OPERATOR:
      {
	m_ui.arithmetic_operator->setCurrentIndex(4);
	break;
      }
    case OperatorTypes::DIVISION_OPERATOR:
      {
	m_ui.arithmetic_operator->setCurrentIndex(3);
	break;
      }
    case OperatorTypes::MULTIPLICATION_OPERATOR:
      {
	m_ui.arithmetic_operator->setCurrentIndex(1);
	break;
      }
    case OperatorTypes::REMAINDER_OPERATOR:
      {
	m_ui.arithmetic_operator->setCurrentIndex(0);
	break;
      }
    case OperatorTypes::SUBTRACTION_OPERATOR:
      {
	m_ui.arithmetic_operator->setCurrentIndex(2);
	break;
      }
    default:
      {
	m_ui.arithmetic_operator->setCurrentIndex(5);
	break;
      }
    }

  m_ui.arithmetic_operator->blockSignals(false);
  setName(m_ui.arithmetic_operator->currentText());
}

void glitch_object_arithmetic_operator_arduino::setProperties
(const QStringList &list)
{
  glitch_object::setProperties(list);

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("arithmetic_operator = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::ARITHMETIC_OPERATOR] = string.trimmed();
	}
    }

  setOperatorType
    (m_properties.value(glitch_object::ARITHMETIC_OPERATOR).toString());
}

void glitch_object_arithmetic_operator_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::ARITHMETIC_OPERATOR:
      {
	m_ui.arithmetic_operator->blockSignals(true);
	m_ui.arithmetic_operator->setCurrentIndex
	  (m_ui.arithmetic_operator->findText(value.toString()));
	m_ui.arithmetic_operator->blockSignals(false);
	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_object_arithmetic_operator_arduino::slotArithmeticOperatorChanged
(void)
{
  setOperatorType(m_ui.arithmetic_operator->currentText());

  if(!m_undoStack)
    return;

  auto undoCommand = new glitch_undo_command
    (m_ui.arithmetic_operator->currentText(),
     m_properties.value(Properties::ARITHMETIC_OPERATOR).toString(),
     glitch_undo_command::PROPERTY_CHANGED,
     Properties::ARITHMETIC_OPERATOR,
     this);

  m_properties[Properties::ARITHMETIC_OPERATOR] =
    m_ui.arithmetic_operator->currentText();
  undoCommand->setText(tr("arithmetic operator changed"));
  m_undoStack->push(undoCommand);
  emit changed();
}