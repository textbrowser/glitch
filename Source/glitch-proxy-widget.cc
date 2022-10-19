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
#include <QLineEdit>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QToolButton>
#include <QtDebug>
#include <QtMath>

#include "glitch-floating-context-menu.h"
#include "glitch-object.h"
#include "glitch-proxy-widget.h"
#include "glitch-scene.h"
#include "glitch-tools.h"

static qreal s_intelligentDistance = 15.0;

glitch_proxy_widget::glitch_proxy_widget
(QGraphicsItem *parent, Qt::WindowFlags wFlags):
  QGraphicsProxyWidget(parent, wFlags)
{
  m_hoveredSection = Sections::XYZ;
}

glitch_proxy_widget::~glitch_proxy_widget()
{
}

QPointer<glitch_object> glitch_proxy_widget::object(void) const
{
  return m_object;
}

bool glitch_proxy_widget::isMandatory(void) const
{
  if(m_object)
    return m_object->isMandatory();
  else
    return false;
}

bool glitch_proxy_widget::isMovable(void) const
{
  return QGraphicsItem::ItemIsMovable & flags();
}

bool glitch_proxy_widget::isWired(void) const
{
  if(m_object)
    return m_object->isWired();
  else
    return true;
}

glitch_proxy_widget::Sections glitch_proxy_widget::hoveredSection(void) const
{
  return m_hoveredSection;
}

void glitch_proxy_widget::contextMenuEvent
(QGraphicsSceneContextMenuEvent *event)
{
  if(event)
    {
      if(m_object)
	{
	  QMenu menu;

	  m_object->addActions(menu);
	  menu.exec(event->screenPos());
	}
      else
	QGraphicsProxyWidget::contextMenuEvent(event);
    }
  else
    QGraphicsProxyWidget::contextMenuEvent(event);
}

void glitch_proxy_widget::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
  QGraphicsProxyWidget::hoverEnterEvent(event);
  prepareHoverSection(event);
  update();
}

void glitch_proxy_widget::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
  QGraphicsProxyWidget::hoverLeaveEvent(event);
  m_hoveredSection = Sections::XYZ;
  update();
}

void glitch_proxy_widget::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
  QGraphicsProxyWidget::hoverMoveEvent(event);
  prepareHoverSection(event);
  update();
}

void glitch_proxy_widget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  if(event && (Qt::RightButton & event->buttons()))
    /*
    ** Activate a context menu.
    */

    return;

  if(!m_object)
    {
      QGraphicsProxyWidget::mousePressEvent(event);
      return;
    }

  auto comboBox = qobject_cast<QComboBox *>
    (m_object->childAt(event->pos().toPoint()));

  if(comboBox)
    {
      QGraphicsProxyWidget::mousePressEvent(event);
      return;
    }

  auto lineEdit = qobject_cast<QLineEdit *>
    (m_object->childAt(event->pos().toPoint()));

  if(lineEdit)
    {
      QGraphicsProxyWidget::mousePressEvent(event);
      return;
    }

  auto toolButton = qobject_cast<QToolButton *>
    (m_object->childAt(event->pos().toPoint()));

  if(toolButton)
    {
      QGraphicsProxyWidget::mousePressEvent(event);
      return;
    }

  auto widget = m_object->childAt(event->pos().toPoint());

  if(widget && widget->objectName().toLower().contains("viewport"))
    {
      QGraphicsProxyWidget::mousePressEvent(event);
      return;
    }

  m_object->setFocus();
}

