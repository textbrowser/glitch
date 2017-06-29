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

#include "glowbot-object-analog-read-arduino.h"
#include "glowbot-object-function-arduino.h"
#include "glowbot-proxy-widget.h"
#include "glowbot-scene.h"

glowbot_scene::glowbot_scene(QObject *parent):QGraphicsScene(parent)
{
  m_mainScene = false;
}

glowbot_scene::~glowbot_scene()
{
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
	  if(text.startsWith("glowbot-arduino-analogread()"))
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
    }
}

void glowbot_scene::addObject(const QPointF &point, glowbot_object *object)
{
  if(!object)
    return;

  glowbot_proxy_widget *proxy = new glowbot_proxy_widget();

  connect(object,
	  SIGNAL(changed(void)),
	  this,
	  SIGNAL(changed(void)),
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
  addItem(proxy);
  object->move(point.toPoint());
  proxy->setPos(point);
  emit changed();

  if(qobject_cast<glowbot_object_function_arduino *> (object))
    {
      connect(object,
	      SIGNAL(nameChanged(const QString &, const QString &)),
	      this,
	      SIGNAL(functionNameChanged(const QString &,
					 const QString &)),
	      Qt::UniqueConnection);
      emit functionAdded
	(qobject_cast<glowbot_object_function_arduino *> (object)->name());
    }

  emit sceneResized();
}

void glowbot_scene::deleteItems(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QList<QGraphicsItem *> list(items());
  bool state = false;

  for(int i = 0; i < list.size(); i++)
    {
      glowbot_proxy_widget *proxy =
	qgraphicsitem_cast<glowbot_proxy_widget *> (list.at(i));

      if(!proxy)
	continue;
      else if(proxy->isMandatory() || !proxy->isSelected())
	continue;

      removeItem(proxy);
      delete proxy;
      state = true;
    }

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

void glowbot_scene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
  if(event && event->mimeData())
    {
      QString text(event->mimeData()->text().toLower().trimmed());
      glowbot_object *object = 0;

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
	  addObject(event->scenePos(), object);
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

	  if(!proxy)
	    continue;
	  else if(!(proxy->flags() & QGraphicsItem::ItemIsMovable))
	    continue;

	  QPointF point(proxy->mapToParent(event->scenePos() - m_lastScenePos));

	  if(point.x() < 0 || point.y() < 0)
	    continue;

	  moved = true;
	  proxy->setPos(point);

	  if(proxy->widget())
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
