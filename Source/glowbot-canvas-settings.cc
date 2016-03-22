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
#include <QSqlError>
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

bool glowbot_canvas_settings::save(QString &error) const
{
  QString connectionName("");
  bool ok = false;

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
	   "project_type TEXT NOT NULL CHECK "
	   "(project_type IN ('Arduino')), "
	   "update_mode TEXT NOT NULL CHECK "
	   "(update_mode IN ('bounding_rectangle', 'full', 'minimal', "
	   "'smart'))"
	   ")");
	query.prepare
	  ("INSERT OR REPLACE INTO canvas_settings "
	   "(background_color, "
	   "name, "
	   "project_type, "
	   "update_mode) "
	   "VALUES (?, ?, ?, ?)");
	query.bindValue(0, m_ui.background_color->text());
	query.bindValue(1, m_ui.name->text());
	query.bindValue(2, m_ui.project_type->currentText());
	query.bindValue
	  (3, m_ui.update_mode->currentText().toLower().replace(' ', '_'));

	if(!(ok = query.exec()))
	  error = query.lastError().text();
      }
    else
      db.lastError().text();

    db.close();
  }

  glowbot_common::discardDatabase(connectionName);
  return ok;
}

void glowbot_canvas_settings::accept(void)
{
  QString error("");

  if(save(error))
    QDialog::accept();
  else
    glowbot_misc::showErrorDialog(error, this);
}

void glowbot_canvas_settings::setName(const QString &name)
{
  m_ui.name->setText(name);
}
