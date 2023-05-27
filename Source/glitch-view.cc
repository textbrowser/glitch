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

#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QMainWindow>
#include <QMenu>
#include <QProcess>
#include <QResizeEvent>
#include <QSettings>
#include <QScrollBar>
#include <QSplitter>
#include <QSqlError>
#include <QSqlQuery>
#include <QTableWidget>

#include "Arduino/glitch-object-function-arduino.h"
#include "glitch-alignment.h"
#include "glitch-documentation.h"
#include "glitch-docked-container.h"
#include "glitch-find-objects.h"
#include "glitch-floating-context-menu.h"
#include "glitch-graphicsview.h"
#include "glitch-misc.h"
#include "glitch-object.h"
#include "glitch-object-view.h"
#include "glitch-proxy-widget.h"
#include "glitch-scene.h"
#include "glitch-separated-diagram-window.h"
#include "glitch-syntax-highlighter.h"
#include "glitch-tools.h"
#include "glitch-ui.h"
#include "glitch-undo-command.h"
#include "glitch-user-functions.h"
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
  m_canvasSettings = new glitch_canvas_settings(this);
  m_canvasSettings->setFileName(fileName);
  m_canvasSettings->setName(name);
  m_canvasSettings->prepare();
  m_dockedWidgetPropertyEditors = new glitch_docked_container(this);
  m_dockedWidgetPropertyEditors->resize
    (m_dockedWidgetPropertyEditors->sizeHint());
  m_dockedWidgetPropertyEditors->setMinimumWidth(250);
  m_fileName = fileName;
  m_generateSourceViewTimer.setInterval(250);
  m_generateSourceViewTimer.setSingleShot(true);
  m_generateTimer.setInterval(1500);
  m_generateTimer.setSingleShot(true);
  m_menuAction = new QAction
    (QIcon(":/Logo/glitch-arduino-logo.png"), m_canvasSettings->name(), this);
  m_projectType = projectType;
  m_scene = new glitch_scene(m_projectType, this);
  m_scene->setBackgroundBrush(QColor(Qt::white));
  m_scene->setCanvasSettings(m_canvasSettings);
  m_scene->setDotsGridsColor(Qt::white);
  m_scene->setMainScene(true);
  m_scene->setShowCanvasDots(m_canvasSettings->showCanvasDots());
  m_scene->setShowCanvasGrids(m_canvasSettings->showCanvasGrids());
  m_scene->setUndoStack(m_undoStack = new QUndoStack(this));
  m_settings = m_canvasSettings->settings();
  m_splitter = new QSplitter(this);
  m_tools = new glitch_tools(this);
  m_undoStack->setUndoLimit(m_canvasSettings->redoUndoStackSize());
  m_userFunctions = new glitch_user_functions(this);
  m_userFunctions->setProjectType(m_projectType);
  m_view = new glitch_graphicsview(this);
  m_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  m_view->setCacheMode(QGraphicsView::CacheNone);
  m_view->setCornerWidget(new QLabel(this));
  m_view->setDragMode(QGraphicsView::RubberBandDrag);
  m_view->setFrameStyle(QFrame::NoFrame);
  m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  m_view->setInteractive(true);
  m_view->setRenderHints(QPainter::Antialiasing |
			 QPainter::SmoothPixmapTransform |
			 QPainter::TextAntialiasing);
  m_view->setRubberBandSelectionMode(Qt::IntersectsItemShape);
  m_view->setScene(m_scene);
  m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
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
  connect(m_alignment,
	  &glitch_alignment::changed,
	  this,
	  &glitch_view::slotChanged);
  connect(m_canvasSettings,
	  SIGNAL(accepted(const bool)),
	  this,
	  SLOT(slotCanvasSettingsChanged(const bool)));
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
	  SIGNAL(functionReturnTypeChanged(const QString &,
					   const QString &,
					   glitch_object *)),
	  this,
	  SLOT(slotFunctionReturnTypeChanged(const QString &,
					     const QString &,
					     glitch_object *)));
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
  connect(m_undoStack,
	  SIGNAL(indexChanged(int)),
	  this,
	  SLOT(slotUndoStackChanged(void)));
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
  layout()->addWidget(m_splitter);
  prepareDatabaseTables();
  prepareDefaultActions();
}

