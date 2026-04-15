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

#include <QGraphicsSceneMouseEvent>
#include <QtMath>

#include "glitch-proxy-widget.h"
#include "glitch-resize-widget-rectangle.h"

qreal glitch_resize_widget_rectangle::SQUARE_SIZE = 8.0;

glitch_resize_widget_rectangle::glitch_resize_widget_rectangle
(QGraphicsItem *parent, const RectangleLocations location):
  QGraphicsRectItem(parent)
{
  m_location = location;
  m_parentLocked = false;
  setAcceptHoverEvents(true);
}

glitch_resize_widget_rectangle::~glitch_resize_widget_rectangle()
{
}

glitch_resize_widget_rectangle::RectangleLocations
glitch_resize_widget_rectangle::location(void) const
{
  return m_location;
}

void glitch_resize_widget_rectangle::hoverEnterEvent
(QGraphicsSceneHoverEvent *event)
{
  switch(m_location)
    {
    case RectangleLocations::BottomCenter:
    case RectangleLocations::TopCenter:
      {
	setCursor(Qt::SizeVerCursor);
	break;
      }
    case RectangleLocations::BottomLeft:
    case RectangleLocations::TopRight:
      {
	if(m_parentLocked)
	  {
	    if(m_location == RectangleLocations::BottomLeft)
	      setCursor(Qt::SizeVerCursor);
	    else
	      setCursor(Qt::SizeHorCursor);
	  }
	else
	  setCursor(Qt::SizeBDiagCursor);

	break;
      }
    case RectangleLocations::BottomRight:
    case RectangleLocations::TopLeft:
      {
	setCursor(Qt::SizeFDiagCursor);
	break;
      }
    case RectangleLocations::CenterLeft:
    case RectangleLocations::CenterRight:
      {
	setCursor(Qt::SizeHorCursor);
	break;
      }
    default:
      {
	break;
      }
    }

  QGraphicsRectItem::hoverEnterEvent(event);
}

void glitch_resize_widget_rectangle::hoverLeaveEvent
(QGraphicsSceneHoverEvent *event)
{
  unsetCursor();
  QGraphicsRectItem::hoverLeaveEvent(event);
}

