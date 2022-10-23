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

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QMimeData>
#include <QTableWidget>
#include <QUndoStack>
#include <QtDebug>

#include <cmath>

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
#include "Arduino/glitch-object-logical-operator-arduino.h"
#include "Arduino/glitch-object-mathematics-arduino.h"
#include "Arduino/glitch-object-random-arduino.h"
#include "Arduino/glitch-object-serial-arduino.h"
#include "Arduino/glitch-object-stream-arduino.h"
#include "Arduino/glitch-object-syntax-arduino.h"
#include "Arduino/glitch-object-time-arduino.h"
#include "Arduino/glitch-object-trigonometry-arduino.h"
#include "Arduino/glitch-object-utilities-arduino.h"
#include "Arduino/glitch-object-variable-arduino.h"
#include "Arduino/glitch-structures-arduino.h"
#include "glitch-graphicsview.h"
#include "glitch-misc.h"
#include "glitch-proxy-widget.h"
#include "glitch-scene.h"
#include "glitch-undo-command.h"
#include "glitch-wire.h"

inline static qreal round(const qreal s, const qreal value)
{
  auto step = s;

  if(!(step < 0.0 || step > 0.0))
    step = 25.0;

  auto tmp = static_cast<int> (value + step / 2.0);

  tmp -= tmp % static_cast<int> (step);
  return static_cast<qreal> (tmp);
}

glitch_scene::glitch_scene(const glitch_common::ProjectTypes projectType,
			   QObject *parent):QGraphicsScene(parent)
{
  m_dotsGridsColor = Qt::white;
  m_mainScene = false;
  m_projectType = projectType;
  m_showCanvasDots = false;
  m_showCanvasGrids = !m_showCanvasDots;
  m_toolsOperation = glitch_tools::Operations::INTELLIGENT;
  m_undoStack = nullptr;
  setItemIndexMethod(QGraphicsScene::NoIndex);
}

glitch_scene::~glitch_scene()
{
  if(m_canvasSettings)
    disconnect(m_canvasSettings, nullptr, this, nullptr);
}

QList<glitch_object *> glitch_scene::objects(void) const
{
  QList<glitch_object *> widgets;
  auto list(items());

  foreach(auto i, list)
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(!proxy || !(proxy->flags() & QGraphicsItem::ItemIsSelectable))
	continue;

      widgets << qobject_cast<glitch_object *> (proxy->widget());
    }

  return widgets;
}

QList<glitch_object *> glitch_scene::orderedObjects(void) const
{
  QMultiMap<int, glitch_object *> widgets; /*
					   ** Order may not be unique. For
					   ** example, loop() and setup()
					   ** have 0 orders.
					   */
  auto list(items());

  foreach(auto i, list)
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(!proxy || !proxy->widget())
	continue;

      widgets.insert
	(objectOrder(proxy), qobject_cast<glitch_object *> (proxy->widget()));
    }

  return widgets.values();
}

QList<glitch_object *> glitch_scene::selectedObjects(void) const
{
  QList<glitch_object *> widgets;
  auto list(items());

  foreach(auto i, list)
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(!proxy ||
	 !(proxy->flags() & QGraphicsItem::ItemIsSelectable) ||
	 !proxy->isSelected())
	continue;

      widgets << qobject_cast<glitch_object *> (proxy->widget());
    }

  return widgets;
}

QPointer<QUndoStack> glitch_scene::undoStack(void) const
{
  return m_undoStack;
}

QPointer<glitch_canvas_settings> glitch_scene::canvasSettings(void) const
{
  return m_canvasSettings;
}

QSet<glitch_wire *> glitch_scene::wires(void) const
{
  return m_wires;
}

bool glitch_scene::allowDrag
(QGraphicsSceneDragDropEvent *event, const QString &t) const
{
  if(!event)
    return false;
  else
    {
      auto text(t.trimmed().remove("glitch-"));

      if(text.endsWith("(-)"))
	text = "subtraction (-)";
      else if(text.endsWith("(--)"))
	text = "decrement (--)";
      else if(text.endsWith("(-=)"))
	text = "subtraction (-=)";
      else
	text.remove(text.indexOf('-') + 1, text.lastIndexOf('-') - 7);

      if(m_mainScene)
	{
	  if(glitch_structures_arduino::containsStructure(text))
	    {
	      event->accept();
	      return true;
	    }
	  else
	    {
	      event->ignore();
	      return false;
	    }
	}
      else
	{
	  switch(m_projectType)
	    {
	    case glitch_common::ProjectTypes::ArduinoProject:
	      {
		if(glitch_structures_arduino::containsStructure(text))
		  /*
		  ** Functions cannot be defined in other functions.
		  */

		  if(!text.startsWith("arduino-function"))
		    {
		      event->accept();
		      return true;
		    }

		break;
	      }
	    default:
	      {
		break;
	      }
	    }

	  auto tableWidget = qobject_cast<QTableWidget *> (event->source());

	  if(glitch_misc::sameAncestors(tableWidget, this))
	    {
	      auto item = tableWidget->currentItem();

	      if(item)
		{
		  auto text(item->data(Qt::UserRole).toString().toLower());

		  if(text == "glitch-user-function")
		    {
		      event->accept();
		      return true;
		    }
		}
	    }

	  event->ignore();
	  return false;
	}
    }
}

