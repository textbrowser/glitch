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

#ifndef _glowbot_undo_command_h_
#define _glowbot_undo_command_h_

#include <QPointer>
#include <QUndoCommand>

#include "glowbot-canvas-settings.h"

class glowbot_object;
class glowbot_proxy_widget;
class glowbot_scene;

class glowbot_undo_command: public QUndoCommand
{
 public:
  enum Types
  {
    CANVAS_SETTINGS_CHANGED = 0,
    FUNCTION_RENAMED,
    ITEM_ADDED,
    ITEM_DELETED,
    ITEM_MOVED,
    ITEM_RENAMED,
    STYLESHEET_CHANGED
  };

  glowbot_undo_command(const QHash<glowbot_canvas_settings::Settings,
		                   QVariant> &previousCanvasSettings,
		       const Types type,
		       glowbot_canvas_settings *canvasSettings,
		       QUndoCommand *parent = nullptr);
  glowbot_undo_command(const QPointF &previousPosition,
		       const Types type,
		       glowbot_proxy_widget *proxy,
		       glowbot_scene *scene,
		       QUndoCommand *parent = nullptr);
  glowbot_undo_command(const QString &previousFunctionName,
		       const Types type,
		       glowbot_object *object,
		       QUndoCommand *parent = nullptr);
  glowbot_undo_command(const Types type,
		       glowbot_proxy_widget *proxy,
		       glowbot_scene *scene,
		       QUndoCommand *parent = nullptr);
  ~glowbot_undo_command();
  void redo(void);
  void undo(void);

 private:
  QHash<glowbot_canvas_settings::Settings, QVariant> m_currentCanvasSettings;
  QHash<glowbot_canvas_settings::Settings, QVariant> m_previousCanvasSettings;
  QPointF m_currentPosition;
  QPointF m_previousPosition;
  QPointer<glowbot_canvas_settings> m_canvasSettings;
  QPointer<glowbot_object> m_object;
  QPointer<glowbot_proxy_widget> m_proxy;
  QPointer<glowbot_scene> m_scene;
  QString m_currentFunctionName;
  QString m_currentString;
  QString m_previousFunctionName;
  Types m_type;
};

#endif
