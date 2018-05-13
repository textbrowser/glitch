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

#ifndef _glowbot_view_h_
#define _glowbot_view_h_

#include <QGraphicsView>
#include <QWidget>

#include "glowbot-canvas-settings.h"
#include "glowbot-common.h"
#include "ui_glowbot-view.h"

class glowbot_alignment;
class glowbot_object;
class glowbot_object_start;
class glowbot_scene;
class glowbot_user_functions;

class glowbot_view: public QWidget
{
  Q_OBJECT

 public:
  glowbot_view(const QString &name,
	       const glowbot_common::ProjectType projectType,
	       QWidget *parent);
  virtual ~glowbot_view();
  QAction *menuAction(void) const;
  QString name(void) const;
  bool hasChanged(void) const;
  bool save(QString &error);
  bool saveAs(const QString &fileName, QString &error);
  glowbot_common::ProjectType projectType(void) const;
  glowbot_scene *scene(void) const;
  quint64 nextId(void) const;
  virtual bool open(const QString &fileName, QString &error);
  void deleteItems(void);
  void save(void);
  void selectAll(void);
  void showAlignment(void);

 private:
  void prepareDatabaseTables(const QString &fileName) const;

 protected:
  QAction *m_menuAction;
  QGraphicsView *m_view;
  QString m_fileName;
  QString m_name;
  Ui_glowbot_view m_ui;
  bool m_changed;
  glowbot_alignment *m_alignment;
  glowbot_canvas_settings *m_canvasSettings;
  glowbot_common::ProjectType m_projectType;
  glowbot_object_start *m_startObject;
  glowbot_scene *m_scene;
  glowbot_user_functions *m_userFunctions;
  bool saveImplementation(const QString &fileName, QString &error);
  void contextMenuEvent(QContextMenuEvent *event);
  void resizeEvent(QResizeEvent *event);
  void setSceneRect(const QSize &size);

 protected slots:
  void slotCanvasSettingsChanged(void);
  void slotChanged(void);
  void slotCustomContextMenuRequested(const QPoint &point);
  void slotFunctionAdded(const QString &name);
  void slotFunctionNameChanged(const QString &before, const QString &after);
  void slotSave(void);
  void slotSaveAs(void);
  void slotSceneObjectDestroyed(QObject *object);
  void slotSceneResized(void);
  void slotSeparate(void);
  void slotShowAlignmentTool(void);
  void slotShowCanvasSettings(void);
  void slotShowUserFunctions(void);
  void slotUnite(void);

 signals:
  void changed(void);
  void saved(void);
  void separate(glowbot_view *view);
  void showStructures(void);
  void unite(glowbot_view *view);
};

#endif
