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

#include <QSqlError>
#include <QSqlQuery>

#include "glowbot-object-analog-read-arduino.h"

glowbot_object_analog_read_arduino::glowbot_object_analog_read_arduino
(QWidget *parent):glowbot_object(parent)
{
  m_ui.setupUi(this);
  m_ui.label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
  m_ui.label->setAutoFillBackground(true);
}

glowbot_object_analog_read_arduino::~glowbot_object_analog_read_arduino()
{
}

bool glowbot_object_analog_read_arduino::hasView(void) const
{
  return false;
}

bool glowbot_object_analog_read_arduino::isMandatory(void) const
{
  return false;
}

void glowbot_object_analog_read_arduino::addActions(QMenu &menu) const
{
  Q_UNUSED(menu);
}

void glowbot_object_analog_read_arduino::save(const QSqlDatabase &db,
					      QString &error)
{
  QSqlQuery query(db);
  glowbot_object *parent = qobject_cast<glowbot_object *> (m_parent);

  query.prepare("INSERT OR REPLACE INTO objects "
		"(myoid, parent_oid, position, type) "
		"VALUES(?, ?, ?, ?)");
  query.addBindValue(m_id);

  if(parent)
    query.addBindValue(parent->id());
  else
    query.addBindValue(-1);

  query.addBindValue(QString("(%1,%2)").arg(pos().x()).arg(pos().y()));
  query.addBindValue("arduino-analogread");
  query.exec();

  if(query.lastError().isValid())
    error = query.lastError().text();
}
