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
**    derived from GlowBot without specific prior written permission.
**
** GLOWBOT IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** GLOWBOT, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _glowbot_object_function_arduino_h_
#define _glowbot_object_function_arduino_h_

#include <QPointer>

#include "glowbot-object.h"
#include "ui_glowbot-object-function-arduino.h"

class glowbot_object_edit_window;
class glowbot_view_arduino;

class glowbot_object_function_arduino: public glowbot_object
{
  Q_OBJECT

 public:
  glowbot_object_function_arduino(QWidget *parent);
  glowbot_object_function_arduino(const QString &name, QWidget *parent);
  glowbot_object_function_arduino(const quint64 id, QWidget *parent);
  ~glowbot_object_function_arduino();
  static glowbot_object_function_arduino *createFromValues
    (const QMap<QString, QVariant> &values, QString &error, QWidget *parent);
  QString name(void) const;
  bool hasView(void) const;
  bool isMandatory(void) const;
  glowbot_object_function_arduino *clone(QWidget *parent) const;
  void addActions(QMenu &menu) const;
  void addChild(const QPointF &point, glowbot_object *object);
  void closeEditWindow(void);
  void save(const QSqlDatabase &db, QString &error);

 private:
  QPointer<glowbot_object_edit_window> m_editWindow;
  Ui_glowbot_object_function_arduino m_ui;
  bool m_isFunctionReference;
  glowbot_view_arduino *m_view;
  void initialize(QWidget *parent);
  void mouseDoubleClickEvent(QMouseEvent *event);
  void setProperties(const QString &properties);

 private slots:
  void slotEdit(void);
  void slotSetFunctionName(void);

 signals:
  void nameChanged(const QString &before, const QString &after);
};

#endif
