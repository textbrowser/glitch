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

#include <QMessageBox>
#include <QScrollBar>
#include <QSettings>
#include <QSqlError>
#include <QTimer>

#include "glitch-alignment.h"
#include "glitch-object.h"
#include "glitch-object-view.h"
#include "glitch-proxy-widget.h"
#include "glitch-scene.h"
#include "glitch-ui.h"
#include "glitch-undo-command.h"
#include "glitch-view.h"

glitch_object_view::glitch_object_view(QWidget *parent):
  glitch_object_view(glitch_common::ProjectTypes::ArduinoProject,
		     0,
		     nullptr,
		     parent)
{
}

glitch_object_view::glitch_object_view
(const glitch_common::ProjectTypes projectType,
 const qint64 id,
 QUndoStack *undoStack,
 QWidget *parent):QGraphicsView(parent)
{
  m_alignment = new glitch_alignment(this);
  m_id = id;
  m_projectType = projectType;
  m_scene = new glitch_scene(m_projectType, this);
  m_scene->setBackgroundBrush(QColor(0, 170, 255));
  m_scene->setUndoStack(m_undoStack = undoStack);
  setAlignment(Qt::AlignLeft | Qt::AlignTop);
  setCacheMode(QGraphicsView::CacheNone);
  setCornerWidget(new QLabel(this));
  setDragMode(QGraphicsView::RubberBandDrag);
  setFrameStyle(QFrame::NoFrame);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setInteractive(true);
  setRenderHints(QPainter::Antialiasing |
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
		 QPainter::LosslessImageRendering |
#endif
		 QPainter::SmoothPixmapTransform |
		 QPainter::TextAntialiasing);
  setRubberBandSelectionMode(Qt::IntersectsItemShape);
  setScene(m_scene);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  connect(m_alignment,
	  &glitch_alignment::changed,
	  this,
	  &glitch_object_view::slotChanged);
  connect(m_scene,
	  &glitch_scene::changed,
	  this,
	  &glitch_object_view::changed);
  connect(m_scene,
	  &glitch_scene::copy,
	  this,
	  &glitch_object_view::copy);
  connect(m_scene,
	  &glitch_scene::saveSignal,
	  this,
	  &glitch_object_view::saveSignal);
  connect(m_scene,
	  &glitch_scene::sceneResized,
	  this,
	  &glitch_object_view::slotSceneResized,
	  Qt::QueuedConnection);
  connect(this,
	  SIGNAL(customContextMenuRequested(const QPoint &)),
	  this,
	  SLOT(slotCustomContextMenuRequested(const QPoint &)));
}

glitch_object_view::~glitch_object_view()
{
  m_scene->purgeRedoUndoProxies();
}

QList<QAction *> glitch_object_view::alignmentActions(void) const
{
  return m_alignment->actions();
}

QList<glitch_object *> glitch_object_view::objects(void) const
{
  return m_scene->objects();
}

glitch_scene *glitch_object_view::scene(void) const
{
  return m_scene;
}

qint64 glitch_object_view::id(void) const
{
  return m_id;
}

void glitch_object_view::adjustScrollBars(void)
{
  QTimer::singleShot(250, this, &glitch_object_view::slotResizeScene);
}

void glitch_object_view::artificialDrop
(const QPointF &point, glitch_object *object, bool &ok)
{
  m_scene->artificialDrop(point, object, ok);
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
      QMenu menu(this);
      auto action = menu.addAction(tr("Paste"), this, SLOT(slotPaste(void)));

      action->setEnabled(!glitch_ui::s_copiedObjects.isEmpty());
      action->setIcon(QIcon(":/paste.png"));
      menu.exec(mapToGlobal(event->pos()));
    }
  else
    QGraphicsView::contextMenuEvent(event);
}

void glitch_object_view::endMacro(void)
{
  if(m_undoStack)
    m_undoStack->endMacro();
}

void glitch_object_view::mouseMoveEvent(QMouseEvent *event)
{
  QGraphicsView::mouseMoveEvent(event);

  if(event && event->buttons() == Qt::LeftButton)
    {
      const auto offset = 7;

      if(event->pos().x() >= width() - 50 && horizontalScrollBar())
	horizontalScrollBar()->setValue
	  (horizontalScrollBar()->value() + offset);
      else if(event->pos().x() - 50 < 0 && horizontalScrollBar())
	horizontalScrollBar()->setValue
	  (horizontalScrollBar()->value() - offset);

      if(event->pos().y() >= height() - 50 && verticalScrollBar())
	verticalScrollBar()->setValue(verticalScrollBar()->value() + offset);
      else if(event->pos().y() - 50 < 0 && verticalScrollBar())
	verticalScrollBar()->setValue(verticalScrollBar()->value() - offset);
    }
}

