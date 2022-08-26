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

#include "glitch-object-bits-and-bytes-arduino.h"

glitch_object_bits_and_bytes_arduino::glitch_object_bits_and_bytes_arduino
(const QString &babType, QWidget *parent):
  glitch_object_bits_and_bytes_arduino(1, parent)
{
  m_babType = stringToType(babType);

  switch(m_babType)
    {
    case BIT:
      {
	m_ui.label->setText("bit()");
	break;
      }
    case BIT_CLEAR:
      {
	m_ui.label->setText("bitClear()");
	break;
      }
    case BIT_READ:
      {
	m_ui.label->setText("bitRead()");
	break;
      }
    case BIT_SET:
      {
	m_ui.label->setText("bitSet()");
	break;
      }
    case BIT_WRITE:
      {
	m_ui.label->setText("bitWrite()");
	break;
      }
    case HIGH_BYTE:
      {
	m_ui.label->setText("highByte()");
	break;
      }
    case LOW_BYTE:
      {
	m_ui.label->setText("lowByte()");
	break;
      }
    default:
      {
	m_ui.label->setText("bit()");
	break;
      }
    }

  setName(m_ui.label->text());
}

glitch_object_bits_and_bytes_arduino::glitch_object_bits_and_bytes_arduino
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_type = "arduino-bitsandbytes";
  m_ui.setupUi(this);
  prepareContextMenu();
  setName(m_ui.label->text());
}

glitch_object_bits_and_bytes_arduino::~glitch_object_bits_and_bytes_arduino()
{
}

QString glitch_object_bits_and_bytes_arduino::code(void) const
{
  switch(m_babType)
    {
    case Type::BIT:
      {
	return QString("bit(%1);").arg(inputs().value(0));
      }
    case Type::BIT_CLEAR:
      {
	return QString("bitClear(%1, %2);").
	  arg(inputs().value(0)).
	  arg(inputs().value(1));
      }
    case Type::BIT_READ:
      {
	return QString("bitRead(%1, %2);").
	  arg(inputs().value(0)).
	  arg(inputs().value(1));
      }
    case Type::BIT_SET:
      {
	return QString("bitSet(%1, %2);").
	  arg(inputs().value(0)).
	  arg(inputs().value(1));
      }
    case Type::BIT_WRITE:
      {
	return QString("bitWrite(%1, %2, %3);").
	  arg(inputs().value(0)).
	  arg(inputs().value(1)).
	  arg(inputs().value(2));
      }
    case Type::HIGH_BYTE:
      {
	return QString("highByte(%1);").arg(inputs().value(0));
      }
    case Type::LOW_BYTE:
      {
	return QString("lowByte(%1);").arg(inputs().value(0));
      }
    default:
      {
	return QString("bit(%1);").arg(inputs().value(0));
      }
    }
}

bool glitch_object_bits_and_bytes_arduino::hasInput(void) const
{
  return true;
}

bool glitch_object_bits_and_bytes_arduino::hasOutput(void) const
{
  if(m_babType == Type::BIT_SET || m_babType == Type::BIT_WRITE)
    return false;
  else
    return true;
}

bool glitch_object_bits_and_bytes_arduino::isFullyWired(void) const
{
  switch(m_babType)
    {
    case Type::BIT:
      {
	return inputs().size() >= 1;
      }
    case Type::BIT_CLEAR:
      {
	return inputs().size() >= 2;
      }
    case Type::BIT_READ:
      {
	return inputs().size() >= 2;
      }
    case Type::BIT_SET:
      {
	return inputs().size() >= 2;
      }
    case Type::BIT_WRITE:
      {
	return inputs().size() >= 3;
      }
    case Type::HIGH_BYTE:
      {
	return inputs().size() >= 1;
      }
    case Type::LOW_BYTE:
      {
	return inputs().size() >= 1;
      }
    default:
      {
	return inputs().size() >= 1;
      }
    }
}

bool glitch_object_bits_and_bytes_arduino::shouldPrint(void) const
{
  switch(m_babType)
    {
    case Type::BIT_SET:
    case Type::BIT_WRITE:
      {
	return true;
      }
    default:
      {
	return false;
      }
    }
}

glitch_object_bits_and_bytes_arduino *glitch_object_bits_and_bytes_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_bits_and_bytes_arduino(typeToString(), parent);

  clone->cloneWires(m_wires);
  clone->m_babType = m_babType;
  clone->m_properties = m_properties;
  clone->m_ui.label->setText(m_ui.label->text());
  clone->resize(size());
  clone->setCanvasSettings(m_canvasSettings);
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_bits_and_bytes_arduino *glitch_object_bits_and_bytes_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_bits_and_bytes_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties(values.value("properties").toString().split('&'));
  object->setStyleSheet(values.value("stylesheet").toString());
  object->m_babType = stringToType
    (object->m_properties.value(Properties::BITS_AND_BYTES_TYPE).toString());
  object->m_ui.label->setText
    (object->m_properties.value(Properties::BITS_AND_BYTES_TYPE).toString());
  return object;
}

void glitch_object_bits_and_bytes_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_bits_and_bytes_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["bab_type"] = m_ui.label->text().trimmed();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_bits_and_bytes_arduino::setProperties
(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::BITS_AND_BYTES_TYPE] = "bit()";

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("bab_type = "))
	{
	  string = string.mid(string.indexOf('=') + 1).toLower();
	  string.remove("\"");

	  if(string.contains("bitclear"))
	    string = "bitClear()";
	  else if(string.contains("bitread"))
	    string = "bitRead()";
	  else if(string.contains("bitset"))
	    string = "bitSet()";
	  else if(string.contains("bitwrite"))
	    string = "bitWrite()";
	  else if(string.contains("highbyte"))
	    string = "highByte()";
	  else if(string.contains("lowbyte"))
	    string = "lowByte()";
	  else
	    string = "bit()";

	  m_properties[Properties::BITS_AND_BYTES_TYPE] = string.trimmed();
	}
    }

  setName(m_properties.value(Properties::BITS_AND_BYTES_TYPE).toString());
}
