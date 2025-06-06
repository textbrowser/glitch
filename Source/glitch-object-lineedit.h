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

#ifndef _glitch_object_lineedit_h_
#define _glitch_object_lineedit_h_

#include <QApplication>
#include <QKeyEvent>
#include <QLineEdit>

#include "glitch-variety.h"

class glitch_object_lineedit: public QLineEdit
{
  Q_OBJECT

 public:
  glitch_object_lineedit(QWidget *parent):QLineEdit(parent)
  {
    connect(this,
	    &QLineEdit::returnPressed,
	    this,
	    &glitch_object_lineedit::slotReturnPressed);
    setReadOnly(true);
  }

  ~glitch_object_lineedit()
  {
  }

 private:
  void keyPressEvent(QKeyEvent *event)
  {
    if(!event)
      {
	QLineEdit::keyPressEvent(event);
	return;
      }

    switch(event->key())
      {
      case Qt::Key_A:
      case Qt::Key_C:
      case Qt::Key_V:
	{
	  if(glitch_variety::keyboardModifiers() & Qt::ControlModifier &&
	     isReadOnly())
	    {
	      event->ignore();
	      return;
	    }

	  break;
	}
      default:
	{
	  break;
	}
      }

    QLineEdit::keyPressEvent(event);
  }

  void leaveEvent(QEvent *event)
  {
    QApplication::restoreOverrideCursor();
    QLineEdit::leaveEvent(event);
    deselect();
    emit editingFinished();
    setReadOnly(true);
  }

  void mouseDoubleClickEvent(QMouseEvent *event)
  {
    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(QCursor(Qt::IBeamCursor));
    QLineEdit::mouseDoubleClickEvent(event);
    setReadOnly(false);
  }

  void mouseMoveEvent(QMouseEvent *event)
  {
    if(isReadOnly())
      return;

    QLineEdit::mouseMoveEvent(event);
  }

  void mousePressEvent(QMouseEvent *event)
  {
    if(isReadOnly())
      return;

    QLineEdit::mousePressEvent(event);
  }

 private slots:
  void slotReturnPressed(void)
  {
    QApplication::restoreOverrideCursor();
  }
};

#endif
