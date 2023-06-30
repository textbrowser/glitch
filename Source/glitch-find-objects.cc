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

#include <QCoreApplication>
#ifndef Q_OS_ANDROID
#include <QShortcut>
#endif

#include "glitch-collapse-expand-tool-button.h"
#include "glitch-find-objects.h"
#include "glitch-find-objects-position-item.h"
#include "glitch-floating-context-menu.h"
#include "glitch-object.h"
#include "glitch-view.h"

glitch_find_objects::glitch_find_objects(QWidget *parent):QMainWindow(nullptr)
{
  m_ui.setupUi(this);
  connect(&m_searchTimer,
	  &QTimer::timeout,
	  this,
	  &glitch_find_objects::slotSearch);
#ifdef Q_OS_ANDROID
  connect(m_ui.close,
	  &QPushButton::clicked,
	  this,
	  &glitch_find_objects::hide);
#else
  connect(m_ui.close,
	  &QPushButton::clicked,
	  this,
	  &glitch_find_objects::close);
#endif
  connect(m_ui.find,
	  &QPushButton::clicked,
	  this,
	  &glitch_find_objects::slotFind);
  connect(m_ui.search,
	  SIGNAL(textEdited(const QString &)),
	  &m_searchTimer,
	  SLOT(start(void)));
  connect(m_ui.synchronize,
	  &QCheckBox::clicked,
	  this,
	  &glitch_find_objects::slotSynchronize);
  connect(m_ui.tree,
	  SIGNAL(customContextMenuRequested(const QPoint &)),
	  this,
	  SLOT(slotCustomContextMenuRequested(const QPoint &)));
  connect(m_ui.tree,
	  SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
	  this,
	  SLOT(slotItemDoubleClicked(QTreeWidgetItem *, int)));
  connect(parent,
	  SIGNAL(destroyed(void)),
	  this,
	  SLOT(deleteLater(void)));
  m_collapse = new glitch_collapse_expand_tool_button(m_ui.tree);
  m_searchTimer.setInterval(10);
  m_searchTimer.setSingleShot(true);
  m_ui.close->setIcon(QIcon(":/close.png"));
  m_ui.find->setIcon(QIcon(":/find.png"));
  m_ui.tree->setContextMenuPolicy(Qt::CustomContextMenu);
  m_ui.tree->sortItems(0, Qt::AscendingOrder);
  m_view = qobject_cast<glitch_view *> (parent);
#ifndef Q_OS_ANDROID
  new QShortcut(tr("Ctrl+F"),
		m_ui.search,
		SLOT(setFocus(void)));
  new QShortcut(tr("Ctrl+W"),
		this,
		SLOT(close(void)));
#endif
  setWindowFlags(Qt::Dialog | windowFlags());
  QTimer::singleShot(100, this, SLOT(slotFind(void)));
}

glitch_find_objects::~glitch_find_objects()
{
}

void glitch_find_objects::find
(QTreeWidgetItem *i, const QSet<qint64> &ids, glitch_object *object)
{
  if(!i || !object)
    return;

  foreach(auto child, object->objects())
    if(child)
      {
	auto item = new glitch_find_objects_position_item(i);

	connect(this,
		SIGNAL(setTotals(const QHash<QString, int> &)),
		item,
		SLOT(slotSetTotals(const QHash<QString, int> &)));
	item->setObject(child);
	item->setSelected(ids.contains(child->id()));
	item->setText(static_cast<int> (Columns::Object), child->name());
	item->setText(static_cast<int> (Columns::Position), child->position());
	item->setText(static_cast<int> (Columns::Type), child->objectType());
	m_items << item;
	m_typeTotals[child->objectType()] = m_typeTotals.value
	  (child->objectType(), 0) + 1;
	find(item, ids, child);
      }
}

