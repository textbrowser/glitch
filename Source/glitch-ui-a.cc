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

#include <QActionGroup>
#include <QCloseEvent>
#include <QDir>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QSettings>
#include <QShortcut>
#include <QSqlError>
#include <QSqlQuery>
#include <QToolButton>
#include <QWidgetAction>

#include "Arduino/glitch-object-function-arduino.h"
#include "Arduino/glitch-view-arduino.h"
#include "glitch-documentation.h"
#include "glitch-graphicsview.h"
#include "glitch-preferences.h"
#include "glitch-recent-diagram.h"
#include "glitch-recent-diagrams-view.h"
#include "glitch-separated-diagram-window.h"
#include "glitch-swifty.h"
#include "glitch-ui.h"
#include "glitch-undo-command.h"
#include "glitch-variety.h"
#include "glitch-version.h"
#include "ui_glitch-errors-dialog.h"

QFont glitch_ui::s_defaultApplicationFont;
QPointer<QMainWindow> glitch_ui::s_mainWindow;
QSet<glitch_object *> glitch_ui::s_copiedObjectsSet;
QTranslator *glitch_ui::s_translator1 = nullptr;
QTranslator *glitch_ui::s_translator2 = nullptr;
glitch_aware_multi_map<QPair<int, int>, QPointer<glitch_object> >
glitch_ui::s_copiedObjects;
static const char * const COMPILED_ON = __DATE__ " @ " __TIME__;

glitch_ui::glitch_ui(void):QMainWindow(nullptr)
{
  qRegisterMetaType<QVectorQPairQImageQString> ("QVectorQPairQImageQString");
  m_about.setIconPixmap
    (QPixmap(":/Logo/glitch-logo.png").scaled(QSize(256, 256),
					      Qt::KeepAspectRatio,
					      Qt::SmoothTransformation));
  m_about.setStandardButtons(QMessageBox::Close);
  m_about.setText
    (QString(tr("<html>"
		"<b>Glitch Version %1</b><br><br>"
		"<b>G</b>raphical<br>"
		"<b>L</b>ightweight<br>"
		"<b>I</b>nteractive<br>"
		"<b>T</b>ool<br>"
		"<b>C</b>ompiler<br>"
		"<b>H</b>ybrid<br><br>"
		"Glitch is a visual compiler and designer for ") +
	     "Arduino" +
	     tr(".<br>"
		"Develop through block diagrams.<br>"
		"Software for and from the margins.<br><br>"
		"Made with love by textbrowser.<br><br>"
		"Architecture: %2.<br>"
		"Compiled On: %3.<br>"
		"Product: %4.<br>"
		"Qt version %5 (runtime version %6).<br><br>"
		"Please visit "
		"<a href=\"https://textbrowser.github.io/glitch\">"
		"https://textbrowser.github.io/glitch</a> for more details.")).
     arg(GLITCH_VERSION_STRING).
     arg(QSysInfo::currentCpuArchitecture()).
     arg(COMPILED_ON).
     arg(QSysInfo::prettyProductName()).
     arg(QT_VERSION_STR).
     arg(qVersion()));
  m_about.setTextFormat(Qt::RichText);
  m_about.setWindowIcon(windowIcon());
  m_about.setWindowModality(Qt::NonModal);
  m_about.setWindowTitle(tr("Glitch: About"));
  m_arduino = nullptr;
  m_generateSource = false;
  m_ideProcessIndicator = new QProgressBar(this);
  m_ideProcessIndicator->setMaximumWidth(100);
  m_ideProcessIndicator->setRange(0, 0);
  m_ideProcessIndicator->setToolTip(tr("Active IDE Process"));
  m_preferences = new glitch_preferences(this);
  m_recentDiagramsView = new glitch_recent_diagrams_view(this);
  m_recentFilesFileName = glitch_variety::homePath() +
    QDir::separator() +
    "Glitch" +
    QDir::separator() +
    "glitch_recent_files.db";
  m_releaseNotes = nullptr;
  m_statusBarTimer.start(500);
  m_swifty = new swifty
    (GLITCH_VERSION_STRING,
     "#define GLITCH_VERSION_STRING_LTS",
     QUrl::fromUserInput(GLITCH_VERSION_FILE_URL),
     this);
  m_ui.setupUi(this);
  connect(&m_statusBarTimer,
	  &QTimer::timeout,
	  this,
	  &glitch_ui::slotStatusBarTimerTimeout);
  connect
    (&s_copiedObjects,
     &glitch_aware_multi_map<QPair<int, int>, QPointer<glitch_object> >::
     cleared,
     this,
     &glitch_ui::slotCopiedObjectsChanged,
     Qt::QueuedConnection);
  connect
    (&s_copiedObjects,
     &glitch_aware_multi_map<QPair<int, int>, QPointer<glitch_object> >::
     inserted,
     this,
     &glitch_ui::slotCopiedObjectsChanged,
     Qt::QueuedConnection);
  connect(m_preferences,
	  &glitch_preferences::accept,
	  this,
	  &glitch_ui::slotPreferencesAccepted);
  connect(m_recentDiagramsView,
	  SIGNAL(openDiagram(const QString &)),
	  this,
	  SLOT(slotOpenDiagram(const QString &)));
  connect(m_recentDiagramsView,
	  SIGNAL(openDiagram(void)),
	  this,
	  SLOT(slotOpenDiagram(void)));
  connect(m_recentDiagramsView,
	  SIGNAL(remove(const QString &)),
	  this,
	  SLOT(slotForgetRecentDiagram(const QString &)));
  connect(m_recentDiagramsView->menuAction(),
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotSelectPage);
  connect(m_swifty,
	  SIGNAL(different(const QString &)),
	  this,
	  SLOT(slotSwifty(void)));
  connect(m_swifty,
	  &swifty::same,
	  this,
	  &glitch_ui::slotSwifty);
  connect(m_ui.action_About,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotAbout);
  connect(m_ui.action_Arduino_Documentation,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotShowArduinoDocumentation);
  connect(m_ui.action_Canvas_Settings,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotShowCanvasSettings);
  connect(m_ui.action_Clear_Copied_Widgets_Buffer,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotClearCopiedWidgetsBuffer);
  connect(m_ui.action_Close_All_Diagrams,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotCloseAllDiagrams);
  connect(m_ui.action_Close_Diagram,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotCloseDiagram(void)));
  connect(m_ui.action_Copy,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotCopy(void)));
  connect(m_ui.action_Delete,
	  &QAction::triggered,
	  this,
	  &::glitch_ui::slotDelete);
  connect(m_ui.action_Diagram_Context_Menu,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotShowDiagramContextMenu);
  connect(m_ui.action_Find,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotFind);
  connect(m_ui.action_Full_Screen,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotShowFullScreenMode);
  connect(m_ui.action_Generate_Source,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotGenerateSource);
  connect(m_ui.action_Generate_Source_Clipboard,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotGenerateSourceClipboard);
  connect(m_ui.action_Generate_Source_View,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotGenerateSourceView);
  connect(m_ui.action_Glitch_Preferences,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotShowPreferences);
  connect(m_ui.action_New_Arduino,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotNewArduinoDiagram);
  connect(m_ui.action_Open_Diagram,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotOpenDiagram(void)));
  connect(m_ui.action_Paste,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotPaste(void)));
  connect(m_ui.action_Print,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotPrint);
  connect(m_ui.action_Quit,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotQuit);
  connect(m_ui.action_Redo,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotRedo);
  connect(m_ui.action_Redo_Undo_Stack,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotShowRedoUndoStack);
  connect(m_ui.action_Release_Notes,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotShowReleaseNotes);
  connect(m_ui.action_Save_As_PNG,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotSaveAsPNG);
  connect(m_ui.action_Save_Current_Diagram,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotSaveCurrentDiagram);
  connect(m_ui.action_Save_Current_Diagram_As,
	  &QAction::triggered,
	  this,
	  &::glitch_ui::slotSaveCurrentDiagramAs);
  connect(m_ui.action_Select_All,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotSelectAll);
  connect(m_ui.action_Separate_Current_Canvas,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSeparate(void)));
  connect(m_ui.action_Serial_Port_Window,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotShowSerialPortWindow);
  connect(m_ui.action_Show_Project_IDE,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotShowProjectIDE);
  connect(m_ui.action_Terminate,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotIDETerminate);
  connect(m_ui.action_Tools,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotShowTools);
  connect(m_ui.action_Undo,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotUndo);
  connect(m_ui.action_Unite_All_Canvases,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotUniteAllDiagrams);
  connect(m_ui.action_Upload,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotIDEUpload);
  connect(m_ui.action_User_Functions,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotShowUserFunctions);
  connect(m_ui.action_Verify,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotIDEVerify);
  connect(m_ui.action_View_Tab_Bar,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotViewTabBar);
  connect(m_ui.action_View_Tool_Bars,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotViewToolBars);
  connect(m_ui.action_Zoom_In,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotZoom);
  connect(m_ui.action_Zoom_Out,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotZoom);
  connect(m_ui.action_Zoom_Reset,
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotZoom);
  connect(m_ui.menu_Project,
	  &QMenu::aboutToShow,
	  this,
	  &glitch_ui::slotAboutToShowProjectMenu);
  connect(m_ui.menu_Recent_Diagrams,
	  &QMenu::aboutToShow,
	  this,
	  &glitch_ui::slotAboutToShowRecentDiagrams);
  connect(m_ui.menu_Recent_Diagrams,
	  SIGNAL(hovered(QAction *)),
	  this,
	  SLOT(slotRecentDiagramHovered(QAction *)));
  connect(m_ui.menu_Tabs,
	  &QMenu::aboutToShow,
	  this,
	  &glitch_ui::slotAboutToShowTabsMenu);
  connect(m_ui.tab,
	  SIGNAL(currentChanged(int)),
	  this,
	  SLOT(slotPageSelected(int)));
  connect(m_ui.tab,
	  SIGNAL(separate(QWidget *)),
	  this,
	  SLOT(slotSeparate(QWidget *)),
	  Qt::QueuedConnection);
  connect(m_ui.tab,
	  SIGNAL(tabCloseRequested(int)),
	  this,
	  SLOT(slotCloseDiagram(int)));
  connect(m_ui.tab->tabBar(),
	  SIGNAL(tabMoved(int, int)),
	  this,
	  SLOT(slotTabMoved(int, int)),
	  Qt::QueuedConnection);
