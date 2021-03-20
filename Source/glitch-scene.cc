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

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QMimeData>
#include <QTableWidget>
#include <QUndoStack>
#include <QtDebug>

#include "Arduino/glitch-object-analog-read-arduino.h"
#include "Arduino/glitch-object-block-comment-arduino.h"
#include "Arduino/glitch-object-function-arduino.h"
#include "Arduino/glitch-object-logical-operator-arduino.h"
#include "Arduino/glitch-structures-arduino.h"
#include "glitch-proxy-widget.h"
#include "glitch-scene.h"
#include "glitch-undo-command.h"
#include "glitch-graphicsview.h"

glitch_scene::glitch_scene(const glitch_common::ProjectType projectType,
			   QObject *parent):QGraphicsScene(parent)
{
  m_mainScene = false;
  m_projectType = projectType;
  m_undoStack = nullptr;
}

glitch_scene::~glitch_scene()
{
}

QList<glitch_object *> glitch_scene::objects(void) const
{
  QList<QGraphicsItem *> list(items());
  QList<glitch_object *> widgets;

  for(auto i : list)
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(!proxy || !(proxy->flags() & QGraphicsItem::ItemIsSelectable))
	continue;

      widgets << qobject_cast<glitch_object *> (proxy->widget());
    }

  return widgets;
}

QList<glitch_object *> glitch_scene::selectedObjects(void) const
{
  QList<QGraphicsItem *> list(items());
  QList<glitch_object *> widgets;

  for(auto i : list)
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(!proxy ||
	 !(proxy->flags() & QGraphicsItem::ItemIsSelectable) ||
	 !proxy->isSelected())
	continue;

      widgets << qobject_cast<glitch_object *> (proxy->widget());
    }

  return widgets;
}

QPointer<QUndoStack> glitch_scene::undoStack(void) const
{
  return m_undoStack;
}

bool glitch_scene::allowDrag
(QGraphicsSceneDragDropEvent *event, const QString &t)
{
  if(!event)
    return false;
  else
    {
      QString text(t.trimmed().remove("glitch-"));

      if(m_mainScene)
	{
	  if(glitch_structures_arduino::containsStructure(text))
	    {
	      event->accept();
	      return true;
	    }
	  else
	    {
	      event->ignore();
	      return false;
	    }
	}
      else
	{
	  switch(m_projectType)
	    {
	    case glitch_common::ArduinoProject:
	      {
		if(glitch_structures_arduino::containsStructure(text))
		  /*
		  ** Functions cannot be defined in other functions.
		  */

		  if(!text.startsWith("arduino-function"))
		    {
		      event->accept();
		      return true;
		    }

		break;
	      }
	    default:
	      {
		break;
	      }
	    }

	  auto tableWidget = qobject_cast<QTableWidget *> (event->source());

	  if(tableWidget)
	    {
	      auto item = tableWidget->currentItem();

	      if(item)
		{
		  QString text(item->data(Qt::UserRole).toString().toLower());

		  if(text == "glitch-user-function")
		    {
		      event->accept();
		      return true;
		    }
		}
	    }

	  event->ignore();
	  return false;
	}
    }
}

