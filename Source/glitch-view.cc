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
#include <QBuffer>
#include <QDateTime>
#include <QFileDialog>
#include <QFontDialog>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QResizeEvent>
#include <QSettings>
#include <QSplitter>
#include <QSqlError>
#include <QSqlQuery>
#include <QToolButton>
#include <QtConcurrent>

#include "Arduino/glitch-object-function-arduino.h"
#include "glitch-alignment.h"
#include "glitch-application.h"
#include "glitch-ash.h"
#include "glitch-canvas-preview.h"
#include "glitch-docked-container.h"
#include "glitch-documentation.h"
#include "glitch-find-objects.h"
#include "glitch-floating-context-menu.h"
#include "glitch-graphicsview.h"
#include "glitch-object-edit-window.h"
#include "glitch-object-view.h"
#include "glitch-object.h"
#include "glitch-proxy-widget.h"
#include "glitch-recent-diagrams-view.h"
#include "glitch-redo-undo-stack.h"
#include "glitch-scene.h"
#include "glitch-separated-diagram-window.h"
#include "glitch-syntax-highlighter.h"
#include "glitch-tools.h"
#include "glitch-ui.h"
#include "glitch-undo-command.h"
#include "glitch-user-functions.h"
#include "glitch-variety.h"
#include "glitch-view.h"
#include "glitch-wire.h"

glitch_view::glitch_view
(const QString &fileName,
 const QString &name,
 const glitch_common::ProjectTypes projectType,
 QWidget *parent):QWidget(parent), m_scene(nullptr)
{
  m_ui.setupUi(this);
  m_alignment = new glitch_alignment(this);
  m_ash = new glitch_ash(true, this);
  m_bottomSplitter = new QSplitter(Qt::Vertical, this);
  m_canvasPreview = new glitch_canvas_preview(this);
  m_canvasPreview->setScene(m_scene = new glitch_scene(m_projectType, this));
  m_canvasPreview->setVisible(false);
  m_canvasSettings = new glitch_canvas_settings(this);
  m_canvasSettings->prepare(fileName);
  m_canvasSettings->setName(name);
  m_dockedWidgetPropertyEditors = new glitch_docked_container(this);
  m_dockedWidgetPropertyEditors->resize
    (m_dockedWidgetPropertyEditors->sizeHint());
  m_dockedWidgetPropertyEditors->setMinimumWidth(250);
  m_fileName = fileName;
  m_generateSourceViewTimer.setInterval(1500);
  m_generateSourceViewTimer.setSingleShot(true);
  m_generateTimer.setInterval(1500);
  m_generateTimer.setSingleShot(true);
  m_ideClear = new QToolButton(m_ideOutput = new QTextBrowser(this));
  m_ideClear->setIcon(QIcon::fromTheme("edit-clear"));
#ifdef Q_OS_MACOS
  m_ideClear->setStyleSheet
    ("QToolButton {border: none; margin-bottom: 0px; margin-top: 0px;}"
     "QToolButton::menu-button {border: none;}"
     "QToolButton::menu-indicator {image: none;}");
#else
  m_ideClear->setStyleSheet
    ("QToolButton {border: none; margin-bottom: 5px; margin-top: 5px;}"
     "QToolButton::menu-button {border: none;}"
     "QToolButton::menu-indicator {image: none;}");
#endif
  m_ideClear->setVisible(false);
  m_ideOutput->setPlaceholderText(tr("IDE Output"));
  m_menuAction = new QAction
    (QIcon(":/Logo/glitch-arduino-logo.png"), m_canvasSettings->name(), this);
  m_projectType = projectType;
  m_redoUndoStack = nullptr;
  m_resizeSceneTimer.setInterval(500);
  m_resizeSceneTimer.setSingleShot(true);
  m_rightSplitter = new QSplitter(Qt::Vertical, this);
  m_saveTimer.setInterval(1500);
  m_saveTimer.setSingleShot(true);
  m_scene->setBackgroundBrush(QColor("#55aaff"));
  m_scene->setCanvasSettings(m_canvasSettings);
  m_scene->setDotsGridsColor(QColor(Qt::white));
  m_scene->setMainScene(true);
  m_scene->setShowCanvasDots(m_canvasSettings->showCanvasDots());
  m_scene->setShowCanvasGrids(m_canvasSettings->showCanvasGrids());
  m_scene->setUndoStack(m_undoStack = new QUndoStack(this));
  m_settings = m_canvasSettings->settings();
  m_splitter = new QSplitter(this);
  m_tabPullDown = new QToolButton(this);
  m_tabPullDown->setArrowType(Qt::NoArrow);
  m_tabPullDown->setAutoRaise(false);
  m_tabPullDown->setIcon(QIcon(":/down.png"));
  m_tabPullDown->setIconSize(QSize(24, 24));
  m_tabPullDown->setMenu(new QMenu(m_tabPullDown));
  m_tabPullDown->setPopupMode(QToolButton::DelayedPopup);
#ifdef Q_OS_MACOS
  m_tabPullDown->setStyleSheet
    ("QToolButton {border: none; margin-bottom: 0px; margin-top: 0px;}"
     "QToolButton::menu-button {border: none;}"
     "QToolButton::menu-indicator {image: none;}");
#else
  m_tabPullDown->setStyleSheet
    ("QToolButton {border: none; margin-bottom: 5px; margin-top: 5px;}"
     "QToolButton::menu-button {border: none;}"
     "QToolButton::menu-indicator {image: none;}");
#endif
  m_tabPullDown->setToolTip(tr("Tab Menu"));
  m_tabPullDown->menu()->setStyleSheet("QMenu {menu-scrollable: 1;}");
  m_ui.tab->setCornerWidget(m_tabPullDown, Qt::TopLeftCorner);
  m_ui.tab->setDocumentMode(false);
  m_ui.tab->setTabPosition(m_canvasSettings->tabPosition());
  m_ui.tab->setTabsClosable(false);
  m_undoStack->setUndoLimit(m_canvasSettings->redoUndoStackSize());
  m_userFunctions = new glitch_user_functions(this);
  m_userFunctions->setModel
    (m_userFunctionsModel = new glitch_user_functions_model(this));
  m_userFunctions->setProjectType(m_projectType);
  m_userFunctionsModel->setHorizontalHeaderLabels
    (QStringList() << tr("Function Name"));
  m_view = new glitch_graphicsview(this);
  m_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  m_view->setCacheMode(QGraphicsView::CacheNone);
  m_view->setCornerWidget(new QLabel(this));
  m_view->setDragMode(QGraphicsView::RubberBandDrag);
  m_view->setFrameStyle(QFrame::NoFrame);
  m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  m_view->setInteractive(true);
  m_view->setRenderHints(QPainter::Antialiasing |
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
			 QPainter::LosslessImageRendering |
#endif
			 QPainter::SmoothPixmapTransform |
			 QPainter::TextAntialiasing);
  m_view->setRubberBandSelectionMode(Qt::IntersectsItemShape);
  m_view->setScene(m_scene);
  m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  connect
    (&glitch_ui::s_copiedObjects,
     &glitch_aware_multi_map<QPair<int, int>, QPointer<glitch_object> >::
     cleared,
     this,
     &glitch_view::slotCopiedObjectsChanged,
     Qt::QueuedConnection);
  connect
    (&glitch_ui::s_copiedObjects,
     &glitch_aware_multi_map<QPair<int, int>, QPointer<glitch_object> >::
     inserted,
     this,
     &glitch_view::slotCopiedObjectsChanged,
     Qt::QueuedConnection);
  connect(&m_generateSourceViewTimer,
	  &QTimer::timeout,
	  this,
	  &glitch_view::slotGenerateSourceView);
  connect(&m_generateTimer,
	  &QTimer::timeout,
	  this,
	  &glitch_view::slotGenerate);
  connect(&m_ideProcess,
	  &QProcess::started,
	  this,
	  &glitch_view::ideProcessStarted);
  connect(&m_ideProcess,
	  SIGNAL(finished(int, QProcess::ExitStatus)),
	  this,
	  SIGNAL(ideProcessFinished(void)));
  connect(&m_resizeSceneTimer,
	  &QTimer::timeout,
	  this,
	  &glitch_view::slotResizeScene);
  connect(&m_saveTimer,
	  &QTimer::timeout,
	  this,
	  &glitch_view::slotSave);
  connect(m_alignment,
	  &glitch_alignment::changed,
	  this,
	  &glitch_view::slotChanged);
  connect(m_canvasSettings,
	  SIGNAL(accepted(const bool)),
	  this,
	  SLOT(slotCanvasSettingsChanged(const bool)),
	  Qt::QueuedConnection);
  connect(m_ideClear,
	  &QPushButton::clicked,
	  m_ideOutput,
	  &QTextBrowser::clear);
  connect(m_ideOutput,
	  &QTextBrowser::textChanged,
	  this,
	  &glitch_view::slotIDEOutputTextChanged);
  connect(m_scene,
	  &glitch_scene::changed,
	  this,
	  &glitch_view::slotChanged,
	  Qt::QueuedConnection);
  connect(m_scene,
	  &glitch_scene::copy,
	  this,
	  &glitch_view::slotCopy);
  connect(m_scene,
	  &glitch_scene::saveSignal,
	  this,
	  &glitch_view::slotSave);
  connect(m_scene,
	  &glitch_scene::sceneResized,
	  this,
	  &glitch_view::slotSceneResized,
	  Qt::QueuedConnection);
  connect(m_scene,
	  &glitch_scene::selectionChanged,
	  this,
	  &glitch_view::selectionChanged);
  connect(m_scene,
	  &glitch_scene::selectionChanged,
	  this,
	  &glitch_view::slotSelectionChanged);
  connect(m_scene,
	  SIGNAL(destroyed(QObject *)),
	  this,
	  SLOT(slotSceneObjectDestroyed(QObject *)),
	  Qt::QueuedConnection);
  connect(m_scene,
	  SIGNAL(dockPropertyEditor(QWidget *)),
	  this,
	  SLOT(slotDockPropertyEditor(QWidget *)));
  connect(m_scene,
	  SIGNAL(functionAdded(const QString &, const bool)),
	  this,
	  SLOT(slotFunctionAdded(const QString &, const bool)));
  connect(m_scene,
	  SIGNAL(functionDeleted(const QString &)),
	  this,
	  SLOT(slotFunctionDeleted(const QString &)));
  connect(m_scene,
	  SIGNAL(functionNameChanged(const QString &,
				     const QString &,
				     glitch_object *)),
	  this,
	  SLOT(slotFunctionNameChanged(const QString &,
				       const QString &,
				       glitch_object *)));
  connect(m_scene,
	  SIGNAL(functionReturnPointerChanged(const bool,
					      const bool,
					      glitch_object *)),
	  this,
	  SLOT(slotFunctionReturnPointerChanged(const bool,
						const bool,
						glitch_object *)));
  connect(m_scene,
	  SIGNAL(functionReturnTypeChanged(const QString &,
					   const QString &,
					   glitch_object *)),
	  this,
	  SLOT(slotFunctionReturnTypeChanged(const QString &,
					     const QString &,
					     glitch_object *)));
  connect(m_scene,
	  SIGNAL(processCommand(const QString &, const QStringList &)),
	  this,
	  SLOT(slotProcessCommand(const QString &, const QStringList &)));
  connect(m_scene,
	  SIGNAL(showEditWindow(QMainWindow *)),
	  this,
	  SLOT(slotShowEditWindow(QMainWindow *)));
  connect(m_tabPullDown,
	  &QToolButton::clicked,
	  m_tabPullDown,
	  &QToolButton::showMenu);
  connect(m_tabPullDown->menu(),
	  &QMenu::aboutToShow,
	  this,
	  &glitch_view::slotAboutToShowTabCornerMenu);
  connect(m_ui.tab,
	  SIGNAL(tabCloseRequested(int)),
	  this,
	  SLOT(slotCloseTab(int)));
  connect(m_undoStack,
	  SIGNAL(indexChanged(int)),
	  this,
	  SLOT(slotUndoStackChanged(int)));
  connect(m_view,
	  &glitch_graphicsview::mouseEnterEvent,
	  this,
	  &glitch_view::mouseEnterEvent);
  connect(m_view,
	  &glitch_graphicsview::mouseLeaveEvent,
	  this,
	  &glitch_view::mouseLeaveEvent);
  connect(m_view,
	  SIGNAL(customContextMenuRequested(const QPoint &)),
	  this,
	  SLOT(slotCustomContextMenuRequested(const QPoint &)));
  connect(this,
	  SIGNAL(customContextMenuRequested(const QPoint &)),
	  this,
	  SLOT(slotCustomContextMenuRequested(const QPoint &)));
  copyCornerWidget();
  m_bottomSplitter->addWidget(m_ash->frame());
  m_bottomSplitter->addWidget(m_ideOutput);
  m_bottomSplitter->setStretchFactor(0, 1);
  m_bottomSplitter->setStretchFactor(1, 0);
  m_ui.bottom_layout->addWidget(m_bottomSplitter);
  m_ui.top_layout->addWidget(m_splitter);
  prepareASH(parent);
  prepareDatabaseTables();
  prepareDefaultActions();
  prepareTabWidget();
  prepareTabWidgetCloseButtons();
}

