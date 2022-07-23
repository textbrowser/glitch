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
#include <QSqlRecord>
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
     arg(QColor(0, 170, 255).name()));
  m_ui.background_color->setText(QColor(0, 170, 255).name());
  m_ui.dots_color->setStyleSheet("QPushButton {background-color: white}");
  m_ui.dots_color->setText(QColor(Qt::white).name());
  m_ui.name->setMaxLength(static_cast<int> (Limits::NAME_MAXIMUM_LENGTH));
  m_ui.project_type->setEnabled(false);
  m_ui.wire_color->setStyleSheet
    (QString("QPushButton {background-color: %1}").
     arg(QColor(255, 192, 203, 175).name()));
  m_ui.wire_color->setText(QColor(255, 192, 203, 175).name());
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
  connect(m_ui.wire_color,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectColor(void)));
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

QColor glitch_canvas_settings::wireColor(void) const
{
  return QColor(m_settings.value(Settings::WIRE_COLOR).
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
  hash[Settings::GENERATE_PERIODICALLY] =
    m_ui.generate_periodically->isChecked();
  hash[Settings::OUTPUT_FILE] = m_ui.output_file->text();
  hash[Settings::REDO_UNDO_STACK_SIZE] = m_ui.redo_undo_stack_size->value();
  hash[Settings::SHOW_CANVAS_DOTS] = m_ui.show_canvas_dots->isChecked();
  hash[Settings::SHOW_CANVAS_GRIDS] = m_ui.show_canvas_grids->isChecked();

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

  hash[Settings::WIRE_COLOR] = m_ui.wire_color->text().remove('&').trimmed();
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

bool glitch_canvas_settings::generatePeriodically(void) const
{
  return m_settings.value(Settings::GENERATE_PERIODICALLY).toBool();
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
	   "generate_periodically INTEGER NOT NULL DEFAULT 0, "
	   "name TEXT NOT NULL PRIMARY KEY, "
	   "output_file TEXT, "
	   "project_type TEXT NOT NULL CHECK "
	   "(project_type IN ('Arduino')), "
	   "redo_undo_stack_size INTEGER NOT NULL DEFAULT 500, "
	   "show_canvas_dots INTEGER NOT NULL DEFAULT 1, "
	   "show_canvas_grids INTEGER NOT NULL DEFAULT 1, "
	   "update_mode TEXT NOT NULL CHECK "
	   "(update_mode IN ('bounding_rectangle', 'full', 'minimal', "
	   "'smart')), "
	   "wire_color TEXT NOT NULL"
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
	   "generate_periodically, "
	   "name, "
	   "output_file, "
	   "project_type, "
	   "redo_undo_stack_size, "
	   "show_canvas_dots, "
	   "show_canvas_grids, "
	   "update_mode, "
	   "wire_color) "
	   "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	query.addBindValue(m_ui.background_color->text().remove('&'));
	query.addBindValue(m_ui.dots_color->text().remove('&'));
	query.addBindValue(m_ui.generate_periodically->isChecked());

	auto name(m_ui.name->text().trimmed());

	if(name.isEmpty())
	  name = defaultName();

	query.addBindValue(name);
	query.addBindValue(m_ui.output_file->text());
	query.addBindValue(m_ui.project_type->currentText());
	query.addBindValue(m_ui.redo_undo_stack_size->value());
	query.addBindValue(m_ui.show_canvas_dots->isChecked());
	query.addBindValue(m_ui.show_canvas_grids->isChecked());
	query.addBindValue
	  (m_ui.update_mode->currentText().toLower().replace(' ', '_'));
	query.addBindValue(m_ui.wire_color->text().remove('&'));

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

bool glitch_canvas_settings::showCanvasGrids(void) const
{
  return m_settings.value(Settings::SHOW_CANVAS_GRIDS).toBool();
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
      m_ui.name->setToolTip(m_ui.name->text());
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

	query.setForwardOnly(true);

	if(query.exec(QString("SELECT "
			      "SUBSTR(background_color, 1, 50), "
			      "SUBSTR(dots_color, 1, 50), "
			      "generate_periodically, "
			      "SUBSTR(name, 1, %1), "
			      "SUBSTR(output_file, 1, 5000), "
			      "SUBSTR(project_type, 1, 50), "
			      "redo_undo_stack_size, "
			      "show_canvas_dots, "
			      "show_canvas_grids, "
			      "SUBSTR(update_mode, 1, 100), "
			      "SUBSTR(wire_color, 1, 50) "
			      "FROM canvas_settings").
		      arg(static_cast<int> (Limits::NAME_MAXIMUM_LENGTH))) &&
	   query.next())
	  {
	    QColor color;
	    QColor dotsColor;
	    QColor wireColor;
	    QString name("");
	    QString outputFile("");
	    QString projectType("");
	    QString updateMode("");
	    auto generatePeriodically = false;
	    auto record(query.record());
	    auto showCanvasDots = true;
	    auto showCanvasGrids = true;
	    int redoUndoStackSize = 0;

	    for(int i = 0; i < record.count(); i++)
	      {
		auto fieldName(record.fieldName(i));

		if(fieldName.contains("background_color"))
		  color = QColor
		    (record.value(i).toString().remove('&').trimmed());
		else if(fieldName.contains("dots_color"))
		  dotsColor = QColor
		    (record.value(i).toString().remove('&').trimmed());
		else if(fieldName.contains("generate_periodically"))
		  generatePeriodically = record.value(i).toBool();
		else if(fieldName.contains("name"))
		  name = record.value(i).toString().trimmed();
		else if(fieldName.contains("output_file"))
		  outputFile = record.value(i).toString();
		else if(fieldName.contains("project_type"))
		  projectType = record.value(i).toString().trimmed();
		else if(fieldName.contains("redo_undo_stack_size"))
		  redoUndoStackSize = record.value(i).toInt();
		else if(fieldName.contains("show_canvas_dots"))
		  showCanvasDots = record.value(i).toBool();
		else if(fieldName.contains("show_canvas_grids"))
		  showCanvasGrids = record.value(i).toBool();
		else if(fieldName.contains("update_mode"))
		  updateMode = record.value(i).toString().trimmed();
		else if(fieldName.contains("wire_color"))
		  wireColor = QColor
		    (record.value(i).toString().remove('&').trimmed());
	      }

	    if(!color.isValid())
	      color = QColor(0, 170, 255);

	    if(!dotsColor.isValid())
	      dotsColor = QColor(Qt::white);

	    m_ui.background_color->setStyleSheet
	      (QString("QPushButton {background-color: %1}").arg(color.name()));
	    m_ui.background_color->setText(color.name());
	    m_ui.dots_color->setStyleSheet
	      (QString("QPushButton {background-color: %1}").
	       arg(dotsColor.name()));
	    m_ui.dots_color->setText(dotsColor.name());
	    m_ui.generate_periodically->setChecked(generatePeriodically);

	    if(name.isEmpty())
	      name = defaultName();

	    m_ui.name->setText(name);
	    m_ui.name->setToolTip(name);
	    m_ui.name->setCursorPosition(0);
	    m_ui.output_file->setText(outputFile);
	    m_ui.output_file->setToolTip(m_ui.output_file->text());
	    m_ui.output_file->setCursorPosition(0);
	    m_ui.project_type->setCurrentIndex
	      (m_ui.project_type->findText(projectType));

	    if(m_ui.project_type->currentIndex() < 0)
	      m_ui.project_type->setCurrentIndex(0);

	    m_ui.redo_undo_stack_size->setValue(redoUndoStackSize);
	    m_ui.show_canvas_dots->setChecked(showCanvasDots);
	    m_ui.show_canvas_grids->setChecked(showCanvasGrids);
	    m_ui.update_mode->setCurrentIndex
	      (m_ui.update_mode->findText(updateMode, Qt::MatchFixedString));

	    if(m_ui.update_mode->currentIndex() < 0)
	      m_ui.update_mode->setCurrentIndex(1); // Full

	    m_ui.wire_color->setStyleSheet
	      (QString("QPushButton {background-color: %1}").
	       arg(wireColor.name()));
	    m_ui.wire_color->setText(wireColor.name());
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
  m_ui.name->setToolTip(m_ui.name->text());
  m_ui.name->setCursorPosition(0);
}

void glitch_canvas_settings::setOutputFile(const QString &fileName)
{
  m_settings[Settings::OUTPUT_FILE] = fileName;
  m_ui.output_file->setText(fileName);
  m_ui.output_file->setToolTip(fileName);
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
  m_ui.generate_periodically->setChecked
    (hash.value(Settings::GENERATE_PERIODICALLY).toBool());
  m_ui.redo_undo_stack_size->setValue
    (hash.value(Settings::REDO_UNDO_STACK_SIZE).toInt());
  color = QColor
    (hash.value(Settings::WIRE_COLOR).toString().remove('&').trimmed());
  m_ui.wire_color->setStyleSheet
    (QString("QPushButton {background-color: %1}").arg(color.name()));
  m_ui.wire_color->setText(color.name());
  setName(hash.value(Settings::CANVAS_NAME).toString());
  setOutputFile(hash.value(Settings::OUTPUT_FILE).toString());
  setResult(QDialog::Accepted);
  setShowCanvasDots(hash.value(Settings::SHOW_CANVAS_DOTS).toBool());
  setShowCanvasGrids(hash.value(Settings::SHOW_CANVAS_GRIDS).toBool());
  setViewportUpdateMode
    (QGraphicsView::ViewportUpdateMode(hash.value(Settings::
						  VIEW_UPDATE_MODE).toInt()));
  emit accepted(false);
}

void glitch_canvas_settings::setShowCanvasDots(const bool state)
{
  m_ui.show_canvas_dots->setChecked(state);
}

void glitch_canvas_settings::setShowCanvasGrids(const bool state)
{
  m_ui.show_canvas_grids->setChecked(state);
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
  else if(button == m_ui.dots_color)
    dialog.setCurrentColor(QColor(m_ui.dots_color->text().remove('&')));
  else
    dialog.setCurrentColor(QColor(m_ui.wire_color->text().remove('&')));

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
      m_ui.output_file->setToolTip(m_ui.output_file->text());
      m_ui.output_file->setCursorPosition(0);
    }
}
