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

#include <QColorDialog>
#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QShortcut>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

#include "glitch-canvas-settings.h"
#include "glitch-common.h"
#include "glitch-misc.h"
#include "glitch-view.h"

glitch_canvas_settings::glitch_canvas_settings(QWidget *parent):
  QDialog(parent)
{
  m_outputFileExtension = "";
  m_ui.setupUi(this);
  m_ui.background_color->setStyleSheet
    (QString("QPushButton {background-color: %1}").
     arg(QColor(211, 211, 211).name()));
  m_ui.background_color->setText(QColor(211, 211, 211).name());
  m_ui.dots_color->setStyleSheet("QPushButton {background-color: black}");
  m_ui.dots_color->setText(QColor(Qt::black).name());
  m_ui.name->setMaxLength(static_cast<int> (Limits::NAME_MAXIMUM_LENGTH));
  m_ui.project_type->setEnabled(false);
  m_settings = settings();
  new QShortcut(tr("Ctrl+W"),
		this,
		SLOT(close(void)));
  connect(m_ui.background_color,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectColor(void)));
  connect(m_ui.buttonBox->button(QDialogButtonBox::Apply),
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(accept(void)));
  connect(m_ui.dots_color,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectColor(void)));
  connect(m_ui.select_output_file,
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::slotSelectOutputFile);
  setWindowModality(Qt::NonModal);
}

glitch_canvas_settings::~glitch_canvas_settings()
{
}

QColor glitch_canvas_settings::canvasBackgroundColor(void) const
{
  return QColor(m_settings.value(Settings::CANVAS_BACKGROUND_COLOR).
		toString().remove('&').trimmed());
}

QColor glitch_canvas_settings::dotsColor(void) const
{
  return QColor(m_settings.value(Settings::DOTS_COLOR).
		toString().remove('&').trimmed());
}

QGraphicsView::ViewportUpdateMode glitch_canvas_settings::
viewportUpdateMode(void) const
{
  return QGraphicsView::ViewportUpdateMode
    (m_settings.value(Settings::VIEW_UPDATE_MODE).toInt());
}

QHash<glitch_canvas_settings::Settings, QVariant> glitch_canvas_settings::
settings(void) const
{
  QHash<Settings, QVariant> hash;

  hash[Settings::CANVAS_BACKGROUND_COLOR] =
    m_ui.background_color->text().remove('&').trimmed();
  hash[Settings::CANVAS_NAME] = m_ui.name->text().trimmed();
  hash[Settings::DOTS_COLOR] = m_ui.dots_color->text().remove('&').trimmed();
  hash[Settings::OUTPUT_FILE] = m_ui.output_file->text();
  hash[Settings::REDO_UNDO_STACK_SIZE] = m_ui.redo_undo_stack_size->value();
  hash[Settings::SHOW_CANVAS_DOTS] = m_ui.show_canvas_dots->isChecked();

  switch(m_ui.update_mode->currentIndex())
    {
    case 0:
      {
	hash[Settings::VIEW_UPDATE_MODE] =
	  QGraphicsView::BoundingRectViewportUpdate;
	break;
      }
    case 1:
      {
	hash[Settings::VIEW_UPDATE_MODE] = QGraphicsView::FullViewportUpdate;
	break;
      }
    case 2:
      {
	hash[Settings::VIEW_UPDATE_MODE] = QGraphicsView::MinimalViewportUpdate;
	break;
      }
    case 3:
      {
	hash[Settings::VIEW_UPDATE_MODE] = QGraphicsView::SmartViewportUpdate;
	break;
      }
    default:
      {
	hash[Settings::VIEW_UPDATE_MODE] = QGraphicsView::FullViewportUpdate;
	break;
      }
    }

  return hash;
}

QString glitch_canvas_settings::defaultName(void) const
{
  if(m_ui.project_type->currentText() == tr("Arduino"))
    return "Arduino-Diagram";
  else
    return "Diagram";
}

QString glitch_canvas_settings::name(void) const
{
  return m_settings.value(Settings::CANVAS_NAME).toString().trimmed();
}

QString glitch_canvas_settings::outputFile(void) const
{
  return m_settings.value(Settings::OUTPUT_FILE).toString();
}

