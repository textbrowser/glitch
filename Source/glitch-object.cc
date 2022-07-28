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

#include "Arduino/glitch-object-advanced-io-arduino.h"
#include "Arduino/glitch-object-analog-io-arduino.h"
#include "Arduino/glitch-object-arithmetic-operator-arduino.h"
#include "Arduino/glitch-object-bits-and-bytes-arduino.h"
#include "Arduino/glitch-object-bitwise-operator-arduino.h"
#include "Arduino/glitch-object-block-comment-arduino.h"
#include "Arduino/glitch-object-constant-arduino.h"
#include "Arduino/glitch-object-digital-io-arduino.h"
#include "Arduino/glitch-object-flow-control-arduino.h"
#include "Arduino/glitch-object-function-arduino.h"
#include "Arduino/glitch-object-logical-operator-arduino.h"
#include "Arduino/glitch-object-loop-arduino.h"
#include "Arduino/glitch-object-serial-arduino.h"
#include "Arduino/glitch-object-setup-arduino.h"
#include "Arduino/glitch-object-time-arduino.h"
#include "Arduino/glitch-object-variable-arduino.h"
#include "Arduino/glitch-view-arduino.h"
#include "glitch-floating-context-menu.h"
#include "glitch-object-edit-window.h"
#include "glitch-object-view.h"
#include "glitch-object.h"
#include "glitch-scene.h"
#include "glitch-style-sheet.h"
#include "glitch-undo-command.h"
#include "glitch-view.h"
#include "glitch-wire.h"

QRegularExpression glitch_object::s_splitRegularExpression =
  QRegularExpression("&(?=([^\"]*\"[^\"]*\")*[^\"]*$)");

inline static bool order_less_than(glitch_object *o1, glitch_object *o2)
{
  if(!o1 || !o1->proxy() || !o1->scene() || !o2 || !o2->proxy() || !o2->scene())
    return false;
  else
    return o1->scene()->objectOrder(o1->proxy()) <
      o2->scene()->objectOrder(o2->proxy());
}

glitch_object::glitch_object(QWidget *parent):glitch_object(1, parent)
{
}

glitch_object::glitch_object(const qint64 id, QWidget *parent):
  QWidget(nullptr), m_id(id)
{
  m_contextMenu = new glitch_floating_context_menu(parent);
  m_contextMenu->setObject(this);
  m_drawInputConnector = false;
  m_drawOutputConnector = false;
  m_parent = parent;
  m_properties[Properties::POSITION_LOCKED] = false;

  {
    auto view = qobject_cast<glitch_object_view *> (parent);

    if(view)
      {
	connect(view,
		&glitch_object_view::closed,
		m_contextMenu,
		&glitch_floating_context_menu::close);
	setUndoStack(view->scene()->undoStack());
      }
  }

  {
    auto view = qobject_cast<glitch_view *> (parent);

    if(view)
      setUndoStack(view->scene()->undoStack());
  }

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

	  break;
	}

      p = p->parentWidget();
    }
  while(true);
}

glitch_object::~glitch_object()
{
  for(auto object : m_copiedChildren)
    if(object)
      object->deleteLater();

  m_contextMenu->deleteLater();

  if(m_editView)
    m_editView->deleteLater();

  if(m_editWindow)
    m_editWindow->deleteLater();

  qDebug() << "Done (glitch_object)!";
}

QPointF glitch_object::scenePos(void) const
{
  if(m_proxy)
    return m_proxy->scenePos();
  else
    return {0.0, 0.0};
}

