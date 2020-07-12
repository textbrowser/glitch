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

#include <QCloseEvent>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QSqlQuery>

#include "Arduino/glitch-structures-arduino.h"
#include "Arduino/glitch-view-arduino.h"
#include "glitch-alignment.h"
#include "glitch-graphicsview.h"
#include "glitch-misc.h"
#include "glitch-object.h"
#include "glitch-scene.h"
#include "glitch-separated-diagram-window.h"
#include "glitch-ui.h"
#include "glitch-undo-command.h"
#include "ui_glitch-errors-dialog.h"

QMultiMap<QPair<int, int>, QPointer<glitch_object> >
glitch_ui::s_copiedObjects;

glitch_ui::glitch_ui(void):QMainWindow(nullptr)
{
  m_arduinoStructures = nullptr;
  m_recentFilesFileName = glitch_misc::homePath() + QDir::separator() +
    "glitch_recent_files.db";
  m_ui.setupUi(this);
  connect(m_ui.action_Alignment,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowAlignment(void)));
  connect(m_ui.action_Canvas_Settings,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowCanvasSettings(void)));
  connect(m_ui.action_Close_Diagram,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotCloseDiagram(void)));
  connect(m_ui.action_Copy,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotCopy(void)));
  connect(m_ui.action_Delete,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotDelete(void)));
  connect(m_ui.action_New_Arduino,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotNewArduinoDiagram(void)));
  connect(m_ui.action_Open_Diagram,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotOpenDiagram(void)));
  connect(m_ui.action_Paste,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotPaste(void)));
  connect(m_ui.action_Quit,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotQuit(void)));
  connect(m_ui.action_Redo,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotRedo(void)));
  connect(m_ui.action_Save_Current_Diagram,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSaveCurrentDiagram(void)));
  connect(m_ui.action_Save_Current_Diagram_As,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSaveCurrentDiagramAs(void)));
  connect(m_ui.action_Select_All,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSelectAll(void)));
  connect(m_ui.action_Structures,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowStructures(void)));
  connect(m_ui.action_Undo,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotUndo(void)));
  connect(m_ui.menu_Tabs,
	  SIGNAL(aboutToShow(void)),
	  this,
	  SLOT(slotAboutToShowTabsMenu(void)));
  connect(m_ui.tab,
	  SIGNAL(currentChanged(int)),
	  this,
	  SLOT(slotPageSelected(int)));
  connect(m_ui.tab,
	  SIGNAL(tabCloseRequested(int)),
	  this,
	  SLOT(slotCloseDiagram(int)));
  connect(m_ui.tab->tabBar(),
	  SIGNAL(tabMoved(int, int)),
	  this,
	  SLOT(slotTabMoved(int, int)),
	  Qt::QueuedConnection);
  m_ui.action_Copy->setEnabled(false);
  m_ui.action_Delete->setEnabled(false);
  m_ui.action_Paste->setEnabled(false);
  m_ui.action_Select_All->setEnabled(false);
  m_ui.menu_Tabs->setStyleSheet("QMenu {menu-scrollable: 1;}");
  m_ui.tab->setMovable(true);
  m_ui.tab->setTabsClosable(true);
  prepareActionWidgets();
  prepareRecentFiles();
  prepareToolBar();
}

glitch_ui::~glitch_ui()
{
}

QMultiMap<QPair<int, int>, QPointer<glitch_object> > glitch_ui::
copiedObjects(void)
{
  return s_copiedObjects;
}

bool glitch_ui::openDiagram(const QString &fileName, QString &error)
{
  QFileInfo fileInfo(fileName);

  if(!fileInfo.isReadable() && !fileInfo.isWritable())
    {
      error = tr("The file must be both readable and writable.");
      return false;
    }

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QString connectionName("");
  QString name("");
  QString type("");
  bool ok = true;

  {
    QSqlDatabase db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(fileName);

    if((ok = db.open()))
      {
	QSqlQuery query(db);

	query.setForwardOnly(true);

	if(query.exec("SELECT name, type FROM diagram"))
	  if(query.next())
	    {
	      name = query.value(0).toString().trimmed();
	      type = query.value(1).toString().trimmed();
	    }

	if(name.isEmpty() || type != "ArduinoProject")
	  {
	    if(name.isEmpty())
	      error = tr("Empty diagram name.");
	    else
	      error = tr("Expecting a diagram type of ArduinoProject.");

	    ok = false;
	  }
      }
    else
      error = tr("Unable to open %1.").arg(fileName);

    db.close();
  }

  glitch_common::discardDatabase(connectionName);

  if(ok)
    {
      if(type == "ArduinoProject")
	{
	  auto *view = newArduinoDiagram(fileName, name, true);

	  if((ok = view->open(fileName, error)))
	    saveRecentFile(fileName);
	}
      else
	ok = false;
    }

  QApplication::restoreOverrideCursor();
  return ok;
}

