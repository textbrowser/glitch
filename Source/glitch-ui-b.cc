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

#include "glitch-scene.h"
#include "glitch-ui.h"
#include "glitch-view.h"

void glitch_ui::slotAboutToShowProjectMenu(void)
{
  m_ui.action_Generate_Source_Clipboard->setEnabled
    (QApplication::clipboard() && m_currentView);
}

void glitch_ui::slotGenerateSourceClipboard(void)
{
  if(m_currentView)
    m_currentView->generateSourceClipboard();
}

void glitch_ui::slotSaveAsPNG(void)
{
}

void glitch_ui::slotSeparate(void)
{
  slotSeparate(m_currentView);
}

void glitch_ui::slotSpecialTools(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  if(!m_currentView || !m_currentView->scene())
    return;

  auto type(action->data().toString());

  if(type == "adjust-all-sizes")
    m_currentView->slotAllWidgetsAdjustSize();
  else if(type == "adjust-sizes")
    m_currentView->scene()->slotSelectedWidgetsAdjustSize();
  else if(type == "compress-widgets")
    m_currentView->scene()->slotSelectedWidgetsCompress();
  else if(type == "disconnect-widgets")
    m_currentView->scene()->slotSelectedWidgetsDisconnect();
  else if(type == "fonts")
    m_currentView->slotFonts();
  else if(type == "lock-positions")
    m_currentView->scene()->slotSelectedWidgetsLock();
  else if(type == "widget-properties")
    m_currentView->slotSelectedWidgetsProperties();
}
