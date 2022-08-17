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
  m_ui.label->setText
    (QString("Serial.%1()").arg(serialTypeToString(m_serialType)));
  setName(m_ui.label->text());
}

glitch_object_serial_arduino::glitch_object_serial_arduino
(const qint64 id, QWidget *parent):glitch_object(id, parent)
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
    case Type::AVAILABLE:
      {
	return "Serial.available();";
      }
    case Type::AVAILABLE_FOR_WRITE:
      {
	return "Serial.availableForWrite();";
      }
    case Type::BEGIN:
      {
	if(inputs().size() == 1)
	  return QString("Serial.begin(%1);").arg(inputs().value(0));
	else
	  return QString("Serial.begin(%1, %2);").
	    arg(inputs().value(0)).
	    arg(inputs().value(1));
      }
    case Type::END:
      {
	return "Serial.end();";
      }
    case Type::FIND:
      {
	if(inputs().size() == 1)
	  return QString("Serial.find(%1);").arg(inputs().value(0));
	else
	  return QString("Serial.find(%1, %2);").
	    arg(inputs().value(0)).
	    arg(inputs().value(1));
      }
    case Type::FIND_UNTIL:
      {
	return QString("Serial.findUntil(%1, %2);").
	  arg(inputs().value(0)).
	  arg(inputs().value(1));
      }
    case Type::FLUSH:
      {
	return "Serial.flush();";
      }
    case Type::PARSE_FLOAT:
      {
	if(inputs().isEmpty())
	  return "Serial.parseFloat();";
	else if(inputs().size() == 1)
	  return QString("Serial.parseFloat(%1);").arg(inputs().value(0));
	else
	  return QString("Serial.parseFloat(%1, %2);").
	    arg(inputs().value(0)).
	    arg(inputs().value(1));
      }
    case Type::PARSE_INT:
      {
	if(inputs().isEmpty())
	  return "Serial.parseInt();";
	else if(inputs().size() == 1)
	  return QString("Serial.parseInt(%1);").arg(inputs().value(0));
	else
	  return QString("Serial.parseInt(%1, %2);").
	    arg(inputs().value(0)).
	    arg(inputs().value(1));
      }
    case Type::PEEK:
      {
	return "Serial.peek();";
      }
    case Type::PRINT:
      {
	if(inputs().size() == 1)
	  return QString("Serial.print(%1);").arg(inputs().value(0));
	else
	  return QString("Serial.print(%1, %2);").
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
    case Type::READ:
      {
	return "Serial.read();";
      }
    case Type::READ_BYTES:
      {
	return QString("Serial.readBytes(%1, %2);").
	  arg(inputs().value(0)).
	  arg(inputs().value(1));
      }
    case Type::READ_BYTES_UNTIL:
      {
	return QString("Serial.readBytesUntil(%1, %2, %3);").
	  arg(inputs().value(0)).
	  arg(inputs().value(1)).
	  arg(inputs().value(2));
      }
    case Type::READ_STRING:
      {
	return "Serial.readString();";
      }
    case Type::READ_STRING_UNTIL:
      {
	return QString("Serial.readStringUntil(%1);").arg(inputs().value(0));
      }
    case Type::SET_TIMEOUT:
      {
	return QString("Serial.setTimeout(%1);").arg(inputs().value(0));
      }
    case Type::WRITE:
      {
	if(inputs().size() == 1)
	  return QString("Serial.write(%1);").arg(inputs().value(0));
	else
	  return QString("Serial.write(%1, %2);").
	    arg(inputs().value(0)).
	    arg(inputs().value(1));
      }
    default:
      {
	return "Serial.available();";
      }
    }
}

bool glitch_object_serial_arduino::hasInput(void) const
{
  switch(m_serialType)
    {
    case Type::BEGIN:
    case Type::FIND:
    case Type::FIND_UNTIL:
    case Type::PARSE_FLOAT:
    case Type::PARSE_INT:
    case Type::PRINT:
    case Type::PRINTLN:
    case Type::READ_BYTES:
    case Type::READ_BYTES_UNTIL:
    case Type::READ_STRING_UNTIL:
    case Type::SET_TIMEOUT:
    case Type::WRITE:
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
    case Type::AVAILABLE:
    case Type::AVAILABLE_FOR_WRITE:
    case Type::FIND:
    case Type::FIND_UNTIL:
    case Type::PARSE_FLOAT:
    case Type::PARSE_INT:
    case Type::PEEK:
    case Type::PRINT:
    case Type::PRINTLN:
    case Type::READ:
    case Type::READ_BYTES:
    case Type::READ_BYTES_UNTIL:
    case Type::READ_STRING:
    case Type::READ_STRING_UNTIL:
    case Type::WRITE:
      {
	return true;
      }
    default:
      {
	return false;
      }
    }
}

bool glitch_object_serial_arduino::isFullyWired(void) const
{
  switch(m_serialType)
    {
    case Type::BEGIN:
      {
	return inputs().size() >= 1;
      }
    case Type::FIND:
      {
	return inputs().size() >= 1;
      }
    case Type::FIND_UNTIL:
      {
	return inputs().size() >= 2;
      }
    case Type::PARSE_FLOAT:
      {
	return inputs().size() >= 2;
      }
    case Type::PARSE_INT:
      {
	return inputs().size() >= 2;
      }
    case Type::PRINT:
      {
	return inputs().size() >= 2;
      }
    case Type::PRINTLN:
      {
	return inputs().size() >= 2;
      }
    case Type::READ_BYTES:
      {
	return inputs().size() >= 2;
      }
    case Type::READ_BYTES_UNTIL:
      {
	return inputs().size() >= 3;
      }
    case Type::READ_STRING_UNTIL:
      {
	return inputs().size() >= 1;
      }
    case Type::SET_TIMEOUT:
      {
	return inputs().size() >= 1;
      }
    case Type::WRITE:
      {
      	return inputs().size() >= 2;
      }
    default:
      {
	return true;
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
  auto clone = new glitch_object_serial_arduino
    (serialTypeToString(m_serialType), parent);

  clone->cloneWires(m_wires);
  clone->m_properties = m_properties;
  clone->m_serialType = m_serialType;
  clone->m_ui.label->setText(m_ui.label->text());
  clone->resize(size());
  clone->setCanvasSettings(m_canvasSettings);
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
    (values.value("myoid").toLongLong(), parent);

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
	  string = serialTypeToString(stringToSerialType(string));
	  string = QString("Serial.%1()").arg(string);
	  m_properties[Properties::SERIAL_TYPE] = string.trimmed();
	}
    }

  setName(m_properties.value(Properties::SERIAL_TYPE).toString());
}
