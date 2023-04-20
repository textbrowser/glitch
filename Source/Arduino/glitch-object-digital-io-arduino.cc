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

#include "glitch-object-digital-io-arduino.h"

glitch_object_digital_io_arduino::glitch_object_digital_io_arduino
(const QString &ioType, QWidget *parent):
  glitch_object_digital_io_arduino(1, parent)
{
  m_ioType = stringToIOType(ioType);

  QString string("");

  switch(m_ioType)
    {
    case Type::READ:
      {
	string = "digitalRead()";
	break;
      }
    case Type::WRITE:
      {
	string = "digitalWrite()";
	break;
      }
    default:
      {
	string = "pinMode()";
	break;
      }
    }

  m_properties[Properties::DIGITAL_IO_TYPE] = string;
  m_text = string;
  setName(m_text);
}

glitch_object_digital_io_arduino::glitch_object_digital_io_arduino
(const qint64 id, QWidget *parent):glitch_object_simple_text_arduino(id, parent)
{
  m_type = "arduino-digitalio";
  prepareContextMenu();
  setName(m_text);
}

glitch_object_digital_io_arduino::~glitch_object_digital_io_arduino()
{
}

QString glitch_object_digital_io_arduino::code(void) const
{
  switch(m_ioType)
    {
    case Type::READ:
      {
	return QString("digitalRead(%1);").arg(inputs().value(0));
      }
    case Type::WRITE:
      {
	return QString("digitalWrite(%1, %2);").
	  arg(inputs().value(0)).
	  arg(inputs().value(1));
      }
    default:
      {
	return QString("pinMode(%1, %2);").
	  arg(inputs().value(0)).
	  arg(inputs().value(1));
      }
    }
}

bool glitch_object_digital_io_arduino::hasInput(void) const
{
  return true;
}

bool glitch_object_digital_io_arduino::hasOutput(void) const
{
  if(m_ioType == Type::READ)
    return true;
  else
    return false;
}

bool glitch_object_digital_io_arduino::isFullyWired(void) const
{
  switch(m_ioType)
    {
    case Type::READ:
      {
	return inputs().size() >= 1;
      }
    case Type::WRITE:
      {
	return inputs().size() >= 2;
      }
    default:
      {
	return inputs().size() >= 2;
      }
    }
}

bool glitch_object_digital_io_arduino::shouldPrint(void) const
{
  switch(m_ioType)
    {
    case Type::PIN_MODE:
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

glitch_object_digital_io_arduino *glitch_object_digital_io_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_digital_io_arduino(ioTypeToString(), parent);

  clone->cloneWires(m_copiedConnectionsPositions);
  clone->cloneWires(m_wires);
  clone->m_ioType = m_ioType;
  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->m_text = m_text;
  clone->resize(size());
  clone->setCanvasSettings(m_canvasSettings);
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_digital_io_arduino *glitch_object_digital_io_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_digital_io_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties(values.value("properties").toString().split('&'));
  object->setStyleSheet(values.value("stylesheet").toString());
  object->m_ioType = stringToIOType
    (object->m_properties.value(Properties::DIGITAL_IO_TYPE).toString());
  return object;
}

void glitch_object_digital_io_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["io_type"] = m_text;
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_digital_io_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::COMPRESSED_WIDGET] = false;
  m_properties[Properties::DIGITAL_IO_TYPE] = "digitalRead()";
  m_properties[Properties::TRANSPARENT] = true;

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("io_type = "))
	{
	  string = string.mid(string.indexOf('=') + 1).toLower();
	  string.remove("\"");

	  if(string.contains("pin"))
	    string = "pinMode()";
	  else if(string.contains("write"))
	    string = "digitalWrite()";
	  else
	    string = "digitalRead()";

	  m_properties[Properties::DIGITAL_IO_TYPE] = string.trimmed();
	}
    }

  m_ioType = stringToIOType
    (m_properties.value(Properties::DIGITAL_IO_TYPE).toString());
  m_text = m_properties.value(Properties::DIGITAL_IO_TYPE).toString();
  setName(m_properties.value(Properties::DIGITAL_IO_TYPE).toString());
}

void glitch_object_digital_io_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::DIGITAL_IO_TYPE:
      {
	m_ioType = stringToIOType(value.toString());
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
