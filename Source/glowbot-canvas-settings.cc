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

#include <QDir>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "glowbot-canvas-settings.h"
#include "glowbot-common.h"
#include "glowbot-misc.h"
#include "glowbot-view.h"

glowbot_canvas_settings::glowbot_canvas_settings(QWidget *parent):
  QDialog(parent)
{
  m_ui.setupUi(this);
  m_ui.background_color->setStyleSheet
    (QString("QPushButton {background-color: %1}").
     arg(QColor(211, 211, 211).name()));
  m_ui.background_color->setText(QColor(211, 211, 211).name());
  setModal(false);
  setWindowFlags(Qt::WindowStaysOnTopHint | windowFlags());
}

glowbot_canvas_settings::~glowbot_canvas_settings()
{
}

void glowbot_canvas_settings::accept(void)
{
  save();
  QDialog::accept();
}

void glowbot_canvas_settings::save(void) const
{
  QString connectionName("");

  {
    QSqlDatabase db(glowbot_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName
      (glowbot_misc::homePath() + QDir::separator() + "canvas-settings.db");

    if(db.open())
      {
	QSqlQuery query(db);

	query.exec
	  ("CREATE TABLE IF NOT EXISTS canvas_settings ("
	   "background_color TEXT NOT NULL, "
	   "name TEXT NOT NULL PRIMARY KEY, "
	   "type TEXT NOT NULL CHECK "
	   "(type IN ('arduino')), "
	   "update_mode TEXT NOT NULL CHECK "
	   "(update_mode IN ('bounding_rectangle', 'full', 'minimal', "
	   "'smart'))"
	   ")");
      }

    db.close();
  }

  glowbot_common::discardDatabase(connectionName);
}
