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

#include <QCheckBox>
#include <QComboBox>
#ifdef GLITCH_MEASURE_ELAPSED_TIME
#include <QElapsedTimer>
#endif
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
#include "glitch-resize-widget.h"
#include "glitch-scene.h"
#include "glitch-tools.h"
#include "glitch-variety.h"

qreal static s_intelligentDistance = 20.0;

glitch_proxy_widget::glitch_proxy_widget
(QGraphicsItem *parent, Qt::WindowFlags wFlags):
  QGraphicsProxyWidget(parent, wFlags)
{
  connect(&m_hoverTimer,
	  &QTimer::timeout,
	  this,
	  &glitch_proxy_widget::slotHoverTimerTimeout);
  m_hoverTimer.setInterval(150);
  m_hoveredSection = Sections::XYZ;
  m_resizeWidget = nullptr;
  setAcceptHoverEvents(true);
  setAttribute(Qt::WA_OpaquePaintEvent, true); // We paint pixels!
  setCacheMode(QGraphicsItem::NoCache);
}

glitch_proxy_widget::~glitch_proxy_widget()
{
  m_hoverTimer.stop();
}

QColor glitch_proxy_widget::selectionColor(void) const
{
  if(m_object && m_object->canvasSettings())
    {
      if(!isMovable())
	return m_object->canvasSettings()->lockColor();
      else
	return m_object->canvasSettings()->selectionColor();
    }
  else if(isMovable())
    return QColor(0, 0, 139);
  else
    return QColor(231, 84, 128);
}

QList<glitch_resize_widget_rectangle *> glitch_proxy_widget::
resizeRectangles(void) const
{
  if(m_resizeWidget)
    return m_resizeWidget->rectangles();
  else
    return QList<glitch_resize_widget_rectangle *> ();
}

QPointer<glitch_object> glitch_proxy_widget::object(void) const
{
  return m_object;
}

QVariant glitch_proxy_widget::itemChange
(GraphicsItemChange change, const QVariant &value)
{
  switch(change)
    {
    case QGraphicsItem::ItemFlagsChange:
      {
	foreach(auto item, resizeRectangles())
	  if(item)
	    item->setVisible(isSelected());

	if(m_object && m_resizeWidget)
	  {
	    if(isMandatory())
	      m_resizeWidget->showEdgeRectanglesForLockedPosition(false, false);
	    else
	      m_resizeWidget->showEdgeRectanglesForLockedPosition
		(isSelected(), m_object->positionLocked());
	  }

	return value;
      }
    case QGraphicsItem::ItemSelectedChange:
      {
	foreach(auto item, resizeRectangles())
	  if(item)
	    item->setVisible(value.toBool());

	if(m_object && m_resizeWidget)
	  {
	    if(isMandatory())
	      m_resizeWidget->showEdgeRectanglesForLockedPosition(false, false);
	    else
	      m_resizeWidget->showEdgeRectanglesForLockedPosition
		(value.toBool(), m_object->positionLocked());
	  }

	return value;
      }
    default:
      {
	break;
      }
    }

  return QGraphicsProxyWidget::itemChange(change, value);
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
  if(event && m_object)
    {
      auto view = m_object->view();

      if(view && view->contextMenuAllowed() == false)
	{
	  QGraphicsProxyWidget::contextMenuEvent(event);
	  return;
	}

      QMenu menu;

      m_object->addActions(menu);
      menu.exec(event->screenPos());
    }
  else
    QGraphicsProxyWidget::contextMenuEvent(event);
}

void glitch_proxy_widget::drawWireIndicator
(QPainter *painter,
 const QPainterPath &path,
 const QRectF &rect,
 const int number)
{
  if(!m_object || !painter || path.isEmpty() || rect.isNull())
    return;

  /*
  ** Draw an input or an output connection indicator.
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

  /*
  ** Draw wiring order text.
  */

  auto font(painter->font());

  font.setBold(true);
  font.setPointSizeF(25.0);
  pen.setColor(QColor(Qt::white));
  painter->setFont(font);
  painter->setPen(pen);
  painter->drawText
    (path.boundingRect(),
     Qt::AlignCenter,
     QString::number(qBound(1, number, 2)));
}

void glitch_proxy_widget::geometryChanged(const QRectF &previousRect)
{
  emit geometryChangedSignal(previousRect);
}

void glitch_proxy_widget::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
  QGraphicsProxyWidget::hoverEnterEvent(event);
  update();
}

