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

#ifndef _glitch_object_arrow_h_
#define _glitch_object_arrow_h_

#include "glitch-object.h"

class glitch_object_arrow: public glitch_object
{
  Q_OBJECT

 public:
  glitch_object_arrow(const QString &text, QWidget *parent);
  ~glitch_object_arrow();

  QSize sizeHint(void) const
  {
    return QSize(100, 30);
  }

  QString code(void) const
  {
    return "";
  }

  bool isFullyWired(void) const
  {
    return true;
  }

  bool shouldPrint(void) const
  {
    return false;
  }

  glitch_object_arrow *clone(QWidget *parent) const;
  static glitch_object_arrow *createFromValues
    (const QMap<QString, QVariant> &values, QString &error, QWidget *parent);
  void addActions(QMenu &menu);
  void save(const QSqlDatabase &db, QString &error);

 private:
  enum class Arrows
  {
    LEFT = 0,
    LEFT_RIGHT,
    RIGHT
  };

  Arrows m_arrow;

  QString arrowToString(void) const
  {
    if(m_arrow == Arrows::LEFT)
      return "left";
    else if(m_arrow == Arrows::RIGHT)
      return "right";
    else
      return "left+right";
  }

  glitch_object_arrow(const qint64 id, QWidget *parent);

  static Arrows stringToArrow(const QString &s)
  {
    auto string(s.toLower().trimmed());

    if(string == "left")
      return Arrows::LEFT;
    else if(string == "right")
      return Arrows::RIGHT;
    else
      return Arrows::LEFT_RIGHT;
  }

  void paintEvent(QPaintEvent *event);
  void setProperties(const QStringList &list);
};

#endif
