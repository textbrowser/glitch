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

#ifndef _glowbot_object_view_h_
#define _glowbot_object_view_h_

#include <QGraphicsView>
#include <QSqlDatabase>

#include "glowbot-common.h"

class QUndoStack;
class glowbot_alignment;
class glowbot_scene;

class glowbot_object_view: public QGraphicsView
{
  Q_OBJECT

 public:
  glowbot_object_view(const glowbot_common::ProjectType projectType,
		      const quint64 id,
		      QWidget *parent);
  ~glowbot_object_view();
  glowbot_scene* scene(void) const;
  quint64 id(void) const;
  void save(const QSqlDatabase &db, QString &error);
  void setSceneRect(const QSize &size);
  void setUndoStack(QUndoStack *undoStack);

 private:
  glowbot_alignment *m_alignment;
  glowbot_common::ProjectType m_projectType;
  glowbot_scene *m_scene;
  quint64 m_id;
  void contextMenuEvent(QContextMenuEvent *event);

 protected slots:
  void slotCustomContextMenuRequested(const QPoint &point);
  void slotParentWindowClosed(void);
  void slotRedo(void);
  void slotSceneResized(void);
  void slotUndo(void);

 signals:
  void changed(void);
};

#endif
