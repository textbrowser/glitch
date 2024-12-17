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

#include <QGraphicsPixmapItem>
#include <QMouseEvent>

#include "glitch-recent-diagrams-view.h"

class glitch_recent_diagrams_view_item: public QGraphicsPixmapItem
{
 public:
  glitch_recent_diagrams_view_item(const QPixmap &pixmap):
    QGraphicsPixmapItem(pixmap)
  {
  }

  QRectF boundingRect(void) const
  {
    return QRectF(0.0, 0.0, 1.0 * pixmap().width(), 1.0 * pixmap().height());
  }

  void paint(QPainter *painter,
	     const QStyleOptionGraphicsItem *option,
	     QWidget *widget)
  {
    if(!option || !painter || !widget)
      {
	QGraphicsPixmapItem::paint(painter, option, widget);
      }

    QPen pen;

    pen.setColor(QColor(Qt::white));
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(2.5);
    painter->setBrush(QBrush(pixmap()));
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawRoundedRect(boundingRect(), 5.0, 5.0); // Order.
  }
};

glitch_recent_diagrams_view::glitch_recent_diagrams_view(QWidget *parent):
  QGraphicsView(parent)
{
  setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  setDragMode(QGraphicsView::RubberBandDrag);
  setRenderHints(QPainter::Antialiasing |
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
		 QPainter::LosslessImageRendering |
#endif
		 QPainter::SmoothPixmapTransform |
		 QPainter::TextAntialiasing);
  setRubberBandSelectionMode(Qt::IntersectsItemShape);
  setScene(new QGraphicsScene(this));
  setStyleSheet("QGraphicsView {background: transparent; border: none;}");
#ifdef Q_OS_MACOS
  // qt.pointer.dispatch: skipping QEventPoint()

  viewport()->setAttribute(Qt::WA_AcceptTouchEvents, false);
#endif
}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
void glitch_recent_diagrams_view::enterEvent(QEnterEvent *event)
#else
void glitch_recent_diagrams_view::enterEvent(QEvent *event)
#endif
{
  QGraphicsView::enterEvent(event);
  setFocus();
}

void glitch_recent_diagrams_view::leaveEvent(QEvent *event)
{
  QGraphicsView::leaveEvent(event);
}

void glitch_recent_diagrams_view::mouseDoubleClickEvent(QMouseEvent *event)
{
  QGraphicsView::mouseDoubleClickEvent(event);
}

void glitch_recent_diagrams_view::mouseMoveEvent(QMouseEvent *event)
{
  QGraphicsView::mouseMoveEvent(event);
}

void glitch_recent_diagrams_view::populate
(const QVectorQPairQImageQString &vector)
{
  resetTransform();
  scene()->clear();
  setSceneRect(0.0, 0.0, 1.0, 1.0);

  if(vector.isEmpty())
    return;

  const int static columns = 3;

  setSceneRect
    (0.0,
     0.0,
     150.0 * static_cast<qreal> (columns),
     (vector.size() / static_cast<qreal> (columns)) * 200.0 + 200.0);

  int columnIndex = 0;
  int rowIndex = 0;

  for(int i = 0; i < vector.size(); i++)
    {
      auto item = new glitch_recent_diagrams_view_item
	(QPixmap::fromImage(vector.at(i).first));

      if(rowIndex == 0)
	item->setPos(150.0 * columnIndex + 15.0, 15.0);
      else
	item->setPos(150.0 * columnIndex + 15.0, 200.0 * rowIndex + 15.0);

      columnIndex += 1;
      item->setFlag(QGraphicsItem::ItemIsSelectable, true);
      scene()->addItem(item);

      if(columns <= columnIndex)
	{
	  columnIndex = 0;
	  rowIndex += 1;
	}
    }

  setSceneRect(scene()->itemsBoundingRect());
}
