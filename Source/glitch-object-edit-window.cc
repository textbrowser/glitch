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

#include <QMenuBar>
#include <QResizeEvent>
#include <QTimer>
#include <QtDebug>

#include "glitch-object-edit-window.h"
#include "glitch-object-view.h"
#include "glitch-scene.h"
#include "glitch-ui.h"

glitch_object_edit_window::glitch_object_edit_window(QWidget *parent):
  QMainWindow(parent)
{
  auto menu = menuBar()->addMenu(tr("&File"));

  m_actions["close"] =
    menu->addAction(tr("&Close"), this, SLOT(close(void)), tr("Ctrl+W"));
  menu = menuBar()->addMenu(tr("&Edit"));
  connect(menu,
	  SIGNAL(aboutToShow(void)),
	  this,
	  SLOT(slotAboutToShowEditMenu(void)));
  m_actions["undo"] =
    menu->addAction(tr("Undo"), this, SIGNAL(undo(void)), tr("Ctrl+Z"));
  m_actions["redo"] =
    menu->addAction(tr("Redo"), this, SIGNAL(redo(void)), tr("Ctrl+Shift+Z"));
  menu->addSeparator();
  m_actions["copy"] =
    menu->addAction(tr("&Copy"), this, SIGNAL(copy(void)), tr("Ctrl+C"));
  m_actions["paste"] =
    menu->addAction(tr("Paste"), this, SIGNAL(paste(void)), tr("Ctrl+V"));
  menu->addSeparator();
  m_actions["delete"] =
    menu->addAction(tr("&Delete"), this, SIGNAL(deleteSignal(void)), tr("Del"));
  m_actions["select all"] = menu->addAction
    (tr("Select &All"), this, SIGNAL(selectAll(void)), tr("Ctrl+A"));
  connect(m_actions.value("copy"),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotAboutToShowEditMenu(void)));
  m_toolBar = new QToolBar(tr("Tool Bar"), this);
  m_toolBar->setIconSize(QSize(32, 32));
  addToolBar(m_toolBar);
  prepareIcons();
  setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint | windowFlags());
}

QWidget *glitch_object_edit_window::centralWidget(void) const
{
  auto frame = qobject_cast<QFrame *> (QMainWindow::centralWidget());

  if(frame && frame->layout() && frame->layout()->itemAt(0))
    return frame->layout()->itemAt(0)->widget();
  else
    return nullptr;
}

glitch_object_edit_window::~glitch_object_edit_window()
{
}

bool glitch_object_edit_window::event(QEvent *event)
{
  if(event && event->type() == QEvent::Show)
    QTimer::singleShot(1500, this, SLOT(slotAboutToShowEditMenu(void)));

  return QMainWindow::event(event);
}

void glitch_object_edit_window::closeEvent(QCloseEvent *event)
{
  QMainWindow::closeEvent(event);
  emit closed();
}

void glitch_object_edit_window::prepareIcons(void)
{
  m_actions.value("close")->setIcon(QIcon::fromTheme("window-close"));
  m_actions.value("copy")->setIcon(QIcon::fromTheme("edit-copy"));
  m_actions.value("delete")->setIcon(QIcon::fromTheme("edit-delete"));
  m_actions.value("paste")->setIcon(QIcon::fromTheme("edit-paste"));
  m_actions.value("redo")->setIcon(QIcon::fromTheme("edit-redo"));
  m_actions.value("select all")->setIcon(QIcon::fromTheme("edit-select-all"));
  m_actions.value("undo")->setIcon(QIcon::fromTheme("edit-undo"));
}

void glitch_object_edit_window::resizeEvent(QResizeEvent *event)
{
  if(event)
    {
      auto view = qobject_cast<glitch_object_view *> (centralWidget());

      if(view)
	view->setSceneRect(event->size());
    }

  QMainWindow::resizeEvent(event);
}

