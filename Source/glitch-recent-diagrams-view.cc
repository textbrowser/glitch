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

#include <QGraphicsDropShadowEffect>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QScrollBar>
#include <QStyleOptionGraphicsItem>

#include "glitch-recent-diagrams-view.h"

class glitch_recent_diagrams_view_item: public QGraphicsPixmapItem
{
 public:
  glitch_recent_diagrams_view_item(const QPixmap &pixmap):
    QGraphicsPixmapItem(pixmap)
  {
    setCacheMode(QGraphicsItem::NoCache);
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
  }

  QRectF boundingRect(void) const
  {
    return QRectF(0.0, 0.0, 1.0 * pixmap().width(), 1.0 * pixmap().height());
  }

  QString fileName(void) const
  {
    return m_fileName;
  }

  void paint(QPainter *painter,
	     const QStyleOptionGraphicsItem *option,
	     QWidget *widget)
  {
    if(!option || !painter || !widget)
      {
	QGraphicsPixmapItem::paint(painter, option, widget);
	return;
      }

    QPen pen;

    pen.setColor(QColor(Qt::white));
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(0.0);
    painter->setBrush(QBrush(pixmap()));
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawRoundedRect(boundingRect(), 5.0, 5.0); // Order.

    if(option->state & (QStyle::State_HasFocus | QStyle::State_Selected))
      {
	QPainterPath path;
	auto rect(boundingRect());
	const qreal offset = 5.0;

	rect.setHeight(offset + rect.height());
	rect.setWidth(offset + rect.width());
	rect.setX(-offset + rect.x());
	rect.setY(-offset + rect.y());
	path.addRoundedRect(rect, 5.0, 5.0);
	painter->fillPath(path, QColor(222, 141, 174, 100)); // Sassy Pink
      }
  }

  void setFileName(const QString &fileName)
  {
    m_fileName = fileName;
  }

 private:
  QString m_fileName;
};

glitch_recent_diagrams_view::glitch_recent_diagrams_view(QWidget *parent):
  QGraphicsView(parent)
{
  m_menuAction = new QAction
    (QIcon(":/recent.png"), tr("Recent Diagrams"), this);
  setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  setCacheMode(QGraphicsView::CacheNone);
  setDragMode(QGraphicsView::NoDrag);
  setRenderHints(QPainter::Antialiasing |
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
		 QPainter::LosslessImageRendering |
#endif
		 QPainter::SmoothPixmapTransform |
		 QPainter::TextAntialiasing);
  setRubberBandSelectionMode(Qt::IntersectsItemShape);
  setScene(new QGraphicsScene(this));
  setStyleSheet("QGraphicsView {background: transparent; border: none;}");
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
#ifdef Q_OS_MACOS
  // qt.pointer.dispatch: skipping QEventPoint()

  viewport()->setAttribute(Qt::WA_AcceptTouchEvents, false);
#endif
}

QAction *glitch_recent_diagrams_view::menuAction(void) const
{
  return m_menuAction;
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

void glitch_recent_diagrams_view::populate
(const QVectorQPairQImageQString &vector)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  auto const fileName
    (scene()->selectedItems().value(0)->data(Qt::UserRole).toString());
  auto const hValue = horizontalScrollBar()->value();
  auto const vValue = verticalScrollBar()->value();

  resetTransform();
  scene()->clear();
  setSceneRect(0.0, 0.0, 1.0, 1.0);

  const int static columns = 2;
  const qreal height = 330.0;
  const qreal offseth = 15.0;
  const qreal offsetw = 15.0;
  const qreal width = 490.0;
  int columnIndex = 0;
  int rowIndex = 0;

  for(int i = 0; i < vector.size(); i++)
    {
      auto pixmap(QPixmap::fromImage(vector.at(i).first));

      pixmap = pixmap.scaled
	(480, 320, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

      auto effect = new QGraphicsDropShadowEffect();
      auto item = new glitch_recent_diagrams_view_item(pixmap);

      effect->setBlurRadius(10.0);
      effect->setColor(QColor(Qt::gray));
      effect->setOffset(2.5, 2.5);
      item->setGraphicsEffect(effect);

      if(rowIndex == 0)
	item->setPos(columnIndex * width + offsetw, offseth);
      else
	item->setPos
	  (columnIndex * width + offsetw, height * rowIndex + offseth);

      columnIndex += 1;
      item->setData(Qt::UserRole, vector.at(i).second);
      item->setFileName(vector.at(i).second);
      item->setFlag(QGraphicsItem::ItemIsSelectable, true);
      item->setSelected(fileName == vector.at(i).second);
      item->setToolTip(vector.at(i).second);
      scene()->addItem(item);

      if(columnIndex >= columns)
	{
	  columnIndex = 0;
	  rowIndex += 1;
	}
    }

  auto rect(scene()->itemsBoundingRect());

  rect.setX(0.0);
  rect.setY(0.0);
  rect.setHeight(offseth + rect.height());

  horizontalScrollBar()->setValue(hValue);
  setSceneRect(rect);
  verticalScrollBar()->setValue(vValue);
  QApplication::restoreOverrideCursor();
}
