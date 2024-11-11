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

#ifndef _glitch_view_h_
#define _glitch_view_h_

#include <QPointer>
#include <QTimer>
#include <QUndoStack>

#include "glitch-canvas-settings.h"
#include "glitch-common.h"
#include "glitch-tools.h"
#include "ui_glitch-view.h"

class QPrinter;
class QSplitter;
class QTextBrowser;
class QTreeWidgetItem;
class glitch_alignment;
class glitch_ash;
class glitch_canvas_preview;
class glitch_documentation;
class glitch_docked_container;
class glitch_find_objects;
class glitch_graphicsview;
class glitch_object;
class glitch_object_start;
class glitch_redo_undo_stack;
class glitch_scene;
class glitch_source_preview;
class glitch_syntax_highlighter;
class glitch_tools;
class glitch_undo_command;
class glitch_user_functions;
class glitch_user_functions_model;
class glitch_wire;

class glitch_view: public QWidget
{
  Q_OBJECT

 public:
  enum class Limits
  {
    OPEN_WIDGETS_PROPERTIES_PROMPT = 10,
    PROPERTIES_MAXIMUM_LENGTH = 50000,
    STYLESHEET_MAXIMUM_LENGTH = 10000
  };

  glitch_view(const QString &fileName,
	      const QString &name,
	      const glitch_common::ProjectTypes projectType,
	      QWidget *parent);
  virtual ~glitch_view();
  QAction *menuAction(void) const;
  QImage snap(void) const;
  QList<QAction *> alignmentActions(void) const;
  QList<QAction *> defaultActions(void) const;
  QList<glitch_object *> allObjects(void) const;
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
  glitch_object *find(const qint64 id) const;
  glitch_scene *scene(void) const;
  glitch_tools::Operations toolsOperation(void) const;
  glitch_user_functions_model *userFunctionsModel(void) const;
  qint64 nextId(void) const;
  qreal scalingFactor(void) const;
  virtual QString projectOutputFileExtension(void) const = 0;
  virtual QString source(void) const;
  virtual QStringList keywords(void) const = 0;
  virtual bool open(const QString &fileName, QString &error);
  virtual void generateSourceClipboard(void) const;
  virtual void generateSourceFile(void) const;
  virtual void openFunction(const QString &name) const = 0;
  virtual void separate(void) = 0;
  virtual void unite(void) = 0;
  virtual void upload(const QStringList &arguments = QStringList()) = 0;
  virtual void verify(const QStringList &arguments = QStringList()) = 0;
  void beginMacro(const QString &text);
  void clearSelection(void);
  void deleteItems(void);
  void endMacro(void);
  void find(void);
  void generateSourceView(const bool raise = true);
  void launchProjectIDE(void) const;
  void populateToolsMenu(QMenu *menu, QWidget *parent);
  void prepareASH(QWidget *parent);
  void print(void);
  void push(glitch_undo_command *undoCommand);
  void redo(void);
  void save(void);
  void selectAll(void);
  void showCanvasSettings(void) const;
  void showRedoUndoStack(void);
  void showSourcePreview(void) const;
  void showTools(void);
  void showUserFunctions(void) const;
  void undo(void);
  void zoom(const int direction);

 public slots:
  void slotAllWidgetsAdjustSize(void);
  void slotPreferencesAccepted(void);
  void slotCopy(void);
  void slotFonts(void);
  void slotPaste(void);
  void slotSave(void);
  void slotSelectedWidgetsProperties(void);
  void slotSelectionChanged(void);
  void slotShowFind(void);
  void slotSimulateDelete(void);
  void slotUnite(void);

 private:
  QList<glitch_wire *> m_delayedWires;
  glitch_redo_undo_stack *m_redoUndoStack;
  glitch_object *find
    (const QList<glitch_object *> &list,
     const qint64 id,
     glitch_object *object) const;
  void createParentFromValues
    (QHash<qint64, char> &ids,
     QHash<qint64, glitch_object *> &parents,
     const QSqlDatabase &db,
     const qint64 oid) const;
  void prepareDatabaseTables(void) const;
  void prepareDefaultActions(void);