bool glitch_scene::areObjectsWired
(glitch_object *object1, glitch_object *object2) const
{
  if(!object1 || !object2)
    return false;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QSetIterator<glitch_wire *> it(m_wires);

  while(it.hasNext())
    {
      auto wire = it.next();

      if(!wire)
	continue;

      if((object1->proxy() == wire->leftProxy() &&
	  object2->proxy() == wire->rightProxy()) ||
	 (object1->proxy() == wire->rightProxy() &&
	  object2->proxy() == wire->leftProxy()))
	{
	  QApplication::restoreOverrideCursor();
	  return true;
	}
    }

  QApplication::restoreOverrideCursor();
  return false;
}

glitch_proxy_widget *glitch_scene::addObject(glitch_object *object)
{
  if(!object || object->proxy() || object->scene() == this)
    return nullptr;

  auto proxy = new glitch_proxy_widget();

  connect(object,
	  &glitch_object::changed,
	  this,
	  &glitch_scene::changed,
	  Qt::UniqueConnection);
  connect(object,
	  &glitch_object::deletedViaContextMenu,
	  this,
	  &glitch_scene::slotObjectDeletedViaContextMenu,
	  Qt::UniqueConnection);
  connect(object,
	  &glitch_object::destroyed,
	  proxy,
	  &glitch_proxy_widget::deleteLater,
	  Qt::QueuedConnection);
  connect(object,
	  SIGNAL(destroyed(QObject *)),
	  this,
	  SIGNAL(destroyed(QObject *)),
	  Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection));
  connect(object,
	  SIGNAL(undoStackCreated(QUndoStack *)),
	  this,
	  SIGNAL(undoStackCreated(QUndoStack *)),
	  Qt::UniqueConnection);
  connect(proxy,
	  &glitch_proxy_widget::changed,
	  this,
	  &glitch_scene::slotProxyChanged);

  if(object->editScene())
    connect(this,
	    &glitch_scene::functionDeleted,
	    object->editScene(),
	    &glitch_scene::slotFunctionDeleted,
	    Qt::UniqueConnection);

  object->setCanvasSettings(m_canvasSettings);
  object->setProxy(proxy);
  object->setUndoStack(m_undoStack);
  proxy->setFlag(QGraphicsItem::ItemIsSelectable, true);
  proxy->setWidget(object);
  emit changed();

  if(qobject_cast<glitch_object_function_arduino *> (object))
    {
      auto function = qobject_cast<glitch_object_function_arduino *> (object);

      connect(function,
	      SIGNAL(nameChanged(const QString &,
				 const QString &,
				 glitch_object *)),
	      this,
	      SIGNAL(functionNameChanged(const QString &,
					 const QString &,
					 glitch_object *)),
	      Qt::UniqueConnection);
      connect(function,
	      SIGNAL(nameChanged(const QString &,
				 const QString &,
				 glitch_object *)),
	      this,
	      SLOT(slotFunctionNameChanged(const QString &,
					   const QString &,
					   glitch_object *)),
	      Qt::UniqueConnection);
      connect(function,
	      SIGNAL(returnTypeChanged(const QString &,
				       const QString &,
				       glitch_object *)),
	      this,
	      SIGNAL(functionReturnTypeChanged(const QString &,
					       const QString &,
					       glitch_object *)),
	      Qt::UniqueConnection);
      connect(function,
	      SIGNAL(returnTypeChanged(const QString &,
				       const QString &,
				       glitch_object *)),
	      this,
	      SLOT(slotFunctionReturnTypeChanged(const QString &,
						 const QString &,
						 glitch_object *)),
	      Qt::UniqueConnection);

      /*
      ** If the function does not exist, it is not a clone.
      */

      auto view = qobject_cast<glitch_graphicsview *> (views().value(0));

      if(view && !view->containsFunction(function->name()))
	emit functionAdded(function->name(), false);
      else
	emit functionAdded(function->name(), function->isClone());
    }

  emit sceneResized();
  return proxy;
}

glitch_tools::Operations glitch_scene::toolsOperation(void) const
{
  return m_toolsOperation;
}

