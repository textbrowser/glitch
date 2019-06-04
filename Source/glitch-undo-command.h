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

#ifndef _glitch_undo_command_h_
#define _glitch_undo_command_h_

#include <QPointer>
#include <QUndoCommand>

#include "glitch-canvas-settings.h"

class glitch_object;
class glitch_proxy_widget;
class glitch_scene;

class glitch_undo_command: public QUndoCommand
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

  glitch_undo_command(const QHash<glitch_canvas_settings::Settings,
		                   QVariant> &previousCanvasSettings,
		       const Types type,
		       glitch_canvas_settings *canvasSettings,
		       QUndoCommand *parent = nullptr);
  glitch_undo_command(const QPointF &previousPosition,
		       const Types type,
		       glitch_proxy_widget *proxy,
		       glitch_scene *scene,
		       QUndoCommand *parent = nullptr);
  glitch_undo_command(const QString &previousFunctionName,
		       const Types type,
		       glitch_object *object,
		       QUndoCommand *parent = nullptr);
  glitch_undo_command(const Types type,
		       glitch_proxy_widget *proxy,
		       glitch_scene *scene,
		       QUndoCommand *parent = nullptr);
  ~glitch_undo_command();
  void redo(void);
  void undo(void);

 private:
  QHash<glitch_canvas_settings::Settings, QVariant> m_currentCanvasSettings;
  QHash<glitch_canvas_settings::Settings, QVariant> m_previousCanvasSettings;
  QPointF m_currentPosition;
  QPointF m_previousPosition;
  QPointer<glitch_canvas_settings> m_canvasSettings;
  QPointer<glitch_object> m_object;
  QPointer<glitch_proxy_widget> m_proxy;
  QPointer<glitch_scene> m_scene;
  QString m_currentFunctionName;
  QString m_currentString;
  QString m_previousFunctionName;
  Types m_type;
};

#endif
