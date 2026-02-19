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

#ifndef _glitch_ui_h_
#define _glitch_ui_h_

#include <QMessageBox>
#include <QPointer>
#include <QTimer>

#include "glitch-aware-containers.h"
#include "glitch-separated-diagram-window.h"
#include "glitch-tools.h"
#include "ui_glitch-mainwindow.h"

class QGraphicsView;
class QProgressBar;
class QShortcut;
class QUndoStack;
class glitch_documentation;
class glitch_object;
class glitch_preferences;
class glitch_recent_diagrams_view;
class glitch_view;
class glitch_view_arduino;
class swifty;

typedef QVector<QPair<QImage, QString> > QVectorQPairQImageQString;

class glitch_ui: public QMainWindow
{
  Q_OBJECT

 public:
  enum class Limits
  {
    FILE_NAME_MAXIMUM_LENGTH = 2500,
    NAME_MAXIMUM_LENGTH = 100,
    TYPE_MAXIMUM_LENGTH = 100
  };

  glitch_ui(void);
  ~glitch_ui();

  static QPointer<QMainWindow> s_mainWindow;
  static QSet<glitch_object *> s_copiedObjectsSet;
  static QTranslator *s_translator1;
  static QTranslator *s_translator2;
  static glitch_aware_multi_map<QPair<int, int>, QPointer<glitch_object> >
    s_copiedObjects;

  static QFont defaultApplicationFont(void)
  {
    return s_defaultApplicationFont;
  }

  static QList<glitch_object *> copySelected
    (QGraphicsView *view,
     QList<QPointF> *points,
     const bool deselectOriginal = false,
     const bool selected = true);
  static void copy(QGraphicsView *view, const bool selected = true);
  static void copy(glitch_object *object);
  static void paste(QGraphicsView *view, QUndoStack *undoStack);

  static void setDefaultApplicationFont(const QFont &font)
  {
    s_defaultApplicationFont = font;
  }

 private:
  QHash<QObject *, QPointer<glitch_separated_diagram_window> >
    m_separatedWindows;
  QList<QShortcut *> m_tabWidgetShortcuts;
  QMessageBox m_about;
  QPointer<QAction> m_recentDiagramHoveredAction;
  QPointer<glitch_separated_diagram_window> m_separatedWindow;
  QPointer<glitch_view> m_currentView;
  QProgressBar *m_ideProcessIndicator;
  QString m_recentFilesFileName;
  QStringList m_delayedDiagrams; // Open after launch.
  QTimer m_statusBarTimer;
  Ui_glitch_mainwindow m_ui;
  bool m_generateSource;
  glitch_documentation *m_arduino;
  glitch_documentation *m_releaseNotes;
  glitch_preferences *m_preferences;
  glitch_recent_diagrams_view *m_recentDiagramsView;
  static QFont s_defaultApplicationFont;
  swifty *m_swifty;
  QString about(void) const;
  bool openDiagram(const QString &fileName, QString &error);
  glitch_view *page(const int index);
  glitch_view_arduino *newArduinoDiagram(const QString &fileName,
					 const QString &name,
					 const bool fromFile);
  static void clearCopiedObjects(void);
  void closeEvent(QCloseEvent *event);
#ifdef Q_OS_ANDROID
  void copyExamplesForAndroid(void);
#endif
  void parseCommandLineArguments(void);
  void prepareActionWidgets(void);
  void prepareFonts(void);
  void prepareIcons(void);
  void prepareRecentDiagramsView(void);
  void prepareRecentFiles(void);
  void prepareRedoUndoActions(void);
  void prepareStatusBar(void);
  void prepareTab(void);
  void prepareTabShortcuts(void);
  void prepareToolBars(void);
  void restoreSettings(void);
  void saveRecentFile(const QString &fileName);
  void saveSettings(void);
  void setTabText(glitch_view *view);
  void setWindowTitle(glitch_view *view);
  void showStatusBarMessage(const QString &text, const int timeout = 0);

 private slots:
  void slotAbout(void);
  void slotAboutToShowProjectMenu(void);
  void slotAboutToShowRecentDiagrams(void);
  void slotAboutToShowTabsMenu(void);
  void slotArduinoViewDestroyed(void);
  void slotClearCopiedWidgetsBuffer(void);
  void slotClearRecentFiles(void);
  void slotCloseAllDiagrams(void);
  void slotCloseDiagram(int index);
  void slotCloseDiagram(void);
  void slotCopiedObjectsChanged(void);
  void slotCopy(QGraphicsView *view);
  void slotCopy(glitch_view *view);
  void slotCopy(void);
  void slotDelayedOpenDiagrams(void);
  void slotDelayedToolBarPreparation(void);
  void slotDelete(void);
  void slotFind(void);
  void slotForgetRecentDiagram(const QString &fileName, bool *ok = nullptr);
  void slotForgetRecentDiagram(void);
  void slotGenerateSource(void);
  void slotGenerateSourceClipboard(void);
  void slotGenerateSourceView(void);
  void slotHideTearOffMenu(void);
  void slotIDEProcessFinished(void);
  void slotIDEProcessStarted(void);
  void slotIDETerminate(void);
  void slotIDEUpload(void);
  void slotIDEVerify(void);
  void slotNewArduinoDiagram(void);
  void slotOpenDiagram(const QString &fileName);
  void slotOpenDiagram(void);
  void slotOpenRecentDiagram(void);
  void slotPageChanged(void);
  void slotPageSaved(void);
  void slotPageSelected(int index);
  void slotPaste(glitch_view *view);
  void slotPaste(void);
  void slotPreferencesAccepted(void);
  void slotPrepareStatusBar(void);
  void slotPrint(void);
  void slotProcessCommand(const QString &command);
  void slotQuit(void);
  void slotRecentDiagramHovered(QAction *action);
  void slotRedo(void);
  void slotSaveAsPNG(void);
  void slotSaveCurrentDiagram(void);
  void slotSaveCurrentDiagramAs(void);
  void slotSelectAll(void);
  void slotSelectPage(void);
  void slotSelectionChanged(void);
  void slotSeparate(QWidget *widget);
  void slotSeparate(glitch_view *view);
  void slotSeparate(void);
  void slotSeparatedWindowDestroyed(QObject *object);
  void slotShowAllTools(void);
  void slotShowArduinoDocumentation(void);
  void slotShowCanvasSettings(void);
  void slotShowDiagramContextMenu(void);
  void slotShowFullScreenMode(void);
  void slotShowPreferences(void);
  void slotShowProjectIDE(void);
  void slotShowRedoUndoStack(void);
  void slotShowReleaseNotes(void);
  void slotShowSerialPortWindow(void);
  void slotShowTearOffMenu(void);
  void slotShowTools(void);
  void slotShowUserFunctions(void);
  void slotSpecialTools(void);
  void slotStatusBarTimerTimeout(void);
  void slotSwifty(void);
  void slotTabMoved(int from, int to);
  void slotTabWidgetShortcutActivated(void);
  void slotToolsOperationChanged(const glitch_tools::Operations operation);
  void slotUndo(void);
  void slotUnite(glitch_view *view);
  void slotUniteAllDiagrams(void);
  void slotViewTabBar(void);
  void slotViewToolBars(void);
  void slotZoom(void);

 public slots:
  void show(void);

 signals:
  void information(const QString &text);
};

#endif
