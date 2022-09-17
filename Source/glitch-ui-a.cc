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
#include <QTimer>
#include <QWidgetAction>

#include "Arduino/glitch-view-arduino.h"
#include "glitch-documentation.h"
#include "glitch-graphicsview.h"
#include "glitch-misc.h"
#include "glitch-object.h"
#include "glitch-recent-diagram.h"
#include "glitch-scene.h"
#include "glitch-separated-diagram-window.h"
#include "glitch-ui.h"
#include "glitch-undo-command.h"
#include "glitch-version.h"
#include "ui_glitch-errors-dialog.h"

QMultiMap<QPair<int, int>, QPointer<glitch_object> >
glitch_ui::s_copiedObjects;

glitch_ui::glitch_ui(void):QMainWindow(nullptr)
{
  m_about.setIconPixmap
    (QPixmap(":/Logo/glitch-logo.png").scaled(QSize(256, 256),
					      Qt::KeepAspectRatio,
					      Qt::SmoothTransformation));
  m_about.setText
    (tr("<html>"
	"<b>Glitch Version %1</b><br><br>"
	"Glitch is a visual compiler and designer for Arduino.<br>"
	"Develop through diagrams.<br><br>"
	"Software for and from the margins.<br><br>"
	"Qt version %2 (runtime %3).<br><br>"
	"Please visit "
	"<a href=\"https://textbrowser.github.io/glitch\">"
	"https://textbrowser.github.io/glitch</a> for more details.").
     arg(GLITCH_VERSION_STR).
     arg(QT_VERSION_STR).
     arg(qVersion()));
  m_about.setTextFormat(Qt::RichText);
  m_about.setWindowIcon(windowIcon());
  m_about.setWindowModality(Qt::NonModal);
  m_about.setWindowTitle(tr("Glitch: About"));
  m_recentFilesFileName = glitch_misc::homePath() +
    QDir::separator() +
    "Glitch" +
    QDir::separator() +
    "glitch_recent_files.db";
  m_releaseNotes = nullptr;
  m_ui.setupUi(this);
  connect(m_ui.action_About,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotAbout);
  connect(m_ui.action_Canvas_Settings,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowCanvasSettings(void)));
  connect(m_ui.action_Clear_Copied_Widgets_Buffer,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotClearCopiedWidgetsBuffer(void)));
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
  connect(m_ui.action_Diagram_Context_Menu,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotShowDiagramContextMenu);
  connect(m_ui.action_Full_Screen,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotShowFullScreenMode);
  connect(m_ui.action_Generate_Source,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotGenerateSource);
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
  connect(m_ui.action_Release_Notes,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotShowReleaseNotes);
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
  connect(m_ui.action_Tools,
	  SIGNAL(triggered(void)),
	  SLOT(slotShowTools(void)));
  connect(m_ui.action_Undo,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotUndo(void)));
  connect(m_ui.action_User_Functions,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotShowUserFunctions);
  connect(m_ui.action_View_Tools,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotViewTools);
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
  m_ui.menu_Recent_Diagrams->setStyleSheet("QMenu {menu-scrollable: 1;}");
  m_ui.menu_Tabs->setStyleSheet("QMenu {menu-scrollable: 1;}");
  m_ui.tab->setMovable(true);
  m_ui.tab->setTabsClosable(true);
  m_ui.toolBar->setIconSize(QSize(24, 24));
  prepareActionWidgets();
  prepareIcons();
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
      if(fileName.isEmpty())
	error = tr("Empty file name.");
      else
	error = tr
	  ("The file %1 must be both readable and writable.").arg(fileName);

      return false;
    }

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  if(statusBar())
    {
      statusBar()->showMessage(tr("Opening %1...").arg(fileName));
      statusBar()->repaint();
    }

  QString connectionName("");
  QString name("");
  QString type("");
  auto ok = true;

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(fileName);

    if((ok = db.open()))
      {
	QSqlQuery query(db);

	query.setForwardOnly(true);

	if(query.exec(QString("SELECT SUBSTR(name, 1, %1), "
			      "SUBSTR(type, 1, %2) "
			      "FROM diagram").
		      arg(static_cast<int> (Limits::NAME_MAXIMUM_LENGTH)).
		      arg(static_cast<int> (Limits::TYPE_MAXIMUM_LENGTH))))
	  if(query.next())
	    {
	      name = query.value(0).toString().trimmed();

	      if(name.isEmpty())
		name = fileInfo.fileName();

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
	  auto view = newArduinoDiagram(fileName, name, true);

	  view->setUpdatesEnabled(false);

	  if((ok = view->open(fileName, error)))
	    saveRecentFile(fileName);

	  view->setUpdatesEnabled(true);
	}
      else
	ok = false;
    }

  if(statusBar())
    {
      statusBar()->showMessage("");
      statusBar()->repaint();
    }

  QApplication::restoreOverrideCursor();
  return ok;
}

