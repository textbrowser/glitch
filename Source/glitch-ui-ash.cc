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

#include "glitch-ui.h"
#include "glitch-view.h"

enum class States
{
  About,
  Display,
  FullScreen,
  NormalScreen,
  Save,
  ZZZ
};

void glitch_ui::slotProcessCommand(const QString &command)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QListIterator<QString> it(command.split(' ', Qt::SkipEmptyParts));
#else
  QListIterator<QString> it(command.split(' ', QString::SkipEmptyParts));
#endif
  States state = States::ZZZ;

  while(it.hasNext())
    {
      auto const token(it.next());

      /*
      ** Here be single-state states.
      */

      if(token.startsWith(tr("about"), Qt::CaseInsensitive))
	state = States::About;
      else if(token.startsWith(tr("full-screen"), Qt::CaseInsensitive))
	state = States::FullScreen;
      else if(token.startsWith(tr("normal-screen"), Qt::CaseInsensitive))
	state = States::NormalScreen;
      else if(token.startsWith(tr("save"), Qt::CaseInsensitive))
	state = States::Save;

      if(state != States::ZZZ)
	goto state_label;

      /*
      ** Here be multiple-state states.
      */

      if(token.startsWith(tr("display"), Qt::CaseInsensitive) ||
	 token.startsWith(tr("show"), Qt::CaseInsensitive))
	{
	  state = States::Display;
	  continue;
	}

    state_label:

      switch(state)
	{
	case States::About:
	  {
	    emit information(about());
	    state = States::ZZZ;
	    break;
	  }
	case States::Display:
	  {
	    if(token == tr("settings"))
	      slotShowPreferences();

	    state = States::ZZZ;
	    break;
	  }
	case States::FullScreen:
	  {
	    if(!isFullScreen())
	      slotShowFullScreenMode();

	    state = States::ZZZ;
	    break;
	  }
	case States::NormalScreen:
	  {
	    if(isFullScreen())
	      slotShowFullScreenMode();

	    state = States::ZZZ;
	    break;
	  }
	case States::Save:
	  {
	    slotSaveCurrentDiagram();
	    state = States::ZZZ;
	    break;
	  }
	default:
	  {
	    break;
	  }
	}
    }
}
