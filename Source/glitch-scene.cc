/*
** Copyright (c) 2015 - 10^10^10, Alexis Megas.
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
#include <QSqlError>
#include <QSqlQuery>
#include <QTableView>
#include <QUndoStack>
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
#include "Arduino/glitch-object-library-function-arduino.h"
#include "Arduino/glitch-object-mathematics-arduino.h"
#include "Arduino/glitch-object-random-arduino.h"
#include "Arduino/glitch-object-serial-arduino.h"
#include "Arduino/glitch-object-syntax-arduino.h"
#include "Arduino/glitch-object-time-arduino.h"
#include "Arduino/glitch-object-trigonometry-arduino.h"
#include "Arduino/glitch-object-utilities-arduino.h"
#include "Arduino/glitch-object-variable-arduino.h"
#include "Arduino/glitch-structures-arduino.h"
#include "glitch-graphicsview.h"
#include "glitch-object-arrow.h"
#include "glitch-object-view.h"
#include "glitch-resize-widget.h"
#include "glitch-scene.h"
#include "glitch-ui.h"
#include "glitch-undo-command.h"
#include "glitch-variety.h"
#include "glitch-wire.h"

inline static qreal round(const qreal s, const qreal value)
{
  auto step = s;

  if(!(step < 0.0 || step > 0.0))
    step = 25.0;

  auto tmp = static_cast<int> (step / 2.0 + value);

  tmp -= tmp % static_cast<int> (step);
  return static_cast<qreal> (tmp);
}

glitch_scene::glitch_scene
(const glitch_common::ProjectTypes projectType, QObject *parent):
  QGraphicsScene(parent)
{
  m_dotsGridsColor = Qt::white;
  m_loadingFromFile = false;
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

QGraphicsView *glitch_scene::primaryView(void) const
{
  foreach(auto view, views())
    if(qobject_cast<glitch_graphicsview *> (view) ||
       qobject_cast<glitch_object_view *> (view))
      return view;

  return nullptr;
}

QList<glitch_object *> glitch_scene::allObjects(void) const
{
  QList<glitch_object *> widgets;

  foreach(auto object1, objects())
    if(object1)
      {
	widgets << object1;

	foreach(auto object2, object1->allObjects())
	  widgets << object2;
      }

  return widgets;
}

QList<glitch_object *> glitch_scene::objects(void) const
{
  QList<glitch_object *> objects;

  foreach(auto i, items())
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(proxy && proxy->object())
	objects << proxy->object();
    }

  return objects;
}

QList<glitch_object *> glitch_scene::orderedObjects(void) const
{
  QMultiMap<int, glitch_object *> widgets; /*
					   ** Order may not be unique. For
					   ** example, loop() and setup()
					   ** have 0 orders.
					   */

  foreach(auto i, items())
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

  foreach(auto i, items())
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(proxy &&
	 proxy->flags() & QGraphicsItem::ItemIsSelectable &&
	 proxy->isSelected() &&
	 proxy->widget())
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

      if(text.endsWith("---"))
	text = "--";
      else if(text.endsWith("--"))
	text = "-";
      else if(text.endsWith("-="))
	text = "-=";
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

	  auto tableView = qobject_cast<QTableView *> (event->source());

	  if(glitch_variety::sameAncestors(tableView, this))
	    {
	      auto index = tableView->currentIndex();

	      if(index.isValid())
		{
		  auto text(index.data(Qt::UserRole + 1).toString().toLower());

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

bool glitch_scene::event(QEvent *event)
{
#ifndef GLITCH_FUTURE
  if(event)
    switch(event->type())
      {
      case QEvent::GraphicsSceneMouseMove:
	{
	  auto e = static_cast<QGraphicsSceneMouseEvent *> (event);

	  if(e && e->buttons() == Qt::NoButton)
	    {
	      QRectF rect(e->scenePos(), QSizeF(20.0, 20.0));

	      rect.moveCenter(e->scenePos());

	      auto items(this->items(rect));

	      foreach(auto item, items)
		{
		  auto proxy = qgraphicsitem_cast<glitch_proxy_widget *>
		    (item);

		  if(proxy)
		    {
		      proxy->prepareHoverSection(e->scenePos());
		      proxy->update();
		      break;
		    }
		}
	    }

	  break;
	}
      default:
	{
	  break;
	}
      }
#endif

  return QGraphicsScene::event(event);
}

bool glitch_scene::objectToBeWired(glitch_proxy_widget *proxy) const
{
  QHashIterator<QString, QPointer<glitch_proxy_widget> > it(m_objectsToWire);

  while(it.hasNext())
    {
      it.next();

      if(it.value().data() == proxy)
	return true;
    }

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
	  &glitch_object::copy,
	  this,
	  &glitch_scene::copy,
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
	  &glitch_object::saveSignal,
	  this,
	  &glitch_scene::saveSignal,
	  Qt::UniqueConnection);
  connect(object,
	  SIGNAL(destroyed(QObject *)),
	  this,
	  SIGNAL(destroyed(QObject *)),
	  Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection));
  connect(object,
	  SIGNAL(dockPropertyEditor(QWidget *)),
	  this,
	  SIGNAL(dockPropertyEditor(QWidget *)),
	  Qt::UniqueConnection);
  connect(proxy,
	  &glitch_proxy_widget::changed,
	  this,
	  &glitch_scene::slotProxyChanged);
  connect(proxy,
	  SIGNAL(geometryChangedSignal(const QRectF &)),
	  this,
	  SLOT(slotProxyGeometryChanged(const QRectF &)));

  if(object->editScene())
    connect(this,
	    &glitch_scene::functionDeleted,
	    object->editScene(),
	    &glitch_scene::slotFunctionDeleted,
	    Qt::UniqueConnection);

  object->setCanvasSettings(m_canvasSettings);
  object->setProxy(proxy);
  object->setUndoStack(m_undoStack);

  /*
  ** Eliminate MacOS error (outside any known screen, using primary screen).
  */

#ifndef Q_OS_MACOS
  if(m_loadingFromFile)
    object->setVisible(false);
#endif

  proxy->setFlag(QGraphicsItem::ItemIsSelectable, true);
  proxy->setWidget(object);

#ifndef Q_OS_MACOS
  if(m_loadingFromFile)
    QTimer::singleShot(50, object, &glitch_object::show);
#endif

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
	      SIGNAL(returnPointerChanged(const bool,
					  const bool,
					  glitch_object *)),
	      this,
	      SIGNAL(functionReturnPointerChanged(const bool,
						  const bool,
						  glitch_object *)),
	      Qt::UniqueConnection);
      connect(function,
	      SIGNAL(returnPointerChanged(const bool,
					  const bool,
					  glitch_object *)),
	      this,
	      SLOT(slotFunctionReturnPointerChanged(const bool,
						    const bool,
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

      auto view = qobject_cast<glitch_graphicsview *> (primaryView());

      if(view && view->containsFunction(function->name()) == false)
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

int glitch_scene::objectOrder(glitch_proxy_widget *proxy) const
{
  if(!proxy)
    return -1;

  return std::distance
    (m_objectsMap.begin(),
     m_objectsMap.find(m_objectsHash.value(proxy), proxy));
}

void glitch_scene::addItem(QGraphicsItem *item)
{
  if(item && item->scene() == nullptr)
    QGraphicsScene::addItem(item);

  auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (item);

  if(proxy)
    {
      if(!m_redoUndoProxies.contains(proxy))
	m_redoUndoProxies[proxy] = 0;

      if(qobject_cast<glitch_object_function_arduino *> (proxy->widget()))
	emit functionAdded
	  (qobject_cast<glitch_object_function_arduino *> (proxy->widget())->
	   name(),
	   qobject_cast<glitch_object_function_arduino *> (proxy->widget())->
	   isClone());

      if(proxy->object() && proxy->object()->editScene())
	connect(this,
		&glitch_scene::functionDeleted,
		proxy->object()->editScene(),
		&glitch_scene::slotFunctionDeleted,
		Qt::UniqueConnection);

      proxy->setParent(this);
    }

  recordProxyOrder(proxy);

  auto wire = qgraphicsitem_cast<glitch_wire *> (item);

  if(wire)
    m_wires << wire;

  if(glitch_ui::s_copiedObjectsSet.contains(proxy ? proxy->object() : nullptr))
    /*
    ** Connect the pasted objects.
    */

    QTimer::singleShot(50, this, SIGNAL(wireObjects(void)));
}

void glitch_scene::artificialDrop
(const QPointF &point, glitch_object *object, bool &ok)
{
  if(!object)
    {
      ok = false;
      return;
    }

  QSignalBlocker blocker(this); // Are we creating from a database?
  auto proxy = addObject(object);

  if(proxy)
    {
      addItem(proxy);
      proxy->blockSignals(true);
      proxy->setPos(point);
      proxy->blockSignals(false);
    }
  else
    ok = false;

  blocker.unblock();

  if(glitch_ui::s_copiedObjectsSet.contains(object) && ok)
    connect(this,
	    SIGNAL(wireObjects(void)),
	    object,
	    SLOT(slotWireObjects(void)),
	    Qt::UniqueConnection);
}

void glitch_scene::bringToFront(glitch_proxy_widget *proxy)
{
  if(proxy)
    proxy->setZValue(1);
  else
    foreach(auto i, items())
      {
	auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

	if(proxy)
	  proxy->setZValue(0);
      }
}

void glitch_scene::deleteFunctionClones(const QString &name)
{
  foreach(auto i, items())
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
		(glitch_undo_command::Types::ITEM_DELETED, proxy, this);

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

void glitch_scene::deleteItems
(const QList<QGraphicsItem *> &items, const bool redoUndoMacro)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QList<QGraphicsItem *> list;

  if(items.isEmpty())
    foreach(auto i, this->items())
      {
	auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

	if(!proxy)
	  continue;
	else if(!proxy->isSelected() || proxy->isMandatory())
	  continue;
	else
	  list << i;
      }
  else
    list = items;

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
	  if(!began && redoUndoMacro)
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
	    (glitch_undo_command::Types::ITEM_DELETED, proxy, this);

	  m_undoStack->push(undoCommand);
	}
      else
	{
	  removeItem(proxy);
	  proxy->deleteLater();
	}
    }

  if(began && m_undoStack && redoUndoMacro)
    m_undoStack->endMacro();

  QApplication::restoreOverrideCursor();

  if(state)
    emit changed();
}

void glitch_scene::disconnectWireIfNecessary(glitch_wire *wire)
{
  if(!wire)
    return;

  auto state = false;

  if(m_toolsOperation == glitch_tools::Operations::INTELLIGENT)
    {
      auto instance = qobject_cast<QGuiApplication *>
	(QApplication::instance());

      if(instance && instance->keyboardModifiers() & Qt::ControlModifier)
	state = true;
    }
  else if(m_toolsOperation == glitch_tools::Operations::WIRE_DISCONNECT)
    state = true;

  if(!state)
    return;

  if(m_undoStack)
    {
      auto undoCommand = new glitch_undo_command
	(glitch_undo_command::Types::WIRE_DELETED, this, wire);

      undoCommand->setText(tr("widgets disconnected"));
      m_undoStack->push(undoCommand);
    }
  else
    wire->deleteLater();

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

  if(backgroundBrush().color() == m_dotsGridsColor)
    /*
    ** Avoid expensive instructions.
    */

    return;

  painter->setBrushOrigin(0, 0);

  if(m_showCanvasDots)
    {
      QPen pen;

      pen.setBrush(m_dotsGridsColor);
      pen.setWidthF(1.25);
      painter->setPen(pen);

      QVector<QPointF> points;
      auto left = rect.left() - static_cast<int> (rect.left()) % 20;
      auto top = rect.top() - static_cast<int> (rect.top()) % 20;
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

	  for(auto x = start - step; std::isless(x, rect.right()); x += step)
	    painter->drawLine(x, rect.top(), x, rect.bottom());

	  start = round(step, rect.top());

	  if(rect.top() < start)
	    start -= step;

	  for(auto y = start - step; std::isless(y, rect.bottom()); y += step)
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
	  auto view = primaryView();

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
	  else if(text.startsWith("glitch-arduino-catalog-method"))
	    object = new glitch_object_library_function_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-characters"))
	    object = new glitch_object_characters_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-compound operators"))
	    object = new glitch_object_compound_operator_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-constant"))
	    object = new glitch_object_constant_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-conversion"))
	    object = new glitch_object_conversion_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-decorations-"
				  "horizontal arrow"))
	    object = new glitch_object_arrow(text, view);
	  else if(text.startsWith("glitch-arduino-digital i/o-"))
	    object = new glitch_object_digital_io_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-external interrupts") ||
		  text.startsWith("glitch-arduino-interrupts"))
	    object = new glitch_object_interrupts_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-flow control-"))
	    object = new glitch_object_flow_control_arduino(text, view);
	  else if(text.startsWith("glitch-arduino-function"))
	    object = new glitch_object_function_arduino
	      (event->mimeData()->text().
	       mid(static_cast<int> (qstrlen("glitch-arduino-function-"))),
	       view);
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
		    (glitch_undo_command::Types::ITEM_ADDED, proxy, this);

		  undoCommand->setText
		    (tr("item added (%1, %2)").
		     arg(event->scenePos().x()).arg(event->scenePos().y()));
		  m_undoStack->push(undoCommand);
		}
	      else
		addItem(proxy);

	      object->resize(object->sizeHint());
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
    case Qt::Key_Backspace:
    case Qt::Key_Delete:
      {
	if(QGuiApplication::keyboardModifiers() & Qt::AltModifier)
	  deleteItems();

	break;
      }
    case Qt::Key_Down:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
      {
	foreach(auto i, selectedItems())
	  {
	    auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

	    if(!proxy)
	      continue;

	    auto object = qobject_cast<glitch_object *> (proxy->widget());

	    if(!object)
	      continue;

	    if(qobject_cast<QLineEdit *> (object->focusWidget()) &&
	       qobject_cast<QLineEdit *> (object->focusWidget())->
	       isReadOnly() == false)
	      goto done_label;
	    else if(qobject_cast<QPlainTextEdit *> (object->focusWidget()) &&
		    qobject_cast<QPlainTextEdit *> (object->focusWidget())->
		    isReadOnly() == false)
	      goto done_label;
	  }

	auto began = false;
	auto moved = false;
	auto pixels = (QGuiApplication::keyboardModifiers() &
		       Qt::ControlModifier) ? 50 : 1;
	auto updateMode = QGraphicsView::FullViewportUpdate;
	auto view = primaryView();

	if(view)
	  {
	    updateMode = view->viewportUpdateMode();
	    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	  }

	foreach(auto i, selectedItems())
	  {
	    QPoint point;
	    auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

	    if(!proxy)
	      continue;

	    auto object = qobject_cast<glitch_object *> (proxy->widget());

	    if(!object || !proxy->isMovable())
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
		       glitch_undo_command::Types::ITEM_MOVED,
		       proxy,
		       this);

		    m_undoStack->push(undoCommand);
		  }

		if(view)
		  view->ensureVisible(proxy);
	      }
	  }

	if(began && m_undoStack)
	  m_undoStack->endMacro();

	emit sceneResized();

	if(view)
	  view->setViewportUpdateMode(updateMode);

	return;
      }
    case Qt::Key_Z:
      {
	if(QGuiApplication::keyboardModifiers() & Qt::ControlModifier)
	  {
	    if(QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
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

 done_label:
  QGraphicsScene::keyPressEvent(event);
}

void glitch_scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  if(event && !m_lastScenePos.isNull())
    {
      prepareBackgroundForMove(true);

      auto instance = qobject_cast<QGuiApplication *>
	(QApplication::instance());
      auto objects(selectedObjects());

      if(instance &&
	 instance->keyboardModifiers() & Qt::ControlModifier &&
	 objects.size() > 0)
	{
	  /*
	  ** Drag and copy.
	  */

	  if(!property("drag-and-copy").toBool())
	    {
	      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	      QList<QPointF> points;

	      /*
	      ** Copy and deselect the original, selected widgets.
	      */

	      objects = glitch_ui::copySelected
		(primaryView(), &points, true, true);

	      /*
	      ** Paste the newly-copied widgets at the current location.
	      */

	      paste(points, objects, event->scenePos());

	      /*
	      ** Wire the newly-pasted widgets.
	      */

	      QTimer::singleShot(5, this, SIGNAL(wireObjects(void)));

	      /*
	      ** Retrieve the newly-pasted widgets.
	      */

	      objects = selectedObjects();
	      setProperty("drag-and-copy", true);
	      QApplication::restoreOverrideCursor();
	    }
	}

      auto cursorChanged = false;
      auto moved = false;
      auto viewport = primaryView() ? primaryView()->viewport() : nullptr;

      foreach(auto object, objects)
	{
	  if(!object)
	    continue;

	  auto proxy = object->proxy();

	  if(!proxy || !proxy->isMovable())
	    continue;

	  if(object->mouseOverScrollBar(event->scenePos()))
	    continue;
	  else
	    {
	      {
		auto w = qobject_cast<QLineEdit *> (object->focusWidget());

		if(w && w->isReadOnly() == false)
		  continue;
	      }

	      {
		auto w = qobject_cast<QPlainTextEdit *> (object->focusWidget());

		if(w && w->isReadOnly() == false)
		  continue;
	      }
	    }

	  auto point(proxy->mapToParent(event->scenePos() - m_lastScenePos));

	  if(point == proxy->pos() || point.x() < 0 || point.y() < 0)
	    continue;

	  if(!cursorChanged && viewport)
	    {
	      cursorChanged = true;

	      if(property("drag-and-copy").toBool())
		viewport->setCursor(Qt::DragCopyCursor);
	      else
		viewport->setCursor(Qt::DragMoveCursor);
	    }

	  moved = true;
	  proxy->setPos(point);
	  proxy->showResizeHelpers(false);
	}

      m_lastScenePos = event->scenePos();

      if(moved)
	emit sceneResized();
    }

  QGraphicsScene::mouseMoveEvent(event);
}

void glitch_scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  if(m_toolsOperation == glitch_tools::Operations::SELECT)
    {
      m_lastScenePos = QPointF();
      m_movedPoints.clear();
      prepareBackgroundForMove(false);
    }

  if(event)
    {
      auto item = itemAt(event->scenePos(), QTransform());

      if(qgraphicsitem_cast<glitch_resize_widget_rectangle *> (item))
	/*
	** We need to be able to interact with the resize rectangles!
	*/

	goto done_label;

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
		      prepareBackgroundForMove(false);
		    }
		  else
		    {
		      auto instance = qobject_cast<QGuiApplication *>
			(QApplication::instance());

		      if(instance &&
			 instance->keyboardModifiers() & Qt::ControlModifier)
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

		      if(event->button() == Qt::LeftButton)
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
	      parent->setData
		(static_cast<int> (ItemProperties::WasSelected),
		 parent->isSelected());
	      parent->setSelected(true);
	      goto done_label;
	    }

	  if(glitch_variety::keyboardModifiers() & Qt::ControlModifier)
	    m_lastScenePos = event->scenePos();
	  else
	    {
	      if(!parent->isSelected())
		clearSelection();

	      m_lastScenePos = event->scenePos();
	    }

	  parent->setData
	    (static_cast<int> (ItemProperties::WasSelected),
	     parent->isSelected());
	  parent->setSelected(true);

	  if(!m_lastScenePos.isNull())
	    {
	      foreach(auto i, selectedItems())
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
      else
	clearSelection();
    }
  else
    clearSelection();

 done_label:
  QGraphicsScene::mousePressEvent(event);
  emit mousePressed();
}

