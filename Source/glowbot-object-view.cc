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

#include <QScrollBar>
#include <QSqlError>
#include <QShortcut>

#include "glowbot-alignment.h"
#include "glowbot-object.h"
#include "glowbot-object-view.h"
#include "glowbot-proxy-widget.h"
#include "glowbot-scene.h"
#include "glowbot-view.h"

glowbot_object_view::glowbot_object_view
(const glowbot_common::ProjectType projectType,
 const quint64 id,
 QWidget *parent):QGraphicsView(parent)
{
  m_alignment = new glowbot_alignment(this);
  m_id = id;
  m_projectType = projectType;
  m_scene = new glowbot_scene(m_projectType, this);
  m_scene->setBackgroundBrush(QBrush(QColor(211, 211, 211), Qt::SolidPattern));
  new QShortcut(tr("Ctrl+Shift+Z"),
		this,
		SLOT(slotRedo(void)));
  new QShortcut(tr("Ctrl+Z"),
		this,
		SLOT(slotUndo(void)));
  setDragMode(QGraphicsView::RubberBandDrag);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setInteractive(true);
  setRenderHints(QPainter::Antialiasing |
		 QPainter::HighQualityAntialiasing | // OpenGL?
		 QPainter::SmoothPixmapTransform |
		 QPainter::TextAntialiasing);
  setRubberBandSelectionMode(Qt::IntersectsItemShape);
  setScene(m_scene);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  connect(m_scene,
	  SIGNAL(changed(void)),
	  this,
	  SIGNAL(changed(void)));
  connect(m_scene,
	  SIGNAL(sceneResized(void)),
	  this,
	  SLOT(slotSceneResized(void)));
  connect(this,
	  SIGNAL(customContextMenuRequested(const QPoint &)),
	  this,
	  SLOT(slotCustomContextMenuRequested(const QPoint &)));
}

glowbot_object_view::~glowbot_object_view()
{
}

quint64 glowbot_object_view::id(void) const
{
  return m_id;
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

void glowbot_object_view::save(const QSqlDatabase &db, QString &error)
{
  /*
  ** Save the children!
  */

  QList<QGraphicsItem *> list(m_scene->items());

  for(int i = 0; i < list.size(); i++)
    {
      glowbot_proxy_widget *proxy =
	qgraphicsitem_cast<glowbot_proxy_widget *> (list.at(i));

      if(!proxy)
	continue;

      glowbot_object *widget = qobject_cast<glowbot_object *>
	(proxy->widget());

      if(!widget)
	continue;

      widget->save(db, error);

      if(!error.isEmpty())
	break;
    }
}

void glowbot_object_view::setSceneRect(const QSize &size)
{
  Q_UNUSED(size);

  QRectF b(m_scene->itemsBoundingRect());

  b.setTopLeft(QPointF(0, 0));
  m_scene->setSceneRect
    (0,
     0,
     qMax(static_cast<int> (b.width()), width() - 2 * frameWidth()),
     qMax(static_cast<int> (b.height()), height() - 2 * frameWidth()));
}

void glowbot_object_view::slotCustomContextMenuRequested(const QPoint &point)
{
  QMenu menu(this);

  menu.addAction(tr("&Alignment..."),
		 m_alignment,
		 SLOT(show(void)));
  menu.exec(mapToGlobal(point));
}

void glowbot_object_view::slotParentWindowClosed(void)
{
  m_alignment->close();
}

void glowbot_object_view::slotRedo(void)
{
}

void glowbot_object_view::slotSceneResized(void)
{
  if(parentWidget())
    setSceneRect(parentWidget()->size());
  else
    setSceneRect(size()); // Slight incorrectness.
}

void glowbot_object_view::slotUndo(void)
{
}
