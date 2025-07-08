/*
** Copyright (c) 2015 - 10^10^10, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
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

#include <QClipboard>
#include <QColorDialog>
#include <QDir>
#include <QFileDialog>
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
#include <QSerialPortInfo>
#endif
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStorageInfo>

#include "Arduino/glitch-structures-arduino.h"
#include "glitch-canvas-settings.h"
#include "glitch-common.h"
#include "glitch-variety.h"
#include "glitch-view.h"

glitch_canvas_settings::glitch_canvas_settings(QWidget *parent):
  QDialog(parent)
{
  m_outputFileExtension = "";
  m_timer.start(1500);
  m_ui.setupUi(this);
  glitch_variety::sortCombinationBox(m_ui.tab_position);
  glitch_variety::sortCombinationBox(m_ui.update_mode);
  glitch_variety::sortCombinationBox(m_ui.wire_type);
  glitch_variety::assignImage(m_ui.background_color, QColor("#55aaff"));
  glitch_variety::assignImage(m_ui.dots_grids_color, QColor(Qt::white));
  glitch_variety::assignImage(m_ui.lock_color, QColor(231, 84, 128));
  glitch_variety::assignImage(m_ui.selection_color, QColor(0, 0, 139));
  glitch_variety::assignImage(m_ui.wire_color, QColor(255, 192, 203, 175));
  m_ui.background_color->setText(QColor("#55aaff").name(QColor::HexArgb));
  m_ui.button_box->button(QDialogButtonBox::Apply)->setShortcut(tr("Ctrl+S"));
  m_ui.button_box->button(QDialogButtonBox::Close)->setShortcut(tr("Ctrl+W"));
  m_ui.dots_grids_color->setText(QColor(Qt::white).name(QColor::HexArgb));
  m_ui.lock_color->setText(QColor(231, 84, 128).name(QColor::HexArgb));
  m_ui.name->setMaxLength(static_cast<int> (Limits::NAME_MAXIMUM_LENGTH));
#ifndef GLITCH_SERIAL_PORT_SUPPORTED
  m_ui.project_board->setEnabled(false);
  m_ui.project_board->setToolTip
    (tr("The QtSerialPort module is not available."));
  m_ui.project_communications_port->setEnabled(false);
  m_ui.project_communications_port->setToolTip
    (tr("The QtSerialPort module is not available."));
  m_ui.project_communications_port_refresh->setEnabled(false);
  m_ui.project_communications_port_refresh->setToolTip
    (tr("The QtSerialPort module is not available."));
#endif
#ifdef Q_OS_LINUX
  m_ui.project_ide->setText("/usr/bin/arduino");
#endif
  m_ui.project_ide->setToolTip(m_ui.project_ide->text());
  m_ui.project_ide->setCursorPosition(0);
  m_ui.project_ide->selectAll();
  m_ui.project_ide_warning_label->setVisible(false);
  m_ui.project_type->setEnabled(false);
  m_ui.reset_source_view_keywords->setIcon(QIcon(":/reset.png"));
  m_ui.select_project_ide->setIcon(QIcon(":/open.png"));
  m_ui.selection_color->setText(QColor(0, 0, 139).name(QColor::HexArgb));
  m_ui.source_view_keywords->setItemDelegateForColumn
    (1, m_itemDelegate = new glitch_canvas_settings_item_delegate(this));
  m_ui.special_copy->setIcon(QIcon(":/copy.png"));
  m_ui.special_paste->setIcon(QIcon(":/paste.png"));
  m_ui.tab_position->setCurrentIndex
    (qBound(0,
	    m_ui.tab_position->findText(tr("North")),
	    m_ui.tab_position->count() - 1));
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
  m_ui.wire_color->setText(QColor(255, 192, 203, 175).name(QColor::HexArgb));
  m_ui.wire_width->setToolTip(tr("[%1, %2]").
			      arg(m_ui.wire_width->minimum()).
			      arg(m_ui.wire_width->maximum()));
  m_settings = settings();
  connect(&m_timer,
	  &QTimer::timeout,
	  this,
	  &glitch_canvas_settings::slotTimerTimeout);
  connect(m_ui.background_color,
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::slotSelectColor);
  connect(m_ui.button_box->button(QDialogButtonBox::Apply),
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::accept);
  connect(m_ui.dots_grids_color,
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::slotSelectColor);
  connect(m_ui.lock_color,
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::slotSelectColor);
  connect(m_ui.project_communications_port_refresh,
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::slotCommunicationsPortRefresh);
  connect(m_ui.reset_source_view_keywords,
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::slotResetSourceViewKeywords);
  connect(m_ui.select_project_ide,
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::slotSelectProjectIDE);
  connect(m_ui.selection_color,
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::slotSelectColor);
  connect(m_ui.special_copy,
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::slotSpecialCopy);
  connect(m_ui.special_paste,
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::slotSpecialPaste);
  connect(m_ui.wire_color,
	  &QPushButton::clicked,
	  this,
	  &glitch_canvas_settings::slotSelectColor);
  prepareWidgets();
  setWindowModality(Qt::NonModal);
}

glitch_canvas_settings::~glitch_canvas_settings()
{
  m_timer.stop();
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

QColor glitch_canvas_settings::lockColor(void) const
{
  return QColor
    (m_settings.value(Settings::LOCK_COLOR).toString().remove('&').trimmed());
}

QColor glitch_canvas_settings::selectionColor(void) const
{
  return QColor
    (m_settings.value(Settings::SELECTION_COLOR).toString().remove('&').
     trimmed());
}

QColor glitch_canvas_settings::wireColor(void) const
{
  return QColor
    (m_settings.value(Settings::WIRE_COLOR).toString().remove('&').trimmed());
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
  hash[Settings::GENERATE_SOURCE_VIEW_PERIODICALLY] =
    m_ui.generate_source_view_periodically->isChecked();
  hash[Settings::KEYWORD_COLORS] = keywordColorsFromTableAsString().trimmed();
  hash[Settings::LOCK_COLOR] = m_ui.lock_color->text().remove('&').trimmed();
  hash[Settings::MAXIMIZE_EDIT_WINDOWS] =
    m_ui.maximize_edit_windows->isChecked();
  hash[Settings::PROJECT_BOARD] = m_ui.project_board->currentText().trimmed();
  hash[Settings::PROJECT_COMMUNICATIONS_PORT] =
    m_ui.project_communications_port->currentText().trimmed();
  hash[Settings::PROJECT_IDE] = QFileInfo
    (m_ui.project_ide->text()).absoluteFilePath();
  hash[Settings::REDO_UNDO_STACK_SIZE] = m_ui.redo_undo_stack_size->value();
  hash[Settings::SAVE_PERIODICALLY] = m_ui.save_periodically->isChecked();
  hash[Settings::SELECTION_COLOR] =
    m_ui.selection_color->text().remove('&').trimmed();
  hash[Settings::SHOW_CANVAS_DOTS] = m_ui.show_canvas_dots->isChecked();
  hash[Settings::SHOW_CANVAS_GRIDS] = m_ui.show_canvas_grids->isChecked();
  hash[Settings::SHOW_ORDER_INDICATORS] =
    m_ui.show_order_indicators->isChecked();
  hash[Settings::SHOW_PREVIEW] = m_ui.show_preview->isChecked();
  hash[Settings::TAB_POSITION_INDEX] = m_ui.tab_position->currentIndex();
  hash[Settings::TABBED_EDIT_WINDOWS] = m_ui.tabbed_edit_windows->isChecked();

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

QMap<QString, QColor> glitch_canvas_settings::keywordColorsAsMap(void) const
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QMap<QString, QColor> map;
  auto const text(m_settings.value(Settings::KEYWORD_COLORS).toString());

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  foreach(auto const &string, text.mid(7).split(',', Qt::SkipEmptyParts))
#else
  foreach(auto const &string, text.mid(7).split(',', QString::SkipEmptyParts))
#endif
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
      auto const list(string.split(';', Qt::SkipEmptyParts));
#else
      auto const list(string.split(';', QString::SkipEmptyParts));
#endif

      if(list.size() == 2)
	map[list.at(0)] = QColor(list.at(1));
    }

  if(m_ui.project_type->currentText() == "Arduino" && map.isEmpty())
    {
      QHashIterator<QString, QColor> it
	(glitch_structures_arduino::defaultColors());

      while(it.hasNext())
	{
	  it.next();
	  map[it.key()] = it.value();
	}
    }

  QApplication::restoreOverrideCursor();
  return map;
}

QString glitch_canvas_settings::categoriesIconSize(void) const
{
  return m_settings.value(Settings::CATEGORIES_ICON_SIZE).toString().trimmed();
}

QString glitch_canvas_settings::defaultName(void) const
{
  if(m_ui.project_type->currentText() == "Arduino")
    return "Arduino-Diagram";
  else
    return "Diagram";
}

QString glitch_canvas_settings::keywordColorsAsString(void) const
{
  return m_settings.value(Settings::KEYWORD_COLORS).toString().trimmed();
}

QString glitch_canvas_settings::keywordColorsFromTableAsString(void) const
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QString string("");

  for(int i = 0; i < m_ui.source_view_keywords->rowCount(); i++)
    {
      auto item1 = m_ui.source_view_keywords->item(i, 0);
      auto item2 = m_ui.source_view_keywords->item(i, 1);

      if(item1 && item2)
	{
	  string.append(item1->text());
	  string.append(";");
	  string.append(item2->text());
	  string.append(",");
	}
    }

  QApplication::restoreOverrideCursor();

  if(!string.isEmpty())
    return "glitch-" + string.mid(0, string.length() - 1);
  else
    return string;
}

QString glitch_canvas_settings::name(void) const
{
  auto const name
    (m_settings.value(Settings::CANVAS_NAME).toString().trimmed());

  if(name.isEmpty())
    return "Arduino-Diagram";
  else
    return name;
}

QString glitch_canvas_settings::outputFile(void) const
{
  auto const name(this->name());
  auto const path
    (QSettings().value("preferences/output_directory").toString().trimmed());

  QDir().mkpath(path + QDir::separator() + name);
  return QFileInfo
    (path +
     QDir::separator() +
     name +
     QDir::separator() +
     name +
     m_outputFileExtension).absoluteFilePath();
}

QString glitch_canvas_settings::projectBoard(void) const
{
  return m_settings.value(Settings::PROJECT_BOARD).toString();
}

QString glitch_canvas_settings::projectCommunicationsPort(void) const
{
  return m_settings.value(Settings::PROJECT_COMMUNICATIONS_PORT).toString();
}

QString glitch_canvas_settings::projectIDE(void) const
{
  return QFileInfo
    (m_settings.value(Settings::PROJECT_IDE).toString()).absoluteFilePath();
}

QString glitch_canvas_settings::wireType(void) const
{
  return m_settings.value(Settings::WIRE_TYPE).toString().trimmed();
}

QTabWidget::TabPosition glitch_canvas_settings::tabPosition(void) const
{
  auto const text
    (m_ui.tab_position->
     itemText(m_settings.value(Settings::TAB_POSITION_INDEX, -1).toInt()));

  if(text == tr("East"))
    return QTabWidget::East;
  else if(text == tr("South"))
    return QTabWidget::South;
  else if(text == tr("West"))
    return QTabWidget::West;
  else
    return QTabWidget::North;
}

bool glitch_canvas_settings::generatePeriodically(void) const
{
  return m_settings.value(Settings::GENERATE_PERIODICALLY).toBool();
}

bool glitch_canvas_settings::generateSourceViewPeriodically(void) const
{
  return m_settings.value
    (Settings::GENERATE_SOURCE_VIEW_PERIODICALLY).toBool();
}

bool glitch_canvas_settings::maximizeEditWindows(void) const
{
#ifndef Q_OS_ANDROID
  return m_settings.value(Settings::MAXIMIZE_EDIT_WINDOWS).toBool();
#else
  return true;
#endif
}

bool glitch_canvas_settings::save(QString &error) const
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  alterDatabase();

  QString connectionName("");
  auto ok = true;

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_fileName);

    if((ok = db.open()))
      {
	QSqlQuery query(db);

	query.exec("PRAGMA JOURNAL_MODE = WAL");
	query.exec("PRAGMA SYNCHRONOUS = NORMAL");
	query.exec
	  ("CREATE TABLE IF NOT EXISTS canvas_settings ("
	   "background_color TEXT NOT NULL, "
	   "categories_icon_size TEXT NOT NULL, "
	   "dots_grids_color TEXT NOT NULL, "
	   "generate_periodically INTEGER NOT NULL DEFAULT 0, "
	   "generate_source_view_periodically INTEGER NOT NULL DEFAULT 0, "
	   "keyword_colors TEXT, "
	   "lock_color TEXT NOT NULL, "
	   "maximize_edit_windows INTEGER NOT NULL DEFAULT 0, "
	   "name TEXT NOT NULL PRIMARY KEY, "
	   "output_file TEXT, "
	   "project_board TEXT, "
	   "project_ide TEXT, "
	   "project_communications_port TEXT, "
	   "project_type TEXT NOT NULL CHECK "
	   "(project_type IN ('Arduino')), "
	   "redo_undo_stack_size INTEGER NOT NULL DEFAULT 1500, "
	   "save_periodically INTEGER NOT NULL DEFAULT 0, "
	   "selection_color TEXT NOT NULL, "
	   "show_canvas_dots INTEGER NOT NULL DEFAULT 1, "
	   "show_canvas_grids INTEGER NOT NULL DEFAULT 1, "
	   "show_order_indicators INTEGER NOT NULL DEFAULT 1, "
	   "show_preview INTEGER NOT NULL DEFAULT 0, "
	   "tab_position_index INTEGER NOT NULL DEFAULT -1, "
	   "tabbed_edit_windows INTEGER NOT NULL DEFAULT 1, "
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
	  ("INSERT INTO canvas_settings "
	   "(background_color, "
	   "categories_icon_size, "
	   "dots_grids_color, "
	   "generate_periodically, "
	   "generate_source_view_periodically, "
	   "keyword_colors, "
	   "lock_color, "
	   "maximize_edit_windows, "
	   "name, "
	   "output_file, "
	   "project_board, "
	   "project_communications_port, "
	   "project_ide, "
	   "project_type, "
	   "redo_undo_stack_size, "
	   "save_periodically, "
	   "selection_color, "
	   "show_canvas_dots, "
	   "show_canvas_grids, "
	   "show_order_indicators, "
	   "show_preview, "
	   "tab_position_index, "
	   "tabbed_edit_windows, "
	   "update_mode, "
	   "wire_color, "
	   "wire_type, "
	   "wire_width) "
	   "VALUES "
	   "(?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?, "
	   "?)");
	query.addBindValue(m_ui.background_color->text().remove('&'));
	query.addBindValue(m_ui.categories_icon_size->currentText());
	query.addBindValue(m_ui.dots_grids_color->text().remove('&'));
	query.addBindValue(m_ui.generate_periodically->isChecked());
	query.addBindValue
	  (m_ui.generate_source_view_periodically->isChecked());
	query.addBindValue(keywordColorsFromTableAsString());
	query.addBindValue(m_ui.lock_color->text().remove('&'));
	query.addBindValue(m_ui.maximize_edit_windows->isChecked());

	auto name(m_ui.name->text().trimmed());

	if(name.isEmpty())
	  name = defaultName();

	query.addBindValue(name);
	query.addBindValue("");
	query.addBindValue(m_ui.project_board->currentText().trimmed());
	query.addBindValue(m_ui.project_communications_port->currentText());
	query.addBindValue
	  (QFileInfo(m_ui.project_ide->text()).absoluteFilePath());
	query.addBindValue(m_ui.project_type->currentText());
	query.addBindValue(m_ui.redo_undo_stack_size->value());
	query.addBindValue(m_ui.save_periodically->isChecked());
	query.addBindValue(m_ui.selection_color->text().remove('&'));
	query.addBindValue(m_ui.show_canvas_dots->isChecked());
	query.addBindValue(m_ui.show_canvas_grids->isChecked());
	query.addBindValue(m_ui.show_order_indicators->isChecked());
	query.addBindValue(m_ui.show_preview->isChecked());
	query.addBindValue(m_ui.tab_position->currentIndex());
	query.addBindValue(m_ui.tabbed_edit_windows->isChecked());
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

bool glitch_canvas_settings::savePeriodically(void) const
{
  return m_settings.value(Settings::SAVE_PERIODICALLY).toBool();
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

bool glitch_canvas_settings::showPreview(void) const
{
  return m_settings.value(Settings::SHOW_PREVIEW).toBool();
}

bool glitch_canvas_settings::tabbedEditWindows(void) const
{
  return m_settings.value(Settings::TABBED_EDIT_WINDOWS).toBool();
}

double glitch_canvas_settings::wireWidth(void) const
{
  return m_settings.value(Settings::WIRE_WIDTH).toDouble();
}

int glitch_canvas_settings::adjustedTabPositionIndexFromIndex
(const int index) const
{
  if(index < 0 || index >= m_ui.tab_position->count())
    return qBound(0,
		  m_ui.tab_position->findText(tr("North")),
		  m_ui.tab_position->count() - 1);
  else
    return index;
}

int glitch_canvas_settings::redoUndoStackSize(void) const
{
  return m_settings.value(Settings::REDO_UNDO_STACK_SIZE).toInt();
}

void glitch_canvas_settings::accept(void)
{
  auto const name(m_ui.name->text().trimmed());

  if(name.isEmpty())
    m_ui.name->setText(defaultName());

  m_ui.name->setCursorPosition(0);
  m_ui.name->selectAll();
  setProjectIDE(m_ui.project_ide->text());
  setResult(QDialog::Accepted);
  setWindowTitle(tr("Glitch: Canvas Settings (%1)").arg(this->name()));
  emit accepted(true);
}

void glitch_canvas_settings::alterDatabase(void) const
{
  QString connectionName("");

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_fileName);

    if(db.open())
      {
	QSqlQuery query(db);

	query.exec("ALTER TABLE canvas_settings ADD project_board TEXT");
	query.exec
	  ("ALTER TABLE canvas_settings ADD project_communications_port TEXT");
	query.exec
	  ("ALTER TABLE canvas_settings ADD show_preview "
	   "INTEGER NOT NULL DEFAULT 0");
	query.exec
	  ("ALTER TABLE canvas_settings ADD tab_position_index "
	   "INTEGER NOT NULL DEFAULT -1");
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);
}

void glitch_canvas_settings::closeEvent(QCloseEvent *event)
{
  QDialog::closeEvent(event);
  m_timer.stop();
}

void glitch_canvas_settings::prepare(const QString &fileName)
{
  m_fileName = fileName;

  QFileInfo const fileInfo(m_fileName);

  if(!fileInfo.isReadable())
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  alterDatabase();

  QString connectionName("");

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_fileName);

    if(db.open())
      {
	QSqlQuery query(db);

	query.setForwardOnly(true);
	query.exec(QString("SELECT "
			   "SUBSTR(background_color, 1, 50), "
			   "SUBSTR(categories_icon_size, 1, 50), "
			   "SUBSTR(dots_grids_color, 1, 50), "
			   "generate_periodically, "
			   "generate_source_view_periodically, "
			   "SUBSTR(keyword_colors, 1, 5000), "
			   "SUBSTR(lock_color, 1, 50), "
			   "maximize_edit_windows, "
			   "SUBSTR(name, 1, %1), "
			   "SUBSTR(output_file, 1, 5000), "
			   "SUBSTR(project_board, 1, 5000), "
			   "SUBSTR(project_communications_port, 1, 5000), "
			   "SUBSTR(project_ide, 1, 5000), "
			   "SUBSTR(project_type, 1, 50), "
			   "redo_undo_stack_size, "
			   "save_periodically, "
			   "SUBSTR(selection_color, 1, 50), "
			   "show_canvas_dots, "
			   "show_canvas_grids, "
			   "show_order_indicators, "
			   "show_preview, "
			   "tab_position_index, "
			   "tabbed_edit_windows, "
			   "SUBSTR(update_mode, 1, 100), "
			   "SUBSTR(wire_color, 1, 50), "
			   "SUBSTR(wire_type, 1, 50), "
			   "wire_width "
			   "FROM canvas_settings").
		   arg(static_cast<int> (Limits::NAME_MAXIMUM_LENGTH)));
	query.next();

	QColor color;
	QColor dotsGridsColor;
	QColor lockColor;
	QColor selectionColor;
	QColor wireColor;
	QString categoriesIconSize("");
	QString keywordColors("");
	QString name("");
	QString projectBoard("");
	QString projectCommunicationsPort("");
#ifdef Q_OS_LINUX
	QString projectIDE("/usr/bin/arduino");
#else
	QString projectIDE("");
#endif
	QString projectType("");
	QString updateMode("");
	QString wireType("");
	auto const record(query.record());
	auto generatePeriodically = false;
	auto generateSourceViewPeriodically = false;
	auto maximizeEditWindows = false;
	auto redoUndoStackSize = 0;
	auto savePeriodically = false;
	auto showCanvasDots = true;
	auto showCanvasGrids = true;
	auto showOrderIndicators = true;
	auto showPreview = false;
	auto tabbedEditWindows = true;
	int tabPositionIndex = -1;

	for(int i = 0; i < record.count(); i++)
	  {
	    auto const fieldName(record.fieldName(i));

	    if(fieldName.contains("background_color"))
	      color = QColor(record.value(i).toString().remove('&').trimmed());
	    else if(fieldName.contains("categories_icon_size"))
	      categoriesIconSize = record.value(i).toString().trimmed();
	    else if(fieldName.contains("dots_grids_color"))
	      dotsGridsColor = QColor
		(record.value(i).toString().remove('&').trimmed());
	    else if(fieldName.contains("generate_periodically"))
	      generatePeriodically = record.value(i).toBool();
	    else if(fieldName.contains("generate_source_view_periodically"))
	      generateSourceViewPeriodically = record.value(i).toBool();
	    else if(fieldName.contains("keyword_colors"))
	      keywordColors = record.value(i).toString().trimmed();
	    else if(fieldName.contains("lock_color"))
	      lockColor = QColor
		(record.value(i).toString().remove('&').trimmed());
	    else if(fieldName.contains("maximize_edit_windows"))
	      maximizeEditWindows = record.value(i).toBool();
	    else if(fieldName.contains("name"))
	      name = record.value(i).toString().trimmed();
	    else if(fieldName.contains("project_board"))
	      projectBoard = record.value(i).toString().trimmed();
	    else if(fieldName.contains("project_communications_port"))
	      projectCommunicationsPort = record.value(i).toString().trimmed();
	    else if(fieldName.contains("project_ide"))
	      {
		projectIDE = QFileInfo
		  (record.value(i).toString().trimmed()).absoluteFilePath();

#ifdef Q_OS_LINUX
		if(projectIDE.isEmpty())
		  projectIDE = "/usr/bin/arduino";
#endif
	      }
	    else if(fieldName.contains("project_type"))
	      projectType = record.value(i).toString().trimmed();
	    else if(fieldName.contains("redo_undo_stack_size"))
	      redoUndoStackSize = record.value(i).toInt();
	    else if(fieldName.contains("save_periodically"))
	      savePeriodically = record.value(i).toBool();
	    else if(fieldName.contains("selection_color"))
	      selectionColor = QColor
		(record.value(i).toString().remove('&').trimmed());
	    else if(fieldName.contains("show_canvas_dots"))
	      showCanvasDots = record.value(i).toBool();
	    else if(fieldName.contains("show_canvas_grids"))
	      showCanvasGrids = record.value(i).toBool();
	    else if(fieldName.contains("show_order_indicators"))
	      showOrderIndicators = record.value(i).toBool();
	    else if(fieldName.contains("show_preview"))
	      showPreview = record.value(i).toBool();
	    else if(fieldName.contains("tab_position_index"))
	      tabPositionIndex = record.value(i).toInt();
	    else if(fieldName.contains("tabbed_edit_windows"))
	      tabbedEditWindows = record.value(i).toBool();
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
	  color = QColor("#55aaff");

	if(!dotsGridsColor.isValid())
	  dotsGridsColor = QColor(Qt::white);

	if(!lockColor.isValid())
	  lockColor = QColor("orange");

	if(name.isEmpty())
	  name = defaultName();

	if(!selectionColor.isValid())
	  selectionColor = QColor("lightgreen");

	glitch_variety::assignImage(m_ui.background_color, color);
	glitch_variety::assignImage(m_ui.dots_grids_color, dotsGridsColor);
	glitch_variety::assignImage(m_ui.lock_color, lockColor);
	glitch_variety::assignImage(m_ui.selection_color, selectionColor);
	glitch_variety::assignImage(m_ui.wire_color, wireColor);
	m_ui.background_color->setText(color.name(QColor::HexArgb));
	m_ui.categories_icon_size->setCurrentIndex
	  (m_ui.categories_icon_size->findText(categoriesIconSize));

	if(m_ui.categories_icon_size->currentIndex() < 0)
	  m_ui.categories_icon_size->setCurrentIndex(1); // 24x24

	m_ui.dots_grids_color->setText(dotsGridsColor.name(QColor::HexArgb));
	m_ui.generate_periodically->setChecked(generatePeriodically);
	m_ui.generate_source_view_periodically->setChecked
	  (generateSourceViewPeriodically);
	m_ui.lock_color->setText(lockColor.name(QColor::HexArgb));
	m_ui.maximize_edit_windows->setChecked(maximizeEditWindows);
	m_ui.name->setText(name);
	m_ui.name->setCursorPosition(0);
	m_ui.name->selectAll();
	m_ui.project_board->setCurrentIndex
	  (m_ui.project_board->findText(projectBoard));
	m_ui.project_communications_port->setCurrentIndex
	  (m_ui.project_communications_port->
	   findText(projectCommunicationsPort));

	if(m_ui.project_communications_port->currentIndex() < 0)
	  m_ui.project_communications_port->setCurrentIndex(0);

	m_ui.project_ide->setText(projectIDE);
	m_ui.project_ide->setToolTip(m_ui.project_ide->text());
	m_ui.project_ide->setCursorPosition(0);
	m_ui.project_ide->selectAll();
	m_ui.project_type->setCurrentIndex
	  (m_ui.project_type->findText(projectType));

	if(m_ui.project_type->currentIndex() < 0)
	  m_ui.project_type->setCurrentIndex(0);

	m_ui.redo_undo_stack_size->setValue(redoUndoStackSize);
	m_ui.save_periodically->setChecked(savePeriodically);
	m_ui.selection_color->setText(selectionColor.name(QColor::HexArgb));
	m_ui.show_canvas_dots->setChecked(showCanvasDots);
	m_ui.show_canvas_grids->setChecked(showCanvasGrids);
	m_ui.show_order_indicators->setChecked(showOrderIndicators);
	m_ui.show_preview->setChecked(showPreview);
	m_ui.tab_position->setCurrentIndex
	  (adjustedTabPositionIndexFromIndex(tabPositionIndex));
	m_ui.tabbed_edit_windows->setChecked(tabbedEditWindows);
	m_ui.update_mode->setCurrentIndex
	  (m_ui.update_mode->findText(updateMode));

	if(m_ui.update_mode->currentIndex() < 0)
	  m_ui.update_mode->setCurrentIndex
	    (m_ui.update_mode->findText(tr("Full")));

	m_ui.wire_color->setText(wireColor.name(QColor::HexArgb));
	m_ui.wire_type->setCurrentIndex(m_ui.wire_type->findText(wireType));

	if(m_ui.wire_type->currentIndex() < 0)
	  m_ui.wire_type->setCurrentIndex
	    (m_ui.wire_type->findText(tr("Curve")));

	prepareKeywordColors(keywordColors); // Order (a) is important.
	m_settings = settings(); // Order (b) is important.
	setResult(QDialog::Accepted);
	setWindowTitle(tr("Glitch: Canvas Settings (%1)").arg(this->name()));
	emit accepted(false);
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);
  QApplication::restoreOverrideCursor();
}

void glitch_canvas_settings::prepareKeywordColors(const QString &text)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QMap<QString, QColor> map;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  foreach(auto const &string, text.mid(7).split(',', Qt::SkipEmptyParts))
#else
  foreach(auto const &string, text.mid(7).split(',', QString::SkipEmptyParts))
#endif
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
      auto const list(string.split(';', Qt::SkipEmptyParts));
#else
      auto const list(string.split(';', QString::SkipEmptyParts));
#endif

      if(list.size() == 2)
	map[list.at(0)] = QColor(list.at(1));
    }

  if(m_ui.project_type->currentText() == "Arduino" && map.isEmpty())
    {
      QHashIterator<QString, QColor> it
	(glitch_structures_arduino::defaultColors());

      while(it.hasNext())
	{
	  it.next();
	  map[it.key()] = it.value();
	}
    }

  m_ui.source_view_keywords->setSortingEnabled(false);

  for(int i = 0; i < m_ui.source_view_keywords->rowCount(); i++)
    {
      auto item1 = m_ui.source_view_keywords->item(i, 0);
      auto item2 = m_ui.source_view_keywords->item(i, 1);

      if(item1 && item2)
	{
	  item2->setData
	    (Qt::DecorationRole, map.value(item1->text()));
	  item2->setText(map.value(item1->text()).name(QColor::HexArgb));
	}
    }

  m_ui.source_view_keywords->setSortingEnabled(true);
  QApplication::restoreOverrideCursor();
}

void glitch_canvas_settings::prepareWidgets(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
  auto const systemLocation(m_ui.project_communications_port->currentText());

  m_ui.project_communications_port->clear();

  foreach(auto const &port, QSerialPortInfo::availablePorts())
    m_ui.project_communications_port->addItem(port.systemLocation());

  glitch_variety::sortCombinationBox(m_ui.project_communications_port);
  m_ui.project_communications_port->setCurrentIndex
    (m_ui.project_communications_port->findText(systemLocation));

  if(m_ui.project_communications_port->currentIndex() < 0)
    m_ui.project_communications_port->setCurrentIndex(0);

  if(m_ui.project_communications_port->count() == 0)
    {
      m_ui.project_communications_port->addItem
	("/dev/null"); // Do not translate.
      m_ui.project_communications_port->setCurrentIndex(0);
    }
#endif
  QApplication::restoreOverrideCursor();
}

void glitch_canvas_settings::setCategoriesIconSize(const QString &text)
{
  m_ui.categories_icon_size->setCurrentIndex
    (m_ui.categories_icon_size->findText(text.trimmed()));

  if(m_ui.categories_icon_size->currentIndex() < 0)
    m_ui.categories_icon_size->setCurrentIndex(1); // 24x24
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
    m_ui.name->setText
      (QString(name).remove("(*)").replace(" ", "-").trimmed());

  m_settings[Settings::CANVAS_NAME] = m_ui.name->text();
  m_ui.name->setCursorPosition(0);
  m_ui.name->selectAll();
}

void glitch_canvas_settings::setOutputFileExtension(const QString &extension)
{
  m_outputFileExtension = extension;
}

void glitch_canvas_settings::setProjectIDE(const QString &fileName)
{
  m_settings[Settings::PROJECT_IDE] = QFileInfo
    (fileName.trimmed()).absoluteFilePath();
  m_ui.project_ide->setText
    (m_settings.value(Settings::PROJECT_IDE).toString());
  m_ui.project_ide->setToolTip(m_ui.project_ide->text());
  m_ui.project_ide->setCursorPosition(0);
  m_ui.project_ide->selectAll();
}

void glitch_canvas_settings::setProjectKeywords(const QStringList &list)
{
  QHash<QString, QColor> colors;

  if(m_ui.project_type->currentText() == "Arduino")
    colors = glitch_structures_arduino::defaultColors();

  m_ui.source_view_keywords->setRowCount(list.size());
  m_ui.source_view_keywords->setSortingEnabled(false);

  for(int i = 0; i < m_ui.source_view_keywords->rowCount(); i++)
    {
      auto item = new QTableWidgetItem(list.at(i));

      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.source_view_keywords->setItem(i, 0, item);
      item = new QTableWidgetItem(QColor(Qt::black).name(QColor::HexArgb));
      item->setData(Qt::DecorationRole, QColor(item->text()));
      item->setFlags
	(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

      if(colors.contains(list.at(i)))
	{
	  item->setData(Qt::DecorationRole, colors.value(list.at(i)));
	  item->setText(colors.value(list.at(i)).name(QColor::HexArgb));
	}

      m_ui.source_view_keywords->setItem(i, 1, item);
    }

  m_ui.source_view_keywords->resizeColumnToContents(0);
  m_ui.source_view_keywords->setSortingEnabled(true);
  m_ui.source_view_keywords->sortByColumn(0, Qt::AscendingOrder);
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
  QColor color
    (hash.value(Settings::CANVAS_BACKGROUND_COLOR).toString().remove('&').
     trimmed());
  auto const same = hash == m_settings;

  m_settings = hash;
  glitch_variety::assignImage(m_ui.background_color, color);
  m_ui.background_color->setText(color.name(QColor::HexArgb));
  color = QColor
    (hash.value(Settings::DOTS_GRIDS_COLOR).toString().remove('&').trimmed());
  glitch_variety::assignImage(m_ui.dots_grids_color, color);
  m_ui.dots_grids_color->setText(color.name(QColor::HexArgb));
  m_ui.generate_periodically->setChecked
    (hash.value(Settings::GENERATE_PERIODICALLY).toBool());
  m_ui.generate_source_view_periodically->setChecked
    (hash.value(Settings::GENERATE_SOURCE_VIEW_PERIODICALLY).toBool());
  color = QColor
    (hash.value(Settings::LOCK_COLOR).toString().remove('&').trimmed());
  glitch_variety::assignImage(m_ui.lock_color, color);
  m_ui.lock_color->setText(color.name(QColor::HexArgb));
  m_ui.maximize_edit_windows->setChecked
    (hash.value(Settings::MAXIMIZE_EDIT_WINDOWS).toBool());
  m_ui.project_board->setCurrentIndex
    (m_ui.project_board->
     findText(hash.value(Settings::PROJECT_BOARD).toString().trimmed()));
  m_ui.project_communications_port->setCurrentIndex
    (m_ui.project_communications_port->
     findText(hash.value(Settings::PROJECT_COMMUNICATIONS_PORT).
	      toString().trimmed()));

  if(m_ui.project_communications_port->currentIndex() < 0)
    m_ui.project_communications_port->setCurrentIndex(0);

  m_ui.redo_undo_stack_size->setValue
    (hash.value(Settings::REDO_UNDO_STACK_SIZE).toInt());
  m_ui.save_periodically->setChecked
    (hash.value(Settings::SAVE_PERIODICALLY).toBool());
  color = QColor
    (hash.value(Settings::SELECTION_COLOR).toString().remove('&').trimmed());
  glitch_variety::assignImage(m_ui.selection_color, color);
  m_ui.selection_color->setText(color.name(QColor::HexArgb));
  m_ui.tab_position->setCurrentIndex
    (qBound(0,
	    hash.value(Settings::TAB_POSITION_INDEX).toInt(),
	    m_ui.tab_position->count() - 1));
  m_ui.tabbed_edit_windows->setChecked
    (hash.value(Settings::TABBED_EDIT_WINDOWS).toBool());
  color = QColor
    (hash.value(Settings::WIRE_COLOR).toString().remove('&').trimmed());
  glitch_variety::assignImage(m_ui.wire_color, color);
  m_ui.wire_color->setText(color.name(QColor::HexArgb));
  prepareKeywordColors(hash.value(Settings::KEYWORD_COLORS).toString());
  setCategoriesIconSize(hash.value(Settings::CATEGORIES_ICON_SIZE).toString());
  setName(hash.value(Settings::CANVAS_NAME).toString());
  setProjectIDE(hash.value(Settings::PROJECT_IDE).toString());
  setResult(QDialog::Accepted);
  setShowCanvasDots(hash.value(Settings::SHOW_CANVAS_DOTS).toBool());
  setShowCanvasGrids(hash.value(Settings::SHOW_CANVAS_GRIDS).toBool());
  setShowOrderIndicators(hash.value(Settings::SHOW_ORDER_INDICATORS).toBool());
  setShowPreview(hash.value(Settings::SHOW_PREVIEW).toBool());
  setViewportUpdateMode
    (QGraphicsView::
     ViewportUpdateMode(hash.value(Settings::VIEW_UPDATE_MODE).toInt()));
  setWindowTitle(tr("Glitch: Canvas Settings (%1)").arg(name()));
  setWireType(hash.value(Settings::WIRE_TYPE).toString());
  setWireWidth(hash.value(Settings::WIRE_WIDTH).toDouble());
  same ? (void) 0 : emit accepted(false);
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

void glitch_canvas_settings::setShowPreview(const bool state)
{
  m_ui.show_preview->setChecked(state);
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

void glitch_canvas_settings::showEvent(QShowEvent *event)
{
  QDialog::showEvent(event);
  m_timer.start();
}

void glitch_canvas_settings::showPage(const Pages page)
{
  m_ui.tab->setCurrentIndex(static_cast<int> (page));
}

void glitch_canvas_settings::slotCommunicationsPortRefresh(void)
{
  prepareWidgets();
}

void glitch_canvas_settings::slotResetSourceViewKeywords(void)
{
  QHash<QString, QColor> colors;

  if(m_ui.project_type->currentText() == "Arduino")
    colors = glitch_structures_arduino::defaultColors();

  m_ui.source_view_keywords->clearSelection();
  m_ui.source_view_keywords->scrollToTop();
  m_ui.source_view_keywords->setSortingEnabled(false);

  for(int i = 0; i < m_ui.source_view_keywords->rowCount(); i++)
    {
      auto item1 = m_ui.source_view_keywords->item(i, 0);
      auto item2 = m_ui.source_view_keywords->item(i, 1);

      if(item1 && item2)
	{
	  if(colors.contains(item1->text()))
	    {
	      item2->setData(Qt::DecorationRole, colors.value(item1->text()));
	      item2->setText
		(colors.value(item1->text()).name(QColor::HexArgb));
	    }
	  else
	    {
	      item2->setData(Qt::DecorationRole, QColor(Qt::black));
	      item2->setText(QColor(Qt::black).name(QColor::HexArgb));
	    }
	}
    }

  m_ui.source_view_keywords->setSortingEnabled(true);
}

void glitch_canvas_settings::slotSelectColor(void)
{
  auto button = qobject_cast<QPushButton *> (sender());

  if(!button)
    return;

  QColorDialog dialog(this);

  dialog.setCurrentColor(QColor(button->text().remove('&')));
  dialog.setOption(QColorDialog::ShowAlphaChannel, true);
  dialog.setWindowIcon(windowIcon());
#ifdef Q_OS_ANDROID
  dialog.showMaximized();
#endif
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();

      auto const color(dialog.selectedColor());

      button->setText(color.name(QColor::HexArgb));
      glitch_variety::assignImage(button, color);
    }
  else
    QApplication::processEvents();
}

void glitch_canvas_settings::slotSelectProjectIDE(void)
{
  QFileDialog dialog(this);

  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setDirectory(QStorageInfo::root().displayName());
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setLabelText(QFileDialog::Accept, tr("Select"));
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowIcon(windowIcon());
  dialog.setWindowTitle(tr("Glitch: Select Project IDE"));
  dialog.selectFile(m_ui.project_ide->text().trimmed());
#ifdef Q_OS_ANDROID
  dialog.showMaximized();
#endif
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      m_ui.project_ide->setText(dialog.selectedFiles().value(0));
      m_ui.project_ide->setToolTip(m_ui.project_ide->text().trimmed());
      m_ui.project_ide->setCursorPosition(0);
      m_ui.project_ide->selectAll();
    }
  else
    QApplication::processEvents();
}

void glitch_canvas_settings::slotSpecialCopy(void)
{
  auto clipboard = QGuiApplication::clipboard();

  if(!clipboard)
    return;
  else
    {
      auto const string(keywordColorsFromTableAsString());

      if(!string.isEmpty())
	clipboard->setText(string);
    }
}

void glitch_canvas_settings::slotSpecialPaste(void)
{
  auto clipboard = QGuiApplication::clipboard();

  if(!clipboard || !clipboard->text().startsWith("glitch-"))
    return;

  prepareKeywordColors(clipboard->text());
}

void glitch_canvas_settings::slotTimerTimeout(void)
{
  QFileInfo const fileInfo(m_ui.project_ide->text());

  m_ui.project_ide->setToolTip(m_ui.project_ide->text());

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