glitch_view::~glitch_view()
{
  disconnect(m_undoStack,
	     SIGNAL(indexChanged(int)),
	     this,
	     SLOT(slotUndoStackChanged(void)));
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

  auto action = m_pasteAction = m_contextMenu->addAction
    (tr("Paste"), this, SLOT(slotPaste(void)));

  action->setEnabled(!glitch_ui::s_copiedObjects.isEmpty());
  action->setIcon(QIcon::fromTheme("edit-paste"));
  m_contextMenu->addSeparator();
  action = m_saveDiagramAction = m_contextMenu->addAction
    (tr("&Save"), this, SLOT(slotSave(void)));
  action->setIcon(QIcon::fromTheme("document-save"));
  m_contextMenu->addAction(tr("Save &As..."),
			   this,
			   SLOT(slotSaveAs(void)))->
    setIcon(QIcon::fromTheme("document-save-as"));
  m_contextMenu->addSeparator();
  m_contextMenu->addAction(tr("Tools..."),
			   this,
			   SLOT(slotShowTools(void)));
  m_contextMenu->addSeparator();
  action = m_contextMenu->addAction(tr("Se&parate Canvas..."),
				    this,
				    SLOT(slotSeparate(void)));

  if(qobject_cast<QMainWindow *> (parentWidget()))
    action->setEnabled(false);
  else
    action->setEnabled(true);

  action = m_contextMenu->addAction(tr("&Unite"),
				    this,
				    SLOT(slotUnite(void)));

  if(qobject_cast<glitch_separated_diagram_window *> (parentWidget()))
    action->setEnabled(true);
  else
    action->setEnabled(false);

  m_contextMenu->addSeparator();
  m_contextMenu->addAction(tr("Show Canvas &Settings..."),
			   this,
			   SLOT(slotShowCanvasSettings(void)))->
    setIcon(QIcon::fromTheme("preferences-system"));
  m_contextMenu->addSeparator();
  m_contextMenu->addAction(tr("&User Functions..."),
			   this,
			   SLOT(slotShowUserFunctions(void)));
  return m_contextMenu;
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

bool glitch_view::open(const QString &fileName, QString &error)
{
  QFileInfo fileInfo(fileName);

  if(!fileInfo.isReadable() && !fileInfo.isWritable())
    {
      error = tr("The file must be both readable and writable.");
      return false;
    }

  m_canvasSettings->setFileName(fileName);
  m_canvasSettings->prepare();
  m_delayedWires.clear();
  m_fileName = fileName;
  m_settings = m_canvasSettings->settings();
  disconnect(m_scene,
	     &glitch_scene::changed,
	     this,
	     &glitch_view::slotChanged);
  disconnect(m_scene,
	     &glitch_scene::sceneResized,
	     this,
	     &glitch_view::slotSceneResized);

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

	if(query.exec(QString("SELECT "
			      "myoid, "
			      "parent_oid, "
			      "position, "
			      "SUBSTR(properties, 1, %1), "
			      "SUBSTR(stylesheet, 1, %2), "
			      "SUBSTR(type, 1, %3) "
			      "FROM objects ORDER BY "
			      "parent_oid, properties").
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
		auto id = query.value(0).toLongLong();
		auto point(query.value(2).toString().trimmed());
		auto properties(query.value(3).toString().trimmed());
		auto type(query.value(5).toString().toLower().trimmed());

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
				  (glitch_misc::dbPointToPointF(point));
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
			auto child = glitch_object::createFromValues
			  (values, object, error, object->editView());

			if(child)
			  {
			    child->setCanvasSettings(m_canvasSettings);
			    ids[child->id()] = 0;
			    object->addChild
			      (glitch_misc::dbPointToPointF(point), child);
			    object->hideOrShowOccupied();
			    objects[child->id()] = child;
			  }
		      }
		  }
	      }
	  }
	else
	  {
	    error = tr("An error occurred while accessing the objects table.");
	    ok = false;
	  }

	if(query.exec(QString("SELECT SUBSTR(properties, 1, %1) FROM "
			      "properties").
		      arg(static_cast<int> (Limits::
					    PROPERTIES_MAXIMUM_LENGTH))) &&
	   query.next())
	  {
	    auto list(query.value(0).toString().trimmed().split('&'));

	    for(int i = 0; i < list.size(); i++)
	      {
		auto string(list.at(i));

		if(string.startsWith("splitter_state"))
		  {
		    string = string.mid(string.indexOf('=') + 1);
		    string.remove("\"");
		    m_properties["splitter_state"] = QByteArray::fromBase64
		      (string.toLatin1());
		  }
	      }

	    m_splitter->restoreState
	      (m_properties.value("splitter_state").toByteArray());
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
		      auto wire = new glitch_wire(nullptr);

		      connect(object1->scene(),
			      SIGNAL(changed(const QList<QRectF> &)),
			      wire,
			      SLOT(slotUpdate(const QList<QRectF> &)));
		      m_delayedWires << wire;
		      object1->scene()->addItem(wire);
		      object2->setWiredObject(object1, wire);
		      wire->setBoundingRect(object1->scene()->sceneRect());
		      wire->setColor(m_canvasSettings->wireColor());
		      wire->setLeftProxy(object2->proxy());
		      wire->setRightProxy(object1->proxy());
		      wire->setVisible(false);
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

	auto list(m_scene->items());

	foreach(auto i, list)
	  {
	    auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

	    if(!proxy)
	      continue;

	    auto object = qobject_cast<glitch_object *> (proxy->widget());

	    if(!object)
	      continue;

	    object->save(db, error);

	    if(!error.isEmpty())
	      {
		ok = false;
		break;
	      }
	  }
      }
    else
      error = db.lastError().text();

  done_label:
    db.close();
  }

  error = error.trimmed();
  glitch_common::discardDatabase(connectionName);

  if(ok)
    m_undoStack->setClean();

  QApplication::restoreOverrideCursor();
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

