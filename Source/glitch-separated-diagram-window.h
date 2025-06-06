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

#ifndef _glitch_separated_diagram_window_h_
#define _glitch_separated_diagram_window_h_

#include <QPointer>
#include <QTimer>

#include "glitch-tools.h"
#include "ui_glitch-separated-diagram-window.h"

class QProgressBar;
class glitch_view;

class glitch_separated_diagram_window: public QMainWindow
{
  Q_OBJECT

 public:
  glitch_separated_diagram_window(QWidget *parent);
  ~glitch_separated_diagram_window();
  QPointer<glitch_view> view(void) const;
  void setCentralWidget(QWidget *widget);

 public slots:
  void slotPreferencesAccepted(void);

 private:
  QPointer<glitch_view> m_view;
  QProgressBar *m_ideProcessIndicator;
  QTimer m_statusBarTimer;
  Ui_glitch_separated_diagram_window m_ui;
  void closeEvent(QCloseEvent *event);
  void prepareActionWidgets(void);
  void prepareIcons(void);
  void prepareRedoUndoActions(void);
  void prepareToolBar(void);

 private slots:
  void slotCopy(void);
  void slotDelete(void);
  void slotFind(void);
  void slotGenerateSource(void);
  void slotGenerateSourceClipboard(void);
  void slotGenerateSourceView(void);
  void slotHideTearOffMenu(void);
  void slotIDEProcessFinished(void);
  void slotIDEProcessStarted(void);
  void slotPageChanged(void);
  void slotPageSaved(void);
  void slotPaste(void);
  void slotPrint(void);
  void slotRedo(void);
  void slotSaveDiagram(void);
  void slotSelectAll(void);
  void slotSelectionChanged(void);
  void slotShowContextMenu(void);
  void slotShowFullScreenMode(void);
  void slotShowTearOffMenu(void);
  void slotSpecialTools(void);
  void slotStatusBarTimerTimeout(void);
  void slotTerminate(void);
  void slotToolsOperationChanged(const glitch_tools::Operations operation);
  void slotUndo(void);
  void slotUpload(void);
  void slotVerify(void);
  void slotZoom(void);

 signals:
  void copy(glitch_view *view);
  void paste(glitch_view *view);
  void unite(void);
};

#endif
