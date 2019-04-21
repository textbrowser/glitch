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
**    derived from GlowBot without specific prior written permission.
**
** GLOWBOT IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** GLOWBOT, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QDrag>
#include <QMimeData>

#include "glowbot-user-functions-tablewidget.h"

glowbot_user_functions_tablewidget::
glowbot_user_functions_tablewidget(QWidget *parent):QTableWidget(parent)
{
  m_projectType = glowbot_common::XYZProject;
}

glowbot_user_functions_tablewidget::
~glowbot_user_functions_tablewidget()
{
}

void glowbot_user_functions_tablewidget::setProjectType
(const glowbot_common::ProjectType projectType)
{
  m_projectType = projectType;
}

void glowbot_user_functions_tablewidget::startDrag
(Qt::DropActions supportedActions)
{
  Q_UNUSED(supportedActions);

  QTableWidgetItem *item = selectedItems().value(0);

  if(!item)
    return;

  QDrag *drag = new QDrag(this);
  QMimeData *mimeData = new QMimeData();

  if(m_projectType == glowbot_common::ArduinoProject)
    mimeData->setText("glowbot-arduino-function-" + item->text());
  else
    mimeData->setText("glowbot-function-" + item->text());

  drag->setMimeData(mimeData);
  drag->exec(Qt::CopyAction);
}
