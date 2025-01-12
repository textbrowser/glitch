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

#ifndef _glitch_recent_diagrams_view_h_
#define _glitch_recent_diagrams_view_h_

#include <QGraphicsDropShadowEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QStyleOptionGraphicsItem>

#include "glitch-ui.h"

class glitch_recent_diagrams_view_item:
  public QObject, public QGraphicsPixmapItem
{
  Q_OBJECT

 public:
  glitch_recent_diagrams_view_item(const QPixmap &pixmap):
    QObject(), QGraphicsPixmapItem(pixmap)
  {
    setAcceptHoverEvents(true);
    setCacheMode(QGraphicsItem::NoCache);
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
  }

  ~glitch_recent_diagrams_view_item()
  {
  }

  QRectF boundingRect(void) const
  {
    return QRectF(0.0, 0.0, 372.0, 240.0);
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
  QPainterPath m_removeButton;
  QPointF m_hoverPoint;
  QString m_fileName;

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

  void mousePressEvent(QGraphicsSceneMouseEvent *event)
  {
    if(!event)
      {
	QGraphicsPixmapItem::mousePressEvent(event);
	return;
      }

    m_removeButton.contains(event->pos()) ? emit remove(m_fileName) : (void) 0;
    m_removeButton.contains(event->pos()) ? emit remove(this) : (void) 0;
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

    QIcon const icon(":/clear.png");

    m_removeButton.clear();
    m_removeButton.addEllipse(-31.5 + boundingRect().topRight().x(),
			      8.5 + boundingRect().topRight().y(),
			      25.0,
			      25.0);
    m_removeButton.closeSubpath();
    pen.setColor(QColor(222, 141, 174));
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(3.5);
    painter->setPen(pen);
    painter->drawPath(m_removeButton);
    painter->fillPath
      (m_removeButton,
       m_removeButton.contains(m_hoverPoint) ?
       QColor(Qt::black) : QColor(Qt::white));
    icon.paint(painter, m_removeButton.boundingRect().toRect());

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
  }

 signals:
  void remove(QGraphicsItem *item);
  void remove(const QString &fileName);
};

class glitch_recent_diagrams_view: public QGraphicsView
{
  Q_OBJECT

 public:
  glitch_recent_diagrams_view(QWidget *parent);
  QAction *menuAction(void) const;
  void populate(const QVectorQPairQImageQString &vector);

 public slots:
  void slotOpen(void);

 private:
  QAction *m_menuAction;
  QStringList selectedFileNames(void) const;
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
  void enterEvent(QEnterEvent *event);
#else
  void enterEvent(QEvent *event);
#endif

 private slots:
  void slotRemove(QGraphicsItem *item);

 signals:
  void openDiagram(const QString &fileName);
  void openDiagram(void);
  void remove(const QString &fileName);
};

#endif
