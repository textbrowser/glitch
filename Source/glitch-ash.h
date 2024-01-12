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

#ifndef _glitch_ash_h_
#define _glitch_ash_h_

#include <QTextEdit>

class glitch_ash_textedit: public QTextEdit
{
  Q_OBJECT

 public:
  glitch_ash_textedit(QWidget *parent):QTextEdit(parent)
  {
    auto font(this->font());

    font.setFamily("Courier");
    font.setPointSizeF(11.5);
    setCurrentFont(font);
    m_historyIndex = 0;
    m_promptBlockNumber = 0;
    m_promptLength = 4;
    displayPrompt();
  }

  void setCommands(const QMultiMap<QString, QString> &commands)
  {
    m_commands = commands;
  }

 private:
  QMultiMap<QString, QString> m_commands;
  QStringList m_history;
  int m_historyIndex;
  int m_promptBlockNumber;
  int m_promptLength;
  QString currentCommand(void) const;
  bool handleBackspaceKey(void) const;
  void handleDownKey(void);
  void handleUpKey(void);
  void displayPrompt(void);
  void handleInterrupt(void);
  void handleReturnKey(void);
  void handleTabKey(void);
  void keyPressEvent(QKeyEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void replaceCurrentCommand(const QString &command);
  void showEvent(QShowEvent *event);

 signals:
  void processCommand(const QString &command);
};

#include "ui_glitch-ash.h"

class glitch_ash: public QDialog
{
  Q_OBJECT

 public:
  glitch_ash(QWidget *parent);
  ~glitch_ash();

  QFrame *frame(void) const
  {
    return m_ui.frame;
  }

 private:
  QMultiMap<QString, QString> m_commands;
  Ui_glitch_ash m_ui;

 private slots:
  void slotCanvasNameChanged(const QString &name);
  void slotCommandProcessed(const QString &results);
  void slotInformationReceived(const QString &text);
  void slotProcessCommand(const QString &command);

 signals:
  void processCommand(const QString &command);
};

#endif