bool glitch_canvas_settings::save(QString &error) const
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QString connectionName("");
  auto ok = false;

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_fileName);

    if(db.open())
      {
	QSqlQuery query(db);

	query.exec
	  ("CREATE TABLE IF NOT EXISTS canvas_settings ("
	   "background_color TEXT NOT NULL, "
	   "dots_color TEXT NOT NULL, "
	   "name TEXT NOT NULL PRIMARY KEY, "
	   "output_file TEXT, "
	   "project_type TEXT NOT NULL CHECK "
	   "(project_type IN ('Arduino')), "
	   "redo_undo_stack_size INTEGER NOT NULL DEFAULT 500, "
	   "show_canvas_dots INTEGER NOT NULL DEFAULT 1, "
	   "tools_operation TEXT NOT NULL DEFAULT 'select', "
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
	   "dots_color, "
	   "name, "
	   "output_file, "
	   "project_type, "
	   "redo_undo_stack_size, "
	   "show_canvas_dots, "
	   "update_mode) "
	   "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
	query.addBindValue(m_ui.background_color->text().remove('&'));
	query.addBindValue(m_ui.dots_color->text().remove('&'));

	auto name(m_ui.name->text().trimmed());

	if(name.isEmpty())
	  name = defaultName();

	query.addBindValue(name);
	query.addBindValue(m_ui.output_file->text());
	query.addBindValue(m_ui.project_type->currentText());
	query.addBindValue(m_ui.redo_undo_stack_size->value());
	query.addBindValue(m_ui.show_canvas_dots->isChecked());
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

  error = error.trimmed();
  glitch_common::discardDatabase(connectionName);
  QApplication::restoreOverrideCursor();
  return ok;
}

bool glitch_canvas_settings::showCanvasDots(void) const
{
  return m_settings.value(Settings::SHOW_CANVAS_DOTS).toBool();
}

int glitch_canvas_settings::redoUndoStackSize(void) const
{
  return m_settings.value(Settings::REDO_UNDO_STACK_SIZE).toInt();
}

void glitch_canvas_settings::accept(void)
{
  auto name(m_ui.name->text().trimmed());

  if(name.isEmpty())
    {
      m_ui.name->setText(defaultName());
      m_ui.name->setCursorPosition(0);
    }

  setResult(QDialog::Accepted);
  emit accepted(true);
}

void glitch_canvas_settings::prepare(void)
{
  QFileInfo fileInfo(m_fileName);

  if(!fileInfo.isReadable())
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QString connectionName("");

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_fileName);

    if(db.open())
      {
	QSqlQuery query(db);

	if(query.exec(QString("SELECT "
			      "SUBSTR(background_color, 1, 50), " // 0
			      "SUBSTR(dots_color, 1, 50), "       // 1
			      "SUBSTR(name, 1, %1), "             // 2
			      "SUBSTR(output_file, 1, 5000), "    // 3
			      "SUBSTR(project_type, 1, 50), "     // 4
			      "redo_undo_stack_size, "            // 5
			      "show_canvas_dots, "                // 6
			      "SUBSTR(update_mode, 1, 100) "      // 7
			      "FROM canvas_settings").
		      arg(static_cast<int> (Limits::NAME_MAXIMUM_LENGTH))) &&
	   query.next())
	  {
	    QColor color(query.value(0).toString().remove('&').trimmed());
	    QColor dotsColor(query.value(1).toString().remove('&').trimmed());
	    auto name(query.value(2).toString().trimmed());
	    auto outputFile(query.value(3).toString());
	    auto projectType(query.value(4).toString().trimmed());
	    auto redoUndoStackSize = query.value(5).toInt();
	    auto showCanvasDots = query.value(6).toBool();
	    auto updateMode(query.value(7).toString().trimmed());

	    if(!color.isValid())
	      color = QColor(211, 211, 211);

	    if(!dotsColor.isValid())
	      dotsColor = QColor(Qt::black);

	    m_ui.background_color->setStyleSheet
	      (QString("QPushButton {background-color: %1}").arg(color.name()));
	    m_ui.background_color->setText(color.name());
	    m_ui.dots_color->setStyleSheet
	      (QString("QPushButton {background-color: %1}").
	       arg(dotsColor.name()));
	    m_ui.dots_color->setText(dotsColor.name());

	    if(name.isEmpty())
	      name = defaultName();

	    m_ui.name->setText(name);
	    m_ui.name->setCursorPosition(0);
	    m_ui.output_file->setText(outputFile);
	    m_ui.output_file->setCursorPosition(0);
	    m_ui.project_type->setCurrentIndex
	      (m_ui.project_type->findText(projectType));

	    if(m_ui.project_type->currentIndex() < 0)
	      m_ui.project_type->setCurrentIndex(0);

	    m_ui.redo_undo_stack_size->setValue(redoUndoStackSize);
	    m_ui.show_canvas_dots->setChecked(showCanvasDots);
	    m_ui.update_mode->setCurrentIndex
	      (m_ui.update_mode->findText(updateMode, Qt::MatchFixedString));

	    if(m_ui.update_mode->currentIndex() < 0)
	      m_ui.update_mode->setCurrentIndex(1); // Full

	    m_settings = settings();
	    setResult(QDialog::Accepted);
	    emit accepted(false);
	  }
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);
  QApplication::restoreOverrideCursor();
}