glitch_view::~glitch_view()
{
  disconnect(m_undoStack,
	     SIGNAL(indexChanged(int)),
	     this,
	     SLOT(slotUndoStackChanged(int)));
  m_generateSourceViewTimer.stop();
  m_generateTimer.stop();
  m_resizeSceneTimer.stop();
  m_saveSnapFuture.cancel();
  m_saveSnapFuture.waitForFinished();
  m_saveTimer.stop();
  m_scene->purgeRedoUndoProxies();
}

QAction *glitch_view::menuAction(void) const
{
  return m_menuAction;
}

QImage glitch_view::snap(void) const
{
  QImage image(m_scene->sceneRect().size().toSize(), QImage::Format_RGB32);
  QPainter painter;

  image.fill(Qt::white);
  painter.begin(&image);
  m_scene->render(&painter, QRectF(), m_scene->sceneRect());
  painter.end();
  return image;
}

QList<QAction *> glitch_view::alignmentActions(void) const
{
  return m_alignment->actions();
}

QList<QAction *> glitch_view::defaultActions(void) const
{
  return m_defaultActions;
}

QList<glitch_object *> glitch_view::allObjects(void) const
{
  if(m_scene)
    return m_scene->allObjects();
  else
    return QList<glitch_object *> ();
}

QList<glitch_object *> glitch_view::objects(void) const
{
  if(m_scene)
    return m_scene->objects();
  else
    return QList<glitch_object *> ();
}

QList<glitch_object *> glitch_view::selectedObjects(void) const
{
  return m_scene->selectedObjects();
}

QMenu *glitch_view::defaultContextMenu(void)
{
  if(!m_contextMenu)
    {
      m_contextMenu = new QMenu(this);
#ifndef Q_OS_ANDROID
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
      m_contextMenu->setTearOffEnabled(true);
#endif
      m_contextMenu->setTitle(tr("Glitch: Floating Context Menu"));
#endif
    }
  else
    m_contextMenu->clear();

  m_contextMenu->addSection(tr("glitch-view"));

  auto action = m_pasteAction = m_contextMenu->addAction
    (tr("Paste"), this, SLOT(slotPaste(void)));

  action->setEnabled(!glitch_ui::s_copiedObjects.isEmpty());
  action->setIcon(QIcon(":/paste.png"));
  m_contextMenu->addSeparator();
  action = m_saveDiagramAction = m_contextMenu->addAction
    (tr("&Save"), this, SLOT(slotSave(void)));
  action->setIcon(QIcon(":/save.png"));
  m_contextMenu->addAction(tr("Save &As..."),
			   this,
			   SLOT(slotSaveAs(void)))->
    setIcon(QIcon(":/save-as.png"));
  m_contextMenu->addSeparator();
  m_contextMenu->addAction(tr("Tools..."),
			   this,
			   SLOT(slotShowTools(void)));
  m_contextMenu->addSeparator();
#ifndef Q_OS_ANDROID
  action = m_contextMenu->addAction(tr("Se&parate Canvas..."),
				    this,
				    SLOT(slotSeparate(void)));

  if(qobject_cast<QMainWindow *> (parentWidget()))
    action->setEnabled(false);
  else
    action->setEnabled(true);

  action = m_contextMenu->addAction(tr("&Unite Canvas"),
				    this,
				    SLOT(slotUnite(void)));

  if(qobject_cast<glitch_separated_diagram_window *> (parentWidget()))
    action->setEnabled(true);
  else
    action->setEnabled(false);

  m_contextMenu->addSeparator();
#endif
  m_contextMenu->addAction(tr("Show Canvas &Settings..."),
			   this,
			   SLOT(slotShowCanvasSettings(void)))->
    setIcon(QIcon(":/settings.png"));
  m_contextMenu->addSeparator();
  m_contextMenu->addAction(tr("&User Functions..."),
			   this,
			   SLOT(slotShowUserFunctions(void)));
  m_contextMenu->addSeparator();
  m_contextMenu->addAction(tr("Zoom Reset"), this, SLOT(slotZoomReset(void)))->
    setIcon(QIcon(":/zoom-reset.png"));
  return m_contextMenu;
}

