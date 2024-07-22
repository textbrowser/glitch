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

#ifndef _glitch_structures_treewidget_h_
#define _glitch_structures_treewidget_h_

#include <QTimer>
#include <QTreeWidget>

#include "glitch-common.h"

class Ui_glitch_structures;

class glitch_structures_treewidget: public QTreeWidget
{
  Q_OBJECT

 public:
  glitch_structures_treewidget(QWidget *parent);
  ~glitch_structures_treewidget();
  void setProjectType(const glitch_common::ProjectTypes projectType);

 private:
  QList<Ui_glitch_structures *> m_uis;
  QTimer m_pressAndHoldTimer;
  glitch_common::ProjectTypes m_projectType;
  void mousePressEvent(QMouseEvent *event);
  void startDrag(Qt::DropActions supportedActions);

 private slots:
  void slotCustomContextMenuRequested(const QPoint &point);
  void slotFloatingCategoryDialog(void);
  void slotPressAndHoldTimeout(void);
};

#endif
