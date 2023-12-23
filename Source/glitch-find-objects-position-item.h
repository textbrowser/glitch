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

#ifndef _glitch_find_objects_position_item_h_
#define _glitch_find_objects_position_item_h_

#include <QObject>
#include <QPointer>
#include <QTreeWidgetItem>

#include "glitch-find-objects.h"
#include "glitch-object.h"

class glitch_find_objects_position_item: public QObject, public QTreeWidgetItem
{
  Q_OBJECT

 public:
  glitch_find_objects_position_item(QTreeWidget *parent):
    QObject(parent), QTreeWidgetItem(parent)
  {
  }

  glitch_find_objects_position_item(QTreeWidgetItem *parent):
    QObject(), QTreeWidgetItem(parent)
  {
  }

  QPointer<glitch_object> object(void) const
  {
    return m_object;
  }

  bool operator<(const QTreeWidgetItem &other) const
  {
    auto i = treeWidget()->sortColumn();

    switch(static_cast<glitch_find_objects::Columns> (i))
      {
      case glitch_find_objects::Columns::Identifier:
	{
	  return other.text(i).toLongLong() > text(i).toLongLong();
	}
      case glitch_find_objects::Columns::Object:
      case glitch_find_objects::Columns::Type:
	{
	  return other.text(i) > text(i);
	}
      case glitch_find_objects::Columns::TypeTotal:
	{
	  return other.text(i).toInt() > text(i).toInt();
	}
      default:
	{
	  auto list1
	    (other.text(i).remove(' ').remove('(').remove(')').split(','));
	  auto list2(text(i).remove(' ').remove('(').remove(')').split(','));
	  auto x1 = list1.value(0).toInt();
	  auto x2 = list2.value(0).toInt();
	  auto y1 = list1.value(1).toInt();
	  auto y2 = list2.value(1).toInt();

	  return !(x1 < x2 || (x1 == x2 && y1 < y2));
	}
      }
  }

  void setObject(glitch_object *object)
  {
    m_object = object;
  }

 private:
  QPointer<glitch_object> m_object;

 private slots:
  void slotSetTotals(const QHash<QString, int> &totals)
  {
    auto i = static_cast<int> (glitch_find_objects::Columns::Type);
    auto j = static_cast<int> (glitch_find_objects::Columns::TypeTotal);

    setText(j, QString::number(totals.value(text(i), 0)));
  }
};

#endif
