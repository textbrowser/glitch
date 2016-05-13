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

#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include "glowbot-object.h"
#include "glowbot-proxy-widget.h"

glowbot_proxy_widget::glowbot_proxy_widget
(QGraphicsItem *parent, Qt::WindowFlags wFlags):
  QGraphicsProxyWidget(parent, wFlags)
{
}

glowbot_proxy_widget::~glowbot_proxy_widget()
{
}

void glowbot_proxy_widget::contextMenuEvent
(QGraphicsSceneContextMenuEvent *event)
{
  if(event)
    {
      glowbot_object *object = qobject_cast<glowbot_object *> (widget());

      if(object)
	{
	  QMenu menu;

	  object->addActions(menu);

	  if(menu.actions().isEmpty())
	    QGraphicsProxyWidget::contextMenuEvent(event);
	  else
	    menu.exec(event->screenPos());
	}
      else
	QGraphicsProxyWidget::contextMenuEvent(event);
    }
  else
    QGraphicsProxyWidget::contextMenuEvent(event);
}

void glowbot_proxy_widget::paint
(QPainter *painter, const QStyleOptionGraphicsItem *opt, QWidget *widget)
{
  if(opt && (opt->state & QStyle::State_Selected) && painter)
    {
      QPen pen;

      pen.setBrush(QColor(70, 130, 180));
      pen.setWidth(3);
      painter->setPen(pen);
      painter->drawRect(boundingRect());
    }

  QGraphicsProxyWidget::paint(painter, opt, widget);
}
