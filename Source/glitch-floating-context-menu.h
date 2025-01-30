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

#ifndef _glitch_floating_context_menu_h_
#define _glitch_floating_context_menu_h_

#include <QPointer>

#include "glitch-object.h"
#include "ui_glitch-floating-context-menu.h"

class glitch_floating_context_menu: public QDialog
{
  Q_OBJECT

 public:
  glitch_floating_context_menu(QWidget *parent);
  ~glitch_floating_context_menu();

  QPointer<glitch_object> object(void) const
  {
    return m_object;
  }

  void addActions(const QList<QAction *> &actions);
  void dockedFloatingContextMenu(const bool state);
  void setIdentifier(const qint64 id);
  void setObject(glitch_object *object);
  void setName(const QString &n);
  void setProperty
    (const glitch_object::Properties property, const QVariant &value);

 public slots:
  void slotActionChanged(void);
  void slotObjectChanged(void);
  void slotSizeChanged(int value);
  void slotZValueChanged(qreal value);

 private:
  QPointer<QDoubleSpinBox> m_zValue;
  QPointer<glitch_object> m_object;
  Ui_glitch_floating_context_menu m_ui;
  void closeEvent(QCloseEvent *event);
  void hideEvent(QHideEvent *event);
  void setPushButtonColors(void);

 signals:
  void closed(void);
  void propertyChanged
    (const glitch_object::Properties property, const QVariant &value);
};

#endif