#ifdef Q_OS_ANDROID
  copyExamplesForAndroid();
#endif
#ifndef GLITCH_PDF_SUPPORTED
  m_ui.action_Arduino_Documentation->setEnabled(false);
#endif
  m_ui.action_Copy->setEnabled(false);
  m_ui.action_Delete->setEnabled(false);
#ifdef Q_OS_ANDROID
  m_ui.action_Full_Screen->setEnabled(false);
#endif
  m_ui.action_Paste->setEnabled(false);
  m_ui.action_Select_All->setEnabled(false);
#ifndef GLITCH_SERIAL_PORT_SUPPORTED
  m_ui.action_Serial_Port_Window->setEnabled(false);
#endif
  m_ui.edit_toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
  m_ui.edit_toolbar->setIconSize(QSize(24, 24));
  m_ui.file_toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
  m_ui.file_toolbar->setIconSize(QSize(24, 24));
  m_ui.menu_Recent_Diagrams->setStyleSheet("QMenu {menu-scrollable: 1;}");
  m_ui.menu_Tabs->addAction(tr("Empty"))->setEnabled(false);
  m_ui.menu_Tabs->setStyleSheet("QMenu {menu-scrollable: 1;}");
  m_ui.miscellaneous_toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
  m_ui.miscellaneous_toolbar->setIconSize(QSize(24, 24));
  m_ui.project_toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
  m_ui.project_toolbar->setIconSize(QSize(24, 24));
  m_ui.tab->addTab(m_recentDiagramsView, tr("Recent Diagrams"));
  m_ui.tab->setMovable(true);
  m_ui.tab->setTabsClosable(true);
  m_ui.tools_toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
  m_ui.tools_toolbar->setIconSize(QSize(24, 24));
  m_ui.zoom_toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
  m_ui.zoom_toolbar->setIconSize(QSize(24, 24));
  menuBar()->setContextMenuPolicy(Qt::PreventContextMenu);
  prepareActionWidgets();
  prepareIcons();
  prepareRecentDiagramsView();
  prepareTab();
  prepareToolBars();
  slotPreferencesAccepted();
  statusBar() ?
    statusBar()->addPermanentWidget(m_ideProcessIndicator) : (void) 0;
  m_ideProcessIndicator->setVisible(false);

  if(QSettings().value("preferences/download_version_information", false).
     toBool())
    QTimer::singleShot(5000, m_swifty, &swifty::slot_download);
}

glitch_ui::~glitch_ui()
{
  m_statusBarTimer.stop();
}

bool glitch_ui::openDiagram(const QString &fileName, QString &error)
{
  QFileInfo const fileInfo(fileName);

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
  showStatusBarMessage(tr("Opening %1...").arg(fileName));

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
	  {
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
		  error = tr("Expecting a diagram type of ") +
		    "ArduinoProject" +
		    tr(".");

		ok = false;
	      }
	  }
	else
	  {
	    error = tr("Unable (%1) to execute a database query.").arg
	      (query.lastError().text().toLower().trimmed());
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
	  QElapsedTimer timer;

	  timer.start();

	  auto view = newArduinoDiagram(fileName, name, true);

	  if(view)
	    {
	      QApplication::processEvents();
	      setUpdatesEnabled(false);

	      if((ok = view->open(fileName, error)))
		saveRecentFile(fileName);

	      setUpdatesEnabled(true);
	      QApplication::processEvents();
	    }

	  showStatusBarMessage
	    (tr("%1 opened in %2 second(s).").
	     arg(fileName).arg(timer.elapsed() / 1000.0),
	     5000);
	}
      else
	ok = false;
    }
  else
    showStatusBarMessage("");

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
      (glitch_variety::homePath() + QDir::separator() + name + ".db",
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

  connect(m_preferences,
	  &glitch_preferences::accept,
	  view,
	  &glitch_view::slotPreferencesAccepted);
  connect(view,
	  &glitch_view_arduino::changed,
	  this,
	  &glitch_ui::slotPageChanged);
  connect(view,
	  &glitch_view_arduino::ideProcessFinished,
	  this,
	  &glitch_ui::slotIDEProcessFinished);
  connect(view,
	  &glitch_view_arduino::ideProcessStarted,
	  this,
	  &glitch_ui::slotIDEProcessStarted);
  connect(view,
	  SIGNAL(copy(QGraphicsView *)),
	  this,
	  SLOT(slotCopy(QGraphicsView *)));
  connect(view,
	  SIGNAL(paste(glitch_view *)),
	  this,
	  SLOT(slotPaste(glitch_view *)));
  connect(view,
	  &QAction::destroyed,
	  this,
	  &glitch_ui::slotArduinoViewDestroyed,
	  Qt::QueuedConnection); /*
				 ** Prevent abnormal termination
				 ** as prepareActionWidgets() is
				 ** issued after m_ui is destroyed.
				 */
  connect(view,
	  &glitch_view_arduino::saved,
	  this,
	  &glitch_ui::slotPageSaved);
  connect(view,
	  &glitch_view_arduino::selectionChanged,
	  this,
	  &glitch_ui::slotSelectionChanged,
	  Qt::QueuedConnection); /*
				 ** Prevent abnormal termination
				 ** after m_ui is destroyed.
				 */
  connect(view,
	  &glitch_view_arduino::zoomReset,
	  this,
	  &glitch_ui::slotPrepareStatusBar);
  connect(view,
	  SIGNAL(separate(glitch_view *)),
	  this,
	  SLOT(slotSeparate(glitch_view *)));
  connect(view,
	  SIGNAL(toolsOperationChanged(const glitch_tools::Operations)),
	  this,
	  SLOT(slotToolsOperationChanged(const glitch_tools::Operations)));
  connect(view,
	  SIGNAL(unite(glitch_view *)),
	  this,
	  SLOT(slotUnite(glitch_view *)));
  connect(view->menuAction(),
	  &QAction::triggered,
	  this,
	  &glitch_ui::slotSelectPage);
  m_ui.tab->addTab(view, view->menuAction()->icon(), name);
  m_ui.tab->setCurrentWidget(view);
  m_ui.tab->setTabToolTip
    (m_ui.tab->indexOf(view), "<html>" + name + "</html>");
  prepareActionWidgets();
  prepareStatusBar();
  prepareTab();
  prepareTabShortcuts();
  setWindowTitle(view);
  slotAboutToShowTabsMenu();

  if(!fromFile)
    view->save();

  return view;
}

glitch_view *glitch_ui::page(const int index)
{
  return qobject_cast<glitch_view *> (m_ui.tab->widget(index));
}

void glitch_ui::clearCopiedObjects(void)
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

  s_copiedObjects.clear();
  s_copiedObjectsSet.clear();
  QApplication::restoreOverrideCursor();
}