qint64 glitch_view::nextId(void) const
{
  QString connectionName("");
  qint64 id = 0;

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_fileName);

    if(db.open())
      {
	QSqlQuery query(db);

	if(query.exec("INSERT INTO sequence VALUES (NULL)"))
	  {
	    auto variant(query.lastInsertId());

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

void glitch_view::adjustScrollBars(void)
{
  QTimer::singleShot(250, this, SLOT(slotResizeScene(void)));
}

void glitch_view::beginMacro(const QString &text)
{
  if(text.trimmed().isEmpty())
    m_undoStack->beginMacro(tr("unknown"));
  else
    m_undoStack->beginMacro(text);
}

void glitch_view::contextMenuEvent(QContextMenuEvent *event)
{
  if(event)
    emit customContextMenuRequested(event->pos());
  else
    emit customContextMenuRequested(QCursor::pos());
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
  auto point(query.value(1).toString().trimmed());
  auto properties(query.value(2).toString().trimmed());
  auto type(query.value(4).toString().toLower().trimmed());

  values["myoid"] = oid;
  values["parentId"] = query.value(0).toLongLong();
  values["properties"] = properties;
  values["stylesheet"] = query.value(3).toString().trimmed();
  values["type"] = type;

  auto child = glitch_object::createFromValues
    (values, object, error, object->editView());

  if(child)
    {
      child->setCanvasSettings(m_canvasSettings);
      ids[child->id()] = 0;
      object->addChild(glitch_misc::dbPointToPointF(point), child);
      object->hideOrShowOccupied();
      parents[oid] = child;
    }
}

void glitch_view::deleteItems(void)
{
  m_scene->deleteItems();
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
  m_findObjects->showNormal();
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

  m_sourceView->setPlainText(source());

  if(raise)
    {
      m_sourceView->showNormal();
      m_sourceView->activateWindow();
      m_sourceView->raise();
    }
}

void glitch_view::launchProjectIDE(void) const
{
  auto program(m_canvasSettings->projectIDE());
  auto outputFile(m_canvasSettings->outputFile());

  if(QFileInfo(program).isExecutable())
    {
      if(outputFile.length() > 0)
	QProcess::startDetached(program, QStringList() << outputFile);
      else
	QProcess::startDetached(program, QStringList());
    }
  else
    {
      m_canvasSettings->showPage(glitch_canvas_settings::Pages::Project);
      showCanvasSettings();
    }
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
	query.exec("CREATE TABLE IF NOT EXISTS objects ("
		   "myoid INTEGER NOT NULL UNIQUE, "
		   "parent_oid INTEGER NOT NULL DEFAULT -1, "
		   "position TEXT NOT NULL, "
		   "properties TEXT, "
		   "stylesheet TEXT, "
		   "type TEXT NOT NULL, "
		   "PRIMARY KEY (myoid, parent_oid))");
	query.exec("CREATE TABLE IF NOT EXISTS properties "
		   "(properties TEXT)");
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

void glitch_view::push(glitch_undo_command *undoCommand)
{
  if(undoCommand)
    m_undoStack->push(undoCommand);
}

void glitch_view::redo(void)
{
  if(m_undoStack->canRedo())
    {
      m_undoStack->redo();
      slotChanged();
    }
}

void glitch_view::reparent(void)
{
  if(m_findObjects && m_findObjects->isVisible())
    {
      m_findObjects->deleteLater();
      QTimer::singleShot(500, this, &glitch_view::slotShowFind);
    }

  if(m_tools && m_tools->isVisible())
    {
      m_tools->deleteLater();
      QTimer::singleShot(500, this, &glitch_view::slotShowTools);
    }
}

void glitch_view::resizeEvent(QResizeEvent *event)
{
  QWidget::resizeEvent(event);
  setSceneRect(m_view->size());
}

void glitch_view::save(void)
{
  QString error("");

  save(error);
}

void glitch_view::saveProperties(void)
{
  QString connectionName("");

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_fileName);

    if(db.open())
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

	query.exec("DELETE FROM properties");
	query.prepare
	  ("INSERT OR REPLACE INTO properties (properties) VALUES(?)");
	query.addBindValue(string);
	query.exec();
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);
}

void glitch_view::selectAll(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  auto list(m_scene->items());

  foreach(auto i, list)
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

void glitch_view::setTabButton(QPushButton *pushButton)
{
  m_tabButton = pushButton;
}

void glitch_view::showCanvasSettings(void) const
{
  m_canvasSettings->setSettings(m_settings);
  m_canvasSettings->showNormal();
  m_canvasSettings->activateWindow();
  m_canvasSettings->raise();
}

void glitch_view::showTools(void)
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
    }

  m_tools->setOperation
    (glitch_tools::Operations(property("tools-operation").toInt()));
  m_tools->setWindowTitle
    (tr("Glitch: Tools (%1)").arg(m_canvasSettings->name()));
  m_tools->showNormal();
  m_tools->activateWindow();
  m_tools->raise();
  setProperty("tools-operation", QVariant());
}

