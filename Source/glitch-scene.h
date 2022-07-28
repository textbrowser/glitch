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
#include <QMultiMap>
#include <QPointer>
#include <QSet>
#include <QtGlobal>

#include "glitch-canvas-settings.h"
#include "glitch-common.h"
#include "glitch-point.h"
#include "glitch-tools.h"

class QUndoStack;
class glitch_object;
class glitch_proxy_widget;
class glitch_wire;

class glitch_scene: public QGraphicsScene
{
  Q_OBJECT

 public:
  glitch_scene(const glitch_common::ProjectTypes projectType, QObject *parent);
  ~glitch_scene();
  QList<glitch_object *> objects(void) const;
  QList<glitch_object *> orderedObjects(void) const;
  QList<glitch_object *> selectedObjects(void) const;
  QPointer<QUndoStack> undoStack(void) const;
  QSet<glitch_wire *> wires(void) const;
  glitch_proxy_widget *addObject(glitch_object *object);
  glitch_tools::Operations toolsOperation(void) const;

  int objectOrder(glitch_proxy_widget *proxy) const
  {
    if(!proxy)
      return -1;

    return std::distance
      (m_objectsMap.begin(),
       m_objectsMap.find(m_objectsHash.value(proxy), proxy));
  }

  int selectedForWiringCount(void) const
  {
    return m_objectsToWire.size();
  }

  void addItem(QGraphicsItem *item);
  void artificialDrop(const QPointF &point, glitch_object *object);
  void deleteItems(void);
  void purgeRedoUndoProxies(void);
  void removeItem(QGraphicsItem *item);
  void setCanvasSettings(glitch_canvas_settings *canvasSettings);
  void setDotsGridsColor(const QColor &color);
  void setMainScene(const bool state);
  void setShowCanvasDots(const bool state);
  void setShowCanvasGrids(const bool state);
  void setUndoStack(QUndoStack *undoStack);

 private:
  QColor m_dotsGridsColor;
  QHash<QString, QPointer<glitch_proxy_widget> > m_objectsToWire;
  QHash<glitch_proxy_widget *, glitch_point> m_objectsHash; // For ordering.
  QList<QPair<QPointF, glitch_proxy_widget *> > m_movedPoints;
  QMap<QPointer<glitch_proxy_widget>, char> m_redoUndoProxies;
  QMultiMap<glitch_point, glitch_proxy_widget *> m_objectsMap; // For ordering.
  QPointF m_lastScenePos;
  QPointer<QUndoStack> m_undoStack;
  QPointer<glitch_canvas_settings> m_canvasSettings;
  QSet<glitch_wire *> m_wires;
  bool m_mainScene;
  bool m_showCanvasDots;
  bool m_showCanvasGrids;
  glitch_common::ProjectTypes m_projectType;
  glitch_tools::Operations m_toolsOperation;
  bool allowDrag(QGraphicsSceneDragDropEvent *event, const QString &t) const;
  bool areObjectsWired(glitch_object *object1, glitch_object *object2) const;
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
  void recordProxyOrder(glitch_proxy_widget *proxy);
  void wireConnectObjects(glitch_proxy_widget *proxy);
  void wireDisconnectObjects(const QPointF &point, glitch_proxy_widget *proxy);

 public slots:
  void slotToolsOperationChanged(const glitch_tools::Operations operation);

 private slots:
  void slotCanvasSettingsChanged(const bool undo);
  void slotFunctionDeleted(const QString &name);
  void slotFunctionNameChanged(const QString &after,
			       const QString &before,
			       glitch_object *object);
  void slotFunctionReturnTypeChanged(const QString &after,
				     const QString &before,
				     glitch_object *object);
  void slotObjectDeletedViaContextMenu(void);
  void slotProxyChanged(void);
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
  void mousePressed(void);
  void sceneResized(void);
  void undoStackCreated(QUndoStack *undoStack);
};

#endif