void glitch_proxy_widget::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
  QGraphicsProxyWidget::hoverLeaveEvent(event);
  setCursor(QCursor(Qt::ArrowCursor));
  update();
}

void glitch_proxy_widget::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
  QGraphicsProxyWidget::hoverMoveEvent(event);
  update();
}

void glitch_proxy_widget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  if(!event)
    {
      QGraphicsProxyWidget::mousePressEvent(event);
      return;
    }

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

  auto checkBox = qobject_cast<QCheckBox *>
    (m_object->childAt(event->pos().toPoint()));

  if(checkBox)
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
(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  QGraphicsProxyWidget::paint(painter, option, widget);

  if(!m_scene)
    m_scene = qobject_cast<glitch_scene *> (scene());

  if(painter)
    {
      painter->setRenderHints(QPainter::Antialiasing |
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
			      QPainter::LosslessImageRendering |
#endif
			      QPainter::SmoothPixmapTransform |
			      QPainter::TextAntialiasing,
			      true);

      if(m_object && (m_object->objectType() == "arduino-booleanoperator" ||
		      m_object->objectType() == "decoration-arrow" ||
		      m_object->objectType() == "decoration-frame" ||
		      m_object->objectType() == "digitalio-device-display"))
       	{
	  if(!m_object->path().isEmpty())
	    {
	      /*
	      ** Draw a border.
	      */

	      QPen pen;

	      pen.setColor(m_object->borderColor());
	      pen.setJoinStyle(Qt::RoundJoin);
	      pen.setStyle(Qt::SolidLine);
	      pen.setWidthF
		(m_scene ? (m_scene->backgroundBrush().color() ==
			    QColor(Qt::white) ? 2.5 : 3.5) : 3.5);
	      painter->save();
	      painter->setPen(pen);
	      painter->drawPath(m_object->path());
	      painter->restore();
	    }
	}
      else
	{
	  /*
	  ** Draw a border.
	  */

	  QPen pen;

	  if(m_object)
	    pen.setColor(m_object->borderColor());
	  else
	    pen.setColor(QColor(168, 169, 173));

	  pen.setJoinStyle(Qt::RoundJoin);
	  pen.setStyle(Qt::SolidLine);
	  pen.setWidthF
	    (m_scene ? (m_scene->backgroundBrush().color() ==
			QColor(Qt::white) ? 2.5 : 3.5) : 3.5);
	  painter->save();
	  painter->setPen(pen);
	  painter->drawRoundedRect(boundingRect(), 5.0, 5.0);
	  painter->restore();
	}

      if(isMandatory() && option && (option->state & QStyle::State_Selected))
	{
	  /*
	  ** Draw a selection rectangle.
	  */

	  QPen pen;
	  const qreal offset = 0.0;

	  pen.setColor(selectionColor());
	  pen.setJoinStyle(Qt::RoundJoin);
	  pen.setStyle(Qt::SolidLine);
	  pen.setWidthF(2.5);
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

	  QFontMetricsF const fontMetrics(painter->font());
	  QPainterPath path;
	  QPen pen;
	  auto const order(QString::number(objectOrder()));
	  auto const point(this->rect().topRight() / 2.0);
	  auto font(painter->font());

	  path.addRect
	    (point.x() - 15.0,
	     point.y() - 25.0,
	     30.0 + fontMetrics.boundingRect(order).width(),
	     30.0);
	  pen.setColor(QColor(Qt::blue));
	  pen.setWidthF(1.0);
	  painter->save();
	  painter->setPen(pen);
	  painter->drawPath(path);
	  painter->fillPath(path, QColor(173, 216, 230, 150));
	  painter->restore();
	  painter->save();
	  font.setBold(true);
	  font.setPointSizeF(11.5);
	  pen.setColor(QColor(Qt::white));
	  painter->setFont(font);
	  painter->setPen(pen);
	  painter->drawText(path.boundingRect(), Qt::AlignCenter, order);
	  painter->restore();
	}

      if(m_object)
	{
	  auto canDisconnectInput = false;
	  auto canDisconnectOuput = false;

	  if(m_object->hasInput())
	    {
	      /*
	      ** Draw an input port.
	      */

	      QPainterPath path;
	      auto const rect(this->rect());
	      auto const size =
		m_scene && m_scene->objectToBeWiredIn(m_object->proxy()) ?
		30.0 : 10.0;

	      path.addEllipse
		(rect.topLeft().x() - (0.5 + size / 2.0),
		 rect.height() / 2.0 + rect.topLeft().y() - size / 2.0,
		 size,
		 size);

	      if(size < 30.0)
		{
		  if((canDisconnectInput = m_object->isInputWired()))
		    {
		      auto const portColorType =
			m_object->isFullyWired() == false ?
			glitch_object::PortColors::INPUT_CONNECTED :
			glitch_object::PortColors::INPUT_FULL;

		      painter->fillPath
			(path, m_object->portColor(portColorType));
		    }
		  else
		    painter->fillPath
		      (path,
		       m_object->portColor(glitch_object::PortColors::
					   INPUT_DISCONNECTED));
		}
	      else
		{
		  /*
		  ** Draw the first ("1") indicator.
		  */

		  QPen pen;
		  auto font(painter->font());

		  font.setBold(true);
		  font.setPointSizeF(size / 2.0);
		  pen.setColor(QColor(Qt::white));
		  painter->fillPath(path, QColor(255, 121, 0));
		  painter->setFont(font);
		  painter->setPen(pen);
		  painter->drawText(path.boundingRect(), Qt::AlignCenter, "1");
		}
	    }

	  if(m_object->hasOutput())
	    {
	      /*
	      ** Draw an output port.
	      */

	      QPainterPath path;
	      auto const rect(this->rect());
	      auto const size =
		m_scene && m_scene->objectToBeWiredOut(m_object->proxy()) ?
		30.0 : 10.0;

	      path.addEllipse
		(rect.topRight().x() - (0.5 + size / 2.0),
		 rect.height() / 2.0 + rect.topRight().y() - size / 2.0,
		 size,
		 size);

	      if(size < 30.0)
		{
		  if((canDisconnectOuput = m_object->isOutputWired()))
		    painter->fillPath
		      (path,
		       m_object->portColor(glitch_object::PortColors::
					   OUTPUT_CONNECTED));
		  else
		    painter->fillPath
		      (path,
		       m_object->portColor(glitch_object::PortColors::
					   OUTPUT_DISCONNECTED));
		}
	      else
		{
		  /*
		  ** Draw the first ("1") indicator.
		  */

		  QPen pen;
		  auto font(painter->font());

		  font.setBold(true);
		  font.setPointSizeF(size / 2.0);
		  pen.setColor(QColor(Qt::white));
		  painter->fillPath(path, QColor(255, 121, 0));
		  painter->setFont(font);
		  painter->setPen(pen);
		  painter->drawText(path.boundingRect(), Qt::AlignCenter, "1");
		}
	    }

	  if((m_hoveredSection == Sections::LEFT && m_object->hasInput()) ||
	     (m_hoveredSection == Sections::RIGHT && m_object->hasOutput()))
	    {
	      QPainterPath path;
	      auto canDisconnect = true;
	      auto const operation = m_scene ?
		m_scene->toolsOperation() : glitch_tools::Operations::XYZ;
	      auto const rect(this->rect());

	      if(m_hoveredSection == Sections::LEFT && m_object->hasInput())
		{
		  canDisconnect = canDisconnectInput;
		  path.addEllipse(rect.topLeft().x() - size().height() / 2.0,
				  rect.topLeft().y(),
				  size().height(),
				  size().height());
		}
	      else
		{
		  canDisconnect = canDisconnectOuput;
		  path.addEllipse(rect.topRight().x() - size().height() / 2.0,
				  rect.topRight().y(),
				  size().height(),
				  size().height());
		}

	      if(operation == glitch_tools::Operations::INTELLIGENT ||
		 operation == glitch_tools::Operations::WIRE_DISCONNECT)
		{
		  if(operation == glitch_tools::Operations::INTELLIGENT)
		    {
		      if(glitch_variety::keyboardModifiers() &
			 Qt::ControlModifier)
			{
			  if(canDisconnect)
			    {
			      QIcon const static icon(":/clear.png");

			      icon.paint
				(painter, path.boundingRect().toRect());
			      return;
			    }
			  else
			    return;
			}
		    }
		  else
		    {
		      if(canDisconnect)
			{
			  QIcon const static icon(":/clear.png");

			  icon.paint(painter, path.boundingRect().toRect());
			  return;
			}
		      else
			return;
		    }
		}

	      if(m_scene &&
		 m_scene->areObjectsWireCompatible(m_object))
		drawWireIndicator
		  (painter,
		   path,
		   rect,
		   m_scene ? m_scene->selectedForWiringCount() + 1 : 1);
	    }
	}
    }
}

