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

#include <QColorDialog>
#include <QScrollBar>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QtDebug>

#include "Arduino/glitch-object-advanced-io-arduino.h"
#include "Arduino/glitch-object-analog-io-arduino.h"
#include "Arduino/glitch-object-arithmetic-operator-arduino.h"
#include "Arduino/glitch-object-bits-and-bytes-arduino.h"
#include "Arduino/glitch-object-bitwise-operator-arduino.h"
#include "Arduino/glitch-object-block-comment-arduino.h"
#include "Arduino/glitch-object-boolean-operator-arduino.h"
#include "Arduino/glitch-object-characters-arduino.h"
#include "Arduino/glitch-object-compound-operator-arduino.h"
#include "Arduino/glitch-object-constant-arduino.h"
#include "Arduino/glitch-object-conversion-arduino.h"
#include "Arduino/glitch-object-digital-io-arduino.h"
#include "Arduino/glitch-object-flow-control-arduino.h"
#include "Arduino/glitch-object-function-arduino.h"
#include "Arduino/glitch-object-interrupts-arduino.h"
#include "Arduino/glitch-object-loop-arduino.h"
#include "Arduino/glitch-object-mathematics-arduino.h"
#include "Arduino/glitch-object-random-arduino.h"
#include "Arduino/glitch-object-serial-arduino.h"
#include "Arduino/glitch-object-setup-arduino.h"
#include "Arduino/glitch-object-stream-arduino.h"
#include "Arduino/glitch-object-syntax-arduino.h"
#include "Arduino/glitch-object-time-arduino.h"
#include "Arduino/glitch-object-trigonometry-arduino.h"
#include "Arduino/glitch-object-utilities-arduino.h"
#include "Arduino/glitch-object-variable-arduino.h"
#include "Arduino/glitch-view-arduino.h"
#include "glitch-floating-context-menu.h"
#include "glitch-font-filter.h"
#include "glitch-object-arrow.h"
#include "glitch-object-edit-window.h"
#include "glitch-object-view.h"
#include "glitch-object.h"
#include "glitch-port-colors.h"
#include "glitch-scene.h"
#include "glitch-style-sheet.h"
#include "glitch-ui.h"
#include "glitch-undo-command.h"
#include "glitch-view.h"
#include "glitch-wire.h"

QRegularExpression glitch_object::s_splitRegularExpression =
  QRegularExpression("&(?=([^\"]*\"[^\"]*\")*[^\"]*$)");
static qreal s_windowOpacity = 0.85;

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
  installEventFilter(new glitch_font_filter(this));
  m_drawInputConnector = false;
  m_drawOutputConnector = false;
  m_occupied = false;
  m_parent = parent;
  m_properties[Properties::BACKGROUND_COLOR] = QColor(230, 230, 250);
  m_properties[Properties::BORDER_COLOR] = QColor(168, 169, 173);
  m_properties[Properties::COMPRESSED_WIDGET] = false;
  m_properties[Properties::PORT_COLORS] =
    QColor(0, 80, 181).name() +    // Input Connected
    "-" +
    QColor(118, 134, 146).name() + // Input Disconnected
    "-" +
    QColor(0, 80, 181).name() +    // Output Connected
    "-" +
    QColor(118, 134, 146).name();  // Output Disconnected
  m_properties[Properties::POSITION_LOCKED] = false;
  m_properties[Properties::TOOL_BAR_VISIBLE] = true;
  m_properties[Properties::TRANSPARENT] = true;
  m_properties[Properties::Z_VALUE] = 0.0;

  {
    auto view = qobject_cast<glitch_object_view *> (parent);

    if(view)
      setUndoStack(view->scene()->undoStack());
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

  setFont(glitch_ui::s_defaultApplicationFont);
  setWindowOpacity(s_windowOpacity);
}

glitch_object::~glitch_object()
{
  foreach(auto object, m_copiedChildren)
    if(object)
      object->deleteLater();

  if(m_canvasSettings)
    disconnect(m_canvasSettings, nullptr, this, nullptr);

  if(m_contextMenu)
    m_contextMenu->deleteLater();

  if(m_editView)
    m_editView->deleteLater();

  if(m_editWindow)
    m_editWindow->deleteLater();
}

QList<glitch_object *> glitch_object::objects(void) const
{
  if(m_editView)
    return m_editView->objects();
  else
    return QList<glitch_object *> ();
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
	    if(!objects.contains(object))
	      objects << object;
	}
    }

  /*
  ** Sort the discovered objects according to their scene orders.
  */

  std::sort(objects.begin(), objects.end(), order_less_than);

  QStringList inputs;

  foreach(auto object, objects)
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
	    if(!objects.contains(object))
	      objects << object;
	}
    }

  /*
  ** Sort the discovered objects according to their scene orders.
  */

  std::sort(objects.begin(), objects.end(), order_less_than);

  QStringList outputs;

  foreach(auto object, objects)
    if(object)
      {
	auto code(object->code());

	if(code.endsWith(';'))
	  code = code.mid(0, code.length() - 1);

	outputs << code;
      }

  return outputs;
}