void glitch_scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  m_lastScenePos = QPointF();
  setProperty("drag-and-copy", false);

  if(glitch_variety::keyboardModifiers() & Qt::ControlModifier)
    {
      auto item = itemAt(event->scenePos(), QTransform());

      if(!qgraphicsitem_cast<glitch_wire *> (item) && item)
	{
	  auto parent = item->parentItem();

	  if(!parent)
	    parent = item;

	  if(!parent)
	    {
	      QGraphicsScene::mouseReleaseEvent(event);
	      return;
	    }

	  m_movedPoints.clear();
	  prepareBackgroundForMove(false);

	  if(parent->data(static_cast<int> (ItemProperties::WasSelected)).
	     toBool())
	    parent->setSelected(!parent->isSelected());

	  QGraphicsScene::mouseReleaseEvent(event);
	  return;
	}
    }

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
	     glitch_undo_command::Types::ITEM_MOVED,
	     movedPoint.second,
	     this);

	  m_undoStack->push(undoCommand);
	  movedPoint.second->showResizeHelpers(true);
	}

      m_movedPoints.clear();

      if(began)
	{
	  m_undoStack->endMacro();
	  emit changed();
	}
    }

  m_movedPoints.clear();
  prepareBackgroundForMove(false);

  auto view = primaryView();

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

