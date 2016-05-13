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

#include "glowbot-scene.h"

glowbot_scene::glowbot_scene(QObject *parent):QGraphicsScene(parent)
{
}

glowbot_scene::~glowbot_scene()
{
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