QPointer<glitch_object_view> glitch_object::editView(void) const
{
  return m_editView;
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

QStringList glitch_object::inputs(void) const
{
  /*
  ** Must be rapidly unique!
  */

  auto scene = this->scene();

  if(!scene)
    return QStringList();

  /*
  ** Discover objects on the right ends of wires.
  */

  QList<glitch_object *> objects;
  QSetIterator<glitch_wire *> it(scene->wires());

  while(it.hasNext())
    {
      auto wire = it.next();

      if(!wire || !wire->rightProxy())
	continue;

      if(this == wire->rightProxy()->object() &&
	 wire->leftProxy() &&
	 wire->leftProxy()->widget())
	{
	  auto object = qobject_cast<glitch_object *>
	    (wire->leftProxy()->widget());

	  if(object && object->scene())
	    objects << object;
	}
    }

  /*
  ** Sort the discovered objects according to their scene orders.
  */

  std::sort(objects.begin(), objects.end(), order_less_than);

  QStringList inputs;

  for(auto object : objects)
    if(object)
      {
	auto code(object->code());

	if(code.endsWith(';'))
	  code = code.mid(0, code.length() - 1);

	inputs << code;
      }

  return inputs;
}

QStringList glitch_object::outputs(void) const
{
  /*
  ** Must be rapidly unique!
  */

  auto scene = this->scene();

  if(!scene)
    return QStringList();

  /*
  ** Discover objects on the left ends of wires.
  */

  QList<glitch_object *> objects;
  QSetIterator<glitch_wire *> it(scene->wires());

  while(it.hasNext())
    {
      auto wire = it.next();

      if(!wire || !wire->leftProxy())
	continue;

      if(this == wire->leftProxy()->object() &&
	 wire->rightProxy() &&
	 wire->rightProxy()->widget())
	{
	  auto object = qobject_cast<glitch_object *>
	    (wire->rightProxy()->widget());

	  if(object && object->scene())
	    objects << object;
	}
    }

  /*
  ** Sort the discovered objects according to their scene orders.
  */

  std::sort(objects.begin(), objects.end(), order_less_than);

  QStringList outputs;

  for(auto object : objects)
    if(object)
      {
	auto code(object->code());

	if(code.endsWith(';'))
	  code = code.mid(0, code.length() - 1);

	outputs << code;
      }

  return outputs;
}

bool glitch_object::canResize(void) const
{
  return false;
}

bool glitch_object::hasInput(void) const
{
  return false;
}

bool glitch_object::hasOutput(void) const
{
  return false;
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
  for(auto scrollBar : findChildren<QScrollBar *> ())
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

  if(type == "arduino-advancedio")
    object = glitch_object_advanced_io_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-analogio")
    object = glitch_object_analog_io_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-arithmeticoperator")
    object = glitch_object_arithmetic_operator_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-bitsandbytes")
    object = glitch_object_bits_and_bytes_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-bitwiseoperator")
    object = glitch_object_bitwise_operator_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-blockcomment")
    object = glitch_object_block_comment_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-constant")
    object = glitch_object_constant_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-digitalio")
    object = glitch_object_digital_io_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-flow-control")
    object = glitch_object_flow_control_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-function")
    object = glitch_object_function_arduino::createFromValues
      (values, parentObject, error, parent);
  else if(type == "arduino-logicaloperator")
    object = glitch_object_logical_operator_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-loop" || type == "arduino-setup")
    {
      auto view = qobject_cast<glitch_view_arduino *> (parent);

      if(view)
	{
	  if(type == "arduino-loop")
	    object = view->loopObject();
	  else
	    object = view->setupObject();
	}
    }
  else if(type == "arduino-serial")
    object = glitch_object_serial_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-time")
    object = glitch_object_time_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-variable")
    object = glitch_object_variable_arduino::createFromValues
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

glitch_scene *glitch_object::editScene(void) const
{
  if(m_editView)
    return m_editView->scene();
  else
    return nullptr;
}

glitch_scene *glitch_object::scene(void) const
{
  if(m_proxy)
    return qobject_cast<glitch_scene *> (m_proxy->scene());
  else
    return nullptr;
}

glitch_view *glitch_object::findNearestGlitchView(QWidget *widget) const
{
  if(!widget)
    return nullptr;
  else if(qobject_cast<glitch_view *> (widget))
    return qobject_cast<glitch_view *> (widget);

  auto parent = widget->parentWidget();

  do
    {
      if(qobject_cast<glitch_view *> (parent))
	return qobject_cast<glitch_view *> (parent);
      else if(parent)
	parent = parent->parentWidget();
      else
	break;
    }
  while(true);

  return nullptr;
}

qint64 glitch_object::id(void) const
{
  return m_id;
}

void glitch_object::addChild(const QPointF &point, glitch_object *object)
{
  if(!object)
    return;

  if(m_editView)
    m_editView->artificialDrop(point, object);
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

void glitch_object::createActions(void)
{
  if(!m_actions.contains(DefaultMenuActions::ADJUST_SIZE))
    {
      auto action = new QAction(tr("&Adjust Size"), this);

      action->setData(DefaultMenuActions::ADJUST_SIZE);
      action->setEnabled(!isMandatory());
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object::slotAdjustSize);
      m_actions[DefaultMenuActions::ADJUST_SIZE] = action;
    }

  if(!m_actions.contains(DefaultMenuActions::DELETE))
    {
      auto action = new QAction(tr("&Delete"), this);

      action->setData(DefaultMenuActions::DELETE);
      action->setEnabled(!isMandatory());
      action->setIcon(QIcon::fromTheme("edit-delete"));
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

void glitch_object::hideOrShowOccupied(void)
{
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
  for(auto toolButton : findChildren<QToolButton *> ())
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

void glitch_object::prepareEditSignals(const glitch_view *parentView)
{
  if(m_editView && m_editWindow)
    {
      connect(m_editView,
	      SIGNAL(paste(void)),
	      m_editView,
	      SLOT(slotPaste(void)),
	      Qt::UniqueConnection);
      connect(m_editWindow,
	      SIGNAL(closed(void)),
	      m_editView,
	      SLOT(slotParentWindowClosed(void)),
	      Qt::UniqueConnection);
      connect(m_editWindow,
	      &glitch_object_edit_window::closed,
	      m_editView,
	      &glitch_object_view::closed,
	      Qt::UniqueConnection);
      connect(m_editWindow,
	      SIGNAL(copy(void)),
	      m_editView,
	      SIGNAL(copy(void)),
	      Qt::UniqueConnection);
      connect(m_editWindow,
	      SIGNAL(deleteSignal(void)),
	      m_editView,
	      SLOT(slotDelete(void)),
	      Qt::UniqueConnection);
      connect(m_editWindow,
	      SIGNAL(paste(void)),
	      m_editView,
	      SIGNAL(paste(void)),
	      Qt::UniqueConnection);
      connect(m_editWindow,
	      SIGNAL(redo(void)),
	      m_editView,
	      SLOT(slotRedo(void)),
	      Qt::UniqueConnection);
      connect(m_editWindow,
	      SIGNAL(selectAll(void)),
	      m_editView,
	      SLOT(slotSelectAll(void)),
	      Qt::UniqueConnection);
      connect(m_editWindow,
	      SIGNAL(undo(void)),
	      m_editView,
	      SLOT(slotUndo(void)),
	      Qt::UniqueConnection);
    }
  else
    {
      if(!m_editView)
	qDebug() << tr("m_editView is zero! Signals cannot be established!");

      if(!m_editWindow)
	qDebug() << tr("m_editWindow is zero! Signals cannot be established!");
    }

  if(m_editView && parentView)
    {
      connect(m_editView,
	      SIGNAL(copy(void)),
	      parentView,
	      SLOT(slotCopy(void)),
	      Qt::UniqueConnection);
      connect
	(parentView,
	 QOverload<const glitch_tools::Operations>::of
	 (&glitch_view::toolsOperationChanged),
	 m_editView->scene(),
	 QOverload<const glitch_tools::Operations>::of
	 (&glitch_scene::slotToolsOperationChanged));
    }
  else
    {
      if(!m_editView)
	qDebug() << tr("m_editView is zero! Signals cannot be established!");
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

  if(error.isEmpty())
    saveWires(db, error);
}

void glitch_object::saveProperties(const QMap<QString, QVariant> &p,
				   const QSqlDatabase &db,
				   QString &error)
{
  auto properties(p);

  properties["position_locked"] = m_properties.value
    (Properties::POSITION_LOCKED).toBool();
  properties["size"] = QString("(%1, %2)").
    arg(size().width()).arg(size().height());

  QMapIterator<QString, QVariant> it(properties);
  QSqlQuery query(db);
  QString string("");

  while(it.hasNext())
    {
      it.next();
      string += it.key();
      string += " = ";
      string += "\"";
      string += it.value().toString();
      string += "\"";

      if(it.hasNext())
	string += "&";
    }

  query.prepare("UPDATE objects SET properties = ? WHERE myoid = ?");
  query.addBindValue(string);
  query.addBindValue(m_id);
  query.exec();

  if(query.lastError().isValid())
    error = query.lastError().text();
}

void glitch_object::saveWires(const QSqlDatabase &db, QString &error)
{
  QHashIterator<qint64, QPointer<glitch_wire> > it(m_wires);
  QSqlQuery query(db);

  while(it.hasNext())
    {
      it.next();

      if(!it.value() || !it.value()->scene())
	continue;

      query.prepare
	("INSERT OR REPLACE INTO wires (object_input_oid, object_output_oid) "
	 "VALUES (?, ?)");
      query.addBindValue(it.key());
      query.addBindValue(m_id);
      query.exec();

      if(error.isEmpty() && query.lastError().isValid())
	error = query.lastError().text();
    }
}

void glitch_object::setCanvasSettings(glitch_canvas_settings *canvasSettings)
{
  m_canvasSettings = canvasSettings;

  auto scene = editScene();

  if(scene)
    scene->setCanvasSettings(m_canvasSettings);
}

void glitch_object::setName(const QString &n)
{
  auto name(n.trimmed().mid(0, static_cast<int> (Limits::NAME_MAXIMUM_LENGTH)));

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

      if(string.simplified().startsWith("comment = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::COMMENT] = string.trimmed();
	}
      else if(string.simplified().startsWith("name = "))
	{
	  if(!isMandatory())
	    {
	      string = string.mid(string.indexOf('=') + 1);
	      string.remove("\"");
	      string = string.mid
		(0, static_cast<int> (Limits::NAME_MAXIMUM_LENGTH));
	      m_properties[Properties::NAME] = string.trimmed();
	    }
	}
      else if(string.simplified().startsWith("position_locked = "))
	{
	  if(!isMandatory())
	    {
	      string = string.mid(string.indexOf('=') + 1);
	      string.remove("\"");
	      m_properties[Properties::POSITION_LOCKED] =
		QVariant(string.trimmed()).toBool();
	    }
	}
      else if(string.simplified().startsWith("size = "))
	{
	  if(!isMandatory())
	    {
	      string = string.mid(string.indexOf('=') + 1);
	      string.remove('"').remove('(').remove(')');

	      QSize size(this->size());

	      size.setWidth
		(qBound(100, string.split(',').value(0).toInt(), 500));
	      resize(size);
	    }
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
    case Properties::SIZE:
      {
	resize(value.toSize());
	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_object::setProperty(const char *name, const QVariant &value)
{
  QObject::setProperty(name, value);
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

void glitch_object::setWiredObject(glitch_object *object, glitch_wire *wire)
{
  if(!object || !wire || m_id == object->id() || m_wires.contains(object->id()))
    return;

  connect(wire,
	  &glitch_wire::destroyed,
	  this,
	  &glitch_object::slotWireDestroyed);
  m_wires[object->id()] = wire;
}

void glitch_object::simulateDelete(void)
{
  if(m_editWindow)
    m_editWindow->close();

  m_contextMenu->close();
}

void glitch_object::slotAdjustSize(void)
{
  auto before(size());

  resize(sizeHint().width(), height());

  if(before == this->size())
    return;

  if(m_undoStack)
    {
      auto undoCommand = new glitch_undo_command
	(size(),
	 before,
	 glitch_undo_command::PROPERTY_CHANGED,
	 glitch_object::Properties::SIZE,
	 this);

      undoCommand->setText
	(tr("object size changed (%1, %2)").
	 arg(scenePos().x()).arg(scenePos().y()));
      m_undoStack->push(undoCommand);
    }

  emit changed();
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
  m_contextMenu->setIdentifier(m_id);
  m_contextMenu->setName(name());
  m_contextMenu->show();
}

void glitch_object::slotUndoStackCreated(void)
{
  if(m_editView)
    emit undoStackCreated(m_editView->undoStack());
}

void glitch_object::slotWireDestroyed(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QMutableHashIterator<qint64, QPointer<glitch_wire> > it(m_wires);

  while(it.hasNext())
    {
      it.next();

      if(!it.value())
	it.remove();
    }

  QApplication::restoreOverrideCursor();
}
