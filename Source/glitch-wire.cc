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

/*
** Wires are not assigned direct parents.
*/

#include <QPainter>

#include "glitch-object.h"
#include "glitch-proxy-widget.h"
#include "glitch-wire.h"

static int s_alpha = 175;
static qreal s_penWidth = 2.5;

glitch_wire::glitch_wire(QGraphicsItem *parent):QGraphicsObject(parent)
{
  m_color = QColor(255, 192, 203, s_alpha);
  m_wireType = WireTypes::CURVE;
  m_wireWidth = s_penWidth;
  setCacheMode(QGraphicsItem::NoCache);
  setFlag(QGraphicsItem::ItemIsSelectable, false);
  setZValue(glitch_common::s_minimumZValue - static_cast<qreal> (1));
}

glitch_wire::~glitch_wire()
{
}

QPainterPath glitch_wire::shape(void) const
{
  QPainterPath path;

  path.addRect(m_boundingRect);
  return path;
}

QPointer<glitch_proxy_widget> glitch_wire::leftProxy(void) const
{
  return m_leftProxy;
}

QPointer<glitch_proxy_widget> glitch_wire::rightProxy(void) const
{
  return m_rightProxy;
}

QRectF glitch_wire::boundingRect(void) const
{
  return m_boundingRect;
}

void glitch_wire::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsObject::mousePressEvent(event);
  emit disconnectWireIfNecessary();
}

