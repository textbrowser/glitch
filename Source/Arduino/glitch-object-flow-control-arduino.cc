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

#include <QTextStream>

#include "glitch-object-edit-window.h"
#include "glitch-object-flow-control-arduino.h"
#include "glitch-object-view.h"
#include "glitch-undo-command.h"
#include "glitch-view.h"

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
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_editView = new glitch_object_view
    (glitch_common::ProjectTypes::ArduinoProject,
     m_id,
     new QUndoStack(this), // New redo/undo stack.
     this);
  m_editWindow = new glitch_object_edit_window(parent);
  m_editWindow->prepareToolBar(m_editView->alignmentActions());
  m_editWindow->setCentralWidget(m_editView);
  m_editWindow->setEditView(m_editView);
  m_editWindow->setUndoStack(m_editView->undoStack());
  m_editWindow->setWindowIcon(QIcon(":Logo/glitch-logo.png"));
  m_editWindow->setWindowTitle(tr("Glitch: flow control"));
  m_editWindow->resize(600, 600);
  m_flowControlType = FlowControlTypes::BREAK;
  m_type = "arduino-flow-control";
  m_ui.setupUi(this);
  m_ui.occupied->setVisible(false);
  connect(m_editView,
	  &glitch_object_view::changed,
	  this,
	  &glitch_object_flow_control_arduino::changed);
  connect(m_editView->undoStack(),
	  &QUndoStack::indexChanged,
	  this,
	  &glitch_object_flow_control_arduino::slotHideOrShowOccupied);
  connect(m_ui.condition,
	  &QLineEdit::returnPressed,
	  this,
	  &glitch_object_flow_control_arduino::slotConditionChanged);
  connect(m_ui.flow_control_type,
	  QOverload<int>::of(&QComboBox::currentIndexChanged),
	  this,
	  &glitch_object_flow_control_arduino::slotFlowControlTypeChanged);
  prepareContextMenu();
  prepareEditSignals(findNearestGlitchView(parent));
  setName(m_type);
  QTimer::singleShot(1500, this, SLOT(slotUndoStackCreated(void)));
}

glitch_object_flow_control_arduino::~glitch_object_flow_control_arduino()
{
  disconnect(m_editView->undoStack(),
	     &QUndoStack::indexChanged,
	     this,
	     &glitch_object_flow_control_arduino::slotHideOrShowOccupied);
}

QString glitch_object_flow_control_arduino::code(void) const
{
  if(m_ui.flow_control_type->currentText() == "break")
    return "break;";
  else if(m_ui.flow_control_type->currentText() == "continue")
    return "continue;";
  else if(m_ui.flow_control_type->currentText() == "goto")
    return QString("goto %1;").arg(m_ui.condition->text().trimmed());
  else if(m_ui.flow_control_type->currentText() == "label")
    return QString("%1:").arg(m_ui.condition->text().trimmed());
  else if(m_ui.flow_control_type->currentText() == "return")
    return QString("return (%1);").arg(inputs().value(0));

  QString code("");
  QTextStream stream(&code);
  auto widgets(m_editView->scene()->orderedObjects());

  if(m_ui.flow_control_type->currentText() == "case")
    stream << "case "
	   << m_ui.condition->text().trimmed()
	   << ":";
  else if(m_ui.flow_control_type->currentText() == "do while")
    stream << "do";
  else if(m_ui.flow_control_type->currentText() == "else")
    stream << "else";
  else if(m_ui.flow_control_type->currentText() == "else if")
    stream << "else if("
	   << m_ui.condition->text().trimmed()
	   << ")";
  else if(m_ui.flow_control_type->currentText() == "for")
    stream << "for("
	   << m_ui.condition->text().trimmed()
	   << ")";
  else if(m_ui.flow_control_type->currentText() == "if")
    stream << "if("
	   << m_ui.condition->text().trimmed()
	   << ")";
  else if(m_ui.flow_control_type->currentText() == "switch")
    stream << "switch("
	   << m_ui.condition->text().trimmed()
	   << ")";
  else if(m_ui.flow_control_type->currentText() == "while")
    stream << "while("
	   << m_ui.condition->text().trimmed()
	   << ")";

  stream << Qt::endl
	 << "\t"
	 << "{"
	 << Qt::endl;

  for(auto w : widgets)
    {
      if(!w || !w->shouldPrint())
	continue;

      auto code(w->code());

      if(!code.trimmed().isEmpty())
	stream << "\t"
	       << "\t"
	       << code
	       << Qt::endl;
    }

  stream << "\t"
	 << "}";

  if(m_ui.flow_control_type->currentText() == "do while")
    stream << " while ("
	   << m_ui.condition->text().trimmed()
	   << ");";

  return code;
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

bool glitch_object_flow_control_arduino::hasView(void) const
{
  return true;
}

bool glitch_object_flow_control_arduino::isFullyWired(void) const
{
  if(m_ui.flow_control_type->currentText() == "return")
    return inputs().size() >= 1;
  else
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

  foreach(auto object, m_editView->scene()->objects())
    clone->m_copiedChildren << object->clone(nullptr);

  return clone;
}

glitch_object_flow_control_arduino *glitch_object_flow_control_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_flow_control_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties
    (values.value("properties").toString().split(s_splitRegularExpression));
  object->setStyleSheet(values.value("stylesheet").toString());
  object->m_ui.condition->setText
    (object->m_properties.value(Properties::CONDITION).toString().trimmed());
  object->m_ui.condition->selectAll();
  object->prepareEditWindowHeader();
  return object;
}

