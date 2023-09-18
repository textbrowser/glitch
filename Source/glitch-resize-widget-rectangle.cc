/*
** Copyright (c) 2015 - 10^10^10, Alexis Megas.
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

#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QtMath>

#include "glitch-proxy-widget.h"
#include "glitch-resize-widget-rectangle.h"

qreal glitch_resize_widget_rectangle::MINIMUM_DISTANCE_FOR_RESIZE = 10.0;
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

  auto distance = qPow
    (qPow(event->scenePos().x() - m_lastPress.x(), 2) +
     qPow(event->scenePos().y() - m_lastPress.y(), 2), 0.5);

  if(MINIMUM_DISTANCE_FOR_RESIZE >= distance)
    {
      QGraphicsRectItem::mouseMoveEvent(event);
      return;
    }

  auto parent = qgraphicsitem_cast<glitch_proxy_widget *> (parentItem());
  const auto o = MINIMUM_DISTANCE_FOR_RESIZE;

  if(!parent)
    {
      QGraphicsRectItem::mouseMoveEvent(event);
      return;
    }

  QList<glitch_resize_widget_rectangle *> list;
  auto rectangle(parent->boundingRect());

  switch(m_location)
    {
    case RectangleLocations::BottomCenter:
      {
	rectangle.setHeight(event->pos().y());
	rectangle.setHeight(qMax(parent->minimumHeight(), rectangle.height()));
	rectangle.setHeight(o * qFloor(rectangle.height() / o));

	if(parent->minimumHeight() > rectangle.height())
	  /*
	  ** Do not move the widget.
	  */

	  return;

	foreach(auto item, parent->resizeRectangles())
	  {
	    if(!item)
	      continue;

	    switch(item->location())
	      {
	      case RectangleLocations::BottomCenter:
	      case RectangleLocations::BottomLeft:
	      case RectangleLocations::BottomRight:
	      case RectangleLocations::CenterLeft:
	      case RectangleLocations::CenterRight:
		{
		  list.append(item);
		  break;
		}
	      default:
		{
		  break;
		}
	      }
	  }

	parent->setGeometry(parent->mapToScene(rectangle).boundingRect());
	break;
      }
    case RectangleLocations::BottomLeft:
      {
	rectangle = parent->mapToScene(rectangle).boundingRect();
	rectangle.setHeight(event->pos().y());
	rectangle.setHeight(qMax(parent->minimumHeight(), rectangle.height()));
	rectangle.setHeight(o * qFloor(rectangle.height() / o));

	if(!m_parentLocked)
	  {
	    rectangle.setX(qMax(0.0, event->scenePos().x()));

	    if(parent->minimumWidth() > rectangle.width())
	      rectangle.setX(m_lastRect.x());

	    rectangle.setX(o * qCeil(rectangle.x() / o));
	  }

	if(parent->minimumHeight() > rectangle.height())
	  rectangle.setY(m_lastRect.y());

	rectangle.setY(o * qCeil(rectangle.y() / o));

	foreach(auto item, parent->resizeRectangles())
	  {
	    if(!item)
	      continue;

	    switch(item->location())
	      {
	      case RectangleLocations::BottomCenter:
	      case RectangleLocations::BottomLeft:
	      case RectangleLocations::BottomRight:
	      case RectangleLocations::CenterLeft:
	      case RectangleLocations::CenterRight:
	      case RectangleLocations::TopCenter:
	      case RectangleLocations::TopRight:
		{
		  list.append(item);
		  break;
		}
	      default:
		{
		  break;
		}
	      }
	  }

	parent->setGeometry(rectangle);
	break;
      }
    case RectangleLocations::BottomRight:
      {
	rectangle.setHeight(event->pos().y());
	rectangle.setHeight(qMax(parent->minimumHeight(), rectangle.height()));
	rectangle.setHeight(o * qFloor(rectangle.height() / o));
	rectangle.setWidth(event->pos().x());
	rectangle.setWidth(qMax(parent->minimumWidth(), rectangle.width()));
	rectangle.setWidth(o * qFloor(rectangle.width() / o));

	if(parent->minimumHeight() > rectangle.height())
	  rectangle.setY(m_lastRect.y());

	if(parent->minimumWidth() > rectangle.width())
	  rectangle.setX(m_lastRect.x());

	rectangle.setX(o * qCeil(rectangle.x() / o));
	rectangle.setY(o * qCeil(rectangle.y() / o));

	foreach(auto item, parent->resizeRectangles())
	  {
	    if(!item)
	      continue;

	    switch(item->location())
	      {
	      case RectangleLocations::BottomCenter:
	      case RectangleLocations::BottomLeft:
	      case RectangleLocations::BottomRight:
	      case RectangleLocations::CenterLeft:
	      case RectangleLocations::CenterRight:
	      case RectangleLocations::TopCenter:
	      case RectangleLocations::TopRight:
		{
		  list.append(item);
		  break;
		}
	      default:
		{
		  break;
		}
	      }
	  }

	parent->setGeometry(parent->mapToScene(rectangle).boundingRect());
	break;
      }
    case RectangleLocations::CenterLeft:
      {
	rectangle = parent->mapToScene(rectangle).boundingRect();
	rectangle.setX(qMax(0.0, event->scenePos().x()));

        if(parent->minimumWidth() > rectangle.width())
	  rectangle.setX(m_lastRect.x());

	rectangle.setX(o * qCeil(rectangle.x() / o));

	foreach(auto item, parent->resizeRectangles())
	  {
	    if(!item)
	      continue;

	    switch(item->location())
	      {
	      case RectangleLocations::BottomCenter:
	      case RectangleLocations::BottomRight:
	      case RectangleLocations::CenterRight:
	      case RectangleLocations::TopCenter:
	      case RectangleLocations::TopRight:
		{
		  list.append(item);
		  break;
		}
	      default:
		{
		  break;
		}
	      }
	  }

	parent->setGeometry(rectangle);
	break;
      }
    case RectangleLocations::CenterRight:
      {
	rectangle.setWidth(event->pos().x());
	rectangle.setWidth(qMax(parent->minimumWidth(), rectangle.width()));
	rectangle.setWidth(o * qFloor(rectangle.width() / o));

	if(parent->minimumWidth() > rectangle.width())
	  /*
	  ** Do not move the widget.
	  */

	  return;

	foreach(auto item, parent->resizeRectangles())
	  {
	    if(!item)
	      continue;

	    switch(item->location())
	      {
	      case RectangleLocations::BottomCenter:
	      case RectangleLocations::BottomRight:
	      case RectangleLocations::CenterRight:
	      case RectangleLocations::TopCenter:
	      case RectangleLocations::TopRight:
		{
		  list.append(item);
		  break;
		}
	      default:
		{
		  break;
		}
	      }
	  }

	parent->setGeometry(parent->mapToScene(rectangle).boundingRect());
	break;
      }
    case RectangleLocations::TopCenter:
      {
	rectangle = parent->mapToScene(rectangle).boundingRect();
	rectangle.setY(qMax(0.0, event->scenePos().y()));

	if(parent->minimumHeight() > rectangle.height())
	  rectangle.setY(m_lastRect.y());

	rectangle.setY(o * qCeil(rectangle.y() / o));

	foreach(auto item, parent->resizeRectangles())
	  {
	    if(!item)
	      continue;

	    switch(item->location())
	      {
	      case RectangleLocations::BottomCenter:
	      case RectangleLocations::BottomLeft:
	      case RectangleLocations::BottomRight:
	      case RectangleLocations::CenterLeft:
	      case RectangleLocations::CenterRight:
		{
		  list.append(item);
		  break;
		}
	      default:
		{
		  break;
		}
	      }
	  }

	parent->setGeometry(rectangle);
	break;
      }
    case RectangleLocations::TopLeft:
      {
	rectangle = parent->mapToScene(rectangle).boundingRect();
	rectangle.setX(qMax(0.0, event->scenePos().x()));
	rectangle.setY(qMax(0.0, event->scenePos().y()));

	if(parent->minimumHeight() > rectangle.height())
	  rectangle.setY(m_lastRect.y());

        if(parent->minimumWidth() > rectangle.width())
	  rectangle.setX(m_lastRect.x());

	rectangle.setX(o * qCeil(rectangle.x() / o));
	rectangle.setY(o * qCeil(rectangle.y() / o));

	foreach(auto item, parent->resizeRectangles())
	  {
	    if(!item)
	      continue;

	    switch(item->location())
	      {
	      case RectangleLocations::BottomCenter:
	      case RectangleLocations::BottomLeft:
	      case RectangleLocations::BottomRight:
	      case RectangleLocations::CenterLeft:
	      case RectangleLocations::CenterRight:
	      case RectangleLocations::TopCenter:
	      case RectangleLocations::TopRight:
		{
		  list.append(item);
		  break;
		}
	      default:
		{
		  break;
		}
	      }
	  }

	parent->setGeometry(rectangle);
	break;
      }
    case RectangleLocations::TopRight:
      {
	rectangle = parent->mapToScene(rectangle).boundingRect();
	rectangle.setWidth(event->pos().x());
	rectangle.setWidth(qMax(parent->minimumWidth(), rectangle.width()));
	rectangle.setWidth(o * qFloor(rectangle.width() / o));

	if(!m_parentLocked)
	  {
	    rectangle.setY(qMax(0.0, event->scenePos().y()));

	    if(parent->minimumHeight() > rectangle.height())
	      rectangle.setY(m_lastRect.y());

	    rectangle.setY(o * qCeil(rectangle.y() / o));
	  }

	if(parent->minimumWidth() > rectangle.width())
	  rectangle.setX(m_lastRect.x());

	rectangle.setX(o * qCeil(rectangle.x() / o));

	foreach(auto item, parent->resizeRectangles())
	  {
	    if(!item)
	      continue;

	    switch(item->location())
	      {
	      case RectangleLocations::BottomCenter:
	      case RectangleLocations::BottomLeft:
	      case RectangleLocations::BottomRight:
	      case RectangleLocations::CenterLeft:
	      case RectangleLocations::CenterRight:
	      case RectangleLocations::TopCenter:
	      case RectangleLocations::TopRight:
		{
		  list.append(item);
		  break;
		}
	      default:
		{
		  break;
		}
	      }
	  }

	parent->setGeometry(rectangle);
	break;
      }
    default:
      {
	break;
      }
    }

  m_lastPress = event->scenePos();

  for(int i = 0; i < list.size(); i++)
    switch(list.at(i)->location())
      {
      case RectangleLocations::BottomCenter:
	{
	  list.at(i)->setRect
	    (QRectF(list.at(i)->x() + rectangle.width() / 2 - SQUARE_SIZE / 2,
		    list.at(i)->y() + rectangle.height() + 1,
		    SQUARE_SIZE,
		    SQUARE_SIZE));
	  break;
	}
      case RectangleLocations::BottomLeft:
	{
	  list.at(i)->setRect
	    (QRectF(list.at(i)->x() - SQUARE_SIZE - 1,
		    list.at(i)->y() + rectangle.height() + 1,
		    SQUARE_SIZE,
		    SQUARE_SIZE));
	  break;
	}
      case RectangleLocations::BottomRight:
	{
	  list.at(i)->setRect
	    (QRectF(list.at(i)->x() + rectangle.width() + 1,
		    list.at(i)->y() + rectangle.height() + 1,
		    SQUARE_SIZE,
		    SQUARE_SIZE));
	  break;
	}
      case RectangleLocations::CenterLeft:
	{
	  list.at(i)->setRect
	    (QRectF(list.at(i)->x() - SQUARE_SIZE - 1,
		    list.at(i)->y() + rectangle.height() / 2 - SQUARE_SIZE / 2,
		    SQUARE_SIZE,
		    SQUARE_SIZE));
	  break;
	}
      case RectangleLocations::CenterRight:
	{
	  list.at(i)->setRect
	    (QRectF(list.at(i)->x() + rectangle.width() + 1,
		    list.at(i)->y() + rectangle.height() / 2 - SQUARE_SIZE / 2,
		    SQUARE_SIZE,
		    SQUARE_SIZE));
	  break;
	}
      case RectangleLocations::TopCenter:
	{
	  list.at(i)->setRect
	    (QRectF(list.at(i)->x() + rectangle.width() / 2 - SQUARE_SIZE / 2,
		    list.at(i)->y() - SQUARE_SIZE - 1,
		    SQUARE_SIZE,
		    SQUARE_SIZE));
	  break;
	}
      case RectangleLocations::TopLeft:
	{
	  list.at(i)->setRect
	    (QRectF(list.at(i)->x() - SQUARE_SIZE - 1,
		    list.at(i)->y() - SQUARE_SIZE - 1,
		    SQUARE_SIZE,
		    SQUARE_SIZE));
	  break;
	}
      case RectangleLocations::TopRight:
	{
	  list.at(i)->setRect
	    (QRectF(list.at(i)->x() + rectangle.width() + 1,
		    list.at(i)->y() - SQUARE_SIZE - 1,
		    SQUARE_SIZE,
		    SQUARE_SIZE));
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
  if(event)
    m_lastPress = event->scenePos();

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
