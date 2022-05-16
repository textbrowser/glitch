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

#include <QScrollBar>
#include <QSqlError>
#include <QTimer>
#include <QtDebug>

#include "glitch-alignment.h"
#include "glitch-object.h"
#include "glitch-object-view.h"
#include "glitch-proxy-widget.h"
#include "glitch-scene.h"
#include "glitch-ui.h"
#include "glitch-undo-command.h"
#include "glitch-view.h"

glitch_object_view::glitch_object_view
(const glitch_common::ProjectTypes projectType,
 const quint64 id,
 QUndoStack *undoStack,
 QWidget *parent):QGraphicsView(parent)
{
  m_id = id;
  m_projectType = projectType;
  m_scene = new glitch_scene(m_projectType, this);
  m_scene->setBackgroundBrush(QBrush(QColor(211, 211, 211), Qt::SolidPattern));
  m_scene->setUndoStack(m_undoStack = undoStack);
  setDragMode(QGraphicsView::RubberBandDrag);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setInteractive(true);
  setRenderHints(QPainter::Antialiasing |
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
		 QPainter::HighQualityAntialiasing | // OpenGL?
#endif
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

glitch_object_view::~glitch_object_view()
{
  if(m_alignment)
    m_alignment->deleteLater();
}

QUndoStack *glitch_object_view::undoStack(void) const
{
  return m_undoStack;
}

glitch_scene *glitch_object_view::scene(void) const
{
  return m_scene;
}

quint64 glitch_object_view::id(void) const
{
  return m_id;
}

void glitch_object_view::adjustScrollBars(void)
{
  QTimer::singleShot(250, this, SLOT(slotResizeScene(void)));
}

void glitch_object_view::artificialDrop
(const QPointF &point, glitch_object *object)
{
  m_scene->artificialDrop(point, object);
  adjustScrollBars();
}

void glitch_object_view::beginMacro(const QString &text)
{
  if(!m_undoStack)
    return;

  if(text.trimmed().isEmpty())
    m_undoStack->beginMacro(tr("unknown"));
  else
    m_undoStack->beginMacro(text);
}

void glitch_object_view::contextMenuEvent(QContextMenuEvent *event)
{
  if(event && items(event->pos()).isEmpty())
    {
      event->ignore();
      emit customContextMenuRequested(event->pos());
    }
  else
    QGraphicsView::contextMenuEvent(event);
}

void glitch_object_view::endMacro(void)
{
  if(m_undoStack)
    m_undoStack->endMacro();
}

void glitch_object_view::push(glitch_undo_command *undoCommand)
{
  if(undoCommand)
    m_undoStack->push(undoCommand);
}

void glitch_object_view::save(const QSqlDatabase &db, QString &error)
{
  /*
  ** Save the children!
  */

  for(const auto i : m_scene->items())
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(!proxy)
	continue;

      auto object = qobject_cast<glitch_object *> (proxy->widget());

      if(!object)
	continue;

      object->save(db, error);

      if(!error.isEmpty())
	break;
    }
}

void glitch_object_view::setSceneRect(const QSize &size)
{
  Q_UNUSED(size);

  auto b(m_scene->itemsBoundingRect().adjusted(0.0, 0.0, 25.0, 25.0));

  b.setTopLeft(QPointF(0.0, 0.0));
  m_scene->setSceneRect
    (0.0,
     0.0,
     static_cast<double> (qMax(static_cast<int> (b.width()),
			       width() - 2 * frameWidth())),
     static_cast<double> (qMax(static_cast<int> (b.height()),
			       height() - 2 * frameWidth())));
}

void glitch_object_view::slotChanged(void)
{
  setSceneRect(size());
}

void glitch_object_view::slotCustomContextMenuRequested(const QPoint &point)
{
  QMenu menu(this);

  menu.addAction(tr("&Alignment Tool..."),
		 this,
		 SLOT(slotShowAlignment(void)));
  menu.exec(mapToGlobal(point));
}

void glitch_object_view::slotDelete(void)
{
  m_scene->deleteItems();
}

void glitch_object_view::slotParentWindowClosed(void)
{
  if(m_alignment)
    m_alignment->close();
}

void glitch_object_view::slotPaste(void)
{
  glitch_ui::paste(this, m_undoStack);
  adjustScrollBars();
}

void glitch_object_view::slotRedo(void)
{
  if(m_undoStack && m_undoStack->canRedo())
    {
      m_undoStack->redo();
      adjustScrollBars();
    }
}

void glitch_object_view::slotResizeScene(void)
{
  setSceneRect(size());
}

void glitch_object_view::slotSceneResized(void)
{
  if(parentWidget())
    setSceneRect(parentWidget()->size());
  else
    setSceneRect(size()); // Slight incorrectness.
}

void glitch_object_view::slotSelectAll(void)
{
  for(const auto i : m_scene->items())
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(proxy)
	proxy->setSelected(true);
    }
}

void glitch_object_view::slotShowAlignment(void)
{
  if(!m_alignment)
    {
      m_alignment = new glitch_alignment(this);
      connect(m_alignment,
	      &glitch_alignment::changed,
	      this,
	      &glitch_object_view::slotChanged);
    }

  m_alignment->showNormal();
  m_alignment->activateWindow();
  m_alignment->raise();
}

void glitch_object_view::slotUndo(void)
{
  if(m_undoStack && m_undoStack->canUndo())
    {
      m_undoStack->undo();
      adjustScrollBars();
    }
}
