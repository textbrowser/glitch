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

class QMainWindow;
class QUndoStack;
class glitch_graphicsview;
class glitch_object;
class glitch_proxy_widget;
class glitch_wire;

class glitch_scene: public QGraphicsScene
{
  Q_OBJECT

 public:
  glitch_scene(const glitch_common::ProjectTypes projectType, QObject *parent);
  ~glitch_scene();
  QList<glitch_object *> allObjects(void) const;
  QList<glitch_object *> objects(void) const;
  QList<glitch_object *> orderedObjects(void) const;
  QList<glitch_object *> selectedObjects(void) const;

  QPointF lastHoverScenePos(void) const
  {
    return m_lastHoverScenePos;
  }

  QPointer<QUndoStack> undoStack(void) const;
  QPointer<glitch_canvas_settings> canvasSettings(void) const;
  QSet<glitch_wire *> wires(void) const;
  bool areObjectsWireCompatible(glitch_object *object) const;
  bool areObjectsWired(glitch_object *object1, glitch_object *object2) const;
  bool objectToBeWired(glitch_proxy_widget *proxy) const;
  bool objectToBeWiredIn(glitch_proxy_widget *proxy) const;
  bool objectToBeWiredOut(glitch_proxy_widget *proxy) const;
  glitch_proxy_widget *addObject(glitch_object *object);
  glitch_tools::Operations toolsOperation(void) const;
  int objectOrder(glitch_proxy_widget *proxy) const;

  int selectedForWiringCount(void) const
  {
    return m_objectsToWire.size();
  }

  void addItem(QGraphicsItem *item, const bool visible = true);
  void artificialDrop(const QPointF &point, glitch_object *object, bool &ok);
  void deleteItems
    (const QList<QGraphicsItem *> &list = QList<QGraphicsItem *> (),
     const bool redoUndoMacro = true);
  void purgeRedoUndoProxies(void);
  void redo(void);
  void removeItem(QGraphicsItem *item);
  void saveWires(const QSqlDatabase &db, QString &error);
  void setCanvasSettings(glitch_canvas_settings *canvasSettings);
  void setDotsGridsColor(const QColor &color);
  void setMainScene(const bool state);
  void setLoadingFromFile(const bool state);
  void setShowCanvasDots(const bool state);
  void setShowCanvasGrids(const bool state);
  void setUndoStack(QUndoStack *undoStack);
  void undo(void);

 private:
  enum class ItemProperties
  {
    WasSelected = 0
  };

  QColor m_dotsGridsColor;
  QHash<QString, QPointer<glitch_proxy_widget> > m_objectsToWire;
  QHash<glitch_proxy_widget *, glitch_point> m_objectsHash; // For ordering.
  QList<QPair<QPointF, glitch_proxy_widget *> > m_movedPoints;
  QMap<QPointer<glitch_proxy_widget>, char> m_redoUndoProxies;
  QMultiMap<glitch_point, glitch_proxy_widget *> m_objectsMap; // For ordering.
  QPointF m_lastHoverScenePos;
  QPointF m_lastScenePos;
  QPointer<QUndoStack> m_undoStack;
  QPointer<glitch_canvas_settings> m_canvasSettings;
  bool m_loadingFromFile;
  bool m_mainScene;
  bool m_showCanvasDots;
  bool m_showCanvasGrids;
  glitch_common::ProjectTypes m_projectType;
  glitch_tools::Operations m_toolsOperation;
  mutable QSet<glitch_wire *> m_wires;
  QGraphicsView *primaryView(void) const;
  bool allowDrag(QGraphicsSceneDragDropEvent *event, const QString &t) const;
  void bringToFront(glitch_proxy_widget *proxy);
  void deleteFunctionClones(const QString &name);
  void disconnectWireIfNecessary(glitch_wire *wire);
  void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
  void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
  void drawBackground(QPainter *painter, const QRectF &rect);
  void dropEvent(QGraphicsSceneDragDropEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  void paste(const QList<QPointF> &points,
	     const QList<glitch_object *> &objects,
	     const QPointF &position);
  void prepareBackgroundForMove(const bool state);
  void recordProxyOrder(glitch_proxy_widget *proxy);
  void wireConnectObjects(glitch_proxy_widget *proxy);
  void wireDisconnectObjects(const QPointF &point, glitch_proxy_widget *proxy);
  void wireDisconnectObjects(glitch_proxy_widget *proxy);

 public slots:
  void slotDisconnectWireIfNecessary(void);
  void slotSelectedWidgetsAdjustSize(void);
  void slotSelectedWidgetsCompress(void);
  void slotSelectedWidgetsDisconnect(void);
  void slotSelectedWidgetsLock(void);
  void slotToolsOperationChanged(const glitch_tools::Operations operation);

 private slots:
  void slotCanvasSettingsChanged(const bool state);
  void slotFunctionDeleted(const QString &name);
  void slotFunctionNameChanged(const QString &after,
			       const QString &before,
			       glitch_object *object);
  void slotFunctionReturnPointerChanged(const bool after,
					const bool before,
					glitch_object *object);
  void slotFunctionReturnTypeChanged(const QString &after,
				     const QString &before,
				     glitch_object *object);
  void slotObjectDeletedViaContextMenu(void);
  void slotProxyChanged(void);
  void slotProxyGeometryChanged(const QRectF &previousRect);

 signals:
  void changed(void);
  void copy(void);
  void dockPropertyEditor(QWidget *widget);
  void functionAdded(const QString &name, const bool isClone);
  void functionDeleted(const QString &name);
  void functionNameChanged(const QString &after,
			   const QString &before,
			   glitch_object *object);
  void functionReturnPointerChanged(const bool after,
				    const bool before,
				    glitch_object *object);
  void functionReturnTypeChanged(const QString &after,
				 const QString &before,
				 glitch_object *object);
  void mousePressed(void);
  void processCommand(const QString &command, const QStringList &arguments);
  void saveSignal(void);
  void sceneResized(void);
  void showEditWindow(QMainWindow *window);
  void wireObjects(void);
};

#endif
