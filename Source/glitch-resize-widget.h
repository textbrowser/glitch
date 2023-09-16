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

#ifndef _glitch_resize_widget_h_
#define _glitch_resize_widget_h_

#include <QGraphicsItem>

#include "glitch-resize-widget-rectangle.h"

class glitch_proxy_widget;

class glitch_resize_widget: public QGraphicsItem
{
 public:
  glitch_resize_widget(QGraphicsItem *parent);
  ~glitch_resize_widget();
  QList<glitch_resize_widget_rectangle *> rectangles(void) const;
  QRectF boundingRect(void) const;
  void paint(QPainter *painter,
	     const QStyleOptionGraphicsItem *option,
	     QWidget *widget = 0);
  void positionEdgeRectangles(void);
  void showEdgeRectangles(const bool state);
  void showEdgeRectanglesForLockedPosition(const bool isParentSelected,
					   const bool state);

 private:
  QHash<glitch_resize_widget_rectangle::RectangleLocations,
        glitch_resize_widget_rectangle *> m_rectangles;
  glitch_proxy_widget *m_parent;
  void prepareRectangles(void);
};

#endif