void glitch_ui::closeEvent(QCloseEvent *event)
{
  /*
  ** Detect modified diagrams.
  */

  if(event)
    {
      foreach(auto view, findChildren<glitch_view *> ())
	if(view && view->hasChanged())
	  {
	    QMessageBox mb(this);

	    mb.setIcon(QMessageBox::Question);
	    mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
	    mb.setText
	      (tr("At least one diagram has not been saved. Are you sure that "
		  "you wish to exit Glitch?"));
	    mb.setWindowIcon(windowIcon());
	    mb.setWindowModality(Qt::ApplicationModal);
	    mb.setWindowTitle(tr("Glitch: Confirmation"));
	    m_ui.tab->setCurrentWidget(view);
	    QApplication::processEvents();

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

void glitch_ui::copy(QGraphicsView *view, const bool selected)
{
  clearCopiedObjects();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QList<QPointF> points;
  auto const list(copySelected(view, &points, false, selected));

  for(int i = 0; i < list.size(); i++)
    {
      auto const point(points.value(i));

      s_copiedObjects.insert
	(QPair<int, int> (point.toPoint().x(), point.toPoint().y()),
	 list.at(i));
    }

  QApplication::restoreOverrideCursor();
}

void glitch_ui::copy(glitch_object *object)
{
  if(!object)
    return;

  clearCopiedObjects();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  auto const point(object->scenePos());
  glitch_object *clone = nullptr;

  if(qobject_cast<glitch_object_function_arduino *> (object))
    {
      /*
      ** Clone the real function.
      */

      auto proxy = object->proxy();

      object = qobject_cast<glitch_object_function_arduino *>
	(object)->parentFunction();

      if(!object && proxy)
	object = proxy->object();

      if(object)
	{
	  clone = object->clone(nullptr);
	  clone->setOriginalPosition(point);
	}
    }
  else
    clone = object->clone(nullptr);

  if(clone)
    s_copiedObjects.insert
      (QPair<int, int> (point.toPoint().x(), point.toPoint().y()), clone);

  QApplication::restoreOverrideCursor();
}

void glitch_ui::parseCommandLineArguments(void)
{
  QApplication::processEvents();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  auto const list(QApplication::arguments());
  auto showSerialPortWindow = false;
  auto showTools = false;

  for(int i = 1; i < list.size(); i++)
    if(list.at(i) == "--full-screen")
      {
      }
    else if(list.at(i) == "--generate-source")
      {
      }
    else if(list.at(i) == "--new-arduino-diagram")
      {
	i += 1;

	auto view = newArduinoDiagram("", list.value(i), false);

	if(view)
	  {
	    saveRecentFile(QString("%1%2%3.db").
			   arg(glitch_variety::homePath()).
			   arg(QDir::separator()).
			   arg(view->name()));

	    if(showTools)
	      view->showTools();
	  }
      }
    else if(list.at(i) == "--open-arduino-diagrams")
      {
	while(true)
	  if(i + 1 < list.size() && list.value(i + 1).endsWith(".db"))
	    {
	      i += 1;
	      m_delayedDiagrams << QFileInfo(list.value(i)).absoluteFilePath();
	    }
	  else
	    break;
      }
    else if(list.at(i) == "--show-serial-port-window" ||
	    list.at(i) == "--show-tools" ||
	    list.at(i) == "--version")
      {
      }
    else if(!list.at(i).trimmed().isEmpty())
      {
	QFileInfo const fileInfo(list.at(i).trimmed());

	if(fileInfo.isReadable() && fileInfo.suffix() == "db")
	  m_delayedDiagrams << fileInfo.absoluteFilePath();
	else
	  qDebug() << tr("The option ")
		   << list.at(i)
		   << tr(" is not supported.");
      }

  QApplication::processEvents();

  for(int i = 1; i < list.size(); i++)
    if(list.at(i) == "--generate-source")
      m_generateSource = true;
    else if(list.at(i) == "--show-serial-port-window")
      {
	if(!showSerialPortWindow)
	  {
	    QTimer::singleShot
	      (1500, this, &glitch_ui::slotShowSerialPortWindow);
	    showSerialPortWindow = true;
	  }
      }
    else if(list.at(i) == "--show-tools")
      {
	if(!showTools)
	  {
	    QTimer::singleShot(1500, this, &glitch_ui::slotShowAllTools);
	    showTools = true;
	  }
      }

  QApplication::restoreOverrideCursor();

  if(!m_delayedDiagrams.isEmpty())
    QTimer::singleShot(500, this, &glitch_ui::slotDelayedOpenDiagrams);
}

void glitch_ui::paste(QGraphicsView *view, QUndoStack *undoStack)
{
  if(s_copiedObjects.isEmpty() || undoStack == nullptr || view == nullptr)
    return;

  auto scene = qobject_cast<glitch_scene *> (view->scene());

  if(!scene)
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

  if(point.x() < 0)
    point.setX(0);

  if(point.y() < 0)
    point.setY(0);

  while(it.hasNext())
    {
      it.next();

      auto object(it.value());

      if(!object)
	continue;

      object->setCanvasSettings(scene->canvasSettings());

      if(!(object = object->clone(view)))
	continue;
      else
	{
	  connect(scene,
		  SIGNAL(wireObjects(void)),
		  object,
		  SLOT(slotWireObjects(void)),
		  Qt::UniqueConnection);
	  s_copiedObjectsSet << object;
	}

      auto const x = it.key().first;
      auto const y = it.key().second;

      if(!f)
	{
	  first = QPoint(x, y);

	  auto proxy = scene->addObject(object);

	  if(proxy)
	    {
	      object->afterPaste();

	      if(!began)
		{
		  began = true;
		  undoStack->beginMacro(tr("widget(s) pasted"));
		}

	      auto undoCommand = new glitch_undo_command
		(glitch_undo_command::Types::ITEM_ADDED, proxy, scene);

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
	      object->afterPaste();

	      if(!began)
		{
		  began = true;
		  undoStack->beginMacro(tr("widget(s) pasted"));
		}

	      auto undoCommand = new glitch_undo_command
		(glitch_undo_command::Types::ITEM_ADDED, proxy, scene);

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
  m_ui.action_Unite_All_Canvases->setEnabled(m_separatedWindows.size() > 0);

  if(m_ui.tab->count() <= 1)
    {
      m_statusBarTimer.stop();
      m_ui.action_Canvas_Settings->setEnabled(false);
      m_ui.action_Close_All_Diagrams->setEnabled(false);
      m_ui.action_Close_Diagram->setEnabled(false);
      m_ui.action_Copy->setEnabled(false);
      m_ui.action_Delete->setEnabled(false);
      m_ui.action_Diagram_Context_Menu->setEnabled(false);
      m_ui.action_Find->setEnabled(false);
      m_ui.action_Generate_Source->setEnabled(false);
      m_ui.action_Generate_Source_Clipboard->setEnabled(false);
      m_ui.action_Generate_Source_View->setEnabled(false);
      m_ui.action_Paste->setEnabled(false);
      m_ui.action_Print->setEnabled(false);
      m_ui.action_Redo_Undo_Stack->setEnabled(false);
      m_ui.action_Save_As_PNG->setEnabled(false);
      m_ui.action_Save_Current_Diagram->setEnabled(false);
      m_ui.action_Save_Current_Diagram_As->setEnabled(false);
      m_ui.action_Select_All->setEnabled(false);
      m_ui.action_Separate_Current_Canvas->setEnabled(false);
      m_ui.action_Show_Project_IDE->setEnabled(false);
      m_ui.action_Tools->setEnabled(false);
      m_ui.action_Terminate->setEnabled(false);
      m_ui.action_Upload->setEnabled(false);
      m_ui.action_User_Functions->setEnabled(false);
      m_ui.action_Verify->setEnabled(false);
      m_ui.action_Zoom_In->setEnabled(false);
      m_ui.action_Zoom_Out->setEnabled(false);
      m_ui.action_Zoom_Reset->setEnabled(false);
      m_ui.menu_Zoom->setEnabled(false);
    }
  else
    {
      m_statusBarTimer.start();
      m_ui.action_Canvas_Settings->setEnabled(m_currentView);
      m_ui.action_Close_All_Diagrams->setEnabled(m_ui.tab->count() > 1);
      m_ui.action_Close_Diagram->setEnabled(m_currentView);
      m_ui.action_Copy->setEnabled
	(m_currentView && m_currentView->selectedCanCopyOrDelete());
      m_ui.action_Delete->setEnabled
	(m_currentView && m_currentView->selectedCanCopyOrDelete());
      m_ui.action_Diagram_Context_Menu->setEnabled(m_currentView);
      m_ui.action_Find->setEnabled(m_currentView);
      m_ui.action_Generate_Source->setEnabled(m_currentView);
      m_ui.action_Generate_Source_Clipboard->setEnabled(m_currentView);
      m_ui.action_Generate_Source_View->setEnabled(m_currentView);
      m_ui.action_Paste->setEnabled(!s_copiedObjects.isEmpty());
      m_ui.action_Print->setEnabled(m_currentView);
      m_ui.action_Redo_Undo_Stack->setEnabled(m_currentView);
      m_ui.action_Save_As_PNG->setEnabled(m_currentView);
      m_ui.action_Save_Current_Diagram->setEnabled(m_currentView);
      m_ui.action_Save_Current_Diagram_As->setEnabled(m_currentView);
      m_ui.action_Select_All->setEnabled
	(m_currentView &&
	 m_currentView->scene() &&
	 m_currentView->scene()->items().size() > 0);
#ifdef Q_OS_ANDROID
      m_ui.action_Separate_Current_Canvas->setEnabled(false);
      m_ui.action_Separate_Current_Canvas->setText
	(tr("Separate Current Canvas (Disabled On Android)"));
#else
      m_ui.action_Separate_Current_Canvas->setEnabled(m_currentView);
#endif
      m_ui.action_Show_Project_IDE->setEnabled(m_currentView);
      m_ui.action_Terminate->setEnabled(m_currentView);
      m_ui.action_Tools->setEnabled(m_currentView);
      m_ui.action_Upload->setEnabled(m_currentView);
      m_ui.action_User_Functions->setEnabled(m_currentView);
      m_ui.action_Verify->setEnabled(m_currentView);
      m_ui.action_Zoom_In->setEnabled(m_currentView);
      m_ui.action_Zoom_Out->setEnabled(m_currentView);
      m_ui.action_Zoom_Reset->setEnabled(m_currentView);
      m_ui.menu_Zoom->setEnabled(m_currentView);
    }

  prepareRedoUndoActions();
}

void glitch_ui::prepareFonts(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QFont font;
  QSettings settings;
  auto const string1
    (settings.value("preferences/application_font").
     toString().remove('&').trimmed());
  auto const string2
    (settings.value("preferences/font_hinting").toString().trimmed());

  if(string1.isEmpty() || !font.fromString(string1))
    font = QApplication::font();

  if(string2 == tr("Full"))
    font.setHintingPreference(QFont::PreferFullHinting);
  else if(string2 == tr("None"))
    font.setHintingPreference(QFont::PreferNoHinting);
  else
    font.setHintingPreference(QFont::PreferDefaultHinting);

  font.setStyleStrategy
    (static_cast<QFont::StyleStrategy> (QFont::PreferAntialias |
					QFont::PreferQuality));
  QApplication::setFont(font);

  foreach(auto widget, QApplication::allWidgets())
    if(widget)
      {
	auto f(font);

	f.setBold(widget->font().bold());
	widget->setFont(f);
	widget->updateGeometry();
      }

  QApplication::restoreOverrideCursor();
}

void glitch_ui::prepareIcons(void)
{
  m_ui.action_About->setIcon(QIcon(":/about.png"));
  m_ui.action_Arduino_Documentation->setIcon
    (QIcon(":/Logo/glitch-arduino-logo.png"));
  m_ui.action_Canvas_Settings->setIcon(QIcon(":/settings.png"));
  m_ui.action_Clear_Copied_Widgets_Buffer->setIcon(QIcon(":/clear.png"));
  m_ui.action_Close_All_Diagrams->setIcon(QIcon(":/close.png"));
  m_ui.action_Close_Diagram->setIcon(QIcon(":/close.png"));
  m_ui.action_Copy->setIcon(QIcon(":/copy.png"));
  m_ui.action_Delete->setIcon(QIcon(":/delete.png"));
  m_ui.action_Find->setIcon(QIcon(":/find.png"));
  m_ui.action_Full_Screen->setIcon(QIcon(":/fullscreen.png"));
  m_ui.action_Generate_Source_View->setIcon(QIcon(":/source.png"));
  m_ui.action_Glitch_Preferences->setIcon(QIcon(":/settings.png"));
  m_ui.action_Open_Diagram->setIcon(QIcon(":/open.png"));
  m_ui.action_Paste->setIcon(QIcon(":/paste.png"));
  m_ui.action_Print->setIcon(QIcon(":/print.png"));
  m_ui.action_Quit->setIcon(QIcon(":/quit.png"));
  m_ui.action_Redo->setIcon(QIcon(":/redo.png"));
  m_ui.action_Save_As_PNG->setIcon(QIcon(":/png.png"));
  m_ui.action_Save_Current_Diagram->setIcon(QIcon(":/save.png"));
  m_ui.action_Save_Current_Diagram_As->setIcon(QIcon(":/save-as.png"));
  m_ui.action_Select_All->setIcon(QIcon(":/select-all.png"));
  m_ui.action_Serial_Port_Window->setIcon(QIcon(":/Arduino/serial.png"));
  m_ui.action_Terminate->setIcon(QIcon(":/quit.png"));
  m_ui.action_Undo->setIcon(QIcon(":/undo.png"));
  m_ui.action_Verify->setIcon(QIcon(":/verify.png"));
  m_ui.action_Upload->setIcon(QIcon(":/upload.png"));
  m_ui.action_Zoom_In->setIcon(QIcon(":/zoom-in.png"));
  m_ui.action_Zoom_Out->setIcon(QIcon(":/zoom-out.png"));
  m_ui.action_Zoom_Reset->setIcon(QIcon(":/zoom-reset.png"));
  m_ui.menu_New_Diagram->setIcon(QIcon(":/new-document.png"));
  m_ui.menu_Recent_Diagrams->setIcon(QIcon(":/recent.png"));
  m_ui.tab->setTabIcon(0, QIcon(":/recent.png"));
}

void glitch_ui::prepareRecentFiles(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  QDir().mkdir(glitch_variety::homePath() + QDir::separator() + "Glitch");

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
	query.exec("CREATE TABLE IF NOT EXISTS glitch_recent_files "
		   "(file_name TEXT NOT NULL PRIMARY KEY, image TEXT)");

	if(query.exec(QString("SELECT SUBSTR(file_name, 1, %1) "
			      "FROM glitch_recent_files ORDER BY 1").
		      arg(static_cast<int> (Limits::FILE_NAME_MAXIMUM_LENGTH))))
	  while(query.next())
	    {
	      QFileInfo const fileInfo(query.value(0).toString());

	      list << fileInfo.absoluteFilePath();
	    }
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);

  m_ui.menu_Recent_Diagrams->clear();

  for(int i = 0; i < list.size(); i++)
    {
#if defined(Q_OS_ANDROID) || defined(Q_OS_MACOS)
      auto action = m_ui.menu_Recent_Diagrams->addAction(list.at(i));

      action->setProperty("file-name", list.at(i));
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_ui::slotOpenRecentDiagram);
#else
      auto action = new glitch_recent_diagram
	(list.at(i), m_ui.menu_Recent_Diagrams);

      action->setProperty("file-name", list.at(i));
      connect(action,
	      SIGNAL(clicked(void)),
	      this,
	      SLOT(slotForgetRecentDiagram(void)));
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_ui::slotOpenRecentDiagram,
	      Qt::QueuedConnection); // Prevent MacOS fault.
      m_ui.menu_Recent_Diagrams->addAction(action);

      if(action->label())
	{
	  QFileInfo const fileInfo(action->fileName());

	  if(!fileInfo.exists() || !fileInfo.isReadable())
	    {
	      action->label()->setStyleSheet
		("QLabel {color: rgb(240, 128, 128);}");

	      if(!fileInfo.exists())
		action->label()->setToolTip(tr("File does not exist."));
	      else
		action->label()->setToolTip(tr("File is not readable."));
	    }
	  else
	    {
	      action->label()->setStyleSheet("");
	      action->label()->setToolTip("");
	    }
	}
#endif
    }

  if(!list.isEmpty())
    m_ui.menu_Recent_Diagrams->addSeparator();

  m_ui.menu_Recent_Diagrams->addAction
    (tr("Clear"), this, SLOT(slotClearRecentFiles(void)))->setIcon
    (QIcon(":/clear.png"));
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

void glitch_ui::prepareStatusBar(void)
{
  if(m_currentView)
    {
      QString text("");

      switch(m_currentView->toolsOperation())
	{
	case glitch_tools::Operations::INTELLIGENT:
	  {
	    text = tr("Connections Mode: Intelligent");
	    break;
	  }
	case glitch_tools::Operations::SELECT:
	  {
	    text = tr("Connections Mode: Select");
	    break;
	  }
	case glitch_tools::Operations::WIRE_CONNECT:
	  {
	    text = tr("Connections Mode: Wire (Connect)");
	    break;
	  }
	default:
	  {
	    text = tr("Connections Mode: Wire (Disconnect)");
	    break;
	  }
	}

      text.append
	(tr(" | Scaling Factor: %1").
	 arg(m_currentView->scalingFactor(), 0, 'f', 2));
      showStatusBarMessage(text);
    }
  else
    showStatusBarMessage("");
}

void glitch_ui::prepareTabShortcuts(void)
{
  foreach(auto shortcut, m_tabWidgetShortcuts)
    if(shortcut)
      shortcut->deleteLater();

  m_tabWidgetShortcuts.clear();

  for(int i = 0; i < qMin(10, m_ui.tab->count()); i++)
    {
      auto widget = m_ui.tab->widget(i);

      if(!widget)
	continue;

      QShortcut *shortcut = nullptr;

      if(i == 9)
	{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	  shortcut = new QShortcut
	    (Qt::AltModifier + Qt::Key_0,
	     this,
	     SLOT(slotTabWidgetShortcutActivated(void)));
#else
	  shortcut = new QShortcut
	    (Qt::AltModifier | Qt::Key_0,
	     this,
	     SLOT(slotTabWidgetShortcutActivated(void)));
#endif
	  m_tabWidgetShortcuts << shortcut;
	}
      else
	{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	  shortcut = new QShortcut
	    (Qt::AltModifier + Qt::Key(Qt::Key_1 + i),
	     this,
	     SLOT(slotTabWidgetShortcutActivated(void)));
#else
	  shortcut = new QShortcut
	    (Qt::AltModifier | Qt::Key(Qt::Key_1 + i),
	     this,
	     SLOT(slotTabWidgetShortcutActivated(void)));
#endif
	  m_tabWidgetShortcuts << shortcut;

	  if(i == m_ui.tab->count() - 1)
	    {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	      shortcut = new QShortcut
		(Qt::AltModifier + Qt::Key_0,
		 this,
		 SLOT(slotTabWidgetShortcutActivated(void)));
#else
	      shortcut = new QShortcut
		(Qt::AltModifier | Qt::Key_0,
		 this,
		 SLOT(slotTabWidgetShortcutActivated(void)));
#endif
	      m_tabWidgetShortcuts << shortcut;
	    }
	}
    }
}

void glitch_ui::prepareToolBars(void)
{
  QTimer::singleShot(100, this, &glitch_ui::slotDelayedToolBarPreparation);

  if(m_ui.edit_toolbar->actions().isEmpty())
    {
      m_ui.edit_toolbar->addAction(m_ui.action_Undo);
      m_ui.edit_toolbar->addAction(m_ui.action_Redo);
      m_ui.edit_toolbar->addSeparator();
      m_ui.edit_toolbar->addAction(m_ui.action_Copy);
      m_ui.edit_toolbar->addAction(m_ui.action_Paste);
      m_ui.edit_toolbar->addSeparator();
      m_ui.edit_toolbar->addAction(m_ui.action_Delete);
      m_ui.edit_toolbar->addAction(m_ui.action_Select_All);
      m_ui.edit_toolbar->addSeparator();
      m_ui.edit_toolbar->addAction(m_ui.action_Find);
      m_ui.edit_toolbar->addSeparator();
      m_ui.edit_toolbar->addAction(m_ui.action_Canvas_Settings);
    }

  if(m_ui.file_toolbar->actions().isEmpty())
    {
      m_ui.file_toolbar->addAction(m_ui.action_New_Arduino);
      m_ui.file_toolbar->addAction(m_ui.action_Open_Diagram);
      m_ui.file_toolbar->addSeparator();
      m_ui.file_toolbar->addAction(m_ui.action_Save_Current_Diagram);
      m_ui.file_toolbar->addSeparator();
      m_ui.file_toolbar->addAction(m_ui.action_Quit);
    }

  if(m_ui.project_toolbar->actions().isEmpty())
    {
      m_ui.project_toolbar->addAction(m_ui.action_Terminate);
      m_ui.project_toolbar->addAction(m_ui.action_Upload);
      m_ui.project_toolbar->addAction(m_ui.action_Verify);
    }

  if(m_ui.zoom_toolbar->actions().isEmpty())
    {
      m_ui.zoom_toolbar->addAction(m_ui.action_Zoom_In);
      m_ui.zoom_toolbar->addAction(m_ui.action_Zoom_Out);
      m_ui.zoom_toolbar->addAction(m_ui.action_Zoom_Reset);
    }

  QTimer::singleShot(100, this, SLOT(repaint(void)));
}

void glitch_ui::restoreSettings(void)
{
  QSettings settings;

  restoreState(settings.value("main_window/state").toByteArray());
  m_ui.action_View_Tab_Bar->setChecked
    (settings.value("preferences/tab_bar", true).toBool());
  m_ui.action_View_Tool_Bars->setChecked
    (settings.value("main_window/view_tools", true).toBool());
  m_ui.edit_toolbar->setVisible(m_ui.action_View_Tool_Bars->isChecked());
  m_ui.file_toolbar->setVisible(m_ui.action_View_Tool_Bars->isChecked());
  m_ui.miscellaneous_toolbar->setVisible
    (m_currentView && m_ui.action_View_Tool_Bars->isChecked());
  m_ui.project_toolbar->setVisible(m_ui.action_View_Tool_Bars->isChecked());
  m_ui.tab->tabBar()->setVisible(m_ui.action_View_Tab_Bar->isChecked());
  m_ui.tools_toolbar->setVisible
    (m_currentView && m_ui.action_View_Tool_Bars->isChecked());
  m_ui.zoom_toolbar->setVisible(m_ui.action_View_Tool_Bars->isChecked());

  if(!restoreGeometry(settings.value("main_window/geometry").toByteArray()))
    showMaximized();
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

	query.exec("CREATE TABLE IF NOT EXISTS glitch_recent_files "
		   "(file_name TEXT NOT NULL PRIMARY KEY, image TEXT)");
	query.prepare
	  ("INSERT INTO glitch_recent_files (file_name) VALUES (?)");
	query.addBindValue(QFileInfo(fileName).absoluteFilePath());
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

  auto const index = m_ui.tab->indexOf(view);

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
	QMainWindow::setWindowTitle
	  (tr("Glitch: %1 (*)").arg(view->fileNameOrName()));
      else
	QMainWindow::setWindowTitle
	  (tr("Glitch: %1").arg(view->fileNameOrName()));
    }
  else if(m_currentView)
    {
      if(m_currentView->hasChanged())
	QMainWindow::setWindowTitle
	  (tr("Glitch: %1 (*)").arg(m_currentView->fileNameOrName()));
      else
	QMainWindow::setWindowTitle
	  (tr("Glitch: %1").arg(m_currentView->fileNameOrName()));
    }
  else
    QMainWindow::setWindowTitle(tr("Glitch"));
}

void glitch_ui::show(void)
{
  restoreSettings();

  if(QApplication::arguments().contains("--full-screen") == false)
    {
#ifndef Q_OS_ANDROID
      QMainWindow::show();
#else
      QMainWindow::showMaximized();
#endif
    }
  else
    QMainWindow::showFullScreen();

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
      QFileInfo const fileInfo("qt.conf");
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

  QFileInfo const fileInfo(glitch_variety::homePath());

  if(!fileInfo.isReadable() || !fileInfo.isWritable())
    {
      QMessageBox::critical
	(this,
	 tr("Glitch: Error"),
	 tr("Glitch's home directory %1 must be readable and writable.").
	 arg(glitch_variety::homePath()));
      QApplication::processEvents();
    }

  parseCommandLineArguments();
}

void glitch_ui::showStatusBarMessage(const QString &text, const int timeout)
{
  if(statusBar())
    {
      statusBar()->showMessage(text, timeout);
      QTimer::singleShot(100, statusBar(), SLOT(repaint(void)));
    }
}

void glitch_ui::slotAbout(void)
{
  connect(m_about.button(QMessageBox::Close),
	  &QPushButton::clicked,
	  &m_about,
	  &QMessageBox::close,
	  Qt::UniqueConnection);
  m_about.button(QMessageBox::Close)->setShortcut(tr("Ctrl+W"));
  m_about.resize(m_about.sizeHint());
  m_about.showNormal();
  m_about.activateWindow();
  m_about.raise();
}

void glitch_ui::slotAboutToShowTabsMenu(void)
{
  slotTabMoved(0, 0);
}

void glitch_ui::slotArduinoViewDestroyed(void)
{
  prepareActionWidgets();
}

void glitch_ui::slotClearCopiedWidgetsBuffer(void)
{
  clearCopiedObjects();
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
	    (tr("The current diagram has been modified. Are you sure that "
		"you wish to close it?"));
	  mb.setWindowIcon(windowIcon());
	  mb.setWindowModality(Qt::ApplicationModal);
	  mb.setWindowTitle(tr("Glitch: Confirmation"));
	  QApplication::processEvents();

	  if(mb.exec() != QMessageBox::Yes)
	    {
	      QApplication::processEvents();
	      return;
	    }

	  QApplication::processEvents();
	}

      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      m_ui.tab->removeTab(index);
      QApplication::restoreOverrideCursor();

      /*
      ** Avoid flickering of tool bar buttons.
      */

      QTimer::singleShot(150, view, SLOT(deleteLater(void)));
    }

  prepareActionWidgets();
  prepareStatusBar();
  prepareTab();
  prepareTabShortcuts();
  slotAboutToShowTabsMenu();
}

void glitch_ui::slotCloseDiagram(void)
{
  slotCloseDiagram(m_ui.tab->currentIndex());
}

void glitch_ui::slotCopiedObjectsChanged(void)
{
  m_ui.action_Clear_Copied_Widgets_Buffer->setEnabled
    (!s_copiedObjects.isEmpty());
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
      showStatusBarMessage
	(tr("%1 widget(s) copied.").arg(s_copiedObjects.size()), 5000);
    }
}

