/*
** Copyright (c) 2015 - 10^10^10, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
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

#include <QFont>
#include <QIcon>

#include "glitch-application.h"
#include "glitch-object-lineedit.h"
#include "glitch-ui.h"

QAtomicInteger<int> glitch_application::s_blockShortcuts = 0;

glitch_application::glitch_application(int &argc, char **argv):
  QApplication(argc, argv)
{
  installEventFilter(this);

  auto font(this->font());

  font.setStyleStrategy
    (QFont::StyleStrategy(QFont::PreferAntialias | QFont::PreferQuality));
  glitch_ui::s_defaultApplicationFont = font;
  setFont(font);
  setWindowIcon(QIcon(":/Logo/glitch-logo.png"));
}

bool glitch_application::eventFilter(QObject *object, QEvent *event)
{
  if(!event || !object)
    return QApplication::eventFilter(object, event);

  if(event->type() == QEvent::Shortcut ||
     event->type() == QEvent::ShortcutOverride)
    {
      auto lineedit = qobject_cast<glitch_object_lineedit *> (object);

      if(lineedit && lineedit->isReadOnly())
	return true;
    }

  if(blockShortcuts() > 0 && event->type() == QEvent::Shortcut)
    {
      s_blockShortcuts.fetchAndStoreOrdered(0);
      return true;
    }

  return QApplication::eventFilter(object, event);
}

int glitch_application::blockShortcuts(void)
{
  return s_blockShortcuts.fetchAndAddOrdered(0);
}

void glitch_application::blockShortcutsDecrement(void)
{
  auto const value = s_blockShortcuts.fetchAndSubOrdered(1);

  if(value <= 0)
    s_blockShortcuts.fetchAndStoreOrdered(0);
}

void glitch_application::blockShortcutsIncrement(void)
{
  s_blockShortcuts.fetchAndAddOrdered(1);
}