glitch_view_arduino *glitch_ui::newArduinoDiagram
(const QString &fileName, const QString &n, const bool fromFile)
{
  QApplication::processEvents();

  QString name(n);

  name.remove("(*)");
  name.replace(" ", "-");

  if(name.isEmpty())
    name = "Arduino-Diagram";

  glitch_view_arduino *view = nullptr;

  if(fileName.isEmpty())
    view = new glitch_view_arduino
      (glitch_misc::homePath() + QDir::separator() + name + ".db",
       name,
       fromFile,
       glitch_common::ArduinoProject,
       this);
  else
    view = new glitch_view_arduino
      (fileName, name, fromFile, glitch_common::ArduinoProject, this);

  connect(view,
	  SIGNAL(changed(void)),
	  this,
	  SLOT(slotPageChanged(void)));
  connect(view,
	  SIGNAL(copy(QGraphicsView *)),
	  this,
	  SLOT(slotCopy(QGraphicsView *)));
  connect(view,
	  SIGNAL(destroyed(void)),
	  this,
	  SLOT(slotArduinoViewDestroyed(void)));
  connect(view,
	  SIGNAL(saved(void)),
	  this,
	  SLOT(slotPageSaved(void)));
  connect(view,
	  SIGNAL(selectionChanged(void)),
	  this,
	  SLOT(slotSelectionChanged(void)));
  connect(view,
	  SIGNAL(separate(glitch_view *)),
	  this,
	  SLOT(slotSeparate(glitch_view *)));
  connect(view,
	  SIGNAL(showStructures(void)),
	  this,
	  SLOT(slotShowStructures(void)));
  connect(view,
	  SIGNAL(unite(glitch_view *)),
	  this,
	  SLOT(slotUnite(glitch_view *)));
  connect(view->menuAction(),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSelectPage(void)));
  m_ui.tab->addTab(view, view->menuAction()->icon(), name);
  m_ui.tab->setCurrentWidget(view);
  m_ui.tab->setTabToolTip(m_ui.tab->indexOf(view), name);
  prepareActionWidgets();
  setWindowTitle(view);

  if(!fromFile)
    view->save();

  return view;
}

glitch_view *glitch_ui::page(const int index)
{
  return qobject_cast<glitch_view *> (m_ui.tab->widget(index));
}

void glitch_ui::closeEvent(QCloseEvent *event)
{
  /*
  ** Detect modified diagrams.
  */

  if(event)
    foreach(auto *view, findChildren<glitch_view *> ())
      if(view->hasChanged())
	{
	  QMessageBox mb(this);

	  mb.setIcon(QMessageBox::Question);
	  mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
	  mb.setText
	    (tr("At least one display has not been saved. Are you sure that "
		"you wish to exit Glitch?"));
	  mb.setWindowIcon(windowIcon());
	  mb.setWindowModality(Qt::WindowModal);
	  mb.setWindowTitle(tr("Glitch: Confirmation"));

	  if(mb.exec() == QMessageBox::Yes)
	    {
	      QApplication::processEvents();
	      break;
	    }
	  else
	    {
	      QApplication::processEvents();
	      event->ignore();
	      return;
	    }
	}

  saveSettings();
  QMainWindow::closeEvent(event);
  QApplication::exit();
}