QStringList glitch_object::parameters(void) const
{
  return QStringList();
}

QToolButton *glitch_object::contextMenuButton(void) const
{
  foreach(auto toolButton, findChildren<QToolButton *> ())
    if(toolButton &&
       toolButton->objectName() == QString::fromUtf8("context_menu"))
      return toolButton;

  return nullptr;
}

bool glitch_object::canResize(void) const
{
  return false;
}

bool glitch_object::event(QEvent *event)
{
  if(event)
    switch(event->type())
      {
      case QEvent::ApplicationFontChange:
	{
	  QTimer::singleShot(25, this, &glitch_object::slotSetFont);
	  return true;
	}
      default:
	{
	  break;
	}
      }

  return QWidget::event(event);
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
  foreach(auto scrollBar, findChildren<QScrollBar *> ())
    if(scrollBar && scrollBar->isSliderDown())
      return true;
    else if(scrollBar &&
	    scrollBar->rect().
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
  else if(type == "arduino-booleanoperator")
    object = glitch_object_boolean_operator_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-characters")
    object = glitch_object_characters_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-compoundoperator")
    object = glitch_object_compound_operator_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-constant")
    object = glitch_object_constant_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-conversion")
    object = glitch_object_conversion_arduino::createFromValues
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
  else if(type == "arduino-interrupts")
    object = glitch_object_interrupts_arduino::createFromValues
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
  else if(type == "arduino-mathematics")
    object = glitch_object_mathematics_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-random")
    object = glitch_object_random_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-serial")
    object = glitch_object_serial_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-stream")
    object = glitch_object_stream_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-syntax")
    object = glitch_object_syntax_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-time")
    object = glitch_object_time_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-trigonometry")
    object = glitch_object_trigonometry_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-utilities")
    object = glitch_object_utilities_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-variable")
    object = glitch_object_variable_arduino::createFromValues
      (values, error, parent);
  else if(type == "decoration-arrow")
    object = glitch_object_arrow::createFromValues(values, error, parent);
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
  else if(m_editView != widget && qobject_cast<glitch_view *> (widget))
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

int glitch_object::minimumHeight(int height)
{
  if(height % 10)
    return (10 - height % 10) + height;
  else
    return height;
}

qint64 glitch_object::id(void) const
{
  return m_id;
}

void glitch_object::addChild(const QPointF &point, glitch_object *object)
{
  if(!object)
    return;

  if(object->editView())
    connect(this,
	    &glitch_object::simulateDeleteSignal,
	    object,
	    &glitch_object::slotSimulateDelete,
	    Qt::UniqueConnection);

  if(m_editView)
    m_editView->artificialDrop(point, object);
}

void glitch_object::addDefaultActions(QMenu &menu)
{
  createActions();

  QMapIterator<DefaultMenuActions, QAction *> it(m_actions);

  while(it.hasNext())
    {
      it.next();
      menu.addAction(it.value());

      if((DefaultMenuActions::COPY == it.key() ||
	  DefaultMenuActions::DELETE == it.key() ||
	  DefaultMenuActions::LOCK_POSITION == it.key() ||
	  DefaultMenuActions::SET_FUNCTION_NAME == it.key() ||
	  DefaultMenuActions::TRANSPARENT == it.key()) &&
	 it.hasNext())
	menu.addSeparator();
    }
}

void glitch_object::afterPaste(void)
{
  compressWidget(m_properties.value(Properties::COMPRESSED_WIDGET).toBool());
  setWindowOpacity
    (m_properties.value(Properties::TRANSPARENT).toBool() ?
     s_windowOpacity : 1.0);
}

void glitch_object::cloneWires
(const QHash<qint64, QPointer<glitch_wire> > &wires)
{
  QHashIterator<qint64, QPointer<glitch_wire> > it(wires);

  while(it.hasNext())
    {
      it.next();

      if(!it.value() || !it.value()->scene())
	/*
	** Ignore wires which do not reside in scenes.
	*/

	continue;

      QPair<QPointF, QPointF> pair;

      pair.first = it.value()->leftProxy()->pos();
      pair.second = it.value()->rightProxy()->pos();

      if(!m_copiedConnectionsPositions.contains(pair))
	m_copiedConnectionsPositions << pair;
    }
}

void glitch_object::cloneWires(const QList<QPair<QPointF, QPointF> > &list)
{
  m_copiedConnectionsPositions = list;
}

void glitch_object::compressWidget(const bool state)
{
  auto toolButton = contextMenuButton();

  if(toolButton)
    toolButton->setVisible(!state);
}

void glitch_object::createActions(void)
{
  if(!m_actions.contains(DefaultMenuActions::ADJUST_SIZE))
    {
      auto action = new QAction(tr("&Adjust Size"), this);

      action->setData(static_cast<int> (DefaultMenuActions::ADJUST_SIZE));
      action->setEnabled(!isMandatory());
      action->setToolTip(tr("Resize the widget to its preferred size."));
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object::slotAdjustSize);
      m_actions[DefaultMenuActions::ADJUST_SIZE] = action;
    }

  if(!m_actions.contains(DefaultMenuActions::BACKGROUND_COLOR))
    {
      auto action = new QAction(tr("Background Color..."), this);

      action->setData(static_cast<int> (DefaultMenuActions::BACKGROUND_COLOR));
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object::slotSelectColor);
      m_actions[DefaultMenuActions::BACKGROUND_COLOR] = action;

      if(!isNativelyDrawn())
	action->setEnabled(false);
    }

  if(!m_actions.contains(DefaultMenuActions::BORDER_COLOR))
    {
      auto action = new QAction(tr("Border Color..."), this);

      action->setData(static_cast<int> (DefaultMenuActions::BORDER_COLOR));
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object::slotSelectBorderColor);
      m_actions[DefaultMenuActions::BORDER_COLOR] = action;

      if(m_type == "decoration-arrow")
	action->setEnabled(false);
    }

  if(!m_actions.contains(DefaultMenuActions::COMPRESS_WIDGET))
    {
      auto action = new QAction(tr("&Compress Widget"), this);

      action->setCheckable(true);
      action->setChecked
	(m_properties.value(Properties::COMPRESSED_WIDGET).toBool());
      action->setData(static_cast<int> (DefaultMenuActions::COMPRESS_WIDGET));
      action->setEnabled(!isMandatory());
      action->setToolTip(tr("Hide most of the widget's children."));
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object::slotActionTriggered);
      m_actions[DefaultMenuActions::COMPRESS_WIDGET] = action;
    }
  else
    m_actions[DefaultMenuActions::COMPRESS_WIDGET]->
      setChecked(m_properties.value(Properties::COMPRESSED_WIDGET).toBool());

  if(!m_actions.contains(DefaultMenuActions::CONTEXT_MENU))
    {
      auto action = new QAction(tr("Floating Context Menu..."), this);

      action->setData(static_cast<int> (DefaultMenuActions::CONTEXT_MENU));
      action->setIcon(QIcon(":/context-menu.png"));
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object::slotShowContextMenu);
      m_actions[DefaultMenuActions::CONTEXT_MENU] = action;
    }

  if(!m_actions.contains(DefaultMenuActions::COPY))
    {
      QAction *action = nullptr;

      if(m_type == "arduino-loop" || m_type == "arduino-setup")
	action = new QAction(tr("Copy Internal Object(s)"), this);
      else
	action = new QAction(tr("Copy Object"), this);

      action->setData(static_cast<int> (DefaultMenuActions::COPY));
      action->setIcon(QIcon::fromTheme("edit-copy"));
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object::slotCopy);
      m_actions[DefaultMenuActions::COPY] = action;
    }

  if(!m_actions.contains(DefaultMenuActions::DELETE))
    {
      auto action = new QAction(tr("&Delete"), this);

      action->setData(static_cast<int> (DefaultMenuActions::DELETE));
      action->setEnabled(!isMandatory());
      action->setIcon(QIcon::fromTheme("edit-delete"));
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object::deletedViaContextMenu);
      m_actions[DefaultMenuActions::DELETE] = action;
    }

  if(!m_actions.contains(DefaultMenuActions::LOCK_POSITION))
    {
      auto action = new QAction(tr("&Lock Position"), this);

      action->setCheckable(true);
      action->setChecked
	(m_properties.value(Properties::POSITION_LOCKED).toBool());
      action->setData(static_cast<int> (DefaultMenuActions::LOCK_POSITION));
      action->setEnabled(!isMandatory());
      action->setToolTip(tr("Lock the widget's position."));
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object::slotLockPosition);
      m_actions[DefaultMenuActions::LOCK_POSITION] = action;
    }
  else
    m_actions[DefaultMenuActions::LOCK_POSITION]->
      setChecked(m_properties.value(Properties::POSITION_LOCKED).toBool());

  if(!m_actions.contains(DefaultMenuActions::PORT_COLORS))
    {
      auto action = new QAction(tr("Set &Port Colors..."), this);

      action->setData(static_cast<int> (DefaultMenuActions::PORT_COLORS));
      action->setEnabled(hasInput() || hasOutput());
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object::slotSetPortColors);
      m_actions[DefaultMenuActions::PORT_COLORS] = action;
    }

  if(!m_actions.contains(DefaultMenuActions::SET_STYLE_SHEET))
    {
      auto action = new QAction(tr("&Set Style Sheet..."), this);

      action->setData(static_cast<int> (DefaultMenuActions::SET_STYLE_SHEET));
      action->setEnabled(!isNativelyDrawn());
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object::slotSetStyleSheet);
      m_actions[DefaultMenuActions::SET_STYLE_SHEET] = action;
    }

  if(!m_actions.contains(DefaultMenuActions::TRANSPARENT))
    {
      auto action = new QAction(tr("&Transparent"), this);

      action->setCheckable(true);
      action->setChecked(m_properties.value(Properties::TRANSPARENT).toBool());
      action->setData(static_cast<int> (DefaultMenuActions::TRANSPARENT));
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object::slotActionTriggered);
      m_actions[DefaultMenuActions::TRANSPARENT] = action;
    }
  else
    m_actions[DefaultMenuActions::TRANSPARENT]->setChecked
      (m_properties.value(Properties::TRANSPARENT).toBool());
}

