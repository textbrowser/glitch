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

#include <QColorDialog>
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
  connect(m_ui.background_color,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectBackgroundColor(void)));
  connect(m_ui.buttonBox->button(QDialogButtonBox::Apply),
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(accept(void)));
  setWindowModality(Qt::NonModal);
}

glowbot_canvas_settings::~glowbot_canvas_settings()
{
}

QColor glowbot_canvas_settings::backgroundColor(void) const
{
  return QColor(m_ui.background_color->text());
}

QGraphicsView::ViewportUpdateMode glowbot_canvas_settings::
viewportUpdateMode(void) const
{
  switch(m_ui.update_mode->currentIndex())
    {
    case 0:
      return QGraphicsView::BoundingRectViewportUpdate;
    case 1:
      return QGraphicsView::FullViewportUpdate;
    case 2:
      return QGraphicsView::MinimalViewportUpdate;
    case 3:
      return QGraphicsView::SmartViewportUpdate;
    default:
      return QGraphicsView::FullViewportUpdate;
    }
}

QString glowbot_canvas_settings::name(void) const
{
  return m_ui.name->text();
}

QString glowbot_canvas_settings::settings(void) const
{
  return "";
}

bool glowbot_canvas_settings::save(QString &error) const
{
  QString connectionName("");
  bool ok = false;

  {
    QSqlDatabase db(glowbot_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_fileName);

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

	if(!(ok = query.exec("DELETE FROM canvas_settings")))
	  {
	    error = query.lastError().text();
	    goto done_label;
	  }

	query.prepare
	  ("INSERT OR REPLACE INTO canvas_settings "
	   "(background_color, "
	   "name, "
	   "project_type, "
	   "update_mode) "
	   "VALUES (?, ?, ?, ?)");
	query.addBindValue(m_ui.background_color->text());

	QString name(m_ui.name->text().trimmed());

	if(name.isEmpty())
	  query.addBindValue(QVariant::String);
	else
	  query.addBindValue(name);

	query.addBindValue(m_ui.project_type->currentText());
	query.addBindValue
	  (m_ui.update_mode->currentText().toLower().replace(' ', '_'));

	if(!(ok = query.exec()))
	  error = query.lastError().text();
      }
    else
      error = db.lastError().text();

  done_label:
    db.close();
  }

  glowbot_common::discardDatabase(connectionName);
  return ok;
}

void glowbot_canvas_settings::accept(void)
{
  QString error("");

  if(save(error))
    {
      setResult(QDialog::Accepted);
      emit accepted();
    }
  else
    glowbot_misc::showErrorDialog
      (QString("An error (%1) occurred.").arg(error), this);
}

void glowbot_canvas_settings::setFileName(const QString &fileName)
{
  m_fileName = fileName;
}

void glowbot_canvas_settings::setName(const QString &name)
{
  m_ui.name->setText(QString(name).remove("(*)").replace(" ", "-").trimmed());
}

void glowbot_canvas_settings::setViewportUpdateMode
(const QGraphicsView::ViewportUpdateMode mode)
{
  switch(mode)
    {
    case QGraphicsView::BoundingRectViewportUpdate:
      {
	m_ui.update_mode->setCurrentIndex
	  (m_ui.update_mode->findText(tr("Bounding Rectangle")));
	break;
      }
    case QGraphicsView::FullViewportUpdate:
      {
	m_ui.update_mode->setCurrentIndex
	  (m_ui.update_mode->findText(tr("Full")));
	break;
      }
    case QGraphicsView::MinimalViewportUpdate:
      {
	m_ui.update_mode->setCurrentIndex
	  (m_ui.update_mode->findText(tr("Minimal")));
	break;
      }
    case QGraphicsView::SmartViewportUpdate:
      {
	m_ui.update_mode->setCurrentIndex
	  (m_ui.update_mode->findText(tr("Smart")));
	break;
      }
    default:
      m_ui.update_mode->setCurrentIndex
	(m_ui.update_mode->findText(tr("Full")));
    }

  if(m_ui.update_mode->currentIndex() < 0)
    m_ui.update_mode->setCurrentIndex(1); // Full.
}

void glowbot_canvas_settings::slotSelectBackgroundColor(void)
{
  QColorDialog dialog(this);

  dialog.setCurrentColor(QColor(m_ui.background_color->text()));
  dialog.setWindowIcon(windowIcon());

  if(dialog.exec() == QDialog::Accepted)
    {
      QColor color(dialog.selectedColor());

      m_ui.background_color->setText(color.name());
      m_ui.background_color->setStyleSheet
	(QString("QPushButton {background-color: %1;}").
	 arg(color.name()));
    }
}