QString glitch_view::fileName(void) const
{
  return m_fileName;
}

QString glitch_view::fileNameOrName(void) const
{
  if(m_fileName.trimmed().isEmpty() == false)
    return m_fileName;
  else
    return name();
}

QString glitch_view::name(void) const
{
  return m_canvasSettings->name();
}

QString glitch_view::redoText(void) const
{
  return m_undoStack->redoText();
}

QString glitch_view::source(void) const
{
  return "";
}

QString glitch_view::undoText(void) const
{
  return m_undoStack->undoText();
}

QUndoStack *glitch_view::undoStack(void) const
{
  return m_undoStack;
}

bool glitch_view::canRedo(void) const
{
  return m_undoStack->canRedo();
}

bool glitch_view::canUndo(void) const
{
  return m_undoStack->canUndo();
}

bool glitch_view::containsFunction(const QString &name) const
{
  return m_userFunctions->contains(name);
}

bool glitch_view::hasChanged(void) const
{
  return !m_undoStack->isClean();
}

bool glitch_view::isIDEProcessActive(void) const
{
  return m_ideProcess.state() == QProcess::Running;
}

bool glitch_view::open(const QString &fileName, QString &error)
{
  QFileInfo const fileInfo(fileName);

  if(!fileInfo.isReadable() && !fileInfo.isWritable())
    {
      error = tr("The file must be both readable and writable.");
      return false;
    }

  disconnect(m_scene,
	     &glitch_scene::changed,
	     this,
	     &glitch_view::slotChanged);
  disconnect(m_scene,
	     &glitch_scene::sceneResized,
	     this,
	     &glitch_view::slotSceneResized);
  m_canvasSettings->prepare(fileName);
  m_delayedWires.clear();
  m_fileName = fileName;
  m_scene->setLoadingFromFile(true);
  m_settings = m_canvasSettings->settings();
  m_view->setUpdatesEnabled(false);
  m_view->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);

  QString connectionName("");
  auto ok = true;

  {
    QHash<qint64, char> ids;
    QHash<qint64, glitch_object *> objects;
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_fileName);

    if((ok = db.open()))
      {
	QSqlQuery query(db);

	query.setForwardOnly(true);

	if(query.exec(QString("SELECT SUBSTR(properties, 1, %1) FROM "
			      "diagram_properties").
		      arg(static_cast<int> (Limits::
					    PROPERTIES_MAXIMUM_LENGTH))) &&
	   query.next())
	  {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
	    auto const list
	      (query.value(0).toString().trimmed().
	       split('&', Qt::SkipEmptyParts));
#else
	    auto const list
	      (query.value(0).toString().trimmed().
	       split('&', QString::SkipEmptyParts));
#endif

	    for(int i = 0; i < list.size(); i++)
	      {
		auto string(list.at(i));

		if(string.startsWith("bottom_splitter_state"))
		  {
		    string = string.mid(string.indexOf('=') + 1);
		    string.remove("\"");
		    m_properties["bottom_splitter_state"] =
		      QByteArray::fromBase64(string.toLatin1());
		  }
		else if(string.startsWith("main_splitter_state"))
		  {
		    string = string.mid(string.indexOf('=') + 1);
		    string.remove("\"");
		    m_properties["main_splitter_state"] =
		      QByteArray::fromBase64(string.toLatin1());
		  }
		else if(string.startsWith("right_splitter_state"))
		  {
		    string = string.mid(string.indexOf('=') + 1);
		    string.remove("\"");
		    m_properties["right_splitter_state"] =
		      QByteArray::fromBase64(string.toLatin1());
		  }
		else if(string.startsWith("splitter_state"))
		  {
		    string = string.mid(string.indexOf('=') + 1);
		    string.remove("\"");
		    m_properties["splitter_state"] = QByteArray::fromBase64
		      (string.toLatin1());
		  }
	      }

	    m_bottomSplitter->restoreState
	      (m_properties.value("bottom_splitter_state").toByteArray());
	    m_rightSplitter->restoreState
	      (m_properties.value("right_splitter_state").toByteArray());
	    m_splitter->restoreState
	      (m_properties.value("splitter_state").toByteArray());
	    m_ui.splitter->restoreState
	      (m_properties.value("main_splitter_state").toByteArray());
	  }

	if(query.exec(QString("SELECT "
			      "myoid, "
			      "parent_oid, "
			      "position, "
			      "SUBSTR(properties, 1, %1), "
			      "SUBSTR(stylesheet, 1, %2), "
			      "SUBSTR(type, 1, %3) "
			      "FROM objects ORDER BY parent_oid, properties").
		      arg(static_cast<int> (Limits::PROPERTIES_MAXIMUM_LENGTH)).
		      arg(static_cast<int> (Limits::STYLESHEET_MAXIMUM_LENGTH)).
		      arg(static_cast<int> (glitch_ui::Limits::
					    TYPE_MAXIMUM_LENGTH))))
	  {
	    QHash<qint64, glitch_object *> parents;

	    while(query.next())
	      {
		if(ids.contains(query.value(0).toLongLong()))
		  continue;

		QMap<QString, QVariant> values;
		auto const id = query.value(0).toLongLong();
		auto const point(query.value(2).toString().trimmed());
		auto const properties(query.value(3).toString().trimmed());
		auto const type(query.value(5).toString().toLower().trimmed());

		values["myoid"] = id;
		values["parentId"] = query.value(1).toLongLong();
		values["properties"] = properties;
		values["stylesheet"] = query.value(4).toString().trimmed();
		values["type"] = type;

		if(query.value(1).toLongLong() == -1)
		  {
		    QString error("");
		    auto object = glitch_object::createFromValues
		      (values, nullptr, error, this);

		    if(object)
		      {
			ids[object->id()] = 0;
			object->setCanvasSettings(m_canvasSettings);

			if(object->isMandatory())
			  parents[id] = object;
			else
			  {
			    auto proxy = m_scene->addObject(object);

			    if(proxy)
			      {
				m_scene->addItem(proxy);
				object->setUndoStack(m_undoStack);
				objects[object->id()] = object;
				parents[id] = object;
				proxy->setPos
				  (glitch_variety::dbPointToPointF(point));
			      }
			    else
			      object->deleteLater();
			  }
		      }
		  }
		else
		  {
		    auto object = objects.value(query.value(1).toLongLong());

		    if(!object)
		      object = parents.value(query.value(1).toLongLong());

		    if(!object)
		      {
			createParentFromValues
			  (ids, parents, db, query.value(1).toLongLong());
			object = parents.value(query.value(1).toLongLong());
		      }

		    if(object && object->editView())
		      {
			object->editView()->scene()->setLoadingFromFile(true);

			auto child = glitch_object::createFromValues
			  (values, object, error, object->editView());

			if(child)
			  {
			    auto ok = true;

			    child->setCanvasSettings(m_canvasSettings);
			    ids[child->id()] = 0;
			    object->addChild
			      (glitch_variety::dbPointToPointF(point),
			       child,
			       ok);
			    object->hideOrShowOccupied();

			    if(ok)
			      objects[child->id()] = child;
			    else
			      {
				ids.remove(child->id());
				child->deleteLater();
			      }
			  }

			object->editView()->scene()->setLoadingFromFile(false);
		      }
		  }
	      }
	  }
	else
	  {
	    error = tr("An error occurred while accessing the objects table.");
	    ok = false;
	  }

	if(error.isEmpty())
	  {
	    if(query.exec("SELECT object_input_oid, object_output_oid "
			  "FROM wires"))
	      while(query.next())
		{
		  auto object1 = objects.value(query.value(0).toLongLong());
		  auto object2 = objects.value(query.value(1).toLongLong());

		  if(object1 != object2 &&
		     object1 &&
		     object1->proxy() &&
		     object1->scene() &&
		     object1->scene()->
		     areObjectsWired(object1, object2) == false &&
		     object2 &&
		     object2->proxy())
		    {
		      auto wire = new glitch_wire(nullptr, false);

		      connect(object1->scene(),
			      SIGNAL(changed(const QList<QRectF> &)),
			      wire,
			      SLOT(slotUpdate(const QList<QRectF> &)));
		      connect(wire,
			      &glitch_wire::disconnectWireIfNecessary,
			      object1->scene(),
			      &glitch_scene::slotDisconnectWireIfNecessary,
			      Qt::QueuedConnection);
		      m_delayedWires << wire;
		      object1->scene()->addItem(wire, false);
		      object1->setWiredObject(object2, wire, false);
		      object2->setWiredObject(object1, wire, false);
		      wire->setBoundingRect(object1->scene()->sceneRect());
		      wire->setColor(m_canvasSettings->wireColor());
		      wire->setLeftProxy(object2->proxy());
		      wire->setRightProxy(object1->proxy());
		      wire->setWireType(m_canvasSettings->wireType());
		      wire->setWireWidth(m_canvasSettings->wireWidth());
		    }
		}
	    else
	      {
		error = tr
		  ("An error occurred while accessing the wires table.");
		ok = false;
	      }
	  }
      }

    db.close();
  }

  QTimer::singleShot(100, this, &glitch_view::slotShowWires);
  QTimer::singleShot(1500, this, &glitch_view::slotSaveSnap);
  error = error.trimmed();
  glitch_common::discardDatabase(connectionName);
  adjustScrollBars();
  connect(m_scene,
	  &glitch_scene::changed,
	  this,
	  &glitch_view::slotChanged,
	  Qt::QueuedConnection);
  connect(m_scene,
	  &glitch_scene::sceneResized,
	  this,
	  &glitch_view::slotSceneResized,
	  Qt::QueuedConnection);
  m_scene->setLoadingFromFile(false);
  m_view->setUpdatesEnabled(true);
  m_view->setViewportUpdateMode(m_canvasSettings->viewportUpdateMode());
  return ok;
}

