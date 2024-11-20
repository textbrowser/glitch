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

#include <QTextStream>

#include "glitch-object-edit-window.h"
#include "glitch-object-flow-control-arduino.h"
#include "glitch-object-view.h"
#include "glitch-scroll-filter.h"
#include "glitch-ui.h"
#include "glitch-undo-command.h"
#include "glitch-variety.h"
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
  m_properties[Properties::FLOW_CONTROL_TYPE] =
    m_ui.flow_control_type->currentText();
  resize(sizeHint());
}

glitch_object_flow_control_arduino::glitch_object_flow_control_arduino
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_editView = new glitch_object_view
    (glitch_common::ProjectTypes::ArduinoProject,
     m_id,
     m_undoStack,
     this);
  m_editView->setVisible(false);
  m_flowControlType = FlowControlTypes::BREAK;
  m_type = "arduino-flow-control";
  m_ui.setupUi(this);
  m_ui.flow_control_type->installEventFilter(new glitch_scroll_filter(this));
  connect(m_editView,
	  &glitch_object_view::changed,
	  this,
	  &glitch_object_flow_control_arduino::changed);
  connect(m_ui.condition,
	  &QLineEdit::editingFinished,
	  this,
	  &glitch_object_flow_control_arduino::slotConditionChanged);
  connect(m_ui.flow_control_type,
	  SIGNAL(currentIndexChanged(int)),
	  this,
	  SLOT(slotFlowControlTypeChanged(void)));
  prepareContextMenu();
  setName(m_ui.flow_control_type->currentText());
}

glitch_object_flow_control_arduino::~glitch_object_flow_control_arduino()
{
  if(m_undoStack)
    disconnect(m_undoStack, nullptr, this, nullptr);
}

QString glitch_object_flow_control_arduino::code(void) const
{
  if(!property(Properties::GENERATE_SOURCE).toBool())
    return "";

  if(m_ui.flow_control_type->currentText() == "break")
    return "break;";
  else if(m_ui.flow_control_type->currentText() == "continue")
    return "continue;";
  else if(m_ui.flow_control_type->currentText() == "goto")
    return QString("goto %1;").arg(m_ui.condition->text().trimmed());
  else if(m_ui.flow_control_type->currentText() == "label")
    return QString("%1:").arg(m_ui.condition->text().trimmed());
  else if(m_ui.flow_control_type->currentText() == "return")
    {
      if(inputs().isEmpty())
	return "return;";
      else
	return QString("return(%1);").arg(inputs().value(0));
    }

  QString code("");
  QTextStream stream(&code);

  if(m_ui.flow_control_type->currentText() == "case")
    stream << "case "
	   << m_ui.condition->text().trimmed()
	   << ":";
  else if(m_ui.flow_control_type->currentText() == "default")
    stream << "default:";
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

  QObject *w = nullptr;
  auto level = 0;

  if(scene())
    w = scene()->parent();

  do
    {
      if(qobject_cast<glitch_object_view *> (w))
	level += 1;

      if(w)
	w = w->parent();
    }
  while(w);

  level = qMax(1, level);
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
  stream << endl
	 << QString(level, glitch_common::s_indentationCharacter)
	 << "{"
	 << endl;
#else
  stream << Qt::endl
	 << QString(level, glitch_common::s_indentationCharacter)
	 << "{"
	 << Qt::endl;
#endif

  if(m_editView->scene())
    {
      foreach(auto w, m_editView->scene()->orderedObjects())
	{
	  if(!w || !w->shouldPrint())
	    continue;

	  auto const code(w->code());

	  if(!code.trimmed().isEmpty())
	    stream << QString(level + 1, glitch_common::s_indentationCharacter)
		   << code
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
		   << endl;
#else
	           << Qt::endl;
#endif
	}
    }

  stream << QString(level, glitch_common::s_indentationCharacter)
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

QString glitch_object_flow_control_arduino::simplified(const QString &text)
{
  QString string("");
  auto state = 0;

  for(int i = 0; i < text.length(); i++)
    if(state == 0)
      {
	if(i > 0 && text.at(i) == ' ' && text.at(i - 1) == ' ')
	  continue;
	else if(text.at(i) == '"')
	  state = 1;

	string.append(text.at(i));
      }
    else if(state == 1)
      {
	if(text.at(i) == '"')
	  state = 0;

	string.append(text.at(i));
      }

  return string.trimmed();
}

bool glitch_object_flow_control_arduino::editable(void) const
{
  switch(m_flowControlType)
    {
    case FlowControlTypes::BREAK:
    case FlowControlTypes::CONTINUE:
    case FlowControlTypes::GOTO:
    case FlowControlTypes::LABEL:
    case FlowControlTypes::RETURN:
      {
	return false;
      }
    default:
      {
	return true;
      }
    }
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

  clone->cloneWires(m_copiedConnectionsPositions);
  clone->cloneWires(m_wires);
  clone->m_flowControlType = m_flowControlType;
  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->m_ui.condition->setText(simplified(m_ui.condition->text()));
  clone->m_ui.condition->setCursorPosition(0);
  clone->resize(size());
  clone->setCanvasSettings(m_canvasSettings);
  clone->setFlowControlType(m_ui.flow_control_type->currentText());
  clone->setStyleSheet(styleSheet());

  if(m_copiedChildren.isEmpty() && m_editView->scene())
    /*
    ** First, copy!
    */

    foreach(auto object, m_editView->scene()->objects())
      {
	if(!object)
	  continue;

	auto child = object->clone(nullptr);

	if(child)
	  {
	    child->setProperty("position", object->scenePos());
	    clone->m_copiedChildren << child;
	  }
      }
  else
    {
      /*
      ** Now, paste!
      */

      foreach(auto object, m_copiedChildren)
	{
	  if(!object)
	    continue;

	  auto child = object->clone(clone->m_editView);

	  if(child)
	    {
	      auto ok = true;

	      glitch_ui::s_copiedObjectsSet << child;
	      child->compressWidget
		(child->property(Properties::COMPRESSED_WIDGET).toBool());
	      child->setCanvasSettings(m_canvasSettings);
	      clone->addChild
		(object->property("position").toPoint(), child, ok);

	      if(!ok)
		{
		  glitch_ui::s_copiedObjectsSet.remove(child);
		  child->deleteLater();
		}
	    }
	}

      clone->hideOrShowOccupied();
    }

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
    (splitPropertiesRegularExpression(values.value("properties")));
  object->setStyleSheet(values.value("stylesheet").toString());
  object->m_ui.condition->setText
    (simplified(object->m_properties.value(Properties::CONDITION).toString()));
  object->m_ui.condition->setCursorPosition(0);
  object->prepareEditWindowHeader();
  glitch_variety::highlight(object->m_ui.condition);
  return object;
}

void glitch_object_flow_control_arduino::addActions(QMenu &menu)
{
  if(!m_actions.contains(DefaultMenuActions::EDIT))
    {
      auto action = new QAction(tr("&Edit..."), this);

      action->setEnabled(editable());
      action->setIcon(QIcon(":/document-edit.png"));
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object_flow_control_arduino::slotEdit);
      m_actions[DefaultMenuActions::EDIT] = action;
      menu.addAction(action);
    }
  else if(m_actions.value(DefaultMenuActions::EDIT, nullptr))
    menu.addAction(m_actions.value(DefaultMenuActions::EDIT));

  addDefaultActions(menu);
}

