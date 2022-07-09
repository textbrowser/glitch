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

#include "glitch-object-time-arduino.h"

glitch_object_time_arduino::glitch_object_time_arduino
(const QString &timeType, QWidget *parent):glitch_object_time_arduino(1, parent)
{
  m_timeType = stringToTimeType(timeType);

  switch(m_timeType)
    {
    case Type::DELAY:
      {
	m_ui.label->setText("delay()");
	break;
      }
    case Type::DELAY_MICROSECONDS:
      {
	m_ui.label->setText("delayMicroseconds()");
	break;
      }
    case Type::MICROS:
      {
	m_ui.label->setText("micros()");
	break;
      }
    case Type::MILLIS:
      {
	m_ui.label->setText("millis()");
	break;
      }
    default:
      {
	m_ui.label->setText("delay()");
	break;
      }
    }

  setName(m_ui.label->text());
}

glitch_object_time_arduino::glitch_object_time_arduino
(const quint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_type = "arduino-time";
  m_ui.setupUi(this);
  prepareContextMenu();
  setName(m_ui.label->text());
}

glitch_object_time_arduino::~glitch_object_time_arduino()
{
}

QString glitch_object_time_arduino::code(void) const
{
  switch(m_timeType)
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
  if(m_timeType == Type::DELAY || m_timeType == Type::DELAY_MICROSECONDS)
    return true;
  else
    return false;
}

bool glitch_object_time_arduino::hasOutput(void) const
{
  if(m_timeType == Type::MICROS || m_timeType == Type::MILLIS)
    return true;
  else
    return false;
}

bool glitch_object_time_arduino::isFullyWired(void) const
{
  return false;
}

bool glitch_object_time_arduino::shouldPrint(void) const
{
  return true;
}

glitch_object_time_arduino *glitch_object_time_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_time_arduino(timeTypeToString(), parent);

  clone->m_timeType = m_timeType;
  clone->m_properties = m_properties;
  clone->m_ui.label->setText(m_ui.label->text());
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
    (values.value("myoid").toULongLong(), parent);

  object->setProperties(values.value("properties").toString().split('&'));
  object->setStyleSheet(values.value("stylesheet").toString());
  object->m_timeType = stringToTimeType
    (object->m_properties.value(Properties::TIME_TYPE).toString());
  object->m_ui.label->setText
    (object->m_properties.value(Properties::TIME_TYPE).toString());
  return object;
}

void glitch_object_time_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_time_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["time_type"] = m_ui.label->text().trimmed();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_time_arduino::setProperties(const QStringList &list)
{
  /*
  ** Redundancies.
  */

  m_properties[Properties::TIME_TYPE] = "delay()";

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("time_type = "))
	{
	  string = string.mid(string.indexOf('=') + 1).toLower();
	  string.remove("\"");

	  if(string.contains("delaymicroseconds"))
	    string = "delayMicroseconds()";
	  else if(string.contains("delay"))
	    string = "delay()";
	  else if(string.contains("macros"))
	    string = "macros()";
	  else if(string.contains("millis"))
	    string = "millis()";
	  else
	    string = "delay()";

	  m_properties[Properties::TIME_TYPE] = string.trimmed();
	}
    }

  setName(m_properties.value(Properties::TIME_TYPE).toString());
}