bool glitch_view::save(QString &error)
{
  return saveImplementation(m_fileName, error);
}

bool glitch_view::saveAs(const QString &fileName, QString &error)
{
  return saveImplementation(fileName, error);
}

bool glitch_view::saveImplementation(const QString &fileName, QString &error)
{
  m_canvasSettings->setFileName(fileName);
  m_fileName = fileName;

  if(!m_canvasSettings->save(error))
    return false;

  prepareDatabaseTables();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

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
	ok = query.exec("DELETE FROM diagram");

	if(!ok)
	  {
	    error = query.lastError().text();
	    goto done_label;
	  }

	query.prepare("INSERT OR REPLACE INTO diagram "
		      "(name, type) "
		      "VALUES (?, ?)");
	query.addBindValue(m_canvasSettings->name());
	query.addBindValue(glitch_common::projectTypeToString(m_projectType));
	ok = query.exec();

	if(!ok)
	  {
	    error = query.lastError().text();
	    goto done_label;
	  }

	ok = query.exec("DELETE FROM objects");

	if(!ok)
	  {
	    error = query.lastError().text();
	    goto done_label;
	  }

	ok = query.exec("DELETE FROM wires");

	if(!ok)
	  {
	    error = query.lastError().text();
	    goto done_label;
	  }

	foreach(auto object, m_scene->objects())
	  if(object)
	    {
	      object->blockSignals(true);
	      object->save(db, error);
	      object->blockSignals(false);

	      if(!error.isEmpty())
		{
		  ok = false;
		  break;
		}
	    }

	if(ok)
	  m_scene->saveWires(db, error);

	if(!error.isEmpty())
	  ok = false;
      }
    else
      error = db.lastError().text();

  done_label:
    db.close();
  }

  error = error.trimmed();
  glitch_common::discardDatabase(connectionName);

  if(ok)
    ok &= saveProperties();

  if(ok)
    m_undoStack->setClean();

  QApplication::restoreOverrideCursor();
  QTimer::singleShot(500, this, &glitch_view::slotSaveSnap);
  return ok;
}

bool glitch_view::saveProperties(void) const
{
  QString connectionName("");
  auto ok = true;

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_fileName);

    if((ok = db.open()))
      {
	QMapIterator<QString, QVariant> it(m_properties);
	QSqlQuery query(db);
	QString string("");

	while(it.hasNext())
	  {
	    it.next();
	    string += it.key();
	    string += " = ";
	    string += "\"";
	    string += it.value().toByteArray().toBase64();
	    string += "\"";

	    if(it.hasNext())
	      string += "&";
	  }

	query.exec("DELETE FROM diagram_properties");
	query.prepare
	  ("INSERT OR REPLACE INTO diagram_properties (properties) VALUES(?)");
	query.addBindValue(string);
	ok = query.exec();
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);
  return ok;
}

glitch_common::ProjectTypes glitch_view::projectType(void) const
{
  return m_projectType;
}

glitch_graphicsview *glitch_view::view(void) const
{
  return m_view;
}

glitch_object *glitch_view::find
(const QList<glitch_object *> &list,
 const qint64 id,
 glitch_object *object) const
{
  foreach(auto i, list)
    if(i)
      {
	if(i->id() == id)
	  {
	    object = i;
	    return object;
	  }

	object = find(i->objects(), id, object);
      }

  return object;
}

glitch_object *glitch_view::find(const qint64 id) const
{
  glitch_object *object = nullptr;

  return find(objects(), id, object);
}

glitch_scene *glitch_view::scene(void) const
{
  return m_scene;
}

glitch_tools::Operations glitch_view::toolsOperation(void) const
{
  if(m_tools)
    return m_tools->operation();
  else
    return glitch_tools::Operations::INTELLIGENT;
}

glitch_user_functions_model *glitch_view::userFunctionsModel(void) const
{
  return m_userFunctionsModel;
}

qint64 glitch_view::nextId(void) const
{
  QString connectionName("");
  auto id = static_cast<qint64> (glitch_object::Limits::MINIMUM_ID);

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_fileName);

    if(db.open())
      {
	QSqlQuery query(db);

	if(query.exec(QString("INSERT INTO sequence SELECT "
			      "COALESCE(MAX(value), %1) + 1 FROM sequence").
		      arg(id)))
	  {
	    auto const variant(query.lastInsertId());

	    if(variant.isValid())
	      {
		id = variant.toLongLong();
		query.exec
		  (QString("DELETE FROM sequence WHERE value < %1").arg(id));
	      }
	  }
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);
  return id;
}

qreal glitch_view::scalingFactor(void) const
{
  return m_view->scalingFactor();
}

void glitch_view::adjustScrollBars(void)
{
  m_resizeSceneTimer.start();
}

void glitch_view::beginMacro(const QString &text)
{
  if(text.trimmed().isEmpty())
    m_undoStack->beginMacro(tr("unknown"));
  else
    m_undoStack->beginMacro(text);
}

void glitch_view::clearSelection(void)
{
  if(m_scene)
    m_scene->clearSelection();
}

void glitch_view::contextMenuEvent(QContextMenuEvent *event)
{
  if(event)
    emit customContextMenuRequested(event->pos());
  else
    emit customContextMenuRequested(QCursor::pos());
}

void glitch_view::copyCornerWidget(void)
{
  if(m_ui.tab->cornerWidget(Qt::TopRightCorner))
    return;

  auto toolButton = new QToolButton(this);

  toolButton->setArrowType(Qt::NoArrow);
  toolButton->setAutoRaise(false);
  toolButton->setIcon(QIcon(":/down.png"));
  toolButton->setIconSize(QSize(24, 24));
  toolButton->setMenu(new QMenu(toolButton));
  toolButton->setPopupMode(QToolButton::DelayedPopup);
#ifdef Q_OS_MACOS
  toolButton->setStyleSheet
    ("QToolButton {border: none; margin-bottom: 0px; margin-top: 0px;}"
     "QToolButton::menu-button {border: none;}"
     "QToolButton::menu-indicator {image: none;}");
#else
  toolButton->setStyleSheet
    ("QToolButton {border: none; margin-bottom: 5px; margin-top: 5px;}"
     "QToolButton::menu-button {border: none;}"
     "QToolButton::menu-indicator {image: none;}");
#endif
  toolButton->setToolTip(tr("Tab Menu"));
  toolButton->menu()->setStyleSheet("QMenu {menu-scrollable: 1;}");
  connect(toolButton,
	  &QToolButton::clicked,
	  toolButton,
	  &QToolButton::showMenu);
  connect(toolButton->menu(),
	  &QMenu::aboutToShow,
	  this,
	  &glitch_view::slotAboutToShowTabCornerMenu);
  m_ui.tab->setCornerWidget(toolButton, Qt::TopRightCorner);
}