void glitch_object_flow_control_arduino::hideOrShowOccupied(void)
{
  glitch_object::hideOrShowOccupied();

  auto scene = editScene();

  if(!scene)
    return;

  m_occupied = !scene->objects().isEmpty();

  auto font(m_properties.value(Properties::FONT).value<QFont> ());

  font.setBold(true);
  font.setUnderline(m_occupied);
  m_ui.flow_control_type->setFont(font);
}

void glitch_object_flow_control_arduino::prepareEditWindowHeader(void)
{
  if(m_editWindow)
    m_editWindow->prepareHeader
      (QString("%1(%2)").
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

  if(error.isEmpty() && (m_flowControlType == FlowControlTypes::CASE ||
			 m_flowControlType == FlowControlTypes::DEFAULT ||
			 m_flowControlType == FlowControlTypes::DO_WHILE ||
			 m_flowControlType == FlowControlTypes::ELSE ||
			 m_flowControlType == FlowControlTypes::ELSE_IF ||
			 m_flowControlType == FlowControlTypes::FOR ||
			 m_flowControlType == FlowControlTypes::IF ||
			 m_flowControlType == FlowControlTypes::SWITCH ||
			 m_flowControlType == FlowControlTypes::WHILE))
    m_editView->save(db, error);
}

void glitch_object_flow_control_arduino::setFlowControlType
(const QString &flowControlType)
{
  auto const f(flowControlType.mid(flowControlType.lastIndexOf('-') + 1).
	       toLower().trimmed());
  auto enabled = true;

  if(f == "break")
    {
      enabled = false;
      m_editWindow ? m_editWindow->setVisible(false) : (void) 0;
      m_flowControlType = FlowControlTypes::BREAK;
      m_ui.condition->setVisible(false);
    }
  else if(f == "case")
    {
      m_flowControlType = FlowControlTypes::CASE;
      m_ui.condition->setVisible(true);
    }
  else if(f == "continue")
    {
      enabled = false;
      m_editWindow ? m_editWindow->setVisible(false) : (void) 0;
      m_flowControlType = FlowControlTypes::CONTINUE;
      m_ui.condition->setVisible(false);
    }
  else if(f == "default")
    {
      m_flowControlType = FlowControlTypes::DEFAULT;
      m_ui.condition->setVisible(false);
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
      enabled = false;
      m_editWindow ? m_editWindow->setVisible(false) : (void) 0;
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
      enabled = false;
      m_editWindow ? m_editWindow->setVisible(false) : (void) 0;
      m_flowControlType = FlowControlTypes::LABEL;
      m_ui.condition->setVisible(true);
    }
  else if(f == "return")
    {
      enabled = false;
      m_editWindow ? m_editWindow->setVisible(false) : (void) 0;
      m_flowControlType = FlowControlTypes::RETURN;
      m_ui.condition->setVisible(false);
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
      enabled = false;
      m_editWindow ? m_editWindow->setVisible(false) : (void) 0;
      m_flowControlType = FlowControlTypes::BREAK;
      m_ui.condition->setVisible(false);
    }

  if(m_actions.value(DefaultMenuActions::EDIT, nullptr))
    m_actions.value(DefaultMenuActions::EDIT)->setEnabled(enabled);

  m_ui.flow_control_type->blockSignals(true);
  m_ui.flow_control_type->setCurrentIndex(m_ui.flow_control_type->findText(f));

  if(m_ui.flow_control_type->currentIndex() < 0)
    m_ui.flow_control_type->setCurrentIndex(0);

  m_ui.flow_control_type->blockSignals(false);
  prepareEditWindowHeader();
  setName(m_ui.flow_control_type->currentText());
}

void glitch_object_flow_control_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::CONDITION] = "";
  m_properties[Properties::FLOW_CONTROL_TYPE] = "break";

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
    (m_properties.value(Properties::FLOW_CONTROL_TYPE).toString());
}

