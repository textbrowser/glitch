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

#ifndef _glitch_view_h_
#define _glitch_view_h_

#include <QPointer>
#include <QTimer>
#include <QUndoStack>

#include "glitch-canvas-settings.h"
#include "glitch-common.h"
#include "glitch-tools.h"
#include "ui_glitch-view.h"

class glitch_alignment;
class glitch_graphicsview;
class glitch_object;
class glitch_object_start;
class glitch_scene;
class glitch_tools;
class glitch_undo_command;
class glitch_user_functions;

class glitch_view: public QWidget
{
  Q_OBJECT

 public:
  enum Limits
  {
    PROPERTIES_MAXIMUM_LENGTH = 50000,
    STYLESHEET_MAXIMUM_LENGTH = 10000
  };

  glitch_view(const QString &fileName,
	      const QString &name,
	      const glitch_common::ProjectTypes projectType,
	      QWidget *parent);
  virtual ~glitch_view();
  QAction *menuAction(void) const;
  QList<QAction *> alignmentActions(void) const;
  QList<QAction *> defaultActions(void) const;
  QList<glitch_object *> objects(void) const;
  QList<glitch_object *> selectedObjects(void) const;
  QMenu *defaultContextMenu(void);
  QString name(void) const;
  QString redoText(void) const;
  QString undoText(void) const;
  QUndoStack *undoStack(void) const;
  bool canRedo(void) const;
  bool canUndo(void) const;
  bool containsFunction(const QString &name) const;
  bool hasChanged(void) const;
  bool save(QString &error);
  bool saveAs(const QString &fileName, QString &error);
  glitch_common::ProjectTypes projectType(void) const;
  glitch_graphicsview *view(void) const;
  glitch_scene *scene(void) const;
  glitch_tools::Operations toolsOperation(void) const;
  qint64 nextId(void) const;
  virtual QString projectOutputFileExtension(void) const = 0;
  virtual bool open(const QString &fileName, QString &error);
  virtual void generateSource(void);
  virtual void separate(void) = 0;
  virtual void showStructures(void) = 0;
  virtual void unite(void) = 0;
  void beginMacro(const QString &text);
  void deleteItems(void);
  void endMacro(void);
  void push(glitch_undo_command *undoCommand);
  void redo(void);
  void save(void);
  void selectAll(void);
  void setTabButton(QPushButton *pushButton);
  void showCanvasSettings(void);
  void showTools(void);
  void showUserFunctions(void);
  void undo(void);

 public slots:
  void slotCopy(void);
  void slotSave(void);

 private:
  void createParentFromValues
    (QHash<qint64, char> &ids,
     QHash<qint64, glitch_object *> &parents,
     const QSqlDatabase &db,
     const qint64 oid) const;
  void prepareDatabaseTables(const QString &fileName) const;
  void prepareDefaultActions(void);

 private slots:
  void slotToolsOperationChanged(const glitch_tools::Operations operation);

 protected:
  QAction *m_menuAction;
  QHash<glitch_canvas_settings::Settings, QVariant> m_settings;
  QList<QAction *> m_defaultActions;
  QList<QPointer<QUndoStack> > m_undoStacks;
  QPointer<QAction> m_saveDiagramAction;
  QPointer<QMenu> m_contextMenu;
  QPointer<QPushButton> m_tabButton;
  QPointer<glitch_tools> m_tools;
  QString m_fileName;
  QTimer m_generateTimer;
  QUndoStack *m_undoStack;
  Ui_glitch_view m_ui;
  bool m_changed;
  glitch_alignment *m_alignment;
  glitch_canvas_settings *m_canvasSettings;
  glitch_common::ProjectTypes m_projectType;
  glitch_graphicsview *m_view;
  glitch_scene *m_scene;
  glitch_user_functions *m_userFunctions;
  bool saveImplementation(const QString &fileName, QString &error);
  void adjustScrollBars(void);
  void contextMenuEvent(QContextMenuEvent *event);
  void resizeEvent(QResizeEvent *event);
  void setSceneRect(const QSize &size);

 protected slots:
  virtual void slotShowStructures(void) = 0;
  void slotCanvasSettingsChanged(const bool undo);
  void slotChanged(void);
  void slotCustomContextMenuRequested(const QPoint &point);
  void slotGenerate(void);
  void slotFunctionAdded(const QString &name, const bool isClone);
  void slotFunctionDeleted(const QString &name);
  void slotFunctionNameChanged(const QString &after,
			       const QString &before,
			       glitch_object *object);
  void slotFunctionReturnTypeChanged(const QString &after,
				     const QString &before,
				     glitch_object *object);
  void slotPaste(void);
  void slotResizeScene(void);
  void slotSaveAs(void);
  void slotSceneObjectDestroyed(QObject *object);
  void slotSceneResized(void);
  void slotSeparate(void);
  void slotShowCanvasSettings(void);
  void slotShowTools(void);
  void slotShowUserFunctions(void);
  void slotUndoStackChanged(void);
  void slotUndoStackCreated(QUndoStack *undoStack);
  void slotUnite(void);

 signals:
  void changed(void);
  void copy(QGraphicsView *view);
  void mouseEnterEvent(void);
  void mouseLeaveEvent(void);
  void saved(void);
  void selectionChanged(void);
  void separate(glitch_view *view);
  void toolsOperationChanged(const glitch_tools::Operations operation);
  void unite(glitch_view *view);
};

#endif
