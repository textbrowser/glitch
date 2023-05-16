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

#ifndef _glitch_object_simple_text_arduino_h_
#define _glitch_object_simple_text_arduino_h_

#include "glitch-object.h"

class glitch_object_simple_text_arduino: public glitch_object
{
  Q_OBJECT

 public:
  glitch_object_simple_text_arduino(const QString &text, QWidget *parent);
  virtual ~glitch_object_simple_text_arduino();

  bool isNativelyDrawn(void) const
  {
    return true;
  }

  virtual QSize minimumSizeHint(void) const
  {
    return QSize(250, 50);
  }

  virtual QSize sizeHint(void) const
  {
    return minimumSizeHint();
  }

  virtual void addActions(QMenu &menu)
  {
    addDefaultActions(menu);
    m_actions.value(DefaultMenuActions::COMPRESS_WIDGET)->setEnabled(false);
    m_actions.value(DefaultMenuActions::TRANSPARENT)->setEnabled(false);
    menu.addSeparator();
    prepareTransformationMenu(menu.addMenu(tr("Transform")));
  }

  void resizeEvent(QResizeEvent *event);
  void setName(const QString &n);

 protected:
  QString m_text;
  QStringList m_functionsList;
  glitch_object_simple_text_arduino(const qint64 id, QWidget *parent);
  virtual void setProperty(const Properties property, const QVariant &value);

 private:
  glitch_object_simple_text_arduino(QWidget *parent);
  void paintEvent(QPaintEvent *event);
  void prepareTransformationMenu(QMenu *menu);

 private slots:
  void slotTransformed(void);
};

#endif
