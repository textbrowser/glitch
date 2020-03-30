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

#include <QComboBox>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QToolButton>
#include <QtDebug>

#include "glitch-object.h"
#include "glitch-proxy-widget.h"

glitch_proxy_widget::glitch_proxy_widget
(QGraphicsItem *parent, Qt::WindowFlags wFlags):
  QGraphicsProxyWidget(parent, wFlags)
{
}

glitch_proxy_widget::~glitch_proxy_widget()
{
  qDebug() << "Destroyed!";
}

bool glitch_proxy_widget::isMandatory(void) const
{
  auto *object = qobject_cast<glitch_object *> (widget());

  if(object)
    return object->isMandatory();
  else
    return false;
}

bool glitch_proxy_widget::isMovable(void) const
{
  return flags() & QGraphicsItem::ItemIsMovable;
}

void glitch_proxy_widget::contextMenuEvent
(QGraphicsSceneContextMenuEvent *event)
{
  if(event)
    {
      auto *object = qobject_cast<glitch_object *> (widget());

      if(object)
	{
	  QMenu menu;

	  object->addActions(menu);
	  menu.exec(event->screenPos());
	}
      else
	QGraphicsProxyWidget::contextMenuEvent(event);
    }
  else
    QGraphicsProxyWidget::contextMenuEvent(event);
}

void glitch_proxy_widget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  if(event && (event->buttons() & Qt::RightButton))
    /*
    ** Activate a context menu.
    */

    return;

  if(!widget())
    {
      QGraphicsProxyWidget::mousePressEvent(event);
      return;
    }

  auto *comboBox = qobject_cast<QComboBox *>
    (widget()->childAt(event->pos().toPoint()));

  if(comboBox)
    {
      QGraphicsProxyWidget::mousePressEvent(event);
      return;
    }

  auto *toolButton = qobject_cast<QToolButton *>
    (widget()->childAt(event->pos().toPoint()));

  if(toolButton)
    {
      QGraphicsProxyWidget::mousePressEvent(event);
      return;
    }
}

void glitch_proxy_widget::paint
(QPainter *painter, const QStyleOptionGraphicsItem *opt, QWidget *widget)
{
  if(painter)
    painter->setRenderHints(QPainter::Antialiasing |
			    QPainter::HighQualityAntialiasing | // OpenGL?
			    QPainter::SmoothPixmapTransform |
			    QPainter::TextAntialiasing,
			    true);

  if(opt && (opt->state & QStyle::State_Selected) && painter)
    {
      QPen pen;

      if(isMovable())
	pen.setColor(Qt::green);
      else
	pen.setColor(Qt::red);

      pen.setJoinStyle(Qt::MiterJoin);
      pen.setStyle(Qt::DashLine);
      pen.setWidthF(6.5);
      painter->setPen(pen);
      painter->drawRect(boundingRect());
    }

  QGraphicsProxyWidget::paint(painter, opt, widget);
}
