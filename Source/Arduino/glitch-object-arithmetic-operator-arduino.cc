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
  m_properties[Properties::ARITHMETIC_OPERATOR] = m_text;
}

glitch_object_arithmetic_operator_arduino::
glitch_object_arithmetic_operator_arduino
(const qint64 id, QWidget *parent):glitch_object_simple_text_arduino(id, parent)
{
  m_functionsList << "-" << "*" << "/" << "%" << "+";
  m_operatorType = OperatorTypes::ADDITION_OPERATOR;
  m_type = "arduino-arithmeticoperator";
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
  return m_text;
}

QString glitch_object_arithmetic_operator_arduino::code(void) const
{
  switch(m_operatorType)
    {
    default:
      {
	QString string("(");
	auto list(inputs());

	for(int i = 0; i < list.size(); i++)
	  {
	    string.append(QString("(%1)").arg(list.at(i)));

	    if(i != list.size() - 1)
	      string.append(QString(" %1 ").arg(m_text));
	  }

	string = string.trimmed();
	string.append(")");
	return string;
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

  clone->cloneWires(m_copiedConnectionsPositions);
  clone->cloneWires(m_wires);
  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->m_text = m_text;
  clone->resize(size());
  clone->setCanvasSettings(m_canvasSettings);
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

void glitch_object_arithmetic_operator_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["arithmetic_operator"] = m_text;
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

  switch(m_operatorType)
    {
    case OperatorTypes::ADDITION_OPERATOR:
      {
	m_text = "+";
	break;
      }
    case OperatorTypes::DIVISION_OPERATOR:
      {
	m_text = "/";
	break;
      }
    case OperatorTypes::MULTIPLICATION_OPERATOR:
      {
	m_text = "*";
	break;
      }
    case OperatorTypes::REMAINDER_OPERATOR:
      {
	m_text = "%";
	break;
      }
    case OperatorTypes::SUBTRACTION_OPERATOR:
      {
	m_text = "-";
	break;
      }
    default:
      {
	m_text = "+";
	break;
      }
    }

  setName(m_text);
}

void glitch_object_arithmetic_operator_arduino::setProperties
(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::ARITHMETIC_OPERATOR] = "+";
  m_properties[Properties::COMPRESSED_WIDGET] = false;
  m_properties[Properties::TRANSPARENT] = true;

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
    (m_properties.value(Properties::ARITHMETIC_OPERATOR).toString());
}

void glitch_object_arithmetic_operator_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object_simple_text_arduino::setProperty(property, value);

  switch(property)
    {
    case Properties::ARITHMETIC_OPERATOR:
      {
	setOperatorType(value.toString());
	break;
      }
    default:
      {
	break;
      }
    }
}
