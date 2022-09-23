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
class glitch_scene;
class glitch_view;

class glitch_object: public QWidget
{
  Q_OBJECT

 public:
  enum DefaultMenuActions
  {
    ADJUST_SIZE = 0,
    DELETE,
    EDIT,
    LOCK_POSITION,
    SET_FUNCTION_NAME,
    SET_STYLE_SHEET,
    SHOW_CONTEXT_MENU_BUTTON
  };

  enum Limits
  {
    NAME_MAXIMUM_LENGTH = 64 // An estimate. May be longer or shorter.
  };

  enum Properties
  {
    /*
    ** Arduino Properties
    */

    ADVANCED_IO_TYPE = 0,
    ANALOG_IO_TYPE,
    ARITHMETIC_OPERATOR,
    BITS_AND_BYTES_TYPE,
    BITWISE_OPERATOR,
    CHARACTERS_TYPE,
    COMMENT,
    COMPOUND_OPERATOR,
    COMPOUND_OPERATOR_PRE,
    CONDITION,
    CONSTANT_OTHER,
    CONSTANT_TYPE,
    CONTEXT_MENU_BUTTON_SHOWN,
    CONVERSION,
    DIGITAL_IO_TYPE,
    FLOW_CONTROL_TYPE,
    LOGICAL_OPERATOR,
    MATHEMATICS_TYPE,
    NAME,
    POSITION_LOCKED,
    RANDOM_TYPE,
    SERIAL_TYPE,
    SIZE,
    STREAM_TYPE,
    SYNTAX,
    TIME_TYPE,
    TOOL_BAR_VISIBLE,
    TRIGONOMETRY_TYPE,
    UTILITIES_TYPE,
    VARIABLE_ARRAY,
    VARIABLE_NAME,
    VARIABLE_POINTER_ACCESS,
    VARIABLE_PROGMEM,
    VARIABLE_QUALIFIER,
    VARIABLE_TYPE,
    XYZ_PROPERTY
  };

  glitch_object(QWidget *parent);
  glitch_object(const qint64 id, QWidget *parent);
  QPointF scenePos(void) const;
  QPointer<glitch_object_view> editView(void) const;
  QPointer<glitch_proxy_widget> proxy(void) const;
  QString type(void) const;
  bool mouseOverScrollBar(const QPointF &point) const;
  bool positionLocked(void) const;

  bool isInputWired(void) const
  {
    return !inputs().isEmpty();
  }

  bool isOutputWired(void) const
  {
    return !outputs().isEmpty();
  }

  bool isWired(void) const
  {
    return isInputWired() || isOutputWired();
  }

  bool showOrderIndicator(void) const
  {
    return m_canvasSettings && m_canvasSettings->showOrderIndicators();
  }

  qint64 id(void) const;

  glitch_floating_context_menu *contextMenu(void) const
  {
    return m_contextMenu;
  }

  glitch_scene *editScene(void) const;
  glitch_scene *scene(void) const;
  static glitch_object *createFromValues
    (const QMap<QString, QVariant> &values,
     glitch_object *parentObject,
     QString &error,
     QWidget *parent);
  virtual QString code(void) const = 0;
  virtual QString name(void) const;
  virtual bool canResize(void) const;
  virtual bool hasInput(void) const;
  virtual bool hasOutput(void) const;
  virtual bool hasView(void) const;
  virtual bool isFullyWired(void) const = 0; // Are all inputs wired?
  virtual bool isMandatory(void) const;
  virtual bool shouldPrint(void) const = 0;
  virtual glitch_object *clone(QWidget *parent) const = 0;
  virtual void addActions(QMenu &menu) = 0;
  virtual void hideOrShowOccupied(void);
  virtual void save(const QSqlDatabase &db, QString &error);
  virtual void setName(const QString &n);
  virtual void setProperty(const Properties property, const QVariant &value);
  virtual void setUndoStack(QUndoStack *undoStack);
  virtual void simulateDelete(void);
  virtual ~glitch_object();
  void addChild(const QPointF &point, glitch_object *object);
  void afterPaste(void);
  void move(const QPoint &point);
  void move(int x, int y);
  void setCanvasSettings(glitch_canvas_settings *canvasSettings);
  void setProperties(const QStringList &list);
  void setProperty(const char *name, const QVariant &value);
  void setProxy(const QPointer<glitch_proxy_widget> &proxy);
  void setWiredObject(glitch_object *object, glitch_wire *wire);

 private:
  QMenu m_menu;
  static qint64 s_id;

 private slots:
  void slotAdjustSize(void);
  void slotSimulateDelete(void);
  void slotLockPosition(void);
  void slotPropertyChanged(const QString &property, const QVariant &value);
  void slotShowContextMenu(void);
  void slotShowContextMenuButton(void);
  void slotWireDestroyed(void);

 protected:
  QHash<Properties, QVariant> m_properties;
  QHash<qint64, QPointer<glitch_wire> > m_wires; /*
						 ** Must be cloned, if
						 ** required!
						 */
  QList<QPointer<glitch_object> > m_copiedChildren;
  QMap<DefaultMenuActions, QAction *> m_actions;
  QPointer<QUndoStack> m_undoStack;
  QPointer<QWidget> m_parent;
  QPointer<glitch_canvas_settings> m_canvasSettings;
  QPointer<glitch_object_edit_window> m_editWindow;
  QPointer<glitch_object_view> m_editView;
  QPointer<glitch_proxy_widget> m_proxy;
  QString m_type;
  bool m_drawInputConnector;
  bool m_drawOutputConnector;
  glitch_floating_context_menu *m_contextMenu;
  qint64 m_id;
  static QRegularExpression s_splitRegularExpression;
  QToolButton *contextMenuButton(void) const;
  glitch_view *findNearestGlitchView(QWidget *widget) const;
  virtual QStringList inputs(void) const;
  virtual QStringList outputs(void) const;
  virtual void createActions(void);
  void addDefaultActions(QMenu &menu);
  void cloneWires(const QHash<qint64, QPointer<glitch_wire> > &wires);
  void prepareContextMenu(void);
  void prepareEditSignals(const glitch_view *parentView);
  void saveProperties(const QMap<QString, QVariant> &p,
		      const QSqlDatabase &db,
		      QString &error);
  void saveWires(const QSqlDatabase &db, QString &error);

 protected slots:
  void slotSetStyleSheet(void);
  void slotUndoStackCreated(void);

 signals:
  void changed(void);
  void deletedViaContextMenu(void);
  void simulateDeleteSignal(void);
  void undoStackCreated(QUndoStack *undoStack);
};

#endif
