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

#include "glitch-redo-undo-stack.h"

glitch_redo_undo_stack::glitch_redo_undo_stack(QWidget *parent):QDialog(parent)
{
  m_ui.setupUi(this);
#ifdef Q_OS_ANDROID
  connect(m_ui.close,
	  &QPushButton::clicked,
	  this,
	  &glitch_redo_undo_stack::hide);
#else
  connect(m_ui.close,
	  &QPushButton::clicked,
	  this,
	  &glitch_redo_undo_stack::close);
#endif
  connect(m_ui.table,
	  SIGNAL(doubleClicked(const QModelIndex &)),
	  this,
	  SLOT(slotDoubleClicked(const QModelIndex &)));
  m_ui.close->setIcon(QIcon(":/close.png"));
  m_ui.table->setIconSize(QSize(16, 16));
  setWindowModality(Qt::NonModal);
}

glitch_redo_undo_stack::~glitch_redo_undo_stack()
{
}

void glitch_redo_undo_stack::setUndoStack(QUndoStack *undoStack)
{
  m_ui.table->setRowCount(0);
  m_undoStack = undoStack;

  if(!m_undoStack)
    return;

  QPixmap pixmap(16, 16);

  pixmap.fill(Qt::white);

  for(int i = 0; i < m_undoStack->count(); i++)
    {
      auto command = m_undoStack->command(i);

      if(!command)
	continue;

      auto item = new QTableWidgetItem
	(QString("%1%2").
	 arg(command->text()).
	 arg(i == m_undoStack->index() - 1? " (Current)" : ""));

      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      item->setIcon
	(i == m_undoStack->index() - 1? QIcon(":/next.png") : QIcon(pixmap));
      item->setSizeHint(QSize(16, 16));
      m_ui.table->setRowCount(m_ui.table->rowCount() + 1);
      m_ui.table->setItem(m_ui.table->rowCount() - 1, 0, item);
    }
}

void glitch_redo_undo_stack::slotDoubleClicked(const QModelIndex &index)
{
  Q_UNUSED(index);
}