glitch_view_arduino *glitch_ui::newArduinoDiagram
(const QString &fileName, const QString &n, const bool fromFile)
{
  QApplication::processEvents();

  auto name(n);

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
       glitch_common::ProjectTypes::ArduinoProject,
       this);
  else
    view = new glitch_view_arduino
      (fileName,
       name,
       fromFile,
       glitch_common::ProjectTypes::ArduinoProject,
       this);

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
	  QOverload<const glitch_tools::Operations>::
	  of(&glitch_view::toolsOperationChanged),
	  this,
	  QOverload<const glitch_tools::Operations>::
	  of(&glitch_ui::slotToolsOperationChanged));
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
  m_ui.tab->setTabToolTip(m_ui.tab->indexOf(view), "<html>" + name + "</html>");
  prepareActionWidgets();
  prepareStatusBar();
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
    {
      foreach(auto view, findChildren<glitch_view *> ())
	if(view->hasChanged())
	  {
	    QMessageBox mb(this);

	    mb.setIcon(QMessageBox::Question);
	    mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
	    mb.setText
	      (tr("At least one display has not been saved. Are you sure that "
		  "you wish to exit Glitch?"));
	    mb.setWindowIcon(windowIcon());
	    mb.setWindowModality(Qt::ApplicationModal);
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

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QMutableMapIterator<QPair<int, int>, QPointer<glitch_object> >
    it(s_copiedObjects);
#else
  QMutableMultiMapIterator<QPair<int, int>, QPointer<glitch_object> >
    it(s_copiedObjects);
#endif

  while(it.hasNext())
    {
      it.next();

      if(it.value())
	it.value()->deleteLater();

      it.remove();
    }

  auto list(view->scene()->selectedItems());

  foreach(auto i, list)
    {
      if(!i)
	continue;

      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(!proxy ||
	 proxy->isMandatory() ||
	 !(proxy->flags() & QGraphicsItem::ItemIsSelectable) ||
	 !proxy->isSelected())
	continue;

      auto widget = qobject_cast<glitch_object *> (proxy->widget());

      if(!widget)
	continue;

      auto clone = widget->clone(nullptr);

      if(!clone)
	continue;

      QPair<int, int> pair;
      auto point(widget->scenePos().toPoint());

      pair.first = point.x();
      pair.second = point.y();
      s_copiedObjects.insert(pair, clone);
    }

  QApplication::restoreOverrideCursor();
}

void glitch_ui::parseCommandLineArguments(void)
{
  QApplication::processEvents();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QString errors("");
  auto list(QApplication::arguments());
  auto showArduinoStructures = false;
  auto showTools = false;

  for(int i = 1; i < list.size(); i++)
    if(list.at(i) == "--new-arduino-diagram")
      {
	i += 1;

	auto view = newArduinoDiagram("", list.value(i), false);

	if(view)
	  {
	    saveRecentFile(QString("%1%2%3.db").
			   arg(glitch_misc::homePath()).
			   arg(QDir::separator()).
			   arg(view->name()));
	    prepareRecentFiles();

	    if(showTools)
	      view->showTools();
	  }
      }
    else if(list.at(i) == "--open-arduino-diagram")
      {
	i += 1;

	if(i >= list.size())
	  errors.append(tr("Incorrect usage of --open-arduino-diagram.\n\n"));
	else
	  m_delayedDiagrams << list.value(i);
      }
    else if(list.at(i) == "--show-arduino-structures" ||
	    list.at(i) == "--show-tools" ||
	    list.at(i) == "--version")
      {
      }
    else if(!list.at(i).trimmed().isEmpty())
      qDebug() << tr("The option ") << list.at(i) << tr(" is not supported.");

  QApplication::processEvents();

  for(int i = 1; i < list.size(); i++)
    if(list.at(i) == "--show-arduino-structures")
      {
	if(!showArduinoStructures)
	  {
	    QTimer::singleShot(1500, this, SLOT(slotShowAllStructures(void)));
	    showArduinoStructures = true;
	  }
      }
    else if(list.at(i) == "--show-tools")
      {
	if(!showTools)
	  {
	    QTimer::singleShot(1500, this, SLOT(slotShowAllTools(void)));
	    showTools = true;
	  }
      }

  QApplication::restoreOverrideCursor();

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

  if(!m_delayedDiagrams.isEmpty())
    QTimer::singleShot(500, this, SLOT(slotDelayedOpenDiagrams(void)));
}

void glitch_ui::paste(QGraphicsView *view, QUndoStack *undoStack)
{
  if(s_copiedObjects.isEmpty())
    return;

  auto scene = qobject_cast<glitch_scene *> (view->scene());

  if(!scene || !undoStack || !view)
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QMapIterator<QPair<int, int>, QPointer<glitch_object> > it(s_copiedObjects);
#else
  QMultiMapIterator<QPair<int, int>, QPointer<glitch_object> >
    it(s_copiedObjects);
#endif
  QPoint first;
  auto began = false;
  auto f = false; // First?
  auto point(view->mapToScene(view->mapFromGlobal(QCursor::pos())).toPoint());

  while(it.hasNext())
    {
      it.next();

      auto object(it.value());

      if(!object)
	continue;

      object->setCanvasSettings(scene->canvasSettings());

      if(!(object = object->clone(view)))
	continue;

      auto x = it.key().first;
      auto y = it.key().second;

      if(!f)
	{
	  first = QPoint(x, y);

	  auto proxy = scene->addObject(object);

	  if(proxy)
	    {
	      if(!began)
		{
		  began = true;
		  undoStack->beginMacro(tr("widget(s) pasted"));
		}

	      auto undoCommand = new glitch_undo_command
		(glitch_undo_command::ITEM_ADDED, proxy, scene);

	      undoStack->push(undoCommand);
	      proxy->setPos(point);
	    }
	  else
	    object->deleteLater();
	}
      else
	{
	  auto p(point);

	  p.setX(p.x() + x - first.x());

	  if(p.x() < 0)
	    p.setX(0);

	  if(y > first.y())
	    p.setY(p.y() + y - first.y());
	  else
	    p.setY(p.y() - (first.y() - y));

	  if(p.y() < 0)
	    p.setY(0);

	  auto proxy = scene->addObject(object);

	  if(proxy)
	    {
	      if(!began)
		{
		  began = true;
		  undoStack->beginMacro(tr("widget(s) pasted"));
		}

	      auto undoCommand = new glitch_undo_command
		(glitch_undo_command::ITEM_ADDED, proxy, scene);

	      undoStack->push(undoCommand);
	      proxy->setPos(p);
	    }
	  else
	    object->deleteLater();
	}

      f = true;
    }

  if(began)
    undoStack->endMacro();

  QApplication::restoreOverrideCursor();
}

void glitch_ui::prepareActionWidgets(void)
{
  m_ui.action_Clear_Copied_Widgets_Buffer->setEnabled
    (!s_copiedObjects.isEmpty());

  if(m_ui.tab->count() == 0)
    {
      m_ui.action_Canvas_Settings->setEnabled(false);
      m_ui.action_Close_Diagram->setEnabled(false);
      m_ui.action_Copy->setEnabled(false);
      m_ui.action_Delete->setEnabled(false);
      m_ui.action_Diagram_Context_Menu->setEnabled(false);
      m_ui.action_Generate_Source->setEnabled(false);
      m_ui.action_Paste->setEnabled(false);
      m_ui.action_Save_Current_Diagram->setEnabled(false);
      m_ui.action_Save_Current_Diagram_As->setEnabled(false);
      m_ui.action_Select_All->setEnabled(false);
      m_ui.action_Structures->setEnabled(false);
      m_ui.action_Structures->setText(tr("&Structures..."));
      m_ui.action_Tools->setEnabled(false);
      m_ui.action_User_Functions->setEnabled(false);
    }
  else
    {
      m_ui.action_Canvas_Settings->setEnabled(true);
      m_ui.action_Close_Diagram->setEnabled(true);
      m_ui.action_Copy->setEnabled
	(m_currentView && !m_currentView->scene()->selectedItems().empty());
      m_ui.action_Delete->setEnabled
	(m_currentView && !m_currentView->scene()->selectedItems().empty());
      m_ui.action_Diagram_Context_Menu->setEnabled(m_currentView);
      m_ui.action_Generate_Source->setEnabled(true);
      m_ui.action_Paste->setEnabled(!s_copiedObjects.isEmpty());
      m_ui.action_Save_Current_Diagram->setEnabled
	(m_currentView && m_currentView->hasChanged());
      m_ui.action_Save_Current_Diagram_As->setEnabled(true);
      m_ui.action_Select_All->setEnabled
	(m_currentView && m_currentView->scene()->items().size() > 0);
      m_ui.action_Structures->setEnabled(true);
      m_ui.action_Tools->setEnabled(true);
      m_ui.action_User_Functions->setEnabled(true);
    }

  prepareRedoUndoActions();
}

void glitch_ui::prepareIcons(void)
{
  m_ui.action_About->setIcon(QIcon::fromTheme("help-about"));
  m_ui.action_Canvas_Settings->setIcon(QIcon::fromTheme("preferences-system"));
  m_ui.action_Clear_Copied_Widgets_Buffer->setIcon
    (QIcon::fromTheme("edit-clear"));
  m_ui.action_Close_Diagram->setIcon(QIcon::fromTheme("window-close"));
  m_ui.action_Copy->setIcon(QIcon::fromTheme("edit-copy"));
  m_ui.action_Delete->setIcon(QIcon::fromTheme("edit-delete"));
  m_ui.action_Full_Screen->setIcon(QIcon::fromTheme("view-fullscreen"));
  m_ui.action_Open_Diagram->setIcon(QIcon::fromTheme("document-open"));
  m_ui.action_Paste->setIcon(QIcon::fromTheme("edit-paste"));
  m_ui.action_Quit->setIcon(QIcon::fromTheme("application-exit"));
  m_ui.action_Redo->setIcon(QIcon::fromTheme("edit-redo"));
  m_ui.action_Save_Current_Diagram->setIcon(QIcon::fromTheme("document-save"));
  m_ui.action_Save_Current_Diagram_As->setIcon
    (QIcon::fromTheme("document-save-as"));
  m_ui.action_Select_All->setIcon(QIcon::fromTheme("edit-select-all"));
  m_ui.action_Undo->setIcon(QIcon::fromTheme("edit-undo"));
  m_ui.menu_New_Diagram->setIcon(QIcon::fromTheme("document-new"));
  m_ui.menu_Recent_Diagrams->setIcon(QIcon::fromTheme("document-open-recent"));
}

void glitch_ui::prepareRecentFiles(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  QDir().mkdir(glitch_misc::homePath() + QDir::separator() + "Glitch");

  QString connectionName("");
  QStringList list;

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_recentFilesFileName);

    if(db.open())
      {
	QSqlQuery query(db);

	query.setForwardOnly(true);
	query.exec("CREATE TABLE IF NOT EXISTS glitch_recent_files ("
		   "file_name TEXT NOT NULL PRIMARY KEY)");

	if(query.exec(QString("SELECT SUBSTR(file_name, 1, %1) "
			      "FROM glitch_recent_files ORDER BY 1").
		      arg(static_cast<int> (Limits::FILE_NAME_MAXIMUM_LENGTH))))
	  while(query.next())
	    {
	      QFileInfo fileInfo(query.value(0).toString());

	      if(fileInfo.exists())
		list << fileInfo.absoluteFilePath();
	      else
		{
		  QSqlQuery query(db);

		  query.prepare
		    ("DELETE FROM glitch_recent_files WHERE file_name = ?");
		  query.addBindValue(fileInfo.absoluteFilePath());
		  query.exec();
		}
	    }
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);

  m_ui.menu_Recent_Diagrams->clear();

  for(int i = 0; i < list.size(); i++)
    {
      auto action = new glitch_recent_diagram
	(list.at(i), m_ui.menu_Recent_Diagrams);

      action->pushButton()->addAction(action);
      action->setProperty("file_name", list.at(i));
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_ui::slotOpenRecentDiagram);
      connect(action->pushButton(),
	      &QPushButton::clicked,
	      this,
	      &glitch_ui::slotForgetRecentDiagram);
      m_ui.menu_Recent_Diagrams->addAction(action);
    }

  if(!list.isEmpty())
    m_ui.menu_Recent_Diagrams->addSeparator();

  m_ui.menu_Recent_Diagrams->addAction
    (tr("Clear"), this, SLOT(slotClearRecentFiles(void)))->setIcon
    (QIcon::fromTheme("edit-clear"));
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

  m_ui.action_Save_Current_Diagram->setEnabled
    (m_currentView->canUndo() || m_currentView->hasChanged());
  m_ui.action_Undo->setEnabled(m_currentView->canUndo());

  if(m_ui.action_Undo->isEnabled())
    m_ui.action_Undo->setText(tr("Undo (%1)").arg(m_currentView->undoText()));
  else
    m_ui.action_Undo->setText(tr("Undo"));
}