void glitch_object_view::push(glitch_undo_command *undoCommand)
{
  if(m_undoStack && undoCommand)
    m_undoStack->push(undoCommand);
}

void glitch_object_view::resizeEvent(QResizeEvent *event)
{
  QGraphicsView::resizeEvent(event);
  setSceneRect(size());
}

void glitch_object_view::save(const QSqlDatabase &db, QString &error)
{
  /*
  ** Save the children!
  */

  foreach(auto i, m_scene->items())
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

  if(error.isEmpty())
    m_scene->saveWires(db, error);
}

void glitch_object_view::setEditable(const bool state)
{
  m_scene->setEditable(state);
  setAcceptDrops(state);
  setCacheMode
    (state ? QGraphicsView::CacheBackground : QGraphicsView::CacheNone);
  setDragMode(state ? QGraphicsView::RubberBandDrag : QGraphicsView::NoDrag);
  setInteractive(state);
  setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, !state);
  setOptimizationFlag(QGraphicsView::DontSavePainterState, !state);
}

void glitch_object_view::setIdentifier(const qint64 id)
{
  m_id = id;
}

void glitch_object_view::setSceneRect(const QSize &size)
{
  Q_UNUSED(size);

  auto b(m_scene->itemsBoundingRect().adjusted(0.0, 0.0, 250.0, 250.0));

  b.setTopLeft(QPointF(0.0, 0.0));
  m_scene->setSceneRect
    (0.0,
     0.0,
     static_cast<double> (qMax(static_cast<int> (b.width()),
			       width() - 2 * frameWidth())),
     static_cast<double> (qMax(static_cast<int> (b.height()),
			       height() - 2 * frameWidth())));
}

void glitch_object_view::setUndoStack(QUndoStack *undoStack)
{
  m_scene->setUndoStack(undoStack);
  m_undoStack = undoStack;
}

void glitch_object_view::slotChanged(void)
{
  setSceneRect(size());
}

void glitch_object_view::slotCustomContextMenuRequested(const QPoint &point)
{
  Q_UNUSED(point);
}

void glitch_object_view::slotDelete(void)
{
  m_scene->deleteItems();
}

void glitch_object_view::slotParentWindowClosed(void)
{
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
  foreach(auto i, m_scene->items())
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(proxy)
	proxy->setSelected(true);
    }
}

void glitch_object_view::slotSelectedWidgetsProperties(void)
{
  auto objects(m_scene->selectedObjects());

  if(objects.isEmpty())
    return;

  if(objects.size() >=
     static_cast<int> (glitch_view::Limits::OPEN_WIDGETS_PROPERTIES_PROMPT))
    {
      QMessageBox mb(this);

      mb.setIcon(QMessageBox::Question);
      mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
      mb.setText
	(tr("Are you sure that you wish to open %1 property editor(s)?").
	 arg(objects.size()));
      mb.setWindowIcon(windowIcon());
      mb.setWindowModality(Qt::ApplicationModal);
      mb.setWindowTitle(tr("Glitch: Confirmation"));

      if(mb.exec() == QMessageBox::No)
	{
	  QApplication::processEvents();
	  return;
	}
    }

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  foreach(auto object, objects)
    if(object)
      object->slotShowContextMenu();

  QApplication::restoreOverrideCursor();
}

void glitch_object_view::slotUndo(void)
{
  if(m_undoStack && m_undoStack->canUndo())
    {
      m_undoStack->undo();
      adjustScrollBars();
    }
}

void glitch_object_view::zoom(const int direction)
{
  QSettings settings;
  const auto factor = qBound
    (1.05, settings.value("preferences/zoom_factor", 1.25).toReal(), 1.75);

  if(direction < 0) // Zoom Out
    {
      QTransform transform;

      transform.scale(factor, factor);

      if(transform.isInvertible())
	setTransform(this->transform() * transform.inverted());
    }
  else if(direction > 0) // Zoom In
    {
      QTransform transform;

      transform.scale(factor, factor);
      setTransform(this->transform() * transform);
    }
  else
    setTransform(QTransform());

  centerOn(0.0, 0.0);
}
