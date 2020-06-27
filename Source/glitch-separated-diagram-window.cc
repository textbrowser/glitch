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

#include <QCloseEvent>
#include <QMessageBox>

#include "glitch-scene.h"
#include "glitch-separated-diagram-window.h"
#include "glitch-ui.h"
#include "glitch-view.h"

glitch_separated_diagram_window::
glitch_separated_diagram_window(QWidget *parent):QMainWindow(parent)
{
  m_ui.setupUi(this);
}

glitch_separated_diagram_window::~glitch_separated_diagram_window()
{
}

void glitch_separated_diagram_window::closeEvent(QCloseEvent *event)
{
  if(event)
    {
      auto *view = qobject_cast<glitch_view *> (centralWidget());

      if(view && view->hasChanged())
	{
	  QMessageBox mb(this);

	  mb.setIcon(QMessageBox::Question);
	  mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
	  mb.setText
	    (tr("The display has not been saved. Are you sure that "
		"you wish to close it?"));
	  mb.setWindowIcon(windowIcon());
	  mb.setWindowModality(Qt::WindowModal);
	  mb.setWindowTitle(tr("Glitch: Confirmation"));

	  if(mb.exec() != QMessageBox::Yes)
	    {
	      QApplication::processEvents();
	      event->ignore();
	      return;
	    }

	  QApplication::processEvents();
	}
    }

  deleteLater();
}

void glitch_separated_diagram_window::prepareActionWidgets(void)
{
  auto *view = qobject_cast<glitch_view *> (centralWidget());

  if(!view)
    {
      m_ui.action_Copy->setEnabled(false);
      m_ui.action_Delete->setEnabled(false);
      m_ui.action_Paste->setEnabled(false);
      m_ui.action_Select_All->setEnabled(false);
    }
  else
    {
      m_ui.action_Copy->setEnabled
	(view && !view->scene()->selectedItems().empty());
      m_ui.action_Delete->setEnabled
	(view && !view->scene()->selectedItems().empty());
      m_ui.action_Paste->setEnabled(!glitch_ui::copiedObjects().isEmpty());
      m_ui.action_Select_All->setEnabled
	(view && view->scene()->items().size() > 2);
    }

  prepareRedoUndoActions();
}

void glitch_separated_diagram_window::prepareRedoUndoActions(void)
{
  auto *view = qobject_cast<glitch_view *> (centralWidget());

  if(!view)
    {
      m_ui.action_Redo->setEnabled(false);
      m_ui.action_Redo->setText(tr("Redo"));
      m_ui.action_Undo->setEnabled(false);
      m_ui.action_Undo->setText(tr("Undo"));
      return;
    }

  m_ui.action_Redo->setEnabled(view->canRedo());

  if(m_ui.action_Redo->isEnabled())
    m_ui.action_Redo->setText(tr("Redo (%1)").arg(view->redoText()));
  else
    m_ui.action_Redo->setText(tr("Redo"));

  m_ui.action_Undo->setEnabled(view->canUndo());

  if(m_ui.action_Undo->isEnabled())
    m_ui.action_Undo->setText(tr("Undo (%1)").arg(view->undoText()));
  else
    m_ui.action_Undo->setText(tr("Undo"));
}

void glitch_separated_diagram_window::setCentralWidget(QWidget *widget)
{
  if(qobject_cast<glitch_view *> (widget))
    {
      connect(qobject_cast<glitch_view *> (widget),
	      SIGNAL(changed(void)),
	      this,
	      SLOT(slotPageChanged(void)),
	      Qt::UniqueConnection);
      connect(qobject_cast<glitch_view *> (widget),
	      SIGNAL(saved(void)),
	      this,
	      SLOT(slotPageSaved(void)),
	      Qt::UniqueConnection);
    }

  QMainWindow::setCentralWidget(widget);
}

void glitch_separated_diagram_window::slotPageChanged(void)
{
  auto *view = qobject_cast<glitch_view *> (sender());

  if(view)
    {
      if(view->hasChanged())
	QMainWindow::setWindowTitle(tr("Glitch: %1 (*)").arg(view->name()));
      else
	QMainWindow::setWindowTitle(tr("Glitch: %1").arg(view->name()));
    }
  else
    QMainWindow::setWindowTitle(tr("Glitch"));
}

void glitch_separated_diagram_window::slotPageSaved(void)
{
  slotPageChanged();
}
