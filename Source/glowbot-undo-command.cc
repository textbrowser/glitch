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

#include <QtDebug>

#include "glowbot-object.h"
#include "glowbot-proxy-widget.h"
#include "glowbot-scene.h"
#include "glowbot-undo-command.h"

glowbot_undo_command::glowbot_undo_command
(const QHash<glowbot_canvas_settings::Settings,
             QVariant> &previousCanvasSettings,
 const Types type,
 glowbot_canvas_settings *canvasSettings,
 QUndoCommand *parent):QUndoCommand(parent)
{
  if(canvasSettings)
    m_currentCanvasSettings = canvasSettings->settings();

  m_canvasSettings = canvasSettings;
  m_previousCanvasSettings = previousCanvasSettings;
  m_type = type;
}

glowbot_undo_command::glowbot_undo_command
(const QPointF &previousPosition,
 const Types type,
 glowbot_proxy_widget *proxy,
 glowbot_scene *scene,
 QUndoCommand *parent):QUndoCommand(parent)
{
  if(proxy)
    m_currentPosition = proxy->scenePos();

  m_previousPosition = previousPosition;
  m_proxy = proxy;
  m_scene = scene;
  m_type = type;
}

glowbot_undo_command::glowbot_undo_command
(const QString &previousFunctionName,
 const Types type,
 glowbot_object *object,
 QUndoCommand *parent):QUndoCommand(parent)
{
  m_object = object;
  m_previousFunctionName = previousFunctionName;
  m_type = type;
}

glowbot_undo_command::glowbot_undo_command
(const Types type,
 glowbot_proxy_widget *proxy,
 glowbot_scene *scene,
 QUndoCommand *parent):QUndoCommand(parent)
{
  if(proxy)
    switch(type)
      {
      default:
	{
	  break;
	}
      }

  m_proxy = proxy;
  m_scene = scene;
  m_type = type;
}

glowbot_undo_command::~glowbot_undo_command()
{
}

void glowbot_undo_command::redo(void)
{
  switch(m_type)
    {
    case CANVAS_SETTINGS_CHANGED:
      {
	if(m_canvasSettings)
	  m_canvasSettings->setSettings(m_currentCanvasSettings);

	break;
      }
    case ITEM_ADDED:
      {
	if(m_proxy && m_scene)
	  {
	    m_proxy->setParent(m_scene);
	    m_scene->addItem(m_proxy);
	    m_scene->update();
	  }

	break;
      }
    case ITEM_DELETED:
      {
	if(m_proxy && m_scene)
	  {
	    m_scene->removeItem(m_proxy);
	    m_scene->update();
	  }

	break;
      }
    case ITEM_MOVED:
      {
	if(m_proxy && m_scene)
	  {
	    m_proxy->setPos(m_currentPosition);
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

void glowbot_undo_command::undo(void)
{
  switch(m_type)
    {
    case CANVAS_SETTINGS_CHANGED:
      {
	if(m_canvasSettings)
	  m_canvasSettings->setSettings(m_previousCanvasSettings);

	break;
      }
    case ITEM_ADDED:
      {
	if(m_proxy && m_scene)
	  {
	    m_scene->removeItem(m_proxy);
	    m_scene->update();
	  }

	break;
      }
    case ITEM_DELETED:
      {
	if(m_proxy && m_scene)
	  {
	    m_scene->addItem(m_proxy);
	    m_scene->update();
	  }

	break;
      }
    case ITEM_MOVED:
      {
	if(m_proxy && m_scene)
	  {
	    m_proxy->setPos(m_previousPosition);
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
