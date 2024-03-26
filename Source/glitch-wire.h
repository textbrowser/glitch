/*
** Copyright (c) 2015 - 10^10^10, Alexis Megas.
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

#ifndef _glitch_wire_h_
#define _glitch_wire_h_

#include <QGraphicsObject>
#include <QPointer>

class glitch_proxy_widget;

class glitch_wire: public QGraphicsObject
{
  Q_OBJECT

 public:
  enum // Must not be a class.
  {
    Type = QGraphicsObject::UserType + 2
  };

  enum class WireTypes
  {
    CURVE = 0,
    LINE
  };

  glitch_wire(QGraphicsItem *parent);
  ~glitch_wire();
  QPointer<glitch_proxy_widget> leftProxy(void) const;
  QPointer<glitch_proxy_widget> rightProxy(void) const;

  int type(void) const
  {
    return Type;
  }

  void setBoundingRect(const QRectF &rect);
  void setColor(const QColor &color);
  void setLeftProxy(glitch_proxy_widget *proxy);
  void setRightProxy(glitch_proxy_widget *proxy);
  void setWireType(const QString &wireType);
  void setWireType(const WireTypes wireType);
  void setWireWidth(const double value);

 private:
  QColor m_color;
  QPointer<glitch_proxy_widget> m_leftProxy;
  QPointer<glitch_proxy_widget> m_rightProxy;
  QRectF m_boundingRect;
  WireTypes m_wireType;
  double m_wireWidth;
  QPainterPath shape(void) const;
  QRectF boundingRect(void) const;
  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  void paint(QPainter *painter,
	     const QStyleOptionGraphicsItem *opt,
	     QWidget *widget);

 private slots:
  void slotUpdate(const QList<QRectF> &region);

 signals:
  void disconnectWireIfNecessary(void);
};

#endif
