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

#include "glitch-graphicsview.h"
#include "glitch-object.h"
#include "glitch-view.h"

enum class States
{
  Deselect,
  Display,
  Generate,
  List,
  Redo,
  Select,
  Set,
  SetWidgetPosition,
  SetWidgetSize,
  Undo,
  ZZZ
};

void glitch_view::slotProcessCommand(const QString &command)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QListIterator<QString> it(command.split(' ', Qt::SkipEmptyParts));
#else
  QListIterator<QString> it(command.split(' ', QString::SkipEmptyParts));
#endif
  States state = States::ZZZ;

  while(it.hasNext())
    {
      auto token(it.next());

      /*
      ** Here be single-state states.
      */

      if(token.startsWith(tr("redo"), Qt::CaseInsensitive))
	state = States::Redo;
      else if(token.startsWith(tr("undo"), Qt::CaseInsensitive))
	state = States::Undo;

      if(state != States::ZZZ)
	goto state_label;

      /*
      ** Here be multiple-state states.
      */

      if(token.startsWith(tr("deselect"), Qt::CaseInsensitive))
	state = States::Deselect;
      else if(token.startsWith(tr("display"), Qt::CaseInsensitive) ||
	      token.startsWith(tr("show"), Qt::CaseInsensitive))
	{
	  state = States::Display;
	  continue;
	}
      else if(token.startsWith(tr("generate"), Qt::CaseInsensitive))
	{
	  state = States::Generate;
	  continue;
	}
      else if(token.startsWith(tr("list"), Qt::CaseInsensitive))
	{
	  state = States::List;
	  continue;
	}
      else if(token.startsWith(tr("select"), Qt::CaseInsensitive))
	state = States::Select;
      else if(token.startsWith(tr("set"), Qt::CaseInsensitive))
	{
	  state = States::Set;
	  continue;
	}

    state_label:

      if(state == States::Set)
	{
	  if(token.startsWith(tr("widget-position"), Qt::CaseInsensitive))
	    state = States::SetWidgetPosition;
	  else if(token.startsWith(tr("widget-size"), Qt::CaseInsensitive))
	    state = States::SetWidgetSize;
	  else
	    {
	      state = States::ZZZ;
	      continue;
	    }
	}

      switch(state)
	{
	case States::Deselect:
	case States::Select:
	  {
	    while(it.hasNext())
	      {
		auto token(it.next());

		if(token == tr("all"))
		  {
		    if(state == States::Deselect)
		      clearSelection();
		    else
		      selectAll();

		    break;
		  }
		else
		  {
		    auto object = find(qAbs(token.toLongLong()));

		    if(object && object->proxy())
		      object->proxy()->setSelected(state != States::Deselect);
		  }
	      }

	    state = States::ZZZ;
	    break;
	  }
	case States::Display:
	  {
	    if(token == tr("canvas-settings"))
	      showCanvasSettings();

	    state = States::ZZZ;
	    break;
	  }
	case States::Generate:
	  {
	    if(token == tr("clipboard"))
	      generateSourceClipboard();
	    else if(token == tr("file"))
	      generateSourceFile();
	    else if(token == tr("view"))
	      generateSourceView();

	    state = States::ZZZ;
	    break;
	  }
	case States::List:
	  {
	    QString string("");

	    if(token == tr("all"))
	      {
		foreach(auto object, allObjects())
		  string.append(object->about()).append(" ");
	      }
	    else if(token == tr("local"))
	      {
		foreach(auto object, objects())
		  string.append(object->about()).append(" ");
	      }

	    if(!string.isEmpty())
	      emit information(string);

	    state = States::ZZZ;
	    break;
	  }
	case States::Redo:
	  {
	    if(m_undoStack)
	      m_undoStack->redo();

	    state = States::ZZZ;
	    break;
	  }
	case States::SetWidgetPosition:
	case States::SetWidgetSize:
	  {
	    QStringList list;
	    qint64 id = -1;

	    while(it.hasNext())
	      {
		auto token(it.next());

		if(id == -1)
		  {
		    id = qAbs(token.toLongLong());
		    continue;
		  }
		else
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
		  list = token.split(',', Qt::SkipEmptyParts);
#else
		  list = token.split(',', QString::SkipEmptyParts);
#endif

		if(list.size() == 2)
		  {
		    auto object = find(id);

		    if(object && object->proxy())
		      {
			if(state == States::SetWidgetPosition)
			  {
			    auto ok1 = true;
			    auto ok2 = true;
			    auto size(object->proxy()->size());
			    auto x = list.at(0).trimmed().toInt(&ok1);
			    auto y = list.at(1).trimmed().toInt(&ok2);

			    if(ok1 && ok2 && x >= 0 && y >= 0)
			      object->setPropertyWithUndo
				(glitch_object::Properties::GEOMETRY,
				 QRectF(x, y, size.width(), size.height()));
			  }
			else
			  {
			    auto b1 = true;
			    auto b2 = true;
			    auto h = list.at(1).trimmed().toInt(&b1);
			    auto w = list.at(0).trimmed().toInt(&b2);

			    if(b1 && b2 && h > 0 && w > 0)
			      object->setPropertyWithUndo
				(glitch_object::Properties::SIZE, QSize(w, h));
			  }
		      }

		    // Reset.

		    id = -1;
		    list.clear();
		  }
		else
		  break;
	      }

	    state = States::ZZZ;
	    break;
	  }
	case States::Undo:
	  {
	    if(m_undoStack)
	      m_undoStack->undo();

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