void glitch_ui::prepareStatusBar(void)
{
  if(!statusBar())
    return;

  if(m_currentView)
    {
      auto operation = m_currentView->toolsOperation();

      if(operation == glitch_tools::Operations::INTELLIGENT)
	statusBar()->showMessage(tr("Intelligent Mode"));
      else if(operation == glitch_tools::Operations::SELECT)
	statusBar()->showMessage(tr("Select Mode"));
      else if(operation == glitch_tools::Operations::WIRE_CONNECT)
	statusBar()->showMessage(tr("Wire (Connect) Mode"));
      else
	statusBar()->showMessage(tr("Wire (Disconnect) Mode"));
    }
  else
    statusBar()->showMessage("");

  statusBar()->repaint();
}

void glitch_ui::prepareToolBar(void)
{
  m_ui.toolBar->clear();

  if(m_currentView)
    m_ui.toolBar->addActions(m_currentView->alignmentActions());
}

void glitch_ui::restoreSettings(void)
{
  QSettings settings;

  m_ui.action_View_Tools->setChecked
    (settings.value("main_window/view_tools", true).toBool());
  m_ui.toolBar->setVisible(m_ui.action_View_Tools->isChecked());
  restoreGeometry(settings.value("main_window/geometry").toByteArray());
  restoreState(settings.value("main_window/state").toByteArray());
}

