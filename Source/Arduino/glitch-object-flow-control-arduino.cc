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

#include "glitch-object-edit-window.h"
#include "glitch-object-flow-control-arduino.h"
#include "glitch-object-view.h"
#include "glitch-undo-command.h"

glitch_object_flow_control_arduino::glitch_object_flow_control_arduino
(QWidget *parent):glitch_object_flow_control_arduino(1, parent)
{
}

glitch_object_flow_control_arduino::glitch_object_flow_control_arduino
(const QString &flowControlType, QWidget *parent):
  glitch_object_flow_control_arduino(1, parent)
{
  setFlowControlType(flowControlType);
}

glitch_object_flow_control_arduino::glitch_object_flow_control_arduino
(const quint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_flowControlType = FlowControlTypes::BREAK;
  m_editView = new glitch_object_view
    (glitch_common::ProjectTypes::ArduinoProject,
     m_id,
     new QUndoStack(this), // New redo/undo stack.
     this);
  m_editWindow = new glitch_object_edit_window(parent);
  m_editWindow->setCentralWidget(m_editView);
  m_editWindow->setEditView(m_editView);
  m_editWindow->setUndoStack(m_editView->undoStack());
  m_editWindow->setWindowIcon(QIcon(":Logo/glitch-logo.png"));
  m_editWindow->setWindowTitle(tr("Glitch: flow control"));
  m_editWindow->resize(600, 600);
  m_type = "arduino-flow-control";
  m_ui.setupUi(this);
  connect(m_ui.condition,
	  &QLineEdit::returnPressed,
	  this,
	  &glitch_object_flow_control_arduino::slotConditionChanged);
  connect(m_ui.flow_control_type,
	  QOverload<int>::of(&QComboBox::currentIndexChanged),
	  this,
	  &glitch_object_flow_control_arduino::slotFlowControlTypeChanged);
  prepareContextMenu();
  setName(m_type);
}

glitch_object_flow_control_arduino::~glitch_object_flow_control_arduino()
{
}

QString glitch_object_flow_control_arduino::code(void) const
{
  return "";
}

QString glitch_object_flow_control_arduino::flowControlType(void) const
{
  return m_ui.flow_control_type->currentText();
}

bool glitch_object_flow_control_arduino::hasInput(void) const
{
  return m_ui.flow_control_type->currentText() == "return";
}

bool glitch_object_flow_control_arduino::hasOutput(void) const
{
  return false;
}

bool glitch_object_flow_control_arduino::isFullyWired(void) const
{
  return true;
}

bool glitch_object_flow_control_arduino::shouldPrint(void) const
{
  return true;
}

glitch_object_flow_control_arduino *glitch_object_flow_control_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_flow_control_arduino(parent);

  clone->m_flowControlType = m_flowControlType;
  clone->m_properties = m_properties;
  clone->m_ui.condition->setText(m_ui.condition->text().trimmed());
  clone->m_ui.condition->selectAll();
  clone->m_ui.flow_control_type->setCurrentIndex
    (m_ui.flow_control_type->currentIndex());
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_flow_control_arduino *glitch_object_flow_control_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_flow_control_arduino
    (values.value("myoid").toULongLong(), parent);

  object->setProperties(values.value("properties").toString().split('&'));
  object->setStyleSheet(values.value("stylesheet").toString());
  object->m_ui.condition->blockSignals(true);
  object->m_ui.condition->setText
    (object->m_properties.value(Properties::CONDITION).toString().trimmed());
  object->m_ui.condition->selectAll();
  object->m_ui.condition->blockSignals(false);
  return object;
}

void glitch_object_flow_control_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_flow_control_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["condition"] = m_ui.condition->text().trimmed();
  properties["flow_control_type"] = m_ui.flow_control_type->currentText();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_flow_control_arduino::setFlowControlType
(const QString &flowControlType)
{
  auto f(flowControlType.toLower().trimmed());

  if(f == "break")
    m_flowControlType = FlowControlTypes::BREAK;
  else if(f == "case")
    m_flowControlType = FlowControlTypes::CASE;
  else if(f == "continue")
    m_flowControlType = FlowControlTypes::CONTINUE;
  else if(f == "do while")
    m_flowControlType = FlowControlTypes::DO_WHILE;
  else if(f == "else if")
    m_flowControlType = FlowControlTypes::ELSE_IF;
  else if(f == "for")
    m_flowControlType = FlowControlTypes::FOR;
  else if(f == "goto")
    m_flowControlType = FlowControlTypes::GOTO;
  else if(f == "if")
    m_flowControlType = FlowControlTypes::IF;
  else if(f == "label")
    m_flowControlType = FlowControlTypes::LABEL;
  else if(f == "return")
    m_flowControlType = FlowControlTypes::RETURN;
  else if(f == "switch")
    m_flowControlType = FlowControlTypes::SWITCH;
  else if(f == "while")
    m_flowControlType = FlowControlTypes::WHILE;
  else
    m_flowControlType = FlowControlTypes::BREAK;

  m_ui.flow_control_type->blockSignals(true);
  m_ui.flow_control_type->setCurrentIndex(m_ui.flow_control_type->findText(f));

  if(m_ui.flow_control_type->currentIndex() < 0)
    m_ui.flow_control_type->setCurrentIndex(0);

  m_ui.flow_control_type->blockSignals(false);
}

void glitch_object_flow_control_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("condition = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::CONDITION] = string.trimmed();
	}
      else if(string.simplified().startsWith("flow_control_type = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::FLOW_CONTROL_TYPE] = string.trimmed();
	}
    }

  setFlowControlType
    (m_properties.value(glitch_object::FLOW_CONTROL_TYPE).toString());
}

void glitch_object_flow_control_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::CONDITION:
      {
	m_ui.condition->setText(value.toString().trimmed());
	m_ui.condition->selectAll();
	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_object_flow_control_arduino::slotConditionChanged(void)
{
  m_ui.condition->setText(m_ui.condition->text().trimmed());
  m_ui.condition->selectAll();

  if(!m_undoStack)
    return;

  auto property = glitch_object::Properties::CONDITION;

  if(m_properties.value(property).toString() == m_ui.condition->text())
    return;

  auto undoCommand = new glitch_undo_command
    (m_ui.condition->text(),
     m_properties.value(property),
     glitch_undo_command::PROPERTY_CHANGED,
     property,
     this);

  m_properties[property] = m_ui.condition->text();
  undoCommand->setText(tr("flow control condition changed"));
  m_undoStack->push(undoCommand);
  emit changed();
}

void glitch_object_flow_control_arduino::slotFlowControlTypeChanged(void)
{
  if(!m_undoStack)
    return;

  auto undoCommand = new glitch_undo_command
    (m_properties.value(Properties::FLOW_CONTROL_TYPE).toString(),
     glitch_undo_command::FLOW_CONTROL_TYPE_CHANGED,
     this);

  m_properties[Properties::FLOW_CONTROL_TYPE] =
    m_ui.flow_control_type->currentText();
  undoCommand->setText(tr("flow control type changed"));
  m_undoStack->push(undoCommand);
  emit changed();
}
