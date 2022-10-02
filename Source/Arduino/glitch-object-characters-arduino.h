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

#ifndef _glitch_object_characters_arduino_h_
#define _glitch_object_characters_arduino_h_

#include "glitch-object.h"
#include "ui_glitch-object-characters-arduino.h"

class glitch_object_characters_arduino: public glitch_object
{
  Q_OBJECT

 public:
  glitch_object_characters_arduino
    (const QString &charactersType, QWidget *parent);
  glitch_object_characters_arduino(const qint64 id, QWidget *parent);
  ~glitch_object_characters_arduino();
  QString code(void) const;
  bool hasInput(void) const;
  bool hasOutput(void) const;
  bool isFullyWired(void) const;
  bool shouldPrint(void) const;
  glitch_object_characters_arduino *clone(QWidget *parent) const;
  static glitch_object_characters_arduino *createFromValues
    (const QMap<QString, QVariant> &values, QString &error, QWidget *parent);
  void addActions(QMenu &menu);
  void save(const QSqlDatabase &db, QString &error);

 private:
  enum Type
  {
    IS_ALPHA = 0,
    IS_ALPHA_NUMERIC,
    IS_ASCII,
    IS_CONTROL,
    IS_DIGIT,
    IS_GRAPH,
    IS_HEXADECIMAL_DIGIT,
    IS_LOWER_CASE,
    IS_PRINTABLE,
    IS_PUNCT,
    IS_SPACE,
    IS_UPPER_CASE,
    IS_WHITESPACE
  };

  Type m_charactersType;
  Ui_glitch_object_characters_arduino m_ui;

  QString charactersTypeToString(void) const
  {
    switch(m_charactersType)
      {
      case Type::IS_ALPHA_NUMERIC:
	{
	  return "isAlphaNumeric";
	}
      case Type::IS_ASCII:
	{
	  return "isAscii";
	}
      case Type::IS_CONTROL:
	{
	  return "isControl";
	}
      case Type::IS_DIGIT:
	{
	  return "isDigit";
	}
      case Type::IS_GRAPH:
	{
	  return "isGraph";
	}
      case Type::IS_HEXADECIMAL_DIGIT:
	{
	  return "isHexadecimalDigit";
	}
      case Type::IS_LOWER_CASE:
	{
	  return "isLowerCase";
	}
      case Type::IS_PRINTABLE:
	{
	  return "isPrintable";
	}
      case Type::IS_PUNCT:
	{
	  return "isPunct";
	}
      case Type::IS_SPACE:
	{
	  return "isSpace";
	}
      case Type::IS_UPPER_CASE:
	{
	  return "isUpperCase";
	}
      case Type::IS_WHITESPACE:
	{
	  return "isWhitespace";
	}
      default:
	{
	  return "isAlpha";
	}
      }
  }

  static Type stringToCharactersType(const QString &s)
  {
    auto string(s.toLower());

    if(string.contains("isalphanumeric"))
      return Type::IS_ALPHA_NUMERIC;
    else if(string.contains("isascii"))
      return Type::IS_ASCII;
    else if(string.contains("iscontrol"))
      return Type::IS_CONTROL;
    else if(string.contains("isdigit"))
      return Type::IS_DIGIT;
    else if(string.contains("isgraph"))
      return Type::IS_GRAPH;
    else if(string.contains("ishexadecimaldigit"))
      return Type::IS_HEXADECIMAL_DIGIT;
    else if(string.contains("islowercase"))
      return Type::IS_LOWER_CASE;
    else if(string.contains("isprintable"))
      return Type::IS_PRINTABLE;
    else if(string.contains("ispunct"))
      return Type::IS_PUNCT;
    else if(string.contains("isspace"))
      return Type::IS_SPACE;
    else if(string.contains("isuppercase"))
      return Type::IS_UPPER_CASE;
    else if(string.contains("iswhitespace"))
      return Type::IS_WHITESPACE;
    else
      return Type::IS_ALPHA;
  }

  void setProperties(const QStringList &list);
  void setProperty(const Properties property, const QVariant &value);

 private slots:
  void slotFunctionChanged(void);
};

#endif
