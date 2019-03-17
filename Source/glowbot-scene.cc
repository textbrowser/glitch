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

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QMimeData>
#include <QTableWidget>
#include <QUndoStack>
#include <QtDebug>

#include "Arduino/glowbot-object-analog-read-arduino.h"
#include "Arduino/glowbot-object-function-arduino.h"
#include "Arduino/glowbot-structures-arduino.h"
#include "glowbot-proxy-widget.h"
#include "glowbot-scene.h"
#include "glowbot-undo-command.h"

glowbot_scene::glowbot_scene(const glowbot_common::ProjectType projectType,
			     QObject *parent):QGraphicsScene(parent)
{
  m_mainScene = false;
  m_projectType = projectType;
  m_undoStack = nullptr;
}

glowbot_scene::~glowbot_scene()
{
}

QList<glowbot_object *> glowbot_scene::objects(void) const
{
  QList<QGraphicsItem *> list(items());
  QList<glowbot_object *> widgets;

  for(int i = 0; i < list.size(); i++)
    {
      glowbot_proxy_widget *proxy =
	qgraphicsitem_cast<glowbot_proxy_widget *> (list.at(i));

      if(!proxy || !(proxy->flags() & Qt::ItemIsSelectable))
	continue;

      widgets << qobject_cast<glowbot_object *> (proxy->widget());
    }

  return widgets;
}

QList<glowbot_object *> glowbot_scene::selectedObjects(void) const
{
  QList<QGraphicsItem *> list(items());
  QList<glowbot_object *> widgets;

  for(int i = 0; i < list.size(); i++)
    {
      glowbot_proxy_widget *proxy =
	qgraphicsitem_cast<glowbot_proxy_widget *> (list.at(i));

      if(!proxy ||
	 !(proxy->flags() & Qt::ItemIsSelectable) ||
	 !proxy->isSelected())
	continue;

      widgets << qobject_cast<glowbot_object *> (proxy->widget());
    }

  return widgets;
}

bool glowbot_scene::allowDrag(QGraphicsSceneDragDropEvent *event,
			      const QString &text)
{
  if(!event)
    return false;
  else
    {
      if(m_mainScene)
	{
	  if(text.startsWith("glowbot-arduino-function()"))
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
	    case glowbot_common::ArduinoProject:
	      {
		/*
		** glowbot-
		*/

		if(glowbot_structures_arduino::containsStructure(text.mid(8)))
		  return true;

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

	      if(item &&
		 item->data(Qt::UserRole).toString() == "glowbot-user-function")
		{
		  event->accept();
		  return true;
		}
	    }

	  event->ignore();
	  return false;
	}
    }
}

glowbot_proxy_widget *glowbot_scene::addObject
(const QPointF &point, glowbot_object *object)
{
  if(!object)
    return nullptr;

  glowbot_proxy_widget *proxy = new glowbot_proxy_widget();

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
  proxy->setFlags
    (QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
  proxy->setWidget(object);
  object->move(point.toPoint());
  object->setProxy(proxy);
  proxy->setPos(point);
  emit changed();

  if(qobject_cast<glowbot_object_function_arduino *> (object))
    {
      connect(object,
	      SIGNAL(nameChanged(const QString &, const QString &)),
	      this,
	      SIGNAL(functionNameChanged(const QString &, const QString &)),
	      Qt::UniqueConnection);
      emit functionAdded
	(qobject_cast<glowbot_object_function_arduino *> (object)->name());
    }

  emit sceneResized();
  return proxy;
}

void glowbot_scene::deleteItems(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QList<QGraphicsItem *> list(items());
  bool state = false;

  if(m_undoStack)
    m_undoStack->beginMacro(tr("items deleted"));

  for(int i = 0; i < list.size(); i++)
    {
      glowbot_proxy_widget *proxy =
	qgraphicsitem_cast<glowbot_proxy_widget *> (list.at(i));

      if(!proxy)
	continue;
      else if(proxy->isMandatory() || !proxy->isSelected())
	continue;

      state = true;

      if(m_undoStack)
	{
	  glowbot_undo_command *undoCommand = new glowbot_undo_command
	    (glowbot_undo_command::ITEM_DELETED, proxy, this);

	  m_undoStack->push(undoCommand);
	}
    }

  if(m_undoStack)
    m_undoStack->endMacro();

  QApplication::restoreOverrideCursor();

  if(state)
    emit changed();
}

void glowbot_scene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
  if(event && event->mimeData())
    {
      QString text(event->mimeData()->text().toLower().trimmed());

      if(allowDrag(event, text))
	{
	  event->accept();
	  return;
	}
    }

  QGraphicsScene::dragEnterEvent(event);
}

