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

#ifndef _glitch_user_functions_h_
#define _glitch_user_functions_h_

#include <QPointer>

#include "glitch-user-functions-model.h"
#include "ui_glitch-user-functions.h"

class glitch_user_functions: public QDialog
{
  Q_OBJECT

 public:
  glitch_user_functions(QWidget *parent);
  ~glitch_user_functions();
  QFrame *frame(void) const;
  bool contains(const QString &name) const;
  void addFunction(const QString &name);
  void deleteFunction(const QString &name);
  void renameFunction(const QString &before, const QString &after);
  void setModel(glitch_user_functions_model *model);
  void setProjectType(const glitch_common::ProjectTypes projectType);

 private:
  QPointer<glitch_user_functions_model> m_model;
  Ui_glitch_user_functions m_ui;

 private slots:
  void slotDoubleClicked(const QModelIndex &index);
};

#endif