void glitch_ui::slotDelayedOpenDiagrams(void)
{
  QString errors("");
  auto state = false;

  foreach(auto const &i, m_delayedDiagrams)
    {
      if(!isVisible())
	/*
	** Glitch has exited!
	*/

	return;

      QString error("");

      if(openDiagram(i, error))
	state = true;
      else
	errors.append
	  (tr("An error occurred while processing the file %1. (%2)\n\n").
	   arg(i.trimmed()).arg(error));
    }

  if(m_generateSource)
    {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

      foreach(auto view, findChildren<glitch_view *> ())
	{
	  if(!isVisible())
	    /*
	    ** Glitch has exited!
	    */

	    break;

	  if(view)
	    view->generateSourceFile();
	}

      QApplication::restoreOverrideCursor();

      if(!isVisible())
	/*
	** Glitch has exited!
	*/

	return;
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
#ifdef Q_OS_ANDROID
      dialog.showMaximized();
#endif
      QApplication::processEvents();
      dialog.exec();
      QApplication::processEvents();
    }

  if(state)
    prepareActionWidgets();
}

void glitch_ui::slotDelayedToolBarPreparation(void)
{
  m_ui.miscellaneous_toolbar->clear();
  m_ui.miscellaneous_toolbar->setVisible(false); // Avoid flicker.
  m_ui.tools_toolbar->clear();
  m_ui.tools_toolbar->setVisible(false); // Avoid flicker.

  if(m_currentView)
    {
      /*
      ** Miscellaneous.
      */

      auto menu = new QMenu(m_currentView);
      auto toolButton = new QToolButton(m_currentView);

#ifdef Q_OS_ANDROID
      connect(menu,
	      SIGNAL(triggered(QAction *)),
	      this,
	      SLOT(slotHideTearOffMenu(void)));
      connect(toolButton,
	      &QToolButton::clicked,
	      this,
	      &glitch_ui::slotShowTearOffMenu);
#else
      connect(toolButton,
	      &QToolButton::clicked,
	      toolButton,
	      &QToolButton::showMenu);
#endif
      m_currentView->populateToolsMenu(menu, this);
      toolButton->setArrowType(Qt::NoArrow);

      auto const static iconWire(QIcon(":/wire.png"));

      toolButton->setIcon(iconWire);
      toolButton->setMenu(menu);
#ifdef Q_OS_MACOS
#else
      toolButton->setPopupMode(QToolButton::MenuButtonPopup);
#endif
#ifdef Q_OS_MACOS
      toolButton->setStyleSheet
	("QToolButton {border: none;}"
	 "QToolButton::menu-button {border: none;}"
	 "QToolButton::menu-indicator {image: none;}");
#endif
      toolButton->setToolTip(tr("Connection Tools"));
      m_ui.miscellaneous_toolbar->addWidget(toolButton);

      QList<QIcon> const static icons
	(QList<QIcon> ()
	 << QIcon(":/adjust-size.png")
	 << QIcon(":/adjust-size.png")
	 << QIcon(":/compress.png")
	 << QIcon(":/disconnect.png")
	 << QIcon(":/document-edit.png")
	 << QIcon(":/font.png")
	 << QIcon(":/pin.png")
	 << QIcon(":/widget-properties.png"));
      QStringList const static data
	(QStringList()
	 << "adjust-all-sizes"
	 << "adjust-sizes"
	 << "compress-widgets"
	 << "disconnect-widgets"
	 << "edit-widgets"
	 << "fonts"
	 << "lock-positions"
	 << "widget-properties");
      QStringList const static texts
	(QStringList()
	 << tr("Adjust Size(s) (All Widgets)")
	 << tr("Adjust Size(s) (Selected Widget(s))")
	 << tr("(De)compress Selected Widget(s)")
	 << tr("Disconnect Selected Widget(s)")
	 << tr("Edit Selected Widgets(s)...")
	 << tr("Fonts (All Widgets)...")
	 << tr("(Un)lock Position(s) (Selected Widget(s))")
	 << tr("Widget(s) Properties (Selected Widget(s))..."));

      menu = new QMenu(m_currentView);

      for(int i = 0; i < data.size(); i++)
	{
	  auto action = menu->addAction(icons.at(i), texts.at(i));

	  action->setData(data.at(i));
	  connect(action,
		  &QAction::triggered,
		  this,
		  &glitch_ui::slotSpecialTools);
	}

      toolButton = new QToolButton(m_currentView);
#ifdef Q_OS_ANDROID
      connect(menu,
	      SIGNAL(triggered(QAction *)),
	      this,
	      SLOT(slotHideTearOffMenu(void)));
      connect(toolButton,
	      &QToolButton::clicked,
	      this,
	      &glitch_ui::slotShowTearOffMenu);
#else
      connect(toolButton,
	      &QToolButton::clicked,
	      toolButton,
	      &QToolButton::showMenu);
#endif
      toolButton->setArrowType(Qt::NoArrow);

      auto const static iconTools(QIcon(":/tools.png"));

      toolButton->setIcon(iconTools);
      toolButton->setMenu(menu);
#ifdef Q_OS_MACOS
#else
      toolButton->setPopupMode(QToolButton::MenuButtonPopup);
#endif
#ifdef Q_OS_MACOS
      toolButton->setStyleSheet
	("QToolButton {border: none;}"
	 "QToolButton::menu-button {border: none;}"
	 "QToolButton::menu-indicator {image: none;}");
#endif
      toolButton->setToolTip(tr("Miscellaneous Tools"));
      m_ui.miscellaneous_toolbar->addWidget(toolButton);

      /*
      ** Tools.
      */

      m_ui.tools_toolbar->addActions(m_currentView->alignmentActions());
    }

  m_ui.miscellaneous_toolbar->setVisible
    (m_currentView && m_ui.action_View_Tool_Bars->isChecked());
  m_ui.tools_toolbar->setVisible
    (m_currentView && m_ui.action_View_Tool_Bars->isChecked());
  repaint();
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

void glitch_ui::slotFind(void)
{
  if(m_currentView)
    m_currentView->find();
}

void glitch_ui::slotForgetRecentDiagram(const QString &fileName, bool *ok)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  if(ok)
    *ok = false;

  QString connectionName("");

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_recentFilesFileName);

    if(db.open())
      {
	QSqlQuery query(db);

	query.prepare("DELETE FROM glitch_recent_files WHERE file_name = ?");
	query.addBindValue(fileName);

	if(query.exec())
	  {
	    if(ok)
	      *ok = true;
	  }
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);
  QApplication::restoreOverrideCursor();
}