void glitch_scene::addItem(QGraphicsItem *item)
{
  if(item && !item->scene())
    QGraphicsScene::addItem(item);

  auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (item);

  if(!m_redoUndoProxies.contains(proxy) && proxy)
    m_redoUndoProxies[proxy] = 0;

  if(proxy && proxy->object() && proxy->object()->editScene())
    connect(this,
	    &glitch_scene::functionDeleted,
	    proxy->object()->editScene(),
	    &glitch_scene::slotFunctionDeleted,
	    Qt::UniqueConnection);

  if(proxy && qobject_cast<glitch_object_function_arduino *> (proxy->widget()))
    emit functionAdded
      (qobject_cast<glitch_object_function_arduino *> (proxy->widget())->
       name(),
       qobject_cast<glitch_object_function_arduino *> (proxy->widget())->
       isClone());

  recordProxyOrder(proxy);

  auto wire = qgraphicsitem_cast<glitch_wire *> (item);

  if(wire)
    m_wires << wire;
}

void glitch_scene::artificialDrop(const QPointF &point, glitch_object *object)
{
  if(!object)
    return;

  QSignalBlocker blocker(this); // Are we creating from a database?
  auto proxy = addObject(object);

  if(proxy)
    {
      addItem(proxy);
      proxy->setPos(point);
    }
  else
    object->deleteLater();

  blocker.unblock();
}

void glitch_scene::bringToFront(glitch_proxy_widget *proxy)
{
  if(proxy)
    proxy->setZValue(1);
  else
    {
      auto list(items());

      foreach(auto i, list)
	{
	  auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

	  if(proxy)
	    proxy->setZValue(0);
	}
    }
}

void glitch_scene::deleteFunctionClones(const QString &name)
{
  auto list(items());

  foreach(auto i, list)
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(!proxy)
	continue;

      auto object = qobject_cast<glitch_object_function_arduino *>
	(proxy->widget());

      if(!object || !object->isClone())
	continue;

      if(name == object->name())
	{
	  if(m_undoStack)
	    {
	      auto undoCommand = new glitch_undo_command
		(glitch_undo_command::ITEM_DELETED, proxy, this);

	      undoCommand->setText(tr("function clone deleted"));
	      m_undoStack->push(undoCommand);
	    }
	  else
	    {
	      removeItem(proxy);
	      proxy->deleteLater();
	    }
	}
    }
}

void glitch_scene::deleteItems(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QList<QGraphicsItem *> list;

  foreach(auto i, items())
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(!proxy)
	continue;
      else if(proxy->isMandatory() || !proxy->isSelected())
	continue;
      else
	list << i;
    }

  if(list.isEmpty())
    {
      QApplication::restoreOverrideCursor();
      return;
    }

  auto began = false;
  auto state = false;

  foreach(auto i, list)
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(!proxy)
	continue;

      state = true;

      if(m_undoStack)
	{
	  if(!began)
	    {
	      began = true;
	      m_undoStack->beginMacro(tr("widget(s) deleted"));
	    }

	  auto object = qobject_cast<glitch_object_function_arduino *>
	    (proxy->widget());

	  if(object && !object->isClone())
	    {
	      deleteFunctionClones(object->name());
	      emit functionDeleted(object->name());
	    }

	  auto undoCommand = new glitch_undo_command
	    (glitch_undo_command::ITEM_DELETED, proxy, this);

	  m_undoStack->push(undoCommand);
	}
      else
	{
	  removeItem(proxy);
	  proxy->deleteLater();
	}
    }

  if(began && m_undoStack)
    m_undoStack->endMacro();

  QApplication::restoreOverrideCursor();

  if(state)
    emit changed();
}

void glitch_scene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
  if(event && event->mimeData())
    {
      auto text(event->mimeData()->text().trimmed());

      if(allowDrag(event, text))
	{
	  event->accept();
	  return;
	}
    }

  QGraphicsScene::dragEnterEvent(event);
}

void glitch_scene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
  if(event && event->mimeData())
    {
      auto text(event->mimeData()->text().trimmed());

      if(allowDrag(event, text))
	{
	  event->accept();
	  return;
	}
    }

  QGraphicsScene::dragMoveEvent(event);
}

