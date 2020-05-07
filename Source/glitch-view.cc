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

#include <QDir>
#include <QFileDialog>
#include <QMainWindow>
#include <QMenu>
#include <QResizeEvent>
#include <QScrollBar>
#include <QSqlError>
#include <QSqlQuery>

#include "Arduino/glitch-object-function-arduino.h"
#include "glitch-alignment.h"
#include "glitch-graphicsview.h"
#include "glitch-misc.h"
#include "glitch-object.h"
#include "glitch-object-view.h"
#include "glitch-proxy-widget.h"
#include "glitch-scene.h"
#include "glitch-separated-diagram-window.h"
#include "glitch-undo-command.h"
#include "glitch-user-functions.h"
#include "glitch-view.h"

glitch_view::glitch_view
(const QString &fileName,
 const QString &name,
 const glitch_common::ProjectType projectType,
 QWidget *parent):QWidget(parent)
{
  m_ui.setupUi(this);
  m_alignment = new glitch_alignment(this);
  m_canvasSettings = new glitch_canvas_settings(this);
  m_canvasSettings->setFileName(fileName);
  m_canvasSettings->prepare();
  m_changed = false;
  m_fileName = fileName;
  m_menuAction = new QAction
    (QIcon(":/Logo/glitch-arduino-logo.png"), name, this);
  m_name = name;
  m_projectType = projectType;
  m_scene = new glitch_scene(m_projectType, this);
  m_scene->setBackgroundBrush(QBrush(QColor(211, 211, 211), Qt::SolidPattern));
  m_scene->setMainScene(true);
  m_scene->setUndoStack(m_undoStack = new QUndoStack(this));
  m_userFunctions = new glitch_user_functions(this);
  m_userFunctions->setProjectType(m_projectType);
  m_view = new glitch_graphicsview(this);
  m_view->setDragMode(QGraphicsView::RubberBandDrag);
  m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  m_view->setInteractive(true);
  m_view->setRenderHints(QPainter::Antialiasing |
			 QPainter::HighQualityAntialiasing | // OpenGL?
			 QPainter::SmoothPixmapTransform |
			 QPainter::TextAntialiasing);
  m_view->setRubberBandSelectionMode(Qt::IntersectsItemShape);
  m_view->setScene(m_scene);
  m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  connect(m_alignment,
	  SIGNAL(changed(void)),
	  this,
	  SLOT(slotChanged(void)));
  connect(m_canvasSettings,
	  SIGNAL(accepted(const bool)),
	  this,
	  SLOT(slotCanvasSettingsChanged(const bool)));
  connect(m_scene,
	  SIGNAL(changed(void)),
	  this,
	  SLOT(slotChanged(void)));
  connect(m_scene,
	  SIGNAL(copy(glitch_scene *)),
	  this,
	  SIGNAL(copy(glitch_scene *)));
  connect(m_scene,
	  SIGNAL(destroyed(QObject *)),
	  this,
	  SLOT(slotSceneObjectDestroyed(QObject *)));
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
	  SIGNAL(sceneResized(void)),
	  this,
	  SLOT(slotSceneResized(void)));
  connect(m_scene,
	  SIGNAL(selectionChanged(void)),
	  this,
	  SIGNAL(selectionChanged(void)));
  connect(m_view,
	  SIGNAL(mouseEnterEvent(void)),
	  this,
	  SIGNAL(mouseEnterEvent(void)));
  connect(m_view,
	  SIGNAL(mouseLeaveEvent(void)),
	  this,
	  SIGNAL(mouseLeaveEvent(void)));
  connect(m_view,
	  SIGNAL(customContextMenuRequested(const QPoint &)),
	  this,
	  SLOT(slotCustomContextMenuRequested(const QPoint &)));
  connect(this,
	  SIGNAL(customContextMenuRequested(const QPoint &)),
	  this,
	  SLOT(slotCustomContextMenuRequested(const QPoint &)));
  layout()->addWidget(m_view);
  prepareDatabaseTables(m_fileName);
  prepareDefaultActions();
}

glitch_view::~glitch_view()
{
  m_scene->purgeRedoUndoProxies();
}

QAction *glitch_view::menuAction(void) const
{
  return m_menuAction;
}

