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
**    derived from GlowBot without specific prior written permission.
**
** GLOWBOT IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** GLOWBOT, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QDir>
#include <QFileDialog>
#include <QMainWindow>
#include <QMenu>
#include <QResizeEvent>
#include <QScrollBar>
#include <QSqlError>
#include <QSqlQuery>

#include "Arduino/glowbot-object-function-arduino.h"
#include "glowbot-alignment.h"
#include "glowbot-graphicsview.h"
#include "glowbot-misc.h"
#include "glowbot-object.h"
#include "glowbot-proxy-widget.h"
#include "glowbot-scene.h"
#include "glowbot-separated-diagram-window.h"
#include "glowbot-undo-command.h"
#include "glowbot-user-functions.h"
#include "glowbot-view.h"

glowbot_view::glowbot_view
(const QString &fileName,
 const QString &name,
 const glowbot_common::ProjectType projectType,
 QWidget *parent):QWidget(parent)
{
  m_ui.setupUi(this);
  m_alignment = new glowbot_alignment(this);
  m_canvasSettings = new glowbot_canvas_settings(this);
  m_canvasSettings->setFileName(fileName);
  m_canvasSettings->prepare();
  m_changed = false;
  m_fileName = fileName;
  m_menuAction = new QAction
    (QIcon(":/Logo/glowbot-arduino-logo.png"), name, this);
  m_name = name;
  m_projectType = projectType;
  m_scene = new glowbot_scene(m_projectType, this);
  m_scene->setBackgroundBrush(QBrush(QColor(211, 211, 211), Qt::SolidPattern));
  m_scene->setMainScene(true);
  m_scene->setUndoStack(m_undoStack = new QUndoStack(this));
  m_startObject = nullptr;
  m_userFunctions = new glowbot_user_functions(this);
  m_userFunctions->setProjectType(m_projectType);
  m_view = new glowbot_graphicsview(this);
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
	  SIGNAL(destroyed(QObject *)),
	  this,
	  SLOT(slotSceneObjectDestroyed(QObject *)));
  connect(m_scene,
	  SIGNAL(functionAdded(const QString &)),
	  this,
	  SLOT(slotFunctionAdded(const QString &)));
  connect(m_scene,
	  SIGNAL(functionNameChanged(const QString &, const QString &)),
	  this,
	  SLOT(slotFunctionNameChanged(const QString &, const QString &)));
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
  connect(this,
	  SIGNAL(customContextMenuRequested(const QPoint &)),
	  this,
	  SLOT(slotCustomContextMenuRequested(const QPoint &)));
  layout()->addWidget(m_view);
  prepareDatabaseTables(m_fileName);
}

glowbot_view::~glowbot_view()
{
  /*
  ** Do not emit signals.
  */

  m_scene->blockSignals(true);
  m_view->blockSignals(true);

  while(m_undoStack->canUndo())
    m_undoStack->undo();
}

QAction *glowbot_view::menuAction(void) const
{
  return m_menuAction;
}

QList<glowbot_object *> glowbot_view::objects(void) const
{
  return m_scene->objects();
}

QList<glowbot_object *> glowbot_view::selectedObjects(void) const
{
  return m_scene->selectedObjects();
}

