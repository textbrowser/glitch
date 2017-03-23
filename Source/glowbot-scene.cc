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
#include "glowbot-proxy-widget.h"
#include "glowbot-scene.h"

glowbot_scene::glowbot_scene(QObject *parent):QGraphicsScene(parent)
{
  m_changed = false;
  connect(this,
	  SIGNAL(changed(void)),
	  this,
	  SLOT(slotChanged(void)));
}

glowbot_scene::~glowbot_scene()
{
}

bool glowbot_scene::hasChanged(void) const
{
  return m_changed;
}

void glowbot_scene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
  if(event && event->mimeData())
    {
      QString text(event->mimeData()->text().toLower().trimmed());

      if(text.startsWith("glowbot-"))
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

      if(text.contains("glowbot-"))
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

      if(text == "glowbot-arduino-analogread()")
	object = new glowbot_object_analog_read_arduino(views().value(0));

      if(object)
	{
	  event->accept();

	  glowbot_proxy_widget *proxy = new glowbot_proxy_widget();

	  proxy->setFlags
	    (QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
	  proxy->setWidget(object);
	  addItem(proxy);
	  proxy->setPos(event->scenePos());
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

      for(int i = 0; i < list.size(); i++)
	{
	  glowbot_proxy_widget *proxy =
	    qgraphicsitem_cast<glowbot_proxy_widget *> (list.at(i));

	  if(!proxy)
	    continue;
	  else if(!(proxy->flags() & QGraphicsItem::ItemIsMovable))
	    continue;

	  QPoint point
	    (proxy->mapToParent(event->scenePos() - m_lastScenePos).toPoint());

	  proxy->setPos(point);
	}

      m_lastScenePos = event->scenePos();
      return;
    }
  else
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
  QGraphicsScene::mouseReleaseEvent(event);
}

void glowbot_scene::slotChanged(void)
{
  m_changed = true;
}