void glitch_ui::saveRecentFile(const QString &fileName)
{
  QString connectionName("");

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_recentFilesFileName);

    if(db.open())
      {
	QSqlQuery query(db);

	query.exec("CREATE TABLE IF NOT EXISTS glitch_recent_files ("
		   "file_name TEXT NOT NULL PRIMARY KEY)");
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

  auto index = m_ui.tab->indexOf(view);

  if(view->hasChanged())
    m_ui.tab->setTabText(index, QString("%1 (*)").arg(view->name()));
  else
    m_ui.tab->setTabText(index, view->name());

  m_ui.tab->setTabToolTip(index, "<html>" + view->name() + "</html>");
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

  if(isFullScreen())
    m_ui.action_Full_Screen->setText(tr("&Normal Screen"));
  else
    m_ui.action_Full_Screen->setText(tr("&Full Screen"));

  if(!QSqlDatabase::isDriverAvailable("QSQLITE"))
    {
      QFileInfo fileInfo("qt.conf");
      QString string("");

      if(fileInfo.isReadable() && fileInfo.size() > 0)
	string = tr("The SQLite database driver is not available. "
		    "The file qt.conf is present in Glitch's "
		    "current working directory. Perhaps a conflict "
		    "exists. Please resolve!");
      else
	string = tr
	  ("The SQLite database driver is not available. Please resolve!");

      QMessageBox::critical(this, tr("Glitch: Error"), string);
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

void glitch_ui::slotAbout(void)
{
  m_about.resize(m_about.sizeHint());
  m_about.showNormal();
  m_about.activateWindow();
  m_about.raise();
}

void glitch_ui::slotAboutToShowTabsMenu(void)
{
  slotTabMoved(0, 0);

  if(m_ui.menu_Tabs->actions().isEmpty())
    m_ui.menu_Tabs->addAction(tr("Empty"))->setEnabled(false);
}

void glitch_ui::slotArduinoViewDestroyed(void)
{
  prepareActionWidgets();
}

void glitch_ui::slotClearCopiedWidgetsBuffer(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QMutableMapIterator<QPair<int, int>, QPointer<glitch_object> >
    it(s_copiedObjects);
#else
  QMutableMultiMapIterator<QPair<int, int>, QPointer<glitch_object > >
    it(s_copiedObjects);
#endif

  while(it.hasNext())
    {
      it.next();

      if(it.value())
	it.value()->deleteLater();

      it.remove();
    }

  QApplication::restoreOverrideCursor();
  prepareActionWidgets();
}

void glitch_ui::slotClearRecentFiles(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QString connectionName("");

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_recentFilesFileName);

    if(db.open())
      {
	QSqlQuery query(db);

	query.exec("DELETE FROM glitch_recent_files");
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);
  m_ui.menu_Recent_Diagrams->clear();
  m_ui.menu_Recent_Diagrams->addAction
    (tr("Clear"), this, SLOT(slotClearRecentFiles(void)));
  QApplication::restoreOverrideCursor();
}

void glitch_ui::slotCloseDiagram(int index)
{
  auto view = page(index);

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
	  mb.setWindowModality(Qt::ApplicationModal);
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
  prepareStatusBar();
}

void glitch_ui::slotCloseDiagram(void)
{
  slotCloseDiagram(m_ui.tab->currentIndex());
}

void glitch_ui::slotCopy(QGraphicsView *view)
{
  copy(view);
  prepareActionWidgets();
}

void glitch_ui::slotCopy(glitch_view *view)
{
  if(view)
    {
      copy(view->view());
      prepareActionWidgets();
    }
}

void glitch_ui::slotCopy(void)
{
  if(m_currentView)
    {
      copy(m_currentView->view());
      prepareActionWidgets();
    }
}

void glitch_ui::slotDelayedOpenDiagrams(void)
{
  QString errors("");
  auto state = false;

  foreach(const auto &i, m_delayedDiagrams)
    {
      QString error("");

      if(openDiagram(i, error))
	state = true;
      else
	errors.append
	  (tr("An error occurred while processing the file %1. (%2)\n\n").
	   arg(i).arg(error));
    }

  m_delayedDiagrams.clear();
  QApplication::processEvents();

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

  if(state)
    {
      prepareActionWidgets();
      prepareRecentFiles();
    }
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

void glitch_ui::slotForgetRecentDiagram(void)
{
  auto pushButton = qobject_cast<QPushButton *> (sender());

  if(!pushButton)
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QString connectionName("");

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_recentFilesFileName);

    if(db.open())
      {
	QSqlQuery query(db);

	query.prepare("DELETE FROM glitch_recent_files WHERE file_name = ?");
	query.addBindValue(pushButton->property("file_name").toString());

	if(query.exec())
	  m_ui.menu_Recent_Diagrams->removeAction
	    (pushButton->actions().value(0));
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);
  QApplication::restoreOverrideCursor();
}

void glitch_ui::slotGenerateSource(void)
{
  if(m_currentView)
    {
      if(statusBar())
	{
	  statusBar()->showMessage(tr("Generating source. Please be patient."));
	  statusBar()->repaint();
	}

      m_currentView->generateSource();

      if(statusBar())
	{
	  statusBar()->showMessage("");
	  statusBar()->repaint();
	}
    }
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

  auto fileName(QString("%1%2%3.db").
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
      mb.setWindowModality(Qt::ApplicationModal);
      mb.setWindowTitle(tr("Glitch: Confirmation"));

      if(mb.exec() != QMessageBox::Yes)
	{
	  QApplication::processEvents();
	  goto restart_label;
	}

      QApplication::processEvents();
    }

  QFile::remove(fileName);
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
      auto list(dialog.selectedFiles());
      auto ok = true;

      for(int i = 0; i < list.size(); i++)
	{
	  QString error("");
	  const auto &fileName(list.at(i));

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
  auto action = qobject_cast<QAction *> (sender());

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
  auto view = qobject_cast<glitch_view *> (m_ui.tab->currentWidget());

  m_ui.action_Save_Current_Diagram->setEnabled(view && view->hasChanged());
  prepareActionWidgets();
  setTabText(qobject_cast<glitch_view *> (sender()));
  setWindowTitle(qobject_cast<glitch_view *> (sender()));
}

void glitch_ui::slotPageSaved(void)
{
  m_ui.action_Save_Current_Diagram->setEnabled(false);
  prepareActionWidgets();
  setTabText(qobject_cast<glitch_view *> (sender()));
  setWindowTitle(qobject_cast<glitch_view *> (sender()));
}

void glitch_ui::slotPageSelected(int index)
{
  m_currentView = qobject_cast<glitch_view *> (m_ui.tab->widget(index));

  if(m_currentView)
    switch(m_currentView->projectType())
      {
      case glitch_common::ProjectTypes::ArduinoProject:
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
  prepareStatusBar();
  prepareToolBar();
  setTabText(m_currentView);
  setWindowTitle(m_currentView);
}

void glitch_ui::slotPaste(glitch_view *view)
{
  if(view)
    paste(view->view(), view->undoStack());
}

void glitch_ui::slotPaste(void)
{
  if(m_currentView)
    paste(m_currentView->view(), m_currentView->undoStack());

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
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
  auto view = page(m_ui.tab->currentIndex());

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
  auto view = page(m_ui.tab->currentIndex());

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
	  auto fileName(dialog.selectedFiles().value(0));

	  if(!fileName.endsWith(".db"))
	    fileName += ".db";

	  if(!view->saveAs(fileName, error))
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
    {
      m_currentView->selectAll();

      if(statusBar())
	statusBar()->showMessage
	  (tr("%1 Item(s) Selected").
	   arg(m_currentView->scene()->selectedItems().size()));
    }
}

void glitch_ui::slotSelectPage(void)
{
  auto action = qobject_cast<QAction *> (sender());

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
	(m_currentView->scene()->items().size() > 0);

      if(statusBar())
	statusBar()->showMessage
	  (tr("%1 Item(s) Selected").
	   arg(m_currentView->scene()->selectedItems().size()));
    }
  else
    {
      m_ui.action_Copy->setEnabled(false);
      m_ui.action_Delete->setEnabled(false);
      m_ui.action_Paste->setEnabled(false);
      m_ui.action_Select_All->setEnabled(false);
      prepareStatusBar();
    }
}

void glitch_ui::slotSeparate(glitch_view *view)
{
  if(!view)
    return;

  m_ui.tab->removeTab(m_ui.tab->indexOf(view));

  auto window = new glitch_separated_diagram_window(this);

  connect(window,
	  SIGNAL(copy(glitch_view *)),
	  this,
	  SLOT(slotCopy(glitch_view *)));
  connect(window,
	  SIGNAL(paste(glitch_view *)),
	  this,
	  SLOT(slotPaste(glitch_view *)));
  window->setCentralWidget(view);
  view->defaultContextMenu()->deleteLater();
  view->show();
  window->resize(view->size());

  if(view->hasChanged())
    window->setWindowTitle(tr("Glitch: %1 (*)").arg(view->name()));
  else
    window->setWindowTitle(tr("Glitch: %1").arg(view->name()));

  window->show();
  prepareActionWidgets();
  prepareStatusBar();
  setWindowTitle(nullptr);
}

void glitch_ui::slotShowAllStructures(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  foreach(auto view, findChildren<glitch_view *> ())
    if(view)
      view->showStructures();

  QApplication::restoreOverrideCursor();
}

void glitch_ui::slotShowAllTools(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  foreach(auto view, findChildren<glitch_view *> ())
    if(view)
      view->showTools();

  QApplication::restoreOverrideCursor();
}

void glitch_ui::slotShowCanvasSettings(void)
{
  auto view = qobject_cast<glitch_view *> (m_ui.tab->currentWidget());

  if(view)
    view->showCanvasSettings();
}

void glitch_ui::slotShowDiagramContextMenu(void)
{
  if(m_currentView)
    {
      auto menu = m_currentView->defaultContextMenu();

      if(menu)
	{
	  menu->update();
	  menu->raise();
	  menu->exec(mapToGlobal(QPoint(size().width() / 2, 0)));
	}
    }
}

void glitch_ui::slotShowFullScreenMode(void)
{
  if(isFullScreen())
    {
      m_ui.action_Full_Screen->setText(tr("&Full Screen"));
      showNormal();
    }
  else
    {
      m_ui.action_Full_Screen->setText(tr("&Normal Screen"));
      showFullScreen();
    }
}

void glitch_ui::slotShowReleaseNotes(void)
{
  if(!m_releaseNotes)
    m_releaseNotes = new glitch_documentation
      (QUrl("qrc:/ReleaseNotes.html"), this);

  m_releaseNotes->showNormal();
  m_releaseNotes->activateWindow();
  m_releaseNotes->raise();
}

void glitch_ui::slotShowStructures(void)
{
  auto view = qobject_cast<glitch_view *> (m_ui.tab->currentWidget());

  if(view)
    view->showStructures();
}

void glitch_ui::slotShowTools(void)
{
  auto view = qobject_cast<glitch_view *> (m_ui.tab->currentWidget());

  if(view)
    view->showTools();
}

void glitch_ui::slotShowUserFunctions(void)
{
  auto view = qobject_cast<glitch_view *> (m_ui.tab->currentWidget());

  if(view)
    view->showUserFunctions();
}

void glitch_ui::slotTabMoved(int from, int to)
{
  Q_UNUSED(from);
  Q_UNUSED(to);

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  m_ui.menu_Tabs->clear();

  for(int i = 0; i < m_ui.tab->count(); i++)
    {
      auto view = qobject_cast<glitch_view *> (m_ui.tab->widget(i));

      if(view)
	{
	  QFont font;
	  auto action = view->menuAction();

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

void glitch_ui::slotToolsOperationChanged
(const glitch_tools::Operations operation)
{
  if(m_ui.tab->currentWidget() != sender())
    return;

  if(statusBar())
    {
      if(operation == glitch_tools::Operations::INTELLIGENT)
	statusBar()->showMessage(tr("Intelligent Mode"));
      else if(operation == glitch_tools::Operations::SELECT)
	statusBar()->showMessage(tr("Select Mode"));
      else if(operation == glitch_tools::Operations::WIRE_CONNECT)
	statusBar()->showMessage(tr("Wire (Connect) Mode"));
      else
	statusBar()->showMessage(tr("Wire (Disconnect) Mode"));

      statusBar()->repaint();
    }
}

void glitch_ui::slotUndo(void)
{
  if(m_currentView)
    {
      m_currentView->undo();
      prepareRedoUndoActions();
      setTabText(m_currentView);
      setWindowTitle(m_currentView);
    }
}

void glitch_ui::slotUnite(glitch_view *view)
{
  if(!view)
    return;

  auto window = qobject_cast<QMainWindow *> (view->parentWidget());

  if(!window)
    return;

  m_ui.action_Save_Current_Diagram->setEnabled(view->hasChanged());
  m_ui.tab->addTab(view, view->menuAction()->icon(), view->name());
  m_ui.tab->setCurrentWidget(view);
  prepareActionWidgets();
  prepareStatusBar();
  setTabText(view);
  setWindowTitle(view);
  view->defaultContextMenu()->deleteLater();
  window->deleteLater();
}

void glitch_ui::slotViewTools(void)
{
  QSettings settings;

  settings.setValue
    ("main_window/view_tools", m_ui.action_View_Tools->isChecked());
  m_ui.toolBar->setVisible(m_ui.action_View_Tools->isChecked());
}
