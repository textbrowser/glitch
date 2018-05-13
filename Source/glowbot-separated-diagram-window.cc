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

#include <QCloseEvent>
#include <QMessageBox>

#include "glowbot-separated-diagram-window.h"
#include "glowbot-view.h"

glowbot_separated_diagram_window::
glowbot_separated_diagram_window(QWidget *parent):QMainWindow(parent)
{
  m_ui.setupUi(this);
}

glowbot_separated_diagram_window::~glowbot_separated_diagram_window()
{
}

void glowbot_separated_diagram_window::closeEvent(QCloseEvent *event)
{
  if(event)
    {
      glowbot_view *view = qobject_cast<glowbot_view *> (centralWidget());

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
	  mb.setWindowTitle(tr("GlowBot: Confirmation"));

	  if(mb.exec() != QMessageBox::Yes)
	    {
	      event->ignore();
	      return;
	    }
	}
    }

  deleteLater();
}

void glowbot_separated_diagram_window::setCentralWidget(QWidget *widget)
{
  if(qobject_cast<glowbot_view *> (widget))
    {
      connect(qobject_cast<glowbot_view *> (widget),
	      SIGNAL(changed(void)),
	      this,
	      SLOT(slotPageChanged(void)),
	      Qt::UniqueConnection);
      connect(qobject_cast<glowbot_view *> (widget),
	      SIGNAL(saved(void)),
	      this,
	      SLOT(slotPageSaved(void)),
	      Qt::UniqueConnection);
    }

  QMainWindow::setCentralWidget(widget);
}

void glowbot_separated_diagram_window::slotPageChanged(void)
{
  glowbot_view *view = qobject_cast<glowbot_view *> (sender());

  if(view)
    {
      if(view->hasChanged())
	QMainWindow::setWindowTitle(tr("GlowBot: %1 (*)").arg(view->name()));
      else
	QMainWindow::setWindowTitle(tr("GlowBot: %1").arg(view->name()));
    }
  else
    QMainWindow::setWindowTitle(tr("GlowBot"));
}

void glowbot_separated_diagram_window::slotPageSaved(void)
{
  slotPageChanged();
}