void glitch_view::showUserFunctions(void) const
{
  slotShowUserFunctions();
}

void glitch_view::slotCanvasSettingsChanged(const bool undo)
{
  auto hash(m_settings);

  if(m_findObjects)
    m_findObjects->setWindowTitle
      (tr("Glitch: Find Objects (%1)").arg(m_canvasSettings->name()));

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

  if(m_tools)
    m_tools->setWindowTitle
      (tr("Glitch: Tools (%1)").arg(m_canvasSettings->name()));

  if(m_undoStack->count() == 0)
    m_undoStack->setUndoLimit(m_canvasSettings->redoUndoStackSize());

  m_userFunctions->setWindowTitle
    (tr("Glitch: User Functions (%1)").arg(m_canvasSettings->name()));
  m_view->setViewportUpdateMode(m_canvasSettings->viewportUpdateMode());

  if(!hash.isEmpty() && hash != m_settings)
    {
      if(undo)
	{
	  auto undoCommand = new glitch_undo_command
	    (hash,
	     glitch_undo_command::CANVAS_SETTINGS_CHANGED,
	     m_canvasSettings);

	  undoCommand->setText(tr("canvas settings changed"));
	  m_undoStack->push(undoCommand);
	}

      slotChanged();
    }
}

void glitch_view::slotChanged(void)
{
  if(m_canvasSettings->generatePeriodically())
    m_generateTimer.start();

  if(m_canvasSettings->generateSourceViewPeriodically())
    m_generateSourceViewTimer.start();

  setSceneRect(m_view->size());
  emit changed();
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
	emit copy(qobject_cast<QGraphicsView *> (scene->views().value(0)));
    }
}

