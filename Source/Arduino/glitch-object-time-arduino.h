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

#ifndef _glitch_object_time_arduino_h_
#define _glitch_object_time_arduino_h_

#include "glitch-object.h"
#include "ui_glitch-object-time-arduino.h"

class glitch_object_time_arduino: public glitch_object
{
  Q_OBJECT

 public:
  glitch_object_time_arduino(const QString &timeType, QWidget *parent);
  glitch_object_time_arduino(const quint64 id, QWidget *parent);
  ~glitch_object_time_arduino();
  QString code(void) const;
  bool hasInput(void) const;
  bool hasOutput(void) const;
  bool isFullyWired(void) const;
  bool shouldPrint(void) const;
  glitch_object_time_arduino *clone(QWidget *parent) const;
  static glitch_object_time_arduino *createFromValues
    (const QMap<QString, QVariant> &values, QString &error, QWidget *parent);
  void addActions(QMenu &menu);
  void save(const QSqlDatabase &db, QString &error);

 private:
  enum Type
  {
    DELAY = 0,
    DELAY_MICROSECONDS,
    MICROS,
    MILLIS
  };

  Type m_timeType;
  Ui_glitch_object_time_arduino m_ui;

  QString timeTypeToString(void) const
  {
    switch(m_timeType)
      {
      case Type::DELAY:
	{
	  return "delay";
	}
      case Type::DELAY_MICROSECONDS:
	{
	  return "delayMicroseconds";
	}
      case Type::MICROS:
	{
	  return "micros";
	}
      case Type::MILLIS:
	{
	  return "millis";
	}
      default:
	{
	  return "delay";
	}
      }
  }

  static Type stringToTimeType(const QString &s)
  {
    auto string(s.toLower());

    if(string.contains("delaymicroseconds"))
      return Type::DELAY_MICROSECONDS;
    else if(string.contains("delay"))
      return Type::DELAY;
    else if(string.contains("micros"))
      return Type::MICROS;
    else if(string.contains("millis"))
      return Type::MILLIS;
    else
      return Type::DELAY;
  }

  void setProperties(const QStringList &list);
};

#endif
