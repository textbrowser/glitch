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

#ifndef _glitch_object_random_arduino_h_
#define _glitch_object_random_arduino_h_

#include "glitch-object-simple-text-arduino.h"

class glitch_object_random_arduino: public glitch_object_simple_text_arduino
{
  Q_OBJECT

 public:
  glitch_object_random_arduino(const QString &randomType, QWidget *parent);
  ~glitch_object_random_arduino();

  QSize minimumSizeHint(void) const
  {
    return QSize(125, 50);
  }

  QString code(void) const;
  bool hasInput(void) const;
  bool hasOutput(void) const;
  bool isFullyWired(void) const;
  bool shouldPrint(void) const;
  glitch_object_random_arduino *clone(QWidget *parent) const;
  static glitch_object_random_arduino *createFromValues
    (const QMap<QString, QVariant> &values, QString &error, QWidget *parent);
  void save(const QSqlDatabase &db, QString &error);

 private:
  enum Type
  {
    RANDOM = 0,
    RANDOM_SEED
  };

  glitch_object_random_arduino(const qint64 id, QWidget *parent);
  Type m_randomType;

  QString randomTypeToString(void) const
  {
    switch(m_randomType)
      {
      case Type::RANDOM_SEED:
	{
	  return "randomSeed";
	}
      default:
	{
	  return "random";
	}
      }
  }

  static Type stringToRandomType(const QString &s)
  {
    auto string(s.toLower());

    if(string.contains("randomseed"))
      return Type::RANDOM_SEED;
    else
      return Type::RANDOM;
  }

  void setProperties(const QStringList &list);
  void setProperty(const Properties property, const QVariant &value);
};

#endif