glitch_proxy_widget *glitch_scene::addObject(glitch_object *object)
{
  if(!object)
    return nullptr;

  auto proxy = new glitch_proxy_widget();

  connect(object,
	  SIGNAL(changed(void)),
	  this,
	  SIGNAL(changed(void)),
	  Qt::UniqueConnection);
  connect(object,
	  SIGNAL(deletedViaContextMenu(void)),
	  this,
	  SLOT(slotObjectDeletedViaContextMenu(void)),
	  Qt::UniqueConnection);
  connect(object,
	  SIGNAL(destroyed(QObject *)),
	  this,
	  SIGNAL(destroyed(QObject *)),
	  Qt::UniqueConnection);
  connect(object,
	  SIGNAL(destroyed(void)),
	  proxy,
	  SLOT(deleteLater(void)),
	  Qt::UniqueConnection);
  object->setProxy(proxy);
  proxy->setFlag(QGraphicsItem::ItemIsSelectable, true);
  proxy->setWidget(object);
  emit changed();

  if(qobject_cast<glitch_object_function_arduino *> (object))
    {
      auto function = qobject_cast<glitch_object_function_arduino *> (object);

      connect(function,
	      SIGNAL(nameChanged(const QString &,
				 const QString &,
				 glitch_object *)),
	      this,
	      SIGNAL(functionNameChanged(const QString &,
					 const QString &,
					 glitch_object *)),
	      Qt::UniqueConnection);
      connect(function,
	      SIGNAL(nameChanged(const QString &,
				 const QString &,
				 glitch_object *)),
	      this,
	      SLOT(slotFunctionNameChanged(const QString &,
					   const QString &,
					   glitch_object *)),
	      Qt::UniqueConnection);
      connect(function,
	      SIGNAL(returnTypeChanged(const QString &,
				       const QString &,
				       glitch_object *)),
	      this,
	      SIGNAL(functionReturnTypeChanged(const QString &,
					       const QString &,
					       glitch_object *)),
	      Qt::UniqueConnection);
      connect(function,
	      SIGNAL(returnTypeChanged(const QString &,
				       const QString &,
				       glitch_object *)),
	      this,
	      SLOT(slotFunctionReturnTypeChanged(const QString &,
						 const QString &,
						 glitch_object *)),
	      Qt::UniqueConnection);

      /*
      ** Does this function exist? If the function does not exist, it
      ** is not a clone.
      */

      auto view = qobject_cast<glitch_graphicsview *> (views().value(0));

      if(view && !view->containsFunction(function->name()))
	{
	  emit functionAdded(function->name(), false);
	  function->declone();
	}
      else
	emit functionAdded(function->name(), function->isClone());
    }

  emit sceneResized();
  return proxy;
}

void glitch_scene::addItem(QGraphicsItem *item)
{
  if(item && !item->scene())
    QGraphicsScene::addItem(item);

  auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (item);

  if(m_redoUndoProxies.contains(proxy) && proxy)
    m_redoUndoProxies[proxy] = 0;

  if(proxy && qobject_cast<glitch_object_function_arduino *> (proxy->widget()))
    emit functionAdded
      (qobject_cast<glitch_object_function_arduino *> (proxy->widget())->
       name(),
       qobject_cast<glitch_object_function_arduino *> (proxy->widget())->
       isClone());
}

void glitch_scene::artificialDrop(const QPointF &point, glitch_object *object)
{
  if(!object)
    return;

  auto proxy = addObject(object);

  if(proxy)
    {
      addItem(proxy);
      proxy->setPos(point);
    }
  else
    object->deleteLater();
}

void glitch_scene::bringToFront(glitch_proxy_widget *proxy)
{
  if(proxy)
    proxy->setZValue(1);
  else
    {
      QList<QGraphicsItem *> list(items());

      for(auto i : list)
	{
	  auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

	  if(proxy)
	    proxy->setZValue(0);
	}
    }
}

void glitch_scene::deleteFunctionClones(const QString &name)
{
  QList<QGraphicsItem *> list(items());

  for(auto i : list)
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(!proxy)
	continue;

      auto object = qobject_cast<glitch_object_function_arduino *>
	(proxy->widget());

      if(!object || !object->isClone())
	continue;

      if(name == object->name())
	{
	  if(m_undoStack)
	    {
	      auto undoCommand = new glitch_undo_command
		(glitch_undo_command::ITEM_DELETED, proxy, this);

	      m_undoStack->push(undoCommand);
	    }
	  else
	    {
	      removeItem(proxy);
	      proxy->deleteLater();
	    }
	}
    }
}

void glitch_scene::deleteItems(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QList<QGraphicsItem *> list;

  for(auto i : items())
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(!proxy)
	continue;
      else if(proxy->isMandatory() || !proxy->isSelected())
	continue;
      else
	list << i;
    }

  if(list.isEmpty())
    {
      QApplication::restoreOverrideCursor();
      return;
    }

  bool state = false;

  if(m_undoStack)
    m_undoStack->beginMacro(tr("widget(s) deleted"));

  for(auto i : list)
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(!proxy)
	continue;

      state = true;

      if(m_undoStack)
	{
	  auto object = qobject_cast<glitch_object_function_arduino *>
	    (proxy->widget());

	  if(object && !object->isClone())
	    {
	      deleteFunctionClones(object->name());
	      emit functionDeleted(object->name());
	    }

	  auto undoCommand = new glitch_undo_command
	    (glitch_undo_command::ITEM_DELETED, proxy, this);

	  m_undoStack->push(undoCommand);
	}
      else
	{
	  removeItem(proxy);
	  proxy->deleteLater();
	}
    }

  if(m_undoStack)
    m_undoStack->endMacro();

  QApplication::restoreOverrideCursor();

  if(state)
    emit changed();
}

