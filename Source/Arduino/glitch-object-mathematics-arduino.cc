/*
** Copyright (c) 2015 - 10^10^10, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
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

#include "glitch-object-mathematics-arduino.h"

glitch_object_mathematics_arduino::glitch_object_mathematics_arduino
(const QString &mathematicsType, QWidget *parent):
  glitch_object_mathematics_arduino(1, parent)
{
  m_text = QString("%1()").arg
    (mathematicsTypeToString(stringToMathematicsType(mathematicsType)));
  setName(m_text);
  setToolTip(description());
}

glitch_object_mathematics_arduino::glitch_object_mathematics_arduino
(const qint64 id, QWidget *parent):glitch_object_simple_text_arduino(id, parent)
{
  m_functionsList << "abs()"
		  << "constrain()"
		  << "map()"
		  << "max()"
		  << "min()"
		  << "pow()"
		  << "sq()"
		  << "sqrt()";
  m_type = "arduino-mathematics";
  prepareContextMenu();
  setName(m_text);
}

glitch_object_mathematics_arduino::~glitch_object_mathematics_arduino()
{
}

QString glitch_object_mathematics_arduino::code(void) const
{
  if(!property(Properties::GENERATE_SOURCE).toBool())
    return "";

  switch(stringToMathematicsType(m_text))
    {
    case Type::ABS:
      {
	return QString("abs(%1);").arg(inputs().value(0));
      }
    case Type::CONSTRAIN:
      {
	return QString("constrain(%1, %2, %3);").
	  arg(inputs().value(0)).
	  arg(inputs().value(1)).
	  arg(inputs().value(2));
      }
    case Type::MAP:
      {
	return QString("map(%1, %2, %3, %4, %5);").
	  arg(inputs().value(0)).
	  arg(inputs().value(1)).
	  arg(inputs().value(2)).
	  arg(inputs().value(3)).
	  arg(inputs().value(4));
      }
    case Type::MAX:
      {
	return QString("max(%1, %2);").
	  arg(inputs().value(0)).
	  arg(inputs().value(1));
      }
    case Type::MIN:
      {
	return QString("min(%1, %2);").
	  arg(inputs().value(0)).
	  arg(inputs().value(1));
      }
    case Type::POW:
      {
	return QString("pow(%1, %2);").
	  arg(inputs().value(0)).
	  arg(inputs().value(1));
      }
    case Type::SQ:
      {
	return QString("sq(%1);").arg(inputs().value(0));
      }
    case Type::SQRT:
      {
	return QString("sqrt(%1);").arg(inputs().value(0));
      }
    default:
      {
	return QString("abs(%1);").arg(inputs().value(0));
      }
    }
}

bool glitch_object_mathematics_arduino::hasInput(void) const
{
  return true;
}

bool glitch_object_mathematics_arduino::hasOutput(void) const
{
  return true;
}

bool glitch_object_mathematics_arduino::isFullyWired(void) const
{
  switch(stringToMathematicsType(m_text))
    {
    case Type::ABS:
      {
	return inputs().size() >= 1;
      }
    case Type::CONSTRAIN:
      {
	return inputs().size() >= 3;
      }
    case Type::MAP:
      {
	return inputs().size() >= 5;
      }
    case Type::MAX:
      {
	return inputs().size() >= 2;
      }
    case Type::MIN:
      {
	return inputs().size() >= 2;
      }
    case Type::POW:
      {
	return inputs().size() >= 2;
      }
    case Type::SQ:
      {
	return inputs().size() >= 1;
      }
    case Type::SQRT:
      {
	return inputs().size() >= 1;
      }
    default:
      {
	return inputs().size() >= 1;
      }
    }
}

bool glitch_object_mathematics_arduino::shouldPrint(void) const
{
  return false;
}

glitch_object_mathematics_arduino *glitch_object_mathematics_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_mathematics_arduino(m_text, parent);

  clone->cloneWires(m_copiedConnectionsPositions);
  clone->cloneWires(m_wires);
  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->m_text = m_text;
  clone->resize(size());
  clone->setCanvasSettings(m_canvasSettings);
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_mathematics_arduino *glitch_object_mathematics_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_mathematics_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties(splitPropertiesAmpersand(values.value("properties")));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_mathematics_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["mathematics_type"] = m_text;
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_mathematics_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::COMPRESSED_WIDGET] = false;

  QString function("abs()");

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("mathematics_type = "))
	{
	  string = string.mid(string.indexOf('=') + 1).toLower();
	  string.remove("\"");
	  string = mathematicsTypeToString(stringToMathematicsType(string));
	  string = QString("%1()").arg(string);
	  function = string.trimmed();
	  break;
	}
    }

  m_text = function;
  setName(m_text);
  setToolTip(description());
}

void glitch_object_mathematics_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object_simple_text_arduino::setProperty(property, value);

  switch(property)
    {
    case Properties::MATHEMATICS_TYPE:
      {
	m_text = value.toString();
	setName(m_text);
	break;
      }
    default:
      {
	break;
      }
    }

  setToolTip(description());
}
