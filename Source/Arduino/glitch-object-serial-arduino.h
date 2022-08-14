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

#ifndef _glitch_object_serial_arduino_h_
#define _glitch_object_serial_arduino_h_

#include "glitch-object.h"
#include "ui_glitch-object-serial-arduino.h"

class glitch_object_serial_arduino: public glitch_object
{
  Q_OBJECT

 public:
  glitch_object_serial_arduino(const QString &serialType, QWidget *parent);
  glitch_object_serial_arduino(const qint64 id, QWidget *parent);
  ~glitch_object_serial_arduino();
  QString code(void) const;
  bool hasInput(void) const;
  bool hasOutput(void) const;
  bool isFullyWired(void) const;
  bool shouldPrint(void) const;
  glitch_object_serial_arduino *clone(QWidget *parent) const;
  static glitch_object_serial_arduino *createFromValues
    (const QMap<QString, QVariant> &values, QString &error, QWidget *parent);
  void addActions(QMenu &menu);
  void save(const QSqlDatabase &db, QString &error);

 private:
  enum Type
  {
    AVAILABLE = 0,
    AVAILABLE_FOR_WRITE,
    BEGIN,
    END,
    FIND,
    FIND_UNTIL,
    FLUSH,
    PARSE_FLOAT,
    PARSE_INT,
    PEEK,
    PRINT,
    PRINTLN,
    READ,
    READ_BYTES,
    READ_BYTES_UNTIL,
    READ_STRING,
    READ_STRING_UNTIL,
    SET_TIMEOUT,
    WRITE
  };

  Type m_serialType;
  Ui_glitch_object_serial_arduino m_ui;

  QString serialTypeToString(void) const
  {
    switch(m_serialType)
      {
      case Type::AVAILABLE:
	{
	  return "available";
	}
      case Type::AVAILABLE_FOR_WRITE:
	{
	  return "availableForWrite";
	}
      case Type::BEGIN:
	{
	  return "begin";
	}
      case Type::END:
	{
	  return "end";
	}
      case Type::FIND:
	{
	  return "find";
	}
      case Type::FIND_UNTIL:
	{
	  return "findUntil";
	}
      case Type::FLUSH:
	{
	  return "flush";
	}
      case Type::PRINT:
	{
	  return "print";
	}
      case Type::PRINTLN:
	{
	  return "println";
	}
      case Type::WRITE:
	{
	  return "write";
	}
      default:
	{
	  return "available";
	}
      }
  }

  static Type stringToSerialType(const QString &s)
  {
    auto string(s.toLower());

    if(string.contains("availableforwrite"))
      return Type::AVAILABLE_FOR_WRITE;
    else if(string.contains("begin"))
      return Type::BEGIN;
    else if(string.contains("end"))
      return Type::END;
    else if(string.contains("finduntil"))
      return Type::FIND_UNTIL;
    else if(string.contains("find"))
      return Type::FIND;
    else if(string.contains("parsefloat"))
      return Type::PARSE_FLOAT;
    else if(string.contains("parseint"))
      return Type::PARSE_INT;
    else if(string.contains("peek"))
      return Type::PEEK;
    else if(string.contains("println"))
      return Type::PRINTLN;
    else if(string.contains("print"))
      return Type::PRINT;
    else if(string.contains("readbytesuntil"))
      return Type::READ_BYTES_UNTIL;
    else if(string.contains("readbytes"))
      return Type::READ_BYTES;
    else if(string.contains("readstringuntil"))
      return Type::READ_STRING_UNTIL;
    else if(string.contains("readstring"))
      return Type::READ_STRING;
    else if(string.contains("read"))
      return Type::READ;
    else if(string.contains("settimeout"))
      return Type::SET_TIMEOUT;
    else if(string.contains("write"))
      return Type::WRITE;
    else
      return Type::AVAILABLE;
  }

  void setProperties(const QStringList &list);
};

#endif