void glitch_scene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
  if(event && event->mimeData())
    {
      QString text(event->mimeData()->text().trimmed());

      if(allowDrag(event, text))
	{
	  event->accept();
	  return;
	}
    }

  QGraphicsScene::dragEnterEvent(event);
}

void glitch_scene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
  if(event && event->mimeData())
    {
      QString text(event->mimeData()->text().trimmed());

      if(allowDrag(event, text))
	{
	  event->accept();
	  return;
	}
    }

  QGraphicsScene::dragMoveEvent(event);
}

void glitch_scene::drawBackground(QPainter *painter, const QRectF &rect)
{
  if(!painter)
    {
      QGraphicsScene::drawBackground(painter, rect);
      return;
    }

  /*
  ** Here be magical points!
  */

  painter->save();
  painter->fillRect(rect, backgroundBrush());
  painter->setBrushOrigin(0, 0);
  painter->restore();

  QPen pen;

  pen.setBrush(QColor(0xff - backgroundBrush().color().red(),
		      0xff - backgroundBrush().color().green(),
		      0xff - backgroundBrush().color().blue()));
  pen.setWidthF(1.00);
  painter->setPen(pen);

  QVector<QPointF> points;
  int gridSize = 20;
  qreal left = static_cast<int> (rect.left()) -
    (static_cast<int> (rect.left()) % gridSize);
  qreal top = static_cast<int> (rect.top()) -
    (static_cast<int> (rect.top()) % gridSize);

  for(qreal x = left; x < rect.right(); x += gridSize)
    for(qreal y = top; y < rect.bottom(); y += gridSize)
      points << QPointF(x, y);

  painter->save();
  painter->drawPoints(points.data(), points.size());
  painter->restore();
}

void glitch_scene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
  if(event && event->mimeData())
    {
      QString text(event->mimeData()->text().toLower().trimmed());
      glitch_object *object = nullptr;

      if(allowDrag(event, text))
	{
	  auto view = views().value(0);

	  if(text.startsWith("glitch-arduino-analogread()"))
	    object = new glitch_object_analog_read_arduino(view);
	  else if(text.startsWith("glitch-arduino-block comment"))
	    object = new glitch_object_block_comment_arduino(view);
	  else if(text.startsWith("glitch-arduino-and (&&)") ||
		  text.startsWith("glitch-arduino-not (!)") ||
		  text.startsWith("glitch-arduino-or (||)"))
	    {
	      object = new glitch_object_logical_operator_arduino(view);

	      if(qobject_cast
		 <glitch_object_logical_operator_arduino *> (object))
		qobject_cast<glitch_object_logical_operator_arduino *>
		  (object)->setOperatorType(text);
	    }
	  else if(text.startsWith("glitch-arduino-function"))
	    {
	      if(text == "glitch-arduino-function()")
		object = new glitch_object_function_arduino(view);
	      else
		object = new glitch_object_function_arduino
		  (text.
		   mid(static_cast<int> (qstrlen("glitch-arduino-function-"))),
		   view);
	    }
	}

      if(object)
	{
	  event->accept();
	  object->setUndoStack(m_undoStack);

	  auto proxy = addObject(object);

	  if(proxy)
	    {
	      if(m_undoStack)
		{
		  auto undoCommand = new glitch_undo_command
		    (glitch_undo_command::ITEM_ADDED, proxy, this);

		  undoCommand->setText
		    (tr("item added (%1, %2)").
		     arg(event->scenePos().x()).arg(event->scenePos().y()));
		  m_undoStack->push(undoCommand);
		}
	      else
		addItem(proxy);

	      proxy->setPos(event->scenePos());
	      emit changed();
	    }
	  else
	    object->deleteLater();

	  return;
	}
    }

  QGraphicsScene::dropEvent(event);
}

