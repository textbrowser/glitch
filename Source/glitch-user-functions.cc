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

#include "glitch-user-functions.h"

glitch_user_functions::glitch_user_functions(QWidget *parent):QDialog(parent)
{
  m_ui.setupUi(this);
#ifdef Q_OS_ANDROID
  connect(m_ui.close,
	  &QPushButton::clicked,
	  this,
	  &glitch_user_functions::hide);
#else
  connect(m_ui.close,
	  &QPushButton::clicked,
	  this,
	  &glitch_user_functions::close);
#endif
  connect(m_ui.functions,
	  SIGNAL(doubleClicked(const QModelIndex &)),
	  this,
	  SLOT(slotDoubleClicked(const QModelIndex &)));
  m_ui.close->setIcon(QIcon(":/close.png"));
  m_ui.functions->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  m_ui.functions->horizontalHeader()->setSortIndicatorShown(true);
  setWindowModality(Qt::NonModal);
}

glitch_user_functions::~glitch_user_functions()
{
}

QFrame *glitch_user_functions::frame(void) const
{
  return m_ui.frame;
}

bool glitch_user_functions::contains(const QString &name) const
{
  if(m_model)
    return !m_model->findItems(name, Qt::MatchExactly).isEmpty();
  else
    return false;
}

void glitch_user_functions::addFunction(const QString &name)
{
  if(m_model == nullptr || name.trimmed().isEmpty())
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  auto found = false;

  for(int i = 0; i < m_model->rowCount(); i++)
    if(m_model->item(i, 0) && m_model->item(i, 0)->text() == name)
      {
	found = true;
	break;
      }

  if(!found)
    {
      auto item = new QStandardItem(name);

      item->setData("glitch-user-function");
      item->setFlags
	(Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_model->setRowCount(m_model->rowCount() + 1);
      m_model->setItem(m_model->rowCount() - 1, 0, item);
      m_ui.functions->sortByColumn
	(0, m_ui.functions->horizontalHeader()->sortIndicatorOrder());
    }

  QApplication::restoreOverrideCursor();
}

void glitch_user_functions::deleteFunction(const QString &name)
{
  if(!m_model)
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  for(int i = 0; i < m_model->rowCount(); i++)
    if(m_model->item(i, 0) && m_model->item(i, 0)->text() == name)
      {
	m_model->removeRow(i);
	break;
      }

  QApplication::restoreOverrideCursor();
}

void glitch_user_functions::renameFunction(const QString &before,
					   const QString &after)
{
  if(!m_model)
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  for(int i = 0; i < m_model->rowCount(); i++)
    if(m_model->item(i, 0) && m_model->item(i, 0)->text() == before)
      {
	m_model->item(i, 0)->setText(after);
	break;
      }

  m_ui.functions->sortByColumn
    (0, m_ui.functions->horizontalHeader()->sortIndicatorOrder());
  QApplication::restoreOverrideCursor();
}

void glitch_user_functions::setModel(glitch_user_functions_model *model)
{
  if(m_model == nullptr && model)
    {
      m_model = model;
      m_ui.functions->setModel(m_model);
    }
}

void glitch_user_functions::setProjectType
(const glitch_common::ProjectTypes projectType)
{
  m_ui.functions->setProjectType(projectType);
}

void glitch_user_functions::slotDoubleClicked(const QModelIndex &index)
{
  if(m_model)
    m_model->openFunction(index);
}
