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

#include "glitch-find-objects.h"
#include "glitch-object.h"
#include "glitch-view.h"

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

void glitch_find_objects::find(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  m_ui.tree->clear();

  if(m_view)
    foreach(auto object, m_view->objects())
      if(object)
	{
	  auto item = new QTreeWidgetItem(m_ui.tree);

	  item->setText(0, object->name());
	  item->setText(1, object->type());
	  m_ui.tree->addTopLevelItem(item);
	}

  m_ui.tree->resizeColumnToContents(0);
  QApplication::restoreOverrideCursor();
}

void glitch_find_objects::slotFind(void)
{
  find();
}
