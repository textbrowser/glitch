/*
** Copyright (c) 2015 - 10^10^10, Alexis Megas.
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

#include "glitch-object-time-arduino.h"

glitch_object_time_arduino::glitch_object_time_arduino
(const QString &timeType, QWidget *parent):glitch_object_time_arduino(1, parent)
{
  QString string("");

  switch(stringToTimeType(timeType))
    {
    case Type::DELAY:
      {
	string = "delay()";
	break;
      }
    case Type::DELAY_MICROSECONDS:
      {
	string = "delayMicroseconds()";
	break;
      }
    case Type::MICROS:
      {
	string = "micros()";
	break;
      }
    case Type::MILLIS:
      {
	string = "millis()";
	break;
      }
    default:
      {
	string = "delay()";
	break;
      }
    }

  m_text = string;
  setName(m_text);
  setToolTip(description());
}

glitch_object_time_arduino::glitch_object_time_arduino
(const qint64 id, QWidget *parent):glitch_object_simple_text_arduino(id, parent)
{
  m_functionsList << "delay()"
		  << "delayMicroseconds()"
		  << "micros()"
		  << "millis()";
  m_type = "arduino-time";
  prepareContextMenu();
  setName(m_text);
}

glitch_object_time_arduino::~glitch_object_time_arduino()
{
}

QString glitch_object_time_arduino::code(void) const
{
  if(!property(Properties::GENERATE_SOURCE).toBool())
    return "";

  switch(stringToTimeType(m_text))
    {
    case Type::DELAY:
      {
	return QString("delay(%1);").arg(inputs().value(0));
      }
    case Type::DELAY_MICROSECONDS:
      {
	return QString("delayMicroseconds(%1);").arg(inputs().value(0));
      }
    case Type::MICROS:
      {
	return "micros();";
      }
    case Type::MILLIS:
      {
	return "millis();";
      }
    default:
      {
	return QString("delay(%1);").arg(inputs().value(0));
      }
    }
}

bool glitch_object_time_arduino::hasInput(void) const
{
  auto const timeType = stringToTimeType(m_text);

  if(timeType == Type::DELAY || timeType == Type::DELAY_MICROSECONDS)
    return true;
  else
    return false;
}

bool glitch_object_time_arduino::hasOutput(void) const
{
  auto const timeType = stringToTimeType(m_text);

  if(timeType == Type::MICROS || timeType == Type::MILLIS)
    return true;
  else
    return false;
}

bool glitch_object_time_arduino::isFullyWired(void) const
{
  switch(stringToTimeType(m_text))
    {
    case Type::DELAY:
      {
	return inputs().size() >= 1;
      }
    case Type::DELAY_MICROSECONDS:
      {
	return inputs().size() >= 1;
      }
    default:
      {
	return true;
      }
    }
}

bool glitch_object_time_arduino::shouldPrint(void) const
{
  auto const timeType = stringToTimeType(m_text);

  if(timeType == Type::DELAY || timeType == Type::DELAY_MICROSECONDS)
    return true;
  else
    return false;
}

glitch_object_time_arduino *glitch_object_time_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_time_arduino(timeTypeToString(), parent);

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

glitch_object_time_arduino *glitch_object_time_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_time_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties(splitPropertiesAmpersand(values.value("properties")));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_time_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["time_type"] = m_text;
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_time_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::COMPRESSED_WIDGET] = false;

  QString function("delay()");

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("time_type = "))
	{
	  string = string.mid(string.indexOf('=') + 1).toLower();
	  string.remove("\"");

	  if(string.contains("delaymicroseconds"))
	    string = "delayMicroseconds()";
	  else if(string.contains("micros"))
	    string = "micros()";
	  else if(string.contains("millis"))
	    string = "millis()";
	  else
	    string = "delay()";

	  function = string.trimmed();
	  break;
	}
    }

  m_text = function;
  setName(m_text);
  setToolTip(description());
}

void glitch_object_time_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object_simple_text_arduino::setProperty(property, value);

  switch(property)
    {
    case Properties::TIME_TYPE:
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