void glitch_wire::paint
(QPainter *painter, const QStyleOptionGraphicsItem *opt, QWidget *widget)
{
  Q_UNUSED(opt);
  Q_UNUSED(widget);

  if(!m_leftProxy ||
     !m_leftProxy->scene() ||
     !m_rightProxy ||
     !m_rightProxy->scene())
    return;

  if(painter)
    {
      if(painter->isActive())
	painter->setRenderHints(QPainter::Antialiasing |
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
				QPainter::LosslessImageRendering |
#endif
				QPainter::SmoothPixmapTransform |
				QPainter::TextAntialiasing,
				true);

      if(m_wireType == WireTypes::CURVE)
	{
	  QPainterPath path;
	  QPen pen;
	  QPointF c1;
	  QPointF c2;
	  QPointF endPoint;
	  auto const x1 = m_leftProxy->pos().x() + m_leftProxy->size().width();
	  auto const y1 = m_leftProxy->pos().y() +
	    m_leftProxy->size().height() / 2.0;
	  auto const x2 = m_rightProxy->pos().x();
	  auto const y2 = m_rightProxy->pos().y() +
	    m_rightProxy->size().height() / 2.0;

	  c1.setX(qAbs(x1 - x2) / 2.0 + x1);
	  c1.setY(y1);
	  c2.setX(c1.x());
	  c2.setY(y2);
	  endPoint = QPointF(x2, y2);
	  path.moveTo(QPointF(x1, y1));
	  pen.setColor(m_color);
	  pen.setJoinStyle(Qt::MiterJoin);
	  pen.setWidthF(m_wireWidth);

	  if(painter->isActive())
	    painter->setPen(pen);

	  path.cubicTo(c1, c2, endPoint);
	  m_boundingRect = path.boundingRect();

	  if(painter->isActive())
	    painter->drawPath(path);

	  return;
	}

      if(m_leftProxy->size().height() / 2.0 + m_leftProxy->y() <=
	 m_rightProxy->size().height() / 2.0 + m_rightProxy->y() &&
	 m_leftProxy->size().width() + m_leftProxy->x() + 15.0 <=
	 m_rightProxy->x())
	{
	  QPen pen;
	  QPolygonF points;

	  pen.setColor(m_color);
	  pen.setJoinStyle(Qt::MiterJoin);
	  pen.setWidthF(m_wireWidth);

	  if(painter->isActive())
	    painter->setPen(pen);

	  auto const x1 = m_leftProxy->pos().x() + m_leftProxy->size().width();
	  auto const x2 = m_rightProxy->pos().x();
	  auto const xd = qAbs(x1 - x2) / 2.0;
	  auto const yd = qAbs
	    (m_leftProxy->size().height() / 2.0 + m_leftProxy->y() -
	     m_rightProxy->size().height() / 2.0 - m_rightProxy->y());

	  points << QPointF(x1 + 6.0,
			    m_leftProxy->pos().y() +
			    m_leftProxy->size().height() / 2.0 - 1.0)
		 << QPointF(x1 + xd,
			    m_leftProxy->pos().y() +
			    m_leftProxy->size().height() / 2.0 - 1.0)
		 << QPointF(x1 + xd,
			    m_leftProxy->pos().y() +
			    m_leftProxy->size().height() / 2.0 - 1.0 + yd)
		 << QPointF(x1 + 2.0 * xd - 6.0,
			    m_leftProxy->pos().y() +
			    m_leftProxy->size().height() / 2.0 - 1.0 + yd);
	  m_boundingRect = points.boundingRect();

	  if(painter->isActive())
	    painter->drawPolyline(points);
	}
      else if(m_leftProxy->size().height() / 2.0 + m_leftProxy->y() >=
	      m_rightProxy->size().height() / 2.0 + m_rightProxy->y() &&
	      m_leftProxy->size().width() + m_leftProxy->x() + 15.0 <=
	      m_rightProxy->x())
	{
	  QPen pen;
	  QPolygonF points;

	  pen.setColor(m_color);
	  pen.setJoinStyle(Qt::MiterJoin);
	  pen.setWidthF(m_wireWidth);

	  if(painter->isActive())
	    painter->setPen(pen);

	  auto const x1 = m_leftProxy->pos().x() + m_leftProxy->size().width();
	  auto const x2 = m_rightProxy->pos().x();
	  auto const xd = qAbs(x1 - x2) / 2.0;
	  auto const yd = qAbs
	    (m_leftProxy->size().height() / 2.0 + m_leftProxy->y() -
	     m_rightProxy->size().height() / 2.0 - m_rightProxy->y());

	  points << QPointF(x1 + 6.0,
			    m_leftProxy->pos().y() +
			    m_leftProxy->size().height() / 2.0 - 1.0)
		 << QPointF(x1 + xd,
			    m_leftProxy->pos().y() +
			    m_leftProxy->size().height() / 2.0 - 1.0)
		 << QPointF(x1 + xd,
			    m_leftProxy->pos().y() +
			    m_leftProxy->size().height() / 2.0 - 1.0 - yd)
		 << QPointF(x1 + 2.0 * xd - 6.0,
			    m_leftProxy->pos().y() +
			    m_leftProxy->size().height() / 2.0 - 1.0 - yd);
	  m_boundingRect = points.boundingRect();

	  if(painter->isActive())
	    painter->drawPolyline(points);
	}
      else if(m_leftProxy->size().width() + m_leftProxy->x() + 15.0 >=
	      m_rightProxy->x() &&
	      m_leftProxy->y() <= m_rightProxy->y())
	{
	  QPen pen;
	  QPolygonF points;

	  pen.setColor(m_color);
	  pen.setJoinStyle(Qt::MiterJoin);
	  pen.setWidthF(m_wireWidth);

	  if(painter->isActive())
	    painter->setPen(pen);

	  auto const x1 = m_leftProxy->pos().x() + m_leftProxy->size().width();
	  auto const xd = m_rightProxy->pos().x() - x1;
	  auto const yd = qAbs
	    (m_leftProxy->size().height() / 2.0 + m_leftProxy->y() -
	     m_rightProxy->size().height() / 2.0 - m_rightProxy->y()) / 2.0;

	  points << QPointF(x1 + 6.0,
			    m_leftProxy->pos().y() +
			    m_leftProxy->size().height() / 2.0 - 1.0)
		 << QPointF(x1 + 26.0,
			    m_leftProxy->pos().y() +
			    m_leftProxy->size().height() / 2.0 - 1.0)
		 << QPointF(x1 + 26.0,
			    m_leftProxy->pos().y() +
			    m_leftProxy->size().height() / 2.0 - 1.0 + yd)
		 << QPointF(x1 + xd - 26.0,
			    m_leftProxy->pos().y() +
			    m_leftProxy->size().height() / 2.0 - 1.0 + yd)
		 << QPointF(x1 + xd - 26.0,
			    m_rightProxy->pos().y() +
			    m_rightProxy->size().height() / 2.0 - 1.0)
		 << QPointF(x1 + xd - 6.0,
			    m_rightProxy->pos().y() +
			    m_rightProxy->size().height() / 2.0 - 1.0);
	  m_boundingRect = points.boundingRect();

	  if(painter->isActive())
	    painter->drawPolyline(points);
	}
      else
	{
	  QPen pen;
	  QPolygonF points;

	  pen.setColor(m_color);
	  pen.setJoinStyle(Qt::MiterJoin);
	  pen.setWidthF(m_wireWidth);

	  if(painter->isActive())
	    painter->setPen(pen);

	  auto const x1 = m_leftProxy->pos().x() + m_leftProxy->size().width();
	  auto const xd = m_rightProxy->pos().x() - x1;
	  auto const yd = qAbs
	    (m_leftProxy->size().height() / 2.0 + m_leftProxy->y() -
	     m_rightProxy->size().height() / 2.0 - m_rightProxy->y()) / 2.0;

	  points << QPointF(x1 + 6.0,
			    m_leftProxy->pos().y() +
			    m_leftProxy->size().height() / 2.0 - 1.0)
		 << QPointF(x1 + 26.0,
			    m_leftProxy->pos().y() +
			    m_leftProxy->size().height() / 2.0 - 1.0)
		 << QPointF(x1 + 26.0,
			    m_leftProxy->pos().y() +
			    m_leftProxy->size().height() / 2.0 - 1.0 - yd)
		 << QPointF(x1 + xd - 26.0,
			    m_leftProxy->pos().y() +
			    m_leftProxy->size().height() / 2.0 - 1.0 - yd)
		 << QPointF(x1 + xd - 26.0,
			    m_rightProxy->pos().y() +
			    m_rightProxy->size().height() / 2.0 - 1.0)
		 << QPointF(x1 + xd - 6.0,
			    m_rightProxy->pos().y() +
			    m_rightProxy->size().height() / 2.0 - 1.0);
	  m_boundingRect = points.boundingRect();

	  if(painter->isActive())
	    painter->drawPolyline(points);
	}
    }
}

