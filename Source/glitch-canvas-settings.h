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

#ifndef _glitch_canvas_settings_h_
#define _glitch_canvas_settings_h_

#include <QDialog>
#include <QGraphicsView>

#include "ui_glitch-canvas-settings.h"

class glitch_canvas_settings: public QDialog
{
  Q_OBJECT

 public:
  enum Settings
  {
   CANVAS_BACKGROUND_COLOR,
   CANVAS_NAME,
   DOTS_COLOR,
   REDO_UNDO_STACK_SIZE,
   SHOW_CANVAS_DOTS,
   VIEW_UPDATE_MODE
  };

  glitch_canvas_settings(QWidget *parent);
  ~glitch_canvas_settings();
  QColor canvasBackgroundColor(void) const;
  QColor dotsColor(void) const;
  QGraphicsView::ViewportUpdateMode viewportUpdateMode(void) const;
  QHash<glitch_canvas_settings::Settings, QVariant> settings(void) const;
  QString name(void) const;
  bool save(QString &error) const;
  bool showCanvasDots(void) const;
  int redoUndoStackSize(void) const;
  void prepare(void);
  void setFileName(const QString &fileName);
  void setName(const QString &name);
  void setRedoUndoStackSize(const int value);
  void setSettings
    (const QHash<glitch_canvas_settings::Settings, QVariant> &hash);
  void setShowCanvasDots(const bool state);
  void setViewportUpdateMode(const QGraphicsView::ViewportUpdateMode mode);

 private:
  QString m_fileName;
  Ui_glitch_canvas_settings m_ui;
  QString defaultName(void) const;

 private slots:
  void accept(void);
  void slotSelectColor(void);

 signals:
  void accepted(const bool undo);
};

#endif