void glowbot_scene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
  if(event && event->mimeData())
    {
      QString text(event->mimeData()->text().toLower().trimmed());

      if(allowDrag(event, text))
	{
	  event->accept();
	  return;
	}
    }

  QGraphicsScene::dragMoveEvent(event);
}

void glowbot_scene::drawBackground(QPainter *painter, const QRectF &rect)
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

void glowbot_scene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
  if(event && event->mimeData())
    {
      QString text(event->mimeData()->text().toLower().trimmed());
      glowbot_object *object = nullptr;

      if(allowDrag(event, text))
	{
	  if(text == "glowbot-arduino-analogread()")
	    object = new glowbot_object_analog_read_arduino(views().value(0));
	  else if(text == "glowbot-arduino-function()")
	    object = new glowbot_object_function_arduino(views().value(0));
	}

      if(object)
	{
	  event->accept();

	  glowbot_proxy_widget *proxy = addObject(event->scenePos(), object);

	  if(proxy)
	    {
	      glowbot_undo_command *undoCommand = new glowbot_undo_command
		(glowbot_undo_command::ITEM_ADDED, proxy, this);

	      undoCommand->setText
		(tr("item added (%1, %2)").arg(proxy->x()).arg(proxy->y()));
	      m_undoStack->push(undoCommand);
	    }
	  else
	    object->deleteLater();

	  emit changed();
	  return;
	}
    }

  QGraphicsScene::dropEvent(event);
}

void glowbot_scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  if(event && !m_lastScenePos.isNull())
    {
      QList<QGraphicsItem *> list(selectedItems());
      bool moved = false;

      for(int i = 0; i < list.size(); i++)
	{
	  glowbot_proxy_widget *proxy =
	    qgraphicsitem_cast<glowbot_proxy_widget *> (list.at(i));

	  if(!proxy || !proxy->widget())
	    continue;
	  else if(!(proxy->flags() & QGraphicsItem::ItemIsMovable))
	    continue;

	  QPointF point(proxy->mapToParent(event->scenePos() - m_lastScenePos));

	  if(point.x() < 0 || point.y() < 0)
	    continue;

	  moved = true;
	  proxy->setPos(point);
	  proxy->widget()->move(point.toPoint());
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

void glowbot_scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
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
		  glowbot_proxy_widget *proxy =
		    qgraphicsitem_cast<glowbot_proxy_widget *> (list.at(i));

		  if(!proxy)
		    continue;
		  else if(!(proxy->flags() & QGraphicsItem::ItemIsMovable))
		    continue;
		  else
		    m_movedPoints << QPair<QPointF, glowbot_proxy_widget *>
		      (proxy->pos(), proxy);
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

void glowbot_scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  m_lastScenePos = QPointF();

  if(!m_movedPoints.isEmpty() && m_undoStack)
    {
      bool began = false;

      while(!m_movedPoints.isEmpty())
	{
	  if(m_movedPoints.first().first == m_movedPoints.first().second->pos())
	    {
	      m_movedPoints.removeFirst();
	      continue;
	    }
	  else
	    {
	      if(!began)
		m_undoStack->beginMacro(tr("items moved"));

	      began = true;
	    }

	  glowbot_undo_command *undoCommand = new glowbot_undo_command
	    (m_movedPoints.first().first,
	     glowbot_undo_command::ITEM_MOVED,
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

void glowbot_scene::setMainScene(const bool state)
{
  m_mainScene = state;
}

void glowbot_scene::setUndoStack(QUndoStack *undoStack)
{
  m_undoStack = undoStack;
}

void glowbot_scene::slotObjectDeletedViaContextMenu(void)
{
  glowbot_object *object = qobject_cast<glowbot_object *> (sender());

  if(!object || !object->proxy())
    return;

  if(m_undoStack)
    {
      glowbot_undo_command *undoCommand = new glowbot_undo_command
	(glowbot_undo_command::ITEM_DELETED, object->proxy(), this);

      undoCommand->setText
	(tr("item deleted (%1, %2)").arg(object->x()).arg(object->y()));
      m_undoStack->push(undoCommand);
    }

  emit changed();
}