QMenu *glowbot_view::defaultContextMenu(void)
{
  /*
  ** The returned object must be destroyed!
  */

  QAction *action = nullptr;
  QMenu *menu = new QMenu(this);

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

  if(m_projectType == glowbot_common::ArduinoProject)
    menu->addAction(tr("Arduino Structures..."),
		    this,
		    SIGNAL(showStructures(void)));
  else
    menu->addAction(tr("Structures..."),
		    this,
		    SIGNAL(showStructures(void)));

  menu->addSeparator();
  action = menu->addAction(tr("Se&parate..."),
			   this,
			   SLOT(slotSeparate(void)));

  if(qobject_cast<QMainWindow *> (parentWidget()))
    action->setEnabled(false);
  else
    action->setEnabled(true);

  action = menu->addAction(tr("&Unite"),
			   this,
			   SLOT(slotUnite(void)));

  if(qobject_cast<glowbot_separated_diagram_window *> (parentWidget()))
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

QString glowbot_view::name(void) const
{
  return m_name;
}

QString glowbot_view::redoText(void) const
{
  return m_undoStack->redoText();
}

QString glowbot_view::undoText(void) const
{
  return m_undoStack->undoText();
}

QUndoStack *glowbot_view::undoStack(void) const
{
  return m_undoStack;
}

bool glowbot_view::canRedo(void) const
{
  return m_undoStack->canRedo();
}

bool glowbot_view::canUndo(void) const
{
  return m_undoStack->canUndo();
}

bool glowbot_view::hasChanged(void) const
{
  return m_changed;
}

bool glowbot_view::open(const QString &fileName, QString &error)
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
    QSqlDatabase db(glowbot_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(fileName);

    if((ok = db.open()))
      {
	QSqlQuery query(db);

	query.setForwardOnly(true);

	if(query.exec("SELECT myoid, parent_oid, position, properties, "
		      "stylesheet, type FROM objects ORDER BY parent_oid"))
	  while(query.next())
	    {
	      QMap<QString, QVariant> values;
	      QString point(query.value(2).toString().trimmed());
	      QString properties(query.value(3).toString().trimmed());
	      QString type(query.value(5).toString().toLower().trimmed());
	      quint64 id = query.value(0).toULongLong();

	      values["myoid"] = id;
	      values["properties"] = properties;
	      values["stylesheet"] = query.value(4).toString().trimmed();
	      values["type"] = type;

	      if(query.value(1).toLongLong() == -1)
		{
		  QString error("");
		  glowbot_object *object = glowbot_object::createFromValues
		    (values, error, this);

		  if(object)
		    {
		      glowbot_proxy_widget *proxy = m_scene->addObject(object);

		      if(proxy)
			{
			  m_scene->addItem(proxy);
			  object->setUndoStack(m_scene->undoStack());
			  proxy->setPos(glowbot_misc::dbPointToPointF(point));
			}
		      else
			object->deleteLater();
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

  glowbot_common::discardDatabase(connectionName);
  connect(m_scene,
	  SIGNAL(changed(void)),
	  this,
	  SLOT(slotChanged(void)));
  return ok;
}

bool glowbot_view::save(QString &error)
{
  return saveImplementation(m_fileName, error);
}

bool glowbot_view::saveAs(const QString &fileName, QString &error)
{
  return saveImplementation(fileName, error);
}

bool glowbot_view::saveImplementation(const QString &fileName, QString &error)
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
    QSqlDatabase db(glowbot_common::sqliteDatabase());

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
	query.addBindValue(glowbot_common::projectTypeToString(m_projectType));
	ok = query.exec();

	if(!ok)
	  {
	    error = query.lastError().text();
	    goto done_label;
	  }

	query.exec("DELETE FROM objects");
	query.exec("DELETE FROM wires");

	QList<QGraphicsItem *> list(m_scene->items());

	for(int i = 0; i < list.size(); i++)
	  {
	    glowbot_proxy_widget *proxy =
	      qgraphicsitem_cast<glowbot_proxy_widget *> (list.at(i));

	    if(!proxy)
	      continue;

	    glowbot_object *widget = qobject_cast<glowbot_object *>
	      (proxy->widget());

	    if(!widget)
	      continue;

	    widget->save(db, error);

	    if(!error.isEmpty())
	      break;
	  }
      }
    else
      error = db.lastError().text();

  done_label:
    db.close();
  }

  glowbot_common::discardDatabase(connectionName);
  m_changed = !ok;
  QApplication::restoreOverrideCursor();
  return ok;
}

glowbot_common::ProjectType glowbot_view::projectType(void) const
{
  return m_projectType;
}

glowbot_graphicsview *glowbot_view::view(void) const
{
  return m_view;
}

glowbot_scene *glowbot_view::scene(void) const
{
  return m_scene;
}

quint64 glowbot_view::nextId(void) const
{
  QString connectionName("");
  quint64 id = 0;

  {
    QSqlDatabase db(glowbot_common::sqliteDatabase());

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

  glowbot_common::discardDatabase(connectionName);
  return id;
}

void glowbot_view::beginMacro(const QString &text)
{
  if(text.trimmed().isEmpty())
    m_undoStack->beginMacro(tr("unknown"));
  else
    m_undoStack->beginMacro(text);
}

void glowbot_view::contextMenuEvent(QContextMenuEvent *event)
{
  if(event && m_view->items(event->pos()).isEmpty())
    {
      event->ignore();
      emit customContextMenuRequested(event->pos());
    }
  else
    QWidget::contextMenuEvent(event);
}

void glowbot_view::deleteItems(void)
{
  m_scene->deleteItems();
}

void glowbot_view::endMacro(void)
{
  m_undoStack->endMacro();
}

void glowbot_view::prepareDatabaseTables(const QString &fileName) const
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QString connectionName("");

  {
    QSqlDatabase db(glowbot_common::sqliteDatabase());

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
		   "PRIMARY KEY (parent_oid, position))");
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

  glowbot_common::discardDatabase(connectionName);
  QApplication::restoreOverrideCursor();
}

void glowbot_view::push(glowbot_undo_command *undoCommand)
{
  if(undoCommand)
    m_undoStack->push(undoCommand);
}

void glowbot_view::redo(void)
{
  if(m_undoStack->canRedo())
    {
      m_changed = true;
      m_undoStack->redo();
      emit changed();
    }
}

void glowbot_view::resizeEvent(QResizeEvent *event)
{
  QWidget::resizeEvent(event);
  setSceneRect(m_view->size());
}

void glowbot_view::save(void)
{
  QString error("");

  save(error);
}

void glowbot_view::selectAll(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QList<QGraphicsItem *> list(m_scene->items());

  for(int i = 0; i < list.size(); i++)
    {
      glowbot_proxy_widget *proxy =
	qgraphicsitem_cast<glowbot_proxy_widget *> (list.at(i));

      if(proxy)
	proxy->setSelected(true);
    }

  QApplication::restoreOverrideCursor();
}

void glowbot_view::setSceneRect(const QSize &size)
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

void glowbot_view::showAlignment(void)
{
  m_alignment->show();
}

void glowbot_view::slotCanvasSettingsChanged(const bool undo)
{
  /*
  ** Canvas settings are applied immediately.
  */

  QHash<glowbot_canvas_settings::Settings, QVariant> hash(m_settings);

  m_name = m_canvasSettings->name();
  m_scene->setBackgroundBrush
    (QBrush(m_canvasSettings->canvasBackgroundColor(), Qt::SolidPattern));
  m_settings[glowbot_canvas_settings::CANVAS_BACKGROUND_COLOR] =
    m_canvasSettings->canvasBackgroundColor();
  m_settings[glowbot_canvas_settings::CANVAS_NAME] = m_canvasSettings->name();
  m_settings[glowbot_canvas_settings::VIEW_UPDATE_MODE] =
    m_canvasSettings->viewportUpdateMode();
  m_view->setViewportUpdateMode(m_canvasSettings->viewportUpdateMode());

  if(hash != m_settings && !hash.isEmpty())
    {
      if(undo)
	{
	  glowbot_undo_command *undoCommand = new glowbot_undo_command
	    (hash,
	     glowbot_undo_command::CANVAS_SETTINGS_CHANGED,
	     m_canvasSettings);

	  undoCommand->setText(tr("canvas settings changed"));
	  m_undoStack->push(undoCommand);
	}

      m_changed = true;
      emit changed();
    }
}

void glowbot_view::slotChanged(void)
{
  m_changed = true;
  emit changed();
}

void glowbot_view::slotCustomContextMenuRequested(const QPoint &point)
{
  QMenu *menu = defaultContextMenu();

  if(!menu)
    return;

  menu->exec(mapToGlobal(point));
  menu->deleteLater();
}

void glowbot_view::slotFunctionAdded(const QString &name)
{
  m_userFunctions->addFunction(name);
}

void glowbot_view::slotFunctionNameChanged(const QString &before,
					   const QString &after)
{
  m_userFunctions->renameFunction(before, after);
}

void glowbot_view::slotSave(void)
{
  QString error("");

  if(!save(error))
    glowbot_misc::showErrorDialog
      (tr("Unable to save %1 (%2).").arg(m_name).arg(error), this);
  else
    emit saved();
}

void glowbot_view::slotSaveAs(void)
{
  QFileDialog dialog(this, tr("GlowBot: Save Current Diagram As"));

  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setConfirmOverwrite(true);
  dialog.setDirectory(glowbot_misc::homePath());
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setNameFilter("GlowBot Files (*.db)");
  dialog.setWindowIcon(windowIcon());

  if(dialog.exec() == QDialog::Accepted)
    {
      QString error("");

      if(!saveAs(dialog.selectedFiles().value(0), error))
	glowbot_misc::showErrorDialog
	  (tr("Unable to save %1 (%2).").arg(m_name).arg(error), this);
      else
	emit saved();
    }
}

void glowbot_view::slotSceneObjectDestroyed(QObject *object)
{
  if(!object)
    return;

  if(object->objectName() == "glowbot_object_function_arduino")
    m_userFunctions->deleteFunction
      (object->property("function_name").toString());

  m_changed = true;
  emit changed();
}

void glowbot_view::slotSceneResized(void)
{
  setSceneRect(size());
}

void glowbot_view::slotSeparate(void)
{
  emit separate(this);
}

void glowbot_view::slotShowAlignmentTool(void)
{
  showAlignment();
}

void glowbot_view::slotShowCanvasSettings(void)
{
  m_canvasSettings->setName(m_name);
  m_canvasSettings->setViewportUpdateMode(m_view->viewportUpdateMode());
  m_canvasSettings->show();
}

void glowbot_view::slotShowUserFunctions(void)
{
  m_userFunctions->show();
}

void glowbot_view::slotUnite(void)
{
  emit unite(this);
}

void glowbot_view::undo(void)
{
  if(m_undoStack->canUndo())
    {
      m_changed = true;
      m_undoStack->undo();
      emit changed();
    }
}
