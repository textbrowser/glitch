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

#ifndef _glitch_ui_h_
#define _glitch_ui_h_

#include <QMessageBox>
#include <QPointer>
#include <QTimer>

#include "glitch-tools.h"
#include "ui_glitch-mainwindow.h"

class QGraphicsView;
class QShortcut;
class QUndoStack;
class glitch_documentation;
class glitch_object;
class glitch_preferences;
class glitch_separated_diagram_window;
class glitch_view;
class glitch_view_arduino;
class swifty;

class glitch_ui: public QMainWindow
{
  Q_OBJECT

 public:
  enum Limits
  {
    FILE_NAME_MAXIMUM_LENGTH = 2500,
    NAME_MAXIMUM_LENGTH = 100,
    TYPE_MAXIMUM_LENGTH = 100
  };

  glitch_ui(void);
  ~glitch_ui();
  static QFont s_defaultApplicationFont;
  static QMultiMap<QPair<int, int>, QPointer<glitch_object> >
     copiedObjects(void);
  static QSet<glitch_object *> s_copiedObjectsSet;
  static QTranslator *s_translator1;
  static QTranslator *s_translator2;
  static void copy(QGraphicsView *view, const bool selected = true);
  static void copy(glitch_object *object);
  static void paste(QGraphicsView *view, QUndoStack *undoStack);

 private:
  QList<QShortcut *> m_tabWidgetShortcuts;
  QMessageBox m_about;
  QPointer<glitch_separated_diagram_window> m_separateWindow;
  QPointer<glitch_view> m_currentView;
  QString m_recentFilesFileName;
  QStringList m_delayedDiagrams; // Open after launch.
  QTimer m_statusBarTimer;
  Ui_glitch_mainwindow m_ui;
  glitch_documentation *m_arduino;
  glitch_documentation *m_releaseNotes;
  glitch_preferences *m_preferences;
  static QMultiMap<QPair<int, int>, QPointer<glitch_object> > s_copiedObjects;
  swifty *m_swifty;
  bool openDiagram(const QString &fileName, QString &error);
  glitch_view *page(const int index);
  glitch_view_arduino *newArduinoDiagram(const QString &fileName,
					 const QString &name,
					 const bool fromFile);
  static void clearCopiedObjects(void);
  void closeEvent(QCloseEvent *event);
  void parseCommandLineArguments(void);
  void prepareActionWidgets(void);
  void prepareFonts(void);
  void prepareIcons(void);
  void prepareRecentFiles(void);
  void prepareRedoUndoActions(void);
  void prepareStatusBar(void);
  void prepareTabShortcuts(void);
  void prepareToolBars(void);
  void restoreSettings(void);
  void saveRecentFile(const QString &fileName);
  void saveSettings(void);
  void setTabText(glitch_view *view);
  void setWindowTitle(glitch_view *view);
  void showStatsuBarMessage(const QString &text, const int timeout = 0);

 private slots:
  void slotAbout(void);
  void slotAboutToShowTabsMenu(void);
  void slotAdjustSizesTool(void);
  void slotArduinoViewDestroyed(void);
  void slotClearCopiedWidgetsBuffer(void);
  void slotClearRecentFiles(void);
  void slotCloseDiagram(int index);
  void slotCloseDiagram(void);
  void slotCompressWidgetsTool(void);
  void slotCopy(QGraphicsView *view);
  void slotCopy(glitch_view *view);
  void slotCopy(void);
  void slotDelayedOpenDiagrams(void);
  void slotDelayedToolBarPreparation(void);
  void slotDelete(void);
  void slotFind(void);
  void slotForgetRecentDiagram(void);
  void slotGenerateSource(void);
  void slotGenerateSourceView(void);
  void slotNewArduinoDiagram(void);
  void slotOpenDiagram(void);
  void slotOpenRecentDiagram(void);
  void slotPageChanged(void);
  void slotPageSaved(void);
  void slotPageSelected(int index);
  void slotPaste(glitch_view *view);
  void slotPaste(void);
  void slotPreferencesAccepted(void);
  void slotQuit(void);
  void slotRedo(void);
  void slotSaveCurrentDiagram(void);
  void slotSaveCurrentDiagramAs(void);
  void slotSelectAll(void);
  void slotSelectPage(void);
  void slotSelectionChanged(void);
  void slotSeparate(QWidget *widget);
  void slotSeparate(glitch_view *view);
  void slotShowAllStructures(void);
  void slotShowAllTools(void);
  void slotShowArduinoDocumentation(void);
  void slotShowCanvasSettings(void);
  void slotShowDiagramContextMenu(void);
  void slotShowFullScreenMode(void);
  void slotShowPreferences(void);
  void slotShowProjectIDE(void);
  void slotShowReleaseNotes(void);
  void slotShowStructures(void);
  void slotShowTools(void);
  void slotShowUserFunctions(void);
  void slotStatusBarTimerTimeout(void);
  void slotSwifty(void);
  void slotTabMoved(int from, int to);
  void slotTabWidgetShortcutActivated(void);
  void slotToolsOperationChanged(const glitch_tools::Operations operation);
  void slotUndo(void);
  void slotUnite(glitch_view *view);
  void slotViewToolBars(void);

 public slots:
  void show(void);
};

#endif
