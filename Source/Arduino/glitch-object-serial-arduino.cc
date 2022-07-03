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

#include "glitch-object-serial-arduino.h"

glitch_object_serial_arduino::glitch_object_serial_arduino
(const QString &serialType, QWidget *parent):
  glitch_object_serial_arduino(1, parent)
{
  m_serialType = stringToSerialType(serialType);

  switch(m_serialType)
    {
    case Type::BEGIN:
      {
	m_ui.label->setText("Serial.begin()");
	break;
      }
    case Type::PRINTLN:
      {
	m_ui.label->setText("Serial.println()");
	break;
      }
    default:
      {
	m_ui.label->setText("Serial.available()");
	break;
      }
    }

  setName(m_ui.label->text());
}

glitch_object_serial_arduino::glitch_object_serial_arduino
(const quint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_type = "arduino-serial";
  m_ui.setupUi(this);
  prepareContextMenu();
  setName(m_ui.label->text());
}

glitch_object_serial_arduino::~glitch_object_serial_arduino()
{
}

QString glitch_object_serial_arduino::code(void) const
{
  switch(m_serialType)
    {
    case Type::BEGIN:
      {
	if(inputs().size() == 1)
	  return QString("Serial.begin(%1);").arg(inputs().value(0));
	else
	  return QString("Serial.begin(%1, %2);").
	    arg(inputs().value(0)).
	    arg(inputs().value(1));
      }
    case Type::PRINTLN:
      {
	if(inputs().size() == 1)
	  return QString("Serial.println(%1);").arg(inputs().value(0));
	else
	  return QString("Serial.println(%1, %2);").
	    arg(inputs().value(0)).
	    arg(inputs().value(1));
      }
    default:
      {
	return QString("Serial.available(%1);").arg(inputs().value(0));
      }
    }
}

bool glitch_object_serial_arduino::hasInput(void) const
{
  switch(m_serialType)
    {
    case Type::BEGIN:
    case Type::PRINTLN:
      {
	return true;
      }
    default:
      {
	return false;
      }
    }
}

bool glitch_object_serial_arduino::hasOutput(void) const
{
  switch(m_serialType)
    {
    case Type::BEGIN:
      {
	return false;
      }
    case Type::PRINTLN:
      {
	return false;
      }
    default:
      {
	return false;
      }
    }
}

bool glitch_object_serial_arduino::shouldPrint(void) const
{
  return true;
}

glitch_object_serial_arduino *glitch_object_serial_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_serial_arduino(serialTypeToString(), parent);

  clone->m_serialType = m_serialType;
  clone->m_properties = m_properties;
  clone->m_ui.label->setText(m_ui.label->text());
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_serial_arduino *glitch_object_serial_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_serial_arduino
    (values.value("myoid").toULongLong(), parent);

  object->setProperties(values.value("properties").toString().split('&'));
  object->setStyleSheet(values.value("stylesheet").toString());
  object->m_serialType = stringToSerialType
    (object->m_properties.value(Properties::SERIAL_TYPE).toString());
  object->m_ui.label->setText
    (object->m_properties.value(Properties::SERIAL_TYPE).toString());
  return object;
}

void glitch_object_serial_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_serial_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["serial_type"] = m_ui.label->text().trimmed();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_serial_arduino::setProperties(const QStringList &list)
{
  /*
  ** Redundancies.
  */

  m_properties[Properties::SERIAL_TYPE] = "Serial.available()";

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("serial_type = "))
	{
	  string = string.mid(string.indexOf('=') + 1).toLower();
	  string.remove("\"");

	  if(string.contains("begin"))
	    string = "Serial.begin()";
	  else if(string.contains("println"))
	    string = "Serial.println()";
	  else
	    string = "Serial.available()";

	  m_properties[Properties::SERIAL_TYPE] = string.trimmed();
	}
    }

  setName(m_properties.value(Properties::SERIAL_TYPE).toString());
}