void glitch_object::hideOrShowOccupied(void)
{
}

void glitch_object::mouseDoubleClickEvent(QMouseEvent *event)
{
  QWidget::mouseDoubleClickEvent(event);
  slotShowContextMenu();
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
  auto toolButton = contextMenuButton();

  if(toolButton)
    {
      connect(toolButton,
	      &QToolButton::clicked,
	      this,
	      &glitch_object::slotShowContextMenu,
	      Qt::UniqueConnection);
      toolButton->setToolTip(tr("Floating Context Menu"));
    }
}

void glitch_object::prepareEditObjects(const glitch_view *parentView)
{
  if(m_editView && m_editWindow)
    {
      connect(m_editView,
	      &glitch_object_view::paste,
	      m_editView,
	      &glitch_object_view::slotPaste,
	      Qt::UniqueConnection);
      connect(m_editView->scene(),
	      SIGNAL(dockPropertyEditor(QWidget *)),
	      m_editWindow,
	      SLOT(slotDockPropertyEditor(QWidget *)),
	      Qt::UniqueConnection);
      connect(m_editWindow,
	      &glitch_object_edit_window::closed,
	      m_editView,
	      &glitch_object_view::slotParentWindowClosed,
	      Qt::UniqueConnection);
      connect(m_editWindow,
	      &glitch_object_edit_window::closed,
	      m_editView,
	      &glitch_object_view::closed,
	      Qt::UniqueConnection);
      connect(m_editWindow,
	      &glitch_object_edit_window::copy,
	      m_editView,
	      &glitch_object_view::copy,
	      Qt::UniqueConnection);
      connect(m_editWindow,
	      &glitch_object_edit_window::deleteSignal,
	      m_editView,
	      &glitch_object_view::slotDelete,
	      Qt::UniqueConnection);
      connect(m_editWindow,
	      &glitch_object_edit_window::paste,
	      m_editView,
	      &glitch_object_view::paste,
	      Qt::UniqueConnection);
      connect(m_editWindow,
	      &glitch_object_edit_window::redo,
	      m_editView,
	      &glitch_object_view::slotRedo,
	      Qt::UniqueConnection);
      connect(m_editWindow,
	      &glitch_object_edit_window::saveSignal,
	      m_editView,
	      &glitch_object_view::saveSignal,
	      Qt::UniqueConnection);
      connect(m_editWindow,
	      &glitch_object_edit_window::selectAll,
	      m_editView,
	      &glitch_object_view::slotSelectAll,
	      Qt::UniqueConnection);
      connect(m_editWindow,
	      &glitch_object_edit_window::undo,
	      m_editView,
	      &glitch_object_view::slotUndo,
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
	      &glitch_object_view::copy,
	      parentView,
	      &glitch_view::slotCopy,
	      Qt::UniqueConnection);
      connect(m_editView,
	      &glitch_object_view::saveSignal,
	      parentView,
	      &glitch_view::slotSave,
	      Qt::UniqueConnection);
      connect(parentView,
	      SIGNAL(toolsOperationChanged(const glitch_tools::Operations)),
	      m_editView->scene(),
	      SLOT(slotToolsOperationChanged(const glitch_tools::Operations)),
	      Qt::UniqueConnection);
    }
  else
    {
      if(!m_editView)
	qDebug() << tr("m_editView is zero! Signals cannot be established!");

      if(!parentView)
	qDebug() << tr("parentView is zero! Signals cannot be established!");
    }

  if(m_editWindow)
    {
      connect(m_editWindow,
	      SIGNAL(propertyChanged(const QString &, const QVariant &)),
	      this,
	      SLOT(slotPropertyChanged(const QString &, const QVariant &)),
	      Qt::UniqueConnection);

      if(parentView)
	connect(parentView,
		&glitch_view::preferencesAccepted,
		m_editWindow,
		&glitch_object_edit_window::slotPreferencesAccepted,
		Qt::UniqueConnection);

      m_editWindow->restoreGeometry
	(m_properties.value(Properties::EDIT_WINDOW_GEOMETRY).toByteArray());
      m_editWindow->restoreState
	(m_properties.value(Properties::EDIT_WINDOW_STATE).toByteArray());
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

  properties["background_color"] = m_properties.value
    (Properties::BACKGROUND_COLOR).toString();
  properties["border_color"] = m_properties.value
    (Properties::BORDER_COLOR).toString();
  properties["compressed_widget"] = m_properties.value
    (Properties::COMPRESSED_WIDGET).toBool();
  properties["edit_window_geometry"] = m_properties.value
    (Properties::EDIT_WINDOW_GEOMETRY).toByteArray().toBase64();
  properties["edit_window_state"] = m_editWindow ?
    m_editWindow->saveState().toBase64() : QByteArray().toBase64();
  properties["port_colors"] = m_properties.value
    (Properties::PORT_COLORS).toString();
  properties["position_locked"] = m_properties.value
    (Properties::POSITION_LOCKED).toBool();
  properties["size"] = QString("(%1, %2)").
    arg(size().width()).arg(size().height());
  properties["structures_view_splitter_state"] = m_properties.value
    (Properties::STRUCTURES_VIEW_SPLITTER_STATE).toByteArray().toBase64();
  properties["tool_bar_visible"] = m_properties.value
    (Properties::TOOL_BAR_VISIBLE).toBool();
  properties["transparent"] = m_properties.value
    (Properties::TRANSPARENT).toBool();
  properties["z_value"] = m_properties.value(Properties::Z_VALUE).toReal();

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

void glitch_object::separate(void)
{
}

void glitch_object::setCanvasSettings(glitch_canvas_settings *canvasSettings)
{
  if(m_canvasSettings)
    disconnect(m_canvasSettings,
	       SIGNAL(accepted(const bool)),
	       this,
	       SLOT(slotCanvasSettingsChanged(const bool)));

  m_canvasSettings = canvasSettings;

  if(m_canvasSettings)
    connect(m_canvasSettings,
	    SIGNAL(accepted(const bool)),
	    this,
	    SLOT(slotCanvasSettingsChanged(const bool)));

  auto scene = editScene();

  if(scene)
    scene->setCanvasSettings(m_canvasSettings);

  slotCanvasSettingsChanged(true);
}

void glitch_object::setName(const QString &n)
{
  auto name(n.trimmed().mid(0, static_cast<int> (Limits::NAME_MAXIMUM_LENGTH)));

  if(!name.isEmpty())
    {
      if(m_contextMenu)
	m_contextMenu->setName(name);

      m_properties[Properties::NAME] = name;
    }
}

void glitch_object::setProperties(const QStringList &list)
{
  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("background_color = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::BACKGROUND_COLOR] = QColor(string.trimmed());
	}
      else if(string.simplified().startsWith("border_color = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::BORDER_COLOR] = QColor(string.trimmed());
	}
      else if(string.simplified().startsWith("compressed_widget = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::COMPRESSED_WIDGET] =
	    QVariant(string.trimmed()).toBool();
	  compressWidget
	    (m_properties.value(Properties::COMPRESSED_WIDGET).toBool());
	}
      else if(string.simplified().startsWith("comment = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::COMMENT] = string.trimmed();
	}
      else if(string.simplified().startsWith("edit_window_geometry = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::EDIT_WINDOW_GEOMETRY] =
	    QByteArray::fromBase64(string.trimmed().toLatin1());
	}
      else if(string.simplified().startsWith("edit_window_state = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::EDIT_WINDOW_STATE] =
	    QByteArray::fromBase64(string.trimmed().toLatin1());
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
      else if(string.simplified().startsWith("port_colors = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::PORT_COLORS] = string.trimmed();
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
	      string = string.mid(string.indexOf('=') + 1).trimmed();
	      string.remove('"').remove('(').remove(')');

	      QSize size;
	      auto list(string.split(','));

	      size.setHeight
		(minimumHeight(qBound(25,
				      list.value(1).trimmed().toInt(),
				      500)));
	      size.setWidth(qBound(50, list.value(0).trimmed().toInt(), 500));
	      resize(size);
	    }
	}
      else if(string.simplified().startsWith("structures_view_splitter_state"))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::STRUCTURES_VIEW_SPLITTER_STATE] =
	    QByteArray::fromBase64(string.trimmed().toLatin1());
	}
      else if(string.simplified().startsWith("tool_bar_visible = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::TOOL_BAR_VISIBLE] =
	    QVariant(string.trimmed()).toBool();
	}
      else if(string.simplified().startsWith("transparent = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::TRANSPARENT] =
	    QVariant(string.trimmed()).toBool();
	  setWindowOpacity
	    (m_properties.value(Properties::TRANSPARENT).toBool() ?
	     s_windowOpacity : 1.0);
	}
      else if(string.simplified().startsWith("z_value = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");

	  if(m_contextMenu)
	    m_contextMenu->setProperty
	      (Properties::Z_VALUE, QVariant(string.trimmed()).toReal());

	  m_properties[Properties::Z_VALUE] =
	    QVariant(string.trimmed()).toReal();
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
    case Properties::COMPRESSED_WIDGET:
      {
	if(m_actions.contains(DefaultMenuActions::COMPRESS_WIDGET))
	  m_actions.value
	    (DefaultMenuActions::COMPRESS_WIDGET)->setChecked(value.toBool());

	compressWidget(value.toBool());
	break;
      }
    case Properties::GEOMETRY:
      {
	if(m_proxy)
	  m_proxy->setGeometry(value.toRectF());

	break;
      }
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
    case Properties::STRUCTURES_VIEW_SPLITTER_STATE:
      {
	emit changed();
	break;
      }
    case Properties::TOOL_BAR_VISIBLE:
      {
	if(m_editWindow)
	  m_editWindow->setToolBarVisible(value.toBool());

	break;
      }
    case Properties::TRANSPARENT:
      {
	if(m_actions.contains(DefaultMenuActions::TRANSPARENT))
	  m_actions.value(DefaultMenuActions::TRANSPARENT)->setChecked
	    (value.toBool());

	setWindowOpacity(value.toBool() ? s_windowOpacity : 1.0);
	break;
      }
    case Properties::Z_VALUE:
      {
	if(m_contextMenu)
	  m_contextMenu->setProperty(property, value);

	if(m_proxy)
	  m_proxy->setZValue(value.toReal());

	break;
      }
    default:
      {
	break;
      }
    }

  if(m_proxy && m_proxy->scene())
    m_proxy->scene()->update();
}

void glitch_object::setProperty(const char *name, const QVariant &value)
{
  QObject::setProperty(name, value);
}

void glitch_object::setProxy(const QPointer<glitch_proxy_widget> &proxy)
{
  m_proxy = proxy;

  if(m_proxy)
    {
      if(m_contextMenu)
	connect(m_proxy,
		&glitch_proxy_widget::changed,
		m_contextMenu,
		&glitch_floating_context_menu::slotObjectChanged,
		Qt::UniqueConnection);

      m_proxy->setFlag
	(QGraphicsItem::ItemIsMovable,
	 !m_properties.value(Properties::POSITION_LOCKED).toBool());
      m_proxy->setZValue(m_properties.value(Properties::Z_VALUE).toReal());
    }
}

void glitch_object::setUndoStack(QUndoStack *undoStack)
{
  if(m_editView)
    m_editView->setUndoStack(undoStack);

  if(m_editWindow)
    m_editWindow->setUndoStack(undoStack);

  m_undoStack = undoStack;

  if(m_undoStack)
    connect(m_undoStack,
	    &QUndoStack::indexChanged,
	    this,
	    &glitch_object::slotHideOrShowOccupied,
	    Qt::UniqueConnection);
}

void glitch_object::setWiredObject(glitch_object *object, glitch_wire *wire)
{
  if(!object || !wire || m_id == object->id())
    return;

  connect(wire,
	  &glitch_wire::destroyed,
	  this,
	  &glitch_object::slotWireDestroyed,
	  Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection));
  m_wires[object->id()] = wire; // Replace the wire object if necessary.
}

void glitch_object::showEditWindow(void) const
{
  if(m_editWindow)
    {
      m_editWindow->restoreGeometry
	(m_properties.value(Properties::EDIT_WINDOW_GEOMETRY).toByteArray());
      m_editWindow->restoreState
	(m_properties.value(Properties::EDIT_WINDOW_STATE).toByteArray());
      m_editWindow->show();
      m_editWindow->activateWindow();
      m_editWindow->raise();
    }
}

void glitch_object::simulateDelete(void)
{
  if(m_contextMenu)
    m_contextMenu->close();

  if(m_editWindow)
    m_editWindow->close();

  emit simulateDeleteSignal();
}

void glitch_object::slotActionTriggered(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  if(action->isCheckable())
    {
      auto property = Properties::Z_Z_Z_PROPERTY;

      switch(DefaultMenuActions(action->data().toInt()))
	{
	case DefaultMenuActions::COMPRESS_WIDGET:
	  {
	    property = Properties::COMPRESSED_WIDGET;
	    break;
	  }
	case DefaultMenuActions::TRANSPARENT:
	  {
	    property = Properties::TRANSPARENT;
	    break;
	  }
	default:
	  {
	    return;
	  }
	}

      if(m_undoStack)
	{
	  auto undoCommand = new glitch_undo_command
	    (!m_properties.value(property).toBool(),
	     m_properties.value(property),
	     glitch_undo_command::PROPERTY_CHANGED,
	     property,
	     this);

	  undoCommand->setText
	    (tr("item property changed (%1, %2)").
	     arg(scenePos().x()).arg(scenePos().y()));
	  m_undoStack->push(undoCommand);
	}
      else
	m_properties[property] = !m_properties.value(property).toBool();
    }

  emit changed();
}

void glitch_object::slotAdjustSize(void)
{
  auto before(size());

  resize(sizeHint().width(), minimumHeight(sizeHint().height()));

  if(before == this->size())
    return;

  if(m_undoStack)
    {
      auto undoCommand = new glitch_undo_command
	(size(),
	 before,
	 glitch_undo_command::PROPERTY_CHANGED,
	 Properties::SIZE,
	 this);

      undoCommand->setText
	(tr("object size changed (%1, %2)").
	 arg(scenePos().x()).arg(scenePos().y()));
      m_undoStack->push(undoCommand);
    }

  emit changed();
}

void glitch_object::slotCanvasSettingsChanged(const bool state)
{
  Q_UNUSED(state);

  if(!m_canvasSettings || !m_editWindow)
    return;

  m_editWindow->setCategoriesIconSize(m_canvasSettings->categoriesIconSize());
  m_editWindow->setWindowTitle
    (tr("Glitch: %1 (%2)").arg(name()).arg(m_canvasSettings->name()));
}

void glitch_object::slotClearTemporaryContainers(void)
{
  m_copiedConnectionsPositions.clear();
  m_originalPosition = QPointF();
}

void glitch_object::slotCopy(void)
{
  glitch_ui::copy(this);
}

void glitch_object::slotHideOrShowOccupied(void)
{
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

void glitch_object::slotPropertyChanged
(const Properties property, const QVariant &value)
{
  if(property != Properties::Z_Z_Z_PROPERTY)
    {
      auto before = m_properties.value(property);

      m_properties[property] = value;

      if(m_undoStack)
	{
	  auto undoCommand = new glitch_undo_command
	    (value,
	     before,
	     glitch_undo_command::PROPERTY_CHANGED,
	     property,
	     this);

	  undoCommand->setText
	    (tr("object property changed (%1, %2)").
	     arg(scenePos().x()).arg(scenePos().y()));
	  m_undoStack->push(undoCommand);
	}
    }
}

void glitch_object::slotPropertyChanged
(const QString &property, const QVariant &value)
{
  auto p = Properties::Z_Z_Z_PROPERTY;

  if(property == "background_color")
    p = Properties::BACKGROUND_COLOR;
  else if(property == "border_color")
    p = Properties::BORDER_COLOR;
  else if(property == "compressed_widget")
    p = Properties::COMPRESSED_WIDGET;
  else if(property == "tool_bar_visible")
    p = Properties::TOOL_BAR_VISIBLE;

  slotPropertyChanged(p, value);
}

void glitch_object::slotSelectColor(void)
{
  QColorDialog dialog(m_parent);

  dialog.setCurrentColor
    (m_properties.value(Properties::BACKGROUND_COLOR).value<QColor> ());
  dialog.setOption(QColorDialog::ShowAlphaChannel, true);
  dialog.setWindowIcon(windowIcon());
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();

      auto color(dialog.selectedColor());

      if(m_undoStack)
	{
	  auto undoCommand = new glitch_undo_command
	    (color.name(QColor::HexArgb),
	     m_properties.value(Properties::BACKGROUND_COLOR),
	     glitch_undo_command::PROPERTY_CHANGED,
	     Properties::BACKGROUND_COLOR,
	     this);

	  undoCommand->setText
	    (tr("background color changed (%1, %2)").
	     arg(scenePos().x()).arg(scenePos().y()));
	  m_undoStack->push(undoCommand);
	}
    }
  else
    QApplication::processEvents();
}

void glitch_object::slotSelectBorderColor(void)
{
  QColorDialog dialog(m_parent);

  dialog.setCurrentColor
    (QColor(m_properties.value(Properties::BORDER_COLOR).toString()));
  dialog.setOption(QColorDialog::ShowAlphaChannel, true);
  dialog.setWindowIcon(windowIcon());
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      slotPropertyChanged("border_color", dialog.selectedColor().name());
    }
  else
    QApplication::processEvents();
}

void glitch_object::slotSetFont(void)
{
  foreach(auto widget, findChildren<QWidget *> ())
    if(widget)
      {
	auto font(glitch_ui::s_defaultApplicationFont);

	font.setBold(widget->font().bold());
	widget->setFont(font);
	widget->updateGeometry();
      }

  QApplication::processEvents();

  if(!isMandatory())
    resize(m_sizeBeforeFontChange);

  hideOrShowOccupied();
}

void glitch_object::slotSetPortColors(void)
{
  glitch_port_colors dialog(m_parent);

  dialog.setObject(this);
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();

      auto before(m_properties.value(Properties::PORT_COLORS).toString());

      m_properties[Properties::PORT_COLORS] = dialog.colors();

      if(m_undoStack)
	{
	  auto undoCommand = new glitch_undo_command
	    (m_properties.value(Properties::PORT_COLORS),
	     before,
	     glitch_undo_command::PROPERTY_CHANGED,
	     Properties::PORT_COLORS,
	     this);

	  undoCommand->setText
	    (tr("port colors changed (%1, %2)").
	     arg(scenePos().x()).arg(scenePos().y()));
	  m_undoStack->push(undoCommand);
	}

      emit changed();
    }
  else
    QApplication::processEvents();
}

void glitch_object::slotSetStyleSheet(void)
{
  auto string(styleSheet());
  glitch_style_sheet dialog(m_parent);

  dialog.setWidget(this);
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted && dialog.styleSheet() != string)
    {
      QApplication::processEvents();
      setStyleSheet(dialog.styleSheet());

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
    {
      QApplication::processEvents();

      /*
      ** Reset. Perhaps a preview?
      */

      setStyleSheet(string);
    }
}

void glitch_object::slotShowContextMenu(void)
{
  m_menu.clear();
  addActions(m_menu);

  if(!m_contextMenu)
    {
      m_contextMenu = new glitch_floating_context_menu(m_parent);
      m_contextMenu->setObject(this);
      connect(m_contextMenu,
	      SIGNAL(propertyChanged(const glitch_object::Properties,
				     const QVariant &)),
	      this,
	      SLOT(slotPropertyChanged(const glitch_object::Properties,
				       const QVariant &)));

      if(m_proxy)
	connect(m_proxy,
		&glitch_proxy_widget::changed,
		m_contextMenu,
		&glitch_floating_context_menu::slotObjectChanged);

      auto view = qobject_cast<glitch_object_view *> (m_parent);

      if(view)
	connect(view,
		&glitch_object_view::closed,
		m_contextMenu,
		&glitch_floating_context_menu::close);
    }

  m_contextMenu->addActions(m_actions.values());
  m_contextMenu->setIdentifier(m_id);
  m_contextMenu->setName(name());
  m_contextMenu->setProperty
    (Properties::Z_VALUE, m_properties.value(Properties::Z_VALUE));

  QSettings settings;

  if(settings.value("preferences/docked_widget_property_editors", true).
     toBool())
    emit dockPropertyEditor(m_contextMenu);
  else
    {
      m_contextMenu->resize(400, 650);
      m_contextMenu->show();
    }
}

void glitch_object::slotSimulateDelete(void)
{
  simulateDelete();
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

void glitch_object::slotWireObjects(void)
{
  auto scene = this->scene();

  if(!scene)
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  disconnect(scene,
	     SIGNAL(wireObjects(void)),
	     this,
	     SLOT(slotWireObjects(void)));

  foreach(const auto &pair, m_copiedConnectionsPositions)
    {
      glitch_object *object1 = nullptr;
      glitch_object *object2 = nullptr;

      foreach(auto object, scene->objects())
	if(object)
	  {
	    if(object->m_originalPosition == pair.first)
	      {
		if(!object1)
		  object1 = object;
	      }
	    else if(object->m_originalPosition == pair.second)
	      {
		if(!object2)
		  object2 = object;
	      }

	    if(object1 && object2)
	      break;
	  }

      if(!scene->areObjectsWired(object1, object2) && object1 && object2)
	{
	  auto wire = new glitch_wire(nullptr);

	  connect(scene,
		  SIGNAL(changed(const QList<QRectF> &)),
		  wire,
		  SLOT(slotUpdate(const QList<QRectF> &)));
	  object1->setWiredObject(object2, wire);
	  scene->addItem(wire);
	  wire->setBoundingRect(scene->sceneRect());
	  wire->setColor
	    (m_canvasSettings ? m_canvasSettings->wireColor() : QColor());
	  wire->setLeftProxy(object1->proxy());
	  wire->setRightProxy(object2->proxy());
	  wire->setWireType
	    (m_canvasSettings ? m_canvasSettings->wireType() : tr("Curve"));
	  wire->setWireWidth
	    (m_canvasSettings ? m_canvasSettings->wireWidth(): 2.5);
	}
    }

  QApplication::restoreOverrideCursor();
  QTimer::singleShot(250, this, &glitch_object::slotClearTemporaryContainers);
}
