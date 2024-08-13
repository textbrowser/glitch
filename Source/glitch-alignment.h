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

#ifndef _glitch_alignment_h_
#define _glitch_alignment_h_

#include "glitch-object-view.h"
#include "glitch-object.h"
#include "glitch-proxy-widget.h"
#include "glitch-scene.h"
#include "glitch-undo-command.h"
#include "glitch-view.h"

#include <limits>

static bool x_coordinate_less_than(glitch_object *w1, glitch_object *w2)
{
  if(!w1 || !w2)
    return false;
  else
    return w1->pos().x() < w2->pos().x();
}

static bool y_coordinate_less_than(glitch_object *w1, glitch_object *w2)
{
  if(!w1 || !w2)
    return false;
  else
    return w1->pos().y() < w2->pos().y();
}

class glitch_alignment: public QWidget
{
  Q_OBJECT

 public:
  glitch_alignment(QWidget *parent);
  ~glitch_alignment();
  QList<QAction *> actions(void);

 private:
  enum class AlignmentTypes
  {
    ALIGN_BOTTOM = 0,
    ALIGN_CENTER_HORIZONTAL,
    ALIGN_CENTER_VERTICAL,
    ALIGN_LEFT,
    ALIGN_RIGHT,
    ALIGN_TOP
  };

  enum class StackTypes
  {
    HORIZONTAL_STACK = 0,
    VERTICAL_STACK
  };

  QList<QAction *> m_actions;

  template<class T> void alignImplementation(const AlignmentTypes alignmentType)
  {
    auto view = qobject_cast<T *> (parentWidget());

    if(!view || !view->scene())
      return;

    auto const list(view->scene()->items(Qt::AscendingOrder));

    if(list.isEmpty())
      return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QPair<int, int> maxP;
    QPair<int, int> minP;
    auto x = 0;
    auto y = 0;

    switch(alignmentType)
      {
      case AlignmentTypes::ALIGN_BOTTOM:
	{
	  y = 0;
	  break;
	}
      case AlignmentTypes::ALIGN_CENTER_HORIZONTAL:
      case AlignmentTypes::ALIGN_CENTER_VERTICAL:
	{
	  maxP.first = maxP.second = 0;
	  minP.first = minP.second = std::numeric_limits<int>::max();
	  break;
	}
      case AlignmentTypes::ALIGN_LEFT:
	{
	  x = std::numeric_limits<int>::max();
	  break;
	}
      case AlignmentTypes::ALIGN_RIGHT:
	{
	  x = 0;
	  break;
	}
      case AlignmentTypes::ALIGN_TOP:
	{
	  y = std::numeric_limits<int>::max();
	  break;
	}
      default:
	{
	  QApplication::restoreOverrideCursor();
	  return;
	}
      }

    auto began = false;
    auto firstIteration = true;

  start_label:

    foreach(auto const i, list)
      {
	auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

	if(!proxy || !proxy->isSelected())
	  continue;

	auto object = qobject_cast<glitch_object *> (proxy->widget());

	if(!object)
	  continue;

	auto const movable = proxy->isMovable();

	switch(alignmentType)
	  {
	  case AlignmentTypes::ALIGN_BOTTOM:
	    {
	      x = object->pos().x();
	      y = qMax(object->height() + object->pos().y(), y);
	      break;
	    }
	  case AlignmentTypes::ALIGN_CENTER_HORIZONTAL:
	  case AlignmentTypes::ALIGN_CENTER_VERTICAL:
	    {
	      maxP.first = qMax
		(maxP.first, object->pos().x() + object->width());
	      maxP.second = qMax
		(maxP.second, object->height() + object->pos().y());
	      minP.first = qMin(minP.first, object->pos().x());
	      minP.second = qMin(minP.second, object->pos().y());
	      break;
	    }
	  case AlignmentTypes::ALIGN_LEFT:
	    {
	      x = qMin(object->pos().x(), x);
	      y = object->pos().y();
	      break;
	    }
	  case AlignmentTypes::ALIGN_RIGHT:
	    {
	      x = qMax(object->pos().x() + object->width(), x);
	      y = object->pos().y();
	      break;
	    }
	  case AlignmentTypes::ALIGN_TOP:
	    {
	      x = object->pos().x();
	      y = qMin(object->pos().y(), y);
	      break;
	    }
	  default:
	    break;
	  }

	if(firstIteration || !movable)
	  continue;

	QPoint point;

	switch(alignmentType)
	  {
	  case AlignmentTypes::ALIGN_BOTTOM:
	    {
	      if(object->height() + object->pos().y() != y)
		{
		  point = object->pos();
		  object->move(x, y - object->height());
		}

	      break;
	    }
	  case AlignmentTypes::ALIGN_CENTER_HORIZONTAL:
	  case AlignmentTypes::ALIGN_CENTER_VERTICAL:
	    {
	      QRect rect(QPoint(minP.first, minP.second),
			 QPoint(maxP.first, maxP.second));

	      point = object->pos();

	      if(alignmentType == AlignmentTypes::ALIGN_CENTER_HORIZONTAL)
		object->move
		  (object->pos().x(), rect.center().y() - object->height() / 2);
	      else
		object->move
		  (rect.center().x() - object->width() / 2, object->pos().y());

	      break;
	    }
	  case AlignmentTypes::ALIGN_RIGHT:
	    {
	      if(object->pos().x() + object->width() != x)
		{
		  point = object->pos();
		  object->move(x - object->width(), y);
		}

	      break;
	    }
	  default:
	    {
	      point = object->pos();
	      object->move(x, y);
	      break;
	    }
	  }

	if(!point.isNull())
	  if(object->pos() != point)
	    {
	      if(!began)
		{
		  began = true;
		  view->beginMacro(tr("items aligned"));
		}

	      auto undoCommand = new glitch_undo_command
		(QPointF(point),
		 glitch_undo_command::Types::ITEM_MOVED,
		 proxy,
		 view->scene());

	      view->push(undoCommand);
	    }
      }

    if(firstIteration)
      {
	firstIteration = false;
	goto start_label;
      }

    if(began)
      {
	view->endMacro();
	emit changed();
      }

    QApplication::restoreOverrideCursor();
  }

