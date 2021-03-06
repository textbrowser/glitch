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

#include <QScrollBar>
#include <QSqlError>
#include <QSqlQuery>
#include <QtDebug>

#include "Arduino/glitch-object-analog-read-arduino.h"
#include "Arduino/glitch-object-block-comment-arduino.h"
#include "Arduino/glitch-object-function-arduino.h"
#include "Arduino/glitch-object-logical-operator-arduino.h"
#include "glitch-floating-context-menu.h"
#include "glitch-object-view.h"
#include "glitch-object.h"
#include "glitch-scene.h"
#include "glitch-style-sheet.h"
#include "glitch-undo-command.h"
#include "glitch-view.h"

glitch_object::glitch_object(QWidget *parent):glitch_object(1, parent)
{
}

glitch_object::glitch_object(const quint64 id, QWidget *parent):QWidget(nullptr)
{
  m_contextMenu = new glitch_floating_context_menu(parent);
  m_id = id;
  m_parent = parent;
  m_properties[Properties::POSITION_LOCKED] = false;

  auto p = parent;

  do
    {
      if(!p)
	break;

      auto view = qobject_cast<glitch_view *> (p);

      if(view)
	{
	  if(m_id <= 1)
	    m_id = view->nextId();

	  setUndoStack(view->scene()->undoStack());
	  break;
	}

      p = p->parentWidget();
    }
  while(true);
}

glitch_object::~glitch_object()
{
  if(m_contextMenu)
    m_contextMenu->deleteLater();

  qDebug() << "Done!";
}

QPointF glitch_object::scenePos(void) const
{
  if(m_proxy)
    return m_proxy->scenePos();
  else
    return {0.0, 0.0};
}

QPointer<glitch_proxy_widget> glitch_object::proxy(void) const
{
  return m_proxy;
}

QString glitch_object::name(void) const
{
  return m_properties.value(Properties::NAME).toString();
}

QString glitch_object::type(void) const
{
  return m_type;
}

bool glitch_object::hasView(void) const
{
  return false;
}

bool glitch_object::isMandatory(void) const
{
  return false;
}

bool glitch_object::mouseOverScrollBar(const QPointF &point) const
{
  foreach(auto scrollBar, findChildren<QScrollBar *> ())
    if(scrollBar->isSliderDown())
      return true;
    else if(scrollBar->rect().
	    contains(scrollBar->mapFromGlobal(point.toPoint())))
      return true;

  return false;
}

bool glitch_object::positionLocked(void) const
{
  return m_properties.value(Properties::POSITION_LOCKED).toBool();
}

glitch_object *glitch_object::createFromValues
(const QMap<QString, QVariant> &values,
 glitch_object *parentObject,
 QString &error,
 QWidget *parent)
{
  auto type(values.value("type").toString().toLower().trimmed());
  glitch_object *object = nullptr;

  if(type == "arduino-analogread")
    object = glitch_object_analog_read_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-blockcomment")
    object = glitch_object_block_comment_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-function")
    object = glitch_object_function_arduino::createFromValues
      (values, parentObject, error, parent);
  else if(type == "arduino-logicaloperator")
    object = glitch_object_logical_operator_arduino::createFromValues
      (values, error, parent);
  else
    {
      if(type.isEmpty())
	error = tr("Empty object type.");
      else
	error = tr("The type %1 is not supported.").arg(type);
    }

  return object;
}

glitch_object_view *glitch_object::editView(void) const
{
  return nullptr;
}

quint64 glitch_object::id(void) const
{
  return m_id;
}

void glitch_object::addChild(const QPointF &point, glitch_object *object)
{
  Q_UNUSED(object);
  Q_UNUSED(point);
}

void glitch_object::addDefaultActions(QMenu &menu)
{
  createActions();

  QMapIterator<DefaultMenuActions, QAction *> it(m_actions);
  int i = 0;

  while(it.hasNext())
    {
      it.next();
      menu.addAction(it.value());

      if(i++ == 2 && it.hasNext())
	menu.addSeparator();
    }
}

