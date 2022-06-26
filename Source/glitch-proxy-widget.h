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

#ifndef _glitch_proxy_widget_h_
#define _glitch_proxy_widget_h_

#include <QGraphicsProxyWidget>
#include <QPointer>

#include "glitch-scene.h"

class glitch_object;

class glitch_proxy_widget: public QGraphicsProxyWidget
{
  Q_OBJECT

 public:
  enum
  {
    Type = QGraphicsProxyWidget::UserType + 1
  };

  enum Sections
  {
    LEFT = 0,
    RIGHT,
    XYZ
  };

  glitch_proxy_widget(QGraphicsItem *parent = nullptr,
		      Qt::WindowFlags wFlags = Qt::WindowFlags());
  ~glitch_proxy_widget();
  QPointer<glitch_object> object(void) const;
  Sections hoveredSection(void) const;
  bool isMandatory(void) const;
  bool isMovable(void) const;

  int objectOrder(void) const
  {
    glitch_scene *scene = qobject_cast<glitch_scene *> (this->scene());

    if(scene)
      return scene->objectOrder(const_cast<glitch_proxy_widget *> (this));
    else
      return -1;
  }

  int type(void) const
  {
    return Type;
  }

  void setPos(const QPointF &point);
  void setWidget(QWidget *widget);

 private:
  QPointer<glitch_object> m_object;
  QPointer<glitch_scene> m_scene;
  Sections m_hoveredSection;
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
  void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
  void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  void paint(QPainter *painter,
	     const QStyleOptionGraphicsItem *opt,
	     QWidget *widget);
  void prepareHoverSection(QGraphicsSceneHoverEvent *event);

 signals:
  void changed(void);
};

#endif
