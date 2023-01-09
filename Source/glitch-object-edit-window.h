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

#ifndef _glitch_object_edit_window_h_
#define _glitch_object_edit_window_h_

#include <QMainWindow>
#include <QMap>
#include <QPointer>

#include "glitch-common.h"
#include "glitch-object.h"

class QLineEdit;
class QSplitter;
class QUndoStack;
class glitch_object_view;
class glitch_structures_arduino;

class glitch_object_edit_window: public QMainWindow
{
  Q_OBJECT

 public:
  glitch_object_edit_window(const glitch_common::ProjectTypes projectType,
			    glitch_object *object,
			    QWidget *parent);
  ~glitch_object_edit_window();
  QWidget *centralWidget(void) const;
  void prepareHeader(const QString &text);
  void prepareToolBar(const QList<QAction *> &actions);
  void setCentralWidget(QWidget *widget);
  void setEditView(glitch_object_view *view);
  void setToolBarVisible(const bool state);
  void setUndoStack(QUndoStack *undoStack);

 private:
  QLineEdit *m_header;
  QMap<QString, QAction *> m_actions;
  QPointer<QUndoStack> m_undoStack;
  QPointer<glitch_object> m_object;
  QPointer<glitch_object_view> m_editView;
  QPointer<glitch_structures_arduino> m_arduinoStructures;
  QSplitter *m_splitter;
  QToolBar *m_toolBar;
  glitch_common::ProjectTypes m_projectType;
  bool event(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void prepareIcons(void);
  void resizeEvent(QResizeEvent *event);
  void showEvent(QShowEvent *event);

 private slots:
  void slotAboutToShowEditMenu(void);
  void slotShowFullScreenMode(void);
  void slotSplitterMoved(void);
  void slotViewTools(void);

 signals:
  void closed(void);
  void copy(void);
  void deleteSignal(void);
  void paste(void);
  void propertyChanged(const QString &property, const QVariant &value);
  void redo(void);
  void saveSignal(void);
  void selectAll(void);
  void undo(void);
};

#endif