void glitch_ui::copy(QGraphicsView *view)
{
  if(!view || !view->scene())
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QMutableMapIterator<QPair<int, int>, QPointer<glitch_object> >
    it(s_copiedObjects);

  while(it.hasNext())
    {
      it.next();

      if(it.value())
	it.value()->deleteLater();

      it.remove();
    }

  QList<QGraphicsItem *> list(view->scene()->selectedItems());

  for(auto i : list)
    {
      if(!i)
	continue;

      auto *proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(!proxy ||
	 proxy->isMandatory() ||
	 !(proxy->flags() & QGraphicsItem::ItemIsSelectable) ||
	 !proxy->isSelected())
	continue;

      auto *widget = qobject_cast<glitch_object *> (proxy->widget());

      if(!widget)
	continue;

      auto *clone = widget->clone(nullptr);

      if(!clone)
	continue;

      QPair<int, int> pair;
      QPoint point(widget->scenePos().toPoint());

      pair.first = point.x();
      pair.second = point.y();
      s_copiedObjects.insert(pair, clone);
    }

  m_ui.action_Paste->setEnabled(!s_copiedObjects.empty());
  QApplication::restoreOverrideCursor();
}

void glitch_ui::parseCommandLineArguments(void)
{
  QString errors("");
  QStringList list(QApplication::arguments());

  for(int i = 1; i < list.size(); i++)
    if(list.at(i) == "--new-arduino-diagram")
      {
	i += 1;

	auto *view = newArduinoDiagram("", list.value(i), false);

	if(view)
	  {
	    saveRecentFile(QString("%1%2%3.db").
			   arg(glitch_misc::homePath()).
			   arg(QDir::separator()).
			   arg(view->name()));
	    prepareRecentFiles();
	  }
      }
    else if(list.at(i) == "--open-arduino-diagram")
      {
	i += 1;

	QString error("");

	if(i >= list.size())
	  errors.append(tr("Incorrect usage of --open-arduino-diagram."));
	else if(openDiagram(list.value(i), error))
	  {
	    prepareActionWidgets();
	    prepareRecentFiles();
	  }
	else
	  errors.append
	    (tr("An error occurred while processing "
		"the file %1. (%2)\n\n").arg(list.value(i)).arg(error));
      }
    else if(list.at(i) == "--version")
      {
      }
    else
      qDebug() << "The option " << list.at(i) << " is not supported.";

  if(!errors.isEmpty())
    {
      QDialog dialog(this);
      Ui_glitch_errors_dialog ui;

      ui.setupUi(&dialog);
      ui.label->setText(tr("The following errors occurred."));
      ui.text->setPlainText(errors.trimmed());
      QApplication::processEvents();
      dialog.exec();
    }
}

void glitch_ui::paste(QGraphicsView *view)
{
  if(!view || !view->scene())
    return;
}

void glitch_ui::prepareActionWidgets(void)
{
  if(m_ui.tab->count() == 0)
    {
      m_ui.action_Alignment->setEnabled(false);
      m_ui.action_Canvas_Settings->setEnabled(false);
      m_ui.action_Close_Diagram->setEnabled(false);
      m_ui.action_Copy->setEnabled(false);
      m_ui.action_Delete->setEnabled(false);
      m_ui.action_Paste->setEnabled(false);
      m_ui.action_Save_Current_Diagram->setEnabled(false);
      m_ui.action_Save_Current_Diagram_As->setEnabled(false);
      m_ui.action_Select_All->setEnabled(false);
      m_ui.action_Structures->setEnabled(false);
      m_ui.action_Structures->setText(tr("&Structures..."));
    }
  else
    {
      m_ui.action_Alignment->setEnabled(true);
      m_ui.action_Canvas_Settings->setEnabled(true);
      m_ui.action_Close_Diagram->setEnabled(true);
      m_ui.action_Copy->setEnabled
	(m_currentView && !m_currentView->scene()->selectedItems().empty());
      m_ui.action_Delete->setEnabled
	(m_currentView && !m_currentView->scene()->selectedItems().empty());
      m_ui.action_Paste->setEnabled(!s_copiedObjects.isEmpty());
      m_ui.action_Save_Current_Diagram->setEnabled
	(m_currentView && m_currentView->hasChanged());
      m_ui.action_Save_Current_Diagram_As->setEnabled(true);
      m_ui.action_Select_All->setEnabled
	(m_currentView && m_currentView->scene()->items().size() > 2);
      m_ui.action_Structures->setEnabled(true);
    }

  prepareRedoUndoActions();
}

