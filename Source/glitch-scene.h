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

#ifndef _glitch_scene_h_
#define _glitch_scene_h_

#include <QGraphicsScene>
#include <QHash>
#include <QPointer>

#include "glitch-common.h"

class QUndoStack;
class glitch_object;
class glitch_proxy_widget;

class glitch_scene: public QGraphicsScene
{
  Q_OBJECT

 public:
  glitch_scene(const glitch_common::ProjectTypes projectType, QObject *parent);
  ~glitch_scene();
  QList<glitch_object *> objects(void) const;
  QList<glitch_object *> selectedObjects(void) const;
  QPointer<QUndoStack> undoStack(void) const;
  glitch_proxy_widget *addObject(glitch_object *object);
  void addItem(QGraphicsItem *item);
  void artificialDrop(const QPointF &point, glitch_object *object);
  void deleteItems(void);
  void purgeRedoUndoProxies(void);
  void removeItem(QGraphicsItem *item);
  void setMainScene(const bool state);
  void setShowDots(const bool state);
  void setUndoStack(QUndoStack *undoStack);

 private:
  QHash<glitch_proxy_widget *, char> m_redoUndoProxies;
  QList<QPair<QPointF, glitch_proxy_widget *> > m_movedPoints;
  QPointF m_lastScenePos;
  QPointer<QUndoStack> m_undoStack;
  bool m_mainScene;
  bool m_showDots;
  glitch_common::ProjectTypes m_projectType;
  bool allowDrag(QGraphicsSceneDragDropEvent *event, const QString &t);
  void bringToFront(glitch_proxy_widget *proxy);
  void deleteFunctionClones(const QString &name);
  void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
  void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
  void drawBackground(QPainter *painter, const QRectF &rect);
  void dropEvent(QGraphicsSceneDragDropEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

 private slots:
  void slotFunctionNameChanged(const QString &after,
			       const QString &before,
			       glitch_object *object);
  void slotFunctionReturnTypeChanged(const QString &after,
				     const QString &before,
				     glitch_object *object);
  void slotObjectDeletedViaContextMenu(void);
  void slotRedo(void);
  void slotUndo(void);

 signals:
  void changed(void);
  void functionAdded(const QString &name, const bool isClone);
  void functionDeleted(const QString &name);
  void functionNameChanged(const QString &after,
			   const QString &before,
			   glitch_object *object);
  void functionReturnTypeChanged(const QString &after,
				 const QString &before,
				 glitch_object *object);
  void sceneResized(void);
};

#endif
