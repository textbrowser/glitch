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

#include "Arduino/glitch-object-constant-arduino.h"
#include "Arduino/glitch-object-flow-control-arduino.h"
#include "Arduino/glitch-object-function-arduino.h"
#include "glitch-object.h"
#include "glitch-proxy-widget.h"
#include "glitch-scene.h"
#include "glitch-undo-command.h"
#include "glitch-user-functions.h"
#include "glitch-wire.h"

glitch_undo_command::glitch_undo_command
(const QHash<glitch_canvas_settings::Settings,
             QVariant> &previousCanvasSettings,
 const Types type,
 glitch_canvas_settings *canvasSettings,
 QUndoCommand *parent):QUndoCommand(parent)
{
  m_canvasSettings = canvasSettings;
  m_currentCanvasSettings = canvasSettings ?
    canvasSettings->settings() :
    QHash<glitch_canvas_settings::Settings, QVariant> ();
  m_currentFunctionReturnPointer = false;
  m_previousCanvasSettings = previousCanvasSettings;
  m_previousFunctionReturnPointer = false;
  m_property = glitch_object::Properties::Z_Z_Z_PROPERTY;
  m_type = type;
}

glitch_undo_command::glitch_undo_command
(const QPointF &previousPosition,
 const Types type,
 glitch_proxy_widget *proxy,
 glitch_scene *scene,
 QUndoCommand *parent):QUndoCommand(parent)
{
  m_currentFunctionReturnPointer = false;
  m_currentPosition = proxy ? proxy->scenePos() : QPointF();
  m_previousFunctionReturnPointer = false;
  m_previousPosition = previousPosition;
  m_property = glitch_object::Properties::Z_Z_Z_PROPERTY;
  m_proxy = proxy;
  m_scene = scene;
  m_type = type;
}

glitch_undo_command::glitch_undo_command
(const QString &previousFunctionValue,
 const Types type,
 glitch_object *object,
 glitch_user_functions *userFunctions,
 QUndoCommand *parent):QUndoCommand(parent)
{
  m_currentFunctionReturnPointer = false;
  m_object = object;
  m_previousFunctionReturnPointer = false;

  switch(type)
    {
    case Types::FUNCTION_RENAMED:
      {
	if(m_object)
	  m_currentFunctionName = object->name();

	m_previousFunctionName = previousFunctionValue;
	break;
      }
    case Types::FUNCTION_RETURN_POINTER_CHANGED:
      {
	if(qobject_cast<glitch_object_function_arduino *> (m_object))
	  m_currentFunctionReturnPointer =
	    qobject_cast<glitch_object_function_arduino *> (m_object)->
	    isPointer();

	m_previousFunctionReturnPointer =
	  QVariant(previousFunctionValue).toBool();
	break;
      }
    case Types::FUNCTION_RETURN_TYPE_CHANGED:
      {
	if(qobject_cast<glitch_object_function_arduino *> (m_object))
	  m_currentFunctionReturnType =
	    qobject_cast<glitch_object_function_arduino *> (m_object)->
	    returnType();

	m_previousFunctionReturnType = previousFunctionValue;
	break;
      }
    default:
      {
	break;
      }
    }

  m_property = glitch_object::Properties::Z_Z_Z_PROPERTY;
  m_type = type;
  m_userFunctions = userFunctions;
}

glitch_undo_command::glitch_undo_command
(const QString &previousString,
 const Types type,
 glitch_object *object,
 QUndoCommand *parent):QUndoCommand(parent)
{
  if(object)
    switch(type)
      {
      case Types::CONSTANT_TYPE_CHANGED:
	{
	  if(qobject_cast<glitch_object_constant_arduino *> (object))
	    m_currentString = qobject_cast<glitch_object_constant_arduino *>
	      (object)->constantType();

	  break;
	}
      case Types::FLOW_CONTROL_TYPE_CHANGED:
	{
	  if(qobject_cast<glitch_object_flow_control_arduino *> (object))
	    m_currentString = qobject_cast<glitch_object_flow_control_arduino *>
	      (object)->flowControlType();

	  break;
	}
      case Types::STYLESHEET_CHANGED:
	{
	  m_currentString = object->styleSheet();
	  break;
	}
      default:
	{
	  break;
	}
      }

  m_currentFunctionReturnPointer = false;
  m_object = object;
  m_previousFunctionReturnPointer = false;
  m_previousString = previousString;
  m_property = glitch_object::Properties::Z_Z_Z_PROPERTY;
  m_type = type;
}

