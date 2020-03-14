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

#include <QMenu>

#include "glitch-floating-context-menu.h"

glitch_floating_context_menu::glitch_floating_context_menu(QWidget *parent):
  QDialog(parent)
{
  m_menu = new QMenu(this);
  m_ui.setupUi(this);
  connect(m_ui.toolButton,
	  SIGNAL(clicked(void)),
	  m_ui.toolButton,
	  SLOT(showMenu(void)));
  m_ui.toolButton->setMenu(m_menu);
  m_ui.toolButton->setStyleSheet
    ("QToolButton {margin-bottom: 1px; margin-top: 1px;}"
     "QToolButton::menu-button {border: none;}"
     "QToolButton::menu-indicator {image: none;}");
  setWindowModality(Qt::NonModal);
}

glitch_floating_context_menu::~glitch_floating_context_menu()
{
}

QMenu *glitch_floating_context_menu::menu(void) const
{
  return m_menu;
}