QList<QAction *> glitch_view::defaultActions(void) const
{
  return m_defaultActions;
}

QList<glitch_object *> glitch_view::objects(void) const
{
  return m_scene->objects();
}

QList<glitch_object *> glitch_view::selectedObjects(void) const
{
  return m_scene->selectedObjects();
}

QMenu *glitch_view::defaultContextMenu(void)
{
  /*
  ** The returned object must be destroyed!
  */

  QAction *action = nullptr;
  auto *menu = new QMenu(this);

  action = menu->addAction(tr("&Save"),
			   this,
			   SLOT(slotSave(void)));
  action->setEnabled(hasChanged());
  menu->addAction(tr("Save &As..."),
		  this,
		  SLOT(slotSaveAs(void)));
  menu->addSeparator();
  menu->addAction(tr("Alignment Tool..."),
		  this,
		  SLOT(slotShowAlignmentTool(void)));

  if(m_projectType == glitch_common::ArduinoProject)
    menu->addAction(tr("Arduino Structures..."),
		    this,
		    SIGNAL(showStructures(void)));
  else
    menu->addAction(tr("Structures..."),
		    this,
		    SIGNAL(showStructures(void)));

  menu->addSeparator();
  action = menu->addAction(tr("Se&parate Canvas..."),
			   this,
			   SLOT(slotSeparate(void)));

  if(qobject_cast<QMainWindow *> (parentWidget()))
    action->setEnabled(false);
  else
    action->setEnabled(true);

  action = menu->addAction(tr("&Unite"),
			   this,
			   SLOT(slotUnite(void)));

  if(qobject_cast<glitch_separated_diagram_window *> (parentWidget()))
    action->setEnabled(true);
  else
    action->setEnabled(false);

  menu->addSeparator();
  menu->addAction(tr("Show Canvas &Settings..."),
		  this,
		  SLOT(slotShowCanvasSettings(void)));
  menu->addSeparator();
  menu->addAction(tr("&User Functions..."),
		  this,
		  SLOT(slotShowUserFunctions(void)));
  return menu;
}

QString glitch_view::name(void) const
{
  return m_name;
}

