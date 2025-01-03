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

#ifndef _glitch_object_bits_and_bytes_arduino_h_
#define _glitch_object_bits_and_bytes_arduino_h_

#include "glitch-object-simple-text-arduino.h"

class glitch_object_bits_and_bytes_arduino:
  public glitch_object_simple_text_arduino
{
  Q_OBJECT

 public:
  glitch_object_bits_and_bytes_arduino(const QString &babType, QWidget *parent);
  ~glitch_object_bits_and_bytes_arduino();
  QString code(void) const;
  bool hasInput(void) const;
  bool hasOutput(void) const;
  bool isFullyWired(void) const;
  bool shouldPrint(void) const;
  glitch_object_bits_and_bytes_arduino *clone(QWidget *parent) const;
  static glitch_object_bits_and_bytes_arduino *createFromValues
    (const QMap<QString, QVariant> &values, QString &error, QWidget *parent);
  void save(const QSqlDatabase &db, QString &error);

 private:
  enum class Type
  {
    BIT = 0,
    BIT_CLEAR,
    BIT_READ,
    BIT_SET,
    BIT_WRITE,
    HIGH_BYTE,
    LOW_BYTE
  };

  glitch_object_bits_and_bytes_arduino(const qint64 id, QWidget *parent);

  QString description(void) const
  {
    switch(stringToType(m_text))
      {
      case Type::BIT:
      default:
	{
	  return "T bit(T a)";
	}
      case Type::BIT_CLEAR:
	{
	  return "T bitClear(T value, T bit)";
	}
      case Type::BIT_READ:
	{
	  return "T bitRead(T value, T bit)";
	}
      case Type::BIT_SET:
	{
	  return "void bitSet(T value, T bit)";
	}
      case Type::BIT_WRITE:
	{
	  return "void bitWrite(T value, T bit, T bitValue)";
	}
      case Type::HIGH_BYTE:
	{
	  return "byte highByte(T a)";
	}
      case Type::LOW_BYTE:
	{
	  return "byte lowByte(T a)";
	}
      }
  }

  QString typeToString(void) const
  {
    switch(stringToType(m_text))
      {
      case Type::BIT:
	{
	  return "bit";
	}
      case Type::BIT_CLEAR:
	{
	  return "bitClear";
	}
      case Type::BIT_READ:
	{
	  return "bitRead";
	}
      case Type::BIT_SET:
	{
	  return "bitSet";
	}
      case Type::BIT_WRITE:
	{
	  return "bitWrite";
	}
      case Type::HIGH_BYTE:
	{
	  return "highByte";
	}
      case Type::LOW_BYTE:
	{
	  return "lowByte";
	}
      default:
	{
	  return "bit";
	}
      }
  }

  static Type stringToType(const QString &s)
  {
    auto const string(s.toLower());

    if(string.contains("bitclear"))
      return Type::BIT_CLEAR;
    else if(string.contains("bitread"))
      return Type::BIT_READ;
    else if(string.contains("bitset"))
      return Type::BIT_SET;
    else if(string.contains("bitwrite"))
      return Type::BIT_WRITE;
    else if(string.contains("highbyte"))
      return Type::HIGH_BYTE;
    else if(string.contains("lowbyte"))
      return Type::LOW_BYTE;
    else
      return Type::BIT;
  }

  void setProperties(const QStringList &list);
  void setProperty(const Properties property, const QVariant &value);
};

#endif