void glitch_view::slotCustomContextMenuRequested(const QPoint &point)
{
  auto menu = defaultContextMenu();

  if(!menu)
    return;

  menu->exec(mapToGlobal(point));
}

void glitch_view::slotDockPropertyEditor(QWidget *widget)
{
  m_dockedWidgetPropertyEditors->add
    (qobject_cast<glitch_floating_context_menu *> (widget));
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
     glitch_undo_command::FUNCTION_RENAMED,
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

void glitch_view::slotFunctionReturnTypeChanged(const QString &after,
						const QString &before,
						glitch_object *object)
{
  Q_UNUSED(after);

  auto undoCommand = new glitch_undo_command
    (before,
     glitch_undo_command::FUNCTION_RETURN_TYPE_CHANGED,
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

void glitch_view::slotPaste(void)
{
  emit paste(this);
}

void glitch_view::slotPreferencesAccepted(void)
{
  QSettings settings;
  auto state = settings.value
    ("preferences/docked_widget_property_editors", true).toBool();

  if(state)
    {
      foreach(auto object, m_scene->objects())
	if(object && object->menu() && object->menu()->isVisible())
	  slotDockPropertyEditor(object->menu());
    }
  else
    m_dockedWidgetPropertyEditors->detach();

  emit preferencesAccepted();
}

void glitch_view::slotResizeScene(void)
{
  setSceneRect(m_view->size());
}

void glitch_view::slotSave(void)
{
  QString error("");

  if(!save(error))
    glitch_misc::showErrorDialog
      (tr("Unable to save %1 (%2).").arg(m_canvasSettings->name()).arg(error),
       this);
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
  dialog.setDirectory(glitch_misc::homePath());
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setNameFilter("Glitch Files (*.db)");
  dialog.setOption(QFileDialog::DontConfirmOverwrite, false);
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowIcon(windowIcon());
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();

      QString error("");

      if(!saveAs(dialog.selectedFiles().value(0), error))
	glitch_misc::showErrorDialog
	  (tr("Unable to save %1 (%2).").
	   arg(m_canvasSettings->name()).arg(error), this);
      else
	emit saved();
    }
  else
    QApplication::processEvents();
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

void glitch_view::slotSeparate(void)
{
  emit separate(this);
}

void glitch_view::slotShowCanvasSettings(void)
{
  showCanvasSettings();
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
  m_userFunctions->showNormal();
  m_userFunctions->activateWindow();
  m_userFunctions->raise();
}

void glitch_view::slotShowWires(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  for(int i = m_delayedWires.size() - 1; i >= 0; i--)
    {
      auto wire = m_delayedWires.at(i);

      if(!wire)
	continue;

      if(wire->leftProxy() &&
	 wire->leftProxy()->isVisible() &&
	 wire->rightProxy() &&
	 wire->rightProxy()->isVisible())
	{
	  m_delayedWires.removeAt(i);
	  wire->setVisible(true);
	}
    }

  if(!m_delayedWires.isEmpty())
    QTimer::singleShot(500, this, &glitch_view::slotShowWires);

  QApplication::restoreOverrideCursor();
}

void glitch_view::slotToolsOperationChanged
(const glitch_tools::Operations operation)
{
  Q_UNUSED(operation);
}

void glitch_view::slotUndoStackChanged(void)
{
  if(m_canvasSettings->generatePeriodically())
    m_generateTimer.start();

  if(m_canvasSettings->generateSourceViewPeriodically())
    m_generateSourceViewTimer.start();

  adjustScrollBars();
  emit changed();
}

void glitch_view::slotUnite(void)
{
  emit unite(this);
}

void glitch_view::undo(void)
{
  if(m_undoStack->canUndo())
    {
      m_undoStack->undo();
      slotChanged();
    }
}
