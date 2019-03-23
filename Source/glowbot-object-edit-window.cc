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
#include <QShortcut>

#include "glowbot-object-edit-window.h"
#include "glowbot-object-view.h"

glowbot_object_edit_window::glowbot_object_edit_window(QWidget *parent):
  QMainWindow(parent, Qt::WindowFlags() | Qt::WindowStaysOnTopHint)
{
  new QShortcut(tr("Ctrl+W"),
		this,
		SLOT(close(void)));
  setContentsMargins(9, 9, 9, 9);
}

glowbot_object_edit_window::~glowbot_object_edit_window()
{
}

void glowbot_object_edit_window::closeEvent(QCloseEvent *event)
{
  emit closed();
  QMainWindow::closeEvent(event);
}

void glowbot_object_edit_window::resizeEvent(QResizeEvent *event)
{
  if(event)
    {
      glowbot_object_view *view = qobject_cast<glowbot_object_view *>
	(centralWidget());

      if(view)
	view->setSceneRect(event->size());
    }

  QMainWindow::resizeEvent(event);
}