void glitch_wire::setBoundingRect(const QRectF &rect)
{
  prepareGeometryChange();
  m_boundingRect = rect;
}

void glitch_wire::setColor(const QColor &color)
{
  if(color.isValid())
    m_color = color;
}

void glitch_wire::setLeftProxy(glitch_proxy_widget *proxy)
{
  if(!proxy || m_leftProxy || m_rightProxy == proxy)
    return;

  auto object = qobject_cast<glitch_object *> (proxy->widget());

  if(!object)
    return;

  m_leftProxy = proxy;
}

void glitch_wire::setRightProxy(glitch_proxy_widget *proxy)
{
  if(!proxy || m_leftProxy == proxy || m_rightProxy)
    return;

  auto object = qobject_cast<glitch_object *> (proxy->widget());

  if(!object)
    return;

  m_rightProxy = proxy;
}

void glitch_wire::setWireType(const QString &wireType)
{
  if(wireType.trimmed() == tr("Line"))
    m_wireType = WireTypes::LINE;
  else
    m_wireType = WireTypes::CURVE;
}

void glitch_wire::setWireType(const WireTypes wireType)
{
  m_wireType = wireType;
}

void glitch_wire::setWireWidth(const double value)
{
  m_wireWidth = qBound(2.5, value, 5.0);
}

void glitch_wire::slotUpdate(const QList<QRectF> &region)
{
  Q_UNUSED(region);

  QPainter painter;

  paint(&painter, nullptr, nullptr);
}
