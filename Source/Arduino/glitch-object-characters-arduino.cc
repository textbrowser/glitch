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

#include "glitch-object-characters-arduino.h"

glitch_object_characters_arduino::glitch_object_characters_arduino
(const QString &charactersType, QWidget *parent):
  glitch_object_characters_arduino(1, parent)
{
  m_charactersType = stringToCharactersType(charactersType);

  switch(m_charactersType)
    {
    case Type::IS_ALPHA_NUMERIC:
      {
	m_ui.label->setText("isAlphaNumeric()");
	break;
      }
    case Type::IS_ASCII:
      {
	m_ui.label->setText("isAscii()");
	break;
      }
    case Type::IS_CONTROL:
      {
	m_ui.label->setText("isControl()");
	break;
      }
    case Type::IS_DIGIT:
      {
	m_ui.label->setText("isDigit()");
	break;
      }
    case Type::IS_HEXADECIMAL_DIGIT:
      {
	m_ui.label->setText("isHexadecimalDigit()");
	break;
      }
    case Type::IS_LOWER_CASE:
      {
	m_ui.label->setText("isLowerCase()");
	break;
      }
    case Type::IS_PRINTABLE:
      {
	m_ui.label->setText("isPrintable()");
	break;
      }
    case Type::IS_PUNCT:
      {
	m_ui.label->setText("isPunct()");
	break;
      }
    case Type::IS_SPACE:
      {
	m_ui.label->setText("isSpace()");
	break;
      }
    case Type::IS_UPPER_CASE:
      {
	m_ui.label->setText("isUpperCase()");
	break;
      }
    case Type::IS_WHITESPACE:
      {
	m_ui.label->setText("isWhitespace()");
	break;
      }
    default:
      {
	m_ui.label->setText("isAlpha()");
	break;
      }
    }

  setName(m_ui.label->text());
}

glitch_object_characters_arduino::glitch_object_characters_arduino
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_type = "arduino-characters";
  m_ui.setupUi(this);
  prepareContextMenu();
  setName(m_ui.label->text());
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
  switch(m_charactersType)
    {
    default:
      {
	return inputs().size() >= 1;
      }
    }
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

  clone->cloneWires(m_wires);
  clone->m_properties = m_properties;
  clone->m_charactersType = m_charactersType;
  clone->m_ui.label->setText(m_ui.label->text());
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
  object->m_charactersType = stringToCharactersType
    (object->m_properties.value(Properties::CHARACTERS_TYPE).toString());
  object->m_ui.label->setText
    (object->m_properties.value(Properties::CHARACTERS_TYPE).toString());
  return object;
}

void glitch_object_characters_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_characters_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["characters_type"] = m_ui.label->text().trimmed();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_characters_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::CHARACTERS_TYPE] = "isAlpha()";

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

	  m_properties[Properties::CHARACTERS_TYPE] = string.trimmed();
	}
    }

  setName(m_properties.value(Properties::CHARACTERS_TYPE).toString());
}