void glitch_scene::drawBackground(QPainter *painter, const QRectF &rect)
{
  if(!painter)
    {
      QGraphicsScene::drawBackground(painter, rect);
      return;
    }

  painter->fillRect(rect, backgroundBrush());
  painter->setBrushOrigin(0, 0);

  if(m_showCanvasDots)
    {
      QPen pen;

      pen.setBrush(m_dotsGridsColor);
      pen.setWidthF(1.25);
      painter->setPen(pen);

      QVector<QPointF> points;
      auto left = static_cast<qreal> (rect.left()) -
	(static_cast<int> (rect.left()) % 20);
      auto top = static_cast<qreal> (rect.top()) -
	(static_cast<int> (rect.top()) % 20);
      const qreal step = 20.0;

      for(auto x = left; std::isless(x, rect.right()); x += step)
	for(auto y = top; std::isless(y, rect.bottom()); y += step)
	  points << QPointF(x, y);

      painter->drawPoints(points.data(), points.size());
    }
  else
    {
      QPen pen;
      auto color(m_dotsGridsColor);
      qreal step = 20.0;

      pen.setWidthF(1.25);

      for(int i = 1; i <= 2; i++)
	{
	  if(i == 1)
	    {
	      color.setAlpha(50);
	      pen.setStyle(Qt::DashLine);
	    }
	  else
	    {
	      color.setAlpha(100);
	      pen.setStyle(Qt::SolidLine);
	      step = 100.0;
	    }

	  pen.setBrush(color);
	  painter->setPen(pen);

	  auto start = round(step, rect.left());

	  if(rect.left() < start)
	    start -= step;

	  for(auto x = start - step; x < rect.right(); x += step)
	    painter->drawLine(x, rect.top(), x, rect.bottom());

	  start = round(step, rect.top());

	  if(rect.top() < start)
	    start -= step;

	  for(auto y = start - step; y < rect.bottom(); y += step)
	    painter->drawLine(rect.left(), y, rect.right(), y);
	}
    }
}

void glitch_scene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
  if(event && event->mimeData())
    {
      auto text(event->mimeData()->text().toLower().trimmed());

      glitch_object *object = nullptr;

      if(allowDrag(event, text))
	{
	  auto view = views().value(0);

	  if(text.startsWith("glitch-arduino-advanced i/o-"))
	    object = new glitch_object_advanced_io_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-analog i/o-analog"))
	    object = new glitch_object_analog_io_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-arithmetic operators"))
	    object = new glitch_object_arithmetic_operator_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-bits and bytes-"))
	    object = new glitch_object_bits_and_bytes_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-bitwise operators"))
	    object = new glitch_object_bitwise_operator_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-boolean operators"))
	    object = new glitch_object_boolean_operator_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-characters"))
	    object = new glitch_object_characters_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-compound operators"))
	    object = new glitch_object_compound_operator_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-constant"))
	    object = new glitch_object_constant_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-conversion"))
	    object = new glitch_object_conversion_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-digital i/o-"))
	    object = new glitch_object_digital_io_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-external interrupts") ||
		  text.startsWith("glitch-arduino-interrupts"))
	    object = new glitch_object_interrupts_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-flow control-"))
	    object = new glitch_object_flow_control_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-function"))
	    object = new glitch_object_function_arduino
	      (text.mid(static_cast<int> (qstrlen("glitch-arduino-function-"))),
	       view);
	  else if(text.startsWith("glitch-arduino-logical operators"))
	    object = new glitch_object_logical_operator_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-mathematics"))
	    object = new glitch_object_mathematics_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-random"))
	    object = new glitch_object_random_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-serial"))
	    object = new glitch_object_serial_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-structures-block comment"))
	    object = new glitch_object_block_comment_arduino(view);
	  else if(text.startsWith("glitch-arduino-structures-function"))
	    {
	      if(text == "glitch-arduino-structures-function()")
		object = new glitch_object_function_arduino(view);
	      else
		object = new glitch_object_function_arduino
		  (text.
		   mid(static_cast<int> (qstrlen("glitch-arduino-"
						 "structures-function-"))),
		   view);
	    }
	  else if(text.startsWith("glitch-arduino-stream"))
	    object = new glitch_object_stream_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-syntax"))
	    object = new glitch_object_syntax_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-time"))
	    object = new glitch_object_time_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-trigonometry"))
	    object = new glitch_object_trigonometry_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-utilities"))
	    object = new glitch_object_utilities_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-variable"))
	    object = new glitch_object_variable_arduino(text, view);
	}

      if(object)
	{
	  event->accept();

	  auto proxy = addObject(object);

	  if(proxy)
	    {
	      if(m_undoStack)
		{
		  auto undoCommand = new glitch_undo_command
		    (glitch_undo_command::ITEM_ADDED, proxy, this);

		  undoCommand->setText
		    (tr("item added (%1, %2)").
		     arg(event->scenePos().x()).arg(event->scenePos().y()));
		  m_undoStack->push(undoCommand);
		}
	      else
		addItem(proxy);

	      proxy->setPos(event->scenePos());
	      emit changed();
	    }
	  else
	    object->deleteLater();

	  return;
	}
    }

  QGraphicsScene::dropEvent(event);
}

