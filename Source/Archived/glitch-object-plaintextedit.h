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

#ifndef _glitch_object_plaintextedit_h_
#define _glitch_object_plaintextedit_h_

#include <QApplication>
#include <QPlainTextEdit>

class glitch_object_plaintextedit: public QPlainTextEdit
{
  Q_OBJECT

 public:
  glitch_object_plaintextedit(QWidget *parent):QPlainTextEdit(parent)
  {
    setReadOnly(true);
  }

  ~glitch_object_plaintextedit()
  {
  }

 private:
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
  void enterEvent(QEnterEvent *event)
#else
  void enterEvent(QEvent *event)
#endif
  {
    if(isReadOnly())
      QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
    else
      QApplication::setOverrideCursor(QCursor(Qt::IBeamCursor));

    QPlainTextEdit::enterEvent(event);
  }

  void leaveEvent(QEvent *event)
  {
    QApplication::restoreOverrideCursor();
    QPlainTextEdit::leaveEvent(event);
    setReadOnly(true);
  }

  void mouseDoubleClickEvent(QMouseEvent *event)
  {
    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(QCursor(Qt::IBeamCursor));
    QPlainTextEdit::mouseDoubleClickEvent(event);
    setReadOnly(false);
  }

  void mouseMoveEvent(QMouseEvent *event)
  {
    if(isReadOnly())
      return;

    QPlainTextEdit::mouseMoveEvent(event);
  }

  void mousePressEvent(QMouseEvent *event)
  {
    if(isReadOnly())
      return;

    QPlainTextEdit::mousePressEvent(event);
  }
};

#endif
