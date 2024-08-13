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

#include <QDrag>
#include <QMimeData>

#include "glitch-user-functions-tableview.h"

glitch_user_functions_tableview::
glitch_user_functions_tableview(QWidget *parent):QTableView(parent)
{
  m_projectType = glitch_common::ProjectTypes::XYZProject;
}

glitch_user_functions_tableview::
~glitch_user_functions_tableview()
{
}

void glitch_user_functions_tableview::setProjectType
(const glitch_common::ProjectTypes projectType)
{
  m_projectType = projectType;
}

void glitch_user_functions_tableview::startDrag
(Qt::DropActions supportedActions)
{
  Q_UNUSED(supportedActions);

  auto const index = selectedIndexes().value(0);

  if(!index.isValid())
    return;

  auto drag = new QDrag(this);
  auto mimeData = new QMimeData();

  if(m_projectType == glitch_common::ProjectTypes::ArduinoProject)
    mimeData->setText("glitch-arduino-function-" + index.data().toString());
  else
    mimeData->setText("glitch-function-" + index.data().toString());

  drag->setMimeData(mimeData);
  drag->exec(Qt::CopyAction);
}
