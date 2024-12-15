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

#ifndef _glitch_object_interrupts_arduino_h_
#define _glitch_object_interrupts_arduino_h_

#include "glitch-object-simple-text-arduino.h"

class glitch_object_interrupts_arduino:
  public glitch_object_simple_text_arduino
{
  Q_OBJECT

 public:
  glitch_object_interrupts_arduino
    (const QString &interruptsType, QWidget *parent);
  ~glitch_object_interrupts_arduino();
  QString code(void) const;
  bool hasInput(void) const;
  bool hasOutput(void) const;
  bool isFullyWired(void) const;
  bool shouldPrint(void) const;
  glitch_object_interrupts_arduino *clone(QWidget *parent) const;
  static glitch_object_interrupts_arduino *createFromValues
    (const QMap<QString, QVariant> &values, QString &error, QWidget *parent);
  void save(const QSqlDatabase &db, QString &error);

 private:
  enum class Type
  {
    ATTACH_INTERRUPT = 0,
    DETACH_INTERRUPT,
    INTERRUPTS,
    NO_INTERRUPTS
  };

  glitch_object_interrupts_arduino(const qint64 id, QWidget *parent);

  QString description(void) const
  {
    switch(stringToInterruptsType(m_text))
      {
      case Type::ATTACH_INTERRUPT:
      default:
	{
	  return "void attachInterrupt"
	    "(pin_size_t interruptNumber, "
	    "voidFuncPtr callback, "
	    "PinStatus mode)";
	}
      case Type::DETACH_INTERRUPT:
	{
	  return "void detachInterrupt(pin_size_t interruptNumber)";
	}
      case Type::INTERRUPTS:
	{
	  return "void interrupts(void)";
	}
      case Type::NO_INTERRUPTS:
	{
	  return "void noInterrupts(void)";
	}
      }
  }

  QString interruptsTypeToString(const Type type) const
  {
    switch(type)
      {
      case Type::ATTACH_INTERRUPT:
	{
	  return "attachInterrupt";
	}
      case Type::DETACH_INTERRUPT:
	{
	  return "detachInterrupt";
	}
      case Type::INTERRUPTS:
	{
	  return "interrupts";
	}
      case Type::NO_INTERRUPTS:
	{
	  return "noInterrupts";
	}
      default:
	{
	  return "attachInterrupt";
	}
      }
  }

  static Type stringToInterruptsType(const QString &s)
  {
    auto const string(s.toLower());

    if(string.contains("attachinterrupt"))
      return Type::ATTACH_INTERRUPT;
    else if(string.contains("detachinterrupt"))
      return Type::DETACH_INTERRUPT;
    else if(string.contains("nointerrupts"))
      return Type::NO_INTERRUPTS;
    else if(string.contains("interrupts"))
      return Type::INTERRUPTS;
    else
      return Type::ATTACH_INTERRUPT;
  }

  void setProperties(const QStringList &list);
  void setProperty(const Properties property, const QVariant &value);
};

#endif