void glitch_proxy_widget::paint
(QPainter *painter, const QStyleOptionGraphicsItem *opt, QWidget *widget)
{
  QGraphicsProxyWidget::paint(painter, opt, widget);

  if(painter)
    {
      painter->setRenderHints(QPainter::Antialiasing |
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
			      QPainter::HighQualityAntialiasing | // OpenGL?
#endif
			      QPainter::SmoothPixmapTransform |
			      QPainter::TextAntialiasing,
			      true);

      QPen pen;

      pen.setColor(QColor(168, 169, 173));
      pen.setJoinStyle(Qt::RoundJoin);
      pen.setStyle(Qt::SolidLine);
      pen.setWidthF(1.0);
      painter->save();
      painter->setPen(pen);
      painter->drawRoundedRect(boundingRect(), 5.0, 5.0);
      painter->restore();

      if(opt && (opt->state & QStyle::State_Selected))
	{
	  /*
	  ** Draw a selection rectangle.
	  */

	  QPen pen;
	  const qreal offset = 10.0;

	  if(isMovable())
	    pen.setColor(QColor("lightgreen"));
	  else
	    pen.setColor(QColor("orange"));

	  pen.setJoinStyle(Qt::RoundJoin);
	  pen.setStyle(Qt::SolidLine);
	  pen.setWidthF(5.0);
	  painter->save();
	  painter->setPen(pen);
	  painter->drawRect
	    (boundingRect().adjusted(-offset, -offset, offset, offset));
	  painter->restore();
	}

      if(!isMandatory() && m_object && m_object->showOrderIndicator())
	{
	  /*
	  ** Draw the object's order.
	  */

	  QFontMetrics fontMetrics(painter->font());
	  QPainterPath path;
	  QPen pen;
	  auto font(painter->font());
	  auto order(QString::number(objectOrder()));
	  auto point(this->rect().topRight() / 2.0);

	  path.addRect
	    (point.x() - 15.0,
	     point.y() - 25.0,
	     30.0 +
	     static_cast<qreal> (fontMetrics.boundingRect(order).width()),
	     30.0);
	  pen.setColor(Qt::blue);
	  pen.setWidthF(1.0);
	  painter->save();
	  painter->setPen(pen);
	  painter->drawPath(path);
	  painter->fillPath(path, QColor(173, 216, 230, 150));
	  painter->restore();
	  painter->save();
	  font.setBold(true);
	  font.setPointSizeF(11.5);
	  pen.setColor(Qt::white);
	  painter->setFont(font);
	  painter->setPen(pen);
	  painter->drawText(path.boundingRect(), Qt::AlignCenter, order);
	  painter->restore();
	}

      if(m_object)
	{
	  if(m_object->hasInput())
	    {
	      /*
	      ** Draw an input port.
	      */

	      QPainterPath path;
	      auto rect(this->rect());

	      path.addEllipse(rect.topLeft().x() - 5.5,
			      rect.height() / 2.0 + rect.topLeft().y() - 5.0,
			      10.0,
			      10.0);

	      if(m_object->isInputWired())
		painter->fillPath(path, QColor(0, 80, 181));
	      else
		painter->fillPath(path, QColor(118, 134, 146));
	    }

	  if(m_object->hasOutput())
	    {
	      /*
	      ** Draw an output port.
	      */

	      QPainterPath path;
	      auto rect(this->rect());

	      path.addEllipse(rect.topRight().x() - 5.5,
			      rect.height() / 2.0 + rect.topRight().y() - 5.0,
			      10.0,
			      10.0);

	      if(m_object->isOutputWired())
		painter->fillPath(path, QColor(0, 80, 181));
	      else
		painter->fillPath(path, QColor(118, 134, 146));
	    }

	  if((m_hoveredSection == Sections::LEFT && m_object->hasInput()) ||
	     (m_hoveredSection == Sections::RIGHT && m_object->hasOutput()))
	    {
	      QPainterPath path;
	      auto operation = m_scene ?
		m_scene->toolsOperation() : glitch_tools::Operations::XYZ;
	      auto rect(this->rect());

	      if(m_hoveredSection == Sections::LEFT && m_object->hasInput())
		path.addEllipse(rect.topLeft().x() - size().height() / 2.0,
				rect.topLeft().y(),
				size().height(),
				size().height());
	      else
		path.addEllipse(rect.topRight().x() - size().height() / 2.0,
				rect.topRight().y(),
				size().height(),
				size().height());

	      if(operation == glitch_tools::Operations::INTELLIGENT ||
		 operation == glitch_tools::Operations::WIRE_DISCONNECT)
		{
		  if(operation == glitch_tools::Operations::INTELLIGENT)
		    {
		      auto instance = qobject_cast<QGuiApplication *>
			(QApplication::instance());

		      if(instance &&
			 instance->keyboardModifiers() & Qt::ShiftModifier)
			{
			  QIcon icon(":clear.png");

			  icon.paint(painter, path.boundingRect().toRect());
			  return;
			}
		    }
		  else
		    {
		      QIcon icon(":clear.png");

		      icon.paint(painter, path.boundingRect().toRect());
		      return;
		    }
		}

	      if(m_hoveredSection == Sections::LEFT &&
		 m_object->hasInput() &&
		 m_object->isFullyWired())
		return;

	      /*
	      ** Draw input or output selection indicators.
	      */

	      painter->fillPath(path, QColor(255, 192, 203, 225));

	      QPen pen;

	      pen.setColor(QColor(199, 21, 133));
	      pen.setWidthF(1.5);
	      painter->save();
	      painter->setPen(pen);

	      if(m_hoveredSection == Sections::LEFT && m_object->hasInput())
		painter->drawEllipse
		  (rect.topLeft().x() - size().height() / 2.0,
		   rect.topLeft().y(),
		   size().height(),
		   size().height());
	      else
		painter->drawEllipse
		  (rect.topRight().x() - size().height() / 2.0,
		   rect.topRight().y(),
		   size().height(),
		   size().height());

	      painter->restore();

	      if(operation == glitch_tools::Operations::INTELLIGENT ||
		 operation == glitch_tools::Operations::WIRE_CONNECT)
		{
		  /*
		  ** Draw wiring order text.
		  */

		  auto font(painter->font());

		  font.setBold(true);
		  font.setPointSizeF(25.0);
		  pen.setColor(Qt::white);
		  painter->setFont(font);
		  painter->setPen(pen);
		  painter->drawText
		    (path.boundingRect(),
		     Qt::AlignCenter,
		     QString::number(m_scene->selectedForWiringCount() + 1));
		}
	    }
	}
    }
}

