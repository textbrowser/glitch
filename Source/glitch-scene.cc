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
#include "Arduino/glitch-object-function-arduino.h"
#include "Arduino/glitch-structures-arduino.h"
#include "glitch-proxy-widget.h"
#include "glitch-scene.h"
#include "glitch-undo-command.h"

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

  for(int i = 0; i < list.size(); i++)
    {
      glitch_proxy_widget *proxy =
	qgraphicsitem_cast<glitch_proxy_widget *> (list.at(i));

      if(!proxy || !(proxy->flags() & Qt::ItemIsSelectable))
	continue;

      widgets << qobject_cast<glitch_object *> (proxy->widget());
    }

  return widgets;
}

QList<glitch_object *> glitch_scene::selectedObjects(void) const
{
  QList<QGraphicsItem *> list(items());
  QList<glitch_object *> widgets;

  for(int i = 0; i < list.size(); i++)
    {
      glitch_proxy_widget *proxy =
	qgraphicsitem_cast<glitch_proxy_widget *> (list.at(i));

      if(!proxy ||
	 !(proxy->flags() & Qt::ItemIsSelectable) ||
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

bool glitch_scene::allowDrag(QGraphicsSceneDragDropEvent *event,
			     const QString &text)
{
  if(!event)
    return false;
  else
    {
      if(m_mainScene)
	{
	  QString t(text.toLower().trimmed());

	  if(t.startsWith("glitch-arduino-analogread()") ||
	     t.startsWith("glitch-arduino-function()"))
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
		QString t(text.toLower().trimmed());

		if(glitch_structures_arduino::
		   containsStructure(t.mid(QString("glitch-").length())))
		  /*
		  ** Functions cannot be defined in other functions.
		  */

		  if(!t.startsWith("glitch-arduino-function()"))
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

	  QTableWidget *tableWidget = qobject_cast<QTableWidget *>
	    (event->source());

	  if(tableWidget)
	    {
	      QTableWidgetItem *item = tableWidget->currentItem();

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

  glitch_proxy_widget *proxy = new glitch_proxy_widget();

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
  object->setProperty("movable", true);
  object->setProxy(proxy);
  proxy->setFlag(QGraphicsItem::ItemIsMovable, true);
  proxy->setFlag(QGraphicsItem::ItemIsSelectable, true);
  proxy->setWidget(object);
  emit changed();

  if(qobject_cast<glitch_object_function_arduino *> (object))
    {
      connect(object,
	      SIGNAL(nameChanged(const QString &,
				 const QString &,
				 glitch_object *)),
	      this,
	      SIGNAL(functionNameChanged(const QString &,
					 const QString &,
					 glitch_object *)),
	      Qt::UniqueConnection);
      emit functionAdded
	(qobject_cast<glitch_object_function_arduino *> (object)->name());
    }

  emit sceneResized();
  return proxy;
}

void glitch_scene::addItem(QGraphicsItem *item)
{
  QGraphicsScene::addItem(item);

  glitch_proxy_widget *proxy = qgraphicsitem_cast<glitch_proxy_widget *> (item);

  if(m_redoUndoProxies.contains(proxy) && proxy)
    m_redoUndoProxies[proxy] = 0;

  if(proxy && qobject_cast<glitch_object_function_arduino *> (proxy->widget()))
    emit functionAdded
      (qobject_cast<glitch_object_function_arduino *> (proxy->widget())->
       name());
}

void glitch_scene::artificialDrop(const QPointF &point, glitch_object *object)
{
  if(!object)
    return;

  glitch_proxy_widget *proxy = addObject(object);

  if(proxy)
    {
      addItem(proxy);
      proxy->setPos(point);
    }
  else
    object->deleteLater();
}

void glitch_scene::deleteItems(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QList<QGraphicsItem *> list(items());
  bool state = false;

  if(m_undoStack)
    m_undoStack->beginMacro(tr("items deleted"));

  for(int i = 0; i < list.size(); i++)
    {
      glitch_proxy_widget *proxy =
	qgraphicsitem_cast<glitch_proxy_widget *> (list.at(i));

      if(!proxy)
	continue;
      else if(proxy->isMandatory() || !proxy->isSelected())
	continue;

      state = true;

      if(m_undoStack)
	{
	  if(qobject_cast<glitch_object_function_arduino *> (proxy->widget()))
	    emit functionDeleted
	      (qobject_cast
	       <glitch_object_function_arduino *> (proxy->widget())->name());

	  glitch_undo_command *undoCommand = new glitch_undo_command
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
      points.append(QPointF(x, y));

  painter->save();
  painter->drawPoints(points.data(), points.size());
  painter->restore();
}

void glitch_scene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
  if(event && event->mimeData())
    {
      QString text(event->mimeData()->text().trimmed());
      glitch_object *object = nullptr;

      if(allowDrag(event, text))
	{
	  text = text.toLower();

	  if(text.startsWith("glitch-arduino-analogread()"))
	    object = new glitch_object_analog_read_arduino(views().value(0));
	  else if(text.startsWith("glitch-arduino-function"))
	    {
	      if(text == "glitch-arduino-function()")
		object = new glitch_object_function_arduino(views().value(0));
	      else
		object = new glitch_object_function_arduino
		  (text.mid(25), views().value(0));
	    }
	}

      if(object)
	{
	  event->accept();
	  object->setUndoStack(m_undoStack);

	  glitch_proxy_widget *proxy = addObject(object);

	  if(proxy)
	    {
	      if(m_undoStack)
		{
		  glitch_undo_command *undoCommand = new glitch_undo_command
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
	QGraphicsView *view = views().value(0);
	QGraphicsView::ViewportUpdateMode updateMode =
	  QGraphicsView::MinimalViewportUpdate;
	QList<QGraphicsItem*> list(selectedItems());
	QPoint point;
	bool began = false;
	bool moved = false;
	int pixels = (event->modifiers() & Qt::ShiftModifier) ? 50 : 1;

	if(view)
	  {
	    updateMode = view->viewportUpdateMode();
	    view->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
	  }

	for(int i = 0; i < list.size(); i++)
	  {
	    glitch_proxy_widget *proxy =
	      qgraphicsitem_cast<glitch_proxy_widget *> (list.at(i));

	    if(!proxy || !proxy->isMovable())
	      continue;

	    glitch_object *widget =
	      qobject_cast<glitch_object *> (proxy->widget());

	    if(!widget)
	      continue;

	    point = widget->pos();

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

	    widget->move(point);

	    if(previousPosition != proxy->pos())
	      moved = true;

	    if(moved)
	      {
		if(!began)
		  {
		    began = true;

		    if(m_undoStack)
		      m_undoStack->beginMacro("widgets moved");
		  }

		if(m_undoStack)
		  {
		    glitch_undo_command *undoCommand = new glitch_undo_command
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

      for(int i = 0; i < list.size(); i++)
	{
	  glitch_proxy_widget *proxy =
	    qgraphicsitem_cast<glitch_proxy_widget *> (list.at(i));

	  if(!proxy || !proxy->widget())
	    continue;
	  else if(!proxy->isMovable())
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
      QGraphicsItem *item = itemAt(event->scenePos(), QTransform());

      if(item)
	{
	  QGraphicsItem *parent = item->parentItem();

	  if(!parent)
	    parent = item;

	  if(!parent)
	    goto done_label;

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

	      for(int i = 0; i < list.size(); i++)
		{
		  glitch_proxy_widget *proxy =
		    qgraphicsitem_cast<glitch_proxy_widget *> (list.at(i));

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

      while(!m_movedPoints.isEmpty())
	{
	  if(!m_movedPoints.first().second)
	    {
	      m_movedPoints.removeFirst();
	      continue;
	    }
	  else if(m_movedPoints.first().first ==
		  m_movedPoints.first().second->pos())
	    {
	      m_movedPoints.removeFirst();
	      continue;
	    }
	  else if(!began)
	    {
	      began = true;
	      m_undoStack->beginMacro(tr("items moved"));
	    }

	  glitch_undo_command *undoCommand = new glitch_undo_command
	    (m_movedPoints.first().first,
	     glitch_undo_command::ITEM_MOVED,
	     m_movedPoints.first().second,
	     this);

	  m_movedPoints.removeFirst();
	  m_undoStack->push(undoCommand);
	}

      if(began)
	{
	  m_undoStack->endMacro();
	  emit changed();
	}
    }

  m_movedPoints.clear();

  QCursor *cursor = QApplication::overrideCursor();

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
  QGraphicsScene::removeItem(item);

  glitch_proxy_widget *proxy = qgraphicsitem_cast<glitch_proxy_widget *> (item);

  if(proxy && qobject_cast<glitch_object_function_arduino *> (proxy->widget()))
    emit functionDeleted
      (qobject_cast<glitch_object_function_arduino *> (proxy->widget())->
       name());
}

void glitch_scene::setMainScene(const bool state)
{
  m_mainScene = state;
}

void glitch_scene::setUndoStack(QUndoStack *undoStack)
{
  m_undoStack = undoStack;
}

void glitch_scene::slotObjectDeletedViaContextMenu(void)
{
  glitch_object *object = qobject_cast<glitch_object *> (sender());

  if(!object)
    return;

  if(m_undoStack && object->proxy())
    {
      if(qobject_cast<glitch_object_function_arduino *> (object))
	emit functionDeleted
	  (qobject_cast<glitch_object_function_arduino *> (object)->name());

      glitch_undo_command *undoCommand = new glitch_undo_command
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