  template<class T> void stackImplementation(const StackTypes stackType)
  {
    auto view = qobject_cast<T *> (parentWidget());

    if(!view || !view->scene())
      return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    auto const list1(view->scene()->selectedItems());

    if(list1.isEmpty())
      {
	QApplication::restoreOverrideCursor();
	return;
      }

    QList<glitch_object *> list2;

    foreach(auto const i, list1)
      {
	auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

	if(!proxy)
	  continue;

	auto object = qobject_cast<glitch_object *> (proxy->widget());

	if(!object)
	  continue;

	list2 << object;
      }

    if(list2.isEmpty())
      {
	QApplication::restoreOverrideCursor();
	return;
      }

    if(stackType == StackTypes::HORIZONTAL_STACK)
      std::sort(list2.begin(), list2.end(), x_coordinate_less_than);
    else
      std::sort(list2.begin(), list2.end(), y_coordinate_less_than);

    auto began = false;
    auto coordinate = 0;

    if(stackType == StackTypes::HORIZONTAL_STACK)
      coordinate = list2.at(0)->pos().x();
    else
      coordinate = list2.at(0)->pos().y();

    foreach(auto const widget, list2)
      {
	if(!widget || !widget->proxy())
	  continue;

	QPoint point;

	if(stackType == StackTypes::HORIZONTAL_STACK)
	  {
	    if(widget->proxy()->isMovable())
	      {
		point = widget->pos();
		widget->move(coordinate, widget->pos().y());
	      }

	    coordinate += widget->width();
	  }
	else
	  {
	    if(widget->proxy()->isMovable())
	      {
		point = widget->pos();
		widget->move(widget->pos().x(), coordinate);
	      }

	    coordinate += widget->height();
	  }

	if(!point.isNull())
	  if(point != widget->pos())
	    {
	      if(!began)
		{
		  began = true;
		  view->beginMacro(tr("items stacked"));
		}

	      auto undoCommand = new glitch_undo_command
		(QPointF(point),
		 glitch_undo_command::Types::ITEM_MOVED,
		 widget->proxy(),
		 view->scene());

	      view->push(undoCommand);
	    }
      }

    if(began)
      {
	view->endMacro();
	emit changed();
      }

    QApplication::restoreOverrideCursor();
  }

  void align(const AlignmentTypes alignmentType)
  {
    if(qobject_cast<glitch_object_view *> (parentWidget()))
      alignImplementation<glitch_object_view> (alignmentType);
    else if(qobject_cast<glitch_view *> (parentWidget()))
      alignImplementation<glitch_view> (alignmentType);
  }

  void stack(const StackTypes stackType)
  {
    if(qobject_cast<glitch_object_view *> (parentWidget()))
      stackImplementation<glitch_object_view> (stackType);
    else if(qobject_cast<glitch_view *> (parentWidget()))
      stackImplementation<glitch_view> (stackType);
  }

 private slots:
  void slotAlign(void);
  void slotStack(void);

 signals:
  void changed(void);
};

#endif
