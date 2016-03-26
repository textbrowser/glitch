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

#include <QSettings>

#include "glowbot-misc.h"
#include "glowbot-ui.h"
#include "glowbot-view.h"

glowbot_ui::glowbot_ui(void):QMainWindow(0)
{
  m_ui.setupUi(this);
  connect(m_ui.action_New_Arduino,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotNewArduinoDiagram(void)));
  connect(m_ui.action_Quit,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotQuit(void)));
  connect(m_ui.action_Save_Current_Diagram,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSaveCurrentDiagram(void)));
  connect(m_ui.tab,
	  SIGNAL(tabCloseRequested(int)),
	  this,
	  SLOT(slotCloseDiagram(int)));
  restoreSettings();
  show();
}

glowbot_ui::~glowbot_ui()
{
}

glowbot_view *glowbot_ui::page(const int index)
{
  return qobject_cast<glowbot_view *> (m_ui.tab->widget(index));
}

void glowbot_ui::closeEvent(QCloseEvent *event)
{
  saveSettings();
  QMainWindow::closeEvent(event);
}

void glowbot_ui::restoreSettings(void)
{
  QSettings settings;

  restoreGeometry(settings.value("main_window/geometry").toByteArray());
}

void glowbot_ui::saveSettings(void)
{
  QSettings settings;

  settings.setValue("main_window/geometry", saveGeometry());
}

void glowbot_ui::slotCloseDiagram(int index)
{
  glowbot_view *page = this->page(index);

  if(page)
    page->deleteLater();
}

void glowbot_ui::slotNewArduinoDiagram(void)
{
  glowbot_view *page = new glowbot_view
    (tr("Arduino Diagram"), glowbot_common::ArduinoProject, this);

  m_ui.tab->addTab(page, tr("Arduino Diagram"));
}

void glowbot_ui::slotQuit(void)
{
  close();
}

void glowbot_ui::slotSaveCurrentDiagram(void)
{
  glowbot_view *page = this->page(m_ui.tab->currentIndex());

  if(page)
    page->save();
}