void glitch_view::createParentFromValues
(QHash<qint64, char> &ids,
 QHash<qint64, glitch_object *> &parents,
 const QSqlDatabase &db,
 const qint64 oid) const
{
  QSqlQuery query(db);

  query.setForwardOnly(true);
  query.prepare
    (QString("SELECT "
	     "parent_oid, "
	     "position, "
	     "SUBSTR(properties, 1, %1), "
	     "SUBSTR(stylesheet, 1, %2), "
	     "SUBSTR(type, 1, %3) "
	     "FROM objects WHERE myoid = ?").
     arg(static_cast<int> (Limits::PROPERTIES_MAXIMUM_LENGTH)).
     arg(static_cast<int> (Limits::STYLESHEET_MAXIMUM_LENGTH)).
     arg(static_cast<int> (glitch_ui::Limits::TYPE_MAXIMUM_LENGTH)));
  query.addBindValue(oid);

  if(!query.exec() || !query.next())
    return;

  auto object = parents.value(query.value(0).toLongLong());

  if(!object)
    return;

  QMap<QString, QVariant> values;
  QString error("");
  auto const point(query.value(1).toString().trimmed());
  auto const properties(query.value(2).toString().trimmed());
  auto const type(query.value(4).toString().toLower().trimmed());

  values["myoid"] = oid;
  values["parentId"] = query.value(0).toLongLong();
  values["properties"] = properties;
  values["stylesheet"] = query.value(3).toString().trimmed();
  values["type"] = type;

  auto child = glitch_object::createFromValues
    (values, object, error, object->editView());

  if(child)
    {
      auto ok = true;

      child->setCanvasSettings(m_canvasSettings);
      ids[child->id()] = 0;
      object->addChild(glitch_variety::dbPointToPointF(point), child, ok);
      object->hideOrShowOccupied();

      if(ok)
	parents[oid] = child;
      else
	{
	  ids.remove(child->id());
	  child->deleteLater();
	}
    }
}

void glitch_view::createTools(void)
{
  if(!m_tools)
    {
      m_tools = new glitch_tools(this);
      connect(m_tools,
	      SIGNAL(operation(const glitch_tools::Operations)),
	      m_scene,
	      SLOT(slotToolsOperationChanged(const glitch_tools::Operations)));
      connect(m_tools,
	      SIGNAL(operation(const glitch_tools::Operations)),
	      this,
	      SLOT(slotToolsOperationChanged(const glitch_tools::Operations)));
      connect(m_tools,
	      SIGNAL(operation(const glitch_tools::Operations)),
	      this,
	      SIGNAL(toolsOperationChanged(const glitch_tools::Operations)));
      m_tools->setOperation
	(glitch_tools::Operations(property("tools-operation").toInt()));
    }
}

void glitch_view::deleteItems(void)
{
  m_scene->deleteItems();
}

void glitch_view::editWidgets(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  foreach(auto object, selectedObjects())
    if(object)
      object->showEditWindow();

  QApplication::restoreOverrideCursor();
}

void glitch_view::endMacro(void)
{
  m_undoStack->endMacro();
}

void glitch_view::find(void)
{
  if(!m_findObjects)
    {
      m_findObjects = new glitch_find_objects(this);
      connect(this,
	      &glitch_view::changed,
	      m_findObjects,
	      &glitch_find_objects::slotSynchronize);
    }

  m_findObjects->setWindowTitle
    (tr("Glitch: Find Objects (%1)").arg(m_canvasSettings->name()));
#ifdef Q_OS_ANDROID
  m_findObjects->showMaximized();
#else
  m_findObjects->showNormal();
#endif
  m_findObjects->activateWindow();
  m_findObjects->raise();
}

void glitch_view::generateSource(QTextStream &stream) const
{
  stream << "// Generated by Glitch!"
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
	 << endl
	 << "// "
	 << QDateTime::currentDateTime().toString()
	 << endl
	 << endl
#else
         << Qt::endl
	 << "// "
	 << QDateTime::currentDateTime().toString()
	 << Qt::endl
	 << Qt::endl
#endif
    ;
}

void glitch_view::generateSourceClipboard(void) const
{
}

void glitch_view::generateSourceFile(void) const
{
}

void glitch_view::generateSourceView(const bool raise)
{
  if(!m_sourceView)
    {
      m_sourceView = new glitch_documentation(this);
      m_sourceView->setWindowTitle(tr("Glitch: Source View (%1)").arg(name()));
      m_sourceViewSyntaxHighlighter = new glitch_syntax_highlighter
	(m_sourceView->document());
      m_sourceViewSyntaxHighlighter->setKeywordsColors
	(m_canvasSettings->keywordColorsAsMap());
    }

  if(m_sourceView->isVisible() || raise)
    m_sourceView->setPlainText(source());

  if(raise)
    {
#ifdef Q_OS_ANDROID
      m_sourceView->showMaximized();
#else
      m_sourceView->showNormal();
#endif
      m_sourceView->activateWindow();
      m_sourceView->raise();
    }
}

void glitch_view::launchProjectIDE(void) const
{
  auto const program(m_canvasSettings->projectIDE());
  auto const outputFile(m_canvasSettings->outputFile());

  if(QFileInfo(program).isExecutable())
    {
      if(outputFile.length() > 0)
	{
	  if(!QFileInfo::exists(outputFile))
	    generateSourceFile();

	  QProcess::startDetached(program, QStringList() << outputFile);
	}
      else
	QProcess::startDetached(program, QStringList());
    }
  else
    {
      m_canvasSettings->showPage(glitch_canvas_settings::Pages::Project);
      showCanvasSettings();
    }
}

void glitch_view::print(void)
{
  QPrinter printer;
  QScopedPointer<QPrintPreviewDialog> dialog
    (new QPrintPreviewDialog(&printer, this));

  connect(dialog.data(),
	  SIGNAL(paintRequested(QPrinter *)),
	  this,
	  SLOT(slotPrint(QPrinter *)));
#ifdef Q_OS_ANDROID
  dialog->showMaximized();
#endif
  dialog->exec();
  QApplication::processEvents();
}

void glitch_view::populateToolsMenu(QMenu *menu, QWidget *parent)
{
  createTools();

  if(m_tools)
    m_tools->populateMenu(menu, parent);
}

void glitch_view::prepareASH(QWidget *parent)
{
  connect(m_ash,
	  SIGNAL(processCommand(const QString &)),
	  this,
	  SLOT(slotProcessCommand(const QString &)),
	  Qt::UniqueConnection);
  connect(this,
	  SIGNAL(canvasNameChanged(const QString &)),
	  m_ash,
	  SLOT(slotCanvasNameChanged(const QString &)),
	  Qt::UniqueConnection);
  connect(this,
	  SIGNAL(information(const QString &)),
	  m_ash,
	  SLOT(slotCommandProcessed(const QString &)),
	  Qt::UniqueConnection);

  if(parent)
    {
      connect(m_ash,
	      SIGNAL(processCommand(const QString &)),
	      parent,
	      SLOT(slotProcessCommand(const QString &)),
	      Qt::UniqueConnection);
      connect(parent,
	      SIGNAL(information(const QString &)),
	      m_ash,
	      SLOT(slotInformationReceived(const QString &)),
	      Qt::UniqueConnection);
    }

  emit canvasNameChanged(m_canvasSettings->name());
}

void glitch_view::prepareDatabaseTables(void) const
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QString connectionName("");

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_fileName);

    if(db.open())
      {
	QSqlQuery query(db);

	query.exec("CREATE TABLE IF NOT EXISTS diagram ("
		   "name TEXT NOT NULL PRIMARY KEY, "
		   "type TEXT NOT NULL)");
	query.exec("CREATE TABLE IF NOT EXISTS diagram_properties "
		   "(properties TEXT)");
	query.exec("CREATE TABLE IF NOT EXISTS objects ("
		   "myoid INTEGER NOT NULL UNIQUE, "
		   "parent_oid INTEGER NOT NULL DEFAULT -1, "
		   "position TEXT NOT NULL, "
		   "properties TEXT, "
		   "stylesheet TEXT, "
		   "type TEXT NOT NULL, "
		   "PRIMARY KEY (myoid, parent_oid))");
	query.exec("CREATE TABLE IF NOT EXISTS sequence ("
		   "value INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT)");
	query.exec("CREATE TABLE IF NOT EXISTS wires ("
		   "object_input_oid INTEGER NOT NULL, "
		   "object_output_oid INTEGER NOT NULL, "
		   "PRIMARY KEY (object_input_oid, object_output_oid))");
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);
  QApplication::restoreOverrideCursor();
}

