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

#include <QFuture>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QStyleOptionGraphicsItem>
#include <QTimer>

#include "glitch-ui.h"

class glitch_recent_diagrams_view_item:
  public QObject, public QGraphicsPixmapItem
{
  Q_OBJECT

 public:
  glitch_recent_diagrams_view_item(const QPixmap &pixmap):
    QObject(), QGraphicsPixmapItem(pixmap)
  {
    m_effect = new QGraphicsDropShadowEffect(this);
    m_effect->setBlurRadius(50.0);
    m_effect->setColor(QColor(59, 59, 59));
    m_effect->setEnabled(false);
    m_effect->setOffset(0.0, 0.0);
    m_showRemoveButton = false;
    setAcceptHoverEvents(true);
    setCacheMode(QGraphicsItem::NoCache);
    setGraphicsEffect(m_effect);
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
  QGraphicsDropShadowEffect *m_effect;
  QPainterPath m_removeButton;
  QPointF m_hoverPoint;
  QString m_fileName;
  bool m_showRemoveButton;

  void hoverEnterEvent(QGraphicsSceneHoverEvent *event)
  {
    QGraphicsPixmapItem::hoverEnterEvent(event);
    m_hoverPoint = event ? event->pos() : QPointF();
    m_showRemoveButton = true;
    prepareEffectOnHoverEnter();
    setCursor(QCursor(Qt::PointingHandCursor));
    update();
  }

  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
  {
    QGraphicsPixmapItem::hoverLeaveEvent(event);
    m_hoverPoint = QPointF();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
    m_removeButton.clear();
#else
    m_removeButton = QPainterPath();
#endif
    m_showRemoveButton = false;
    prepareEffectOnHoverLeave();
    setCursor(QCursor(Qt::ArrowCursor));
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

    if(m_removeButton.contains(event->pos()))
      {
	setCursor(QCursor(Qt::ArrowCursor));
	emit remove(m_fileName);
	emit remove(this);
      }
    else
      emit open(m_fileName);
  }

  void paint(QPainter *painter,
	     const QStyleOptionGraphicsItem *option,
	     QWidget *widget)
  {
    Q_UNUSED(option);

    if(!painter)
      {
	QGraphicsPixmapItem::paint(painter, option, widget);
	return;
      }

    QPen pen;
    const qreal static radius = 7.5;

    pen.setColor(QColor(Qt::white));
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(0.0);
    painter->setBrush(QBrush(pixmap()));
    painter->setPen(pen);
    painter->setRenderHints(QPainter::Antialiasing |
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
			    QPainter::LosslessImageRendering |
#endif
			    QPainter::SmoothPixmapTransform |
			    QPainter::TextAntialiasing,
			    true);
    painter->drawRoundedRect(boundingRect(), radius, radius); // Order.

    QIcon const static icon(":/clear.png");

    m_removeButton.isEmpty() && m_showRemoveButton ?
      m_removeButton.addEllipse(-31.5 + boundingRect().topRight().x(),
				8.5 + boundingRect().topRight().y(),
				25.0,
				25.0),
      m_removeButton.closeSubpath() :
      (void) 0;
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
  }

  void prepareEffectOnHoverEnter(void)
  {
    m_effect->setEnabled(true);
  }

  void prepareEffectOnHoverLeave(void)
  {
    m_effect->setEnabled(false);
  }

 signals:
  void open(const QString &fileName);
  void remove(QGraphicsItem *item);
  void remove(const QString &fileName);
};

class glitch_recent_diagrams_view: public QGraphicsView
{
  Q_OBJECT

 public:
  glitch_recent_diagrams_view(QWidget *parent);
  ~glitch_recent_diagrams_view();
  static QSize s_snapSize;
  QAction *menuAction(void) const;

 private:
  QAction *m_menuAction;
  QAtomicInteger<qint64> m_lastModified;
  QByteArray m_digest;
  QFuture<void> m_gatherRecentDiagramsFuture;
  QString m_recentFilesFileName;
  QTimer m_timer;
  QVector<QGraphicsItem *> m_items;
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
  void enterEvent(QEnterEvent *event);
#else
  void enterEvent(QEvent *event);
#endif
  void gatherRecentDiagrams(const QByteArray &digest, const QString &fileName);
  void keyPressEvent(QKeyEvent *event);
  void populate(const QVectorQPairQImageQString &vector);
  void resizeEvent(QResizeEvent *event);

 private slots:
  void slotPopulateRecentDiagrams(void);
  void slotRecentDiagramsGathered
    (const QByteArray &digest, const QVectorQPairQImageQString &vector);
  void slotRemove(QGraphicsItem *item);

 signals:
  void openDiagram(const QString &fileName);
  void openDiagram(void);
  void recentDiagramsGathered
    (const QByteArray &digest, const QVectorQPairQImageQString &vector);
  void remove(const QString &fileName);
};

#endif