void glitch_scene::paste
(const QList<QPointF> &points,
 const QList<glitch_object *> &objects,
 const QPointF &position)
{
  if(objects.isEmpty())
    return;

  auto view = primaryView();

  if(!view)
    return;

  QPoint first;
  auto began = false;
  auto f = false; // First?
  auto point(position);

  if(point.x() < 0)
    point.setX(0);

  if(point.y() < 0)
    point.setY(0);

  for(int i = 0; i < objects.size(); i++)
    {
      auto object = objects.at(i);

      if(!object)
	continue;

      object->setCanvasSettings(canvasSettings());

      if(!(object = object->clone(view)))
	{
	  objects.at(i)->deleteLater();
	  continue;
	}
      else
	{
	  connect(this,
		  SIGNAL(wireObjects(void)),
		  object,
		  SLOT(slotWireObjects(void)),
		  Qt::UniqueConnection);
	  object->setProperty
	    (glitch_object::Properties::POSITION_LOCKED, false);
	  objects.at(i)->deleteLater();
	}

      auto x = points.value(i).x();
      auto y = points.value(i).y();

      if(!f)
	{
	  first = QPoint(x, y);

	  auto proxy = addObject(object);

	  if(proxy)
	    {
	      if(m_undoStack)
		{
		  if(!began)
		    {
		      began = true;
		      m_undoStack->beginMacro
			(tr("item(s) added via drag-and-copy"));
		    }

		  auto undoCommand = new glitch_undo_command
		    (glitch_undo_command::Types::ITEM_ADDED, proxy, this);

		  m_undoStack->push(undoCommand);
		}
	      else
		addItem(proxy);

	      object->afterPaste();
	      proxy->setPos(point);
	      proxy->setSelected(true);
	      m_movedPoints << QPair<QPointF, glitch_proxy_widget *>
		(proxy->scenePos(), proxy);
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

	  auto proxy = addObject(object);

	  if(proxy)
	    {
	      if(m_undoStack)
		{
		  if(!began)
		    {
		      began = true;
		      m_undoStack->beginMacro
			(tr("item(s) added via drag-and-copy"));
		    }

		  auto undoCommand = new glitch_undo_command
		    (glitch_undo_command::Types::ITEM_ADDED, proxy, this);

		  m_undoStack->push(undoCommand);
		}
	      else
		addItem(proxy);

	      object->afterPaste();
	      proxy->setPos(p);
	      proxy->setSelected(true);
	      m_movedPoints << QPair<QPointF, glitch_proxy_widget *>
		(proxy->scenePos(), proxy);
	    }
	  else
	    object->deleteLater();
	}

      f = true;
    }

  if(began && m_undoStack)
    m_undoStack->endMacro();
}

void glitch_scene::prepareBackgroundForMove(const bool state)
{
  Q_UNUSED(state);
}

void glitch_scene::purgeRedoUndoProxies(void)
{
  /*
  ** Delete proxies from the redo / undo stack.
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
	    {
	      qDebug() << "Deleting magic proxy " << it.key() << ".";
	      it.key()->deleteLater();
	    }

	  it.remove();
	}
    }
}

void glitch_scene::recordProxyOrder(glitch_proxy_widget *proxy)
{
  if(!proxy || proxy->isMandatory())
    return;
  else if(proxy->object() &&
	  proxy->object()->objectType().startsWith("decoration"))
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

void glitch_scene::saveWires(const QSqlDatabase &db, QString &error)
{
  QSetIterator<glitch_wire *> it(m_wires);
  QSqlQuery query(db);

  while(it.hasNext())
    {
      auto wire = it.next();

      if(!wire ||
	 !wire->leftProxy() ||
	 !wire->leftProxy()->object() ||
	 !wire->leftProxy()->scene() ||
	 !wire->rightProxy() ||
	 !wire->rightProxy()->object() ||
	 !wire->rightProxy()->scene() ||
	 !wire->scene())
	continue;

      query.prepare
	("INSERT OR REPLACE INTO wires (object_input_oid, object_output_oid) "
	 "VALUES (?, ?)");
      query.addBindValue(wire->rightProxy()->object()->id());
      query.addBindValue(wire->leftProxy()->object()->id());
      query.exec();

      if(error.isEmpty() && query.lastError().isValid())
	error = query.lastError().text();
    }
}

void glitch_scene::setCanvasSettings(glitch_canvas_settings *canvasSettings)
{
  if(!canvasSettings)
    return;

  if(m_canvasSettings)
    disconnect(m_canvasSettings,
	       SIGNAL(accepted(const bool)),
	       this,
	       SLOT(slotCanvasSettingsChanged(const bool)));

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

void glitch_scene::setLoadingFromFile(const bool state)
{
  m_loadingFromFile = state;
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

  foreach(auto object, objects())
    if(object)
      object->setUndoStack(m_undoStack);
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
	  wire->setWireWidth(m_canvasSettings->wireWidth());
	}
    }

  m_dotsGridsColor = m_canvasSettings->dotsGridsColor();
  m_showCanvasDots = m_canvasSettings->showCanvasDots();
  m_showCanvasGrids = !m_showCanvasDots;
  setBackgroundBrush(m_canvasSettings->canvasBackgroundColor());
  update();
  QApplication::restoreOverrideCursor();
}

void glitch_scene::slotDisconnectWireIfNecessary(void)
{
  disconnectWireIfNecessary(qobject_cast<glitch_wire *> (sender()));
}

void glitch_scene::slotFunctionDeleted(const QString &name)
{
  foreach(auto object, objects())
    if(object &&
       object->name() == name &&
       object->objectType() == "arduino-function")
      {
	if(m_undoStack)
	  {
	    auto undoCommand = new glitch_undo_command
	      (glitch_undo_command::Types::ITEM_DELETED, object->proxy(), this);

	    undoCommand->setText(tr("function clone deleted"));
	    m_undoStack->push(undoCommand);
	  }
	else
	  {
	    removeItem(object->proxy());
	    object->deleteLater();
	  }
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

void glitch_scene::slotFunctionReturnPointerChanged(const bool after,
						    const bool before,
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
	(glitch_undo_command::Types::ITEM_DELETED, object->proxy(), this);

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

void glitch_scene::slotProxyGeometryChanged(const QRectF &previousRect)
{
  auto proxy = qobject_cast<glitch_proxy_widget *> (sender());

  if(!proxy || previousRect == proxy->geometry())
    return;

  if(m_undoStack)
    {
      auto undoCommand = new glitch_undo_command
	(proxy->geometry(),
	 previousRect,
	 glitch_undo_command::Types::PROPERTY_CHANGED,
	 glitch_object::Properties::GEOMETRY,
	 proxy->object());

      undoCommand->setText
	(tr("object geometry changed (%1, %2)").
	 arg(proxy->scenePos().x()).arg(proxy->scenePos().y()));
      m_undoStack->push(undoCommand);
    }
}

void glitch_scene::slotSelectedWidgetsAdjustSize(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  auto began = false;

  foreach(auto object, selectedObjects())
    if(object)
      {
	if(!began && m_undoStack)
	  {
	    began = true;
	    m_undoStack->beginMacro(tr("widget(s) size(s) adjusted"));
	  }

	object->slotAdjustSize();
      }

  if(began && m_undoStack)
    m_undoStack->endMacro();

  QApplication::restoreOverrideCursor();
}

void glitch_scene::slotSelectedWidgetsCompress(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  auto began = false;

  foreach(auto object, selectedObjects())
    if(object)
      {
	if(!began && m_undoStack)
	  {
	    began = true;
	    m_undoStack->beginMacro(tr("widget(s) (de)compressed"));
	  }

	object->slotCompress();
      }

  if(began && m_undoStack)
    m_undoStack->endMacro();

  QApplication::restoreOverrideCursor();
}

void glitch_scene::slotSelectedWidgetsDisconnect(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  auto began = false;

  foreach(auto object, selectedObjects())
    if(object && object->proxy())
      {
	if(!began && m_undoStack)
	  {
	    began = true;
	    m_undoStack->beginMacro(tr("widget(s) disconnected"));
	  }

	wireDisconnectObjects(object->proxy());
      }

  if(began && m_undoStack)
    m_undoStack->endMacro();

  QApplication::restoreOverrideCursor();
}

void glitch_scene::slotSelectedWidgetsLock(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  auto began = false;

  foreach(auto object, selectedObjects())
    if(object)
      {
	if(!began && m_undoStack)
	  {
	    began = true;
	    m_undoStack->beginMacro(tr("widget(s) (un)locked"));
	  }

	object->slotLockPosition();
      }

  if(began && m_undoStack)
    m_undoStack->endMacro();

  QApplication::restoreOverrideCursor();
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
	  if(!object->isFullyWired() &&
	     proxy->hoveredSection() == glitch_proxy_widget::Sections::LEFT)
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
	      if(!object->isFullyWired() &&
		 proxy->hoveredSection() == glitch_proxy_widget::Sections::LEFT)
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

      if(!areObjectsWired(object1, object2) &&
	 object1 != object2 &&
	 object1 &&
	 object2)
	{
	  auto wire = new glitch_wire(nullptr);

	  connect(this,
		  SIGNAL(changed(const QList<QRectF> &)),
		  wire,
		  SLOT(slotUpdate(const QList<QRectF> &)));
	  connect(wire,
		  &glitch_wire::disconnectWireIfNecessary,
		  this,
		  &glitch_scene::slotDisconnectWireIfNecessary,
		  Qt::QueuedConnection);

	  if(m_undoStack)
	    {
	      auto undoCommand = new glitch_undo_command
		(glitch_undo_command::Types::WIRE_ADDED, this, wire);

	      undoCommand->setText(tr("objects connected"));
	      m_undoStack->push(undoCommand);
	    }
	  else
	    addItem(wire);

	  object1->setWiredObject(object2, wire);
	  object2->setWiredObject(object1, wire);
	  wire->setBoundingRect(sceneRect());

	  if(m_canvasSettings)
	    {
	      wire->setColor(m_canvasSettings->wireColor());
	      wire->setWireType(m_canvasSettings->wireType());
	      wire->setWireWidth(m_canvasSettings->wireWidth());
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
		(glitch_undo_command::Types::WIRE_DELETED, this, wire);

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

void glitch_scene::wireDisconnectObjects(glitch_proxy_widget *proxy)
{
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

      if(proxy == wire->leftProxy() || proxy == wire->rightProxy())
	{
	  if(m_undoStack)
	    {
	      if(!began)
		{
		  began = true;
		  m_undoStack->beginMacro(tr("widget(s) disconnected"));
		}

	      auto undoCommand = new glitch_undo_command
		(glitch_undo_command::Types::WIRE_DELETED, this, wire);

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