void glitch_ui::prepareRecentFiles(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QString connectionName("");
  QStringList list;

  {
    QSqlDatabase db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_recentFilesFileName);

    if(db.open())
      {
	QSqlQuery query(db);

	query.exec("CREATE TABLE IF NOT EXISTS glitch_recent_files ("
		   "file_name TEXT NOT NULL PRIMARY KEY)");

	if(query.exec("SELECT file_name FROM glitch_recent_files ORDER BY 1"))
	  while(query.next())
	    list << query.value(0).toString();
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);

  m_ui.menu_Recent_Files->clear();

  for(int i = 0; i < list.size(); i++)
    {
      auto *action = m_ui.menu_Recent_Files->addAction(list.at(i));

      action->setProperty("file_name", list.at(i));
      connect(action,
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotOpenRecentDiagram(void)));
    }

  if(!list.isEmpty())
    m_ui.menu_Recent_Files->addSeparator();

  m_ui.menu_Recent_Files->addAction
    (tr("Clear"), this, SLOT(slotClearRecentFiles(void)));
  QApplication::restoreOverrideCursor();
}

void glitch_ui::prepareRedoUndoActions(void)
{
  if(!m_currentView)
    {
      m_ui.action_Redo->setEnabled(false);
      m_ui.action_Redo->setText(tr("Redo"));
      m_ui.action_Undo->setEnabled(false);
      m_ui.action_Undo->setText(tr("Undo"));
      return;
    }

  m_ui.action_Redo->setEnabled(m_currentView->canRedo());

  if(m_ui.action_Redo->isEnabled())
    m_ui.action_Redo->setText(tr("Redo (%1)").arg(m_currentView->redoText()));
  else
    m_ui.action_Redo->setText(tr("Redo"));

  m_ui.action_Undo->setEnabled(m_currentView->canUndo());

  if(m_ui.action_Undo->isEnabled())
    m_ui.action_Undo->setText(tr("Undo (%1)").arg(m_currentView->undoText()));
  else
    m_ui.action_Undo->setText(tr("Undo"));
}

void glitch_ui::prepareToolBar(void)
{
  m_ui.toolBar->clear();
  m_ui.toolBar->addAction(m_ui.action_Alignment);
  m_ui.toolBar->addAction(m_ui.action_Structures);

  if(m_currentView)
    for(int i = 0; i < m_currentView->defaultActions().size(); i++)
      m_ui.toolBar->addAction(m_currentView->defaultActions().at(i));
}

void glitch_ui::restoreSettings(void)
{
  QSettings settings;

  restoreGeometry(settings.value("main_window/geometry").toByteArray());
  restoreState(settings.value("main_window/state").toByteArray());
}