void glitch_proxy_widget::prepareHoverSection(QGraphicsSceneHoverEvent *event)
{
  if(!m_scene)
    m_scene = qobject_cast<glitch_scene *> (scene());

  if(event &&
     m_scene &&
     m_scene->toolsOperation() != glitch_tools::Operations::SELECT)
    {
      if(m_scene->toolsOperation() == glitch_tools::Operations::INTELLIGENT)
	{
	  /*
	  ** Determine if we're close to an input or an output port.
	  */

	  auto distance = qSqrt
	    (qPow(event->scenePos().x() - pos().x(),
		  2.0) +
	     qPow(event->scenePos().y() - pos().y() - size().height() / 2.0,
		  2.0));

	  if(distance <= s_intelligentDistance)
	    {
	      if(m_object && m_object->hasInput())
		m_hoveredSection = Sections::LEFT;
	      else
		goto done_label;

	      return;
	    }

	  distance = qSqrt
	    (qPow(event->scenePos().x() - pos().x() - size().width(),
		  2.0) +
	     qPow(event->scenePos().y() - pos().y() - size().height() / 2.0,
		  2.0));

	  if(distance <= s_intelligentDistance)
	    {
	      if(m_object && m_object->hasOutput())
		m_hoveredSection = Sections::RIGHT;
	      else
		goto done_label;

	      return;
	    }

	done_label:
	  m_hoveredSection = Sections::XYZ;
	}
      else
	{
	  auto distance1 = qSqrt(qPow(event->scenePos().x() -
				      pos().x(),
				      2.0) +
				 qPow(event->scenePos().y() -
				      pos().y(),
				      2.0));
	  auto distance2 = qSqrt(qPow(event->scenePos().x() -
				      pos().x() -
				      size().width(),
				      2.0) +
				 qPow(event->scenePos().y() -
				      pos().y() -
				      size().height(),
				      2.0));

	  if(distance1 < distance2)
	    m_hoveredSection = Sections::LEFT;
	  else
	    m_hoveredSection = Sections::RIGHT;
	}
    }
  else
    m_hoveredSection = Sections::XYZ;
}

void glitch_proxy_widget::setPos(const QPointF &point)
{
  QGraphicsProxyWidget::setPos(point);
  emit changed();
}

void glitch_proxy_widget::setWidget(QWidget *widget)
{
  QGraphicsProxyWidget::setWidget(widget);

  if(m_object)
    disconnect(this,
	       &glitch_proxy_widget::changed,
	       m_object->contextMenu(),
	       &glitch_floating_context_menu::slotObjectChanged);

  m_object = qobject_cast<glitch_object *> (widget);

  if(m_object)
    connect(this,
	    &glitch_proxy_widget::changed,
	    m_object->contextMenu(),
	    &glitch_floating_context_menu::slotObjectChanged);
}