void glitch_view::prepareDefaultActions(void)
{
  if(!m_defaultActions.isEmpty())
    return;

  QAction *action = nullptr;

  action = new QAction(tr("Se&parate Canvas..."), this);
  connect(action,
	  &QAction::triggered,
	  this,
	  &glitch_view::slotSeparate);
  m_defaultActions << action;
  action = new QAction(tr("&User Functions..."), this);
  connect(action,
	  &QAction::triggered,
	  this,
	  &glitch_view::slotShowUserFunctions);
  m_defaultActions << action;
}

void glitch_view::prepareTabCornerMenu(QToolButton *toolButton)
{
  if(!toolButton || !toolButton->menu())
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  toolButton->menu()->clear();

  auto action = toolButton->menu()->addAction(tr("&Close All"));

  action->setData(-1);
  action->setEnabled(m_ui.tab->count() > 1);
  connect(action,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSelectItemTab(void)));
  toolButton->menu()->addSeparator();

  auto group = toolButton->menu()->findChild<QActionGroup *> ();

  if(!group)
    group = new QActionGroup(toolButton->menu());

  for(int i = 0; i < m_ui.tab->count(); i++)
    {
      auto action = new QAction(m_ui.tab->tabText(i), this);
      auto font(action->font());

      font.setBold(i == m_ui.tab->currentIndex());
      action->setCheckable(true);
      action->setChecked(i == m_ui.tab->currentIndex());
      action->setData(i);
      action->setFont(font);
      connect(action,
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotSelectItemTab(void)));
      group->addAction(action);
      toolButton->menu()->addAction(action);
    }

  if(group->actions().isEmpty())
    group->deleteLater();

  QApplication::restoreOverrideCursor();
}

void glitch_view::prepareTabTitles(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  for(int i = m_ui.tab->count() - 1; i > 0; i--)
    {
      auto window = qobject_cast<glitch_object_edit_window *>
	(m_ui.tab->widget(i));

      if(window && window->object() && window->object()->scene())
	{
	  auto title(window->windowTitle().trimmed());

	  title = title.mid(title.indexOf(':') + 1).trimmed();

	  if(title.isEmpty())
	    title = tr("Unknown");

	  m_ui.tab->setTabText(i, title);
	  m_ui.tab->setTabToolTip(i, m_ui.tab->tabText(i));
	}
      else
	/*
	** The object was deleted, temporarily.
	*/

	m_ui.tab->removeTab(i);
    }

  QApplication::restoreOverrideCursor();
}

void glitch_view::prepareTabWidget(void)
{
  m_ui.tab->disableSeparation();
  m_ui.tab->setContextMenuPolicy(Qt::NoContextMenu);
  m_ui.tab->tabBar()->setContextMenuPolicy(Qt::NoContextMenu);
}

void glitch_view::prepareTabWidgetCloseButtons(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  m_ui.tab->setTabsClosable(m_ui.tab->count() > 1);

  QList<QTabBar::ButtonPosition> const static list
    (QList<QTabBar::ButtonPosition> () << QTabBar::LeftSide
                                       << QTabBar::RightSide);
  const int index = 0;

  for(int i = 0; i < list.size(); i++)
    {
      m_ui.tab->tabBar()->tabButton(index, list.at(i)) ?
	m_ui.tab->tabBar()->tabButton(index, list.at(i))->deleteLater() :
	(void) 0;
      m_ui.tab->tabBar()->setTabButton(index, list.at(i), nullptr);
    }

  QApplication::processEvents();
  QApplication::restoreOverrideCursor();
}

void glitch_view::push(glitch_undo_command *undoCommand)
{
  if(undoCommand)
    m_undoStack->push(undoCommand);
}

void glitch_view::redo(void)
{
  m_scene->redo();
}

void glitch_view::reparent(void)
{
}

void glitch_view::resizeEvent(QResizeEvent *event)
{
  QWidget::resizeEvent(event);
  m_ideClear->isVisible() ?
    m_ideClear->move
    (-QPoint(25 + m_ideClear->width(), -5) + m_ideOutput->rect().topRight()) :
    (void) 0;

  if(m_view->isVisible())
    setSceneRect(m_view->size());
  else
    setSceneRect(size());
}

void glitch_view::save(void)
{
  QString error("");

  save(error);
}

void glitch_view::saveSnap(void)
{
  if(m_fileName.trimmed().isEmpty() || m_saveSnapFuture.isRunning())
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QBuffer buffer;
  QByteArray bytes;
  QImage image;
  QPainter painter;

  image = QImage
    (1.05 * glitch_recent_diagrams_view::s_snapSize, QImage::Format_ARGB32);
  buffer.setBuffer(&bytes);
  buffer.open(QIODevice::WriteOnly);
  image.fill(Qt::white);
  painter.begin(&image);
  painter.setRenderHints(QPainter::Antialiasing |
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
			 QPainter::LosslessImageRendering |
#endif
			 QPainter::SmoothPixmapTransform |
			 QPainter::TextAntialiasing);
  m_scene->render
    (&painter,
     QRectF(),
     QRectF(QPointF(0.0, 0.0),
	    1.05 * QSizeF(glitch_recent_diagrams_view::s_snapSize)));
  painter.end();
  image.save(&buffer, "PNG", 100);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  m_saveSnapFuture = QtConcurrent::run
    (this, &glitch_view::saveSnapToDatabase, bytes, m_fileName);
#else
  m_saveSnapFuture = QtConcurrent::run
    (&glitch_view::saveSnapToDatabase, this, bytes, m_fileName);
#endif
  QApplication::restoreOverrideCursor();
}

void glitch_view::saveSnapToDatabase
(const QByteArray &bytes, const QString &fileName)
{
  QString connectionName("");

  {
    auto db = glitch_common::sqliteDatabase();

    connectionName = db.connectionName();
    db.setDatabaseName
      (glitch_variety::homePath() +
       QDir::separator() +
       "Glitch" +
       QDir::separator() +
       "glitch_recent_files.db");

    if(db.open())
      {
	QSqlQuery query(db);

	query.exec("PRAGMA JOURNAL_MODE = WAL");
	query.exec("PRAGMA SYNCHRONOUS = NORMAL");
	query.prepare
	  ("INSERT OR REPLACE INTO glitch_recent_files (file_name, image) "
	   "VALUES (?, ?)");
	query.addBindValue(fileName);
	query.addBindValue(bytes.toBase64());
	query.exec();
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);
}

void glitch_view::selectAll(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  foreach(auto i, m_scene->items())
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(proxy)
	proxy->setSelected(true);
    }

  QApplication::restoreOverrideCursor();
}

void glitch_view::setSceneRect(const QSize &size)
{
  Q_UNUSED(size);

  auto b(m_scene->itemsBoundingRect().adjusted(0.0, 0.0, 250.0, 250.0));

  b.setTopLeft(QPointF(0.0, 0.0));
  m_scene->setSceneRect
    (0.0,
     0.0,
     static_cast<double> (qMax(static_cast<int> (b.width()),
			       m_view->width() - 2 * m_view->frameWidth())),
     static_cast<double> (qMax(static_cast<int> (b.height()),
			       m_view->height() - 2 * m_view->frameWidth())));
}

void glitch_view::showCanvasSettings(void) const
{
  m_canvasSettings->setSettings(m_settings);
#ifdef Q_OS_ANDROID
  m_canvasSettings->showMaximized();
#else
  m_canvasSettings->showNormal();
#endif
  m_canvasSettings->activateWindow();
  m_canvasSettings->raise();
}

void glitch_view::showRedoUndoStack(void)
{
  if(!m_redoUndoStack)
    {
      m_redoUndoStack = new glitch_redo_undo_stack(this);
      m_redoUndoStack->setUndoStack(m_undoStack);
    }

#ifdef Q_OS_ANDROID
  m_redoUndoStack->showMaximized();
#else
  m_redoUndoStack->showNormal();
#endif
  m_redoUndoStack->activateWindow();
  m_redoUndoStack->raise();
}

void glitch_view::showTools(void)
{
  createTools();

  if(m_tools)
    {
      m_tools->setWindowTitle
	(tr("Glitch: Tools (%1)").arg(m_canvasSettings->name()));
#ifdef Q_OS_ANDROID
      m_tools->showMaximized();
#else
      m_tools->showNormal();
#endif
      m_tools->activateWindow();
      m_tools->raise();
    }
}

