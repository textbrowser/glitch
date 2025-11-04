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

#include <QMouseEvent>
#include <QScrollBar>
#include <QSettings>

#include "glitch-graphicsview.h"
#include "glitch-view.h"
#include "glitch-wire.h"

glitch_graphicsview::glitch_graphicsview(QWidget *parent):QGraphicsView(parent)
{
  m_view = qobject_cast<glitch_view *> (parent);
  setRenderHints(QPainter::Antialiasing |
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
		 QPainter::LosslessImageRendering |
#endif
		 QPainter::SmoothPixmapTransform |
		 QPainter::TextAntialiasing);
#ifdef Q_OS_MACOS
  // qt.pointer.dispatch: skipping QEventPoint()

  viewport()->setAttribute(Qt::WA_AcceptTouchEvents, false);
#endif
}

bool glitch_graphicsview::containsFunction(const QString &name) const
{
  if(m_view && m_view->containsFunction(name))
    return true;
  else
    return false;
}

qreal glitch_graphicsview::scalingFactor(void) const
{
  auto const transform(this->transform());

  if(transform.isScaling())
    return transform.m11();
  else
    return 0.0;
}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
void glitch_graphicsview::enterEvent(QEnterEvent *event)
#else
void glitch_graphicsview::enterEvent(QEvent *event)
#endif
{
  QGraphicsView::enterEvent(event);
  emit mouseEnterEvent();
  setFocus();
}

void glitch_graphicsview::keyPressEvent(QKeyEvent *event)
{
  if((!event) || (scene() && scene()->selectedItems().size() > 0))
    {
      QGraphicsView::keyPressEvent(event);
      return;
    }

  switch(event->key())
    {
    case Qt::Key_Down:
      {
	verticalScrollBar()->setValue
	  (verticalScrollBar()->singleStep() + verticalScrollBar()->value());
	break;
      }
    case Qt::Key_Left:
      {
	horizontalScrollBar()->setValue
	  (-horizontalScrollBar()->singleStep() +
	   horizontalScrollBar()->value());
	break;
      }
    case Qt::Key_Right:
      {
	horizontalScrollBar()->setValue
	  (horizontalScrollBar()->singleStep() +
	   horizontalScrollBar()->value());
	break;
      }
    case Qt::Key_Up:
      {
	verticalScrollBar()->setValue
	  (-verticalScrollBar()->singleStep() + verticalScrollBar()->value());
	break;
      }
    default:
      {
	QGraphicsView::keyPressEvent(event);
	break;
      }
    }
}

void glitch_graphicsview::leaveEvent(QEvent *event)
{
  QGraphicsView::leaveEvent(event);
  emit mouseLeaveEvent();
}

void glitch_graphicsview::mouseDoubleClickEvent(QMouseEvent *event)
{
  if(m_view && m_view->contextMenuAllowed() == false)
    return;

  QGraphicsView::mouseDoubleClickEvent(event);

  if(event && scene())
    {
      auto item = scene()->itemAt(mapToScene(event->pos()), QTransform());

      if(!item || qgraphicsitem_cast<glitch_wire *> (item))
	/*
	** The slot connected to the signal issues mapToGlobal().
	*/

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	emit customContextMenuRequested(event->globalPosition().toPoint());
#else
	emit customContextMenuRequested(event->globalPos());
#endif
    }
}

void glitch_graphicsview::mouseMoveEvent(QMouseEvent *event)
{
  QGraphicsView::mouseMoveEvent(event);

  if(event && event->buttons() == Qt::LeftButton)
    scroll(event->pos());
}

void glitch_graphicsview::scroll(const QPoint &point)
{
  auto const offset = 25;

  if(point.x() >= width() - 50)
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() + offset);
  else if(point.x() - 50 < 0)
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() - offset);

  if(point.y() >= height() - 50)
    verticalScrollBar()->setValue(verticalScrollBar()->value() + offset);
  else if(point.y() - 50 < 0)
    verticalScrollBar()->setValue(verticalScrollBar()->value() - offset);
}

void glitch_graphicsview::zoom(const int direction)
{
  QSettings settings;
  auto const factor = qBound
    (1.05, settings.value("preferences/zoom_factor", 1.25).toReal(), 1.75);

  if(direction < 0) // Zoom Out
    {
      QTransform transform;

      transform.scale(factor, factor);

      if(transform.isInvertible())
	setTransform(this->transform() * transform.inverted());
    }
  else if(direction > 0) // Zoom In
    {
      QTransform transform;

      transform.scale(factor, factor);
      setTransform(this->transform() * transform);
    }
  else
    setTransform(QTransform());

  centerOn(0.0, 0.0);
}