void glitch_proxy_widget::prepareHoverSection(const QPointF &point)
{
  if(!m_scene)
    m_scene = qobject_cast<glitch_scene *> (scene());

  if(m_scene &&
     m_scene->toolsOperation() != glitch_tools::Operations::SELECT)
    {
      if(m_scene->toolsOperation() == glitch_tools::Operations::INTELLIGENT)
	{
	  /*
	  ** Determine if we're close to an input or an output port.
	  */

	  auto distance = qSqrt
	    (qPow(point.x() - pos().x(), 2.0) +
	     qPow(point.y() - pos().y() - size().height() / 2.0, 2.0));

	  if(distance <= s_intelligentDistance)
	    {
	      if(m_object && m_object->hasInput())
		m_hoveredSection = Sections::LEFT;
	      else
		goto done_label;

	      m_hoverTimer.start();
	      update();
	      return;
	    }

	  distance = qSqrt
	    (qPow(point.x() - pos().x() - size().width(), 2.0) +
	     qPow(point.y() - pos().y() - size().height() / 2.0, 2.0));

	  if(distance <= s_intelligentDistance)
	    {
	      if(m_object && m_object->hasOutput())
		m_hoveredSection = Sections::RIGHT;
	      else
		goto done_label;

	      m_hoverTimer.start();
	      update();
	      return;
	    }

	done_label:
	  m_hoverTimer.stop();
	  m_hoveredSection = Sections::XYZ;
	}
      else
	{
	  auto const distance1 = qSqrt(qPow(point.x() - pos().x(), 2.0) +
				       qPow(point.y() - pos().y(), 2.0));
	  auto const distance2 = qSqrt
	    (qPow(point.x() - pos().x() - size().width(), 2.0) +
	     qPow(point.y() - pos().y() - size().height(), 2.0));

	  if(distance1 < distance2)
	    m_hoveredSection = Sections::LEFT;
	  else
	    m_hoveredSection = Sections::RIGHT;

	  m_hoverTimer.start();
	}
    }
  else
    {
      m_hoverTimer.stop();
      m_hoveredSection = Sections::XYZ;
    }

  update();
}