void glitch_object::closeEditWindow(void)
{
}

void glitch_object::createActions(void)
{
  if(!m_actions.contains(DefaultMenuActions::DELETE))
    {
      auto action = new QAction(tr("&Delete"), this);

      action->setData(DefaultMenuActions::DELETE);
      action->setEnabled(!isMandatory());
      connect(action,
	      SIGNAL(triggered(void)),
	      this,
	      SIGNAL(deletedViaContextMenu(void)));
      m_actions[DefaultMenuActions::DELETE] = action;
    }

  if(!m_actions.contains(DefaultMenuActions::LOCK_POSITION))
    {
      auto action = new QAction(tr("&Lock Position"), this);

      action->setCheckable(true);
      action->setChecked
	(m_properties.value(Properties::POSITION_LOCKED).toBool());
      action->setData(DefaultMenuActions::LOCK_POSITION);
      action->setEnabled(!isMandatory());
      connect(action,
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotLockPosition(void)));
      m_actions[DefaultMenuActions::LOCK_POSITION] = action;
    }
  else
    m_actions[DefaultMenuActions::LOCK_POSITION]->
      setChecked(m_properties.value(Properties::POSITION_LOCKED).toBool());

  if(!m_actions.contains(DefaultMenuActions::SET_STYLE_SHEET))
    {
      auto action = new QAction(tr("&Set Style Sheet..."), this);

      action->setData(DefaultMenuActions::SET_STYLE_SHEET);
      connect(action,
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotSetStyleSheet(void)));
      m_actions[DefaultMenuActions::SET_STYLE_SHEET] = action;
    }
}

void glitch_object::move(const QPoint &point)
{
  move(point.x(), point.y());
}

void glitch_object::move(int x, int y)
{
  if(m_properties.value(Properties::POSITION_LOCKED).toBool())
    return;

  auto isChanged = false;

  if(pos().x() != x || pos().y() != y)
    isChanged = true;

  QWidget::move(x, y);

  if(isChanged)
    emit changed();
}

void glitch_object::prepareContextMenu(void)
{
  foreach(auto toolButton, findChildren<QToolButton *> ())
    if(toolButton->objectName() == "context_menu")
      {
	connect(toolButton,
		SIGNAL(clicked(void)),
		this,
		SLOT(slotShowContextMenu(void)),
		Qt::UniqueConnection);
	toolButton->setToolTip(tr("Floating Context Menu"));
	break;
      }
}

void glitch_object::save(const QSqlDatabase &db, QString &error)
{
  QSqlQuery query(db);

  query.prepare("INSERT OR REPLACE INTO objects "
		"(myoid, parent_oid, position, stylesheet, type) "
		"VALUES(?, ?, ?, ?, ?)");
  query.addBindValue(m_id);

  if(qobject_cast<glitch_object_view *> (m_parent))
    query.addBindValue(qobject_cast<glitch_object_view *> (m_parent)->id());
  else
    query.addBindValue(-1);

  query.addBindValue(QString("(%1,%2)").arg(pos().x()).arg(pos().y()));
  query.addBindValue(styleSheet());
  query.addBindValue(m_type);
  query.exec();

  if(query.lastError().isValid())
    error = query.lastError().text();

  if(error.isEmpty())
    saveProperties(QMap<QString, QVariant> (), db, error);
}

void glitch_object::saveProperties(const QMap<QString, QVariant> &p,
				   const QSqlDatabase &db,
				   QString &error)
{
  auto properties(p);

  properties["position_locked"] = m_properties.value
    (Properties::POSITION_LOCKED).toBool();

  QMapIterator<QString, QVariant> it(properties);
  QSqlQuery query(db);
  QString string("");

  while(it.hasNext())
    {
      it.next();
      string.append(it.key());
      string.append(" = ");
      string.append("\"");
      string.append(it.value().toString());
      string.append("\"");

      if(it.hasNext())
	string.append("&");
    }

  query.prepare("UPDATE objects SET properties = ? WHERE myoid = ?");
  query.addBindValue(string);
  query.addBindValue(m_id);
  query.exec();

  if(query.lastError().isValid())
    error = query.lastError().text();
}