void glitch_view::showUserFunctions(void) const
{
  slotShowUserFunctions();
}

void glitch_view::slotAboutToShowTabCornerMenu(void)
{
  auto menu = qobject_cast<QMenu *> (sender());

  if(menu)
    prepareTabCornerMenu(qobject_cast<QToolButton *> (menu->parentWidget()));
}

void glitch_view::slotAllWidgetsAdjustSize(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  auto began = false;

  foreach(auto object, m_scene->allObjects())
    if(object)
      {
	if(!began && m_undoStack)
	  {
	    began = true;
	    m_undoStack->beginMacro(tr("widgets sizes"));
	  }

	object->slotAdjustSize();
      }

  if(began && m_undoStack)
    m_undoStack->endMacro();

  QApplication::restoreOverrideCursor();
}

void glitch_view::slotCanvasSettingsChanged(const bool undo)
{
  auto hash(m_settings);

  if(m_findObjects)
    m_findObjects->setWindowTitle
      (tr("Glitch: Find Objects (%1)").arg(m_canvasSettings->name()));

  m_canvasPreview->setVisible(m_canvasSettings->showPreview());
  m_menuAction->setText(m_canvasSettings->name());
  m_rightSplitter->setVisible
    (QSettings().value("preferences/docked_widget_property_editors",
		       true).toBool() ||
     m_canvasSettings->showPreview());
  m_scene->setBackgroundBrush(m_canvasSettings->canvasBackgroundColor());
  m_scene->setDotsGridsColor(m_canvasSettings->dotsGridsColor());
  m_scene->setShowCanvasDots(m_canvasSettings->showCanvasDots());
  m_scene->setShowCanvasGrids(m_canvasSettings->showCanvasGrids());
  m_settings = m_canvasSettings->settings();

  if(m_sourceView)
    {
      m_sourceViewSyntaxHighlighter->setKeywordsColors
	(m_canvasSettings->keywordColorsAsMap());
      m_sourceView->setPlainText(source());
      m_sourceView->setWindowTitle(tr("Glitch: Source View (%1)").arg(name()));
    }

  m_tools ? m_tools->setWindowTitle
    (tr("Glitch: Tools (%1)").arg(m_canvasSettings->name())) : (void) 0;
  m_ui.tab->setTabPosition(m_canvasSettings->tabPosition());
  m_undoStack->count() == 0 ?
    m_undoStack->setUndoLimit(m_canvasSettings->redoUndoStackSize()) :
    (void) 0;
  m_userFunctions->setWindowTitle
    (tr("Glitch: User Functions (%1)").arg(m_canvasSettings->name()));
  m_view->setViewportUpdateMode(m_canvasSettings->viewportUpdateMode());

  if(!hash.isEmpty() && hash != m_settings)
    {
      if(undo)
	{
	  auto undoCommand = new glitch_undo_command
	    (hash,
	     glitch_undo_command::Types::CANVAS_SETTINGS_CHANGED,
	     m_canvasSettings);

	  undoCommand->setText(tr("canvas settings changed"));
	  m_undoStack->push(undoCommand);
	}

      emit canvasNameChanged(m_canvasSettings->name());
      slotChanged();
    }
}

void glitch_view::slotChanged(void)
{
  if(m_canvasSettings->generatePeriodically())
    m_generateTimer.start();

  if(m_canvasSettings->generateSourceViewPeriodically())
    m_generateSourceViewTimer.start();

  if(m_canvasSettings->savePeriodically())
    m_saveTimer.start();

  setSceneRect(m_view->size());
  emit changed();
}

void glitch_view::slotCloseTab(int index)
{
  if(index > 0)
    {
      auto window = qobject_cast<glitch_object_edit_window *>
	(m_ui.tab->widget(index));

      window ? (void) window->close() : (void) 0;
      m_ui.tab->removeTab(index);
    }

  if(sender()) // Avoid iterations.
    prepareTabWidgetCloseButtons();
}

void glitch_view::slotCopiedObjectsChanged(void)
{
  if(m_pasteAction)
    m_pasteAction->setEnabled(!glitch_ui::s_copiedObjects.isEmpty());
}

void glitch_view::slotCopy(void)
{
  if(qobject_cast<glitch_object_view *> (sender()))
    emit copy(qobject_cast<QGraphicsView *> (sender()));
  else
    {
      auto scene = qobject_cast<glitch_scene *> (sender());

      if(scene)
	{
	  foreach(auto view, scene->views())
	    if(m_view == view)
	      {
		emit copy(view);
		break;
	      }
	}
      else
	emit copy(m_view);
    }
}

void glitch_view::slotCustomContextMenuRequested(const QPoint &point)
{
  auto menu = defaultContextMenu();

  if(!menu)
    return;

  if(sender() == this)
    menu->exec(mapToGlobal(point));
  else
    menu->exec(point);
}

void glitch_view::slotDockPropertyEditor(QWidget *widget)
{
  m_dockedWidgetPropertyEditors->add
    (qobject_cast<glitch_floating_context_menu *> (widget));
}

void glitch_view::slotEditWindowClosed(void)
{
  auto widget = qobject_cast<QWidget *> (sender());

  if(widget)
    {
      auto const index = m_ui.tab->indexOf(widget);

      if(index > 0)
	m_ui.tab->removeTab(index);
    }

  prepareTabWidgetCloseButtons();
}

void glitch_view::slotFonts(void)
{
  QFontDialog dialog(this);

#ifdef Q_OS_ANDROID
  dialog.showMaximized();
#endif
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

      auto began = false;

      foreach(auto object, m_scene->allObjects())
	if(object)
	  {
	    if(!began && m_undoStack)
	      {
		began = true;
		m_undoStack->beginMacro(tr("widgets fonts"));
	      }

	    object->slotPropertyChanged("font", dialog.selectedFont());
	  }

      if(began && m_undoStack)
	m_undoStack->endMacro();

      QApplication::restoreOverrideCursor();
    }
  else
    QApplication::processEvents();
}

void glitch_view::slotFunctionAdded(const QString &name, const bool isClone)
{
  if(!isClone)
    m_userFunctions->addFunction(name);
}

void glitch_view::slotFunctionDeleted(const QString &name)
{
  m_userFunctions->deleteFunction(name);
}

void glitch_view::slotFunctionNameChanged(const QString &after,
					  const QString &before,
					  glitch_object *object)
{
  m_userFunctions->addFunction(after);
  m_userFunctions->deleteFunction(before);

  auto undoCommand = new glitch_undo_command
    (before,
     glitch_undo_command::Types::FUNCTION_RENAMED,
     object,
     m_userFunctions);

  if(object)
    undoCommand->setText
      (tr("function renamed (%1, %2)").
       arg(object->scenePos().x()).
       arg(object->scenePos().y()));
  else
    undoCommand->setText(tr("function renamed"));

  m_undoStack->push(undoCommand);
  emit changed();
}

void glitch_view::slotFunctionReturnPointerChanged(const bool after,
						   const bool before,
						   glitch_object *object)
{
  Q_UNUSED(after);

  auto undoCommand = new glitch_undo_command
    (QVariant(before).toString(),
     glitch_undo_command::Types::FUNCTION_RETURN_POINTER_CHANGED,
     object,
     m_userFunctions);

  if(object)
    undoCommand->setText
      (tr("function return pointer changed (%1, %2)").
       arg(object->scenePos().x()).
       arg(object->scenePos().y()));
  else
    undoCommand->setText(tr("function return pointer changed"));

  m_undoStack->push(undoCommand);
  emit changed();
}

void glitch_view::slotFunctionReturnTypeChanged(const QString &after,
						const QString &before,
						glitch_object *object)
{
  Q_UNUSED(after);

  auto undoCommand = new glitch_undo_command
    (before,
     glitch_undo_command::Types::FUNCTION_RETURN_TYPE_CHANGED,
     object,
     m_userFunctions);

  if(object)
    undoCommand->setText
      (tr("function return type changed (%1, %2)").
       arg(object->scenePos().x()).
       arg(object->scenePos().y()));
  else
    undoCommand->setText(tr("function return type changed"));

  m_undoStack->push(undoCommand);
  emit changed();
}

void glitch_view::slotGenerate(void)
{
  generateSourceFile();
}

void glitch_view::slotGenerateSourceView(void)
{
  generateSourceView(false);
}