void glitch_proxy_widget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
  QGraphicsProxyWidget::resizeEvent(event);

  if(m_object && m_resizeWidget)
    {
      foreach(auto item, resizeRectangles())
	if(item)
	  item->setVisible(isSelected());

      if(isMandatory())
	m_resizeWidget->showEdgeRectanglesForLockedPosition(false, false);
      else
	m_resizeWidget->showEdgeRectanglesForLockedPosition
	  (isSelected(), m_object->positionLocked());
    }

  emit changed();
}

void glitch_proxy_widget::setObject(QWidget *widget)
{
  if(m_object || widget == nullptr)
    return;

  m_object = qobject_cast<glitch_object *> (widget);
}

void glitch_proxy_widget::setPos(const QPointF &point)
{
  /*
  ** An object may not be attached. This is fine.
  */

  auto const pos(this->pos());

  QGraphicsProxyWidget::setPos(point);

  if(m_object)
    m_object->move(point.toPoint()); // A changed() signal may be emitted.

  if(point != pos)
    emit changed();
}

void glitch_proxy_widget::setWidget(QWidget *widget)
{
  if(widget)
    {
#ifdef GLITCH_MEASURE_ELAPSED_TIME
      QElapsedTimer timer;

      timer.start();
#endif

      if(!QGraphicsProxyWidget::widget())
	QGraphicsProxyWidget::setWidget(widget);

      if(!m_object)
	m_object = qobject_cast<glitch_object *> (widget);

      if(!m_resizeWidget)
	m_resizeWidget = new glitch_resize_widget(this);

#ifdef GLITCH_MEASURE_ELAPSED_TIME
      m_object ?
	qDebug() << "glitch_proxy_widget::setWidget()"
		 << m_object->objectType()
		 << timer.elapsed() :
	qDebug() << "glitch_proxy_widget::setWidget()"
		 << timer.elapsed();
#endif
    }
  else
    QGraphicsProxyWidget::setWidget(nullptr);
}

void glitch_proxy_widget::showResizeHelpers(const bool state)
{
  foreach(auto item, resizeRectangles())
    if(item)
      item->setVisible(state);
}

void glitch_proxy_widget::slotHoverTimerTimeout(void)
{
  prepareHoverSection(m_scene ? m_scene->lastHoverScenePos() : QPointF());
}