glitch_undo_command::glitch_undo_command
(const QVariant &currentProperty,
 const QVariant &previousProperty,
 const Types type,
 const glitch_object::Properties property,
 glitch_object *object,
 QUndoCommand *parent):QUndoCommand(parent)
{
  m_currentFunctionReturnPointer = false;
  m_currentProperty = currentProperty;
  m_object = object;
  m_previousFunctionReturnPointer = false;
  m_previousProperty = previousProperty;
  m_property = property;
  m_type = type;
}

glitch_undo_command::glitch_undo_command
(const Types type,
 glitch_proxy_widget *proxy,
 glitch_scene *scene,
 QUndoCommand *parent):QUndoCommand(parent)
{
  m_currentFunctionReturnPointer = false;
  m_previousFunctionReturnPointer = false;
  m_property = glitch_object::Properties::Z_Z_Z_PROPERTY;
  m_proxy = proxy;
  m_scene = scene;
  m_type = type;
}

glitch_undo_command::glitch_undo_command
(const Types type,
 glitch_scene *scene,
 glitch_wire *wire,
 QUndoCommand *parent):QUndoCommand(parent)
{
  m_currentFunctionReturnPointer = false;
  m_previousFunctionReturnPointer = false;
  m_property = glitch_object::Properties::Z_Z_Z_PROPERTY;
  m_scene = scene;
  m_type = type;
  m_wire = wire;
}

glitch_undo_command::~glitch_undo_command()
{
}