void glitch_ui::slotForgetRecentDiagram(void)
{
  auto action = qobject_cast<QWidgetAction *> (sender());

  if(!action)
    return;

  auto ok = true;

  slotForgetRecentDiagram(action->property("file-name").toString(), &ok);
  ok ? m_ui.menu_Recent_Diagrams->removeAction(action) : (void) 0;
}

void glitch_ui::slotGenerateSource(void)
{
  if(m_currentView)
    {
      showStatusBarMessage(tr("Generating source file. Please be patient."));
      m_currentView->generateSourceFile();
      showStatusBarMessage("");
    }
}

void glitch_ui::slotGenerateSourceView(void)
{
  if(m_currentView)
    {
      showStatusBarMessage(tr("Generating source. Please be patient."));
      m_currentView->generateSourceView();
      showStatusBarMessage("");
    }
}

void glitch_ui::slotNewArduinoDiagram(void)
{
  QInputDialog dialog(this);
  QLabel *label = nullptr;
  QString name("");

  dialog.setLabelText
    (tr("Please specify a project name. "
	"A database file having the provided name will be created in "
	"the %1 directory.").arg(glitch_variety::homePath()));
  dialog.setTextValue("Arduino-Diagram");
  dialog.setWindowIcon(windowIcon());
  dialog.setWindowTitle(tr("Glitch: ") + "Arduino" + tr(" Project Name"));
  dialog.resize(450, dialog.sizeHint().height());

  if((label = dialog.findChild<QLabel *> ()))
    label->setWordWrap(true);

#ifdef Q_OS_ANDROID
  dialog.showMaximized();
#endif
  QApplication::processEvents();

  if(dialog.exec() != QDialog::Accepted)
    {
      QApplication::processEvents();
      return;
    }
  else
    name = dialog.textValue().trimmed();

  QApplication::processEvents();

  if(name.isEmpty())
    name = "Arduino-Diagram";

  auto const fileName(QString("%1%2%3.db").
		      arg(glitch_variety::homePath()).
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
      QApplication::processEvents();

      if(mb.exec() != QMessageBox::Yes)
	{
	  QApplication::processEvents();
	  QTimer::singleShot(250, this, &glitch_ui::slotNewArduinoDiagram);
	  return;
	}

      QApplication::processEvents();
    }

  QFile::remove(fileName);
  newArduinoDiagram("", name, false);
  saveRecentFile(fileName);
}

