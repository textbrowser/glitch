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

#include <QDir>
#include <QMenu>
#include <QResizeEvent>
#include <QScrollBar>
#include <QSqlError>
#include <QSqlQuery>

#include "glowbot-misc.h"
#include "glowbot-object.h"
#include "glowbot-object-start.h"
#include "glowbot-proxy-widget.h"
#include "glowbot-scene.h"
#include "glowbot-view.h"

static const int s_scene_rect_fuzzy = 4;

glowbot_view::glowbot_view
(const QString &name,
 const glowbot_common::ProjectType projectType,
 QWidget *parent):QWidget(parent)
{
  m_ui.setupUi(this);
  m_canvasSettings = new glowbot_canvas_settings(this);
  m_menuAction = new QAction(QIcon(":/Logo/glowbot-arduino-logo.png"),
			     name,
			     this);
  m_name = name;
  m_projectType = projectType;
  m_scene = new glowbot_scene(this);
  m_startObject = new glowbot_object_start(this);
  m_view = new QGraphicsView(this);
  m_view->setBackgroundBrush(QBrush(QColor(211, 211, 211), Qt::SolidPattern));
  m_view->setDragMode(QGraphicsView::RubberBandDrag);
  m_view->setInteractive(true);
  m_view->setRenderHints(QPainter::Antialiasing |
			 QPainter::HighQualityAntialiasing | // OpenGL?
			 QPainter::SmoothPixmapTransform |
			 QPainter::TextAntialiasing);
  m_view->setRubberBandSelectionMode(Qt::IntersectsItemShape);
  m_view->setScene(m_scene);
  m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  connect(m_canvasSettings,
	  SIGNAL(accepted(void)),
	  this,
	  SLOT(slotCanvasSettingsChanged(void)));
  connect(m_scene,
	  SIGNAL(changed(void)),
	  this,
	  SIGNAL(changed(void)));
  connect(this,
	  SIGNAL(customContextMenuRequested(const QPoint &)),
	  this,
	  SLOT(slotCustomContextMenuRequested(const QPoint &)));
  layout()->addWidget(m_view);
}

glowbot_view::~glowbot_view()
{
}

QAction *glowbot_view::menuAction(void) const
{
  return m_menuAction;
}

QString glowbot_view::name(void) const
{
  return m_name;
}

bool glowbot_view::hasChanged(void) const
{
  return m_scene->hasChanged();
}

bool glowbot_view::save(QString &error)
{
  QString connectionName("");
  bool ok = false;

  {
    QSqlDatabase db(glowbot_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName
      (glowbot_misc::homePath() + QDir::separator() +
       QString("%1.db").arg(m_name));

    if(db.open())
      {
	QSqlQuery query(db);

	if(!(ok = query.exec()))
	  error = query.lastError().text();
      }
    else
      error = db.lastError().text();

    db.close();
  }

  glowbot_common::discardDatabase(connectionName);
  return ok;
}

void glowbot_view::contextMenuEvent(QContextMenuEvent *event)
{
  if(event && m_view->items(event->pos()).isEmpty())
    {
      event->ignore();
      emit customContextMenuRequested(event->pos());
    }
  else
    QWidget::contextMenuEvent(event);
}

void glowbot_view::resizeEvent(QResizeEvent *event)
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
	    event->size().height() - (m_view->horizontalScrollBar() ?
				      m_view->horizontalScrollBar()->
				      height() : 0) -
	    s_scene_rect_fuzzy));
    }

  QWidget::resizeEvent(event);
}

void glowbot_view::slotCanvasSettingsChanged(void)
{
  m_view->setBackgroundBrush
    (QBrush(m_canvasSettings->backgroundColor(), Qt::SolidPattern));
  m_view->setViewportUpdateMode(m_canvasSettings->viewportUpdateMode());
}

void glowbot_view::slotCustomContextMenuRequested(const QPoint &point)
{
  QMenu menu(this);

  menu.addAction("Show Canvas &Settings...",
		 this,
		 SLOT(slotShowCanvasSettings(void)));
  menu.exec(mapToGlobal(point));
}

void glowbot_view::slotShowCanvasSettings(void)
{
  m_canvasSettings->setName(m_name);
  m_canvasSettings->setViewportUpdateMode(m_view->viewportUpdateMode());
  m_canvasSettings->show();
}
