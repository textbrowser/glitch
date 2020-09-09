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

#ifndef _glitch_object_view_h_
#define _glitch_object_view_h_

#include <QGraphicsView>
#include <QSqlDatabase>

#include "glitch-common.h"

class QUndoStack;
class glitch_alignment;
class glitch_object;
class glitch_scene;

class glitch_object_view: public QGraphicsView
{
  Q_OBJECT

 public:
  glitch_object_view(const glitch_common::ProjectType projectType,
		     const quint64 id,
		     QWidget *parent);
  ~glitch_object_view();
  glitch_scene* scene(void) const;
  quint64 id(void) const;
  void artificialDrop(const QPointF &point, glitch_object *object);
  void save(const QSqlDatabase &db, QString &error);
  void setSceneRect(const QSize &size);
  void setUndoStack(QUndoStack *undoStack);

 private:
  QUndoStack *m_undoStack;
  glitch_alignment *m_alignment;
  glitch_common::ProjectType m_projectType;
  glitch_scene *m_scene;
  quint64 m_id;
  void contextMenuEvent(QContextMenuEvent *event);

 protected slots:
  void slotCustomContextMenuRequested(const QPoint &point);
  void slotParentWindowClosed(void);
  void slotPaste(void);
  void slotRedo(void);
  void slotSceneResized(void);
  void slotSelectAll(void);
  void slotUndo(void);

 signals:
  void changed(void);
  void copy(void);
  void paste(void);
};

#endif