void glitch_canvas_settings::setFileName(const QString &fileName)
{
  m_fileName = fileName;
}

void glitch_canvas_settings::setName(const QString &name)
{
  if(name.trimmed().isEmpty())
    m_ui.name->setText(defaultName());
  else
    m_ui.name->setText(QString(name).remove("(*)").replace(" ", "-").trimmed());

  m_settings[Settings::CANVAS_NAME] = m_ui.name->text();
  m_ui.name->setCursorPosition(0);
}

void glitch_canvas_settings::setOutputFile(const QString &fileName)
{
  m_settings[Settings::OUTPUT_FILE] = fileName;
  m_ui.output_file->setText(fileName);
  m_ui.output_file->setCursorPosition(0);
}

void glitch_canvas_settings::setOutputFileExtension(const QString &extension)
{
  m_outputFileExtension = extension;
}

void glitch_canvas_settings::setRedoUndoStackSize(const int value)
{
  m_ui.redo_undo_stack_size->setValue(value);
  m_settings[Settings::REDO_UNDO_STACK_SIZE] =
    m_ui.redo_undo_stack_size->value();
}

void glitch_canvas_settings::setSettings
(const QHash<glitch_canvas_settings::Settings, QVariant> &hash)
{
  QColor color(hash.value(Settings::CANVAS_BACKGROUND_COLOR).
	       toString().remove('&').trimmed());

  m_settings = hash;
  m_ui.background_color->setStyleSheet
    (QString("QPushButton {background-color: %1}").arg(color.name()));
  m_ui.background_color->setText(color.name());
  color = QColor
    (hash.value(Settings::DOTS_COLOR).toString().remove('&').trimmed());
  m_ui.dots_color->setStyleSheet
    (QString("QPushButton {background-color: %1}").arg(color.name()));
  m_ui.dots_color->setText(color.name());
  m_ui.redo_undo_stack_size->setValue
    (hash.value(Settings::REDO_UNDO_STACK_SIZE).toInt());
  setName(hash.value(Settings::CANVAS_NAME).toString());
  setOutputFile(hash.value(Settings::OUTPUT_FILE).toString());
  setResult(QDialog::Accepted);
  setShowCanvasDots(hash.value(Settings::SHOW_CANVAS_DOTS).toBool());
  setViewportUpdateMode
    (QGraphicsView::ViewportUpdateMode(hash.value(Settings::
						  VIEW_UPDATE_MODE).toInt()));
  emit accepted(false);
}

void glitch_canvas_settings::setShowCanvasDots(const bool state)
{
  m_ui.show_canvas_dots->setChecked(state);
}

void glitch_canvas_settings::setViewportUpdateMode
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
      {
	m_ui.update_mode->setCurrentIndex
	  (m_ui.update_mode->findText(tr("Full")));
	break;
      }
    }

  if(m_ui.update_mode->currentIndex() < 0)
    m_ui.update_mode->setCurrentIndex(1); // Full.
}

void glitch_canvas_settings::slotSelectColor(void)
{
  auto button = qobject_cast<QPushButton *> (sender());

  if(!button)
    return;

  QColorDialog dialog(this);

  if(button == m_ui.background_color)
    dialog.setCurrentColor(QColor(m_ui.background_color->text().remove('&')));
  else
    dialog.setCurrentColor(QColor(m_ui.dots_color->text().remove('&')));

  dialog.setWindowIcon(windowIcon());
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QColor color(dialog.selectedColor());

      button->setStyleSheet
	(QString("QPushButton {background-color: %1;}").arg(color.name()));
      button->setText(color.name());
    }
}

void glitch_canvas_settings::slotSelectOutputFile(void)
{
  QFileDialog dialog(this);

  dialog.selectFile(m_ui.name->text() + m_outputFileExtension);
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setDirectory
    (QStandardPaths::
     standardLocations(QStandardPaths::DesktopLocation).value(0));
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setLabelText(QFileDialog::Accept, tr("Select"));
  dialog.setWindowIcon(windowIcon());
  dialog.setWindowTitle(tr("Glitch: Select Ouput File"));
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      dialog.close();
      m_ui.output_file->setText(dialog.selectedFiles().value(0));
      m_ui.output_file->setCursorPosition(0);
    }
}
