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

#include "glitch-object-trigonometry-arduino.h"

glitch_object_trigonometry_arduino::glitch_object_trigonometry_arduino
(const QString &trigonometryType, QWidget *parent):
  glitch_object_trigonometry_arduino(1, parent)
{
  QString string("");

  switch(stringToTrigonometryType(trigonometryType))
    {
    case Types::COS:
      {
	string = "cos()";
	break;
      }
    case Types::SIN:
      {
	string = "sin()";
	break;
      }
    case Types::TAN:
      {
	string = "tan()";
	break;
      }
    default:
      {
	string = "cos()";
	break;
      }
    }

  m_text = string;
  setName(m_text);
}

glitch_object_trigonometry_arduino::glitch_object_trigonometry_arduino
(const qint64 id, QWidget *parent):glitch_object_simple_text_arduino(id, parent)
{
  m_functionsList << "cos()" << "sin()" << "tan()";
  m_type = "arduino-trigonometry";
  prepareContextMenu();
  setName(m_text);
}

glitch_object_trigonometry_arduino::~glitch_object_trigonometry_arduino()
{
}

QString glitch_object_trigonometry_arduino::code(void) const
{
  switch(stringToTrigonometryType(m_text))
    {
    case Types::COS:
      {
	return QString("cos(%1);").arg(inputs().value(0));
      }
    case Types::SIN:
      {
	return QString("sin(%1);").arg(inputs().value(0));
      }
    case Types::TAN:
      {
	return QString("tan(%1);").arg(inputs().value(0));
      }
    default:
      {
	return QString("cos(%1);").arg(inputs().value(0));
      }
    }
}

bool glitch_object_trigonometry_arduino::hasInput(void) const
{
  return true;
}

bool glitch_object_trigonometry_arduino::hasOutput(void) const
{
  return true;
}

bool glitch_object_trigonometry_arduino::isFullyWired(void) const
{
  return inputs().size() >= 1;
}

bool glitch_object_trigonometry_arduino::shouldPrint(void) const
{
  return false;
}

glitch_object_trigonometry_arduino *glitch_object_trigonometry_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_trigonometry_arduino
    (trigonometryTypeToString(), parent);

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

glitch_object_trigonometry_arduino *glitch_object_trigonometry_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_trigonometry_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties(values.value("properties").toString().split('&'));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_trigonometry_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["trigonometry_type"] = m_text;
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_trigonometry_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::COMPRESSED_WIDGET] = false;

  QString function("cos()");

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("trigonometry_type = "))
	{
	  string = string.mid(string.indexOf('=') + 1).toLower();
	  string.remove("\"");

	  if(string.contains("sin"))
	    string = "sin()";
	  else if(string.contains("tan"))
	    string = "tan()";
	  else
	    string = "cos()";

	  function = string.trimmed();
	  break;
	}
    }

  m_text = function;
  setName(m_text);
}

void glitch_object_trigonometry_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object_simple_text_arduino::setProperty(property, value);

  switch(property)
    {
    case Properties::TRIGONOMETRY_TYPE:
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
}
