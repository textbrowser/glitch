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

#include <cmath>

#include "Arduino/glitch-object-advanced-io-arduino.h"
#include "Arduino/glitch-object-analog-io-arduino.h"
#include "Arduino/glitch-object-block-comment-arduino.h"
#include "Arduino/glitch-object-constant-arduino.h"
#include "Arduino/glitch-object-function-arduino.h"
#include "Arduino/glitch-object-logical-operator-arduino.h"
#include "Arduino/glitch-object-loop-flow-arduino.h"
#include "Arduino/glitch-object-variable-arduino.h"
#include "Arduino/glitch-structures-arduino.h"
#include "glitch-proxy-widget.h"
#include "glitch-scene.h"
#include "glitch-undo-command.h"
#include "glitch-graphicsview.h"

glitch_scene::glitch_scene(const glitch_common::ProjectTypes projectType,
			   QObject *parent):QGraphicsScene(parent)
{
  m_dotsColor = Qt::black;
  m_mainScene = false;
  m_projectType = projectType;
  m_showDots = true;
  m_toolsOperation = glitch_tools::Operations::SELECT;
  m_undoStack = nullptr;
}

glitch_scene::~glitch_scene()
{
}

QList<glitch_object *> glitch_scene::objects(void) const
{
  QList<glitch_object *> widgets;
  auto list(items());

  for(const auto i : list)
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(!proxy || !(proxy->flags() & QGraphicsItem::ItemIsSelectable))
	continue;

      widgets << qobject_cast<glitch_object *> (proxy->widget());
    }

  return widgets;
}

QList<glitch_object *> glitch_scene::orderedObjects(void) const
{
  QMap<int, glitch_object *> widgets;
  auto list(items());

  for(const auto i : list)
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(!proxy || !proxy->widget())
	continue;

      widgets[objectOrder(proxy)] = qobject_cast<glitch_object *>
	(proxy->widget());
    }

  return widgets.values();
}