 private slots:
  void slotCopiedObjectsChanged(void);
  void slotPrint(QPrinter *printer);
  void slotToolsOperationChanged(const glitch_tools::Operations operation);
  void slotZoomReset(void);

 protected:
  QAction *m_menuAction;
  QHash<glitch_canvas_settings::Settings, QVariant> m_settings;
  QList<QAction *> m_defaultActions;
  QMap<QString, QVariant> m_properties;
  QPointer<QAction> m_pasteAction;
  QPointer<QAction> m_saveDiagramAction;
  QPointer<QMenu> m_contextMenu;
  QPointer<glitch_ash> m_ash;
  QPointer<glitch_documentation> m_sourceView;
  QPointer<glitch_find_objects> m_findObjects;
  QPointer<glitch_scene> m_scene;
  QPointer<glitch_syntax_highlighter> m_sourceViewSyntaxHighlighter;
  QPointer<glitch_tools> m_tools;
  QSplitter *m_bottomSplitter;
  QSplitter *m_rightSplitter;
  QSplitter *m_splitter;
  QString m_fileName;
  QTextBrowser *m_ideOutput;
  QTimer m_generateSourceViewTimer;
  QTimer m_generateTimer;
  QTimer m_saveTimer;
  QUndoStack *m_undoStack;
  Ui_glitch_view m_ui;
  glitch_alignment *m_alignment;
  glitch_canvas_preview *m_canvasPreview;
  glitch_canvas_settings *m_canvasSettings;
  glitch_common::ProjectTypes m_projectType;
  glitch_docked_container *m_dockedWidgetPropertyEditors;
  glitch_graphicsview *m_view;
  glitch_source_preview *m_sourcePreview;
  glitch_user_functions *m_userFunctions;
  glitch_user_functions_model *m_userFunctionsModel;
  bool saveImplementation(const QString &fileName, QString &error);
  bool saveProperties(void) const;
  virtual void generateSource(QTextStream &stream) const;
  void adjustScrollBars(void);
  void contextMenuEvent(QContextMenuEvent *event);
  void createTools(void);
  void reparent(void);
  void resizeEvent(QResizeEvent *event);
  void setSceneRect(const QSize &size);

 protected slots:
  virtual void slotCanvasSettingsChanged(const bool undo);
  virtual void slotProcessCommand
    (const QString &command, const QStringList &arguments);
  void slotChanged(void);
  void slotCustomContextMenuRequested(const QPoint &point);
  void slotDockPropertyEditor(QWidget *widget);
  void slotFunctionAdded(const QString &name, const bool isClone);
  void slotFunctionDeleted(const QString &name);
  void slotFunctionNameChanged(const QString &after,
			       const QString &before,
			       glitch_object *object);
  void slotFunctionReturnPointerChanged(const bool after,
					const bool before,
					glitch_object *object);
  void slotFunctionReturnTypeChanged(const QString &after,
				     const QString &before,
				     glitch_object *object);
  void slotGenerate(void);
  void slotGenerateSourceView(void);
  void slotProcessCommand(const QString &command);
  void slotResizeScene(void);
  void slotSaveAs(void);
  void slotSceneObjectDestroyed(QObject *object);
  void slotSceneResized(void);
  void slotSeparate(void);
  void slotShowCanvasSettings(void);
  void slotShowTools(void);
  void slotShowUserFunctions(void) const;
  void slotShowWires(void);
  void slotUndoStackChanged(void);

 signals:
  void canvasNameChanged(const QString &name);
  void changed(void);
  void copy(QGraphicsView *view);
  void information(const QString &text);
  void mouseEnterEvent(void);
  void mouseLeaveEvent(void);
  void paste(glitch_view *view);
  void preferencesAccepted(void);
  void saveSignal(void);
  void saved(void);
  void selectionChanged(void);
  void separate(glitch_view *view);
  void toolsOperationChanged(const glitch_tools::Operations operation);
  void unite(glitch_view *view);
  void zoomReset(void);
};

#endif