void glitch_view::slotIDEOutputTextChanged(void)
{
  if(m_ideOutput->toPlainText().isEmpty())
    m_ideClear->setVisible(false);
  else
    {
      m_ideClear->move
	(-QPoint(25 + m_ideClear->width(), -5) +
	 m_ideOutput->rect().topRight());
      m_ideClear->setVisible(true);
    }
}

void glitch_view::slotPaste(void)
{
  emit paste(this);
}

void glitch_view::slotPreferencesAccepted(void)
{
  auto const state = QSettings().value
    ("preferences/docked_widget_property_editors", true).toBool();

  if(state)
    {
      foreach(auto object, m_scene->objects())
	if(object && object->menu() && object->menu()->isVisible())
	  slotDockPropertyEditor(object->menu());
    }
  else
    m_dockedWidgetPropertyEditors->detach();

  m_dockedWidgetPropertyEditors->setVisible(state);
  m_rightSplitter->setVisible(m_canvasSettings->showPreview() || state);
  emit preferencesAccepted();
}

void glitch_view::slotPrint(QPrinter *printer)
{
  if(!printer)
    return;

  QPainter painter(printer);

  m_scene->render(&painter);
}

void glitch_view::slotProcessCommand
(const QString &command, const QStringList &arguments)
{
  Q_UNUSED(arguments);
  Q_UNUSED(command);
}

void glitch_view::slotResizeScene(void)
{
  setSceneRect(m_view->size());
}

void glitch_view::slotSave(void)
{
  QString error("");

  if(!save(error))
    {
      error = error.trimmed();

      if(error.isEmpty())
	error = tr("unknown error");

      glitch_variety::showErrorDialog
	(tr("Unable to save %1 (%2).").
	 arg(m_canvasSettings->name()).
	 arg(error),
	 this);
    }
  else
    emit saved();
}

void glitch_view::slotSimulateDelete(void)
{
  auto object = qobject_cast<glitch_object *> (sender());

  if(!object)
    return;
}

void glitch_view::slotSaveAs(void)
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

      if(!saveAs(dialog.selectedFiles().value(0), error))
	glitch_variety::showErrorDialog
	  (tr("Unable to save %1 (%2).").
	   arg(m_canvasSettings->name()).arg(error), this);
      else
	emit saved();
    }
  else
    QApplication::processEvents();
}

void glitch_view::slotSaveSnap(void)
{
  saveSnap();
}

void glitch_view::slotSceneObjectDestroyed(QObject *object)
{
  if(!object)
    return;

  if(qobject_cast<glitch_object_function_arduino *> (object))
    m_userFunctions->deleteFunction
      (qobject_cast<glitch_object_function_arduino *> (object)->name());

  emit changed();
}

void glitch_view::slotSceneResized(void)
{
  adjustScrollBars();
}

void glitch_view::slotSelectItemTab(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;
  else if(action->data().toInt() < 0)
    {
      for(int i = m_ui.tab->count() - 1; i > 0; i--)
	slotCloseTab(i);

      prepareTabWidgetCloseButtons();
    }
  else
    m_ui.tab->setCurrentIndex
      (qBound(0, action->data().toInt(), m_ui.tab->count() - 1));
}

void glitch_view::slotSelectedWidgetsProperties(void)
{
  auto objects(selectedObjects());

  if(objects.isEmpty())
    return;

  if(objects.size() >=
     static_cast<int> (Limits::OPEN_WIDGETS_PROPERTIES_PROMPT))
    {
      QMessageBox mb(this);

      mb.setIcon(QMessageBox::Question);
      mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
      mb.setText
	(tr("Are you sure that you wish to open %1 property editor(s)?").
	 arg(objects.size()));
      mb.setWindowIcon(windowIcon());
      mb.setWindowModality(Qt::ApplicationModal);
      mb.setWindowTitle(tr("Glitch: Confirmation"));

      if(mb.exec() == QMessageBox::No)
	{
	  QApplication::processEvents();
	  return;
	}
    }

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  foreach(auto object, objects)
    if(object)
      object->slotShowContextMenu();

  QApplication::restoreOverrideCursor();
}

void glitch_view::slotSelectionChanged(void)
{
  /*
  ** We may arrive here after a scene is purged.
  */
}

void glitch_view::slotSeparate(void)
{
  emit separate(this);
}

void glitch_view::slotShowCanvasSettings(void)
{
  showCanvasSettings();
}

void glitch_view::slotShowEditWindow(QMainWindow *window)
{
  if(!m_canvasSettings->tabbedEditWindows() || !window)
    {
      auto w = qobject_cast<glitch_object_edit_window *> (window);

      if(w && w->isVisible() == false)
	{
	  w->close();
	  QApplication::processEvents();
	  m_ui.tab->removeTab(m_ui.tab->indexOf(w));
	  prepareTabWidgetCloseButtons();
	  w->object() ? w->object()->createEditObjects() : (void) 0;
	  w->object() ? w->object()->showEditWindow(false) : (void) 0;
	  w->prepareForTab(false);
	}

      return;
    }

  auto const index = m_ui.tab->indexOf(window);

  if(index == -1)
    {
      auto w = qobject_cast<glitch_object_edit_window *> (window);

      if(w)
	{
	  disconnect(w,
		     &glitch_object_edit_window::closedByButton,
		     this,
		     &glitch_view::slotEditWindowClosed);
	  w->close();
	  w->prepareForTab(true);
	  QApplication::processEvents();
	  m_ui.tab->addTab
	    (w,
	     w->windowTitle().mid(w->windowTitle().indexOf(':') + 1).trimmed());
	  m_ui.tab->setCurrentIndex(m_ui.tab->count() - 1);
	  m_ui.tab->setTabToolTip
	    (m_ui.tab->count() - 1, m_ui.tab->tabText(m_ui.tab->count() - 1));
	  prepareTabWidgetCloseButtons();
	  connect
	    (w,
	     &glitch_object_edit_window::closedByButton,
	     this,
	     &glitch_view::slotEditWindowClosed);
	}
    }
  else
    m_ui.tab->setCurrentIndex(index);

  QApplication::processEvents();
}

void glitch_view::slotShowFind(void)
{
  find();
}

void glitch_view::slotShowTools(void)
{
  showTools();
}

void glitch_view::slotShowUserFunctions(void) const
{
  m_userFunctions->setWindowTitle
    (tr("Glitch: User Functions (%1)").arg(m_canvasSettings->name()));
#ifdef Q_OS_ANDROID
  m_userFunctions->showMaximized();
#else
  m_userFunctions->showNormal();
#endif
  m_userFunctions->activateWindow();
  m_userFunctions->raise();
}

void glitch_view::slotShowWires(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  for(int i = m_delayedWires.size() - 1; i >= 0; i--)
    {
      auto wire = m_delayedWires.at(i);

      if(wire &&
	 wire->leftProxy() &&
	 wire->leftProxy()->isVisible() &&
	 wire->rightProxy() &&
	 wire->rightProxy()->isVisible())
	{
	  m_delayedWires.removeAt(i);
	  wire->blockSignals(true); // Block changed().
	  wire->setVisible(true);
	  wire->blockSignals(false);
	}
    }

  if(!m_delayedWires.isEmpty())
    QTimer::singleShot(500, this, &glitch_view::slotShowWires);

  QApplication::restoreOverrideCursor();
}

void glitch_view::slotToolsOperationChanged
(const glitch_tools::Operations operation)
{
  setProperty("tools-operation", static_cast<int> (operation));
}

void glitch_view::slotUndoStackChanged(int index)
{
  Q_UNUSED(index);

  if(m_canvasSettings->generatePeriodically())
    m_generateTimer.start();

  if(m_canvasSettings->generateSourceViewPeriodically())
    m_generateSourceViewTimer.start();

  if(m_canvasSettings->savePeriodically())
    m_saveTimer.start();

  adjustScrollBars();
  emit changed();
  prepareTabTitles();
}

void glitch_view::slotUnite(void)
{
  emit unite(this);
}

void glitch_view::slotZoomReset(void)
{
  m_view->zoom(0);
  emit zoomReset();
}

void glitch_view::terminate(const QStringList &arguments)
{
  Q_UNUSED(arguments);
}

void glitch_view::undo(void)
{
  m_scene->undo();
}

void glitch_view::upload(const QStringList &arguments)
{
  Q_UNUSED(arguments);
  m_ui.tab->setCurrentIndex(0);
}

void glitch_view::verify(const QStringList &arguments)
{
  Q_UNUSED(arguments);
  m_ui.tab->setCurrentIndex(0);
}

void glitch_view::zoom(const int direction)
{
  m_view->zoom(direction);
}
