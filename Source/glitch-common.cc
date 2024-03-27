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

#include "glitch-common.h"
#include "glitch-variety.h"

QChar glitch_common::s_indentationCharacter = ' ';
qint64 glitch_common::s_dbId = 0;
qreal glitch_common::s_maximumZValue = 100000000.0;
qreal glitch_common::s_minimumZValue = -100000000.0;

QSqlDatabase glitch_common::sqliteDatabase(void)
{
  QSqlDatabase db;

  s_dbId += 1;
  db = QSqlDatabase::addDatabase
    ("QSQLITE", QString("glitch_database_%1").arg(s_dbId));
  return db;
}

QString glitch_common::projectTypeToString(const ProjectTypes projectType)
{
  switch(projectType)
    {
    case ProjectTypes::ArduinoProject:
      return "ArduinoProject";
    default:
      return "";
    }
}

void glitch_common::discardDatabase(const QString &connectionName)
{
  QSqlDatabase::removeDatabase(connectionName);
}