void glitch_object_edit_window::setCentralWidget(QWidget *widget)
{
  if(centralWidget())
    centralWidget()->deleteLater();

  if(!widget)
    return;

  auto frame = new QFrame(this);

  delete frame->layout();
  frame->setLayout(new QVBoxLayout());
  frame->layout()->addWidget(widget);
  frame->layout()->setContentsMargins(9, 9, 9, 9);
  QMainWindow::setCentralWidget(frame);
}

void glitch_object_edit_window::setEditView(glitch_object_view *view)
{
  if(m_editView)
    {
      disconnect(m_editView->scene(),
		 SIGNAL(changed(void)),
		 this,
		 SLOT(slotAboutToShowEditMenu(void)));
      disconnect(m_editView->scene(),
		 SIGNAL(mousePressed(void)),
		 this,
		 SLOT(slotAboutToShowEditMenu(void)));
      disconnect(m_editView->scene(),
		 SIGNAL(selectionChanged(void)),
		 this,
		 SLOT(slotAboutToShowEditMenu(void)));
    }

  m_editView = view;

  if(m_editView)
    {
      connect(m_editView->scene(),
	      SIGNAL(changed(void)),
	      this,
	      SLOT(slotAboutToShowEditMenu(void)),
	      Qt::QueuedConnection);
      connect(m_editView->scene(),
	      SIGNAL(mousePressed(void)),
	      this,
	      SLOT(slotAboutToShowEditMenu(void)));
      connect(m_editView->scene(),
	      SIGNAL(selectionChanged(void)),
	      this,
	      SLOT(slotAboutToShowEditMenu(void)));
    }
}

void glitch_object_edit_window::setUndoStack(QUndoStack *undoStack)
{
  if(m_undoStack)
    {
      disconnect(m_undoStack,
		 SIGNAL(cleanChanged(bool)),
		 this,
		 SLOT(slotAboutToShowEditMenu(void)));
      disconnect(m_undoStack,
		 SIGNAL(indexChanged(int)),
		 this,
		 SLOT(slotAboutToShowEditMenu(void)));
    }

  m_undoStack = undoStack;

  if(m_undoStack)
    {
      connect(m_undoStack,
	      SIGNAL(cleanChanged(bool)),
	      this,
	      SLOT(slotAboutToShowEditMenu(void)));
      connect(m_undoStack,
	      SIGNAL(indexChanged(int)),
	      this,
	      SLOT(slotAboutToShowEditMenu(void)));
    }
}

void glitch_object_edit_window::showEvent(QShowEvent *event)
{
  QMainWindow::showEvent(event);

  auto view = qobject_cast<glitch_object_view *> (centralWidget());

  if(view)
    view->setSceneRect(size());
}

void glitch_object_edit_window::slotAboutToShowEditMenu(void)
{
  m_actions.value("copy")->setEnabled
    (m_editView && !m_editView->scene()->selectedItems().isEmpty());
  m_actions.value("delete")->setEnabled
    (m_editView && !m_editView->scene()->selectedItems().isEmpty());
  m_actions.value("paste")->setEnabled(!glitch_ui::copiedObjects().isEmpty());
  m_actions.value("redo")->setEnabled(m_undoStack && m_undoStack->canRedo());

  if(m_actions.value("redo")->isEnabled() && m_undoStack)
    m_actions.value("redo")->setText
      (tr("Redo (%1)").arg(m_undoStack->redoText()));
  else
    m_actions.value("redo")->setText(tr("Redo"));

  m_actions.value("select all")->setEnabled
    (m_editView && !m_editView->scene()->items().isEmpty());
  m_actions.value("undo")->setEnabled(m_undoStack && m_undoStack->canUndo());

  if(m_actions.value("undo")->isEnabled() && m_undoStack)
    m_actions.value("undo")->setText
      (tr("Undo (%1)").arg(m_undoStack->undoText()));
  else
    m_actions.value("undo")->setText(tr("Undo"));
}
