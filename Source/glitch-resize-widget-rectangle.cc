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

#include <QApplication>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>

#include "glitch-proxy-widget.h"
#include "glitch-resize-widget-rectangle.h"

qreal glitch_resize_widget_rectangle::SQUARE_SIZE = 10.0;

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
    case BottomCenter:
    case TopCenter:
      {
	setCursor(Qt::SizeVerCursor);
	break;
      }
    case BottomLeft:
    case TopRight:
      {
	if(m_parentLocked)
	  {
	    if(m_location == BottomLeft)
	      setCursor(Qt::SizeVerCursor);
	    else
	      setCursor(Qt::SizeHorCursor);
	  }
	else
	  setCursor(Qt::SizeBDiagCursor);

	break;
      }
    case BottomRight:
    case TopLeft:
      {
	setCursor(Qt::SizeFDiagCursor);
	break;
      }
    case CenterLeft:
    case CenterRight:
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

  QList<glitch_resize_widget_rectangle *> list;
  QRectF rectangle(parent->boundingRect());

  switch(m_location)
    {
    case BottomCenter:
      {
	if(event->pos().y() > 0)
	  rectangle.setHeight(event->pos().y());

	if(parent->minimumHeight() >= rectangle.height())
	  /*
	  ** Do not move the widget.
	  */

	  return;

	foreach(auto item, parent->childItems())
	  {
	    if(!item)
	      continue;

	    auto i = qgraphicsitem_cast<glitch_resize_widget_rectangle *>
	      (item);

	    if(i)
	      {
		if(i->location() == BottomCenter)
		  list.append(i);
		else if(i->location() == BottomLeft)
		  list.append(i);
		else if(i->location() == BottomRight)
		  list.append(i);
		else if(i->location() == CenterLeft)
		  list.append(i);
		else if(i->location() == CenterRight)
		  list.append(i);
	      }
	  }

	break;
      }
    case BottomLeft:
      {
	if(event->pos().y() > 0)
	  rectangle.setHeight(event->pos().y());

	if(parent->minimumHeight() >= rectangle.height())
	  /*
	  ** Do not move the widget.
	  */

	  return;

	if(!m_parentLocked)
	  rectangle.setX(event->pos().x());

	if(parent->minimumWidth() >= rectangle.width())
	  /*
	  ** Do not move the widget.
	  */

	  return;

	foreach(auto item, parent->childItems())
	  {
	    if(!item)
	      continue;

	    auto i = qgraphicsitem_cast<glitch_resize_widget_rectangle *>
	      (item);

	    if(i)
	      {
		if(i->location() == BottomCenter)
		  list.append(i);
	 	else if(i->location() == BottomLeft)
		  list.append(i);
		else if(i->location() == BottomRight)
		  list.append(i);
		else if(i->location() == CenterLeft)
		  list.append(i);
		else if(i->location() == CenterRight)
		  list.append(i);
		else if(i->location() == TopCenter)
		  list.append(i);
		else if(i->location() == TopRight)
		  list.append(i);
	      }
	  }

	break;
      }
    case BottomRight:
      {
	if(event->pos().y() > 0)
	  rectangle.setHeight(event->pos().y());

	if(parent->minimumHeight() >= rectangle.height())
	  /*
	  ** Do not move the widget.
	  */

	  return;

	foreach(auto item, parent->childItems())
	  {
	    if(!item)
	      continue;

	    auto i = qgraphicsitem_cast<glitch_resize_widget_rectangle *>
	      (item);

	    if(i)
	      {
		if(i->location() == BottomCenter)
		  list.append(i);
	 	else if(i->location() == BottomLeft)
		  list.append(i);
		else if(i->location() == BottomRight)
		  list.append(i);
		else if(i->location() == CenterLeft)
		  list.append(i);
		else if(i->location() == CenterRight)
		  list.append(i);
		else if(i->location() == TopCenter)
		  list.append(i);
		else if(i->location() == TopRight)
		  list.append(i);
	      }
	  }

	auto d = -event->lastPos().x() + event->pos().x();

	if(d + rectangle.width() > 0)
	  rectangle.setWidth(d + rectangle.width());

	break;
      }
    case CenterLeft:
      {
	rectangle.setX(event->pos().x());

        if(parent->minimumWidth() >= rectangle.width())
	  /*
	  ** Do not move the widget.
	  */

          return;

	foreach(auto item, parent->childItems())
	  {
	    if(!item)
	      continue;

	    auto i = qgraphicsitem_cast<glitch_resize_widget_rectangle *>
	      (item);

	    if(i)
	      {
		if(i->location() == BottomCenter)
		  list.append(i);
		else if(i->location() == BottomRight)
		  list.append(i);
		else if(i->location() == CenterRight)
		  list.append(i);
		else if(i->location() == TopCenter)
		  list.append(i);
		else if(i->location() == TopRight)
		  list.append(i);
	      }
	  }

	break;
      }
    case CenterRight:
      {
	foreach(auto item, parent->childItems())
	  {
	    if(!item)
	      continue;

	    auto i = qgraphicsitem_cast<glitch_resize_widget_rectangle *>
	      (item);

	    if(i)
	      {
		if(i->location() == BottomCenter)
		  list.append(i);
		else if(i->location() == BottomRight)
		  list.append(i);
		else if(i->location() == CenterRight)
		  list.append(i);
		else if(i->location() == TopCenter)
		  list.append(i);
		else if(i->location() == TopRight)
		  list.append(i);
	      }
	  }

	auto d = -event->lastPos().x() + event->pos().x();

	if(d + rectangle.width() > 0)
	  rectangle.setWidth(d + rectangle.width());

	break;
      }
    case TopCenter:
      {
	rectangle.setY(event->pos().y());

	if(parent->minimumHeight() >= rectangle.height())
	  /*
	  ** Do not move the widget.
	  */

	  return;

	foreach(auto item, parent->childItems())
	  {
	    if(!item)
	      continue;

	    auto i = qgraphicsitem_cast<glitch_resize_widget_rectangle *>
	      (item);

	    if(i)
	      {
		if(i->location() == BottomCenter)
		  list.append(i);
		else if(i->location() == BottomLeft)
		  list.append(i);
		else if(i->location() == BottomRight)
		  list.append(i);
		else if(i->location() == CenterLeft)
		  list.append(i);
		else if(i->location() == CenterRight)
		  list.append(i);
	      }
	  }

	break;
      }
    case TopLeft:
      {
	rectangle.setX(event->pos().x());

	if(parent->minimumWidth() >= rectangle.width())
	  /*
	  ** Do not move the widget.
	  */

	  return;

	rectangle.setY(event->pos().y());

	if(parent->minimumHeight() >= rectangle.height())
	  /*
	  ** Do not move the widget.
	  */

	  return;

	foreach(auto item, parent->childItems())
	  {
	    if(!item)
	      continue;

	    auto i = qgraphicsitem_cast<glitch_resize_widget_rectangle *>
	      (item);

	    if(i)
	      {
		if(i->location() == BottomCenter)
		  list.append(i);
		else if(i->location() == BottomLeft)
		  list.append(i);
		else if(i->location() == BottomRight)
		  list.append(i);
		else if(i->location() == CenterLeft)
		  list.append(i);
		else if(i->location() == CenterRight)
		  list.append(i);
		else if(i->location() == TopCenter)
		  list.append(i);
		else if(i->location() == TopRight)
		  list.append(i);
	      }
	  }

	break;
      }
    case TopRight:
      {
	if(!m_parentLocked)
	  rectangle.setY(event->pos().y());

	if(parent->minimumHeight() >= rectangle.height())
	  /*
	  ** Do not move the widget.
	  */

	  return;

	foreach(auto item, parent->childItems())
	  {
	    if(!item)
	      continue;

	    auto i = qgraphicsitem_cast<glitch_resize_widget_rectangle *>
	      (item);

	    if(i)
	      {
		if(i->location() == BottomCenter)
		  list.append(i);
		else if(i->location() == BottomLeft)
		  list.append(i);
		else if(i->location() == BottomRight)
		  list.append(i);
		else if(i->location() == CenterLeft)
		  list.append(i);
		else if(i->location() == CenterRight)
		  list.append(i);
		else if(i->location() == TopCenter)
		  list.append(i);
		else if(i->location() == TopRight)
		  list.append(i);
	      }
	  }

	auto d = -event->lastPos().x() + event->pos().x();

	if(d + rectangle.width() > 0)
	  rectangle.setWidth(d + rectangle.width());

	break;
      }
    default:
      {
	break;
      }
    }

  parent->setGeometry(parent->mapToScene(rectangle).boundingRect());

  for(int i = 0; i < list.size(); i++)
    if(list.at(i)->location() == BottomCenter)
      list.at(i)->setRect
	(QRectF(list.at(i)->x() + rectangle.width() / 2 - SQUARE_SIZE / 2,
		list.at(i)->y() + rectangle.height() + 1,
                SQUARE_SIZE,
		SQUARE_SIZE));
    else if(list.at(i)->location() == BottomLeft)
      list.at(i)->setRect
	(QRectF(list.at(i)->x() - SQUARE_SIZE - 1,
		list.at(i)->y() + rectangle.height() + 1,
                SQUARE_SIZE,
		SQUARE_SIZE));
    else if(list.at(i)->location() == BottomRight)
      list.at(i)->setRect
	(QRectF(list.at(i)->x() + rectangle.width() + 1,
		list.at(i)->y() + rectangle.height() + 1,
                SQUARE_SIZE,
		SQUARE_SIZE));
    else if(list.at(i)->location() == CenterLeft)
      list.at(i)->setRect
	(QRectF(list.at(i)->x() - SQUARE_SIZE - 1,
		list.at(i)->y() + rectangle.height() / 2 - SQUARE_SIZE / 2,
		SQUARE_SIZE,
		SQUARE_SIZE));
    else if(list.at(i)->location() == CenterRight)
      list.at(i)->setRect
	(QRectF(list.at(i)->x() + rectangle.width() + 1,
		list.at(i)->y() + rectangle.height() / 2 - SQUARE_SIZE / 2,
		SQUARE_SIZE,
		SQUARE_SIZE));
    else if(list.at(i)->location() == TopCenter)
      list.at(i)->setRect
	(QRectF(list.at(i)->x() + rectangle.width() / 2 - SQUARE_SIZE / 2,
		list.at(i)->y() - SQUARE_SIZE - 1,
                SQUARE_SIZE,
                SQUARE_SIZE));
    else if(list.at(i)->location() == TopLeft)
      list.at(i)->setRect
	(QRectF(list.at(i)->x() - SQUARE_SIZE - 1,
		list.at(i)->y() - SQUARE_SIZE - 1,
                SQUARE_SIZE,
                SQUARE_SIZE));
    else if(list.at(i)->location() == TopRight)
      list.at(i)->setRect
	(QRectF(list.at(i)->x() + rectangle.width() + 1,
		list.at(i)->y() - SQUARE_SIZE - 1,
                SQUARE_SIZE,
                SQUARE_SIZE));
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