QString glitch_view::redoText(void) const
{
  return m_undoStack->redoText();
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

bool glitch_view::hasChanged(void) const
{
  return m_changed;
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
  m_fileName = fileName;
  disconnect(m_scene,
	     SIGNAL(changed(void)),
	     this,
	     SLOT(slotChanged(void)));

  QString connectionName("");
  bool ok = true;

  {
    QSqlDatabase db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(fileName);

    if((ok = db.open()))
      {
	QSqlQuery query(db);

	query.setForwardOnly(true);

	if(query.exec("SELECT myoid, parent_oid, position, properties, "
		      "stylesheet, type FROM objects ORDER BY "
		      "parent_oid, properties"))
	  {
	    QHash<quint64, glitch_object *> parents;

	    while(query.next())
	      {
		QMap<QString, QVariant> values;
		QString point(query.value(2).toString().trimmed());
		QString properties(query.value(3).toString().trimmed());
		QString type(query.value(5).toString().toLower().trimmed());
		quint64 id = query.value(0).toULongLong();

		values["myoid"] = id;
		values["parentId"] = query.value(1).toLongLong();
		values["properties"] = properties;
		values["stylesheet"] = query.value(4).toString().trimmed();
		values["type"] = type;

		if(query.value(1).toLongLong() == -1)
		  {
		    QString error("");
		    auto *object = glitch_object::createFromValues
		      (values, error, this);

		    if(object)
		      {
			auto *proxy = m_scene->addObject(object);

			if(proxy)
			  {
			    m_scene->addItem(proxy);
			    object->setUndoStack(m_scene->undoStack());
			    parents[id] = object;
			    proxy->setPos(glitch_misc::dbPointToPointF(point));
			  }
			else
			  object->deleteLater();
		      }
		  }
		else
		  {
		    auto *object = parents.value(query.value(1).toULongLong());

		    if(object && object->editView())
		      object->addChild
			(glitch_misc::
			 dbPointToPointF(point),
			 glitch_object::
			 createFromValues(values, error, object->editView()));
		  }
	      }
	  }
	else
	  {
	    error = tr("An error occurred while accessing the objects table.");
	    ok = false;
	  }
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);
  connect(m_scene,
	  SIGNAL(changed(void)),
	  this,
	  SLOT(slotChanged(void)));
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

  prepareDatabaseTables(fileName);
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QString connectionName("");
  bool ok = true;

  {
    QSqlDatabase db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(fileName);

    if((ok = db.open()))
      {
	QSqlQuery query(db);

	ok = query.exec("DELETE FROM diagram");

	if(!ok)
	  {
	    error = query.lastError().text();
	    goto done_label;
	  }

	query.prepare("INSERT OR REPLACE INTO diagram "
		      "(name, type) "
		      "VALUES (?, ?)");
	query.addBindValue(m_name);
	query.addBindValue(glitch_common::projectTypeToString(m_projectType));
	ok = query.exec();

	if(!ok)
	  {
	    error = query.lastError().text();
	    goto done_label;
	  }

	query.exec("DELETE FROM objects");
	query.exec("DELETE FROM wires");

	QList<QGraphicsItem *> list(m_scene->items());

	for(auto i : list)
	  {
	    auto *proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

	    if(!proxy)
	      continue;

	    auto *object = qobject_cast<glitch_object *> (proxy->widget());

	    if(!object)
	      continue;

	    object->save(db, error);

	    if(!error.isEmpty())
	      break;
	  }
      }
    else
      error = db.lastError().text();

  done_label:
    db.close();
  }

  glitch_common::discardDatabase(connectionName);
  m_changed = !ok;
  QApplication::restoreOverrideCursor();
  return ok;
}

glitch_common::ProjectType glitch_view::projectType(void) const
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

quint64 glitch_view::nextId(void) const
{
  QString connectionName("");
  quint64 id = 0;

  {
    QSqlDatabase db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(m_fileName);

    if(db.open())
      {
	QSqlQuery query(db);

	if(query.exec("INSERT INTO sequence VALUES (NULL)"))
	  {
	    QVariant variant(query.lastInsertId());

	    if(variant.isValid())
	      {
		id = variant.toULongLong();
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

void glitch_view::beginMacro(const QString &text)
{
  if(text.trimmed().isEmpty())
    m_undoStack->beginMacro(tr("unknown"));
  else
    m_undoStack->beginMacro(text);
}

void glitch_view::contextMenuEvent(QContextMenuEvent *event)
{
  if(event && m_view->items(event->pos()).isEmpty())
    {
      event->ignore();
      emit customContextMenuRequested(event->pos());
    }
  else
    QWidget::contextMenuEvent(event);
}

void glitch_view::deleteItems(void)
{
  m_scene->deleteItems();
}

void glitch_view::endMacro(void)
{
  m_undoStack->endMacro();
}

void glitch_view::prepareDatabaseTables(const QString &fileName) const
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QString connectionName("");

  {
    QSqlDatabase db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(fileName);

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
	query.exec("CREATE TABLE IF NOT EXISTS sequence ("
		   "value INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT)");
	query.exec("CREATE TABLE IF NOT EXISTS wires ("
		   "object_input_oid INTEGER NOT NULL, "
		   "object_output_oid INTEGER NOT NULL, "
		   "parent_oid INTEGER NOT NULL, "
		   "PRIMARY KEY (object_input_oid, "
		   "object_output_oid, parent_oid))");
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
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSeparate(void)));
  m_defaultActions << action;
  action = new QAction(tr("&User Functions..."), this);
  connect(action,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowUserFunctions(void)));
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
      m_changed = true;
      m_undoStack->redo();
      emit changed();
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

void glitch_view::selectAll(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QList<QGraphicsItem *> list(m_scene->items());

  for(auto i : list)
    {
      auto *proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(proxy)
	proxy->setSelected(true);
    }

  QApplication::restoreOverrideCursor();
}

void glitch_view::setSceneRect(const QSize &size)
{
  Q_UNUSED(size);

  QRectF b(m_scene->itemsBoundingRect());

  b.setTopLeft(QPointF(0, 0));
  m_scene->setSceneRect
    (0,
     0,
     qMax(static_cast<int> (b.width()),
	  m_view->width() - 2 * m_view->frameWidth()),
     qMax(static_cast<int> (b.height()),
	  m_view->height() - 2 * m_view->frameWidth()));
}

void glitch_view::showAlignment(void)
{
  m_alignment->show();
}

void glitch_view::showCanvasSettings(void)
{
  m_canvasSettings->setName(m_name);
  m_canvasSettings->setViewportUpdateMode(m_view->viewportUpdateMode());
  m_canvasSettings->show();
}

void glitch_view::slotCanvasSettingsChanged(const bool undo)
{
  /*
  ** Canvas settings are applied immediately.
  */

  QHash<glitch_canvas_settings::Settings, QVariant> hash(m_settings);

  m_name = m_canvasSettings->name();
  m_scene->setBackgroundBrush
    (QBrush(m_canvasSettings->canvasBackgroundColor(), Qt::SolidPattern));
  m_settings[glitch_canvas_settings::CANVAS_BACKGROUND_COLOR] =
    m_canvasSettings->canvasBackgroundColor();
  m_settings[glitch_canvas_settings::CANVAS_NAME] = m_canvasSettings->name();
  m_settings[glitch_canvas_settings::VIEW_UPDATE_MODE] =
    m_canvasSettings->viewportUpdateMode();
  m_view->setViewportUpdateMode(m_canvasSettings->viewportUpdateMode());

  if(hash != m_settings && !hash.isEmpty())
    {
      if(undo)
	{
	  auto *undoCommand = new glitch_undo_command
	    (hash,
	     glitch_undo_command::CANVAS_SETTINGS_CHANGED,
	     m_canvasSettings);

	  undoCommand->setText(tr("canvas settings changed"));
	  m_undoStack->push(undoCommand);
	}

      m_changed = true;
      emit changed();
    }
}

void glitch_view::slotChanged(void)
{
  m_changed = true;
  emit changed();
}

void glitch_view::slotCustomContextMenuRequested(const QPoint &point)
{
  auto *menu = defaultContextMenu();

  if(!menu)
    return;

  menu->exec(mapToGlobal(point));
  menu->deleteLater();
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

  auto *undoCommand = new glitch_undo_command
    (before,
     glitch_undo_command::FUNCTION_RENAMED,
     object,
     m_userFunctions);

  undoCommand->setText(tr("function renamed"));
  m_undoStack->push(undoCommand);
  emit changed();
}

void glitch_view::slotFunctionReturnTypeChanged(const QString &after,
						const QString &before,
						glitch_object *object)
{
  Q_UNUSED(after);

  auto *undoCommand = new glitch_undo_command
    (before,
     glitch_undo_command::FUNCTION_RETURN_TYPE_CHANGED,
     object,
     m_userFunctions);

  undoCommand->setText(tr("function return type changed"));
  m_undoStack->push(undoCommand);
  emit changed();
}

void glitch_view::slotSave(void)
{
  QString error("");

  if(!save(error))
    glitch_misc::showErrorDialog
      (tr("Unable to save %1 (%2).").arg(m_name).arg(error), this);
  else
    emit saved();
}

void glitch_view::slotSaveAs(void)
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

      if(!saveAs(dialog.selectedFiles().value(0), error))
	glitch_misc::showErrorDialog
	  (tr("Unable to save %1 (%2).").arg(m_name).arg(error), this);
      else
	emit saved();
    }
}

void glitch_view::slotSceneObjectDestroyed(QObject *object)
{
  if(!object)
    return;

  if(qobject_cast<glitch_object_function_arduino *> (object))
    m_userFunctions->deleteFunction
      (qobject_cast<glitch_object_function_arduino *> (object)->name());

  m_changed = true;
  emit changed();
}

void glitch_view::slotSceneResized(void)
{
  setSceneRect(size());
}

void glitch_view::slotSeparate(void)
{
  emit separate(this);
}

void glitch_view::slotShowAlignmentTool(void)
{
  showAlignment();
}

void glitch_view::slotShowCanvasSettings(void)
{
  showCanvasSettings();
}

void glitch_view::slotShowUserFunctions(void)
{
  m_userFunctions->show();
}

void glitch_view::slotUnite(void)
{
  emit unite(this);
}

void glitch_view::undo(void)
{
  if(m_undoStack->canUndo())
    {
      m_changed = true;
      m_undoStack->undo();
      emit changed();
    }
}