void glitch_ui::saveRecentFile(const QString &fileName)
{
  QString connectionName("");

  {
    QSqlDatabase db(glitch_common::sqliteDatabase());

    db.setDatabaseName(m_recentFilesFileName);

    if(db.open())
      {
	QSqlQuery query(db);

	query.prepare
	  ("INSERT OR REPLACE INTO glitch_recent_files (file_name) VALUES (?)");
	query.addBindValue(fileName);
	query.exec();
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);
}

void glitch_ui::saveSettings(void)
{
  QSettings settings;

  settings.setValue("main_window/geometry", saveGeometry());
  settings.setValue("main_window/state", saveState());
}

void glitch_ui::setTabText(glitch_view *view)
{
  if(!view)
    return;

  int index = m_ui.tab->indexOf(view);

  if(view->hasChanged())
    m_ui.tab->setTabText(index, QString("%1 (*)").arg(view->name()));
  else
    m_ui.tab->setTabText(index, view->name());
}

void glitch_ui::setWindowTitle(glitch_view *view)
{
  if(m_ui.tab->currentWidget() == view && view)
    {
      if(view->hasChanged())
	QMainWindow::setWindowTitle(tr("Glitch: %1 (*)").arg(view->name()));
      else
	QMainWindow::setWindowTitle(tr("Glitch: %1").arg(view->name()));
    }
  else
    QMainWindow::setWindowTitle(tr("Glitch"));
}

void glitch_ui::show(void)
{
  restoreSettings();
  QMainWindow::show();

  /*
  ** Some desktop managers are strange.
  */

  repaint();
  QApplication::processEvents();

  if(!QSqlDatabase::isDriverAvailable("QSQLITE"))
    {
      QFileInfo fileInfo("qt.conf");
      QString str("");

      if(fileInfo.isReadable() && fileInfo.size() > 0)
	str = tr("The SQLite database driver is not available. "
		 "The file qt.conf is present in Glitch's "
		 "current working directory. Perhaps a conflict "
		 "exists. Please resolve!");
      else
	str = tr
	  ("The SQLite database driver is not available. Please resolve!");

      QMessageBox::critical(this, tr("Glitch: Error"), str);
      QApplication::processEvents();
    }

  QFileInfo fileInfo(glitch_misc::homePath());

  if(!fileInfo.isReadable() || !fileInfo.isWritable())
    {
      QMessageBox::critical
	(this,
	 tr("Glitch: Error"),
	 tr("Glitch's home directory %1 must be readable and writable.").
	 arg(glitch_misc::homePath()));
      QApplication::processEvents();
    }

  parseCommandLineArguments();
}

void glitch_ui::slotAboutToShowTabsMenu(void)
{
  slotTabMoved(0, 0);

  if(m_ui.menu_Tabs->actions().isEmpty())
    m_ui.menu_Tabs->addAction(tr("Empty"))->setEnabled(false);
}

void glitch_ui::slotArduinoViewDestroyed(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  if(m_arduinoStructures)
    if(findChildren<glitch_view_arduino *> ().isEmpty())
      m_arduinoStructures->deleteLater();

  QApplication::restoreOverrideCursor();
  prepareActionWidgets();
}

void glitch_ui::slotClearRecentFiles(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QString connectionName("");

  {
    QSqlDatabase db(glitch_common::sqliteDatabase());

    db.setDatabaseName(m_recentFilesFileName);

    if(db.open())
      {
	QSqlQuery query(db);

	query.exec("DELETE FROM glitch_recent_files");
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);
  m_ui.menu_Recent_Files->clear();
  m_ui.menu_Recent_Files->addAction
    (tr("Clear"), this, SLOT(slotClearRecentFiles(void)));
  QApplication::restoreOverrideCursor();
}

void glitch_ui::slotCloseDiagram(int index)
{
  auto *view = page(index);

  if(view)
    {
      if(view->hasChanged())
	{
	  m_ui.tab->setCurrentIndex(index);

	  QMessageBox mb(this);

	  mb.setIcon(QMessageBox::Question);
	  mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
	  mb.setText
	    (tr("The current view has been modified. Are you sure that "
		"you wish to close it?"));
	  mb.setWindowIcon(windowIcon());
	  mb.setWindowModality(Qt::WindowModal);
	  mb.setWindowTitle(tr("Glitch: Confirmation"));

	  if(mb.exec() != QMessageBox::Yes)
	    {
	      QApplication::processEvents();
	      return;
	    }

	  QApplication::processEvents();
	}

      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      view->deleteLater();
      QApplication::restoreOverrideCursor();
    }

  m_ui.tab->removeTab(index);
  prepareActionWidgets();
}

void glitch_ui::slotCloseDiagram(void)
{
  slotCloseDiagram(m_ui.tab->currentIndex());
}

void glitch_ui::slotCopy(QGraphicsView *view)
{
  copy(view);
}

void glitch_ui::slotCopy(void)
{
  if(m_currentView)
    copy(m_currentView->view());
}

void glitch_ui::slotDelete(void)
{
  if(!m_currentView)
    return;

  m_currentView->deleteItems();
  m_ui.action_Undo->setEnabled(m_currentView->canUndo());

  if(m_ui.action_Undo->isEnabled())
    m_ui.action_Undo->setText(tr("Undo (%1)").arg(m_currentView->undoText()));
  else
    m_ui.action_Undo->setText(tr("Undo"));
}

void glitch_ui::slotNewArduinoDiagram(void)
{
  QInputDialog dialog(this);
  QLabel *label = nullptr;
  QString name("");

 restart_label:
  dialog.setLabelText
    (tr("Please specify a project name. "
	"A database file having the provided name will be created in "
	"the %1 directory.").arg(glitch_misc::homePath()));
  dialog.setTextValue("Arduino-Diagram");
  dialog.setWindowIcon(windowIcon());
  dialog.setWindowTitle(tr("Glitch: Arduino Project Name"));

  if((label = dialog.findChild<QLabel *> ()))
    label->setWordWrap(true);

  QApplication::processEvents();

  if(dialog.exec() != QDialog::Accepted)
    return;
  else
    name = dialog.textValue().trimmed();

  if(name.isEmpty())
    name = "Arduino-Diagram";

  QString fileName(QString("%1%2%3.db").
		   arg(glitch_misc::homePath()).
		   arg(QDir::separator()).
		   arg(name));

  if(QFile::exists(fileName))
    {
      QMessageBox mb(this);

      mb.setIcon(QMessageBox::Question);
      mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
      mb.setText(tr("The file %1 already exists. Overwrite?").arg(fileName));
      mb.setWindowIcon(windowIcon());
      mb.setWindowModality(Qt::WindowModal);
      mb.setWindowTitle(tr("Glitch: Confirmation"));

      if(mb.exec() != QMessageBox::Yes)
	{
	  QApplication::processEvents();
	  goto restart_label;
	}

      QApplication::processEvents();
    }

  newArduinoDiagram("", name, false);
  saveRecentFile(fileName);
  prepareRecentFiles();
}

void glitch_ui::slotOpenDiagram(void)
{
  QFileDialog dialog(this);

  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setDirectory(glitch_misc::homePath());
  dialog.setFileMode(QFileDialog::ExistingFiles);
  dialog.setLabelText(QFileDialog::Accept, tr("Select"));
  dialog.setWindowIcon(windowIcon());
  dialog.setWindowTitle(tr("Glitch: Open Diagram"));
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      dialog.close();

      QString errors("");
      QStringList list(dialog.selectedFiles());
      bool ok = true;

      for(int i = 0; i < list.size(); i++)
	{
	  QString error("");
	  const QString &fileName(list.at(i));

	  if(openDiagram(fileName, error))
	    ok = true;
	  else
	    errors.append
	      (tr("An error occurred while processing "
		  "the file %1. (%2)\n\n").arg(fileName).arg(error));
	}

      if(ok)
	{
	  prepareActionWidgets();
	  prepareRecentFiles();
	}

      if(!errors.isEmpty())
	{
	  QDialog dialog(this);
	  Ui_glitch_errors_dialog ui;

	  ui.setupUi(&dialog);
	  ui.label->setText(tr("The following errors occurred."));
	  ui.text->setPlainText(errors.trimmed());
	  QApplication::processEvents();
	  dialog.exec();
	}
    }
}

void glitch_ui::slotOpenRecentDiagram(void)
{
  auto *action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  QString error("");

  if(openDiagram(action->property("file_name").toString(), error))
    prepareActionWidgets();

  if(!error.isEmpty())
    {
      QDialog dialog(this);
      Ui_glitch_errors_dialog ui;

      ui.setupUi(&dialog);
      ui.label->setText(tr("The following errors occurred."));
      ui.text->setPlainText(error.trimmed());
      QApplication::processEvents();
      dialog.exec();
    }
}

void glitch_ui::slotPageChanged(void)
{
  auto *view = qobject_cast<glitch_view *> (m_ui.tab->currentWidget());

  m_ui.action_Save_Current_Diagram->setEnabled(view && view->hasChanged());
  prepareActionWidgets();
  setTabText(qobject_cast<glitch_view *> (sender()));
  setWindowTitle(qobject_cast<glitch_view *> (sender()));
}

void glitch_ui::slotPageSaved(void)
{
  m_ui.action_Save_Current_Diagram->setEnabled(false);
  setTabText(qobject_cast<glitch_view *> (sender()));
  setWindowTitle(qobject_cast<glitch_view *> (sender()));
}

void glitch_ui::slotPageSelected(int index)
{
  m_currentView = qobject_cast<glitch_view *> (m_ui.tab->widget(index));

  if(m_currentView)
    switch(m_currentView->projectType())
      {
      case glitch_common::ArduinoProject:
	{
	  m_ui.action_Structures->setText(tr("Arduino &Structures..."));
	  break;
	}
      default:
	{
	  m_ui.action_Structures->setText(tr("&Structures..."));
	  break;
	}
      }

  prepareActionWidgets();
  prepareToolBar();
  setWindowTitle(m_currentView);
}

void glitch_ui::slotPaste(void)
{
  if(!m_currentView || s_copiedObjects.isEmpty())
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QMapIterator<QPair<int, int>, QPointer<glitch_object> > it(s_copiedObjects);
  QPoint first;
  QPoint point
    (m_currentView->view()->mapToScene(m_currentView->view()->
				       mapFromGlobal(QCursor::pos())).
     toPoint());
  bool f = false;

  m_currentView->beginMacro(tr("widget(s) pasted"));

  while(it.hasNext())
    {
      it.next();

      QPointer<glitch_object> object(it.value());

      if(!object)
	continue;
      else if(!(object = object->clone(m_currentView)))
	continue;

      int x = it.key().first;
      int y = it.key().second;

      if(!f)
	{
	  first = QPoint(x, y);

	  auto *proxy = m_currentView->scene()->addObject(object);

	  if(proxy)
	    {
	      auto *undoCommand = new glitch_undo_command
		(glitch_undo_command::ITEM_ADDED,
		 proxy,
		 m_currentView->scene());

	      m_currentView->push(undoCommand);
	      proxy->setPos(point);
	    }
	  else
	    object->deleteLater();
	}
      else
	{
	  QPoint p(point);

	  p.setX(p.x() + x - first.x());

	  if(y > first.y())
	    p.setY(p.y() + y - first.y());
	  else
	    p.setY(p.y() - (first.y() - y));

	  glitch_proxy_widget *proxy =
	    m_currentView->scene()->addObject(object);

	  if(proxy)
	    {
	      auto *undoCommand = new glitch_undo_command
		(glitch_undo_command::ITEM_ADDED,
		 proxy,
		 m_currentView->scene());

	      m_currentView->push(undoCommand);
	      proxy->setPos(p);
	    }
	  else
	    object->deleteLater();
	}

      f = true;
    }

  m_currentView->endMacro();
  prepareRedoUndoActions();
  QApplication::restoreOverrideCursor();
}

void glitch_ui::slotQuit(void)
{
  close();
}

void glitch_ui::slotRedo(void)
{
  if(m_currentView)
    {
      m_currentView->redo();
      prepareRedoUndoActions();
    }
}

void glitch_ui::slotSaveCurrentDiagram(void)
{
  auto *view = page(m_ui.tab->currentIndex());

  if(view)
    {
      QString error("");

      if(!view->save(error))
	glitch_misc::showErrorDialog
	  (tr("Unable to save %1 (%2).").arg(view->name()).arg(error), this);
      else
	{
	  m_ui.action_Save_Current_Diagram->setEnabled(false);
	  setTabText(view);
	  setWindowTitle(view);
	}
    }
}

void glitch_ui::slotSaveCurrentDiagramAs(void)
{
  auto *view = page(m_ui.tab->currentIndex());

  if(view)
    {
      QFileDialog dialog(this, tr("Glitch: Save Current Diagram As"));

      dialog.setAcceptMode(QFileDialog::AcceptSave);
      dialog.setDirectory(glitch_misc::homePath());
      dialog.setFileMode(QFileDialog::AnyFile);
      dialog.setNameFilter("Glitch Files (*.db)");
      dialog.setOption(QFileDialog::DontConfirmOverwrite, false);
      dialog.setWindowIcon(windowIcon());
      QApplication::processEvents();

      if(dialog.exec() == QDialog::Accepted)
	{
	  QString error("");

	  if(!view->saveAs(dialog.selectedFiles().value(0), error))
	    glitch_misc::showErrorDialog
	      (tr("Unable to save %1 (%2).").arg(view->name()).arg(error),
	       this);
	  else
	    {
	      m_ui.action_Save_Current_Diagram->setEnabled(false);
	      setTabText(view);
	      setWindowTitle(view);
	    }
	}
    }
}

void glitch_ui::slotSelectAll(void)
{
  if(m_currentView)
    m_currentView->selectAll();
}

void glitch_ui::slotSelectPage(void)
{
  auto *action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  m_ui.tab->setCurrentWidget(action->parentWidget());
  setWindowTitle(qobject_cast<glitch_view *> (m_ui.tab->currentWidget()));
}

void glitch_ui::slotSelectionChanged(void)
{
  if(m_currentView)
    {
      m_ui.action_Copy->setEnabled
	(!m_currentView->scene()->selectedItems().empty());
      m_ui.action_Delete->setEnabled
	(!m_currentView->scene()->selectedItems().empty());
      m_ui.action_Paste->setEnabled(!s_copiedObjects.isEmpty());
      m_ui.action_Select_All->setEnabled
	(m_currentView->scene()->items().size() > 2);
    }
  else
    {
      m_ui.action_Copy->setEnabled(false);
      m_ui.action_Delete->setEnabled(false);
      m_ui.action_Paste->setEnabled(false);
      m_ui.action_Select_All->setEnabled(false);
    }
}

void glitch_ui::slotSeparate(glitch_view *view)
{
  if(!view)
    return;

  QMainWindow::setWindowTitle(tr("Glitch"));
  m_ui.tab->removeTab(m_ui.tab->indexOf(view));

  auto *window = new glitch_separated_diagram_window(this);

  window->setCentralWidget(view);
  view->show();
  window->resize(view->size());

  if(view->hasChanged())
    window->setWindowTitle(tr("Glitch: %1 (*)").arg(view->name()));
  else
    window->setWindowTitle(tr("Glitch: %1").arg(view->name()));

  window->show();
  prepareActionWidgets();
}

void glitch_ui::slotShowAlignment(void)
{
  auto *view = qobject_cast<glitch_view *> (m_ui.tab->currentWidget());

  if(view)
    view->showAlignment();
}

void glitch_ui::slotShowCanvasSettings(void)
{
  auto *view = qobject_cast<glitch_view *> (m_ui.tab->currentWidget());

  if(view)
    view->showCanvasSettings();
}

void glitch_ui::slotShowStructures(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  if(!findChildren<glitch_view_arduino *> ().isEmpty())
    {
      QApplication::restoreOverrideCursor();

      if(!m_arduinoStructures)
	m_arduinoStructures = new glitch_structures_arduino(this);

      m_arduinoStructures->showNormal();
      m_arduinoStructures->activateWindow();
      m_arduinoStructures->raise();
    }
  else
    QApplication::restoreOverrideCursor();
}

void glitch_ui::slotTabMoved(int from, int to)
{
  Q_UNUSED(from);
  Q_UNUSED(to);

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  m_ui.menu_Tabs->clear();

  for(int i = 0; i < m_ui.tab->count(); i++)
    {
      auto *view = qobject_cast<glitch_view *> (m_ui.tab->widget(i));

      if(view)
	{
	  QFont font;
	  auto *action = view->menuAction();

	  font = action->font();

	  if(i == m_ui.tab->currentIndex())
	    font.setBold(true);
	  else
	    font.setBold(false);

	  action->setFont(font);
	  m_ui.menu_Tabs->addAction(action);
	}
    }

  QApplication::restoreOverrideCursor();
}

void glitch_ui::slotUndo(void)
{
  if(m_currentView)
    {
      m_currentView->undo();
      prepareRedoUndoActions();
    }
}

void glitch_ui::slotUnite(glitch_view *view)
{
  if(!view)
    return;

  auto *window = qobject_cast<QMainWindow *> (view->parentWidget());

  if(!window)
    return;

  m_ui.action_Save_Current_Diagram->setEnabled(view->hasChanged());
  m_ui.tab->addTab(view, view->menuAction()->icon(), view->name());
  m_ui.tab->setCurrentWidget(view);
  m_ui.tab->setTabToolTip(m_ui.tab->indexOf(view), view->name());
  prepareActionWidgets();
  setTabText(view);
  setWindowTitle(view);
  window->deleteLater();
}
