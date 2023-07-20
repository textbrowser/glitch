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

#include "glitch-object-characters-arduino.h"

glitch_object_characters_arduino::glitch_object_characters_arduino
(const QString &charactersType, QWidget *parent):
  glitch_object_characters_arduino(1, parent)
{
  QString string("");

  switch(stringToCharactersType(charactersType))
    {
    case Type::IS_ALPHA_NUMERIC:
      {
	string = "isAlphaNumeric()";
	break;
      }
    case Type::IS_ASCII:
      {
	string = "isAscii()";
	break;
      }
    case Type::IS_CONTROL:
      {
	string = "isControl()";
	break;
      }
    case Type::IS_DIGIT:
      {
	string = "isDigit()";
	break;
      }
    case Type::IS_GRAPH:
      {
	string = "isGraph()";
	break;
      }
    case Type::IS_HEXADECIMAL_DIGIT:
      {
	string = "isHexadecimalDigit()";
	break;
      }
    case Type::IS_LOWER_CASE:
      {
	string = "isLowerCase()";
	break;
      }
    case Type::IS_PRINTABLE:
      {
	string = "isPrintable()";
	break;
      }
    case Type::IS_PUNCT:
      {
	string = "isPunct()";
	break;
      }
    case Type::IS_SPACE:
      {
	string = "isSpace()";
	break;
      }
    case Type::IS_UPPER_CASE:
      {
	string = "isUpperCase()";
	break;
      }
    case Type::IS_WHITESPACE:
      {
	string = "isWhitespace()";
	break;
      }
    default:
      {
	string = "isAlpha()";
	break;
      }
    }

  m_text = string;
  setName(m_text);
  setToolTip(description());
}

glitch_object_characters_arduino::glitch_object_characters_arduino
(const qint64 id, QWidget *parent):glitch_object_simple_text_arduino(id, parent)
{
  m_functionsList << "isAlpha()"
		  << "isAlphaNumeric()"
		  << "isAscii()"
		  << "isControl()"
		  << "isDigit()"
		  << "isGraph()"
		  << "isHexadecimalDigit()"
		  << "isLowerCase()"
		  << "isPrintable()"
		  << "isPunct()"
		  << "isSpace()"
		  << "isUpperCase()"
		  << "isWhitespace()";
  m_type = "arduino-characters";
  prepareContextMenu();
  setName(m_text);
}

glitch_object_characters_arduino::~glitch_object_characters_arduino()
{
}

QString glitch_object_characters_arduino::code(void) const
{
  return QString("%1(%2);").
    arg(charactersTypeToString()).
    arg(inputs().value(0));
}

bool glitch_object_characters_arduino::hasInput(void) const
{
  return true;
}

bool glitch_object_characters_arduino::hasOutput(void) const
{
  return true;
}

bool glitch_object_characters_arduino::isFullyWired(void) const
{
  return inputs().size() >= 1;
}

bool glitch_object_characters_arduino::shouldPrint(void) const
{
  return false;
}

glitch_object_characters_arduino *glitch_object_characters_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_characters_arduino
    (charactersTypeToString(), parent);

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

glitch_object_characters_arduino *glitch_object_characters_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_characters_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties(values.value("properties").toString().split('&'));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_characters_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["characters_type"] = m_text;
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_characters_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::COMPRESSED_WIDGET] = false;

  QString function("isAlpha()");

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("characters_type = "))
	{
	  string = string.mid(string.indexOf('=') + 1).toLower();
	  string.remove("\"");

	  if(string.contains("isalphanumeric"))
	    string = "isAlphaNumeric()";
	  else if(string.contains("isascii"))
	    string = "isAscii()";
	  else if(string.contains("iscontrol"))
	    string = "isControl()";
	  else if(string.contains("isdigit"))
	    string = "isDigit()";
	  else if(string.contains("isgraph"))
	    string = "isGraph()";
	  else if(string.contains("ishexadecimaldigit"))
	    string = "isHexadecimalDigit()";
	  else if(string.contains("islowercase"))
	    string = "isLowerCase()";
	  else if(string.contains("isprintable"))
	    string = "isPrintable()";
	  else if(string.contains("ispunct"))
	    string = "isPunct()";
	  else if(string.contains("isspace"))
	    string = "isSpace()";
	  else if(string.contains("isuppercase"))
	    string = "isUpperCase()";
	  else if(string.contains("iswhitespace"))
	    string = "isWhitespace()";
	  else
	    string = "isAlpha()";

	  function = string.trimmed();
	  break;
	}
    }

  m_text = function;
  setName(m_text);
  setToolTip(description());
}

void glitch_object_characters_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object_simple_text_arduino::setProperty(property, value);

  switch(property)
    {
    case Properties::CHARACTERS_TYPE:
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