void glitch_scene::keyPressEvent(QKeyEvent *event)
{
  if(!event)
    {
      QGraphicsScene::keyPressEvent(event);
      return;
    }

  switch(event->key())
    {
    case Qt::Key_Down:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
      {
	QPoint point;
	auto began = false;
	auto list(selectedItems());
	auto moved = false;
	auto updateMode = QGraphicsView::FullViewportUpdate;
	auto view = views().value(0);
	int pixels = (event->modifiers() & Qt::ShiftModifier) ? 50 : 1;

	if(view)
	  {
	    updateMode = view->viewportUpdateMode();
	    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	  }

	foreach(auto i, list)
	  {
	    auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

	    if(!proxy)
	      continue;

	    auto object = qobject_cast<glitch_object *> (proxy->widget());

	    if(!object)
	      continue;

	    if(qobject_cast<QLineEdit *> (object->focusWidget()) ||
	       qobject_cast<QPlainTextEdit *> (object->focusWidget()))
	      {
		QGraphicsScene::keyPressEvent(event);
		continue;
	      }

	    if(!proxy->isMovable())
	      continue;

	    point = object->pos();

	    switch(event->key())
	      {
	      case Qt::Key_Down:
		{
		  point.setY(point.y() + pixels);
		  break;
		}
	      case Qt::Key_Left:
		{
		  point.setX(point.x() - pixels);
		  break;
		}
	      case Qt::Key_Right:
		{
		  point.setX(point.x() + pixels);
		  break;
		}
	      case Qt::Key_Up:
		{
		  point.setY(point.y() - pixels);
		  break;
		}
	      default:
		{
		  break;
		}
	      }

	    if(point.x() < 0 || point.y() < 0)
	      continue;

	    auto previousPosition(proxy->scenePos());

	    object->move(point);

	    if(previousPosition != proxy->pos())
	      moved = true;

	    if(moved)
	      {
		if(!began)
		  {
		    began = true;

		    if(m_undoStack)
		      m_undoStack->beginMacro("widget(s) moved");
		  }

		if(m_undoStack)
		  {
		    auto undoCommand = new glitch_undo_command
		      (previousPosition,
		       glitch_undo_command::ITEM_MOVED,
		       proxy,
		       this);

		    m_undoStack->push(undoCommand);
		  }
	      }
	  }

	if(began)
	  {
	    if(m_undoStack)
	      m_undoStack->endMacro();

	    emit changed();
	  }

	if(moved)
	  emit sceneResized();

	if(view)
	  view->setViewportUpdateMode(updateMode);

	return;
      }
    case Qt::Key_Z:
      {
	if(event->modifiers() & Qt::ControlModifier)
	  {
	    if(event->modifiers() & Qt::ShiftModifier)
	      redo();
	    else
	      undo();
	  }
      }
    default:
      {
	break;
      }
    }

  QGraphicsScene::keyPressEvent(event);
}

void glitch_scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  if(event && !m_lastScenePos.isNull())
    {
      auto list(selectedItems());
      auto moved = false;

      foreach(auto i, list)
	{
	  auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

	  if(!proxy || !proxy->isMovable())
	    continue;

	  auto object = qobject_cast<glitch_object *> (proxy->widget());

	  if(object && object->mouseOverScrollBar(event->scenePos()))
	    continue;

	  auto point(proxy->mapToParent(event->scenePos() - m_lastScenePos));

	  if(point.x() < 0 || point.y() < 0)
	    continue;

	  moved = true;
	  proxy->setPos(point);
	}

      m_lastScenePos = event->scenePos();

      if(moved)
	{
	  emit changed();
	  emit sceneResized();

	  auto view = views().value(0);

	  if(view && view->viewport())
	    view->viewport()->setCursor(Qt::ClosedHandCursor);
	}
    }

  QGraphicsScene::mouseMoveEvent(event);
}