void glitch_object::setName(const QString &n)
{
  QString name(n.trimmed());

  if(!name.isEmpty())
    {
      m_contextMenu->setName(name);
      m_properties[Properties::NAME] = name;
    }
}

void glitch_object::setProperties(const QStringList &list)
{
  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.startsWith("comment = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::COMMENT] = string.trimmed();
	}
      else if(string.startsWith("logical_operator = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::LOGICAL_OPERATOR] = string.trimmed();
	}
      else if(string.startsWith("name = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::NAME] = string.trimmed();
	}
      else if(string.startsWith("position_locked = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::POSITION_LOCKED] =
	    QVariant(string.trimmed()).toBool();
	}
    }

  createActions();
}

void glitch_object::setProperty(const Properties property,
				const QVariant &value)
{
  m_properties[property] = value;

  switch(property)
    {
    case Properties::POSITION_LOCKED:
      {
	if(m_actions.contains(DefaultMenuActions::LOCK_POSITION))
	  m_actions.value(DefaultMenuActions::LOCK_POSITION)->setChecked
	    (value.toBool());

	if(m_proxy)
	  m_proxy->setFlag(QGraphicsItem::ItemIsMovable, !value.toBool());

	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_object::setProxy(const QPointer<glitch_proxy_widget> &proxy)
{
  m_proxy = proxy;

  if(m_proxy)
    m_proxy->setFlag
      (QGraphicsItem::ItemIsMovable,
       !m_properties.value(Properties::POSITION_LOCKED).toBool());
}

void glitch_object::setUndoStack(QUndoStack *undoStack)
{
  if(m_editView)
    m_editView->scene()->setUndoStack(undoStack);

  m_undoStack = undoStack;
}

void glitch_object::simulateDelete(void)
{
  m_contextMenu->close();
}

void glitch_object::slotLockPosition(void)
{
  if(m_undoStack)
    {
      auto undoCommand = new glitch_undo_command
	(!m_properties.value(Properties::POSITION_LOCKED).toBool(),
	 m_properties.value(Properties::POSITION_LOCKED),
	 glitch_undo_command::PROPERTY_CHANGED,
	 Properties::POSITION_LOCKED,
	 this);

      undoCommand->setText
	(tr("item property changed (%1, %2)").
	 arg(scenePos().x()).arg(scenePos().y()));
      m_undoStack->push(undoCommand);
    }
  else
    m_properties[Properties::POSITION_LOCKED] =
      !m_properties.value(Properties::POSITION_LOCKED).toBool();

  emit changed();
}

void glitch_object::slotSetStyleSheet(void)
{
  QScopedPointer<glitch_style_sheet> dialog(new glitch_style_sheet(m_parent));
  auto string(styleSheet());

  dialog->setWidget(this);
  QApplication::processEvents();

  if(dialog->exec() == QDialog::Accepted)
    {
      setStyleSheet(dialog->styleSheet());

      if(m_undoStack)
	{
	  auto undoCommand = new glitch_undo_command
	    (string, glitch_undo_command::STYLESHEET_CHANGED, this);

	  undoCommand->setText
	    (tr("stylesheet changed (%1, %2)").
	     arg(scenePos().x()).arg(scenePos().y()));
	  m_undoStack->push(undoCommand);
	}

      emit changed();
    }
  else
    setStyleSheet(string);
}

void glitch_object::slotShowContextMenu(void)
{
  QMenu menu; // Not used.

  addActions(menu);
  m_contextMenu->addActions(m_actions.values());
  m_contextMenu->setName(name());
  m_contextMenu->show();
}
