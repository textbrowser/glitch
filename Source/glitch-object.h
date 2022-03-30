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

#ifndef _glitch_object_h_
#define _glitch_object_h_

#include <QMenu>
#include <QPointer>
#include <QSqlDatabase>
#include <QWidget>

#include "glitch-proxy-widget.h"

class QUndoStack;
class glitch_floating_context_menu;
class glitch_object_edit_window;
class glitch_object_view;
class glitch_view;

class glitch_object: public QWidget
{
  Q_OBJECT

 public:
  enum DefaultMenuActions
  {
   DELETE = 0,
   EDIT,
   LOCK_POSITION,
   SET_FUNCTION_NAME,
   SET_FUNCTION_PARAMETERS,
   SET_STYLE_SHEET
  };

  enum Limits
  {
   NAME_MAXIMUM_LENGTH = 64 // An estimate. May be longer or shorter.
  };

  enum Properties
  {
   COMMENT = 0,
   CONSTANT_TYPE,
   LOGICAL_OPERATOR,
   NAME,
   POSITION_LOCKED,
   XYZ_PROPERTY
  };

  glitch_object(QWidget *parent);
  glitch_object(const quint64 id, QWidget *parent);
  QPointF scenePos(void) const;
  QPointer<glitch_object_view> editView(void) const;
  QPointer<glitch_proxy_widget> proxy(void) const;
  QString type(void) const;
  bool mouseOverScrollBar(const QPointF &point) const;
  bool positionLocked(void) const;
  quint64 id(void) const;
  static glitch_object *createFromValues
    (const QMap<QString, QVariant> &values,
     glitch_object *parentObject,
     QString &error,
     QWidget *parent);
  virtual QString name(void) const;
  virtual bool hasOutput(void) const;
  virtual bool hasView(void) const;
  virtual bool isMandatory(void) const;
  virtual glitch_object *clone(QWidget *parent) const = 0;
  virtual void addActions(QMenu &menu) = 0;
  virtual void closeEditWindow(void);
  virtual void save(const QSqlDatabase &db, QString &error);
  virtual void setName(const QString &n);
  virtual void setProperty(const Properties property, const QVariant &value);
  virtual void setUndoStack(QUndoStack *undoStack);
  virtual ~glitch_object();
  void addChild(const QPointF &point, glitch_object *object);
  void move(const QPoint &point);
  void move(int x, int y);
  void setProxy(const QPointer<glitch_proxy_widget> &proxy);
  virtual void simulateDelete(void);

 private:
  static quint64 s_id;

 private slots:
  void slotLockPosition(void);
  void slotShowContextMenu(void);

 protected:
  QHash<Properties, QVariant> m_properties;
  QMap<DefaultMenuActions, QAction *> m_actions;
  QPointer<QUndoStack> m_undoStack;
  QPointer<QWidget> m_parent;
  QPointer<glitch_floating_context_menu> m_contextMenu;
  QPointer<glitch_object_edit_window> m_editWindow;
  QPointer<glitch_object_view> m_editView;
  QPointer<glitch_proxy_widget> m_proxy;
  QString m_type;
  bool m_drawInputConnector;
  bool m_drawOutputConnector;
  quint64 m_id;
  virtual void createActions(void);
  void addDefaultActions(QMenu &menu);
  void prepareContextMenu(void);
  void prepareEditSignals(const glitch_view *parentView);
  void saveProperties(const QMap<QString, QVariant> &p,
		      const QSqlDatabase &db,
		      QString &error);
  void setProperties(const QStringList &list);

 protected slots:
  void slotSetStyleSheet(void);

 signals:
  void changed(void);
  void deletedViaContextMenu(void);
};

#endif
