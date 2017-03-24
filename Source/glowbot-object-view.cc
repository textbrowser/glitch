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

#include <QResizeEvent>
#include <QScrollBar>

#include "glowbot-object-view.h"
#include "glowbot-scene.h"

static const int s_scene_rect_fuzzy = 4;

glowbot_object_view::glowbot_object_view(QWidget *parent):QGraphicsView(parent)
{
  m_scene = new glowbot_scene(this);
  setBackgroundBrush(QBrush(QColor(211, 211, 211), Qt::SolidPattern));
  setDragMode(QGraphicsView::RubberBandDrag);
  setInteractive(true);
  setRenderHints(QPainter::Antialiasing |
		 QPainter::HighQualityAntialiasing | // OpenGL?
		 QPainter::SmoothPixmapTransform |
		 QPainter::TextAntialiasing);
  setRubberBandSelectionMode(Qt::IntersectsItemShape);
  setScene(m_scene);
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  connect(m_scene,
	  SIGNAL(changed(void)),
	  this,
	  SIGNAL(changed(void)));
  connect(this,
	  SIGNAL(customContextMenuRequested(const QPoint &)),
	  this,
	  SLOT(slotCustomContextMenuRequested(const QPoint &)));
}

glowbot_object_view::~glowbot_object_view()
{
}

void glowbot_object_view::contextMenuEvent(QContextMenuEvent *event)
{
  if(event && items(event->pos()).isEmpty())
    {
      event->ignore();
      emit customContextMenuRequested(event->pos());
    }
  else
    QGraphicsView::contextMenuEvent(event);
}

void glowbot_object_view::resizeEvent(QResizeEvent *event)
{
  if(event)
    {
      QRectF b(m_scene->itemsBoundingRect());

      m_scene->setSceneRect
      (0,
       0,
       qMax(static_cast<int> (b.width()),
	    event->size().width() - s_scene_rect_fuzzy),
       qMax(static_cast<int> (b.height()),
	    event->size().height() - (horizontalScrollBar() ?
				      horizontalScrollBar()->height() : 0) -
	    s_scene_rect_fuzzy));
    }

  QGraphicsView::resizeEvent(event);
}

void glowbot_object_view::slotCustomContextMenuRequested(const QPoint &point)
{
  Q_UNUSED(point);
}
