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

  switch(m_ioType)
    {
    case Type::READ:
      {
	m_ui.label->setText("digitalRead()");
	break;
      }
    case Type::WRITE:
      {
	m_ui.label->setText("digitalWrite()");
	break;
      }
    default:
      {
	m_ui.label->setText("pinMode()");
	break;
      }
    }

  setName(m_ui.label->text());
}

glitch_object_digital_io_arduino::glitch_object_digital_io_arduino
(const quint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_type = "arduino-digitalio";
  m_ui.setupUi(this);
  prepareContextMenu();
  setName(m_ui.label->text());
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
	return QString("digitalRead(%1)").arg(inputs().value(0));
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
  return false;
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

  clone->m_ioType = m_ioType;
  clone->m_properties = m_properties;
  clone->m_ui.label->setText(m_ui.label->text());
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
    (values.value("myoid").toULongLong(), parent);

  object->setProperties(values.value("properties").toString().split('&'));
  object->setStyleSheet(values.value("stylesheet").toString());
  object->m_ioType = stringToIOType
    (object->m_properties.value(Properties::DIGITAL_IO_TYPE).toString());
  object->m_ui.label->setText
    (object->m_properties.value(Properties::DIGITAL_IO_TYPE).toString());
  return object;
}

void glitch_object_digital_io_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_digital_io_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["io_type"] = m_ui.label->text().trimmed();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_digital_io_arduino::setProperties(const QStringList &list)
{
  /*
  ** Redundancies.
  */

  m_properties[Properties::DIGITAL_IO_TYPE] = "digitalRead()";

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("io_type = "))
	{
	  string = string.mid(string.indexOf('=') + 1).toLower();
	  string.remove("\"");

	  if(string.contains("read"))
	    string = "digitalRead()";
	  else if(string.contains("write"))
	    string = "digitalWrite()";
	  else
	    string = "pinMode()";

	  m_properties[Properties::DIGITAL_IO_TYPE] = string.trimmed();
	}
    }

  setName(m_properties.value(Properties::DIGITAL_IO_TYPE).toString());
}