void glitch_scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  if(m_toolsOperation == glitch_tools::Operations::SELECT)
    {
      m_lastScenePos = QPointF();
      m_movedPoints.clear();
    }

  if(event)
    {
      auto item = itemAt(event->scenePos(), QTransform());

      if(!qgraphicsitem_cast<glitch_wire *> (item) && item)
	{
	  auto parent = item->parentItem();

	  if(!parent)
	    parent = item;

	  if(!parent)
	    goto done_label;

	  auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (parent);

	  if(proxy)
	    {
	      if(m_toolsOperation == glitch_tools::Operations::INTELLIGENT)
		{
		  if(proxy->hoveredSection() ==
		     glitch_proxy_widget::Sections::XYZ)
		    {
		      // Select Mode

		      m_lastScenePos = QPointF();
		      m_movedPoints.clear();
		    }
		  else
		    {
		      auto instance = qobject_cast<QGuiApplication *>
			(QApplication::instance());

		      if(instance &&
			 instance->keyboardModifiers() & Qt::ShiftModifier)
			{
			  wireDisconnectObjects(event->scenePos(), proxy);
			  goto done_label;
			}

		      wireConnectObjects(proxy);
		      goto done_label;
		    }
		}
	      else if(m_toolsOperation ==
		      glitch_tools::Operations::WIRE_CONNECT)
		{
		  wireConnectObjects(proxy);
		  goto done_label;
		}
	      else if(m_toolsOperation ==
		      glitch_tools::Operations::WIRE_DISCONNECT)
		{
		  wireDisconnectObjects(event->scenePos(), proxy);
		  goto done_label;
		}

	      auto object = proxy->widget();

	      if(object)
		{
		  auto point(proxy->mapFromScene(event->scenePos()).toPoint());

		  if(qobject_cast<QComboBox *> (object->childAt(point)))
		    {
		      bringToFront(nullptr);
		      bringToFront(proxy);
		      goto done_label;
		    }
		  else if(qobject_cast<QLineEdit *> (object->childAt(point)))
		    {
		      auto lineEdit = qobject_cast<QLineEdit *>
			(object->childAt(point));

		      point = lineEdit->mapFromParent(point);
		      lineEdit->setCursorPosition
			(lineEdit->cursorPositionAt(point));
		      lineEdit->setFocus();
		    }
		  else if(qobject_cast<QToolButton *> (object->childAt(point)))
		    {
		      auto toolButton = qobject_cast<QToolButton *>
			(object->childAt(point));

		      toolButton->setFocus();
		    }
		  else
		    {
		      foreach(auto child, object->findChildren<QWidget *> ())
			if(child &&
			   child->objectName().
			   contains(QString::fromUtf8("viewport")))
			  {
			    child->setFocus();
			    goto exit_label;
			  }

		      object->setFocus();

		    exit_label:
		      {
		      }
		    }
		}
	    }

	  if(event->button() == Qt::RightButton)
	    {
	      if(!parent->isSelected())
		clearSelection();

	      event->setButton(Qt::LeftButton);
	      parent->setSelected(true);
	      goto done_label;
	    }

	  if(event->modifiers() & Qt::ControlModifier)
	    {
	      m_lastScenePos = event->scenePos();
	      parent->setSelected(!parent->isSelected());
	    }
	  else
	    {
	      if(!parent->isSelected())
		clearSelection();

	      m_lastScenePos = event->scenePos();
	      parent->setSelected(true);
	    }

	  if(!m_lastScenePos.isNull())
	    {
	      auto list(selectedItems());

	      foreach(auto i, list)
		{
		  auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

		  if(!proxy || !proxy->isMovable())
		    continue;
		  else
		    m_movedPoints << QPair<QPointF, glitch_proxy_widget *>
		      (proxy->scenePos(), proxy);
		}
	    }
	}
      else if(m_toolsOperation == glitch_tools::Operations::SELECT)
	clearSelection();
    }
  else if(m_toolsOperation == glitch_tools::Operations::SELECT)
    clearSelection();

 done_label:
  QGraphicsScene::mousePressEvent(event);
  emit mousePressed();
}

void glitch_scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  m_lastScenePos = QPointF();

  if(!m_movedPoints.isEmpty() && m_undoStack)
    {
      auto began = false;

      foreach(const auto &movedPoint, m_movedPoints)
	{
	  if(!movedPoint.second)
	    continue;
	  else if(movedPoint.first == movedPoint.second->pos())
	    continue;
	  else if(!began)
	    {
	      began = true;
	      m_undoStack->beginMacro(tr("widget(s) moved"));
	    }

	  auto undoCommand = new glitch_undo_command
	    (movedPoint.first,
	     glitch_undo_command::ITEM_MOVED,
	     movedPoint.second,
	     this);

	  m_undoStack->push(undoCommand);
	}

      m_movedPoints.clear();

      if(began)
	{
	  m_undoStack->endMacro();
	  emit changed();
	}
    }

  m_movedPoints.clear();

  auto view = views().value(0);

  if(view && view->viewport())
    {
      auto cursor = QApplication::overrideCursor();

      if(cursor)
	view->viewport()->setCursor(cursor->shape());
      else
	view->viewport()->setCursor(Qt::ArrowCursor);
    }

  QGraphicsScene::mouseReleaseEvent(event);
}

void glitch_scene::purgeRedoUndoProxies(void)
{
  /*
  ** Move proxies from the redo/undo stack onto the scene.
  */

  QMutableMapIterator<QPointer<glitch_proxy_widget>, char> it
    (m_redoUndoProxies);
  auto list(items());

  while(it.hasNext())
    {
      it.next();

      if(!list.contains(it.key()))
	{
	  if(it.key())
	    QGraphicsScene::addItem(it.key());

	  it.remove();
	}
    }
}

void glitch_scene::recordProxyOrder(glitch_proxy_widget *proxy)
{
  if(!proxy || proxy->isMandatory())
    return;

  auto point(m_objectsHash.value(proxy));

  m_objectsHash[proxy] = glitch_point(proxy->pos());

  if(m_objectsMap.contains(point, proxy))
    m_objectsMap.remove(point, proxy);

  m_objectsMap.insert(glitch_point(proxy->pos()), proxy);
}