void glitch_scene::keyPressEvent(QKeyEvent *event)
{
  if(!event)
    {
      QGraphicsScene::keyPressEvent(event);
      return;
    }

  switch(event->key())
    {
    case Qt::Key_Down:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
      {
	QGraphicsView::ViewportUpdateMode updateMode =
	  QGraphicsView::MinimalViewportUpdate;
	QList<QGraphicsItem *> list(selectedItems());
	QPoint point;
	auto view = views().value(0);
	bool began = false;
	bool moved = false;
	int pixels = (event->modifiers() & Qt::ShiftModifier) ? 50 : 1;

	if(view)
	  {
	    updateMode = view->viewportUpdateMode();
	    view->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
	  }

	for(auto i : list)
	  {
	    auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

	    if(!proxy || !proxy->isMovable())
	      continue;

	    auto object = qobject_cast<glitch_object *> (proxy->widget());

	    if(!object)
	      continue;

	    point = object->pos();

	    switch(event->key())
	      {
	      case Qt::Key_Down:
		{
		  point.setY(point.y() + pixels);
		  break;
		}
	      case Qt::Key_Left:
		{
		  point.setX(point.x() - pixels);
		  break;
		}
	      case Qt::Key_Right:
		{
		  point.setX(point.x() + pixels);
		  break;
		}
	      case Qt::Key_Up:
		{
		  point.setY(point.y() - pixels);
		  break;
		}
	      default:
		{
		  break;
		}
	      }

	    if(point.x() < 0 || point.y() < 0)
	      continue;

	    QPointF previousPosition(proxy->scenePos());

	    object->move(point);

	    if(previousPosition != proxy->pos())
	      moved = true;

	    if(moved)
	      {
		if(!began)
		  {
		    began = true;

		    if(m_undoStack)
		      m_undoStack->beginMacro("widget(s) moved");
		  }

		if(m_undoStack)
		  {
		    auto undoCommand = new glitch_undo_command
		      (previousPosition,
		       glitch_undo_command::ITEM_MOVED,
		       proxy,
		       this);

		    m_undoStack->push(undoCommand);
		  }
	      }
	  }

	if(began)
	  {
	    if(m_undoStack)
	      m_undoStack->endMacro();

	    emit changed();
	  }

	if(moved)
	  emit sceneResized();

	if(view)
	  view->setViewportUpdateMode(updateMode);

	return;
      }
    default:
      {
	break;
      }
    }

  QGraphicsScene::keyPressEvent(event);
}

void glitch_scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  if(event && !m_lastScenePos.isNull())
    {
      QList<QGraphicsItem *> list(selectedItems());
      bool moved = false;

      for(auto i : list)
	{
	  auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

	  if(!proxy || !proxy->isMovable())
	    continue;

	  auto object = qobject_cast<glitch_object *> (proxy->widget());

	  if(object && object->mouseOverScrollBar(event->scenePos()))
	    continue;

	  QPointF point(proxy->mapToParent(event->scenePos() - m_lastScenePos));

	  if(point.x() < 0 || point.y() < 0)
	    continue;

	  moved = true;
	  proxy->setPos(point);
	}

      m_lastScenePos = event->scenePos();

      if(moved)
	{
	  emit changed();
	  emit sceneResized();
	  views().value(0)->viewport()->setCursor(Qt::ClosedHandCursor);
	}
    }

  QGraphicsScene::mouseMoveEvent(event);
}

void glitch_scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  m_lastScenePos = QPointF();
  m_movedPoints.clear();

  if(event)
    {
      auto item = itemAt(event->scenePos(), QTransform());

      if(item)
	{
	  auto parent = item->parentItem();

	  if(!parent)
	    parent = item;

	  if(!parent)
	    goto done_label;

	  auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (parent);

	  if(proxy)
	    {
	      auto object = proxy->widget();

	      if(object)
		{
		  QPoint point
		    (proxy->mapFromScene(event->scenePos()).toPoint());

		  if(qobject_cast<QComboBox *> (object->childAt(point)))
		    {
		      bringToFront(nullptr);
		      bringToFront(proxy);
		      goto done_label;
		    }
		  else if(qobject_cast<QToolButton *> (object->childAt(point)))
		    /*
		    ** Do not grab the item.
		    */

		    goto done_label;
		}
	    }

	  if(event->button() == Qt::RightButton)
	    {
	      if(!parent->isSelected())
		clearSelection();

	      event->setButton(Qt::LeftButton);
	      parent->setSelected(true);
	      goto done_label;
	    }

	  if(event->modifiers() & Qt::ControlModifier)
	    {
	      m_lastScenePos = event->scenePos();
	      parent->setSelected(!parent->isSelected());
	    }
	  else
	    {
	      if(!parent->isSelected())
		clearSelection();

	      m_lastScenePos = event->scenePos();
	      parent->setSelected(true);
	    }

	  if(!m_lastScenePos.isNull())
	    {
	      QList<QGraphicsItem *> list(selectedItems());

	      for(auto i : list)
		{
		  auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

		  if(!proxy || !proxy->isMovable())
		    continue;
		  else
		    m_movedPoints << QPair<QPointF, glitch_proxy_widget *>
		      (proxy->scenePos(), proxy);
		}
	    }
	}
      else
	clearSelection();
    }
  else
    clearSelection();

 done_label:
  QGraphicsScene::mousePressEvent(event);
}