QList<glitch_object *> glitch_scene::selectedObjects(void) const
{
  QList<glitch_object *> widgets;
  auto list(items());

  for(const auto i : list)
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
      auto text(t.trimmed().remove("glitch-"));

      text.remove(text.indexOf('-') + 1, text.lastIndexOf('-') - 7);

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
	    case glitch_common::ProjectTypes::ArduinoProject:
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
		  auto text(item->data(Qt::UserRole).toString().toLower());

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
  if(!object || object->proxy())
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
  connect(proxy,
	  &glitch_proxy_widget::changed,
	  this,
	  &glitch_scene::slotProxyChanged);

  if(object->scene())
    connect(this,
	    &glitch_scene::functionDeleted,
	    object->scene(),
	    &glitch_scene::slotFunctionDeleted,
	    Qt::UniqueConnection);

  object->setProxy(proxy);
  object->setUndoStack(m_undoStack);
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

glitch_tools::Operations glitch_scene::toolsOperation(void) const
{
  return m_toolsOperation;
}

void glitch_scene::addItem(QGraphicsItem *item)
{
  if(item && !item->scene())
    QGraphicsScene::addItem(item);

  auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (item);

  if(m_redoUndoProxies.contains(proxy) && proxy)
    m_redoUndoProxies[proxy] = 0;

  if(proxy && proxy->object() && proxy->object()->scene())
    connect(this,
	    &glitch_scene::functionDeleted,
	    proxy->object()->scene(),
	    &glitch_scene::slotFunctionDeleted,
	    Qt::UniqueConnection);

  if(proxy && qobject_cast<glitch_object_function_arduino *> (proxy->widget()))
    emit functionAdded
      (qobject_cast<glitch_object_function_arduino *> (proxy->widget())->
       name(),
       qobject_cast<glitch_object_function_arduino *> (proxy->widget())->
       isClone());

  recordProxyOrder(proxy);
}

void glitch_scene::artificialDrop(const QPointF &point, glitch_object *object)
{
  if(!object)
    return;

  QSignalBlocker blocker(this); // Are we creating from a database?
  auto proxy = addObject(object);

  if(proxy)
    {
      addItem(proxy);
      proxy->setPos(point);
    }
  else
    object->deleteLater();

  blocker.unblock();
}

void glitch_scene::bringToFront(glitch_proxy_widget *proxy)
{
  if(proxy)
    proxy->setZValue(1);
  else
    {
      auto list(items());

      for(const auto i : list)
	{
	  auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

	  if(proxy)
	    proxy->setZValue(0);
	}
    }
}

void glitch_scene::deleteFunctionClones(const QString &name)
{
  auto list(items());

  for(const auto i : list)
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

	      undoCommand->setText(tr("function clone deleted"));
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

  for(const auto i : items())
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

  auto state = false;

  if(m_undoStack)
    m_undoStack->beginMacro(tr("widget(s) deleted"));

  for(const auto i : list)
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
      auto text(event->mimeData()->text().trimmed());

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
      auto text(event->mimeData()->text().trimmed());

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
  if(!m_showDots || !painter)
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

  pen.setBrush(m_dotsColor);
  pen.setWidthF(1.00);
  painter->setPen(pen);

  QVector<QPointF> points;
  auto left = static_cast<qreal> (rect.left()) -
    (static_cast<int> (rect.left()) % 20);
  auto top = static_cast<qreal> (rect.top()) -
    (static_cast<int> (rect.top()) % 20);
  const int gridSize = 20;

  for(auto x = left; std::isless(x, rect.right()); x += gridSize)
    for(auto y = top; std::isless(y, rect.bottom()); y += gridSize)
      points << QPointF(x, y);

  painter->save();
  painter->drawPoints(points.data(), points.size());
  painter->restore();
}

void glitch_scene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
  if(event && event->mimeData())
    {
      auto text(event->mimeData()->text().toLower().trimmed());

      glitch_object *object = nullptr;

      if(allowDrag(event, text))
	{
	  auto view = views().value(0);

	  if(text.startsWith("glitch-arduino-advanced i/o-"))
	    object = new glitch_object_advanced_io_arduino(text, view);
	  if(text.startsWith("glitch-arduino-analog i/o-analog"))
	    object = new glitch_object_analog_io_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-constant"))
	    object = new glitch_object_constant_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-flow control-"))
	    object = new glitch_object_loop_flow_arduino(view);
	  else if(text.startsWith("glitch-arduino-function"))
	    object = new glitch_object_function_arduino
	      (text.mid(static_cast<int> (qstrlen("glitch-arduino-function-"))),
	       view);
	  else if(text.startsWith("glitch-arduino-logical operators"))
	    object = new glitch_object_logical_operator_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-structures-block comment"))
	    object = new glitch_object_block_comment_arduino(view);
	  else if(text.startsWith("glitch-arduino-structures-function"))
	    {
	      if(text == "glitch-arduino-structures-function()")
		object = new glitch_object_function_arduino(view);
	      else
		object = new glitch_object_function_arduino
		  (text.
		   mid(static_cast<int> (qstrlen("glitch-arduino-"
						 "structures-function-"))),
		   view);
	    }
	  else if(text.startsWith("glitch-arduino-variable"))
	    object = new glitch_object_variable_arduino(text, view);
	}

      if(object)
	{
	  event->accept();

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
	QPoint point;
	auto began = false;
	auto list(selectedItems());
	auto moved = false;
	auto updateMode = QGraphicsView::MinimalViewportUpdate;
	auto view = views().value(0);
	int pixels = (event->modifiers() & Qt::ShiftModifier) ? 50 : 1;

	if(view)
	  {
	    updateMode = view->viewportUpdateMode();
	    view->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
	  }

	for(const auto i : list)
	  {
	    auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

	    if(!proxy || !proxy->isMovable())
	      continue;

	    auto object = qobject_cast<glitch_object *> (proxy->widget());

	    if(!object)
	      continue;

	    if(qobject_cast<QLineEdit *> (object->focusWidget()) ||
	       qobject_cast<QPlainTextEdit *> (object->focusWidget()))
	      {
		QGraphicsScene::keyPressEvent(event);
		continue;
	      }

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

	    auto previousPosition(proxy->scenePos());

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
      auto list(selectedItems());
      auto moved = false;

      for(const auto i : list)
	{
	  auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

	  if(!proxy || !proxy->isMovable())
	    continue;

	  auto object = qobject_cast<glitch_object *> (proxy->widget());

	  if(object && object->mouseOverScrollBar(event->scenePos()))
	    continue;

	  auto point(proxy->mapToParent(event->scenePos() - m_lastScenePos));

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
  if(m_toolsOperation != glitch_tools::Operations::SELECT)
    goto done_label;

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
		  auto point(proxy->mapFromScene(event->scenePos()).toPoint());

		  if(qobject_cast<QComboBox *> (object->childAt(point)))
		    {
		      bringToFront(nullptr);
		      bringToFront(proxy);
		      goto done_label;
		    }
		  else if(qobject_cast<QLineEdit *> (object->childAt(point)))
		    {
		      qobject_cast<QLineEdit *> (object->childAt(point))->
			setFocus();
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
	      auto list(selectedItems());

	      for(const auto i : list)
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
  emit mousePressed();
}

void glitch_scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  m_lastScenePos = QPointF();

  if(!m_movedPoints.isEmpty() && m_undoStack)
    {
      auto began = false;

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

  QMutableHashIterator<glitch_proxy_widget *, char> it(m_redoUndoProxies);
  auto list(items());

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

void glitch_scene::recordProxyOrder(glitch_proxy_widget *proxy)
{
  if(!proxy || proxy->isMandatory())
    return;

  auto point(m_objectsHash.value(proxy));

  m_objectsHash[proxy] = glitch_point(proxy->pos());

  if(m_objectsMap.contains(point, proxy))
    m_objectsMap.remove(point, proxy);

  m_objectsMap.insert(glitch_point(proxy->pos()), proxy);
}

void glitch_scene::removeItem(QGraphicsItem *item)
{
  /*
  ** Do not add removed items to redo/undo stack here.
  */

  if(item && item->scene() == this)
    QGraphicsScene::removeItem(item);

  auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (item);

  if(proxy)
    {
      auto object = qobject_cast<glitch_object_function_arduino *>
	(proxy->widget());

      if(object && !object->isClone())
	{
	  deleteFunctionClones(object->name());
	  emit functionDeleted(object->name());
	}

      /*
      ** Remove object order information.
      */

      auto point(m_objectsHash.value(proxy));

      m_objectsHash.remove(proxy);
      m_objectsMap.remove(point, proxy);
    }
}

void glitch_scene::setDotsColor(const QColor &color)
{
  if(color.isValid())
    m_dotsColor = color;
  else
    m_dotsColor = Qt::black;
}

void glitch_scene::setMainScene(const bool state)
{
  m_mainScene = state;
}

void glitch_scene::setShowDots(const bool state)
{
  m_showDots = state;
}

void glitch_scene::setUndoStack(QUndoStack *undoStack)
{
  m_undoStack = undoStack;
}

void glitch_scene::slotFunctionDeleted(const QString &name)
{
  foreach(auto object, objects())
    if(object && object->name() == name && object->type().contains("function"))
      {
	if(m_undoStack)
	  {
	    auto undoCommand = new glitch_undo_command
	      (glitch_undo_command::ITEM_DELETED, object->proxy(), this);

	    undoCommand->setText(tr("function clone deleted"));
	    m_undoStack->push(undoCommand);
	  }
	else
	  removeItem(object->proxy());

	break;
      }
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
      m_undoStack->beginMacro(tr("widget(s) deleted"));

      if(qobject_cast<glitch_object_function_arduino *> (object) &&
	 !qobject_cast<glitch_object_function_arduino *> (object)->isClone())
	{
	  deleteFunctionClones(object->name());
	  emit functionDeleted
	    (qobject_cast<glitch_object_function_arduino *> (object)->name());
	}

      auto undoCommand = new glitch_undo_command
	(glitch_undo_command::ITEM_DELETED, object->proxy(), this);

      m_undoStack->push(undoCommand);
      m_undoStack->endMacro();
    }
  else
    object->deleteLater();

  emit changed();
}

void glitch_scene::slotProxyChanged(void)
{
  recordProxyOrder(qobject_cast<glitch_proxy_widget *> (sender()));
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

void glitch_scene::slotToolsOperationChanged
(const glitch_tools::Operations operation)
{
  clearSelection();
  m_toolsOperation = operation;
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
