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

#ifndef _glitch_object_trigonometry_arduino_h_
#define _glitch_object_trigonometry_arduino_h_

#include "glitch-object-simple-text-arduino.h"

class glitch_object_trigonometry_arduino:
  public glitch_object_simple_text_arduino
{
  Q_OBJECT

 public:
  glitch_object_trigonometry_arduino
    (const QString &trigonometryType, QWidget *parent);
  ~glitch_object_trigonometry_arduino();
  QString code(void) const;
  bool hasInput(void) const;
  bool hasOutput(void) const;
  bool isFullyWired(void) const;
  bool shouldPrint(void) const;
  glitch_object_trigonometry_arduino *clone(QWidget *parent) const;
  static glitch_object_trigonometry_arduino *createFromValues
    (const QMap<QString, QVariant> &values, QString &error, QWidget *parent);
  void save(const QSqlDatabase &db, QString &error);

 private:
  enum class Types
  {
    COS = 0,
    SIN,
    TAN
  };

  glitch_object_trigonometry_arduino(const qint64 id, QWidget *parent);

  QString description(void) const
  {
    switch(stringToTrigonometryType(m_text))
      {
      case Types::COS:
      default:
	{
	  return "double cos(float rad)";
	}
      case Types::SIN:
	{
	  return "double sin(float rad)";
	}
      case Types::TAN:
	{
	  return "double tan(float rad)";
	}
      }
  }

  QString trigonometryTypeToString(void) const
  {
    switch(stringToTrigonometryType(m_text))
      {
      case Types::COS:
	{
	  return "cos";
	}
      case Types::SIN:
	{
	  return "sin";
	}
      case Types::TAN:
	{
	  return "tan";
	}
      default:
	{
	  return "cos";
	}
      }
  }

  static Types stringToTrigonometryType(const QString &s)
  {
    auto const string(s.toLower());

    if(string.contains("cos"))
      return Types::COS;
    else if(string.contains("sin"))
      return Types::SIN;
    else if(string.contains("tan"))
      return Types::TAN;
    else
      return Types::COS;
  }

  void setProperties(const QStringList &list);
  void setProperty(const Properties property, const QVariant &value);
};

#endif