void glitch_scene::redo(void)
{
  if(m_undoStack && m_undoStack->canRedo())
    {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      m_undoStack->redo();
      emit changed();
      QApplication::restoreOverrideCursor();
    }
}

void glitch_scene::removeItem(QGraphicsItem *item)
{
  /*
  ** Do not add removed items to redo/undo stack here.
  */

  if(item && item->scene() == this)
    QGraphicsScene::removeItem(item);

  auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (item);

  if(proxy)
    {
      auto object = qobject_cast<glitch_object_function_arduino *>
	(proxy->widget());

      if(object && !object->isClone())
	{
	  deleteFunctionClones(object->name());
	  emit functionDeleted(object->name());
	}

      /*
      ** Remove object order information.
      */

      auto point(m_objectsHash.value(proxy));

      m_objectsHash.remove(proxy);
      m_objectsMap.remove(point, proxy);
    }

  auto wire = qgraphicsitem_cast<glitch_wire *> (item);

  if(wire)
    m_wires.remove(wire);
}

void glitch_scene::setCanvasSettings(glitch_canvas_settings *canvasSettings)
{
  if(!canvasSettings || m_canvasSettings)
    return;

  m_canvasSettings = canvasSettings;

  foreach(auto object, objects())
    if(object)
      object->setCanvasSettings(m_canvasSettings);

  connect(m_canvasSettings,
	  SIGNAL(accepted(const bool)),
	  this,
	  SLOT(slotCanvasSettingsChanged(const bool)));
  slotCanvasSettingsChanged(false);
}

void glitch_scene::setDotsGridsColor(const QColor &color)
{
  if(color.isValid())
    m_dotsGridsColor = color;
  else
    m_dotsGridsColor = Qt::white;
}

void glitch_scene::setMainScene(const bool state)
{
  m_mainScene = state;
}

void glitch_scene::setShowCanvasDots(const bool state)
{
  m_showCanvasDots = state;
  m_showCanvasGrids = !state;
}

void glitch_scene::setShowCanvasGrids(const bool state)
{
  m_showCanvasDots = !state;
  m_showCanvasGrids = state;
}

void glitch_scene::setUndoStack(QUndoStack *undoStack)
{
  m_undoStack = undoStack;
}

void glitch_scene::slotCanvasSettingsChanged(const bool state)
{
  Q_UNUSED(state);

  if(!m_canvasSettings)
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QSetIterator<glitch_wire *> it(m_wires);

  while(it.hasNext())
    {
      auto wire = it.next();

      if(wire)
	{
	  wire->setColor(m_canvasSettings->wireColor());
	  wire->setWireType(m_canvasSettings->wireType());
	}
    }

  m_dotsGridsColor = m_canvasSettings->dotsGridsColor();
  m_showCanvasDots = m_canvasSettings->showCanvasDots();
  m_showCanvasGrids = !m_showCanvasDots;
  setBackgroundBrush(m_canvasSettings->canvasBackgroundColor());
  update();
  QApplication::restoreOverrideCursor();
}

void glitch_scene::slotFunctionDeleted(const QString &name)
{
  foreach(auto object, objects())
    if(object && object->name() == name && object->type().contains("function"))
      {
	if(m_undoStack)
	  {
	    auto undoCommand = new glitch_undo_command
	      (glitch_undo_command::ITEM_DELETED, object->proxy(), this);

	    undoCommand->setText(tr("function clone deleted"));
	    m_undoStack->push(undoCommand);
	  }
	else
	  {
	    removeItem(object->proxy());
	    object->deleteLater();
	  }

	break;
      }
}

void glitch_scene::slotFunctionNameChanged(const QString &after,
					   const QString &before,
					   glitch_object *object)
{
  Q_UNUSED(after);
  Q_UNUSED(before);
  Q_UNUSED(object);
}

void glitch_scene::slotFunctionReturnTypeChanged(const QString &after,
						 const QString &before,
						 glitch_object *object)
{
  Q_UNUSED(after);
  Q_UNUSED(before);
  Q_UNUSED(object);
}

void glitch_scene::slotObjectDeletedViaContextMenu(void)
{
  auto object = qobject_cast<glitch_object *> (sender());

  if(!object)
    return;

  if(m_undoStack && object->proxy())
    {
      m_undoStack->beginMacro(tr("widget(s) deleted"));

      if(qobject_cast<glitch_object_function_arduino *> (object) &&
	 !qobject_cast<glitch_object_function_arduino *> (object)->isClone())
	{
	  deleteFunctionClones(object->name());
	  emit functionDeleted
	    (qobject_cast<glitch_object_function_arduino *> (object)->name());
	}

      auto undoCommand = new glitch_undo_command
	(glitch_undo_command::ITEM_DELETED, object->proxy(), this);

      m_undoStack->push(undoCommand);
      m_undoStack->endMacro();
    }
  else
    object->deleteLater();

  emit changed();
}

