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

#ifndef _glowbot_ui_h_
#define _glowbot_ui_h_

#include <QMainWindow>

#include "ui_glowbot-mainwindow.h"

class glowbot_structures_arduino;
class glowbot_view;

class glowbot_ui: public QMainWindow
{
  Q_OBJECT

 public:
  glowbot_ui(void);
  ~glowbot_ui();

 private:
  Ui_glowbot_mainwindow m_ui;
  glowbot_structures_arduino *m_arduinoStructures;
  bool openDiagram(const QString &fileName, QString &error);
  glowbot_view *newArduinoDiagram(const QString &name, const bool fromFile);
  glowbot_view *page(const int index);
  void closeEvent(QCloseEvent *event);
  void parseCommandLineArguments(void);
  void prepareActionWidgets(void);
  void restoreSettings(void);
  void saveSettings(void);
  void setTabText(glowbot_view *page);
  void setWindowTitle(glowbot_view *page);

 private slots:
  void slotAboutToShowTabsMenu(void);
  void slotCloseDiagram(int index);
  void slotCloseDiagram(void);
  void slotDelete(void);
  void slotNewArduinoDiagram(void);
  void slotOpenDiagram(void);
  void slotPageChanged(void);
  void slotPageSelected(int index);
  void slotQuit(void);
  void slotSaveCurrentDiagram(void);
  void slotSaveCurrentDiagramAs(void);
  void slotSelectAll(void);
  void slotSelectPage(void);
  void slotSeparate(glowbot_view *view);
  void slotShowAlignment(void);
  void slotShowStructures(void);
  void slotTabMoved(int from, int to);
  void slotUnite(glowbot_view *view);

 public slots:
  void show(void);
};

#endif