void glitch_ui::slotOpenDiagram(void)
{
  QFileDialog dialog(this);

  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setDirectory(glitch_variety::homePath());
  dialog.setFileMode(QFileDialog::ExistingFiles);
  dialog.setLabelText(QFileDialog::Accept, tr("Select"));
  dialog.setNameFilters(QStringList() << "Arduino" + tr(" Diagrams (*.db)"));
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowIcon(windowIcon());
  dialog.setWindowTitle(tr("Glitch: Open Diagram(s)"));
#ifdef Q_OS_ANDROID
  dialog.showMaximized();
#endif
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();

      QString errors("");
      auto const list(dialog.selectedFiles());
      auto ok = true;

      for(int i = 0; i < list.size(); i++)
	{
	  QString error("");
	  auto const fileName(list.at(i));

	  if(openDiagram(fileName, error))
	    ok = true;
	  else
	    errors.append
	      (tr("An error occurred while processing "
		  "the file %1. (%2)\n\n").arg(fileName.trimmed()).arg(error));
	}

      if(ok)
	prepareActionWidgets();

      if(!errors.isEmpty())
	{
	  QDialog dialog(this);
	  Ui_glitch_errors_dialog ui;

	  ui.setupUi(&dialog);
	  ui.label->setText(tr("The following errors occurred."));
	  ui.text->setPlainText(errors.trimmed());
#ifdef Q_OS_ANDROID
	  dialog.showMaximized();
#endif
	  QApplication::processEvents();
	  dialog.exec();
	  QApplication::processEvents();
	}
    }
  else
    QApplication::processEvents();
}