void glitch_find_objects::find(const QSet<qint64> &ids)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  m_items.clear();
  m_typeTotals.clear();
  m_ui.tree->clear();

  if(m_view)
    {
      foreach(auto object, m_view->objects())
	if(object)
	  {
	    auto item = new glitch_find_objects_position_item(m_ui.tree);

	    connect(this,
		    SIGNAL(setTotals(const QHash<QString, int> &)),
		    item,
		    SLOT(slotSetTotals(const QHash<QString, int> &)));
	    item->setObject(object);
	    item->setSelected(ids.contains(object->id()));
	    item->setText(static_cast<int> (Columns::Object), object->name());
	    item->setText
	      (static_cast<int> (Columns::Position), object->position());
	    item->setText
	      (static_cast<int> (Columns::Type), object->objectType());
	    m_items << item;
	    m_typeTotals[object->objectType()] =
	      m_typeTotals.value(object->objectType(), 0) + 1;
	    m_ui.tree->addTopLevelItem(item);
	    find(item, ids, object);
	  }

      if(!m_typeTotals.isEmpty())
	emit setTotals(m_typeTotals);
    }

  if(m_collapse->isChecked())
    m_ui.tree->expandAll();
  else
    m_ui.tree->collapseAll();

  m_ui.tree->resizeColumnToContents(0);
  slotSearch();
  QApplication::restoreOverrideCursor();
}

void glitch_find_objects::slotCustomContextMenuRequested(const QPoint &point)
{
  auto item = static_cast<glitch_find_objects_position_item *>
    (m_ui.tree->itemAt(point));

  if(item && item->object())
    {
      QMenu menu;

      item->object()->addActions(menu);

      if(!menu.actions().isEmpty())
	menu.exec(mapToGlobal(point));
    }
}

void glitch_find_objects::slotFind(void)
{
  find(QSet<qint64> ());
}

void glitch_find_objects::slotItemDoubleClicked(QTreeWidgetItem *i, int column)
{
  Q_UNUSED(column);

  if(!i)
    return;

  /*
  ** Select the clicked item.
  */

  auto item = static_cast<glitch_find_objects_position_item *> (i);

  if(item && item->object() && item->object()->proxy())
    {
      auto proxy = item->object()->proxy();

      proxy->ensureVisible();
      proxy->setSelected(true);

      if(static_cast<QTreeWidgetItem *> (item)->parent())
	{
	  item = static_cast<glitch_find_objects_position_item *>
	    (static_cast<QTreeWidgetItem *> (item)->parent());

	  if(item && item->object())
	    item->object()->showEditWindow();
	}
    }
}

void glitch_find_objects::slotSearch(void)
{
  auto count = 0;
  auto text(m_ui.search->text().trimmed());

  if(text.isEmpty())
    {
      /*
      ** Show all items.
      */

      foreach(auto item, m_items)
	if(item)
	  {
	    count += 1;
	    item->setHidden(false);
	  }
    }
  else
    {
      int i = 0;

      foreach(auto item, m_items)
	if(item)
	  {
	    auto found = false;

	    for(i = 0; i < item->columnCount(); i++)
	      if(item->text(i).contains(text, Qt::CaseInsensitive))
		{
		  count += 1;
		  found = true;
		  break;
		}

	    if(found)
	      {
		item->setHidden(false);

		auto parent = static_cast<QTreeWidgetItem *> (item)->parent();

		while(parent)
		  {
		    parent->setExpanded(true);
		    parent->setHidden(false);
		    parent = parent->parent();
		  }
	      }
	    else
	      item->setHidden(true);
	  }
    }

  statusBar()->showMessage(tr("%1 object(s).").arg(count));
}

void glitch_find_objects::slotSynchronize(void)
{
  if(!m_ui.synchronize->isChecked())
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QSet<qint64> ids;
  auto list(m_ui.tree->selectedItems());
  auto value1 = m_ui.tree->horizontalScrollBar() ?
    m_ui.tree->horizontalScrollBar()->value() : -1;
  auto value2 = m_ui.tree->verticalScrollBar() ?
    m_ui.tree->verticalScrollBar()->value() : -1;

  for(int i = 0; i < list.size(); i++)
    {
      auto item = static_cast<glitch_find_objects_position_item *>
	(list.at(i));

      if(item && item->object())
	ids << item->object()->id();
    }

  QApplication::restoreOverrideCursor();

  find(ids);

  if(value1 >= 0)
    m_ui.tree->horizontalScrollBar()->setValue(value1);

  if(value2 >= 0)
    m_ui.tree->verticalScrollBar()->setValue(value2);
}
