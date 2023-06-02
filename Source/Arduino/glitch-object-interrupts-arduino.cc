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

#include "glitch-object-interrupts-arduino.h"

glitch_object_interrupts_arduino::glitch_object_interrupts_arduino
(const QString &interruptsType, QWidget *parent):
  glitch_object_interrupts_arduino(1, parent)
{
  m_text = QString("%1()").arg
    (interruptsTypeToString(stringToInterruptsType(interruptsType)));
  setName(m_text);
}

glitch_object_interrupts_arduino::glitch_object_interrupts_arduino
(const qint64 id, QWidget *parent):glitch_object_simple_text_arduino(id, parent)
{
  m_functionsList << "attachInterrupt()"
		  << "detachInterrupt()"
		  << "interrupts()"
		  << "noInterrupts()";
  m_type = "arduino-interrupts";
  prepareContextMenu();
  setName(m_text);
}

glitch_object_interrupts_arduino::~glitch_object_interrupts_arduino()
{
}

QString glitch_object_interrupts_arduino::code(void) const
{
  switch(stringToInterruptsType(m_text))
    {
    case Type::ATTACH_INTERRUPT:
    default:
      {
	return QString("attachInterrupt(%1, %2, %3);").
	  arg(inputs().value(0)).
	  arg(inputs().value(1)).
	  arg(inputs().value(2));
      }
    case Type::DETACH_INTERRUPT:
      {
	return QString("detachInterrupt(%1);").arg(inputs().value(0));
      }
    case Type::INTERRUPTS:
      {
	return "interrupts()";
      }
    case Type::NO_INTERRUPTS:
      {
	return "noInterrupts()";
      }
    }
}

bool glitch_object_interrupts_arduino::hasInput(void) const
{
  switch(stringToInterruptsType(m_text))
    {
    case Type::ATTACH_INTERRUPT:
    case Type::DETACH_INTERRUPT:
      {
	return true;
      }
    default:
      {
	return false;
      }
    }
}

bool glitch_object_interrupts_arduino::hasOutput(void) const
{
  return false;
}

bool glitch_object_interrupts_arduino::isFullyWired(void) const
{
  switch(stringToInterruptsType(m_text))
    {
    case Type::ATTACH_INTERRUPT:
      {
	return inputs().size() >= 3;
      }
    case Type::DETACH_INTERRUPT:
      {
	return inputs().size() >= 1;
      }
    default:
      {
	return true;
      }
    }
}

bool glitch_object_interrupts_arduino::shouldPrint(void) const
{
  return true;
}

glitch_object_interrupts_arduino *glitch_object_interrupts_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_interrupts_arduino
    (interruptsTypeToString(stringToInterruptsType(m_text)), parent);

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

glitch_object_interrupts_arduino *glitch_object_interrupts_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_interrupts_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties(values.value("properties").toString().split('&'));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_interrupts_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["interrupts_type"] = m_text;
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_interrupts_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::COMPRESSED_WIDGET] = false;

  QString function("attachInterrupt()");

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("interrupts_type = "))
	{
	  string = string.mid(string.indexOf('=') + 1).toLower();
	  string.remove("\"");
	  string = interruptsTypeToString(stringToInterruptsType(string));
	  string = QString("%1()").arg(string);
	  function = string.trimmed();
	  break;
	}
    }

  m_text = function;
  setName(m_text);
}

void glitch_object_interrupts_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object_simple_text_arduino::setProperty(property, value);

  switch(property)
    {
    case Properties::INTERRUPTS_TYPE:
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
