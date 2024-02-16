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

#ifndef _glitch_ash_state_machine_h_
#define _glitch_ash_state_machine_h_

#include <QUndoStack>

#include "glitch-object.h"

class glitch_ash_state_machine
{
 public:
  template<class T>
  static void processCommand(const QString &command, T *t)
  {
    if(!t)
      return;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QListIterator<QString> it(command.split(' ', Qt::SkipEmptyParts));
#else
    QListIterator<QString> it(command.split(' ', QString::SkipEmptyParts));
#endif
    States state = States::ZZZ;
    auto undoStack = t->template undoStack();

    while(it.hasNext())
      {
	auto token(it.next());

	/*
	** Here be single-state states.
	*/

	if(token.startsWith(QObject::tr("full-screen"), Qt::CaseInsensitive))
	  state = States::FullScreen;
	else if(token.startsWith(QObject::tr("normal-screen"),
				 Qt::CaseInsensitive))
	  state = States::NormalScreen;
	else if(token.startsWith(QObject::tr("redo"), Qt::CaseInsensitive))
	  state = States::Redo;
	else if(token.startsWith(QObject::tr("save"), Qt::CaseInsensitive))
	  state = States::Save;
	else if(token.startsWith(QObject::tr("undo"), Qt::CaseInsensitive))
	  state = States::Undo;

	if(state != States::ZZZ)
	  goto state_label;

	/*
	** Here be multiple-state states.
	*/

	if(token.startsWith(QObject::tr("delete"), Qt::CaseInsensitive))
	  state = States::Delete;
	else if(token.startsWith(QObject::tr("deselect"), Qt::CaseInsensitive))
	  state = States::Deselect;
	else if(token.startsWith(QObject::tr("display"),
				 Qt::CaseInsensitive) ||
		token.startsWith(QObject::tr("show"), Qt::CaseInsensitive))
	  {
	    state = States::Display;
	    continue;
	  }
	else if(token.startsWith(QObject::tr("generate"), Qt::CaseInsensitive))
	  {
	    state = States::Generate;
	    continue;
	  }
	else if(token.startsWith(QObject::tr("list"), Qt::CaseInsensitive))
	  {
	    state = States::List;
	    continue;
	  }
	else if(token.startsWith(QObject::tr("select"), Qt::CaseInsensitive))
	  state = States::Select;
	else if(token.startsWith(QObject::tr("set"), Qt::CaseInsensitive))
	  {
	    state = States::Set;
	    continue;
	  }

      state_label:

	if(state == States::Set)
	  {
	    if(token.startsWith(QObject::tr("widget-position"),
				Qt::CaseInsensitive))
	      state = States::SetWidgetPosition;
	    else if(token.startsWith(QObject::tr("widget-size"),
				     Qt::CaseInsensitive))
	      state = States::SetWidgetSize;
	    else
	      {
		state = States::ZZZ;
		continue;
	      }
	  }

	switch(state)
	  {
	  case States::Delete:
	    {
	      while(it.hasNext())
		{
		  auto token(it.next());
		  auto object = t->template find(qAbs(token.toLongLong()));

		  if(object &&
		     object->proxy() &&
		     object->proxy()->isMandatory() == false)
		    {
		    }
		}

	      state = States::ZZZ;
	      break;
	    }
	  case States::Deselect:
	  case States::Select:
	    {
	      while(it.hasNext())
		{
		  auto token(it.next());

		  if(token == QObject::tr("all"))
		    {
		      if(state == States::Deselect)
			t->template clearSelection();
		      else
			emit t->template selectAll();

		      break;
		    }
		  else
		    {
		      auto object = t->template find(qAbs(token.toLongLong()));

		      if(object && object->proxy())
			object->proxy()->setSelected
			  (state != States::Deselect);
		    }
		}

	      state = States::ZZZ;
	      break;
	    }
	  case States::Display:
	    {
	      if(token == QObject::tr("canvas-settings"))
		t->template showCanvasSettings();

	      state = States::ZZZ;
	      break;
	    }
	  case States::FullScreen:
	    {
	      t->template showFullScreen();
	      state = States::ZZZ;
	      break;
	    }
	  case States::Generate:
	    {
	      if(token == QObject::tr("clipboard"))
		t->template generateSourceClipboard();
	      else if(token == QObject::tr("file"))
		t->template generateSourceFile();
	      else if(token == QObject::tr("view"))
		t->template generateSourceView();

	      state = States::ZZZ;
	      break;
	    }
	  case States::List:
	    {
	      QString string("");

	      if(token == QObject::tr("all"))
		{
		  foreach(auto object, t->template allObjects())
		    string.append(object->about()).append(" ");
		}
	      else if(token == QObject::tr("local"))
		{
		  foreach(auto object, t->template objects())
		    string.append(object->about()).append(" ");
		}

	      if(!string.isEmpty())
		emit t->template information(string);

	      state = States::ZZZ;
	      break;
	    }
	  case States::NormalScreen:
	    {
	      t->template showNormal();
	      state = States::ZZZ;
	      break;
	    }
	  case States::Redo:
	    {
	      if(undoStack)
		undoStack->redo();

	      state = States::ZZZ;
	      break;
	    }
	  case States::Save:
	    {
	      emit t->template saveSignal();
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
		      auto object = t->template find(id);

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
				  (glitch_object::Properties::SIZE,
				   QSize(w, h));
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
	      if(undoStack)
		undoStack->undo();

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

 private:
  enum class States
  {
    Delete,
    Deselect,
    Display,
    FullScreen,
    Generate,
    List,
    NormalScreen,
    Redo,
    Save,
    Select,
    Set,
    SetWidgetPosition,
    SetWidgetSize,
    Undo,
    ZZZ
  };

  glitch_ash_state_machine(void)
  {
  }

  ~glitch_ash_state_machine()
  {
  }
};

#endif
