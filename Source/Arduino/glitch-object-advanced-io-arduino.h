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

#ifndef _glitch_object_advanced_io_arduino_h_
#define _glitch_object_advanced_io_arduino_h_

#include "glitch-object-simple-text-arduino.h"

class glitch_object_advanced_io_arduino:
  public glitch_object_simple_text_arduino
{
  Q_OBJECT

 public:
  glitch_object_advanced_io_arduino(const QString &ioType, QWidget *parent);
  ~glitch_object_advanced_io_arduino();

  QString code(void) const;
  bool hasInput(void) const;
  bool hasOutput(void) const;
  bool isFullyWired(void) const;
  bool shouldPrint(void) const;

  QString description(void) const
  {
    switch(stringToIOType(m_text))
      {
      case Type::NO_TONE:
      default:
	{
	  return "void noTone(uint8_t pin)";
	}
      case Type::PULSE_IN:
	{
	  return "unsigned long pulseIn"
	    "(uint8_t pin, uint8_t state, unsigned long timeout = 1000000L)";
	}
      case Type::PULSE_IN_LONG:
	{
	  return "unsigned long pulseInLong"
	    "(uint8_t pin, uint8_t state, unsigned long timeout = 1000000L)";
	}
      case Type::SHIFT_IN:
	{
	  return "uint8_t shiftIn"
	    "(pin_size_t dataPin, pin_size_t clockPin, BitOrder bitOrder)";
	}
      case Type::SHIFT_OUT:
	{
	  return "void shiftOut"
	    "(pin_size_t dataPin, "
	    "pin_size_t clockPin, "
	    "BitOrder bitOrder, "
	    "uint8_t val)";
	}
      case Type::TONE:
	{
	  return "void tone"
	    "(uint8_t pin, unsigned int frequency, unsigned long duration = 0)";
	}
      }
  }

  QStringList parameters(void) const
  {
    switch(stringToIOType(m_text))
      {
      case Type::NO_TONE:
      default:
	{
	  return QStringList() << "pin";
	}
      case Type::PULSE_IN:
	{
	  return QStringList() << "pin" << "value" << "timeout";
	}
      case Type::PULSE_IN_LONG:
	{
	  return QStringList() << "pin" << "value" << "timeout";
	}
      case Type::SHIFT_IN:
	{
	  return QStringList() << "dataPin" << "clockPin" << "bitOrder";
	}
      case Type::SHIFT_OUT:
	{
	  return QStringList() << "dataPin"
			       << "clockPin"
			       << "bitOrder"
			       << "value";
	}
      case Type::TONE:
	{
	  return QStringList() << "pin" << "frequency" << "duration";
	}
      }
  }

  glitch_object_advanced_io_arduino *clone(QWidget *parent) const;
  static glitch_object_advanced_io_arduino *createFromValues
    (const QMap<QString, QVariant> &values, QString &error, QWidget *parent);
  void save(const QSqlDatabase &db, QString &error);

 private:
  enum class Type
  {
    NO_TONE = 0,
    PULSE_IN,
    PULSE_IN_LONG,
    SHIFT_IN,
    SHIFT_OUT,
    TONE
  };

  glitch_object_advanced_io_arduino(const qint64 id, QWidget *parent);

  QString ioTypeToString(void) const
  {
    switch(stringToIOType(m_text))
      {
      case Type::NO_TONE:
	{
	  return "noTone";
	}
      case Type::PULSE_IN:
	{
	  return "pulseIn";
	}
      case Type::PULSE_IN_LONG:
	{
	  return "pulseInLong";
	}
      case Type::SHIFT_IN:
	{
	  return "shiftIn";
	}
      case Type::SHIFT_OUT:
	{
	  return "shiftOut";
	}
      case Type::TONE:
	{
	  return "tone";
	}
      default:
	{
	  return "noTone";
	}
      }
  }

  static Type stringToIOType(const QString &s)
  {
    auto string(s.toLower());

    if(string.contains("notone"))
      return Type::NO_TONE;
    else if(string.contains("pulseinlong"))
      return Type::PULSE_IN_LONG;
    else if(string.contains("pulsein"))
      return Type::PULSE_IN;
    else if(string.contains("shiftin"))
      return Type::SHIFT_IN;
    else if(string.contains("shiftout"))
      return Type::SHIFT_OUT;
    else if(string.contains("tone"))
      return Type::TONE;
    else
      return Type::NO_TONE;
  }

  void setProperties(const QStringList &list);
  void setProperty(const Properties property, const QVariant &value);
};

#endif
