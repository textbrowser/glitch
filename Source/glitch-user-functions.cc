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

#include "glitch-user-functions.h"

glitch_user_functions::glitch_user_functions(QWidget *parent):QDialog(parent)
{
  m_ui.setupUi(this);
  m_ui.functions->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  m_ui.functions->horizontalHeader()->setSortIndicatorShown(true);
  new QShortcut(tr("Ctrl+W"),
		this,
		SLOT(close(void)));
  setWindowFlags(Qt::WindowStaysOnTopHint | windowFlags());
  setWindowModality(Qt::NonModal);
}

glitch_user_functions::~glitch_user_functions()
{
}

bool glitch_user_functions::contains(const QString &name) const
{
  return !m_ui.functions->findItems(name, Qt::MatchExactly).isEmpty();
}

void glitch_user_functions::addFunction(const QString &name)
{
  if(name.trimmed().isEmpty())
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  auto found = false;

  for(int i = 0; i < m_ui.functions->rowCount(); i++)
    if(m_ui.functions->item(i, 0) && m_ui.functions->item(i, 0)->text() == name)
      {
	found = true;
	break;
      }

  if(!found)
    {
      auto item = new QTableWidgetItem(name);

      item->setData(Qt::UserRole, "glitch-user-function");
      item->setFlags
	(Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.functions->setRowCount(m_ui.functions->rowCount() + 1);
      m_ui.functions->setItem(m_ui.functions->rowCount() - 1, 0, item);
    }

  QApplication::restoreOverrideCursor();
}

void glitch_user_functions::deleteFunction(const QString &name)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  for(int i = 0; i < m_ui.functions->rowCount(); i++)
    if(m_ui.functions->item(i, 0) && m_ui.functions->item(i, 0)->text() == name)
      {
	m_ui.functions->removeRow(i);
	break;
      }

  QApplication::restoreOverrideCursor();
}

void glitch_user_functions::renameFunction(const QString &before,
					   const QString &after)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  for(int i = 0; i < m_ui.functions->rowCount(); i++)
    if(m_ui.functions->item(i, 0) &&
       m_ui.functions->item(i, 0)->text() == before)
      {
	m_ui.functions->item(i, 0)->setText(after);
	break;
      }

  m_ui.functions->sortByColumn(0, Qt::AscendingOrder);
  QApplication::restoreOverrideCursor();
}

void glitch_user_functions::setProjectType
(const glitch_common::ProjectTypes projectType)
{
  m_ui.functions->setProjectType(projectType);
}
