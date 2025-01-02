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

#include "glitch-object-serial-arduino.h"

glitch_object_serial_arduino::glitch_object_serial_arduino
(const QString &serialType, QWidget *parent):
  glitch_object_serial_arduino(1, parent)
{
  m_text = QString("Serial.%1()").arg
    (serialTypeToString(stringToSerialType(serialType)));
  setName(m_text);
  setToolTip(description());
}

glitch_object_serial_arduino::glitch_object_serial_arduino
(const qint64 id, QWidget *parent):glitch_object_simple_text_arduino(id, parent)
{
  m_functionsList << "Serial.available()"
		  << "Serial.availableForWrite()"
		  << "Serial.begin()"
		  << "Serial.end()"
		  << "Serial.find()"
		  << "Serial.findUntil()"
		  << "Serial.flush()"
		  << "Serial.parseFloat()"
		  << "Serial.parseInt()"
		  << "Serial.peek()"
		  << "Serial.print()"
		  << "Serial.println()"
		  << "Serial.read()"
		  << "Serial.readBytes()"
		  << "Serial.readBytesUntil()"
		  << "Serial.readString()"
		  << "Serial.readStringUntil()"
		  << "Serial.setTimeout()"
		  << "Serial.write()";
  m_type = "arduino-serial";
  prepareContextMenu();
  setName(m_text);
}

glitch_object_serial_arduino::~glitch_object_serial_arduino()
{
}

QString glitch_object_serial_arduino::code(void) const
{
  if(!property(Properties::GENERATE_SOURCE).toBool())
    return "";

  switch(stringToSerialType(m_text))
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
  switch(stringToSerialType(m_text))
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
  switch(stringToSerialType(m_text))
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
  switch(stringToSerialType(m_text))
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
    (serialTypeToString(stringToSerialType(m_text)), parent);

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

glitch_object_serial_arduino *glitch_object_serial_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_serial_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties(splitPropertiesAmpersand(values.value("properties")));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_serial_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["serial_type"] = m_text;
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_serial_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::COMPRESSED_WIDGET] = false;

  QString function("Serial.available()");

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("serial_type = "))
	{
	  string = string.mid(string.indexOf('=') + 1).toLower();
	  string.remove("\"");
	  string = serialTypeToString(stringToSerialType(string));
	  string = QString("Serial.%1()").arg(string);
	  function = string.trimmed();
	  break;
	}
    }

  m_text = function;
  setName(m_text);
  setToolTip(description());
}

void glitch_object_serial_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object_simple_text_arduino::setProperty(property, value);

  switch(property)
    {
    case Properties::SERIAL_TYPE:
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
