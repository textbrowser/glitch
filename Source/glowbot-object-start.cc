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
**    derived from GlowBot without specific prior written permission.
**
** GLOWBOT IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** GLOWBOT, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QPainter>
#include <QPolygonF>

#include "glowbot-object-start.h"

glowbot_object_start::glowbot_object_start(QWidget *parent):
  glowbot_object(parent)
{
  m_sideLength = 30.0;
}

glowbot_object_start::~glowbot_object_start()
{
}

void glowbot_object_start::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);

  QList<QColor> colors;
  QPainter painter(this);
  double x = 0.0;
  double y = 0.0;

  colors << QColor(173, 216, 230)
	 << QColor(60, 179, 113)
	 << QColor(255, 69, 0)
	 << QColor(255, 215, 0);

  for(int i = 0; i < 4; i++)
    {
      if(i % 2 == 0)
	x = pos().x();
      else
	x += m_sideLength + 5;

      if(i < 2)
	y = pos().y();
      else if(i == 2)
	y += m_sideLength + 5;

      painter.setBrush(colors.at(i));
      painter.setRenderHints(QPainter::Antialiasing |
			      QPainter::TextAntialiasing);
      painter.save();

      QPolygonF polygon;

      polygon << QPointF(x, y)
	      << QPointF(x + m_sideLength, y)
	      << QPointF(x + m_sideLength, y + m_sideLength)
	      << QPointF(x, y + m_sideLength)
	      << QPointF(x, y);
      painter.drawConvexPolygon(polygon);
      painter.restore();
    }
}
