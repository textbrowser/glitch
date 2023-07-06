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

#ifndef _glitch_object_h_
#define _glitch_object_h_

#include <QMenu>
#include <QPointer>
#include <QSqlDatabase>
#include <QWidget>
#include <QtDebug>

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
  enum class DefaultMenuActions
  {
    /*
    ** Geometry.
    */

    ADJUST_SIZE = 0,
    COMPRESS_WIDGET,
    LOCK_POSITION,

    /*
    ** Style.
    */

    BACKGROUND_COLOR = 1000,
    BORDER_COLOR,
    FONT,
    FONT_COLOR,
    PORT_COLORS,
    SET_STYLE_SHEET,
    TRANSPARENT,

    /*
    ** Editing.
    */

    COPY = 2000,
    DELETE,
    EDIT,
    FLOATING_CONTEXT_MENU,

    /*
    ** Miscellaneous.
    */

    SET_FUNCTION_NAME = 3000,
    SET_FUNCTION_RETURN_TYPE = 4000
  };

  enum class Limits
  {
    ARDUINO_LOOP_ID = 1,
    ARDUINO_SETUP_ID = 2,
    MINIMUM_ID = 5, // Must be greater than ARDUINO_SETUP_ID.
    NAME_MAXIMUM_LENGTH = 128 // An estimate. May be longer or shorter.
  };

  enum class PortColors
  {
    INPUT_CONNECTED = 0,
    INPUT_DISCONNECTED,
    OUTPUT_CONNECTED,
    OUTPUT_DISCONNECTED
  };

  enum class Properties
  {
    ADVANCED_IO_TYPE = 0,
    ANALOG_IO_TYPE,
    ARITHMETIC_OPERATOR,
    BACKGROUND_COLOR,
    BITS_AND_BYTES_TYPE,
    BITWISE_OPERATOR,
    BOOLEAN_OPERATOR,
    BORDER_COLOR,
    CHARACTERS_TYPE,
    COMMENT,
    COMPOUND_OPERATOR,
    COMPOUND_OPERATOR_PRE,
    COMPRESSED_WIDGET,
    CONDITION,
    CONSTANT_OTHER,
    CONSTANT_TYPE,
    CONVERSION,
    DIGITAL_IO_TYPE,
    EDIT_WINDOW_GEOMETRY,
    EDIT_WINDOW_STATE,
    FLOW_CONTROL_TYPE,
    FONT,
    FONT_COLOR,
    GEOMETRY,
    INTERRUPTS_TYPE,
    LOGICAL_OPERATOR,
    MATHEMATICS_TYPE,
    NAME,
    PORT_COLORS,
    POSITION_LOCKED,
    RANDOM_TYPE,
    SERIAL_TYPE,
    SIZE,
    STREAM_TYPE,
    STRUCTURES_VIEW_SPLITTER_STATE,
    SYNTAX,
    TIME_TYPE,
    TOOL_BAR_VISIBLE,
    TRANSPARENT,
    TRIGONOMETRY_TYPE,
    UTILITIES_TYPE,
    VARIABLE_ARRAY,
    VARIABLE_NAME,
    VARIABLE_POINTER_ACCESS,
    VARIABLE_PROGMEM,
    VARIABLE_QUALIFIER,
    VARIABLE_TYPE,
    Z_VALUE,
    Z_Z_Z_PROPERTY
  };

  glitch_object(QWidget *parent);
  glitch_object(const QString &type, const qint64 id, QWidget *parent);
  glitch_object(const qint64 id, QWidget *parent);

  QColor borderColor(void) const
  {
    return QColor(m_properties.value(Properties::BORDER_COLOR).toString());
  }

  QColor portColor(const glitch_object::PortColors portColor) const
  {
    return QColor
      (m_properties.value(Properties::PORT_COLORS).
       toString().split('-').value(static_cast<int> (portColor)).remove('&'));
  }

  QHash<Properties, QVariant> properties(void) const
  {
    return m_properties;
  }

  QList<glitch_object *> allObjects(void) const;
  QList<glitch_object *> objects(void) const;

  QPainterPath path(void) const
  {
    return m_path;
  }

  QPointF scenePos(void) const;

  QPointer<glitch_canvas_settings> canvasSettings(void) const
  {
    return m_canvasSettings;
  }

  QPointer<glitch_floating_context_menu> menu(void) const
  {
    return m_contextMenu;
  }

  QPointer<glitch_object_view> editView(void) const;
  QPointer<glitch_proxy_widget> proxy(void) const;
  QString objectType(void) const;

  QString position(void) const
  {
    return QString("(%1, %2)").arg(pos().x()).arg(pos().y());
  }

  QVariant property(const Properties property) const
  {
    return m_properties.value(property);
  }

  QVariant property(const char *property) const
  {
    return QObject::property(property);
  }

  bool isInputWired(void) const
  {
    return !inputs().isEmpty();
  }

  bool isOccupied(void) const
  {
    return m_occupied;
  }

  bool isOutputWired(void) const
  {
    return !outputs().isEmpty();
  }

  bool isWired(void) const
  {
    return isInputWired() || isOutputWired();
  }

  bool mouseOverScrollBar(const QPointF &point) const;
  bool positionLocked(void) const;

  bool showOrderIndicator(void) const
  {
    if(m_type.startsWith("decoration"))
      return false;
    else
      return m_canvasSettings && m_canvasSettings->showOrderIndicators();
  }

  glitch_scene *editScene(void) const;
  glitch_scene *scene(void) const;
  qint64 id(void) const;
  static glitch_object *createFromValues
    (const QMap<QString, QVariant> &values,
     glitch_object *parentObject,
     QString &error,
     QWidget *parent);

  virtual QSize sizeHint(void) const
  {
    return QWidget::sizeHint();
  }

  virtual QString code(void) const = 0;
  virtual QString name(void) const;
  virtual QStringList parameters(void) const;
  virtual bool canResize(void) const;
  virtual bool hasInput(void) const;
  virtual bool hasOutput(void) const;
  virtual bool hasView(void) const;
  virtual bool isFullyWired(void) const = 0; // Are all inputs wired?
  virtual bool isMandatory(void) const;
  virtual bool shouldPrint(void) const = 0;
  virtual glitch_object *clone(QWidget *parent) const = 0;
  virtual void addActions(QMenu &menu) = 0;
  virtual void compressWidget(const bool state);
  virtual void hideOrShowOccupied(void);
  virtual void save(const QSqlDatabase &db, QString &error);
  virtual void setName(const QString &n);
  virtual void setProperty(const Properties property, const QVariant &value);
  virtual void simulateDelete(void);
  virtual ~glitch_object();
  void addChild(const QPointF &point, glitch_object *object, bool &ok);
  void afterPaste(void);
  void move(const QPoint &point);
  void move(int x, int y);
  void separate(void);
  void setCanvasSettings(glitch_canvas_settings *canvasSettings);

  void setOriginalPosition(const QPointF &point)
  {
    m_originalPosition = point;
  }

  void setProperties(const QStringList &list);
  void setProperty(const char *name, const QVariant &value);
  void setProxy(const QPointer<glitch_proxy_widget> &proxy);

  void setSizeBeforeFontChange(const QSize &size)
  {
    m_sizeBeforeFontChange = size;
  }

  void setUndoStack(QUndoStack *undoStack);
  void setWiredObject(glitch_object *object, glitch_wire *wire);
  void showEditWindow(void) const;

  void triggerAction(const DefaultMenuActions action)
  {
    /*
    ** Be careful calling this method within a beginMacro() and
    ** endMacro() block for QueuedConnection connections.
    */

    if(!m_actions.value(action, nullptr))
      /*
      ** Let's adapt!
      */

      createActions();

    if(m_actions.value(action, nullptr) &&
       m_actions.value(action)->isEnabled())
      m_actions.value(action)->trigger();
  }

 public slots:
  virtual void slotAdjustSize(void);
  void slotCompress(void);
  void slotLockPosition(void);
  void slotPropertyChanged(const QString &property, const QVariant &value);
  void slotShowContextMenu(void);

 private:
  QMenu m_menu;
  QSize m_sizeBeforeFontChange;
  static qint64 s_id;
  bool event(QEvent *event);
  void allObjectsImplementation(QList<glitch_object *> &list) const;
  void mouseDoubleClickEvent(QMouseEvent *event);

 private slots:
  void slotActionTriggered(void);
  void slotCanvasSettingsChanged(const bool state);
  void slotClearTemporaryContainers(void);
  void slotSetFont(void);
  void slotSimulateDelete(void);
  void slotPropertyChanged
    (const glitch_object::Properties property, const QVariant &value);
  void slotWireDestroyed(void);
  void slotWireObjects(void);

 protected:
  QHash<Properties, QVariant> m_properties;
  QHash<qint64, QPointer<glitch_wire> > m_wires; /*
						 ** Must be cloned, if
						 ** required!
						 */
  QList<QPair<QPointF, QPointF> > m_copiedConnectionsPositions; // Cloning.
  QList<QPointer<glitch_object> > m_copiedChildren; // Cloning.
  QMap<DefaultMenuActions, QAction *> m_actions;
  QPainterPath m_path;
  QPointF m_originalPosition; // Wiring pasted objects.
  QPointer<QUndoStack> m_undoStack;
  QPointer<QWidget> m_parent;
  QPointer<glitch_canvas_settings> m_canvasSettings;
  QPointer<glitch_floating_context_menu> m_contextMenu;
  QPointer<glitch_object_edit_window> m_editWindow;
  QPointer<glitch_object_view> m_editView;
  QPointer<glitch_proxy_widget> m_proxy;
  QString m_type;
  bool m_drawInputConnector;
  bool m_drawOutputConnector;
  bool m_occupied;
  qint64 m_id;
  static QRegularExpression s_splitRegularExpression;
  static int s_widthTweak;
  QToolButton *contextMenuButton(void) const;
  glitch_view *findNearestGlitchView(QWidget *widget) const;
  int minimumHeight(const int height) const;

  virtual QSize preferredSize(void) const
  {
    return minimumSizeHint();
  }

  virtual QStringList inputs(void) const;
  virtual QStringList outputs(void) const;
  virtual bool isActionAllowed(const DefaultMenuActions action) const;

  virtual bool isNativelyDrawn(void) const
  {
    return false;
  }

  void addDefaultActions(QMenu &menu);
  void cloneWires(const QHash<qint64, QPointer<glitch_wire> > &wires);
  void cloneWires(const QList<QPair<QPointF, QPointF> > &list);
  void createActions(void);
  void prepareContextMenu(void);
  void prepareEditObjects(const glitch_view *parentView);
  void prepareFont(void);
  void saveProperties(const QMap<QString, QVariant> &p,
		      const QSqlDatabase &db,
		      QString &error);

 protected slots:
  virtual void slotCopy(void);
  virtual void slotHideOrShowOccupied(void);
  virtual void slotSelectColor(void);
  void slotSelectBorderColor(void);
  void slotSelectFont(void);
  void slotSetPortColors(void);
  void slotSetStyleSheet(void);

 signals:
  void changed(void);
  void copy(void);
  void deletedViaContextMenu(void);
  void dockPropertyEditor(QWidget *widget);
  void saveSignal(void);
  void simulateDeleteSignal(void);
};

inline uint qHash(const glitch_object::DefaultMenuActions &key, uint seed)
{
  return ::qHash(static_cast<uint> (key), seed);
}

inline uint qHash(const glitch_object::Properties &key, uint seed)
{
  return ::qHash(static_cast<uint> (key), seed);
}

#endif
