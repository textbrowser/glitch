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

#include <QShortcut>

#include "glitch-collapse-expand-tool-button.h"
#include "glitch-find-objects.h"
#include "glitch-object.h"
#include "glitch-view.h"

class glitch_find_objects_position_item: public QTreeWidgetItem
{
 public:
  glitch_find_objects_position_item(QTreeWidget *parent):QTreeWidgetItem(parent)
  {
  }

  glitch_find_objects_position_item(QTreeWidgetItem *parent):
    QTreeWidgetItem(parent)
  {
  }

  bool operator<(const QTreeWidgetItem &other) const
  {
    auto i = treeWidget()->sortColumn();

    if(i == static_cast<int> (glitch_find_objects::Columns::Object) ||
       i == static_cast<int> (glitch_find_objects::Columns::Type))
      return other.text(i) > text(i);
    else
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
};

glitch_find_objects::glitch_find_objects(QWidget *parent):QDialog(parent)
{
  m_ui.setupUi(this);
  connect(m_ui.close,
	  &QPushButton::clicked,
	  this,
	  &glitch_find_objects::close);
  connect(m_ui.find,
	  &QPushButton::clicked,
	  this,
	  &glitch_find_objects::slotFind);
  m_collapse = new glitch_collapse_expand_tool_button(m_ui.tree);
  m_ui.close->setIcon(QIcon::fromTheme("window-close"));
  m_ui.find->setIcon(QIcon::fromTheme("edit-find"));
  m_ui.tree->sortItems(0, Qt::AscendingOrder);
  m_view = qobject_cast<glitch_view *> (parent);
  new QShortcut(tr("Ctrl+W"),
		this,
		SLOT(close(void)));
  setWindowFlags(Qt::WindowStaysOnTopHint | windowFlags());
  setWindowModality(Qt::NonModal);
}

glitch_find_objects::~glitch_find_objects()
{
}

void glitch_find_objects::find(QTreeWidgetItem *i, glitch_object *object)
{
  if(!i || !object)
    return;

  foreach(auto child, object->objects())
    if(child)
      {
	auto item = new glitch_find_objects_position_item(i);

	item->setText(static_cast<int> (Columns::Object), child->name());
	item->setText(static_cast<int> (Columns::Position), child->position());
	item->setText(static_cast<int> (Columns::Type), child->type());
	find(item, child);
      }
}

void glitch_find_objects::find(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  m_ui.tree->clear();

  if(m_view)
    foreach(auto object, m_view->objects())
      if(object)
	{
	  auto item = new glitch_find_objects_position_item(m_ui.tree);

	  item->setText(static_cast<int> (Columns::Object), object->name());
	  item->setText
	    (static_cast<int> (Columns::Position), object->position());
	  item->setText(static_cast<int> (Columns::Type), object->type());
	  m_ui.tree->addTopLevelItem(item);
	  find(item, object);
	}

  if(m_collapse->isChecked())
    m_ui.tree->expandAll();
  else
    m_ui.tree->collapseAll();

  m_ui.tree->resizeColumnToContents(0);
  QApplication::restoreOverrideCursor();
}

void glitch_find_objects::slotFind(void)
{
  find();
}