void glitch_scene::slotProxyChanged(void)
{
  recordProxyOrder(qobject_cast<glitch_proxy_widget *> (sender()));
}

void glitch_scene::slotToolsOperationChanged
(const glitch_tools::Operations operation)
{
  m_toolsOperation = operation;
}

void glitch_scene::undo(void)
{
  if(m_undoStack && m_undoStack->canUndo())
    {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      m_undoStack->undo();

      if(m_undoStack->canUndo())
	emit changed();

      QApplication::restoreOverrideCursor();
    }
}

void glitch_scene::wireConnectObjects(glitch_proxy_widget *proxy)
{
  if(!proxy)
    {
      m_objectsToWire.clear();
      return;
    }

  /*
  ** Do not wire objects if they are wired. This requirement will vary
  ** with object type.
  */

  auto object = qobject_cast<glitch_object *> (proxy->widget());

  if(!object)
    {
      m_objectsToWire.clear();
      return;
    }

  if(m_objectsToWire.isEmpty())
    {
      if(object->hasInput() || object->hasOutput())
	{
	  if(proxy->hoveredSection() == glitch_proxy_widget::Sections::LEFT)
	    m_objectsToWire["input"] = proxy;
	  else if(proxy->hoveredSection() ==
		  glitch_proxy_widget::Sections::RIGHT)
	    m_objectsToWire["output"] = proxy;
	}

      return;
    }
  else if(m_objectsToWire.size() == 1)
    {
      if(object->hasInput() || object->hasOutput())
	{
	  if(!m_objectsToWire.values().contains(proxy))
	    {
	      if(proxy->hoveredSection() == glitch_proxy_widget::Sections::LEFT)
		m_objectsToWire["input"] = proxy;
	      else if(proxy->hoveredSection() ==
		      glitch_proxy_widget::Sections::RIGHT)
		m_objectsToWire["output"] = proxy;
	    }
	}
    }

  if(m_objectsToWire.value("input") && m_objectsToWire.value("output"))
    {
      auto object1 = qobject_cast<glitch_object *>
	(m_objectsToWire.value("input")->widget());
      auto object2 = qobject_cast<glitch_object *>
	(m_objectsToWire.value("output")->widget());

      if(!areObjectsWired(object1, object2) && object1 && object2)
	{
	  auto wire(new glitch_wire(nullptr));

	  if(m_undoStack)
	    {
	      auto undoCommand = new glitch_undo_command
		(glitch_undo_command::WIRE_ADDED, this, wire);

	      undoCommand->setText(tr("objects connected"));
	      m_undoStack->push(undoCommand);
	    }
	  else
	    addItem(wire);

	  object2->setWiredObject(object1, wire);
	  wire->setBoundingRect(sceneRect());

	  if(m_canvasSettings)
	    {
	      wire->setColor(m_canvasSettings->wireColor());
	      wire->setWireType(m_canvasSettings->wireType());
	    }

	  wire->setLeftProxy(m_objectsToWire.value("output"));
	  wire->setRightProxy(m_objectsToWire.value("input"));
	  m_objectsToWire.clear();
	  emit changed();
	}
      else
	m_objectsToWire.clear();
    }
}

void glitch_scene::wireDisconnectObjects
(const QPointF &point, glitch_proxy_widget *proxy)
{
  Q_UNUSED(point);

  if(!proxy)
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  auto began = false;
  auto state = false;

  QMutableSetIterator<glitch_wire *> it(m_wires);

  while(it.hasNext())
    {
      auto wire = it.next();

      if(!wire)
	{
	  it.remove();
	  continue;
	}

      glitch_proxy_widget *p = nullptr;

      if(proxy->hoveredSection() == glitch_proxy_widget::Sections::LEFT)
	p = wire->rightProxy();
      else if(proxy->hoveredSection() == glitch_proxy_widget::Sections::RIGHT)
	p = wire->leftProxy();

      if(p == proxy)
	{
	  if(m_undoStack)
	    {
	      if(!began)
		{
		  began = true;
		  m_undoStack->beginMacro(tr("widget(s) disconnected"));
		}

	      auto undoCommand = new glitch_undo_command
		(glitch_undo_command::WIRE_DELETED, this, wire);

	      m_undoStack->push(undoCommand);
	      state = true;
	    }
	  else
	    {
	      it.remove();
	      state = true;
	      wire->deleteLater();
	    }
	}
    }

  if(began && m_undoStack)
    m_undoStack->endMacro();

  QApplication::restoreOverrideCursor();

  if(state)
    emit changed();
}