void glitch_object_flow_control_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::CONDITION:
      {
	m_ui.condition->setText(simplified(value.toString()));
	m_ui.condition->setCursorPosition(0);
	glitch_variety::highlight(m_ui.condition);
	prepareEditWindowHeader();
	break;
      }
    case Properties::FLOW_CONTROL_TYPE:
      {
	setFlowControlType(value.toString().trimmed());
	prepareEditWindowHeader();
	resize(sizeHint());
	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_object_flow_control_arduino::showEditWindow(const bool signal)
{
  if(!m_editWindow)
    {
      m_editView->setVisible(true);
      m_editWindow = new glitch_object_edit_window
	(glitch_common::ProjectTypes::ArduinoProject, this, m_parent);
      m_editWindow->setCentralWidget(m_editView);
      m_editWindow->setEditView(m_editView);
      m_editWindow->setUndoStack(m_undoStack);
      m_editWindow->setWindowTitle(tr("Glitch: flow control"));
      prepareEditObjects(findNearestGlitchView(m_parent));
    }

  glitch_object::showEditWindow(signal);
}

void glitch_object_flow_control_arduino::slotConditionChanged(void)
{
  m_ui.condition->setText(simplified(m_ui.condition->text()));
  m_ui.condition->setCursorPosition(0);
  glitch_variety::highlight(m_ui.condition);

  auto const property = glitch_object::Properties::CONDITION;

  if(m_properties.value(property).toString() == m_ui.condition->text())
    return;

  if(m_undoStack)
    {
      auto undoCommand = new glitch_undo_command
	(m_ui.condition->text(),
	 m_properties.value(property),
	 glitch_undo_command::Types::PROPERTY_CHANGED,
	 property,
	 this);

      m_properties[property] = m_ui.condition->text();
      undoCommand->setText
	(tr("flow control condition changed (%1, %2)").
	 arg(scenePos().x()).arg(scenePos().y()));
      m_undoStack->push(undoCommand);
    }
  else
    m_properties[property] = m_ui.condition->text();

  m_ui.condition->selectAll();
  emit changed();
}

void glitch_object_flow_control_arduino::slotEdit(void)
{
  switch(m_flowControlType)
    {
    case FlowControlTypes::BREAK:
    case FlowControlTypes::CONTINUE:
    case FlowControlTypes::GOTO:
    case FlowControlTypes::LABEL:
    case FlowControlTypes::RETURN:
      {
	return;
      }
    default:
      {
	break;
      }
    }

  showEditWindow();

  if(m_editWindow)
    m_editWindow->setToolBarVisible // Recorded in the window's state.
      (m_properties.value(Properties::TOOL_BAR_VISIBLE).toBool());
}

void glitch_object_flow_control_arduino::slotFlowControlTypeChanged(void)
{
  if(m_undoStack)
    {
      auto undoCommand = new glitch_undo_command
	(m_properties.value(Properties::FLOW_CONTROL_TYPE).toString(),
	 glitch_undo_command::Types::FLOW_CONTROL_TYPE_CHANGED,
	 this);

      m_properties[Properties::FLOW_CONTROL_TYPE] =
	m_ui.flow_control_type->currentText();
      undoCommand->setText
	(tr("flow control type changed (%1, %2)").
	 arg(scenePos().x()).arg(scenePos().y()));
      m_undoStack->push(undoCommand);
    }
  else
    m_properties[Properties::FLOW_CONTROL_TYPE] =
      m_ui.flow_control_type->currentText();

  emit changed();
}

void glitch_object_flow_control_arduino::slotHideOrShowOccupied(void)
{
  hideOrShowOccupied();
}