void glitch_object_flow_control_arduino::addActions(QMenu &menu)
{
  if(!m_actions.contains(DefaultMenuActions::EDIT))
    {
      auto action = new QAction(tr("&Edit..."), this);

      action->setIcon(QIcon::fromTheme("document-edit"));
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object_flow_control_arduino::slotEdit);
      m_actions[DefaultMenuActions::EDIT] = action;
      menu.addAction(action);
    }
  else
    menu.addAction(m_actions.value(DefaultMenuActions::EDIT));

  addDefaultActions(menu);
}

void glitch_object_flow_control_arduino::hideOrShowOccupied(void)
{
  auto scene = editScene();

  if(!scene)
    return;

  m_ui.occupied->setVisible(!scene->objects().isEmpty());
}

void glitch_object_flow_control_arduino::mouseDoubleClickEvent
(QMouseEvent *event)
{
  slotEdit();
  glitch_object::mouseDoubleClickEvent(event);
}

void glitch_object_flow_control_arduino::prepareEditWindowHeader(void)
{
  m_editWindow->prepareHeader
    (QString("%1 (%2)").
     arg(m_ui.flow_control_type->currentText()).
     arg(m_ui.condition->text()));
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

  if(error.isEmpty() && m_editView)
    m_editView->save(db, error);
}

void glitch_object_flow_control_arduino::setFlowControlType
(const QString &flowControlType)
{
  auto f(flowControlType.
	 mid(flowControlType.lastIndexOf('-') + 1).toLower().trimmed());
  int minimumWidth = 500;

  if(f == "break")
    {
      m_flowControlType = FlowControlTypes::BREAK;
      m_ui.condition->setVisible(false);
      minimumWidth = 0;
    }
  else if(f == "case")
    {
      m_flowControlType = FlowControlTypes::CASE;
      m_ui.condition->setVisible(true);
    }
  else if(f == "continue")
    {
      m_flowControlType = FlowControlTypes::CONTINUE;
      m_ui.condition->setVisible(false);
      minimumWidth = 0;
    }
  else if(f == "do while")
    {
      m_flowControlType = FlowControlTypes::DO_WHILE;
      m_ui.condition->setVisible(true);
    }
  else if(f == "else")
    {
      m_flowControlType = FlowControlTypes::ELSE;
      m_ui.condition->setVisible(false);
      minimumWidth = 0;
    }
  else if(f == "else if")
    {
      m_flowControlType = FlowControlTypes::ELSE_IF;
      m_ui.condition->setVisible(true);
    }
  else if(f == "for")
    {
      m_flowControlType = FlowControlTypes::FOR;
      m_ui.condition->setVisible(true);
    }
  else if(f == "goto")
    {
      m_flowControlType = FlowControlTypes::GOTO;
      m_ui.condition->setVisible(true);
    }
  else if(f == "if")
    {
      m_flowControlType = FlowControlTypes::IF;
      m_ui.condition->setVisible(true);
    }
  else if(f == "label")
    {
      m_flowControlType = FlowControlTypes::LABEL;
      m_ui.condition->setVisible(true);
    }
  else if(f == "return")
    {
      m_flowControlType = FlowControlTypes::RETURN;
      m_ui.condition->setVisible(false);
      minimumWidth = 0;
    }
  else if(f == "switch")
    {
      m_flowControlType = FlowControlTypes::SWITCH;
      m_ui.condition->setVisible(true);
    }
  else if(f == "while")
    {
      m_flowControlType = FlowControlTypes::WHILE;
      m_ui.condition->setVisible(true);
    }
  else
    {
      m_flowControlType = FlowControlTypes::BREAK;
      m_ui.condition->setVisible(false);
      minimumWidth = 0;
    }

  m_ui.flow_control_type->blockSignals(true);
  m_ui.flow_control_type->setCurrentIndex(m_ui.flow_control_type->findText(f));

  if(m_ui.flow_control_type->currentIndex() < 0)
    m_ui.flow_control_type->setCurrentIndex(0);

  m_ui.flow_control_type->blockSignals(false);
  m_editWindow->prepareHeader
    (QString("%1 (%2)").
     arg(m_ui.flow_control_type->currentText()).
     arg(m_ui.condition->text()));
  resize(qMax(minimumWidth, sizeHint().width()), height());
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
	  string = string.mid(string.indexOf('"') + 1);
	  string = string.mid(0, string.lastIndexOf('"'));
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
	prepareEditWindowHeader();
	break;
      }
    case Properties::FLOW_CONTROL_TYPE:
      {
	setFlowControlType(value.toString().trimmed());
	prepareEditWindowHeader();
	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_object_flow_control_arduino::setUndoStack(QUndoStack *undoStack)
{
  m_undoStack = undoStack;
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

void glitch_object_flow_control_arduino::slotEdit(void)
{
  m_editWindow->showNormal();
  m_editWindow->raise();
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

void glitch_object_flow_control_arduino::slotHideOrShowOccupied(void)
{
  hideOrShowOccupied();
}
