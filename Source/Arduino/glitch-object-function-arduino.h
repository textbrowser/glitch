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

#ifndef _glitch_object_function_arduino_h_
#define _glitch_object_function_arduino_h_

#include <QPointer>
#include <QTimer>

#include "glitch-object.h"
#include "ui_glitch-object-function-arduino.h"

class glitch_view_arduino;

class glitch_object_function_arduino: public glitch_object
{
  Q_OBJECT

 public:
  glitch_object_function_arduino(QWidget *parent);
  glitch_object_function_arduino(const QString &name, QWidget *parent);
  ~glitch_object_function_arduino();
  QPointer<glitch_object_function_arduino> parentFunction(void) const;
  QString code(void) const;
  QString name(void) const;
  QString returnType(void) const;
  bool hasInput(void) const;
  bool hasOutput(void) const;
  bool hasView(void) const;
  bool isClone(void) const;
  bool isFullyWired(void) const;
  bool isPointer(void) const;
  bool shouldPrint(void) const;
  glitch_object_function_arduino *clone(QWidget *parent) const;
  static glitch_object_function_arduino *createFromValues
    (const QMap<QString, QVariant> &values,
     glitch_object *parentObject,
     QString &error,
     QWidget *parent);
  void addActions(QMenu &menu);
  void createEditObjects(void);
  void save(const QSqlDatabase &db, QString &error);
  void setIsPointer(const bool state);
  void setName(const QString &name);
  void setReturnType(const QString &returnType);

 private:
  glitch_object_function_arduino(const qint64 parentId,
				 const qint64 id,
				 glitch_object *parentObject,
				 QWidget *parent);
  QPointer<glitch_object_function_arduino> m_parentFunction;
  QPointer<glitch_view_arduino> m_parentView;
  QString m_previousReturnType;
  QTimer m_findParentFunctionTimer;
  Ui_glitch_object_function_arduino m_ui;
  bool m_initialized;
  bool m_isFunctionClone;
  bool m_previousAsterisk;
  glitch_object_function_arduino *findParentFunction(void) const;
  void compressWidget(const bool state);
  void hideOrShowOccupied(void);
  void initialize(QWidget *parent);
  void setProperties(const QString &properties);
  void simulateDelete(void);

 private slots:
  void slotAsteriskChanged(void);
  void slotEdit(void);
  void slotFindParentFunctionTimeout(void);
  void slotHideOrShowOccupied(void);
  void slotParentFunctionChanged(void);
  void slotReturnTypeChanged(void);
  void slotSetFunctionName(void);

 signals:
  void nameChanged(const QString &after,
		   const QString &before,
		   glitch_object *object);
  void returnPointerChanged(const bool after,
			    const bool before,
			    glitch_object *object);
  void returnTypeChanged(const QString &after,
			 const QString &before,
			 glitch_object *object);
};

#endif
