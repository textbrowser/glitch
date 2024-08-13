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

#ifndef _glitch_object_flow_control_arduino_h_
#define _glitch_object_flow_control_arduino_h_

#include "glitch-object.h"
#include "ui_glitch-object-flow-control-arduino.h"

class glitch_object_flow_control_arduino: public glitch_object
{
  Q_OBJECT

 public:
  enum class FlowControlTypes
  {
    BREAK = 0,
    CASE,
    CONTINUE,
    DEFAULT,
    DO_WHILE,
    ELSE,
    ELSE_IF,
    FOR,
    GOTO,
    IF,
    LABEL,
    RETURN,
    SWITCH,
    WHILE
  };

  glitch_object_flow_control_arduino
    (const QString &flowControlType, QWidget *parent);
  ~glitch_object_flow_control_arduino();

  QSize sizeHint(void) const
  {
    QFontMetrics fontMetrics(font());
    auto const w1 = 5 * qCeil
      (m_ui.condition->isVisible() ? (fontMetrics.
				      boundingRect(m_ui.condition->text().
						   trimmed()).width() / 5.0) :
       0.0) +
      (m_ui.condition->isVisible() ?
       5 * qCeil(m_ui.condition->sizeHint().width() / 5.0) : 0);
    auto const w2 = 5 * qCeil
      (fontMetrics.boundingRect(m_ui.flow_control_type->currentText()).width() /
       5.0);

    return QSize(s_widthTweak + w1 + w2, qMax(50, fontMetrics.height()));
  }

  QString code(void) const;
  QString flowControlType(void) const;
  bool hasInput(void) const;
  bool hasOutput(void) const;
  bool hasView(void) const;
  bool isFullyWired(void) const;
  bool shouldPrint(void) const;
  static glitch_object_flow_control_arduino *createFromValues
    (const QMap<QString, QVariant> &values, QString &error, QWidget *parent);
  glitch_object_flow_control_arduino *clone(QWidget *parent) const;
  void addActions(QMenu &menu);
  void hideOrShowOccupied(void);
  void save(const QSqlDatabase &db, QString &error);
  void setFlowControlType(const QString &flowControlType);
  void setProperty(const Properties property, const QVariant &value);
  void showEditWindow(void);

 private:
  glitch_object_flow_control_arduino(QWidget *parent);
  glitch_object_flow_control_arduino(const qint64 id, QWidget *parent);
  FlowControlTypes m_flowControlType;
  Ui_glitch_object_flow_control_arduino m_ui;
  bool editable(void) const;
  static QString simplified(const QString &text);
  void prepareEditWindowHeader(void);
  void setProperties(const QStringList &list);

 private slots:
  void slotConditionChanged(void);
  void slotEdit(void);
  void slotFlowControlTypeChanged(void);
  void slotHideOrShowOccupied(void);
};

#endif