void glitch_resize_widget_rectangle::mouseMoveEvent
(QGraphicsSceneMouseEvent *event)
{
  if(!event)
    {
      QGraphicsRectItem::mouseMoveEvent(event);
      return;
    }

  auto parent = qgraphicsitem_cast<glitch_proxy_widget *> (parentItem());

  if(!parent)
    {
      QGraphicsRectItem::mouseMoveEvent(event);
      return;
    }

  auto const diagonal = qCeil(SQUARE_SIZE * qPow(2.0, 0.5));
  auto rectangle(parent->boundingRect());

  switch(m_location)
    {
    case RectangleLocations::BottomCenter:
      {
	auto const delta = -event->lastPos().y() + event->pos().y();

	rectangle.setHeight(delta + rectangle.height());
	rectangle.setHeight(qMax(parent->minimumHeight(), rectangle.height()));

	if(parent->minimumHeight() > rectangle.height())
	  /*
	  ** Do not move the widget.
	  */

	  return;

	parent->setGeometry(parent->mapToScene(rectangle).boundingRect());
	break;
      }
    case RectangleLocations::BottomLeft:
      {
	auto const delta = -event->lastPos().y() + event->pos().y();

	rectangle = parent->mapToScene(rectangle).boundingRect();
	rectangle.setHeight(delta + rectangle.height());
	rectangle.setHeight(qMax(parent->minimumHeight(), rectangle.height()));

	if(!m_parentLocked)
	  {
	    auto const delta = -event->lastScenePos().x() +
	      event->scenePos().x();

	    rectangle.setX(qMax(0.0, delta + rectangle.left()));

	    if(parent->minimumWidth() > rectangle.width())
	      rectangle.setX(-parent->minimumWidth() + rectangle.right());
	  }

	if(parent->minimumHeight() > rectangle.height())
	  rectangle.setY(-parent->minimumHeight() + rectangle.top());

	parent->setGeometry(rectangle);
	break;
      }
    case RectangleLocations::BottomRight:
      {
	auto const delta1 = -event->lastPos().x() + event->pos().x();
	auto const delta2 = -event->lastPos().y() + event->pos().y();

	rectangle.setHeight(delta2 + rectangle.height());
	rectangle.setHeight(qMax(parent->minimumHeight(), rectangle.height()));
	rectangle.setWidth(delta1 + rectangle.right());
	rectangle.setWidth(qMax(parent->minimumWidth(), rectangle.width()));

	if(parent->minimumHeight() > rectangle.height())
	  rectangle.setY(-parent->minimumHeight() + rectangle.top());

	if(parent->minimumWidth() > rectangle.width())
	  rectangle.setX(-parent->minimumWidth() + rectangle.left());

	parent->setGeometry(parent->mapToScene(rectangle).boundingRect());
	break;
      }
    case RectangleLocations::CenterLeft:
      {
	auto const delta = -event->lastScenePos().x() + event->scenePos().x();

	rectangle = parent->mapToScene(rectangle).boundingRect();
	rectangle.setX(qMax(0.0, delta + rectangle.left()));

        if(parent->minimumWidth() > rectangle.width())
	  rectangle.setX(-parent->minimumWidth() + rectangle.right());

	parent->setGeometry(rectangle);
	break;
      }
    case RectangleLocations::CenterRight:
      {
	auto const delta = -event->lastPos().x() + event->pos().x();

	rectangle.setWidth(delta + rectangle.width());
	rectangle.setWidth(qMax(parent->minimumWidth(), rectangle.width()));

	if(parent->minimumWidth() > rectangle.width())
	  /*
	  ** Do not move the widget.
	  */

	  return;

	parent->setGeometry(parent->mapToScene(rectangle).boundingRect());
	break;
      }
    case RectangleLocations::TopCenter:
      {
	auto const delta = -event->lastScenePos().y() + event->scenePos().y();

	rectangle = parent->mapToScene(rectangle).boundingRect();
	rectangle.setY(qMax(0.0, delta + rectangle.top()));

	if(parent->minimumHeight() > rectangle.height())
	  rectangle.setY(-parent->minimumHeight() + rectangle.bottom());

	parent->setGeometry(rectangle);
	break;
      }
    case RectangleLocations::TopLeft:
      {
	rectangle = parent->mapToScene(rectangle).boundingRect();
	rectangle.setX(qMax(0.0, diagonal + event->scenePos().x()));
	rectangle.setY(qMax(0.0, diagonal + event->scenePos().y()));

	if(parent->minimumHeight() > rectangle.height())
	  rectangle.setY(-parent->minimumHeight() + rectangle.bottom());

        if(parent->minimumWidth() > rectangle.width())
	  rectangle.setX(-parent->minimumWidth() + rectangle.right());

	parent->setGeometry(rectangle);
	break;
      }
    case RectangleLocations::TopRight:
      {
	rectangle = parent->mapToScene(rectangle).boundingRect();
	rectangle.setWidth(-diagonal + event->pos().x());
	rectangle.setWidth(qMax(parent->minimumWidth(), rectangle.width()));

	if(!m_parentLocked)
	  {
	    rectangle.setY(qMax(0.0, diagonal + event->scenePos().y()));

	    if(parent->minimumHeight() > rectangle.height())
	      rectangle.setY(-parent->minimumHeight() + rectangle.bottom());
	  }

	if(parent->minimumWidth() > rectangle.width())
	  rectangle.setX(-parent->minimumWidth() + rectangle.left());

	parent->setGeometry(rectangle);
	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_resize_widget_rectangle::mousePressEvent
(QGraphicsSceneMouseEvent *event)
{
  Q_UNUSED(event);

  auto parent = qgraphicsitem_cast<glitch_proxy_widget *> (parentItem());

  if(parent)
    m_lastRect = parent->geometry();
}

void glitch_resize_widget_rectangle::mouseReleaseEvent
(QGraphicsSceneMouseEvent *event)
{
  QGraphicsRectItem::mouseReleaseEvent(event);

  auto parent = qgraphicsitem_cast<glitch_proxy_widget *> (parentItem());

  if(parent)
    parent->geometryChanged(m_lastRect);

  m_lastRect = QRectF();
}

void glitch_resize_widget_rectangle::setParentLocked(const bool parentLocked)
{
  m_parentLocked = parentLocked;
}