void glitch_undo_command::redo(void)
{
  switch(m_type)
    {
    case Types::CANVAS_SETTINGS_CHANGED:
      {
	if(m_canvasSettings)
	  m_canvasSettings->setSettings(m_currentCanvasSettings);

	break;
      }
    case Types::CONSTANT_TYPE_CHANGED:
      {
	if(qobject_cast<glitch_object_constant_arduino *> (m_object))
	  qobject_cast<glitch_object_constant_arduino *>
	    (m_object)->setConstantType(m_currentString);

	break;
      }
    case Types::FLOW_CONTROL_TYPE_CHANGED:
      {
	if(qobject_cast<glitch_object_flow_control_arduino *> (m_object))
	  qobject_cast<glitch_object_flow_control_arduino *>
	    (m_object)->setFlowControlType(m_currentString);

	break;
      }
    case Types::FUNCTION_RENAMED:
      {
	if(m_object && m_userFunctions)
	  {
	    m_object->setName(m_currentFunctionName);
	    m_userFunctions->addFunction(m_currentFunctionName);
	    m_userFunctions->deleteFunction(m_previousFunctionName);
	  }

	break;
      }
    case Types::FUNCTION_RETURN_POINTER_CHANGED:
      {
	if(qobject_cast<glitch_object_function_arduino *> (m_object))
	  qobject_cast<glitch_object_function_arduino *> (m_object)->
	    setIsPointer(m_currentFunctionReturnPointer);

	break;
      }
    case Types::FUNCTION_RETURN_TYPE_CHANGED:
      {
	if(qobject_cast<glitch_object_function_arduino *> (m_object))
	  qobject_cast<glitch_object_function_arduino *> (m_object)->
	    setReturnType(m_currentFunctionReturnType);

	break;
      }
    case Types::ITEM_ADDED:
      {
	if(m_proxy && m_scene)
	  {
	    m_scene->addItem(m_proxy);
	    m_scene->update();
	  }

	break;
      }
    case Types::ITEM_DELETED:
      {
	if(m_proxy && m_scene)
	  {
	    auto object = qobject_cast<glitch_object *> (m_proxy->widget());

	    if(object)
	      object->simulateDelete();

	    m_scene->removeItem(m_proxy);
	    m_scene->update();
	  }

	break;
      }
    case Types::ITEM_MOVED:
      {
	if(m_proxy && m_scene)
	  {
	    m_proxy->setPos(m_currentPosition);
	    m_scene->update();
	  }

	break;
      }
    case Types::PROPERTY_CHANGED:
      {
	if(m_object)
	  m_object->setProperty(m_property, m_currentProperty);

	break;
      }
    case Types::STYLESHEET_CHANGED:
      {
	if(m_object)
	  m_object->setStyleSheet(m_currentString);

	break;
      }
    case Types::WIRE_ADDED:
      {
	if(m_scene && m_wire)
	  {
	    m_scene->addItem(m_wire);
	    m_scene->update();
	  }

	break;
      }
    case Types::WIRE_DELETED:
      {
	if(m_scene && m_wire)
	  {
	    m_scene->removeItem(m_wire);
	    m_scene->update();
	  }

	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_undo_command::undo(void)
{
  switch(m_type)
    {
    case Types::CANVAS_SETTINGS_CHANGED:
      {
	if(m_canvasSettings)
	  m_canvasSettings->setSettings(m_previousCanvasSettings);

	break;
      }
    case Types::CONSTANT_TYPE_CHANGED:
      {
      	if(qobject_cast<glitch_object_constant_arduino *> (m_object))
	  qobject_cast<glitch_object_constant_arduino *>
	    (m_object)->setConstantType(m_previousString);

	break;
      }
    case Types::FLOW_CONTROL_TYPE_CHANGED:
      {
	if(qobject_cast<glitch_object_flow_control_arduino *> (m_object))
	  qobject_cast<glitch_object_flow_control_arduino *>
	    (m_object)->setFlowControlType(m_previousString);

	break;
      }
    case Types::FUNCTION_RENAMED:
      {
	if(m_object && m_userFunctions)
	  {
	    m_object->setName(m_previousFunctionName);
	    m_userFunctions->addFunction(m_previousFunctionName);
	    m_userFunctions->deleteFunction(m_currentFunctionName);
	  }

	break;
      }
    case Types::FUNCTION_RETURN_POINTER_CHANGED:
      {
	if(qobject_cast<glitch_object_function_arduino *> (m_object))
	  qobject_cast<glitch_object_function_arduino *> (m_object)->
	    setIsPointer(m_previousFunctionReturnPointer);

	break;
      }
    case Types::FUNCTION_RETURN_TYPE_CHANGED:
      {
	if(qobject_cast<glitch_object_function_arduino *> (m_object))
	  qobject_cast<glitch_object_function_arduino *> (m_object)->
	    setReturnType(m_previousFunctionReturnType);

	break;
      }
    case Types::ITEM_ADDED:
      {
	if(m_proxy && m_scene)
	  {
	    auto object = qobject_cast<glitch_object *> (m_proxy->widget());

	    if(object)
	      object->simulateDelete();

	    m_scene->removeItem(m_proxy);
	    m_scene->update();
	  }

	break;
      }
    case Types::ITEM_DELETED:
      {
	if(m_proxy && m_scene)
	  {
	    m_proxy->setSelected(false);
	    m_scene->addItem(m_proxy);
	    m_scene->update();
	  }

	break;
      }
    case Types::ITEM_MOVED:
      {
	if(m_proxy && m_scene)
	  {
	    m_proxy->setPos(m_previousPosition);
	    m_scene->update();
	  }

	break;
      }
    case Types::PROPERTY_CHANGED:
      {
	if(m_object)
	  m_object->setProperty(m_property, m_previousProperty);

	break;
      }
    case Types::STYLESHEET_CHANGED:
      {
	if(m_object)
	  m_object->setStyleSheet(m_previousString);

	break;
      }
    case Types::WIRE_ADDED:
      {
	if(m_scene && m_wire)
	  {
	    m_scene->removeItem(m_wire);
	    m_scene->update();
	  }

	break;
      }
    case Types::WIRE_DELETED:
      {
	if(m_scene && m_wire)
	  {
	    if(m_wire->leftProxy() &&
	       m_wire->leftProxy()->object() &&
	       m_wire->rightProxy() &&
	       m_wire->rightProxy()->object())
	      {
		m_wire->leftProxy()->object()->setWiredObject
		  (m_wire->rightProxy()->object(), m_wire);
		m_wire->rightProxy()->object()->setWiredObject
		  (m_wire->leftProxy()->object(), m_wire);
	      }

	    m_scene->addItem(m_wire);
	    m_scene->update();
	  }

	break;
      }
    default:
      {
	break;
      }
    }
}
