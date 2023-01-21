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

#include "glitch-object-arrow.h"

glitch_object_arrow::glitch_object_arrow(QWidget *parent):
  glitch_object_arrow(1, parent)
{
}

glitch_object_arrow::glitch_object_arrow
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
}

glitch_object_arrow::~glitch_object_arrow()
{
}

glitch_object_arrow *glitch_object_arrow::clone(QWidget *parent) const
{
  auto clone = new glitch_object_arrow(parent);

  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->compressWidget
    (m_properties.value(Properties::COMPRESSED_WIDGET).toBool());
  clone->resize(size());
  clone->setCanvasSettings(m_canvasSettings);
  clone->setName(clone->name());
  clone->setStyleSheet(styleSheet());
  return clone;
}

void glitch_object_arrow::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_arrow::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);

  QPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing);
}

void glitch_object_arrow::save(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;
}
