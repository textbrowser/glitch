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

#include "glitch-object-conversion-arduino.h"

glitch_object_conversion_arduino::glitch_object_conversion_arduino
(QWidget *parent):glitch_object_conversion_arduino(1, parent)
{
}

glitch_object_conversion_arduino::
glitch_object_conversion_arduino
(const QString &conversionType, QWidget *parent):
  glitch_object_conversion_arduino(1, parent)
{
  setConversionType(conversionType);
  setToolTip(description());
}

glitch_object_conversion_arduino::glitch_object_conversion_arduino
(const qint64 id, QWidget *parent):glitch_object_simple_text_arduino(id, parent)
{
  m_functionsList << "(uint8_t)"
		  << "(uint16_t)"
		  << "(uint32_t)"
		  << "(unsigned int)"
		  << "(unsigned long)"
		  << "byte()"
		  << "char()"
		  << "float()"
		  << "int()"
		  << "long()"
		  << "word()";
  m_type = "arduino-conversion";
  prepareContextMenu();
  setConversionType(ConversionTypes::UNSIGNED_INT);
}

glitch_object_conversion_arduino::~glitch_object_conversion_arduino()
{
}

QString glitch_object_conversion_arduino::code(void) const
{
  if(!property(Properties::GENERATE_SOURCE).toBool())
    return "";

  if(m_text == "(uint8_t)" ||
     m_text == "(uint16_t)" ||
     m_text == "(uint32_t)" ||
     m_text == "(unsigned int)" ||
     m_text == "(unsigned long)")
    return QString("%1 (%2)").arg(m_text).arg(inputs().value(0));
  else
    return QString("%1(%2)").
      arg(QString(m_text).remove("()")).
      arg(inputs().value(0));
}

QString glitch_object_conversion_arduino::description(void) const
{
  if(m_text == "(uint8_t)")
    return "(uint8_t) x";
  else if(m_text == "(uint16_t)")
    return "(uint16_t) x";
  else if(m_text == "(uint32_t)")
    return "(uint32_t) x";
  else if(m_text == "(unsigned int)")
    return "(unsigned int) x";
  else if(m_text == "(unsigned long)")
    return "(unsigned long) x";
  else if(m_text == "byte()")
    return "byte byte(x)";
  else if(m_text == "char()")
    return "char char(x)";
  else if(m_text == "float()")
    return "float float(x)";
  else if(m_text == "int()")
    return "int int(x)";
  else if(m_text == "long()")
    return "long long(x)";
  else if(m_text == "word()")
    return "uint16_t word(byte h, byte l) / uint16_t word(uint16_t w)";
  else
    return "";
}

bool glitch_object_conversion_arduino::hasInput(void) const
{
  return true;
}

bool glitch_object_conversion_arduino::hasOutput(void) const
{
  return true;
}

bool glitch_object_conversion_arduino::isFullyWired(void) const
{
  return inputs().size() >= 1;
}

bool glitch_object_conversion_arduino::shouldPrint(void) const
{
  return false;
}

glitch_object_conversion_arduino *glitch_object_conversion_arduino::clone
(QWidget *parent) const
{
  auto clone = new glitch_object_conversion_arduino(parent);

  clone->cloneWires(m_copiedConnectionsPositions);
  clone->cloneWires(m_wires);
  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->m_text = m_text;
  clone->resize(size());
  clone->setCanvasSettings(m_canvasSettings);
  clone->setConversionType(m_text);
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_conversion_arduino *glitch_object_conversion_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_conversion_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties
    (splitPropertiesRegularExpression(values.value("properties")));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_conversion_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["conversion"] = m_text;
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_conversion_arduino::setConversionType
(const ConversionTypes conversionType)
{
  switch(conversionType)
    {
    case ConversionTypes::BYTE:
      {
	m_text = "byte()";
	break;
      }
    case ConversionTypes::CHAR:
      {
	m_text = "char()";
	break;
      }
    case ConversionTypes::FLOAT:
      {
	m_text = "float()";
	break;
      }
    case ConversionTypes::INT:
      {
	m_text = "int()";
	break;
      }
    case ConversionTypes::LONG:
      {
	m_text = "long()";
	break;
      }
    case ConversionTypes::UINT8_T:
      {
	m_text = "(uint8_t)";
	break;
      }
    case ConversionTypes::UINT16_T:
      {
	m_text = "(uint16_t)";
	break;
      }
    case ConversionTypes::UINT32_T:
      {
	m_text = "(uint32_t)";
	break;
      }
    case ConversionTypes::UNSIGNED_LONG:
      {
	m_text = "(unsigned long)";
	break;
      }
    case ConversionTypes::WORD:
      {
	m_text = "word()";
	break;
      }
    default:
      {
	m_text = "(unsigned int)";
	break;
      }
    }

  setName(m_text);
}

void glitch_object_conversion_arduino::setConversionType(const QString &ct)
{
  auto conversionType(ct.toLower());

  if(conversionType.contains("(uint8_t)"))
    setConversionType(ConversionTypes::UINT8_T);
  else if(conversionType.contains("(uint16_t)"))
    setConversionType(ConversionTypes::UINT16_T);
  else if(conversionType.contains("(uint32_t)"))
    setConversionType(ConversionTypes::UINT32_T);
  else if(conversionType.contains("(unsigned int)"))
    setConversionType(ConversionTypes::UNSIGNED_INT);
  else if(conversionType.contains("(unsigned long)"))
    setConversionType(ConversionTypes::UNSIGNED_LONG);
  else if(conversionType.contains("byte"))
    setConversionType(ConversionTypes::BYTE);
  else if(conversionType.contains("char"))
    setConversionType(ConversionTypes::CHAR);
  else if(conversionType.contains("float"))
    setConversionType(ConversionTypes::FLOAT);
  else if(conversionType.contains("int"))
    setConversionType(ConversionTypes::INT);
  else if(conversionType.contains("long"))
    setConversionType(ConversionTypes::LONG);
  else if(conversionType.contains("word"))
    setConversionType(ConversionTypes::WORD);
  else
    setConversionType(ConversionTypes::UNSIGNED_INT);
}

void glitch_object_conversion_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::COMPRESSED_WIDGET] = false;

  QString function("(unsigned int)");

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("conversion = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  function = string.trimmed();
	  break;
	}
    }

  m_text = function;
  setConversionType(m_text);
  setToolTip(description());
}

void glitch_object_conversion_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object_simple_text_arduino::setProperty(property, value);

  switch(property)
    {
    case Properties::CONVERSION:
      {
	m_text = value.toString();
	setConversionType(m_text);
	break;
      }
    default:
      {
	break;
      }
    }

  setToolTip(description());
}