void glitch_ui::slotOpenRecentDiagram(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  QString error("");

  if(openDiagram(action->property("file-name").toString(), error))
    prepareActionWidgets();

  if(!error.isEmpty())
    {
      QDialog dialog(this);
      Ui_glitch_errors_dialog ui;

      ui.setupUi(&dialog);
      ui.label->setText(tr("The following errors occurred."));
      ui.text->setPlainText(error.trimmed());
#ifdef Q_OS_ANDROID
      dialog.showMaximized();
#endif
      QApplication::processEvents();
      dialog.exec();
      QApplication::processEvents();
    }
}

void glitch_ui::slotPageChanged(void)
{
  prepareActionWidgets();
  setTabText(qobject_cast<glitch_view *> (sender()));
  setWindowTitle(qobject_cast<glitch_view *> (sender()));
}

void glitch_ui::slotPageSaved(void)
{
  prepareActionWidgets();
  setTabText(qobject_cast<glitch_view *> (sender()));
  setWindowTitle(qobject_cast<glitch_view *> (sender()));
}

void glitch_ui::slotPageSelected(int index)
{
  m_currentView = qobject_cast<glitch_view *> (m_ui.tab->widget(index));
  m_ideProcessIndicator->setVisible
    (m_currentView && m_currentView->isIDEProcessActive());
  prepareActionWidgets();
  prepareStatusBar();
  prepareToolBars();
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
  auto view = qobject_cast<QGraphicsView *> (QApplication::focusWidget());

  if(view)
    {
      auto scene = qobject_cast<glitch_scene *> (view->scene());

      if(scene)
	{
	  paste(view, scene->undoStack());
	  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	  prepareRedoUndoActions();
	  QApplication::restoreOverrideCursor();
	}
    }
}

void glitch_ui::slotPreferencesAccepted(void)
{
  QSettings settings;

  m_ui.action_View_Tab_Bar->setChecked
    (settings.value("preferences/tab_bar", true).toBool());
  m_ui.menu_Edit->setTearOffEnabled
    (settings.value("preferences/tear_off_menus", true).toBool());
  m_ui.menu_Windows->setTearOffEnabled
    (settings.value("preferences/tear_off_menus", true).toBool());
  m_ui.tab->tabBar()->setVisible(m_ui.action_View_Tab_Bar->isChecked());
  prepareFonts();
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
	glitch_variety::showErrorDialog
	  (tr("Unable to save %1 (%2).").arg(view->name()).arg(error), this);
      else
	{
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
      dialog.setDirectory(glitch_variety::homePath());
      dialog.setFileMode(QFileDialog::AnyFile);
      dialog.setNameFilter("Glitch Files (*.db)");
      dialog.setOption(QFileDialog::DontConfirmOverwrite, false);
      dialog.setOption(QFileDialog::DontUseNativeDialog);
      dialog.setWindowIcon(windowIcon());
#ifdef Q_OS_ANDROID
      dialog.showMaximized();
#endif
      QApplication::processEvents();

      if(dialog.exec() == QDialog::Accepted)
	{
	  QApplication::processEvents();

	  QString error("");
	  auto fileName(dialog.selectedFiles().value(0));

	  if(!fileName.endsWith(".db"))
	    fileName += ".db";

	  if(!view->saveAs(fileName, error))
	    glitch_variety::showErrorDialog
	      (tr("Unable to save %1 (%2).").arg(view->name()).arg(error),
	       this);
	  else
	    {
	      setTabText(view);
	      setWindowTitle(view);
	    }
	}
      else
	QApplication::processEvents();
    }
}

void glitch_ui::slotSelectAll(void)
{
  if(m_currentView && m_currentView->scene())
    {
      m_currentView->selectAll();
      showStatusBarMessage
	(tr("%1 Primary Diagram Item(s) Selected").
	 arg(m_currentView->scene()->selectedItems().size()));
    }
}

void glitch_ui::slotSelectPage(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
  m_ui.tab->setCurrentWidget(qobject_cast<QWidget *> (action->parent()));
#else
  m_ui.tab->setCurrentWidget(action->parentWidget());
#endif
  setWindowTitle(qobject_cast<glitch_view *> (m_ui.tab->currentWidget()));
}

