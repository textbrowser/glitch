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

#ifndef _glitch_object_mathematics_arduino_h_
#define _glitch_object_mathematics_arduino_h_

#include "glitch-object-simple-text-arduino.h"

class glitch_object_mathematics_arduino:
  public glitch_object_simple_text_arduino
{
  Q_OBJECT

 public:
  glitch_object_mathematics_arduino
    (const QString &mathematicsType, QWidget *parent);
  ~glitch_object_mathematics_arduino();
  QString code(void) const;
  bool hasInput(void) const;
  bool hasOutput(void) const;
  bool isFullyWired(void) const;
  bool shouldPrint(void) const;
  glitch_object_mathematics_arduino *clone(QWidget *parent) const;
  static glitch_object_mathematics_arduino *createFromValues
    (const QMap<QString, QVariant> &values, QString &error, QWidget *parent);
  void save(const QSqlDatabase &db, QString &error);

 private:
  enum class Type
  {
    ABS = 0,
    CONSTRAIN,
    MAP,
    MAX,
    MIN,
    POW,
    SQ,
    SQRT
  };

  glitch_object_mathematics_arduino(const qint64 id, QWidget *parent);

  QString description(void) const
  {
    switch(stringToMathematicsType(m_text))
      {
      case Type::ABS:
      default:
	{
	  return "T abs(T x)";
	}
      case Type::CONSTRAIN:
	{
	  return "T constrain(T amt, T low, T high)";
	}
      case Type::MAP:
	{
	  return "long map"
	    "(long x, long in_min, long in_max, long out_min, long out_max)";
	}
      case Type::MAX:
	{
	  return "T max(T a, T b)";
	}
      case Type::MIN:
	{
	  return "T min(T a, T b)";
	}
      case Type::POW:
	{
	  return "double pow(float base, float exponent)";
	}
      case Type::SQ:
	{
	  return "double sq(T x)";
	}
      case Type::SQRT:
	{
	  return "double sqrt(T x)";
	}
      }
  }

  QString mathematicsTypeToString(const Type type) const
  {
    switch(type)
      {
      case Type::ABS:
	{
	  return "abs";
	}
      case Type::CONSTRAIN:
	{
	  return "constrain";
	}
      case Type::MAP:
	{
	  return "map";
	}
      case Type::MAX:
	{
	  return "max";
	}
      case Type::MIN:
	{
	  return "min";
	}
      case Type::POW:
	{
	  return "pow";
	}
      case Type::SQ:
	{
	  return "sq";
	}
      case Type::SQRT:
	{
	  return "sqrt";
	}
      default:
	{
	  return "abs";
	}
      }
  }

  static Type stringToMathematicsType(const QString &s)
  {
    auto const string(s.toLower());

    if(string.contains("abs"))
      return Type::ABS;
    else if(string.contains("constrain"))
      return Type::CONSTRAIN;
    else if(string.contains("map"))
      return Type::MAP;
    else if(string.contains("max"))
      return Type::MAX;
    else if(string.contains("min"))
      return Type::MIN;
    else if(string.contains("pow"))
      return Type::POW;
    else if(string.contains("sqrt"))
      return Type::SQRT;
    else if(string.contains("sq"))
      return Type::SQ;
    else
      return Type::ABS;
  }

  void setProperties(const QStringList &list);
  void setProperty(const Properties property, const QVariant &value);
};

#endif
