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
#include <QShortcut>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStandardPaths>
#include <QStorageInfo>

#include "glitch-canvas-settings.h"
#include "glitch-common.h"
#include "glitch-misc.h"
#include "glitch-view.h"

glitch_canvas_settings::glitch_canvas_settings(QWidget *parent):
  QDialog(parent)
{
  m_outputFileExtension = "";
  m_timer.start(2500);
  m_ui.setupUi(this);
  glitch_misc::sortCombinationBox(m_ui.update_mode);
  glitch_misc::sortCombinationBox(m_ui.wire_type);
  m_ui.background_color->setStyleSheet("QPushButton {background-color: white}");
  m_ui.background_color->setText(QColor(Qt::white).name());
  m_ui.dots_grids_color->setStyleSheet("QPushButton {background-color: white}");
  m_ui.dots_grids_color->setText(QColor(Qt::white).name());
  m_ui.name->setMaxLength(static_cast<int> (Limits::NAME_MAXIMUM_LENGTH));
  m_ui.output_file_warning_label->setVisible(false);
  m_ui.project_ide->setText("/usr/bin/arduino");
  m_ui.project_ide_warning_label->setVisible(false);
  m_ui.project_type->setEnabled(false);
  m_ui.select_output_file->setIcon(QIcon::fromTheme("document-open"));
  m_ui.select_project_ide->setIcon(QIcon::fromTheme("document-open"));
  m_ui.selection_color->setStyleSheet
    ("QPushButton {background-color: lightgreen}");
  m_ui.selection_color->setText("lightgreen");
  m_ui.update_mode->setCurrentIndex(m_ui.update_mode->findText(tr("Full")));
  m_ui.update_mode->setItemData
    (0,
     tr("<html>The bounding rectangle of all changes in a viewport "
	"will be redrawn. This mode has the advantage that the view "
	"searches only one region for changes, minimizing time spent "
	"determining what needs redrawing. The disadvantage is that "
	"areas that have not changed also need to be redrawn.</html>"),
     Qt::ToolTipRole);
  m_ui.update_mode->setItemData
    (1,
     tr("<html>When any visible part of the scene changes or is reexposed, "
	"the view will update the entire viewport. This approach "
	"is fastest when the view spends more time figuring out what "
	"to draw than it would spend drawing (e.g., when very many small "
	"items are repeatedly updated).</html>"),
     Qt::ToolTipRole);
  m_ui.update_mode->setItemData
    (2,
     tr("<html>The view will determine the minimal viewport region that "
	"requires a redraw, minimizing the time spent drawing by avoiding "
	"a redraw of areas that have not changed. "
	"Although this approach provides the best performance "
	"in general, if there are many small visible changes on the scene, "
	"the view might end up spending more time finding the minimal "
	"approach than it will spend drawing.</html>"),
     Qt::ToolTipRole);
  m_ui.update_mode->setItemData
    (3,
     tr("<html>The view will attempt to find an optimal update mode "
	"by analyzing the areas that require a redraw.</html>"),
     Qt::ToolTipRole);
  m_ui.wire_color->setStyleSheet
    (QString("QPushButton {background-color: %1}").
     arg(QColor(255, 192, 203, 175).name(QColor::HexArgb)));
  m_ui.wire_color->setText(QColor(255, 192, 203, 175).name(QColor::HexArgb));
  m_ui.wire_width->setToolTip(QString("[%1, %2]").
			      arg(m_ui.wire_width->minimum()).
			      arg(m_ui.wire_width->maximum()));
  m_settings = settings();
  new QShortcut(tr("Ctrl+W"),
		this,
		SLOT(close(void)));
  connect(&m_timer,
	  &QTimer::timeout,
	  this,
	  &glitch_canvas_settings::slotTimerTimeout);
  connect(m_ui.background_color,
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::slotSelectColor);
  connect(m_ui.buttonBox->button(QDialogButtonBox::Apply),
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::accept);
  connect(m_ui.dots_grids_color,
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::slotSelectColor);
  connect(m_ui.select_output_file,
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::slotSelectOutputFile);
  connect(m_ui.select_project_ide,
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::slotSelectProjectIDE);
  connect(m_ui.selection_color,
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::slotSelectColor);
  connect(m_ui.wire_color,
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::slotSelectColor);
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

QColor glitch_canvas_settings::dotsGridsColor(void) const
{
  return QColor(m_settings.value(Settings::DOTS_GRIDS_COLOR).
		toString().remove('&').trimmed());
}

QColor glitch_canvas_settings::selectionColor(void) const
{
  return QColor(m_settings.value(Settings::SELECTION_COLOR).
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
  hash[Settings::CATEGORIES_ICON_SIZE] =
    m_ui.categories_icon_size->currentText();
  hash[Settings::DOTS_GRIDS_COLOR] =
    m_ui.dots_grids_color->text().remove('&').trimmed();
  hash[Settings::GENERATE_PERIODICALLY] =
    m_ui.generate_periodically->isChecked();
  hash[Settings::OUTPUT_FILE] = m_ui.output_file->text();
  hash[Settings::PROJECT_IDE] = m_ui.project_ide->text();
  hash[Settings::REDO_UNDO_STACK_SIZE] = m_ui.redo_undo_stack_size->value();
  hash[Settings::SELECTION_COLOR] =
    m_ui.selection_color->text().remove('&').trimmed();
  hash[Settings::SHOW_CANVAS_DOTS] = m_ui.show_canvas_dots->isChecked();
  hash[Settings::SHOW_CANVAS_GRIDS] = m_ui.show_canvas_grids->isChecked();
  hash[Settings::SHOW_ORDER_INDICATORS] =
    m_ui.show_order_indicators->isChecked();

  if(m_ui.update_mode->currentText() == tr("Bounding Rectangle"))
    hash[Settings::VIEW_UPDATE_MODE] =
      QGraphicsView::BoundingRectViewportUpdate;
  else if(m_ui.update_mode->currentText() == tr("Full"))
    hash[Settings::VIEW_UPDATE_MODE] = QGraphicsView::FullViewportUpdate;
  else if(m_ui.update_mode->currentText() == tr("Minimal"))
    hash[Settings::VIEW_UPDATE_MODE] = QGraphicsView::MinimalViewportUpdate;
  else if(m_ui.update_mode->currentText() == tr("Smart"))
    hash[Settings::VIEW_UPDATE_MODE] = QGraphicsView::SmartViewportUpdate;
  else
    hash[Settings::VIEW_UPDATE_MODE] = QGraphicsView::FullViewportUpdate;

  hash[Settings::WIRE_COLOR] = m_ui.wire_color->text().remove('&').trimmed();
  hash[Settings::WIRE_TYPE] = m_ui.wire_type->currentText();
  hash[Settings::WIRE_WIDTH] = m_ui.wire_width->value();
  return hash;
}

QString glitch_canvas_settings::categoriesIconSize(void) const
{
  return m_settings.value(Settings::CATEGORIES_ICON_SIZE).toString().trimmed();
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

QString glitch_canvas_settings::projectIDE(void) const
{
  return m_settings.value(Settings::PROJECT_IDE).toString();
}

QString glitch_canvas_settings::wireType(void) const
{
  return m_settings.value(Settings::WIRE_TYPE).toString().trimmed();
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
	   "categories_icon_size TEXT NOT NULL, "
	   "dots_grids_color TEXT NOT NULL, "
	   "generate_periodically INTEGER NOT NULL DEFAULT 0, "
	   "name TEXT NOT NULL PRIMARY KEY, "
	   "output_file TEXT, "
	   "project_ide TEXT, "
	   "project_type TEXT NOT NULL CHECK "
	   "(project_type IN ('Arduino')), "
	   "redo_undo_stack_size INTEGER NOT NULL DEFAULT 500, "
	   "selection_color TEXT NOT NULL, "
	   "show_canvas_dots INTEGER NOT NULL DEFAULT 1, "
	   "show_canvas_grids INTEGER NOT NULL DEFAULT 1, "
	   "show_order_indicators INTEGER NOT NULL DEFAULT 1, "
	   "update_mode TEXT NOT NULL, "
	   "wire_color TEXT NOT NULL, "
	   "wire_type TEXT NOT NULL, "
	   "wire_width REAL"
	   ")");

	if(!(ok = query.exec("DELETE FROM canvas_settings")))
	  {
	    error = query.lastError().text();
	    goto done_label;
	  }

	query.prepare
	  ("INSERT OR REPLACE INTO canvas_settings "
	   "(background_color, "
	   "categories_icon_size, "
	   "dots_grids_color, "
	   "generate_periodically, "
	   "name, "
	   "output_file, "
	   "project_ide, "
	   "project_type, "
	   "redo_undo_stack_size, "
	   "selection_color, "
	   "show_canvas_dots, "
	   "show_canvas_grids, "
	   "show_order_indicators, "
	   "update_mode, "
	   "wire_color, "
	   "wire_type, "
	   "wire_width) "
	   "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	query.addBindValue(m_ui.background_color->text().remove('&'));
	query.addBindValue(m_ui.categories_icon_size->currentText());
	query.addBindValue(m_ui.dots_grids_color->text().remove('&'));
	query.addBindValue(m_ui.generate_periodically->isChecked());

	auto name(m_ui.name->text().trimmed());

	if(name.isEmpty())
	  name = defaultName();

	query.addBindValue(name);
	query.addBindValue(m_ui.output_file->text());
	query.addBindValue(m_ui.project_ide->text());
	query.addBindValue(m_ui.project_type->currentText());
	query.addBindValue(m_ui.redo_undo_stack_size->value());
	query.addBindValue(m_ui.selection_color->text().remove('&'));
	query.addBindValue(m_ui.show_canvas_dots->isChecked());
	query.addBindValue(m_ui.show_canvas_grids->isChecked());
	query.addBindValue(m_ui.show_order_indicators->isChecked());
	query.addBindValue(m_ui.update_mode->currentText());
	query.addBindValue(m_ui.wire_color->text().remove('&'));
	query.addBindValue(m_ui.wire_type->currentText());
	query.addBindValue(m_ui.wire_width->value());

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

bool glitch_canvas_settings::showOrderIndicators(void) const
{
  return m_settings.value(Settings::SHOW_ORDER_INDICATORS).toBool();
}

double glitch_canvas_settings::wireWidth(void) const
{
  return m_settings.value(Settings::WIRE_WIDTH).toDouble();
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

  notify();
  setResult(QDialog::Accepted);
  setWindowTitle(tr("Glitch: Canvas Settings (%1)").arg(this->name()));
  emit accepted(true);
}

void glitch_canvas_settings::notify(void)
{
  if(m_ui.output_file->text().trimmed().isEmpty())
    {
      m_ui.output_file_warning_label->setVisible(true);
      m_ui.tab->setCurrentIndex(static_cast<int> (Pages::Project));
    }
  else
    m_ui.output_file_warning_label->setVisible(false);

  if(m_ui.project_ide->text().trimmed().isEmpty())
    {
      m_ui.project_ide_warning_label->setVisible(true);
      m_ui.tab->setCurrentIndex(static_cast<int> (Pages::Project));
    }
  else
    m_ui.project_ide_warning_label->setVisible(false);
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
	query.exec("ALTER TABLE canvas_settings ADD categories_icon_size TEXT");
	query.exec("ALTER TABLE canvas_settings ADD project_ide TEXT");
	query.exec("ALTER TABLE canvas_settings ADD selection_color TEXT");
	query.exec("ALTER TABLE canvas_settings ADD wire_width REAL");
	query.exec(QString("SELECT "
			   "SUBSTR(background_color, 1, 50), "
			   "SUBSTR(categories_icon_size, 1, 50), "
			   "SUBSTR(dots_grids_color, 1, 50), "
			   "generate_periodically, "
			   "SUBSTR(name, 1, %1), "
			   "SUBSTR(output_file, 1, 5000), "
			   "SUBSTR(project_ide, 1, 5000), "
			   "SUBSTR(project_type, 1, 50), "
			   "redo_undo_stack_size, "
			   "SUBSTR(selection_color, 1, 50), "
			   "show_canvas_dots, "
			   "show_canvas_grids, "
			   "show_order_indicators, "
			   "SUBSTR(update_mode, 1, 100), "
			   "SUBSTR(wire_color, 1, 50), "
			   "SUBSTR(wire_type, 1, 50), "
			   "wire_width "
			   "FROM canvas_settings").
		   arg(static_cast<int> (Limits::NAME_MAXIMUM_LENGTH)));
	query.next();

	QColor color;
	QColor dotsGridsColor;
	QColor selectionColor;
	QColor wireColor;
	QString categoriesIconSize("");
	QString name("");
	QString outputFile("");
	QString projectIDE("");
	QString projectType("");
	QString updateMode("");
	QString wireType("");
	auto generatePeriodically = false;
	auto record(query.record());
	auto showCanvasDots = true;
	auto showCanvasGrids = true;
	auto showOrderIndicators = true;
	int redoUndoStackSize = 0;

	for(int i = 0; i < record.count(); i++)
	  {
	    auto fieldName(record.fieldName(i));

	    if(fieldName.contains("background_color"))
	      color = QColor(record.value(i).toString().remove('&').trimmed());
	    else if(fieldName.contains("categories_icon_size"))
	      categoriesIconSize = record.value(i).toString().trimmed();
	    else if(fieldName.contains("dots_grids_color"))
	      dotsGridsColor = QColor
		(record.value(i).toString().remove('&').trimmed());
	    else if(fieldName.contains("generate_periodically"))
	      generatePeriodically = record.value(i).toBool();
	    else if(fieldName.contains("name"))
	      name = record.value(i).toString().trimmed();
	    else if(fieldName.contains("output_file"))
	      outputFile = record.value(i).toString();
	    else if(fieldName.contains("project_ide"))
	      projectIDE = record.value(i).toString().trimmed();
	    else if(fieldName.contains("project_type"))
	      projectType = record.value(i).toString().trimmed();
	    else if(fieldName.contains("redo_undo_stack_size"))
	      redoUndoStackSize = record.value(i).toInt();
	    else if(fieldName.contains("selection_color"))
	      selectionColor = QColor
		(record.value(i).toString().remove('&').trimmed());
	    else if(fieldName.contains("show_canvas_dots"))
	      showCanvasDots = record.value(i).toBool();
	    else if(fieldName.contains("show_canvas_grids"))
	      showCanvasGrids = record.value(i).toBool();
	    else if(fieldName.contains("show_order_indicators"))
	      showOrderIndicators = record.value(i).toBool();
	    else if(fieldName.contains("update_mode"))
	      updateMode = record.value(i).toString().trimmed();
	    else if(fieldName.contains("wire_color"))
	      wireColor = QColor
		(record.value(i).toString().remove('&').trimmed());
	    else if(fieldName.contains("wire_type"))
	      wireType = record.value(i).toString().trimmed();
	    else if(fieldName.contains("wire_width"))
	      m_ui.wire_width->setValue(record.value(i).toDouble());
	  }

	if(!color.isValid())
	  color = QColor(Qt::white);

	if(!dotsGridsColor.isValid())
	  dotsGridsColor = QColor(Qt::white);

	if(name.isEmpty())
	  name = defaultName();

	if(!selectionColor.isValid())
	  selectionColor = QColor("lightgreen");

	m_ui.background_color->setStyleSheet
	  (QString("QPushButton {background-color: %1}").arg(color.name()));
	m_ui.background_color->setText(color.name());
	m_ui.categories_icon_size->setCurrentIndex
	  (m_ui.categories_icon_size->findText(categoriesIconSize));

	if(m_ui.categories_icon_size->currentIndex() < 0)
	  m_ui.categories_icon_size->setCurrentIndex(0);

	m_ui.dots_grids_color->setStyleSheet
	  (QString("QPushButton {background-color: %1}").
	   arg(dotsGridsColor.name()));
	m_ui.dots_grids_color->setText(dotsGridsColor.name());
	m_ui.generate_periodically->setChecked(generatePeriodically);
	m_ui.name->setText(name);
	m_ui.name->setCursorPosition(0);
	m_ui.output_file->setText(outputFile);
	m_ui.output_file->setToolTip(m_ui.output_file->text());
	m_ui.output_file->setCursorPosition(0);
	m_ui.project_ide->setText(projectIDE);
	m_ui.project_ide->setToolTip(m_ui.project_ide->text());
	m_ui.project_ide->setCursorPosition(0);
	m_ui.project_type->setCurrentIndex
	  (m_ui.project_type->findText(projectType));

	if(m_ui.project_type->currentIndex() < 0)
	  m_ui.project_type->setCurrentIndex(0);

	m_ui.redo_undo_stack_size->setValue(redoUndoStackSize);
	m_ui.selection_color->setStyleSheet
	  (QString("QPushButton {background-color: %1}").
	   arg(selectionColor.name(QColor::HexArgb)));
	m_ui.selection_color->setText(selectionColor.name(QColor::HexArgb));
	m_ui.show_canvas_dots->setChecked(showCanvasDots);
	m_ui.show_canvas_grids->setChecked(showCanvasGrids);
	m_ui.show_order_indicators->setChecked(showOrderIndicators);
	m_ui.update_mode->setCurrentIndex
	  (m_ui.update_mode->findText(updateMode));

	if(m_ui.update_mode->currentIndex() < 0)
	  m_ui.update_mode->setCurrentIndex
	    (m_ui.update_mode->findText(tr("Full")));

	m_ui.wire_color->setStyleSheet
	  (QString("QPushButton {background-color: %1}").
	   arg(wireColor.name(QColor::HexArgb)));
	m_ui.wire_color->setText(wireColor.name(QColor::HexArgb));
	m_ui.wire_type->setCurrentIndex(m_ui.wire_type->findText(wireType));

	if(m_ui.wire_type->currentIndex() < 0)
	  m_ui.wire_type->setCurrentIndex
	    (m_ui.wire_type->findText(tr("Curve")));

	m_settings = settings();
	setResult(QDialog::Accepted);
	setWindowTitle(tr("Glitch: Canvas Settings (%1)").arg(this->name()));
	emit accepted(false);
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);
  QApplication::restoreOverrideCursor();
}

void glitch_canvas_settings::setCategoriesIconSize(const QString &text)
{
  m_ui.categories_icon_size->setCurrentIndex
    (m_ui.categories_icon_size->findText(text.trimmed()));

  if(m_ui.categories_icon_size->currentIndex() < 0)
    m_ui.categories_icon_size->setCurrentIndex(0);
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
  m_ui.output_file->setToolTip(fileName);
  m_ui.output_file->setCursorPosition(0);
}

void glitch_canvas_settings::setOutputFileExtension(const QString &extension)
{
  m_outputFileExtension = extension;
}

void glitch_canvas_settings::setProjectIDE(const QString &fileName)
{
  m_settings[Settings::PROJECT_IDE] = fileName;
  m_ui.project_ide->setText(fileName);
  m_ui.project_ide->setToolTip(fileName);
  m_ui.project_ide->setCursorPosition(0);
}

void glitch_canvas_settings::setProjectKeywords(const QStringList &l)
{
  auto list(l);

  m_ui.source_view_keywords->setRowCount(list.size());
  std::sort(list.begin(), list.end());

  for(int i = 0; i < m_ui.source_view_keywords->rowCount(); i++)
    {
      auto item = new QTableWidgetItem(list.at(i));

      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.source_view_keywords->setItem(i, 0, item);
      item = new QTableWidgetItem();
      item->setFlags
	(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.source_view_keywords->setItem(i, 1, item);
    }

  m_ui.source_view_keywords->resizeColumnToContents(0);
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
    (hash.value(Settings::DOTS_GRIDS_COLOR).toString().remove('&').trimmed());
  m_ui.dots_grids_color->setStyleSheet
    (QString("QPushButton {background-color: %1}").arg(color.name()));
  m_ui.dots_grids_color->setText(color.name());
  m_ui.generate_periodically->setChecked
    (hash.value(Settings::GENERATE_PERIODICALLY).toBool());
  m_ui.redo_undo_stack_size->setValue
    (hash.value(Settings::REDO_UNDO_STACK_SIZE).toInt());
  color = QColor
    (hash.value(Settings::SELECTION_COLOR).toString().remove('&').trimmed());
  m_ui.selection_color->setStyleSheet
    (QString("QPushButton {background-color: %1}").
     arg(color.name(QColor::HexArgb)));
  m_ui.selection_color->setText(color.name(QColor::HexArgb));
  color = QColor
    (hash.value(Settings::WIRE_COLOR).toString().remove('&').trimmed());
  m_ui.wire_color->setStyleSheet
    (QString("QPushButton {background-color: %1}").
     arg(color.name(QColor::HexArgb)));
  m_ui.wire_color->setText(color.name(QColor::HexArgb));
  setCategoriesIconSize(hash.value(Settings::CATEGORIES_ICON_SIZE).toString());
  setName(hash.value(Settings::CANVAS_NAME).toString());
  setOutputFile(hash.value(Settings::OUTPUT_FILE).toString());
  setProjectIDE(hash.value(Settings::PROJECT_IDE).toString().trimmed());
  setResult(QDialog::Accepted);
  setShowCanvasDots(hash.value(Settings::SHOW_CANVAS_DOTS).toBool());
  setShowCanvasGrids(hash.value(Settings::SHOW_CANVAS_GRIDS).toBool());
  setShowOrderIndicators(hash.value(Settings::SHOW_ORDER_INDICATORS).toBool());
  setViewportUpdateMode
    (QGraphicsView::ViewportUpdateMode(hash.value(Settings::
						  VIEW_UPDATE_MODE).toInt()));
  setWindowTitle(tr("Glitch: Canvas Settings (%1)").arg(name()));
  setWireType(hash.value(Settings::WIRE_TYPE).toString());
  setWireWidth(hash.value(Settings::WIRE_WIDTH).toDouble());
  notify();
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

void glitch_canvas_settings::setShowOrderIndicators(const bool state)
{
  m_ui.show_order_indicators->setChecked(state);
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
    m_ui.update_mode->setCurrentIndex(m_ui.update_mode->findText(tr("Full")));
}

void glitch_canvas_settings::setWireType(const QString &string)
{
  m_ui.wire_type->setCurrentIndex(m_ui.wire_type->findText(string));

  if(m_ui.wire_type->currentIndex() < 0)
    m_ui.wire_type->setCurrentIndex(m_ui.wire_type->findText(tr("Curve")));
}

void glitch_canvas_settings::setWireWidth(const double value)
{
  m_ui.wire_width->setValue(value);
}

void glitch_canvas_settings::slotSelectColor(void)
{
  auto button = qobject_cast<QPushButton *> (sender());

  if(!button)
    return;

  QColorDialog dialog(this);

  if(button == m_ui.background_color || button == m_ui.dots_grids_color)
    dialog.setCurrentColor(QColor(button->text().remove('&')));
  else
    {
      dialog.setCurrentColor(QColor(button->text().remove('&')));
      dialog.setOption(QColorDialog::ShowAlphaChannel, true);
    }

  dialog.setWindowIcon(windowIcon());
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();

      QColor color(dialog.selectedColor());
      auto format = button == m_ui.selection_color || m_ui.wire_color ?
	QColor::HexArgb : QColor::HexRgb;

      button->setStyleSheet
	(QString("QPushButton {background-color: %1;}").
	 arg(color.name(format)));
      button->setText(color.name(format));
    }
  else
    QApplication::processEvents();
}

void glitch_canvas_settings::slotSelectOutputFile(void)
{
  QFileDialog dialog(this);
  QFileInfo fileInfo(m_ui.output_file->text());

  dialog.selectFile(m_ui.name->text() + m_outputFileExtension);
  dialog.setAcceptMode(QFileDialog::AcceptOpen);

  if(fileInfo.isWritable())
    dialog.setDirectory(fileInfo.dir());
  else
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
      QApplication::processEvents();
      m_ui.output_file->setText(dialog.selectedFiles().value(0));
      m_ui.output_file->setToolTip(m_ui.output_file->text());
      m_ui.output_file->setCursorPosition(0);
    }
}

void glitch_canvas_settings::slotSelectProjectIDE(void)
{
  QFileDialog dialog(this);
  QFileInfo fileInfo(m_ui.project_ide->text().trimmed());

  dialog.selectFile(m_ui.project_ide->text().trimmed());
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setDirectory(QStorageInfo::root().displayName());
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setLabelText(QFileDialog::Accept, tr("Select"));
  dialog.setWindowIcon(windowIcon());
  dialog.setWindowTitle(tr("Glitch: Select Project IDE"));
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      m_ui.project_ide->setText(dialog.selectedFiles().value(0));
      m_ui.project_ide->setToolTip(m_ui.project_ide->text().trimmed());
      m_ui.project_ide->setCursorPosition(0);
    }
}

void glitch_canvas_settings::slotTimerTimeout(void)
{
  QFileInfo fileInfo(m_ui.output_file->text());

  if(!fileInfo.isWritable() && fileInfo.exists())
    {
      if(m_ui.output_file->text().trimmed().isEmpty())
	m_ui.output_file_warning_label->setToolTip
	  (tr("The output file is not writable."));
      else
	m_ui.output_file_warning_label->setToolTip
	  (tr("<html>The output file %1 is not writable.</html>").
	   arg(fileInfo.absoluteFilePath()));

      m_ui.output_file_warning_label->setVisible(true);
    }
  else
    {
      m_ui.output_file_warning_label->setToolTip("");
      m_ui.output_file_warning_label->setVisible(false);
    }

  fileInfo = QFileInfo(m_ui.project_ide->text());

  if(!fileInfo.isExecutable())
    {
      if(m_ui.project_ide->text().trimmed().isEmpty())
	m_ui.project_ide_warning_label->setToolTip
	  (tr("The project IDE is not an executable."));
      else
	m_ui.project_ide_warning_label->setToolTip
	  (tr("<html>The project IDE %1 is not an executable.</html>").
	   arg(fileInfo.absoluteFilePath()));

      m_ui.project_ide_warning_label->setVisible(true);
    }
  else
    {
      m_ui.project_ide_warning_label->setToolTip("");
      m_ui.project_ide_warning_label->setVisible(false);
    }
}