void glitch_scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  m_lastScenePos = QPointF();

  if(!m_movedPoints.isEmpty() && m_undoStack)
    {
      bool began = false;

      for(const auto &m_movedPoint : m_movedPoints)
	{
	  if(!m_movedPoint.second)
	    continue;
	  else if(m_movedPoint.first ==
		  m_movedPoint.second->pos())
	    continue;
	  else if(!began)
	    {
	      began = true;
	      m_undoStack->beginMacro(tr("widget(s) moved"));
	    }

	  auto undoCommand = new glitch_undo_command
	    (m_movedPoint.first,
	     glitch_undo_command::ITEM_MOVED,
	     m_movedPoint.second,
	     this);

	  m_undoStack->push(undoCommand);
	}

      m_movedPoints.clear();

      if(began)
	{
	  m_undoStack->endMacro();
	  emit changed();
	}
    }

  m_movedPoints.clear();

  auto cursor = QApplication::overrideCursor();

  if(cursor)
    views().value(0)->viewport()->setCursor(cursor->shape());
  else
    views().value(0)->viewport()->setCursor(Qt::ArrowCursor);

  QGraphicsScene::mouseReleaseEvent(event);
}

void glitch_scene::purgeRedoUndoProxies(void)
{
  /*
  ** Delete proxies which are not on the scene. This method should only
  ** be used with care!
  */

  QList<QGraphicsItem *> list(items());
  QMutableHashIterator<glitch_proxy_widget *, char> it(m_redoUndoProxies);

  while(it.hasNext())
    {
      it.next();

      if(!list.contains(it.key()))
	{
	  if(it.key())
	    it.key()->deleteLater();

	  it.remove();
	}
    }
}

void glitch_scene::removeItem(QGraphicsItem *item)
{
  if(item && item->scene() == this)
    QGraphicsScene::removeItem(item);

  auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (item);

  if(proxy)
    {
      auto object = qobject_cast<glitch_object_function_arduino *>
	(proxy->widget());

      if(object && !object->isClone())
	emit functionDeleted(object->name());
    }
}

void glitch_scene::setMainScene(const bool state)
{
  m_mainScene = state;
}

void glitch_scene::setUndoStack(QUndoStack *undoStack)
{
  m_undoStack = undoStack;
}

void glitch_scene::slotFunctionNameChanged(const QString &after,
					   const QString &before,
					   glitch_object *object)
{
  Q_UNUSED(after);
  Q_UNUSED(before);
  Q_UNUSED(object);
}

void glitch_scene::slotFunctionReturnTypeChanged(const QString &after,
						 const QString &before,
						 glitch_object *object)
{
  Q_UNUSED(after);
  Q_UNUSED(before);
  Q_UNUSED(object);
}

void glitch_scene::slotObjectDeletedViaContextMenu(void)
{
  auto object = qobject_cast<glitch_object *> (sender());

  if(!object)
    return;

  if(m_undoStack && object->proxy())
    {
      if(qobject_cast<glitch_object_function_arduino *> (object) &&
	 !qobject_cast<glitch_object_function_arduino *> (object)->isClone())
	emit functionDeleted
	  (qobject_cast<glitch_object_function_arduino *> (object)->name());

      auto undoCommand = new glitch_undo_command
	(glitch_undo_command::ITEM_DELETED, object->proxy(), this);

      undoCommand->setText
	(tr("item deleted (%1, %2)").arg(object->x()).arg(object->y()));
      m_undoStack->push(undoCommand);
    }
  else
    object->deleteLater();

  emit changed();
}

void glitch_scene::slotRedo(void)
{
  if(m_undoStack && m_undoStack->canRedo())
    {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      m_undoStack->redo();
      emit changed();
      QApplication::restoreOverrideCursor();
    }
}

void glitch_scene::slotUndo(void)
{
  if(m_undoStack && m_undoStack->canUndo())
    {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      m_undoStack->undo();
      emit changed();
      QApplication::restoreOverrideCursor();
    }
}
