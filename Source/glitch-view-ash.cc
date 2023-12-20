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

#include "glitch-view.h"

enum class States
{
  SetWidgetGeometry,
  ZZZ
};

void glitch_view::slotProcessCommand(const QString &command)
{
  QListIterator<QString> it(command.split(' '));
  States state = States::ZZZ;

  while(it.hasNext())
    {
      auto token(it.next());

      /*
      ** Here be single-state states.
      */

      if(state != States::ZZZ)
	goto state_label;

      /*
      ** Here be multiple-state states.
      */

      if(token.startsWith(tr("set")))
	state = States::SetWidgetGeometry;

    state_label:

      switch(state)
	{
	case States::SetWidgetGeometry:
	  {
	    QStringList geometry;
	    qint64 id = -1;

	    while(it.hasNext())
	      {
		if(id == -1)
		  id = qAbs(it.next().toLongLong());
		else
		  {
		    geometry = it.next().split(',');
		    break;
		  }
	      }

	    if(geometry.size() == 2)
	      {
		// Locate the object having the identifier id.
	      }

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
