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
#include <QGraphicsSceneHoverEvent>
#include <QScrollBar>
#include <QStyleOptionGraphicsItem>

#include "glitch-recent-diagrams-view.h"

class glitch_recent_diagrams_view_item: public QGraphicsPixmapItem
{
 public:
  glitch_recent_diagrams_view_item(const QPixmap &pixmap):
    QGraphicsPixmapItem(pixmap)
  {
    setAcceptHoverEvents(true);
    setCacheMode(QGraphicsItem::NoCache);
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
  }

  QString fileName(void) const
  {
    return m_fileName;
  }

  void setFileName(const QString &fileName)
  {
    m_fileName = fileName;
  }

 private:
  QPointF m_hoverPoint;
  QString m_fileName;

  QRectF boundingRect(void) const
  {
    return QRectF
      (0.0,
       0.0,
       static_cast<qreal> (pixmap().width()),
       static_cast<qreal> (pixmap().height()));
  }

  void hoverEnterEvent(QGraphicsSceneHoverEvent *event)
  {
    QGraphicsPixmapItem::hoverEnterEvent(event);
    m_hoverPoint = event ? event->pos() : QPointF();
    update();
  }

  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
  {
    QGraphicsPixmapItem::hoverLeaveEvent(event);
    m_hoverPoint = QPointF();
    update();
  }

  void hoverMoveEvent(QGraphicsSceneHoverEvent *event)
  {
    QGraphicsPixmapItem::hoverMoveEvent(event);
    m_hoverPoint = event ? event->pos() : QPointF();
    update();
  }

  void paint(QPainter *painter,
	     const QStyleOptionGraphicsItem *option,
	     QWidget *widget)
  {
    if(!option || !painter)
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
	const qreal static offset = 5.0;

	rect.setHeight(offset + rect.height());
	rect.setWidth(offset + rect.width());
	rect.setX(-offset + rect.x());
	rect.setY(-offset + rect.y());
	path.addRoundedRect(rect, 5.0, 5.0);
	painter->fillPath(path, QColor(222, 141, 174, 100)); // Sassy Pink
      }

    QIcon const icon(":/clear.png");
    QPainterPath path;

    path.addEllipse(-30.0 + boundingRect().topRight().x(),
		    5.0 + boundingRect().topRight().y(),
		    25.0,
		    25.0);
    path.closeSubpath();
    pen.setColor(QColor(222, 141, 174));
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(3.5);
    painter->setPen(pen);
    painter->drawPath(path);
    painter->fillPath
      (path,
       path.contains(m_hoverPoint) ? QColor(Qt::white) : QColor(46, 26, 71));
    icon.paint(painter, path.boundingRect().toRect());
  }
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
  setStyleSheet("QGraphicsView {border: none;}");
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

QStringList glitch_recent_diagrams_view::selectedFileNames(void) const
{
  QMap<QString, char> map;

  foreach(auto item, scene()->selectedItems())
    if(item)
      map[item->data(Qt::UserRole).toString()] = 0;

  return map.keys();
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

  auto const fileNames(selectedFileNames());
  auto const hValue = horizontalScrollBar()->value();
  auto const vValue = verticalScrollBar()->value();

  resetTransform();
  scene()->clear();
  setSceneRect(0.0, 0.0, 1.0, 1.0);

  const int static columns = 3;
  const qreal height = 266.0;
  const qreal offseth = 15.0;
  const qreal offsetw = 15.0;
  const qreal width = 266.0;
  int columnIndex = 0;
  int rowIndex = 0;

  for(int i = 0; i < vector.size(); i++)
    {
      auto pixmap(QPixmap::fromImage(vector.at(i).first));

      pixmap = pixmap.scaled
	(256, 256, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

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
      item->setSelected(fileNames.contains(vector.at(i).second));
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

void glitch_recent_diagrams_view::slotOpen(void)
{
  foreach(auto const &fileName, selectedFileNames())
    emit openDiagram(fileName);
}
