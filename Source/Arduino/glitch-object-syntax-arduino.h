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

#ifndef _glitch_object_syntax_arduino_h_
#define _glitch_object_syntax_arduino_h_

#include "glitch-object.h"
#include "ui_glitch-object-syntax-arduino.h"

class glitch_object_syntax_arduino: public glitch_object
{
  Q_OBJECT

 public:
  glitch_object_syntax_arduino(const QString &syntax, QWidget *parent);
  ~glitch_object_syntax_arduino();

  QSize sizeHint(void) const
  {
    QFontMetrics const fontMetrics(font());
    auto const width = 5 * qCeil
      (fontMetrics.boundingRect(m_ui.text->text().trimmed()).width() / 5.0);

    return QSize(s_widthTweak + width, qMax(50, fontMetrics.height()));
  }

  QString code(void) const;
  bool hasOutput(void) const;
  bool isFullyWired(void) const;
  bool shouldPrint(void) const;
  static glitch_object_syntax_arduino *createFromValues
    (const QMap<QString, QVariant> &values, QString &error, QWidget *parent);
  glitch_object_syntax_arduino *clone(QWidget *parent) const;
  void addActions(QMenu &menu);
  void save(const QSqlDatabase &db, QString &error);

 private:
  glitch_object_syntax_arduino(QWidget *parent);
  glitch_object_syntax_arduino(const qint64 id, QWidget *parent);
  Ui_glitch_object_syntax_arduino m_ui;
  void setProperties(const QStringList &list);
  void setProperty(const Properties property, const QVariant &value);

 private slots:
  void slotSyntaxChanged(void);
};

#endif
