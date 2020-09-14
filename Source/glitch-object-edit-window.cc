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

#include "glitch-object-edit-window.h"
#include "glitch-object-view.h"
#include "glitch-scene.h"
#include "glitch-ui.h"

glitch_object_edit_window::glitch_object_edit_window(QWidget *parent):
  QMainWindow(parent)
{
  auto *menu = menuBar()->addMenu(tr("&File"));

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
  setContentsMargins(9, 9, 9, 9);
}

glitch_object_edit_window::~glitch_object_edit_window()
{
}

void glitch_object_edit_window::closeEvent(QCloseEvent *event)
{
  QMainWindow::closeEvent(event);
  emit closed();
}

void glitch_object_edit_window::resizeEvent(QResizeEvent *event)
{
  if(event)
    {
      auto *view = qobject_cast<glitch_object_view *> (centralWidget());

      if(view)
	view->setSceneRect(event->size());
    }

  QMainWindow::resizeEvent(event);
}

void glitch_object_edit_window::setEditView(glitch_object_view *view)
{
  m_editView = view;
}

void glitch_object_edit_window::slotAboutToShowEditMenu(void)
{
  m_actions.value("delete")->setEnabled
    (m_editView && !m_editView->scene()->selectedItems().isEmpty());
  m_actions.value("select all")->setEnabled
    (m_editView && !m_editView->scene()->items().isEmpty());
}