void glitch_ui::slotSelectionChanged(void)
{
  if(m_currentView && m_currentView->scene())
    {
      m_ui.action_Copy->setEnabled(m_currentView->selectedCanCopyOrDelete());
      m_ui.action_Delete->setEnabled(m_currentView->selectedCanCopyOrDelete());
      m_ui.action_Paste->setEnabled(!s_copiedObjects.isEmpty());
      m_ui.action_Select_All->setEnabled
	(m_currentView->scene()->items().size() > 0);
      showStatusBarMessage
	(tr("%1 Primary Diagram Item(s) Selected").
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

void glitch_ui::slotSeparate(QWidget *widget)
{
  slotSeparate(qobject_cast<glitch_view *> (widget));
}

void glitch_ui::slotSeparate(glitch_view *view)
{
  if(!view)
    {
      /*
      ** Separation via a drag from the tab bar.
      */

      if(m_separatedWindow)
	{
	  m_separatedWindow->move(QCursor::pos() + QPoint(10, 10));
	  m_separatedWindow->showNormal();
	  m_separatedWindow->activateWindow();
	  m_separatedWindow->raise();
	}

      m_separatedWindow = nullptr;
      return;
    }

  m_ui.tab->removeTab(m_ui.tab->indexOf(view));

  auto window = new glitch_separated_diagram_window(this);

  connect(m_preferences,
	  &glitch_preferences::accept,
	  window,
	  &glitch_separated_diagram_window::slotPreferencesAccepted);
  connect(window,
	  SIGNAL(copy(glitch_view *)),
	  this,
	  SLOT(slotCopy(glitch_view *)));
  connect(window,
	  SIGNAL(destroyed(QObject *)),
	  this,
	  SLOT(slotSeparatedWindowDestroyed(QObject *)),
	  Qt::QueuedConnection);
  connect(window,
	  SIGNAL(paste(glitch_view *)),
	  this,
	  SLOT(slotPaste(glitch_view *)));
  m_separatedWindow = m_separatedWindows[window] = window;
  window->setCentralWidget(view);
  view->separate();
  view->show();
  window->resize(view->size());

  if(view->hasChanged())
    window->setWindowTitle(tr("Glitch: %1 (*)").arg(view->fileNameOrName()));
  else
    window->setWindowTitle(tr("Glitch: %1").arg(view->fileNameOrName()));

  if(qobject_cast<QTabWidget *> (sender()))
    m_separatedWindow = window;
  else
    window->show();

  prepareActionWidgets();
  prepareStatusBar();
  prepareTab();
  prepareTabShortcuts();
  setWindowTitle(nullptr);
  slotAboutToShowTabsMenu();
  window->activateWindow();
  window->raise();
}

void glitch_ui::slotShowAllTools(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  foreach(auto view, findChildren<glitch_view *> ())
    if(view)
      view->showTools();

  QApplication::restoreOverrideCursor();
}

void glitch_ui::slotShowArduinoDocumentation(void)
{
  if(!m_arduino)
    {
      m_arduino = new glitch_documentation(":/Arduino/Arduino.pdf", this);
      m_arduino->setWindowTitle
	(tr("Glitch: ") + "Arduino" + tr(" Documentation"));
    }

#ifdef Q_OS_ANDROID
  m_arduino->showMaximized();
#else
  m_arduino->showNormal();
#endif
  m_arduino->activateWindow();
  m_arduino->raise();
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
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
	  menu->exec(mapToGlobal(QPoint(size().width() / 2, 0)));
#else
	  menu->showTearOffMenu(mapToGlobal(QPoint(size().width() / 2, 0)));
#endif
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

void glitch_ui::slotShowPreferences(void)
{
  m_preferences->processSettings();
#ifndef Q_OS_ANDROID
  m_preferences->show();
#else
  m_preferences->showMaximized();
#endif
}

void glitch_ui::slotShowProjectIDE(void)
{
  if(m_currentView)
    m_currentView->launchProjectIDE();
}

void glitch_ui::slotShowReleaseNotes(void)
{
  if(!m_releaseNotes)
    {
      m_releaseNotes = new glitch_documentation
	(QUrl("qrc:/ReleaseNotes.html"), this);
      m_releaseNotes->setWindowTitle(tr("Glitch: Release Notes"));
    }

#ifdef Q_OS_ANDROID
  m_releaseNotes->showMaximized();
#else
  m_releaseNotes->showNormal();
#endif
  m_releaseNotes->activateWindow();
  m_releaseNotes->raise();
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

void glitch_ui::slotStatusBarTimerTimeout(void)
{
  if(statusBar() && statusBar()->currentMessage().trimmed().isEmpty())
    prepareStatusBar();
}

void glitch_ui::slotSwifty(void)
{
  m_about.setText
    (QString(tr("<html>"
		"<b>Glitch Version %1</b><br>"
		"The <b>official version</b> is <b>%2</b>.<br><br>"
		"<b>G</b>raphical<br>"
		"<b>L</b>ightweight<br>"
		"<b>I</b>nteractive<br>"
		"<b>T</b>ool<br>"
		"<b>C</b>ompiler<br>"
		"<b>H</b>ybrid<br><br>"
		"Glitch is an awesome visual compiler and designer for ") +
	     "Arduino" +
	     tr(".<br>"
		"Develop through block diagrams.<br>"
		"Software for and from the margins.<br><br>"
		"Made with love by textbrowser.<br><br>"
		"Architecture: %3.<br>"
		"Compiled On: %4.<br>"
		"Product: %5.<br>"
		"Qt version %6 (runtime version %7).<br><br>"
		"Please visit "
		"<a href=\"https://textbrowser.github.io/glitch\">"
		"https://textbrowser.github.io/glitch</a> for more details.")).
     arg(GLITCH_VERSION_STRING).
     arg(m_swifty->newest_version()).
     arg(QSysInfo::currentCpuArchitecture()).
     arg(COMPILED_ON).
     arg(QSysInfo::prettyProductName()).
     arg(QT_VERSION_STR).
     arg(qVersion()));
}

void glitch_ui::slotTabMoved(int from, int to)
{
  Q_UNUSED(from);
  Q_UNUSED(to);

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  m_ui.menu_Tabs->clear();

  auto group = m_ui.menu_Tabs->findChild<QActionGroup *> ();

  if(!group)
    group = new QActionGroup(m_ui.menu_Tabs);

  for(int i = 0; i < m_ui.tab->count(); i++)
    {
      auto view = qobject_cast<glitch_view *> (m_ui.tab->widget(i));
      auto action = view ?
	view->menuAction() : m_recentDiagramsView->menuAction();

      action->setCheckable(true);
      action->setChecked(i == m_ui.tab->currentIndex());
      group->addAction(action);
      m_ui.menu_Tabs->addAction(action);
    }

  if(group->actions().isEmpty())
    group->deleteLater();

  if(m_ui.menu_Tabs->actions().isEmpty())
    m_ui.menu_Tabs->addAction(tr("Empty"))->setEnabled(false);

  QApplication::restoreOverrideCursor();
}

void glitch_ui::slotTabWidgetShortcutActivated(void)
{
  auto shortcut = qobject_cast<QShortcut *> (sender());

  if(!shortcut)
    return;

  auto const key(shortcut->key());
  auto index = -1;

  for(auto i = Qt::Key_1; i <= Qt::Key_9; i = Qt::Key(i + 1))
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    if(key.matches(QKeySequence(Qt::AltModifier + i)))
#else
    if(key.matches(QKeySequence(Qt::AltModifier | i)))
#endif
      {
	index = -Qt::Key_1 + i;
	break;
      }

  if(index == -1)
    index = m_ui.tab->count() - 1;

  m_ui.tab->setCurrentIndex(index);
}

void glitch_ui::slotToolsOperationChanged
(const glitch_tools::Operations operation)
{
  auto view = qobject_cast<glitch_view *> (sender());

  if(m_ui.tab->currentWidget() != view || view == nullptr)
    return;

  QString text("");

  switch(operation)
    {
    case glitch_tools::Operations::INTELLIGENT:
      {
	text = tr("Connections Mode: Intelligent");
	break;
      }
    case glitch_tools::Operations::SELECT:
      {
	text = tr("Connections Mode: Select");
	break;
      }
    case glitch_tools::Operations::WIRE_CONNECT:
      {
	text = tr("Connections Mode: Wire (Connect)");
	break;
      }
    default:
      {
	text = tr("Connections Mode: Wire (Disconnect)");
	break;
      }
    }

  text.append
    (tr(" | Scaling Factor: %1").
     arg(m_currentView->scalingFactor(), 0, 'f', 2));
  showStatusBarMessage(text);
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

  m_ui.tab->addTab(view, view->menuAction()->icon(), view->name());
  m_ui.tab->setCurrentWidget(view);
  prepareActionWidgets();
  prepareStatusBar();
  prepareTab();
  prepareTabShortcuts();
  setTabText(view);
  setWindowTitle(view);
  slotAboutToShowTabsMenu();
  view->unite();
  window->deleteLater();
}

void glitch_ui::slotViewToolBars(void)
{
  QSettings settings;

  settings.setValue
    ("main_window/view_tools", m_ui.action_View_Tool_Bars->isChecked());
  m_ui.edit_toolbar->setVisible(m_ui.action_View_Tool_Bars->isChecked());
  m_ui.file_toolbar->setVisible(m_ui.action_View_Tool_Bars->isChecked());
  m_ui.miscellaneous_toolbar->setVisible
    (m_currentView && m_ui.action_View_Tool_Bars->isChecked());
  m_ui.project_toolbar->setVisible(m_ui.action_View_Tool_Bars->isChecked());
  m_ui.tools_toolbar->setVisible
    (m_currentView && m_ui.action_View_Tool_Bars->isChecked());
  m_ui.zoom_toolbar->setVisible(m_ui.action_View_Tool_Bars->isChecked());
}
